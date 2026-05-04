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
        // GenericConfigLocation pointe vers AppData/Roaming
        QString roaming = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);

        // On construit le chemin manuellement à la racine de Roaming
        QString base_dir = roaming + "/arrera-hub";

        config_folder = base_dir;
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
                    QString softLower = soft.toLower();

                    write_setting(softLower,"none");
                    write_setting(softLower+"_install","none");
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

void Hub::install_software(QString soft)
{
    perform_installation(soft, [this](bool success) {
        emit app_installed(success);
    });
}

bool Hub::uninstall_software(QString soft)
{
    QString version = read_valeur(soft);
    QString emplacement = read_valeur(soft+"_install");

    if (version == "none" && emplacement == "none") return false;
    else {
        QDir application(emplacement);

        if (application.exists()) {
            if (application.removeRecursively()) {
                write_setting(soft,"none");
                write_setting(soft+"_install","none");
                #if defined(Q_OS_LINUX)
                QString desktopPath = QDir::homePath() + "/.local/share/applications/" + soft + ".desktop";
                if (QFile::exists(desktopPath)){
                    QFile::remove(desktopPath);
                }
                return true;
                #elif defined(Q_OS_WIN)
                QString startMenuPath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
                QString shortcutPath = QDir(startMenuPath).filePath(soft + ".lnk");
                cout << shortcutPath.toStdString();
                if (QFile::exists(shortcutPath)){
                    QFile::remove(shortcutPath);
                }
                return true;
                #elif defined(Q_OS_MAC)
                return true;
                #endif
            }else return false;
        } else return false;
    }
}

void Hub::update_software(QString soft)
{
    QString version_depots = get_data_from_depots(soft, "version");
    QString version_local = read_valeur(soft);
    QString emplacement = read_valeur(soft + "_install");

    if (version_depots == "error" || version_local == "error" || version_local == "none" || version_local == version_depots) {
        emit app_update(false);
        return;
    }

    QDir application(emplacement);
    if (application.exists()) {
        if (application.removeRecursively()) {

            perform_installation(soft, [this](bool success) {
                emit app_update(success);
            });

        } else {
            emit app_update(false);
        }
    } else {
        emit app_update(false);
    }
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
        if (!soft.contains("_install")){
            QString v = read_valeur(soft);

            if (v != "none" && v != "error"){
                out.append(soft);
            }
        }
    }
    return out;
}

QStringList Hub::get_soft_with_update(){
    QStringList out;

    QStringList soft_installed = get_soft_installed();

    for (QString soft : soft_installed){
        QString local = read_valeur(soft);
        QString oneline = get_data_from_depots(soft, "version");
        if (oneline != "error" && oneline != ""){
            if (!(local == oneline)) out.append(soft);
        }
    }

    return out;
}

QString Hub::get_url_img(QString soft){
    return get_data_from_depots(soft, "img");
}

// Methode private

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

