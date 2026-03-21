use std::error::Error;
use crate::config::user_conf::{add_conf, check_config_existing, read_conf};
use crate::depots::gest_depots::{get_all_software, Item};
use crate::depots::index::load_depots;
use crate::depots::install_app::install_app;

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

    pub async fn update(&self) -> Result<(), Box<dyn std::error::Error>> {
        load_depots().await?;
        Ok(())
    }

    pub fn update_all_soft(&self){

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
}
