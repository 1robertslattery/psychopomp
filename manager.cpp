#include "manager.h"
#include <QtNetwork/QtNetwork>
#include <QtNetwork/QNetworkInterface>
#include <QByteArray>
#include <QDebug>
#include "client.h"
#include "socket.h"

static const qint32 BroadcastInterval = 2000;
static const unsigned int broadcastPort = 45000;

Manager::Manager(Client* client) : QObject(client)
{
    this->client = client;
    QStringList envVariables;
    envVariables << "USERNAME" << "USER" << "USERDOMAIN" << "HOSTNAME" << "DOMAINNAME";
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();

    foreach (QString string, envVariables)
    {
        if (environment.contains(string))
        {
            username = environment.value(string).toUtf8();
            break;
        }
    }

    if (username.isEmpty()) username = "unknown_man";

    UpdateAddresses();
    serverPort = 0;

    broadcastSocket.bind(QHostAddress::AnyIPv4, broadcastPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);

    broadcastTimer.setInterval(BroadcastInterval);
    connect(&broadcastTimer, SIGNAL(timeout()), this, SLOT(sendBroadcastDatagram()));
}

void Manager::beginConnections()
{
    connect(&broadcastSocket, SIGNAL(readyRead()), this, SLOT(readBroadcastDatagram()));
}

void Manager::SetServerPort(int port)
{
    serverPort = port;
}

QByteArray Manager::GetUserName() const
{
   return username;
}

void Manager::StartBroadcasting()
{
    broadcastTimer.start();
}

bool Manager::IsLocalHostAddress(const QHostAddress &address)
{
    foreach (QHostAddress localAddress, ipAddresses)
    {
        if (address == localAddress)
            return true;
    }

    return false;
}

void Manager::sendBroadcastDatagram()
{
    QByteArray datagram(username);
    datagram.append('@');
    datagram.append(QByteArray::number(serverPort));

    bool validBroadcastAddresses = true;

    foreach (QHostAddress address, broadcastAddresses)
    {
        if (broadcastSocket.writeDatagram(datagram, address, broadcastPort) == -1)
            validBroadcastAddresses = false;
    }

    if (!validBroadcastAddresses)
        UpdateAddresses();
}

void Manager::readBroadcastDatagram()
{
    while (broadcastSocket.hasPendingDatagrams())
    {
        QHostAddress senderIp;
        quint16 senderPort;
        QByteArray datagram;
        datagram.resize(broadcastSocket.pendingDatagramSize());

        if (broadcastSocket.readDatagram(datagram.data(), datagram.size(), &senderIp, &senderPort) == -1)
            continue;

        QList<QByteArray> list = datagram.split('@');

        if (list.size() != 2)
            continue;

        int senderServerPort = list.at(1).toInt();

        if (IsLocalHostAddress(senderIp) && senderServerPort == serverPort)
            continue;

        if (!client->HasConnection(senderIp))
        {
            Socket* connection = new Socket(this);
            emit newConnection(connection);
            connection->connectToHost(senderIp, senderServerPort);
        }
    }
}

void Manager::UpdateAddresses()
{
    broadcastAddresses.clear();
    ipAddresses.clear();

    foreach (QNetworkInterface interface, QNetworkInterface::allInterfaces())
    {
        foreach (QNetworkAddressEntry entry, interface.addressEntries())
        {
            QHostAddress broadcastAddress = entry.broadcast();

            if (broadcastAddress != QHostAddress::Null && entry.ip() != QHostAddress::LocalHost)
            {
                broadcastAddresses << broadcastAddress;
                ipAddresses << entry.ip();
            }
        }
    }
}
