use std::env;
use std::fs::{self, File};
use std::io::{self, Write};
use crate::depots::gest_depots::{get_link_download, get_name_application, get_version_application};
use futures_util::StreamExt;
use zip::ZipArchive;
use std::path::Path;


#[cfg(target_os = "linux")]
use {
    crate::config::{download_file},
    crate::depots::gest_depots::{get_img_application}
};

#[cfg(windows)]
use {
    mslnk::ShellLink,
    fs_extra::dir::{CopyOptions},
    std::path::{PathBuf},
};

#[cfg(target_os = "macos")]
use {std::process::Command,
     std::path::PathBuf
};

#[cfg(unix)]
use std::os::unix::fs::PermissionsExt;
use crate::config::user_conf::add_conf;

pub async fn install_app(cathegorie: &str, nom: &str) -> Result<(), Box<dyn std::error::Error>> {
    let zip_path = env::temp_dir().join("application.zip");
    let url = get_link_download(cathegorie, nom).await;
    let extract_to = env::temp_dir().join("application");

    let client = reqwest::Client::new();
    let response = client.get(url).send().await?;

    if !response.status().is_success() {
        return Err(format!("Erreur HTTP : {}", response.status()).into());
    }

    let mut stream = response.bytes_stream();
    let mut file = File::create(&zip_path)?;

    while let Some(item) = stream.next().await {
        let chunk = item?;
        file.write_all(&chunk)?;
    }
    let file = File::open(&zip_path)?;
    let mut archive = ZipArchive::new(file)?;

    let target_dir = Path::new(&extract_to);
    if !target_dir.exists() {
        fs::create_dir_all(target_dir)?;
    }

    for i in 0..archive.len() {
        let mut file = archive.by_index(i)?;
        let outpath = target_dir.join(file.mangled_name());

        if file.is_dir() {
            fs::create_dir_all(&outpath)?;
        } else {
            if let Some(p) = outpath.parent() {
                if !p.exists() {
                    fs::create_dir_all(p)?;
                }
            }
            let mut outfile = File::create(&outpath)?;
            io::copy(&mut file, &mut outfile)?;
        }

        #[cfg(unix)]
        {
            use std::os::unix::fs::PermissionsExt;
            if let Some(mode) = file.unix_mode() {
                fs::set_permissions(&outpath, fs::Permissions::from_mode(mode))?;
            }
        }
    }


    #[cfg(target_os = "windows")]
    let result = install_win(target_dir.to_str().unwrap(), cathegorie, nom).await;

    #[cfg(target_os = "linux")]
    let result = install_linux(target_dir.to_str().unwrap(), cathegorie, nom).await;

    #[cfg(target_os = "macos")]
    let result = install_dmg(target_dir.to_str().unwrap());

    if zip_path.exists() {
        let _ = fs::remove_file(&zip_path);
    }
    if extract_to.exists() {
        let _ = fs::remove_dir_all(&extract_to);
    }

    let version_app = get_version_application(cathegorie,nom).await;

    add_conf(&nom.to_lowercase(), &version_app)?;
    result
}
#[cfg(target_os = "macos")]
fn install_dmg(outpath: &str) -> Result<(), Box<dyn std::error::Error>> {
    let mut dmg_file_path: Option<PathBuf> = None;

    for entry in fs::read_dir(outpath)? {
        let entry = entry?;
        let path = entry.path();

        if path.extension().and_then(|ext| ext.to_str()) == Some("dmg") {
            dmg_file_path = Some(path.clone());
            break;
        } else if path.is_dir() {
            for sub_entry in fs::read_dir(&path)? {
                let sub_entry = sub_entry?;
                let sub_path = sub_entry.path();
                if sub_path.extension().and_then(|ext| ext.to_str()) == Some("dmg") {
                    dmg_file_path = Some(sub_path);
                    break;
                }
            }
        }
    }

    let dmg_path = dmg_file_path.ok_or("Aïe, aucun fichier .dmg trouvé dans l'archive.")?;
    let dmg_path_str = dmg_path.to_str().ok_or("Chemin du DMG invalide (caractères non UTF-8)")?;

    let output = Command::new("hdiutil")
        .args(["attach", dmg_path_str, "-nobrowse", "-plist"])
        .output()?;

    if !output.status.success() {
        return Err("Échec lors du montage du DMG avec hdiutil".into());
    }

    let stdout = String::from_utf8_lossy(&output.stdout);
    let volume_path = stdout
        .lines()
        .find(|line| line.contains("/Volumes/"))
        .and_then(|line| {
            let start = line.find("<string>")? + 8;
            let end = line.find("</string>")?;
            Some(line[start..end].trim().to_string())
        })
        .ok_or("Impossible de trouver le point de montage (/Volumes/...) dans le XML")?;

    let mut app_path: Option<PathBuf> = None;
    for entry in fs::read_dir(&volume_path)? {
        let entry = entry?;
        let path = entry.path();
        if path.extension().and_then(|ext| ext.to_str()) == Some("app") {
            app_path = Some(path);
            break;
        }
    }

    let app_path = app_path.ok_or("Aucune application (.app) trouvée dans le DMG monté")?;
    let app_name = app_path.file_name().unwrap().to_string_lossy();
    let dest_path = format!("/Applications/{}", app_name);
    let ditto_status = Command::new("ditto")
        .args([app_path.to_str().unwrap(), &dest_path])
        .status()?;

    if !ditto_status.success() {
        eprintln!("Avertissement: Erreur potentielle lors de la copie avec ditto. Vérifiez que vous avez les droits (sudo).");
    } else {
        let _ = Command::new("xattr")
            .args(["-cr", &dest_path])
            .status();
    }

     Command::new("hdiutil")
        .args(["detach", &volume_path, "-quiet"])
        .status()?;

    if !ditto_status.success() {
        return Err("L'installation a échoué pendant la copie.".into());
    }
    Ok(())
}
#[cfg(target_os = "linux")]
pub async fn install_linux(tager_dir: &str, cathegorie: &str, nom: &str) -> Result<(), Box<dyn std::error::Error>> {

    let mut source_opt = None;
    for entry in fs::read_dir(tager_dir)? {
        let path = entry?.path();
        if path.is_dir() {
            source_opt = Some(path);
            break; // On prend le premier dossier trouvé
        }
    }
    let source = source_opt.ok_or("Aucun dossier n'a été trouvé dans le répertoire d'extraction")?;

    let home = dirs::home_dir().ok_or("Impossible de trouver le Home")?;
    let destination_parent = home.join("Applications");

    if !destination_parent.exists() {
        fs::create_dir_all(&destination_parent)?;
    }

    let mut options = fs_extra::dir::CopyOptions::new();
    options.overwrite = true;
    options.content_only = false;
    fs_extra::dir::copy(&source, &destination_parent, &options)?;

    let dossier_app_final = destination_parent.join(source.file_name().unwrap());

    let mut executable_name_opt = None;
    let mut fallback_name_opt = None;

    for entry in fs::read_dir(&dossier_app_final)? {
        let path = entry?.path();
        if path.is_file() {
            let file_name = path.file_name().unwrap().to_string_lossy().into_owned();
            let mode = fs::metadata(&path)?.permissions().mode();
            
            // Sur Linux, 0o111 vérifie si au moins un bit "x" est présent
            if mode & 0o111 != 0 {
                executable_name_opt = Some(file_name);
                break; // On prend le premier exécutable trouvé
            }
            
            // Fallback si l'archive vient de Windows et a perdu ses droits d'exécution
            if !file_name.ends_with(".png") && !file_name.ends_with(".txt") && !file_name.ends_with(".json") && file_name != "launch.sh" {
                if file_name.to_lowercase() == nom.to_lowercase() {
                    fallback_name_opt = Some(file_name.clone());
                } else if fallback_name_opt.is_none() && !file_name.contains('.') {
                    fallback_name_opt = Some(file_name);
                }
            }
        }
    }
    
    // Si aucun exécutable n'a été trouvé avec les droits Linux, on utilise le fallback
    if executable_name_opt.is_none() {
        executable_name_opt = fallback_name_opt;
    }

    let executable_name = executable_name_opt.ok_or("Aucun exécutable trouvé dans le dossier")?;

    let chemin_icone = dossier_app_final.join("icon.png");
    let version = get_version_application(cathegorie, nom).await;
    let urls_img = get_img_application(cathegorie, nom)?;

    if !urls_img.is_empty() {
        if let Err(e) = download_file(&urls_img, &chemin_icone).await {
            eprintln!("Avertissement : Impossible de télécharger l'icône : {}", e);
        }
    }

    let chemin_exec = dossier_app_final.join(&executable_name);
    let chemin_launch_sh = dossier_app_final.join("launch.sh");

    let contenu_sh = format!(
        "#!/bin/bash\n\
        cd \"{}\"\n\
        exec ./\"{}\"\n",
        dossier_app_final.display(),
        executable_name
    );
    fs::write(&chemin_launch_sh, contenu_sh)?;

    let mut perms_sh = fs::metadata(&chemin_launch_sh)?.permissions();
    perms_sh.set_mode(0o755);
    fs::set_permissions(&chemin_launch_sh, perms_sh)?;

    // Rendre le binaire principal exécutable
    if chemin_exec.exists() {
        let mut perms_exec = fs::metadata(&chemin_exec)?.permissions();
        perms_exec.set_mode(0o755);
        fs::set_permissions(&chemin_exec, perms_exec)?;
    }

    let dossier_lanceurs = home.join(".local/share/applications");
    if !dossier_lanceurs.exists() {
        fs::create_dir_all(&dossier_lanceurs)?;
    }

    let chemin_desktop = dossier_lanceurs.join(format!("{}.desktop", nom.to_lowercase().replace(" ", "_")));

    let contenu_desktop = format!(
        "[Desktop Entry]\n\
        Type=Application\n\
        Version={version}\n\
        Name={nom_app}\n\
        Comment=Lanceur pour {nom_app}\n\
        Exec=\"{exec_path}\"\n\
        Icon={icon_path}\n\
        Terminal=false\n\
        Categories=Utility;Development;\n\
        Path={dossier_app}\n",
        version = version,
        nom_app = nom,
        exec_path = chemin_launch_sh.display(),
        icon_path = chemin_icone.display(),
        dossier_app = dossier_app_final.display()
    );
    fs::write(&chemin_desktop, contenu_desktop)?;
    Ok(())
}
#[cfg(windows)]
async fn install_win(outpath: &str, cathegorie: &str, nom: &str) -> Result<(), Box<dyn std::error::Error>>
{
    let outpath = Path::new(outpath);
    let mut source_folder_opt = None;

    // On cherche le dossier extrait
    for entry in fs::read_dir(outpath)? {
        let entry = entry?;
        let path = entry.path();
        if path.is_dir() {
            source_folder_opt = Some(path);
            break;
        }
    }

    let source_folder = source_folder_opt.ok_or("Aucun dossier trouvé dans le répertoire temporaire.")?;
    let folder_name = source_folder.file_name().unwrap().to_string_lossy().into_owned();

    // Destination : User/Applications
    let home_dir = dirs::home_dir().ok_or("Impossible de trouver le dossier utilisateur")?;
    let target_base_dir = home_dir.join("Applications");

    if !target_base_dir.exists() {
        fs::create_dir_all(&target_base_dir)?;
    }

    let mut options = CopyOptions::new();
    options.overwrite = true;

    fs_extra::dir::copy(&source_folder, &target_base_dir, &options)?;

    let final_app_dir = target_base_dir.join(&folder_name);

    // Recherche de l'exécutable pour le raccourci
    let mut exe_path: Option<PathBuf> = None;
    for entry in fs::read_dir(&final_app_dir)? {
        let entry = entry?;
        let path = entry.path();
        if path.extension().and_then(|ext| ext.to_str()) == Some("exe") {
            exe_path = Some(path);
            break;
        }
    }

    let exe_path = exe_path.ok_or("Aucun exécutable (.exe) trouvé dans le dossier installé")?;
    let exe_path_str = exe_path.to_str().ok_or("Chemin de l'exécutable invalide")?;
    
    // Création du raccourci
    println!("Création du raccourci dans le menu Démarrer...");
    let appdata = env::var("APPDATA")?;
    let start_menu_path = PathBuf::from(appdata)
        .join("Microsoft\\Windows\\Start Menu\\Programs");

    if !start_menu_path.exists() {
        fs::create_dir_all(&start_menu_path)?;
    }

    let app_name = get_name_application(cathegorie, nom).await;
    let shortcut_path = start_menu_path.join(format!("{}.lnk", app_name));

    let mut link = ShellLink::new(exe_path_str)?;
    link.set_working_dir(Some(final_app_dir.to_str().unwrap().to_string()));
    link.create_lnk(&shortcut_path)?;

    Ok(())
}