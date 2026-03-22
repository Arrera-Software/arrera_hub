use std::error::Error;
use crate::config::user_conf::{add_conf, check_config_existing, read_conf};
use crate::depots::gest_depots::{get_all_software, Item};
use crate::depots::index::load_depots;
use crate::depots::install_app::install_app;
use crate::depots::uninstall_app::uninstall_app;

pub struct ArreraHub {
}
impl ArreraHub {
    pub async fn new() -> Result<Self, Box<dyn std::error::Error>> {
        let config_existed = check_config_existing()?;
        
        if config_existed == false {
            load_depots().await?;
            let list_soft = get_all_software()?;
            for software in list_soft {
                add_conf(&*software.name.as_str().to_lowercase(), "NONE")?;
            }
        }

        Ok(Self {})
    }

    pub async fn update_check(&self) -> Result<Vec<Item>, Box<dyn std::error::Error>> {
        load_depots().await?;

        let all_soft = get_all_software()?;
        let mut updates_available = Vec::new();

        for soft in all_soft {
            if let Some(version_conf) = read_conf(&soft.name.to_lowercase()) {
                if version_conf != "NONE" {
                    let mut target_category = None;
                    if crate::depots::gest_depots::get_img_application("application", &soft.name).is_ok() {
                        target_category = Some("application");
                    } else if crate::depots::gest_depots::get_img_application("assistant", &soft.name).is_ok() {
                        target_category = Some("assistant");
                    }

                    if let Some(category) = target_category {
                        let version_depot = crate::depots::gest_depots::get_version_application(category, &soft.name).await;
                        if !version_depot.is_empty() && version_conf != version_depot {
                            updates_available.push(soft);
                        }
                    }
                }
            }
        }

        Ok(updates_available)
    }

    pub async fn update_soft(&self) -> Result<(), Box<dyn std::error::Error>> {
        let apps_to_update = self.update_check().await?;

        for soft in apps_to_update {
            self.uninstall_soft(&soft.name).await?;
            self.install_soft(&soft.name).await?;
        }

        Ok(())
    }
    pub fn get_soft_available(&self) -> Result<Vec<Item>, Box<dyn Error>> {
        return get_all_software();
    }

    pub fn get_soft_installed(&self) -> Result<Vec<Item>, Box<dyn Error>> {
        let all_soft = get_all_software()?;
        let mut installed_soft = Vec::new();

        for soft in all_soft {
            if let Some(version) = read_conf(&soft.name.to_lowercase()) {
                if version != "NONE" {
                    installed_soft.push(soft);
                }
            }
        }

        Ok(installed_soft)
    }

    pub async fn install_soft(&self, name: &str) -> Result<(), Box<dyn std::error::Error>> {
        let mut target_category = None;
        if crate::depots::gest_depots::get_img_application("application", name).is_ok() {
            target_category = Some("application");
        }

        else if crate::depots::gest_depots::get_img_application("assistant", name).is_ok() {
            target_category = Some("assistant");
        }

        if let Some(category) = target_category {
            install_app(category, name).await?;
            Ok(())
        } else {
            Err("".into())
        }
    }

    pub async fn uninstall_soft(&self, name: &str) -> Result<(), Box<dyn std::error::Error>> {
        // Vérification de l'installation de l'application via la configuration
        if let Some(version) = read_conf(&name.to_lowercase()) {
            if version == "NONE" {
                return Err("L'application n'est pas installée.".into());
            }
        } else {
            return Err("L'application est introuvable dans la configuration.".into());
        }

        let mut target_category = None;
               
        if crate::depots::gest_depots::get_img_application("application", name).is_ok() {
            target_category = Some("application");
        }

        else if crate::depots::gest_depots::get_img_application("assistant", name).is_ok() {
            target_category = Some("assistant");
        }

        if let Some(category) = target_category {
            uninstall_app(category, name).await?;
            Ok(())
        } else {
            Err("Impossible de trouver la catégorie de l'application.".into())
        }
    }
}