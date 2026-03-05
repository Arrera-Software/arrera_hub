use std::env;
use std::fs::File;
use std::io::Write;
use crate::config::gest_index::get_link_download;
use futures_util::StreamExt;
use zip::ZipArchive;
use std::fs;
use std::path::Path;
use std::io;

pub async fn install_app(cathegorie: &str, nom: &str) -> Result<(), Box<dyn std::error::Error>> {
    let zip_path =  env::temp_dir().join("application.zip");

    let url = get_link_download(cathegorie,nom).await;

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

        // C'est ici que tu enverrais 'downloaded' à ton GUI
        if total_size > 0 {
            let percent = (downloaded as f64 / total_size as f64) * 100.0;
            print!("\rProgression : {:.2}%", percent);
            std::io::stdout().flush()?;
        }
    }

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

    Ok(())
}