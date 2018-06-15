#ifndef MANAGER_H
#define MANAGER_H

#include <QByteArray>
#include <QList>
#include <QObject>
#include <QTimer>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QUdpSocket>

class Client;
class Socket;

class Manager : public QObject
{

    Q_OBJECT

public:

    Manager(Client* client);

    void SetServerPort(int port);
    QByteArray GetUserName() const;
    QByteArray username;
    void StartBroadcasting();
    bool IsLocalHostAddress(const QHostAddress &address);

    Client *client;

    void beginConnections();

signals:

    void newConnection(Socket *connection);

private slots:

    void sendBroadcastDatagram();
    void readBroadcastDatagram();

private:

    void UpdateAddresses();

    QList<QHostAddress> broadcastAddresses;
    QList<QHostAddress> ipAddresses;
    QUdpSocket broadcastSocket;
    QTimer broadcastTimer;
    int serverPort;

};

#endif
