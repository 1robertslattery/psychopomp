#include <QtNetwork/QtNetwork>
#include <QDataStream>
#include <QBuffer>
#include <QImageWriter>
#include <QImageReader>
#include <QByteArray>
#include <QDebug>
#include "socket.h"
#include "manager.h"
#include "client.h"

Client::Client()
{
    manager = new Manager(this);
    manager->SetServerPort(server.serverPort());
    //manager->StartBroadcasting();

    //QObject::connect(manager, SIGNAL(newLogin(Socket*)), this, SLOT(newLogin(Socket*)));
    //QObject::connect(manager, SIGNAL(newConnection(Socket*)), this, SLOT(newConnection(Socket*)));
    //QObject::connect(&server, SIGNAL(newConnection(Socket*)), this, SLOT(newConnection(Socket*)));
}

Client::~Client()
{
    delete manager;
}

void Client::SendText(const QImage& icon, const QString& from, const QString &message)
{
    if (message.isEmpty() || from.isEmpty() || icon.isNull()) return;

    QList<Socket*> connections = peers.values();

    foreach (Socket* connection, connections)
        connection->IsSendingText(icon, from, message);
}

void Client::SendUsername(const QString& name)
{
    if (name.isEmpty()) return;

    //QList<Socket*> connections = peers.values();

    //foreach (Socket* connection, connections)
    //    connection->WriteUsername(name);

    // And send the username to the manager where it is written to QByteArray
    QByteArray val = name.toUtf8();
    manager->username = val;

    newLogin();
}

QString Client::GetNickName() const
{
    return QString(manager->GetUserName());
            //+ '@' + QHostInfo::localHostName()
           //+ ':' + QString::number(server.serverPort());
}

bool Client::HasConnection(const QHostAddress &senderIp, int senderPort) const
{
    if (senderPort == -1)
        return peers.contains(senderIp);

    if (!peers.contains(senderIp))
        return false;

    QList<Socket*> connections = peers.values(senderIp);

    foreach (Socket* connection, connections)
    {
        if (connection->peerPort() == senderPort)
            return true;
    }

    return false;
}

void Client::newConnection(Socket *socket)
{
    socket->SetGreetingMessage(manager->GetUserName());
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connectionError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyForUse()), this, SLOT(readyForUse())); // wait until login?
}

void Client::newLogin()
{
    manager->StartBroadcasting();
    manager->beginConnections();
    QObject::connect(manager, SIGNAL(newConnection(Socket*)), this, SLOT(newConnection(Socket*)));
    QObject::connect(&server, SIGNAL(newConnection(Socket*)), this, SLOT(newConnection(Socket*)));
}

void Client::readyForUse() //required
{
    Socket *socket = qobject_cast<Socket*>(sender());

    if (!socket || HasConnection(socket->peerAddress(), socket->peerPort()))
        return;

    // Messaging signals
    connect(socket, SIGNAL(newMessage(QImage,QString,QString)), this, SIGNAL(newMessage(QImage,QString,QString)));

    // Animation signals
    connect(socket, SIGNAL(startAnimation(QString)), this, SIGNAL(startAnimation(QString)));
    connect(socket, SIGNAL(endAnimation(QString)), this, SIGNAL(endAnimation(QString)));

    // Sound signal
    connect(socket, SIGNAL(playSound()), this, SIGNAL(playSound()));

    if (!peers.contains(socket->peerAddress()))
        peers.insert(socket->peerAddress(), socket);

    // PROBLEMS.  Control with bool?
    QString nick = socket->username;

    if (!nick.isEmpty())
        emit newParticipant(nick); // is this a problem?
}

void Client::disconnected()
{
    if (Socket* socket = qobject_cast<Socket*>(sender()))
        RemoveConnection(socket);
}

void Client::connectionError(QAbstractSocket::SocketError /* socketError */)
{
    if (Socket* socket = qobject_cast<Socket*>(sender()))
        RemoveConnection(socket);
}

void Client::RemoveConnection(Socket *socket) // PROBLEMS
{
    if (peers.contains(socket->peerAddress()))
    {
        peers.remove(socket->peerAddress());

        QString nick = socket->username;

        if (!nick.isEmpty())
            emit participantLeft(nick);
    }

    socket->deleteLater();
}

void Client::EnableAnimation(const QString &msg)
{
    if (msg.isEmpty()) return;

    QList<Socket*> connections = peers.values();

    foreach (Socket* connection, connections)
        connection->WriteEnableText(msg);
}

void Client::DisableAnimation(const QString &msg)
{
    if (msg.isEmpty()) return;

    QList<Socket*> connections = peers.values();

    foreach (Socket* connection, connections)
        connection->WriteEnableText(msg);
}
