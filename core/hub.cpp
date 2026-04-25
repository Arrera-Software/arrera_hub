#include "hub.h"

Hub::Hub(QString url)
{

}

Hub::~Hub()
{

}

bool Hub::update_depots()
{
    return true;
}

QStringList Hub::get_software_with_update()
{
    return [];
}

bool Hub::get_software_udpate(QString soft)
{
    return true;
}

bool Hub::install_software(QString soft)
{
    return true;
}

bool Hub::uninstall_software(QString soft)
{
    return true;
}

QStringList Hub::get_soft_available()
{
    return [];
}

QStringList Hub::get_soft_installed()
{
    return [];
}
