# Install DMG

```rust
fn install_dmg(dmg_path: &str, app_name: &str, volume_name: &str) -> Result<(), Box<dyn std::error::Error>> {
    // 1. Suppression des attributs de quarantaine (Gatekeeper)
    println!("Nettoyage des attributs de quarantaine...");
    Command::new("xattr")
        .args(["-cr", dmg_path])
        .status()?;

    // 2. Montage du DMG
    println!("Montage du disque...");
    Command::new("hdiutil")
        .args(["attach", dmg_path, "-nobrowse", "-quiet"])
        .status()?;

    // 3. Définition des chemins (Montage vs Destination)
    let source_path = format!("/Volumes/{}/{}", volume_name, app_name);
    let dest_path = format!("/Applications/{}", app_name);

    // 4. Copie de l'application (utilisation de ditto pour macOS)
    println!("Copie de l'application vers /Applications...");
    let status = Command::new("ditto")
        .args([&source_path, &dest_path])
        .status()?;

    if !status.success() {
        eprintln!("Erreur lors de la copie. Vérifiez les permissions.");
    }

    // 5. Démontage du DMG
    println!("Éjection du volume...");
    Command::new("hdiutil")
        .args(["detach", &format!("/Volumes/{}", volume_name), "-quiet"])
        .status()?;

    Ok(())
}
```
```toml
[dependencies]
zip = "0.6"
# Optionnel pour faciliter la manipulation des chemins
path-clean = "1.0"
``

```rust
use std::fs;
use std::io;
use std::path::Path;
use zip::ZipArchive;

fn extract_zip(zip_path: &str, extract_to: &str) -> Result<(), Box<dyn std::error::Error>> {
    let file = fs::File::open(zip_path)?;
    let mut archive = ZipArchive::new(file)?;

    let target_dir = Path::new(extract_to);
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

        // Sur macOS/Linux, il faut remettre les permissions d'exécution si besoin
        #[cfg(unix)]
        {
            use std::os::unix::fs::PermissionsExt;
            if let Some(mode) = file.unix_mode() {
                fs::set_permissions(&outpath, fs::Permissions::from_mode(mode))?;
            }
        }
    }
    println!("Extraction terminée dans : {}", extract_to);
    Ok(())
}
``