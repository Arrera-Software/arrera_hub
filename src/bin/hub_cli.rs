use std::env;
use arrera_hub::hub;
#[tokio::main]
async fn main() {

    let hub = hub::arrera_hub::ArreraHub::new().await.unwrap();

    let args: Vec<String> = env::args().collect();
    if args.len() > 1 {
        if args[1] == "help" {
            println!("Arrera Hub\n- install\n- uninstall\n- installed\n- help \n- about\n- available\n- update\n- check-update")
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
                    Err(e) => println!("Erreur lors de l'installation de {} : {}", soft, e),
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
        }else if args[1] == "check-update" {
            let list_soft = hub.update_check().await.unwrap();
            if list_soft.len() > 0{
                println!("Application arrêtera à mettre à jour");
                for software in list_soft {
                    println!("- {}", software.name);
                }
                println!("Lancer la commande update pour tous les mettre à jour");
            }else{println!("Toutes les applications Arrera sont à jour.");}

        }else if args[1] == "uninstall" {
            if args.len() > 2 {
                let soft = args[2].clone();
                println!("Désinstallation de {} .. ", soft);
                match hub.uninstall_soft(&soft).await {
                    Ok(_) => println!("{} a été désinstallé avec succès", soft),
                    Err(e) => println!("Erreur lors de la désinstallation de {} : {}", soft, e),
                }
            } else {
                println!("uninstall (name software)");
            }
        } else if args[1] == "update" {
            println!("Mise à jour des applications Arrera ...");
            match hub.update_soft().await {
                Ok(_) => println!("Toutes les mises à jour ont été installées avec succès."),
                Err(e) => println!("Erreur lors de la mise à jour : {}", e),
            }
        }
    } else {
        println!("Arrera Hub\n- install\n- uninstall\n- installed\n- help \n- about\n- available\n- update\n- check-update")

    }
}
