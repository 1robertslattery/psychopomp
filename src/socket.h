#ifndef SOCKET_H
#define SOCKET_H

#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpSocket>
#include <QTime>
#include <QTimer>
#include <QImage>

static const int MaxBufferSize = 1024000;

class Socket : public QTcpSocket
{
    Q_OBJECT

public:

    enum ConnectionState
    {
        Waiting,
        Reading,
        ReadyForUse
    };

    enum DataType
    {
        PlainText,
        Ping,
        Pong,
        Greeting,
        Undefined
    };

    Socket(QObject *parent = 0);

    void getName(const QString& name);
    QString username;
    QImage picture;
    void setGreetingMessage(const QString &message);
    void writeMessage(const QImage& icon, const QString& from, const QString &message);
    void writeAnimText(const QString &msg);

signals:

    void readyForUse();
    void newMessage(const QImage& icon, const QString &from, const QString &message);
    void startAnimation(const QString &msg);
    void endAnimation(const QString &msg);
    void playSound();

protected:

    void timerEvent(QTimerEvent *timerEvent) override;

private slots:

    void processReadyRead();
    void sendPing();
    void sendGreetingMessage();

private:

    int readDataIntoBuffer(int maxSize = MaxBufferSize);
    int dataLengthForCurrentDataType();
    bool readProtocolHeader();
    bool hasEnoughData();
    void processData();

    QString greetingMessage;
    QTimer pingTimer;
    QTime pongTime;
    QByteArray buffer;
    QByteArray nameBuffer;
    QByteArray iconBuffer;
    ConnectionState state;
    DataType currentDataType;
    int numBytesForCurrentDataType;
    int transferTimerId;
    bool isGreetingMessageSent;

    static const int transferTimeout;
    static const int pongTimeout;
    static const int pingInterval;
    static const char separatorToken;

};

#endif
