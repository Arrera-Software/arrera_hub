use crate::config::user_conf::{add_conf, check_config_existing};
use crate::depots::gest_depots::get_all_software;
use crate::depots::index::load_depots;
pub struct ArreraHub {
}
impl ArreraHub {
    pub async fn new() -> Result<Self, Box<dyn std::error::Error>> {
        let config_existed = check_config_existing()?;
        
        if config_existed == false {
            load_depots().await?;
            let list_soft = get_all_software()?;
            for software in list_soft {
                add_conf(software.name.as_str(), "NONE")?;
            }
        }

        Ok(Self {})
    }

    pub fn update(&self){

    }

    pub fn update_all_soft(&self){

    }
    pub fn get_soft_available(&self){

    }

    pub fn get_soft_installed(&self){

    }

    pub fn install_soft(&self) {

    }
}