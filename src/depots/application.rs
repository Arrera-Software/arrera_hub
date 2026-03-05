use std::env;
use std::fs::{self, File};
use std::io::{self, Write};
use crate::config::gest_index::get_link_download;
use futures_util::StreamExt;
use zip::ZipArchive;
use std::path::{Path, PathBuf};
use std::process::Command;
use crate::config::dect_os;

pub async fn install_app(cathegorie: &str, nom: &str) -> Result<(), Box<dyn std::error::Error>> {
    let zip_path = env::temp_dir().join("application.zip");
    let url = get_link_download(cathegorie, nom).await;
    let extract_to = env::temp_dir().join("application");

    let client = reqwest::Client::new();
    let response = client.get(url).send().await?;

    if !response.status().is_success() {
        return Err(format!("Erreur HTTP : {}", response.status()).into());
    }

    let total_size = response.content_length().unwrap_or(0);
    let mut downloaded: u64 = 0;
    let mut stream = response.bytes_stream();
    let mut file = File::create(&zip_path)?;

    while let Some(item) = stream.next().await {
        let chunk = item?;
        file.write_all(&chunk)?;
        downloaded += chunk.len() as u64;

        if total_size > 0 {
            let percent = (downloaded as f64 / total_size as f64) * 100.0;
            print!("\rProgression : {:.2}%", percent);
            io::stdout().flush()?;
        }
    }
    println!(); // Nouvelle ligne après la barre de progression

    // UnZip
    let file = fs::File::open(&zip_path)?;
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
            let mut outfile = fs::File::create(&outpath)?;
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

    match dect_os() {
        1 => Err("Pas implémenté pour Windows".into()),
        2 => Err("Pas implémenté pour Linux".into()),
        3 => install_dmg(target_dir.to_str().unwrap()),
        _ => Err("OS non supporté".into()),
    }
}

pub fn install_dmg(outpath: &str) -> Result<(), Box<dyn std::error::Error>> {
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
    println!("Super, le DMG est ici : {}", dmg_path_str);

    println!("Montage du disque...");
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

    println!("Volume monté sur : {}", volume_path);

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