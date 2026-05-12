#include "socket/csocketclient.h"

CArreraClient::CArreraClient(const QString &pnameSoft, QObject *parent)
    : QObject{parent}, m_nameSoft{pnameSoft}
{
    // Connexions utilisant la syntaxe moderne (pointeurs de fonction)
    connect(&m_socketClient, &QWebSocket::connected, this, &CArreraClient::onConnected);
    connect(&m_socketClient, &QWebSocket::disconnected, this, &CArreraClient::onDisconnected);
    connect(&m_socketClient, &QWebSocket::textMessageReceived, this, &CArreraClient::onMessageReceived);
    
    // Correction de la syntaxe d'erreur pour les versions récentes de Qt
    connect(&m_socketClient, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
            this, &CArreraClient::onError);
}

CArreraClient::~CArreraClient()
{
    if (isServerConnected()) {
        m_socketClient.close();
    }
}

void CArreraClient::connectToServeur(const QString &url)
{
    m_socketClient.open(QUrl(url));
}

void CArreraClient::disconnectFromServer()
{
    m_socketClient.close();
}

bool CArreraClient::sendMessage(const QString &message)
{
    if (!isServerConnected()) {
        return false;
    }
    m_socketClient.sendTextMessage(message);
    return true;
}

void CArreraClient::onConnected()
{
    m_socketClient.sendTextMessage(QString("namesoft %1\n").arg(m_nameSoft));
    emit connectionEstablished();
}

void CArreraClient::onDisconnected()
{
    emit connectionClosed();
}

void CArreraClient::onMessageReceived(const QString &message)
{
    emit messageReceived(message);

    if (message != "Message Received" && !message.isEmpty()) {
        m_socketClient.sendTextMessage("Message Received");
    }
}

void CArreraClient::onError(QAbstractSocket::SocketError error)
{
    QString errorInfo = m_socketClient.errorString();
    emit errorOccurred(errorInfo);
}