#include <iostream>
#include <QCoreApplication>
#include "../core/hub.h"

using namespace std;

int main(int argc, char *argv[]){
    QCoreApplication a(argc, argv);

    cout << "Arrera hub"<< endl;
    Hub arrera_hub = Hub("https://raw.githubusercontent.com/Arrera-Software/distribution/refs/heads/main/index.json",&a);

    QObject::connect(&arrera_hub, &Hub::depotsUpdated, &a,&QCoreApplication::quit);

    cout << "Update depots" << endl;
    arrera_hub.update_depots();

    return a.exec();
}
