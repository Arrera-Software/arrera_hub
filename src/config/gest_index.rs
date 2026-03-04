use std::fs;
use std::path::PathBuf;
use crate::config::dect_os;
use dirs;
use crate::config::user_conf::{add_conf, read_conf};
use chrono::Local;
use serde::Deserialize;

#[derive(Deserialize, Debug)]
struct Item {
    name: String,
    url: String,
    img: String,
}

#[derive(Deserialize, Debug)]
struct Root {
    application: Vec<Item>,
    assistants: Vec<Item>,
}
#[derive(Deserialize, Debug)]
pub struct Depot {
    name: String,
    version: String,
    download_linux: String,
    download_windows: String,
    download_macos: String,
}


pub fn get_path_index() -> Result<PathBuf, Box<dyn std::error::Error>> {

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

    Ok(dir)
}
pub fn save_index(content: &str) -> Result<(), Box<dyn std::error::Error>> {


    let dir = get_path_index()?;

    if let Some(parent) = dir.parent() {
        fs::create_dir_all(parent)?;
    }

    let maintenant = Local::now();
    let date_string = maintenant.format("%d/%m/%Y").to_string();

    add_conf("load_index",&date_string)?;

    fs::write(dir, content)?;

    Ok(())
}

pub fn get_img_application(cathegorie : &str, nom : &str) -> Result<Vec<String>, Box<dyn std::error::Error>>{
    if cathegorie != "application" && cathegorie != "assistant" {
        return Err("Catégorie invalide".into());
    }

    let data = fs::read_to_string(get_path_index()?)?;
    let parsed: Root = serde_json::from_str(&data)?;

    // On cherche l'item
    let app = if cathegorie == "application" {
        parsed.application.iter().find(|a| a.name == nom)
    } else if cathegorie == "assistant" {
        parsed.assistants.iter().find(|a| a.name == nom)
    }else{
        return Err("Catégorie invalide".into());
    };

    // On gère l'erreur si non trouvé et on CLONE la valeur
    let item = app.ok_or("Application ou assistant non trouvé")?;

    // 2. On clone l'image pour qu'elle devienne une String indépendante
    Ok(vec![item.img.clone()])
}

pub async fn load_json_application(cathegorie : &str, nom : &str)-> Result<Depot, Box<dyn std::error::Error>>
{
    if cathegorie != "application" && cathegorie != "assistant" {
        return Err("Catégorie invalide".into());
    }

    let data = fs::read_to_string(get_path_index()?)?;
    let parsed: Root = serde_json::from_str(&data)?;

    let app: &Item = if cathegorie == "application" {
        parsed.application
            .iter()
            .find(|a| a.name == nom)
            .ok_or("Application non trouvée")? // Retourne l'erreur si absent
    } else if cathegorie == "assistant" {
        parsed.assistants
            .iter()
            .find(|a| a.name == nom)
            .ok_or("Assistant non trouvé")?
    } else {
        return Err("Catégorie invalide".into());
    };

    let response = reqwest::get(&app.url).await?;

    let depot: Depot = response.json().await?;

    Ok(depot)
}

pub fn check_date() -> bool{
    match read_conf("load_index"){
        Some(v) => return v == Local::now().format("%d/%m/%Y").to_string(),
        None => return false
    }
}