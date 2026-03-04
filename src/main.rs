use crate::config::gest_index::{check_date, get_img_application, load_json_application};
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

    // OU pour une meilleure gestion des erreurs (recommandé) :
    if let Err(e) = load_depots().await {
        println!("Impossible de mettre à jour les dépôts : {}", e);
    }
    println!("Img SIX : {:?}", get_img_application("assistant", "Six"));

    println!("Recuperation de depots de SIX et Arrera Interface");

    println!("Six : {:?}", load_json_application("assistant", "Six").await.unwrap());

    println!("Interface : {:?}", load_json_application("application","arrera").await.unwrap());
}
