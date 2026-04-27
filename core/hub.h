#include <QString>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
// Partie reseau
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
// Partie JSON
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>

// Debug
#include <iostream>
using namespace std;

class Hub : public QObject
{
    Q_OBJECT

    public:
        explicit Hub(QObject *parent = nullptr);
        explicit Hub(QString url = "", QObject *parent = nullptr);
        virtual ~Hub();
        // Gestion de depots
        bool update_depots();

        // Mise a jour des logiel
        QStringList get_software_with_update();

        //Installation et desinstallation des logiciel
        bool install_software(QString soft);
        bool uninstall_software(QString soft);
        bool udpate_software(QString soft);

        // Methode pour avoir sur les logicel
        QStringList get_soft_available();
        QStringList get_soft_installed();

        // Methode utilitaire
        void quit();

    protected:

    private:
        // Atribut
        QString config_folder,config_file;
        QString depots_url;
        QSettings* setting_file;
        bool depots_url_saved, setting_loaded,config_init = false,file_created = false;
        // Methode
        bool write_setting(const QString &key, const QString &value);
        QString read_valeur(const QString &key);

    signals:
        void depotsUpdated(bool success);
        void finnish();

};