void Hub::perform_installation(const QString& soft, std::function<void(bool)> callback)
{
#if defined(Q_OS_LINUX)

    get_dict_software(soft, [this, soft, callback](QJsonObject dict) {
        if (dict.isEmpty()) {
            if (callback) callback(false);
            return;
        }

        QString url_download = dict.value("download_linux").toString();
        QString version = get_data_from_depots(soft,"version");
        if (url_download.isEmpty() || version.isEmpty()) {
            if (callback) callback(false);
            return;
        }

        QUrl url(url_download);
        QFileInfo fileInfo(url.path());
        QString folderName = fileInfo.baseName();

        if (folderName.isEmpty()) folderName = soft.toLower().replace(" ", "_");

        QString appInstallDir = QDir::homePath() + "/Applications/";
        QString zipPath = QDir::tempPath() + "/" + folderName + ".zip";

        QDir().mkpath(appInstallDir);

        QNetworkAccessManager *dlManager = new QNetworkAccessManager(this);
        QNetworkRequest request(url);
        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

        QNetworkReply *reply = dlManager->get(request);

        connect(reply, &QNetworkReply::finished, [this, reply, dlManager, zipPath, appInstallDir, soft, folderName, version, callback]() {
            if (reply->error() == QNetworkReply::NoError) {
                QString iconPath = "system-run";
                QFile file(zipPath);
                if (file.open(QIODevice::WriteOnly)) {
                    file.write(reply->readAll());
                    file.close();

                    QProcess::execute("unzip", {"-o","-q", zipPath, "-d", appInstallDir});

                    QDir appDir(appInstallDir + "/" + folderName);

                    QString executableName;
                    appDir.setFilter(QDir::Files | QDir::NoSymLinks);
                    QFileInfoList fileList = appDir.entryInfoList();

                    for (const QFileInfo &fileInfo : fileList) {
                        QString name = fileInfo.fileName();

                        if (name != "launch.sh" && !name.endsWith(".bat") && !name.endsWith(".dll")) {
                            executableName = name;
                            break;
                        }
                    }

                    for (const QFileInfo &fileInfo : fileList) {
                        QString name = fileInfo.fileName();
                        if (name.endsWith(".png", Qt::CaseInsensitive)) {
                            iconPath = fileInfo.absoluteFilePath();
                            break;
                        }
                    }

                    if (iconPath == "system-run") {
                        QDir assetDir(appInstallDir+ "/" + folderName + "/asset/icon/linux");
                        if (assetDir.exists()) {
                            assetDir.setFilter(QDir::Files | QDir::NoSymLinks);
                            QFileInfoList assetFiles = assetDir.entryInfoList();

                            for (const QFileInfo &fileInfo : assetFiles) {
                                if (fileInfo.fileName().endsWith(".png", Qt::CaseInsensitive)) {
                                    iconPath = fileInfo.absoluteFilePath();
                                    break;
                                }
                            }
                        }
                    }

                    if (iconPath == "system-run") {
                        QDir assetDir(appInstallDir+ "/" + folderName + "/asset/icone/linux");

                        if (assetDir.exists()) {
                            assetDir.setFilter(QDir::Files | QDir::NoSymLinks);
                            QFileInfoList assetFiles = assetDir.entryInfoList();

                            for (const QFileInfo &fileInfo : assetFiles) {
                                if (fileInfo.fileName().endsWith(".png", Qt::CaseInsensitive)) {
                                    iconPath = fileInfo.absoluteFilePath();
                                    break;
                                }
                            }
                        }
                    }

                    if (!executableName.isEmpty()) {
                        QString exePath = appInstallDir + folderName + "/" + executableName;
                        QProcess::execute("chmod", {"+x", exePath});
                    }

                    QString shPath = appInstallDir + folderName + "/launch.sh";
                    QFile shFile(shPath);
                    if (shFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                        QTextStream out(&shFile);
                        out << "#!/bin/bash\n";
                        out << "cd \"$(dirname \"$0\")\"\n";
                        out << "./\"" << executableName << "\"\n";
                        shFile.close();

                        shFile.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner |
                                              QFileDevice::ReadGroup | QFileDevice::ExeGroup |
                                              QFileDevice::ReadOther | QFileDevice::ExeOther);
                    }

                    QString desktopPath = QDir::homePath() + "/.local/share/applications/" + soft + ".desktop";
                    QFile dFile(desktopPath);
                    if (dFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                        QTextStream out(&dFile);
                        out << "[Desktop Entry]\n"
                            << "Name=" << soft << "\n"
                            << "Exec=" << shPath << "\n"
                            << "Path=" << appInstallDir << "\n"
                            << "Icon=" << iconPath << "\n"
                            << "Type=Application\n"
                            << "Terminal=false\n";
                        dFile.close();

                        dFile.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner);
                    } else {
                        if (callback) callback(false);
                    }

                    write_setting(soft, version);
                    write_setting(soft + "_install", appDir.path());
                    if (callback) callback(true);

                } else {
                    if (callback) callback(false);
                }
            } else {
                if (callback) callback(false);
            }

            QFile::remove(zipPath);
            reply->deleteLater();
            dlManager->deleteLater();
        });
    });

