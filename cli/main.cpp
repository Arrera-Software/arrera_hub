#include <iostream>
#include <QCoreApplication>
#include <QTimer>
#include "../core/hub.h"

using namespace std;

int main(int argc, char *argv[]){
    QCoreApplication a(argc, argv);

    cout << "Arrera hub"<< endl;
    Hub arrera_hub = Hub("https://raw.githubusercontent.com/Arrera-Software/distribution/refs/heads/main/index.json",&a);


    QObject::connect(&arrera_hub, &Hub::finnish, &a,&QCoreApplication::quit);
    QObject::connect(&arrera_hub, &Hub::depotsUpdated, &a,&QCoreApplication::quit);



    cout << "Update depots" << endl;
    arrera_hub.update_depots();



     /*cout << "Liste des logiciel installer :\n";
    QStringList list_installed = arrera_hub.get_soft_installed();

    for (const QString l:list_installed){
        cout << "- " + l.toStdString()+"\n";
    }

    cout << "Liste des logiciel disponible :\n";
    QStringList list_soft = arrera_hub.get_soft_available();

    for (const QString l:list_soft){
        cout << "- " + l.toStdString()+"\n";
    }

    cout << endl;*/



    // Declenche la fermeture une fois la boucle Qt demarree.
    QTimer::singleShot(0, &arrera_hub, &Hub::quit);

    return a.exec();
}
