use std::fs;
use ini::Ini;
use crate::config::dect_os;
fn get_path_config () -> String
{
    let os = dect_os();

    let mut dir = match dirs::home_dir() {
        Some(path) => path,
        None => return String::new(),
    };

    if os == 1 {
        dir.push("AppData");
        dir.push("Roaming");
        dir.push("arrera-hub");
    }
    else if os == 2 || os == 3 {
        dir.push(".config");
        dir.push("arrera-hub");
    }
    else {
        return String::new();
    }
    
    if !dir.exists() {
        if let Err(e) = fs::create_dir_all(&dir) {
            eprintln!("Impossible de créer le dossier de configuration {}: {}", dir.display(), e);
            return String::new();
        }
    }else{
        println!("Fichier de confif cree");
    }

    let mut config_path = dir.clone();
    config_path.push("config.ini");
    return config_path.to_string_lossy().to_string();
}

pub fn add_conf(cles : &str, valeur : &str)-> Result<(), Box<dyn std::error::Error>>{
    if cles.is_empty() || valeur.is_empty() {
        return Err("La clé ou la valeur ne peut pas être vide".into());
    }

    let path_config_file = get_path_config();

    let mut conf = Ini::new();

    conf.with_section(Some("general"))
        .set(cles, valeur);

    conf.write_to_file(path_config_file)?;

    Ok(())
}
pub fn read_conf(cles: &str) -> Option<String> {
    let path_config_file: String = get_path_config();

    let conf = Ini::load_from_file(path_config_file).ok()?;

    let value = conf
        .section(Some("general"))
        .and_then(|section| section.get(cles))
        .map(|v| v.to_string());

    value
}