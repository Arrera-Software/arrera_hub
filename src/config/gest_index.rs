use std::fs;
use crate::config::dect_os;
use dirs;
use crate::config::user_conf::{add_conf, read_conf};
use chrono::Local;

pub fn save_index(content: &str) -> Result<(), Box<dyn std::error::Error>> {
    let os: i32 = dect_os();

    let mut dir = dirs::home_dir().ok_or("Impossible de trouver le dossier personnel")?;

    match os {
        1 => { // Windows
            dir.push("AppData");
            dir.push("Roaming");
            dir.push("arrera-hub");
        },
        2 | 3 => { // Linux & Mac
            dir.push(".config");
            dir.push("arrera-hub");
        },
        _ => return Err("OS non supporté".into()),
    }

    dir.push("index.json");

    if let Some(parent) = dir.parent() {
        fs::create_dir_all(parent)?;
    }

    let maintenant = Local::now();
    let date_string = maintenant.format("%d/%m/%Y").to_string();

    add_conf("load_index",&date_string)?;

    fs::write(dir, content)?;

    Ok(())
}

// ToDo : Fonction pour les valeur qui son dans le JSON de apps

pub fn check_date() -> bool{
    match read_conf("load_index"){
        Some(v) => return v == Local::now().format("%d/%m/%Y").to_string(),
        None => return false
    }
}