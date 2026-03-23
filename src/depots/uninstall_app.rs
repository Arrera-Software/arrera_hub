use std::fs;
use std::path::{Path, PathBuf};
use crate::config::user_conf::{add_conf};

#[cfg(target_os = "windows")]
use {
    std::env,
    crate::depots::gest_depots::get_name_application,
};

#[cfg(target_os = "macos")]
use crate::depots::gest_depots::get_name_application;

pub async fn uninstall_app(cathegorie: &str, nom: &str) -> Result<(), Box<dyn std::error::Error>> {
    #[cfg(target_os = "windows")]
    let result = uninstall_win(cathegorie, nom).await;

    #[cfg(target_os = "linux")]
    let result = uninstall_linux(cathegorie, nom).await;

    #[cfg(target_os = "macos")]
    let result = uninstall_macos(cathegorie, nom).await;

    add_conf("general",&nom.to_lowercase(), "NONE")?;

    result
}

#[cfg(target_os = "macos")]
async fn uninstall_macos(cathegorie: &str, nom: &str) -> Result<(), Box<dyn std::error::Error>> {
    let app_name = get_name_application(cathegorie, nom).await;

    // Essayer de supprimer avec le nom exact
    let app_dir = format!("/Applications/{}.app", app_name);
    let path = Path::new(&app_dir);

    if path.exists() {
        fs::remove_dir_all(path)?;
    } else {
        // Fallback avec le nom brut
        let app_dir_fallback = format!("/Applications/{}.app", nom);
        let path_fallback = Path::new(&app_dir_fallback);
        if path_fallback.exists() {
            fs::remove_dir_all(path_fallback)?;
        }
    }
    Ok(())
}

#[cfg(target_os = "linux")]
async fn uninstall_linux(_cathegorie: &str, nom: &str) -> Result<(), Box<dyn std::error::Error>> {
    let home = dirs::home_dir().ok_or("Impossible de trouver le Home")?;
    let dossier_lanceurs = home.join(".local/share/applications");
    let nom_desktop = format!("{}.desktop", nom.to_lowercase().replace(" ", "_"));
    let chemin_desktop = dossier_lanceurs.join(&nom_desktop);

    if chemin_desktop.exists() {
        // Lire le fichier desktop pour trouver le dossier de l'application (champ Path=)
        let contenu = fs::read_to_string(&chemin_desktop)?;
        for ligne in contenu.lines() {
            if ligne.starts_with("Path=") {
                let dossier_app = ligne.replace("Path=", "");
                let path_app = Path::new(&dossier_app);
                if path_app.exists() {
                    fs::remove_dir_all(path_app)?;
                }
                break;
            }
        }
        // Supprimer le fichier .desktop
        fs::remove_file(chemin_desktop)?;
    }
    Ok(())
}

#[cfg(target_os = "windows")]
async fn uninstall_win(cathegorie: &str, nom: &str) -> Result<(), Box<dyn std::error::Error>> {
    let app_name = get_name_application(cathegorie, nom).await;

    // Suppression du raccourci
    let appdata = env::var("APPDATA")?;
    let start_menu_path = PathBuf::from(appdata)
        .join("Microsoft\\Windows\\Start Menu\\Programs");
    let shortcut_path = start_menu_path.join(format!("{}.lnk", app_name));

    if shortcut_path.exists() {
        fs::remove_file(&shortcut_path)?;
    }

    // Recherche et suppression du dossier dans ~/Applications
    let home_dir = dirs::home_dir().ok_or("Impossible de trouver le dossier utilisateur")?;
    let target_base_dir = home_dir.join("Applications");

    if target_base_dir.exists() {
        for entry in fs::read_dir(&target_base_dir)? {
            let entry = entry?;
            let path = entry.path();
            if path.is_dir() {
                let folder_name = entry.file_name().to_string_lossy().to_lowercase();
                if folder_name.contains(&nom.to_lowercase()) || (!app_name.is_empty() && folder_name.contains(&app_name.to_lowercase())) {
                    fs::remove_dir_all(&path)?;
                    break;
                }
            }
        }
    }

    Ok(())
}