#ifndef SOCKET_H
#define SOCKET_H

#include <QtNetwork/QHostAddress>
#include <QString>
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
        WaitingForGreeting,
        ReadingGreeting,
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

    void GetName(const QString& name);
    QString username;
    QImage picture;
    QString anim;
    void SetGreetingMessage(const QString &message);
    void IsSendingText(const QImage& icon, const QString& from, const QString &message);
    //void WriteUsername(const QString &name);
    void WriteEnableText(const QString &msg);

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

    int ReadDataIntoBuffer(int maxSize = MaxBufferSize);
    int DataLengthForCurrentDataType();
    bool ReadProtocolHeader();
    bool HasEnoughData();
    void ProcessData();

    QString greetingMessage;
    QTimer pingTimer;
    QTime pongTime;
    QTimer animTimer;
    QByteArray buffer;
    QByteArray nameBuffer;
    QByteArray iconBuffer;
    ConnectionState state;
    DataType currentDataType;
    int numBytesForCurrentDataType;
    int transferTimerId;
    bool isGreetingMessageSent;

};

#endif
