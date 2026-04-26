#include "hub.h"

Hub::Hub(QObject *parent) : QObject(parent) {}

Hub::Hub(QString url, QObject *parent) : QObject(parent)
{

    if (url.isEmpty() || url == ""){
        depots_url_saved = false;
    }else {
        depots_url = url;
        depots_url_saved = true;
    }

    #if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
        config_init = true;
        config_folder = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.config/arrera-hub";

    #elif defined(Q_OS_WIN)
        config_init = true;
        config_folder = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/arrera-hub";
    #endif

    if (config_init){
        QDir().mkpath(config_folder);

        config_file = config_folder + "/config.ini";

        QFileInfo checkFile(config_file);

        if (!checkFile.exists()){
            QFile newFile(config_file);
            if (newFile.open(QIODevice::WriteOnly)) {
                newFile.close();
                file_created = true;
            }
        }


        try {
            setting_file = new QSettings(config_file,QSettings::IniFormat);
            setting_loaded = true;
        }catch (const std::invalid_argument& e) {
            setting_loaded = false;
        }
        catch (const std::exception& e) {
            setting_loaded = false;
        }
    }

}

Hub::~Hub()
{

}

bool Hub::update_depots()
{
    if (depots_url.isEmpty() || !depots_url_saved) return false;

    try {
        QNetworkAccessManager *manager = new QNetworkAccessManager();
        QUrl url(depots_url);
        QNetworkRequest request(url);

        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

        QNetworkReply *reply = manager->get(request);

        QObject::connect(reply, &QNetworkReply::finished, [this, manager, reply]() {
            bool success = false;

            if (reply->error() == QNetworkReply::NoError) {
                QByteArray data = reply->readAll();
                QString filename = config_folder + "/depots.json";
                QFile file(filename);

                if (file.open(QIODevice::WriteOnly)) {
                    file.write(data);
                    file.close();
                    success = true;
                }
            }

            if (file_created){
                update_depots();
                QStringList list_soft = get_soft_available();
                for(const QString &soft : list_soft){
                    write_setting(soft,"none");
                }
                file_created = false;
            }

            emit depotsUpdated(success);

            reply->deleteLater();
            manager->deleteLater();
        });
        return true;

    } catch (const std::exception& e) {
        return false;
    }
}

QStringList Hub::get_software_with_update()
{
    return {};
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
    QStringList liste_noms;
    QString filename = config_folder + "/depots.json";
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return liste_noms;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
    }

    if (doc.isObject()) {
        QJsonObject rootObj = doc.object();

        QStringList list_application = {"application","assistants"};

        for(const QString &a : list_application ){
            if (rootObj.contains(a) && rootObj[a].isArray()) {

                QJsonArray appArray = rootObj[a].toArray();

                for (int i = 0; i < appArray.size(); ++i) {

                    QJsonObject appObj = appArray[i].toObject(); // On récupère l'objet {"name": "...", "url": "..."}

                    // On extrait le nom et on l'ajoute à notre liste C++
                    if (appObj.contains("name")) {
                        QString nom = appObj["name"].toString();
                        liste_noms.append(nom);
                    }
                }
            }
        }
    }

    return liste_noms;
}

QStringList Hub::get_soft_installed()
{
    return {};
}

// Methode private

bool Hub::write_setting(const QString &key, const QString &value)
{
    if (!setting_loaded) return false;
    if (key.isEmpty() || value.isEmpty()) return false;
    setting_file->setValue("software/" + key, value);
    setting_file->sync();
    return true;
}
