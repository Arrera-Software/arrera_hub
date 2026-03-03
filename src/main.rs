pub mod depots;
pub mod config;

fn main() {
    println!("Arrera Hub");

    config::dect_os();
    let _ = depots::index::load_depots();
    println!("{}", config::user_conf::read_conf("load_index").unwrap())
}
