#ifndef CTIGERDEMON_H
#define CTIGERDEMON_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonArray>
#include <QEventLoop>
#include <QTimer>
#include <QUrl>
// Debug
/*
#include <iostream>
using namespace std;
*/


class CTigerDemon : public QObject
{
    Q_OBJECT
public:
    explicit CTigerDemon(const QString name,QString version, QObject* parent = nullptr);
    void checkUpdate();
    QString get_version();
private:
    QNetworkAccessManager* manager;
    QJsonObject contenuJSON;
    QString offline_version;
    QString online_version,name_soft;
    QString url = "https://raw.githubusercontent.com/Arrera-Software/distribution/refs/heads/main/index.json";
private: // Methode
    bool sate_connection();
    int set_online_version();

signals:
    void updateResult(bool hasUpdate, const QString& newVersion);
    void updateError(int errorCode);

};

#endif // CTIGERDEMON_H
