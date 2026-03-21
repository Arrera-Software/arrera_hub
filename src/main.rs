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
        }else if args[1] == "install"{
            if args.len() > 2 {
                let soft = args[2].clone();
                println!("Install {} .. ",soft);
                match hub.install_soft(&soft).await {
                    Ok(_) => println!("{} installed successfully", soft),
                    Err(e) => println!("Installation de {} ", soft),
                }
            }else{
                println!("install (name software)");
            }
        }else if args[1] == "installed"{
            let list_soft = hub.get_soft_installed().unwrap();
            println!("Logiciels Arrera installés :");
            for software in list_soft {
                println!("- {}", software.name);
            }
        }

    } else {
        println!("Arrera Hub\n- help \n- about\n- available\n- update")

    }

    /*
    else if args[1] == "update" {
            println!("Mise a jour des logiciel Arrera");
            hub.update().await.expect("Update depots failed");
        }
    */
}
