#ifndef CLIENT_H
#define CLIENT_H

#include <QtNetwork/QAbstractSocket>
#include <QHash>
#include <QtNetwork/QHostAddress>
#include <QImage>
#include "server.h"

class Manager;

class Client : public QObject
{
    Q_OBJECT

public:

    Client();

    ~Client();

    void SendText(const QImage& icon, const QString& from, const QString &message);
    void SendUsername(const QString& name);
    QString GetNickName() const;
    bool HasConnection(const QHostAddress &senderIp, int senderPort = -1) const;
    Manager* manager;
    QImage picture;
    void EnableAnimation(const QString &msg);
    void DisableAnimation(const QString &msg);
    void newLogin();

signals:

    void newMessage(const QImage& icon, const QString &from, const QString &message);
    void newParticipant(const QString &nick);
    void participantLeft(const QString &nick);
    void startAnimation(const QString &msg);
    void endAnimation(const QString &msg);
    void playSound();

private slots:

    void newConnection(Socket* socket);

    void connectionError(QAbstractSocket::SocketError socketError);
    void disconnected();
    void readyForUse();

private:

    void RemoveConnection(Socket* socket);

    bool hasLeft;
    Server server;
    QMultiHash<QHostAddress, Socket*> peers;
};

#endif
