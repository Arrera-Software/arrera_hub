#include <iostream>
#include <QCoreApplication>
#include "../core/hub.h"

using namespace std;

int main(int argc, char *argv[]){
    QCoreApplication a(argc, argv);

    cout << "Arrera hub"<< endl;
    Hub arrera_hub = Hub("https://raw.githubusercontent.com/Arrera-Software/distribution/refs/heads/main/index.json",&a);


    QObject::connect(&arrera_hub, &Hub::finnish, &a,&QCoreApplication::quit);
    QObject::connect(&arrera_hub, &Hub::depotsUpdated, &a,&QCoreApplication::quit);

    /*

    cout << "Update depots" << endl;
    arrera_hub.update_depots();
    */


    cout << "Liste depots :\n";
    QStringList list = arrera_hub.get_soft_installed();

    for (const QString l:list){
        cout << "- " + l.toStdString()+"\n";
    }

    cout << endl;

    arrera_hub.quit();

    return a.exec();
}
