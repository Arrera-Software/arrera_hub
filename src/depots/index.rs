use crate::config::gest_index::save_index;

pub async  fn load_depots() -> Result<(), Box<dyn std::error::Error>> {
    // Dois recuprer le contenu de l'index et le stoker dans un fichier json dans .config (Pour mac os et linux)
    let content: String = reqwest::get(
        "https://raw.githubusercontent.com/Arrera-Software/distribution/refs/heads/main/index.json"
    ).await?.text().await?;

    let _ = save_index(&content);
    Ok(())
}