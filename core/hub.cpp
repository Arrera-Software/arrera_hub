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
                depots_file = filename;
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

bool Hub::install_software(QString soft)
{
    return true;
}

bool Hub::uninstall_software(QString soft)
{
    return true;
}

bool Hub::update_software(QString soft)
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

                    QJsonObject appObj = appArray[i].toObject();

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
    QStringList out;

    setting_file->beginGroup("software");
    QStringList key = setting_file->allKeys();
    setting_file->endGroup();

    for (const QString soft : key){
        QString v = read_valeur(soft);

        if (v != "none" && v != "error"){
            out.append(soft);
        }
    }
    return out;
}

void Hub::quit(){
    emit finnish();
}

QString Hub::get_url_img(QString soft){
    QFile file(config_folder + "/depots.json");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        cout << "fichier non ouvert " << depots_file.toStdString() << endl;
        return "error";
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        cout << "Erreur pars " << endl;
        return "error";
    }

    if (doc.isObject()) {
        QJsonObject root = doc.object();

        for (const QString& key : root.keys()) {
            QJsonValue val = root.value(key);

            if (val.isArray()) {
                QJsonArray array = val.toArray();
                for (const QJsonValue& item : array) {
                    if (item.isObject()) {
                        QJsonObject obj = item.toObject();
                        if (obj.value("name").toString().compare(
                                soft, Qt::CaseInsensitive) == 0) {
                            return obj.value("img").toString();
                        }
                    }
                }
            }
        }
    }

    return "error";
}

// Methode private

void Hub::check_software_update(QString soft){
    get_dict_software(soft, [this, soft](QJsonObject dict) {

        if (dict.isEmpty()) {
            cout << "Impossible de récupérer les données pour" << soft.toStdString() << endl;
            return;
        }

        QString version_depots = dict.value("version").toString();
        QString version_local = read_valeur(soft);

        if (version_local == "error" || version_local == version_depots) {
            emit update_check(soft, false);
        } else {
            emit update_check(soft, true);
        }

    });
}

void Hub::get_dict_software(QString soft, std::function<void(QJsonObject)> callback) {
    // 1. Ouvrir le fichier local "depots.json"
    QFile file(config_folder + "/depots.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        callback(QJsonObject()); // Erreur -> on retourne un JSON vide
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    // 2. Parser le fichier local
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        callback(QJsonObject()); // Erreur de syntaxe locale
        return;
    }

    QString targetUrl = "";
    QJsonObject root = doc.object();

    // 3. Chercher l'URL du logiciel ciblé
    for (const QString& key : root.keys()) {
        QJsonValue val = root.value(key);

        if (val.isArray()) {
            QJsonArray array = val.toArray();
            for (const QJsonValue& item : array) {
                if (item.isObject()) {
                    QJsonObject obj = item.toObject();
                    if (obj.value("name").toString().compare(soft, Qt::CaseInsensitive) == 0) {
                        targetUrl = obj.value("url").toString();
                        break;
                    }
                }
            }
        }
        if (!targetUrl.isEmpty()) break;
    }

    // Si on n'a pas trouvé le logiciel ou l'URL
    if (targetUrl.isEmpty()) {
        callback(QJsonObject());
        return;
    }

    // 4. Lancer la requête réseau asynchrone
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request((QUrl(targetUrl)));
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

    QNetworkReply *reply = manager->get(request);

    // 5. Connecter le signal de fin au traitement (Lambda)
    QObject::connect(reply, &QNetworkReply::finished, [manager, reply, callback]() {
        QJsonObject resultObj;

        // Si le téléchargement a réussi
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray remoteData = reply->readAll();
            QJsonParseError remoteError;
            QJsonDocument remoteDoc = QJsonDocument::fromJson(remoteData, &remoteError);

            // Si le JSON distant est valide
            if (remoteError.error == QJsonParseError::NoError && remoteDoc.isObject()) {
                resultObj = remoteDoc.object();
            }
        }

        // On déclenche le callback en lui passant le résultat (rempli ou vide)
        callback(resultObj);

        // On nettoie la mémoire proprement
        reply->deleteLater();
        manager->deleteLater();
    });
}

bool Hub::write_setting(const QString &key, const QString &value)
{
    if (!setting_loaded) return false;
    if (key.isEmpty() || value.isEmpty()) return false;
    setting_file->setValue("software/" + key, value);
    setting_file->sync();
    return true;
}

QString Hub::read_valeur(const QString &key){
    if (!setting_loaded) return "error";
    if (key.isEmpty()) return "error";
    setting_file->beginGroup("software");
    QString val = setting_file->value(key, "error").toString();
    setting_file->endGroup();
    return val;
}