#elif defined(Q_OS_MAC)

    get_dict_software(soft, [this, soft, callback](QJsonObject dict) {
        QString url_download = dict.value("download_macos").toString();
        QString version = get_data_from_depots(soft,"version");

        if (url_download.isEmpty()) {
            if (callback) callback(false);
            return;
        }

        QUrl url(url_download);
        QFileInfo fileInfo(url.path());
        QString fileName = fileInfo.fileName();
        QString folderName = fileInfo.baseName();

        if (fileName.isEmpty()) fileName = soft.toLower().replace(" ", "_") + ".zip";

        QString zipPath = QDir::tempPath() + "/" + fileName;

        QNetworkAccessManager *dlManager = new QNetworkAccessManager(this);
        QNetworkRequest request(url);
        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

        QNetworkReply *reply = dlManager->get(request);

        connect(reply, &QNetworkReply::finished, [this, soft, version, reply, dlManager, zipPath, folderName, callback]() {
            if (reply->error() == QNetworkReply::NoError) {
                QFile file(zipPath);

                if (file.open(QIODevice::WriteOnly)) {
                    file.write(reply->readAll());
                    file.close();

                    QString extractDir = QDir::tempPath() + "/";
                    QDir().mkpath(extractDir);

                    QProcess::execute("unzip", {"-o","-q", zipPath, "-d", extractDir});

                    extractDir = QDir::tempPath() + "/" + folderName;

                    QDir extDir(extractDir);
                    extDir.setFilter(QDir::Files | QDir::NoSymLinks);
                    extDir.setNameFilters({"*.dmg"});
                    QFileInfoList dmgFiles = extDir.entryInfoList();

                    if (!dmgFiles.isEmpty()) {
                        QString dmgPath = dmgFiles.first().absoluteFilePath();

                        QProcess::execute("xattr", {"-cr", dmgPath});

                        QProcess mountProcess;
                        mountProcess.start("hdiutil", QStringList() << "attach" << dmgPath << "-plist" << "-nobrowse");
                        mountProcess.waitForFinished();

                        QDir volumesDir("/Volumes");
                        QStringList volumesAvant = volumesDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

                        QString sourceAppPath;
                        QString volumePath;

                        for (const QString &vol : volumesAvant) {
                            QDir currentVol("/Volumes/" + vol);
                            QStringList apps = currentVol.entryList(QStringList() << "*.app", QDir::Dirs);
                            if (!apps.isEmpty()) {
                                volumePath = "/Volumes/" + vol;
                                sourceAppPath = volumePath + "/" + apps.first();
                                break;
                            }
                        }

                        if (sourceAppPath.isEmpty()) {
                            if (callback) callback(false);
                            return;
                        }

                        QString destDir = "/Applications";

                        QString script = QString("do shell script \"cp -R '%1' '%2'\" with administrator privileges")
                                             .arg(sourceAppPath, destDir);

                        QProcess copyProcess;
                        copyProcess.start("cp", QStringList() << "-R" << sourceAppPath << destDir);
                        copyProcess.waitForFinished();

                        QString appName = QFileInfo(sourceAppPath).fileName();
                        QString cheminFinal = destDir + "/" + appName;

                        if (copyProcess.exitCode() == 0) {
                            QProcess::execute("hdiutil", QStringList() << "detach" << volumePath << "-quiet");

                            if (QFile::exists(zipPath)) {
                                QFile::remove(zipPath);
                            }

                            QDir dirToClean(extractDir);
                            if (dirToClean.exists()) {
                                dirToClean.removeRecursively();
                            }

                            write_setting(soft, version);
                            write_setting(soft + "_install", cheminFinal);

                            if (callback) callback(true);
                        } else {
                            if (callback) callback(false);
                        }

                    } else {
                        if (callback) callback(false);
                    }
                } else {
                    if (callback) callback(false);
                }
            } else {
                if (callback) callback(false);
            }
            reply->deleteLater();
            dlManager->deleteLater();
        });
    });

