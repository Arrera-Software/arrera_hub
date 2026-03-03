use crate::config::gest_index::check_date;
use crate::depots::index::load_depots;

pub mod depots;
pub mod config;

fn main() {
    println!("Arrera Hub");
    if check_date(){
        println!("Depots a jour");
    }else{
        println!("Depots pas a jour");
        match load_depots() {
            Ok(_) => println!("Debots mise a jour avec succes"),
            Err(e) => println!("Imposible de mettre a jour le depots : {}", e),
        }
    }
}
