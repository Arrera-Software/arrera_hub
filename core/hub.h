#include <QString>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
// Partie reseau
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QProcess>
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
        void check_software_update(QString soft);

        //Installation et desinstallation des logiciel
        void install_software(QString soft);
        bool uninstall_software(QString soft);
        bool update_software(QString soft);

        // Methode pour avoir sur les logicel
        QStringList get_soft_available();
        QStringList get_soft_installed();

        // Methode utilitaire
        void quit();

        // Methode pour l'URL IMG
        QString get_url_img(QString soft);

    private:
        // Atribut
        QString config_folder,config_file,depots_file;
        QString depots_url;
        QSettings* setting_file;
        bool depots_url_saved, setting_loaded,config_init = false,file_created = false;
        // Methode
        bool write_setting(const QString &key, const QString &value);
        QString read_valeur(const QString &key);
        void get_dict_software(QString soft, function<void(QJsonObject)> callback);

    signals:
        void depotsUpdated(bool success);
        void update_check(QString soft,bool update);
        void finnish();

        // Download
        void app_installed(bool succes);

};
