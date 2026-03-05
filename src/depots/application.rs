use std::env;
use std::fs::File;
use std::io::Write;
use crate::config::gest_index::get_link_download;
use futures_util::StreamExt;

pub async fn install_app(cathegorie: &str, nom: &str) -> Result<(), Box<dyn std::error::Error>> {
    let zip_path =  env::temp_dir().join("application.zip");

    let url = get_link_download(cathegorie,nom).await;

    let client = reqwest::Client::new();
    let response = client.get(url).send().await?;

    if !response.status().is_success() {
        return Err(format!("Erreur HTTP : {}", response.status()).into());
    }

    let total_size = response.content_length().unwrap_or(0);
    let mut downloaded: u64 = 0;
    let mut stream = response.bytes_stream();

    let mut file = File::create(zip_path)?;

    while let Some(item) = stream.next().await {
        let chunk = item?;
        file.write_all(&chunk)?;

        downloaded += chunk.len() as u64;

        // C'est ici que tu enverrais 'downloaded' à ton GUI
        if total_size > 0 {
            let percent = (downloaded as f64 / total_size as f64) * 100.0;
            print!("\rProgression : {:.2}%", percent);
            std::io::stdout().flush()?;
        }
    }

    Ok(())
}