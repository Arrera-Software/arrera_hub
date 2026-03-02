use reqwest::blocking::get;
pub fn load_depots() -> Result<(), Box<dyn std::error::Error>> {
    // Dois recuprer le contenu de l'index et le stoker dans un fichier json dans .config (Pour mac os et linux)
    let content = reqwest::blocking::get(
        "https://raw.githubusercontent.com/Arrera-Software/distribution/refs/heads/main/index.json")
        ?.text()?;
    
    Ok(())
}