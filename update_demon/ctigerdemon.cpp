#include "ctigerdemon.h"

CTigerDemon::CTigerDemon(QString name,QString version, QObject* parent)
    : QObject(parent)
{
    name_soft = name;
    offline_version = version;
}

QString CTigerDemon::get_version(){
    return offline_version;
}

void CTigerDemon::checkUpdate() {
    int status = set_online_version();

    //cout << status << endl;
    //cout << "Version local : " << offline_version.toStdString() << "\nVersion en ligne : " << online_version.toStdString() << endl;

    if (status != 1) emit updateError(status);

    if (offline_version == "dev") emit updateResult(false,online_version);
    else if (online_version != offline_version && !online_version.isEmpty()){
        emit updateResult(true, online_version);
    } else emit updateResult(false, online_version);
}

bool CTigerDemon::sate_connection(){
    QNetworkAccessManager manager;
    QNetworkRequest request{QUrl(url)};
    QNetworkReply *reply = manager.head(request);
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(2000);
    loop.exec();
    bool isConnected = (reply->error() == QNetworkReply::NoError);
    reply->deleteLater();
    return isConnected;
}

int CTigerDemon::set_online_version(){
    /*
     *  1 : OK (Connecté, JSON lu, logiciel trouvé et version récupérée)
     * -1 : Pas internet
     * -2 : Autre (Erreur réseau, JSON invalide, ou logiciel introuvable)
    */

    if (!sate_connection()) {
        return -1;
    }

    QNetworkAccessManager mgr(this);
    QNetworkRequest req(url);
    QNetworkReply* reply = mgr.get(req);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError) {
        reply->deleteLater();
        return -2;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        return -2;
    }

    QJsonObject root = doc.object();
    bool softwareFound = false;

    QStringList categories = {"application", "assistants"};

    for (const QString& category : categories) {
        if (root.contains(category) && root[category].isArray()) {
            QJsonArray array = root[category].toArray();

            for (int i = 0; i < array.size(); ++i) {
                QJsonObject item = array[i].toObject();

                if (item["name"].toString() == name_soft) {
                    online_version = item["version"].toString();
                    softwareFound = true;
                    break;
                }
            }
        }

        if (softwareFound) {
            break;
        }
    }

    if (softwareFound) {
        return 1;
    } else {
        return -2;
    }
}