#ifndef SERVER_H
#define SERVER_H

#include <QtNetwork/QTcpServer>

class Socket;

class Server : public QTcpServer
{
    Q_OBJECT

public:

    Server(QObject *parent = 0);

signals:

    void newConnection(Socket *connection);

protected:

    virtual void incomingConnection(qintptr socketDescriptor) override;

};

#endif
