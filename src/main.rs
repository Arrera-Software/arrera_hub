use crate::depots::gest_depots::{check_date, get_img_application, get_link_download, get_name_application, get_version_application};
use crate::depots::install_app::install_app;
use crate::depots::index::load_depots;

pub mod depots;
pub mod config;

#[tokio::main]
async fn main() {
    println!("Arrera Hub");
    if check_date(){
        println!("Depots a jour");
    }else{
        println!("Depots pas a jour");
        match load_depots().await {
            Ok(_) => println!("Debots mise a jour avec succes"),
            Err(e) => println!("Imposible de mettre a jour le depots : {}", e),
        }
    }

    if let Err(e) = load_depots().await {
        println!("Impossible de mettre à jour les dépôts : {}", e);
    }
    println!("Img SIX : {:?}", get_img_application("assistant", "Six"));

    println!("Recuperation de depots de SIX et Arrera Interface");

    println!("Six : \n url {:?} \n Version : {:?} \n Name : {:?}",
             get_link_download("assistant", "Six").await,
            get_version_application("assistant", "Six").await,
            get_name_application("assistant", "Six").await);
    println!("\n\n");
    println!("Interface : \n url {:?} \n Version : {:?} \n Name : {:?}",
             get_link_download("application","arrera").await,
             get_version_application("application","arrera").await,
             get_name_application("application","arrera").await);

    println!("\n\n");
    println!("Tentavite de download du zip d'Arrera");

    install_app("assistant", "Six").await.expect("TODO: panic message");
}
