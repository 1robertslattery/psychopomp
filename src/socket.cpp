#include <QtNetwork/QtNetwork>
#include <QByteArray>
#include <QBuffer>
#include <QImage>
#include <QPixmap>
#include <QImageWriter>
#include <QImageReader>
#include "socket.h"

static const int TransferTimeout = 30 * 1000;
static const int PongTimeout = 60 * 1000;
static const int PingInterval = 1000;
static const int AnimInterval = 100;
static const char SeparatorToken = ' ';

Socket::Socket(QObject *parent) : QTcpSocket(parent)
{
    greetingMessage = tr("undefined"); // updating the name of user in list widget and text edit
    username = tr("unknown_connected");
    state = WaitingForGreeting;
    currentDataType = Undefined;
    numBytesForCurrentDataType = -1;
    transferTimerId = 0;
    isGreetingMessageSent = false;
    pingTimer.setInterval(PingInterval);
    animTimer.setInterval(AnimInterval);

    QObject::connect(this, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
    QObject::connect(this, SIGNAL(disconnected()), &pingTimer, SLOT(stop()));
    QObject::connect(&pingTimer, SIGNAL(timeout()), this, SLOT(sendPing()));
    QObject::connect(this, SIGNAL(connected()), this, SLOT(sendGreetingMessage())); // updating the name of user in list widget and text edit
}

void Socket::GetName(const QString& name)
{
    username = name;
}

void Socket::SetGreetingMessage(const QString &message)
{
    greetingMessage = message;
}

// Can't get images to work at all. Maybe try to send file directly into folder directory?
void Socket::IsSendingText(const QImage& icon, const QString& from, const QString &message)
{
    if (message.isEmpty() || from.isEmpty() || icon.isNull())
        return;

    //picture = icon; // could also save here

    QByteArray name = from.toUtf8();
    QByteArray msg = message.toUtf8();

    QFile newDoc(QCoreApplication::applicationDirPath() + "/user.txt");

    if (newDoc.open(QIODevice::WriteOnly | QIODevice::Text))
        newDoc.write(name);

    //write("MESSAGE " + QByteArray::number(name.size()) + name + "/n" + QByteArray::number(msg.size()) + ' ' + msg);
    //write("MESSAGE " + QByteArray::number(img.size()) + img + "/n");
    //write("MESSAGE " + QByteArray::number(name.size()) + name + "/n");
    write("MESSAGE " + QByteArray::number(msg.size()) + ' ' + msg);
    waitForBytesWritten();
    flush();
    newDoc.close();
}

//void Socket::WriteUsername(const QString &name)
//{
//    if (name.isEmpty())
//        return;

    //QByteArray text = name.toUtf8();
    //QFile newDoc(QCoreApplication::applicationDirPath() + "/user.txt");

    //if (newDoc.open(QIODevice::WriteOnly | QIODevice::Text))
    //    newDoc.write(text);

    //newDoc.close();
//}

void Socket::WriteEnableText(const QString &msg)
{
    if (msg.isEmpty())
        return;

    QByteArray text = msg.toUtf8();
    QFile newDoc(QCoreApplication::applicationDirPath() + "/anim.txt");

    if (newDoc.open(QIODevice::WriteOnly | QIODevice::Text))
        newDoc.write(text);

    newDoc.close();
}

void Socket::timerEvent(QTimerEvent *timerEvent)
{
    if (timerEvent->timerId() == transferTimerId)
    {
        abort();
        killTimer(transferTimerId);
        transferTimerId = 0;
    }
}

// on readReady()
void Socket::processReadyRead()
{
    if (state == WaitingForGreeting)
    {
        if (!ReadProtocolHeader()) return;

        if (currentDataType != Greeting)
        {
            abort();
            return;
        }

        state = ReadingGreeting;
    }

    if (state == ReadingGreeting)
    {
        if (!HasEnoughData()) return;

        buffer = read(numBytesForCurrentDataType);

        if (buffer.size() != numBytesForCurrentDataType)
        {
            abort();
            return;
        }

        username = QString(buffer);

        // use this if you want username, ip address, and port number
        // username = QString(buffer) + '@' + peerAddress().toString() + ':' + QString::number(peerPort());

        currentDataType = Undefined;
        numBytesForCurrentDataType = 0;
        buffer.clear();

        if (!isValid())
        {
            abort();
            return;
        }

        if (!isGreetingMessageSent)
            sendGreetingMessage();

        pingTimer.start();
        pongTime.start();
        state = ReadyForUse;
        emit readyForUse();
    }

    do
    {
        if (currentDataType == Undefined)
        {
            if (!ReadProtocolHeader())
                return;
        }

        if (!HasEnoughData())
            return;

        ProcessData();

    } while (bytesAvailable() > 0);
}

void Socket::sendPing()
{
    if (pongTime.elapsed() > PongTimeout)
    {
        abort();
        return;
    }

    write("PING 1 p");
}

// updating the name of user in list widget and text edit
void Socket::sendGreetingMessage()
{
    QByteArray greeting = greetingMessage.toUtf8();
    write("GREETING " + QByteArray::number(greeting.size()) + ' ' + greeting);
    isGreetingMessageSent = true;
}

int Socket::ReadDataIntoBuffer(int maxSize)
{
    if (maxSize > MaxBufferSize)
        return 0;

    int numBytesBeforeRead = buffer.size();

    if (numBytesBeforeRead == MaxBufferSize)
    {
        abort();
        return 0;
    }

    while (bytesAvailable() > 0 && buffer.size() < maxSize)
    {
        buffer.append(read(1));

        if (buffer.endsWith(SeparatorToken))
            break;
    }

    return buffer.size() - numBytesBeforeRead;
}

// Clears buffer of username before we write our message to it
int Socket::DataLengthForCurrentDataType()
{
    if (bytesAvailable() <= 0 || ReadDataIntoBuffer() <= 0 || !buffer.endsWith(SeparatorToken))
        return 0;

    buffer.chop(1);
    int number = buffer.toInt();
    buffer.clear();
    return number;
}

bool Socket::ReadProtocolHeader()
{
    if (transferTimerId)
    {
        killTimer(transferTimerId);
        transferTimerId = 0;
    }

    if (ReadDataIntoBuffer() <= 0)
    {
        transferTimerId = startTimer(TransferTimeout);
        return false;
    }

    if (buffer == "PING ")
    {
        currentDataType = Ping;
    }
    else if (buffer == "PONG ")
    {
        currentDataType = Pong;
    }
    else if (buffer == "MESSAGE ")
    {
        currentDataType = PlainText;
    }
    else if (buffer == "GREETING ")
    {
        currentDataType = Greeting;
    }
    else
    {
        currentDataType = Undefined;
        abort();
        return false;
    }

    buffer.clear();
    numBytesForCurrentDataType = DataLengthForCurrentDataType();
    return true;
}

bool Socket::HasEnoughData()
{
    if (transferTimerId)
    {
        QObject::killTimer(transferTimerId);
        transferTimerId = 0;
    }

    if (numBytesForCurrentDataType <= 0)
        numBytesForCurrentDataType = DataLengthForCurrentDataType();

    if (bytesAvailable() < numBytesForCurrentDataType || numBytesForCurrentDataType <= 0)
    {
        transferTimerId = startTimer(TransferTimeout);
        return false;
    }

    return true;
}

// Where we insert image and text
void Socket::ProcessData()
{
    buffer = read(numBytesForCurrentDataType);

    if (buffer.size() != numBytesForCurrentDataType)
    {
        abort();
        return;
    }

    QFile animFile(QCoreApplication::applicationDirPath() + "/anim.txt");
    animFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = animFile.readAll();
    QString animType = QString::fromUtf8(data);

    if (animType.isEmpty()) return;

    QFile userFile(QCoreApplication::applicationDirPath() + "/user.txt");
    userFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray inputData = userFile.readAll();
    QString name = QString::fromUtf8(inputData);

    if (name.isEmpty()) return;

    QImage* imageObject = new QImage();
    imageObject->load(QCoreApplication::applicationDirPath() + "/image.png");

    QPixmap image = QPixmap::fromImage(*imageObject, Qt::AutoColor);
    picture = image.toImage();

    if (picture.isNull())
        return;

    if (animType.contains("enable"))
        emit startAnimation(animType);
    else if (animType.contains("disable"))
        emit endAnimation(animType);

    switch (currentDataType)
    {
        case PlainText:
            emit newMessage(picture, name, QString::fromUtf8(buffer));
            emit playSound();
            break;
        case Ping:
            write("PONG 1 p");
            break;
        case Pong:
            pongTime.restart();
            break;
        default:
            break;
    }

    currentDataType = Undefined;
    numBytesForCurrentDataType = 0;
    buffer.clear();
    userFile.close();
    animFile.close();
}