#elif defined(Q_OS_WIN)

    get_dict_software(soft, [this, soft, callback](QJsonObject dict) {
        if (dict.isEmpty()) {
            if (callback) callback(false);
            return;
        }

        QString url_download = dict.value("download_windows").toString();
        QString version = get_data_from_depots(soft,"version");
        if (url_download.isEmpty() || version.isEmpty()) {
            if (callback) callback(false);
            return; // Ajout d'un return essentiel
        }

        QUrl url(url_download);
        QFileInfo fileInfo(url.path());

        QString fileName = fileInfo.fileName();

        if (fileName.isEmpty()) fileName = soft.toLower().replace(" ", "_") + ".zip";

        QString zipPath = QDir::tempPath() + "/" + fileName;
        QString folderName = fileInfo.baseName();

        QString applicationsLocation = QDir(
                                           QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
                                           ).absoluteFilePath("Programs")+"/Application/";

        QNetworkAccessManager *dlManager = new QNetworkAccessManager(this);
        QNetworkRequest request(url);
        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

        QNetworkReply *reply = dlManager->get(request);

        connect(reply, &QNetworkReply::finished, [this, soft, version, reply, dlManager, zipPath, folderName, fileName, applicationsLocation, callback](){
            if (reply->error() == QNetworkReply::NoError){
                QFile file(zipPath);

                if (file.open(QIODevice::WriteOnly)) {
                    file.write(reply->readAll());
                    file.close();

                    QString targetPath = QDir(applicationsLocation).filePath(folderName);
                    QDir().mkpath(applicationsLocation);

                    QProcess process;
                    QStringList arguments;
                    arguments << "-xf" << QDir::toNativeSeparators(zipPath) << "-C" << QDir::toNativeSeparators(applicationsLocation);

                    process.start("tar", arguments);
                    process.waitForFinished();

                    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
                        if (callback) callback(false);
                        return;
                    }

                    QString exePath;
                    QDirIterator it(targetPath, QStringList() << "*.exe", QDir::Files, QDirIterator::Subdirectories);

                    while (it.hasNext()) {
                        QString currentFile = it.next();
                        exePath = currentFile;
                        // On s'arrête si le .exe a le même nom que le dossier, sinon on garde le dernier trouvé
                        if (QFileInfo(currentFile).baseName() == folderName) {
                            break;
                        }
                    }

                    if (exePath.isEmpty()) {
                        if (callback) callback(false);
                        return;
                    }

                    QString startMenuPath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
                    QString shortcutPath = QDir(startMenuPath).filePath(soft + ".lnk");

                    if (QFile::exists(shortcutPath)) {
                        QFile::remove(shortcutPath);
                    }

                    bool shortcutCreated = QFile::link(exePath, shortcutPath);

                    if (!shortcutCreated) {
                        if (callback) callback(false);
                        return;
                    }

                    write_setting(soft, version);
                    write_setting(soft + "_install", targetPath);

                    if (callback) callback(true);

                } else {
                    if (callback) callback(false);
                }
            } else {
                if (callback) callback(false);
            }
            reply->deleteLater();
            dlManager->deleteLater();
        });
    });
#endif
}

QString Hub::get_data_from_depots(QString soft, QString data){
    QFile file(config_folder + "/depots.json");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        cout << "fichier non ouvert " << depots_file.toStdString() << endl;
        return "error";
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &error);
    if (error.error != QJsonParseError::NoError) {
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
                        if (obj.value("name").toString().compare(soft, Qt::CaseInsensitive) == 0) {

                            if (obj.contains(data)) {
                                return obj.value(data).toString();
                            } else {
                                return "error";
                            }

                        }
                    }
                }
            }
        }
    }

    return "error";
}
