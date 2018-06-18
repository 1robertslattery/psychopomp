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

    // Client pointer
    Client *client;

    void setServerPort(int port);
    QByteArray getUserName() const;
    QByteArray username;
    void startBroadcasting();
    bool isLocalHostAddress(const QHostAddress &address);

    void beginConnections();

signals:

    void newConnection(Socket *connection);

private slots:

    void sendBroadcastDatagram();
    void readBroadcastDatagram();

private:

    void updateAddresses();

    QList<QHostAddress> broadcastAddresses;
    QList<QHostAddress> ipAddresses;
    QUdpSocket broadcastSocket;
    QTimer broadcastTimer;
    int serverPort;

};

#endif
