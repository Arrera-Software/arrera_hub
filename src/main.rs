use std::env;

mod hub;
mod depots;
mod config;


#[tokio::main]
async fn main() {

    let hub = hub::arrera_hub::ArreraHub::new().await.unwrap();

    let args: Vec<String> = env::args().collect();
    if args.len() > 1 {
        if args[1] == "help" {
            println!("Arrera Hub\n- help \n- about\n- available\n- update")
        } else if args[1] == "about" {
            println!("Arrera Hub by Arrera Software");
            println!("Version I2026-0.00");
            println!("Copyright Arrera-Software by Baptiste P 2023-2026");
        } else if args[1] == "available" {
            let list_soft = hub.get_soft_available().unwrap();
            println!("Logiciels disponibles chez Arrera :");
            for software in list_soft {
                println!("- {}", software.name);
            }
        }

    } else {
        println!("Arrera Hub\n- help \n- about\n - available\n - update")
    }


}
