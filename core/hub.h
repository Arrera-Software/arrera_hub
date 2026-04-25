#include <QString>

class Hub
{
    public:
        Hub(QString url);
        virtual ~Hub();
        // Gestion de depots
        bool update_depots();

        // Mise a jour des logiel
        QStringList get_software_with_update();
        bool get_software_udpate(QString soft);

        //Installation et desinstallation des logiciel
        bool install_software(QString soft);
        bool uninstall_software(QString soft);

        // Methode pour avoir sur les logicel
        QStringList get_soft_available();
        QStringList get_soft_installed();

    protected:

    private:
};
