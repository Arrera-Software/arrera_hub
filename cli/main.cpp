#include <iostream>
#include <QCoreApplication>
#include <QTimer>
#include "../core/hub.h"

using namespace std;

#define VERSION_HUB "I2026-0.00"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QStringList args = app.arguments();

    Hub hub = Hub("https://raw.githubusercontent.com/Arrera-Software/distribution/refs/heads/main/index.json",&app);

    if (args.length() <= 1) {
        cout << "Aucun argument fourni. Utilisez 'help' pour voir la liste des commandes." << endl;
        return 0;
    }

    QString command = args[1];

    if (command == "help" || command == "-help") {
        cout << "Arrera Hub\n" <<
                "- update_depots : Mise à jour du dépôt d'Arrera Hub"<<
                "- install {soft} : Installation du logiciel Arrera écrit à la place de {soft}\n" <<
                "- uninstall {soft} : Désinstallation du logiciel Arrera écrit à la place de {soft}\n" <<
                "- installed : Montre la liste des logiciels Arrera installés sur l'ordinateur avec Arrera Hub\n" <<
                "- help : Montre toutes les commandes possibles d'Arrera Hub\n" <<
                "- about : Vue à propos de l'application en ligne de commande Arrera Hub\n" <<
                "- available : Liste les logiciels disponibles à l'installation avec Arrera Hub\n" <<
                "- update {soft} : Met à jour le logiciel écrit à la place de {soft}\n" <<
                "- available_updates : Liste les applications Arrera qui ont besoin d'une mise à jour" << endl;

        return 0;
    }
    else if (command == "update_depots" || command == "-update_depots") {
        cout << "Mise à jour des dépôts en cours..." << endl;

        // 1. On crée une boucle d'événements locale
        QEventLoop loop;
        bool resultat_maj = false;

        QObject::connect(&hub, &Hub::depotsUpdated, [&resultat_maj, &loop](bool success) {
            resultat_maj = success;
            loop.quit();
        });

        bool requete_lancee = hub.update_depots();

        if (requete_lancee) {
            loop.exec();

            if (resultat_maj) {
                cout << "[SUCCES] Les dépôts ont été mis à jour avec succès !" << endl;
            } else {
                cout << "[ERREUR] Le téléchargement des dépôts a échoué (problème réseau ou serveur)." << endl;
            }
        } else {
            cout << "[ERREUR] Impossible de lancer la mise à jour (URL des dépôts invalide ou manquante)." << endl;
        }
    }
    else if (command == "about" || command == "-about") {
        cout << "Arrera Hub by Arrera Software" << endl;
        cout << "Version " << VERSION_HUB << endl;
        cout << "Copyright Arrera-Software by Baptiste P 2023-2026" << endl;
        return 0;
    }
    else if (command == "install" || command == "-install") {
        if (args.length() < 3) {
            cout << "Erreur : Veuillez preciser le nom de l'application a installer." << endl;
            return 1;
        }
        QString appName = args[2];

        QEventLoop loop;
        QObject::connect(&hub, &Hub::app_installed, [&](bool success) {
            if (success) {
                cout << "Installation de " << appName.toStdString() << " reussie avec succes." << endl;
            } else {
                cout << "Erreur lors de l'installation de " << appName.toStdString() << "." << endl;
            }
            loop.quit(); // Quitte la boucle d'attente
        });

        cout << "Installation de " << appName.toStdString() << " en cours..." << endl;
        hub.install_software(appName);
        loop.exec();

        return 0;
    }
    else if (command == "uninstall" || command == "-uninstall") {
        if (args.length() < 3) {
            cout << "Erreur : Veuillez preciser le nom de l'application a desinstaller." << endl;
            return 1;
        }
        QString appName = args[2];

        QEventLoop loop;
        bool operationSuccess = false;

        QObject::connect(&hub, &Hub::app_uninstall, [&loop, &operationSuccess, appName](bool success) {
            operationSuccess = success;

            if (success) {
                cout << "Desinstallation de " << appName.toStdString() << " reussie." << endl;
            } else {
                cout << "Erreur lors de la desinstallation ou logiciel introuvable." << endl;
            }

            loop.quit();
        });

        QTimer::singleShot(0, [&hub, appName]() {
            hub.uninstall_software(appName);
        });

        loop.exec();

        return operationSuccess ? 0 : 1;
    }
    else if (command == "installed" || command == "-installed") {
        QStringList installedApps = hub.get_soft_installed();

        if (installedApps.isEmpty()) {
            cout << "Aucune application n'est actuellement installee." << endl;
        } else {
            cout << "=== Applications installees ===" << endl;
            for (const QString &soft : installedApps) {
                cout << "- " << soft.toStdString() << endl;
            }
        }

        return 0;
    }
    else if (command == "available_updates" || command == "-available_updates") {
        QStringList soft = hub.get_soft_with_update();

        if (soft.isEmpty()){
            cout << "Tous les logiciels Arrera sont à jour" << endl;
        }else {
            cout << "Liste des mises à jour à faire :" ;
            for (QString s : soft){
                cout << "\n- " << s.toStdString();
            }
            cout << endl;
        }

        return 0;
    }
    else if (command == "update" || command == "-update") {
        // On utilise la nouvelle méthode pour ne cibler que les applications obsolètes
        QStringList appsToUpdate = hub.get_soft_with_update();

        if (appsToUpdate.isEmpty()) {
            cout << "Toutes les applications installees sont deja a jour." << endl;
            return 0;
        }

        cout << "Recherche et application des mises a jour..." << endl;

        for (const QString &soft : appsToUpdate) {
            cout << "\nLancement de la mise a jour pour " << soft.toStdString() << "..." << endl;

            QEventLoop updateLoop;
            QObject::connect(&hub, &Hub::app_update, [&](bool success) {
                if (success) {
                    cout << "-> Mise a jour de " << soft.toStdString() << " reussie." << endl;
                } else {
                    cout << "-> Echec de la mise a jour de " << soft.toStdString() << "." << endl;
                }
                updateLoop.quit();
            });

            hub.update_software(soft);
            updateLoop.exec();
        }
        cout << "\nProcessus de mise a jour termine." << endl;
        return 0;
    }else if (command == "available" || command == "-available"){
        QStringList soft = hub.get_soft_available();

        if (soft.isEmpty()){
            cout << "Aucun logiciel disponible" << endl;
        }else{
            cout << "Logiciels disponibles :";
            for (QString s : soft){
                cout << "\n- " << s.toStdString() ;
            }
            cout << endl;
        }
    }
    else {
        cout << "Commande non reconnue. Utilisez 'help' pour voir la liste des commandes." << endl;
        return 1;
    }

    return 0;
}
