use crate::config::gest_index::{check_date, load_json_application};
use crate::depots::index::load_depots;

pub mod depots;
pub mod config;

#[tokio::main]
async fn main() {
    println!("Arrera Hub");
    /*if check_date(){
        println!("Depots a jour");
    }else{
        println!("Depots pas a jour");
        match load_depots() {
            Ok(_) => println!("Debots mise a jour avec succes"),
            Err(e) => println!("Imposible de mettre a jour le depots : {}", e),
        }
    }*/

    load_depots();

    println!("Recuperation de depots de SIX et Arrera Interface");

    println!("Six : {:?}", load_json_application("assistants", "six").await);

    println!("Interface : {:?}", load_json_application("application","arrera").await);
}
