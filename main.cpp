#include "hub_gui.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    hub_gui w;
    w.show();
    return a.exec();
}
