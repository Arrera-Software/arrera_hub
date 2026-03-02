pub mod depots;
pub mod config;

fn main() {
    println!("Arrera Hub");

    config::dect_os();
    depots::index::load_depots();
}
