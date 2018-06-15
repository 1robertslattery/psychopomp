#include <QtNetwork/QtNetwork>
#include <QDebug>
#include "server.h"
#include "socket.h"

Server::Server(QObject *parent) : QTcpServer(parent)
{
    if (!listen(QHostAddress::AnyIPv4))
    {
        qDebug() << "Server could not start!";
    }
    else
    {
        qDebug() << "Server started!!!";
    }
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    Socket *connection = new Socket(this);
    connection->setSocketDescriptor(socketDescriptor);
    emit newConnection(connection);
}
