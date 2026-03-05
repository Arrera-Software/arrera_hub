use std::path::{PathBuf};

pub mod gest_index;

pub mod user_conf;

pub fn dect_os() -> i32 {
    if cfg!(target_os = "windows") {
        return 1
    } else if cfg!(target_os = "linux") {
        return 2
    } else if cfg!(target_os = "macos") {
        return 3
    } else {
        return 0
    }
}

pub async fn download_file(url: &str, path: &PathBuf) -> Result<(), Box<dyn std::error::Error>> {
    // Utilise reqwest::get (asynchrone) et non reqwest::blocking::get
    let response = reqwest::get(url).await?;

    // Le reste dépend de comment tu écris ton fichier,
    // par exemple avec tokio::fs::write ou en récupérant les bytes :
    let bytes = response.bytes().await?;
    std::fs::write(path, bytes)?;

    Ok(())
}