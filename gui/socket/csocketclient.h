#ifndef CARRERACLIENT_H
#define CARRERACLIENT_H

#include <QObject>
#include <QWebSocket>
#include <QAbstractSocket>
#include <QUrl>

class CArreraClient : public QObject
{
    Q_OBJECT
private:
    QWebSocket m_socketClient;
    QString m_nameSoft;

public:
    explicit CArreraClient(const QString &pnameSoft = "", QObject *parent = nullptr);
    ~CArreraClient();

    void connectToServeur(const QString &url);
    bool sendMessage(const QString &message);
    void disconnectFromServer();

    bool isServerConnected() const { 
        return m_socketClient.state() == QAbstractSocket::ConnectedState; 
    }

signals:
    void messageReceived(const QString &message);
    void connectionEstablished();
    void connectionClosed();
    void errorOccurred(const QString &errorString);

private slots:
    void onConnected();
    void onDisconnected();
    void onMessageReceived(const QString &message); 
    void onError(QAbstractSocket::SocketError error);
};

#endif // CARRERACLIENT_H