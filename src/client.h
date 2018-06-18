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

    void sendMessage(const QImage& icon, const QString& from, const QString &message);
    void sendUsername(const QString& name);
    QString getNickName() const;
    bool hasConnection(const QHostAddress &senderIp, int senderPort = -1) const;
    Manager* manager;
    QImage picture;

    // Typing detection animation
    void enableAnimation(const QString &msg);
    void disableAnimation(const QString &msg);

    // Begin UDP broadcasting once logged in
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

    void removeConnection(Socket* socket);

    bool hasLeft;
    Server server;
    QMultiHash<QHostAddress, Socket*> peers;
};

#endif
