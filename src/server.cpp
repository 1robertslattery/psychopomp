#include <QtNetwork/QtNetwork>
#include <QDebug>
#include "server.h"
#include "socket.h"

// Sets default values
Server::Server(QObject *parent) : QTcpServer(parent)
{
    // Set max number of users that can connect to server
    setMaxPendingConnections(30);

    // Listen for IP address and exit application if one is not found
    if (listen(QHostAddress::AnyIPv4))
    {
        qDebug() << "Server started!!!";
    }
}

// Called when a new connection is available
void Server::incomingConnection(qintptr socketDescriptor)
{
    Socket *connection = new Socket(this);
    connection->setSocketDescriptor(socketDescriptor);
    emit newConnection(connection);
}
