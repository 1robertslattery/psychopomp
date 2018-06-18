#include <QtNetwork/QtNetwork>
#include <QByteArray>
#include <QPixmap>
#include "socket.h"

const int Socket::transferTimeout = 30 * 1000;
const int Socket::pongTimeout = 60 * 1000;
const int Socket::pingInterval = 1000;
const char Socket::separatorToken = ' ';

Socket::Socket(QObject *parent) : QTcpSocket(parent)
{
    greetingMessage = tr("undefined");
    username = tr("unknown_connected");
    state = Waiting;
    currentDataType = Undefined;
    numBytesForCurrentDataType = -1;
    transferTimerId = 0;
    isGreetingMessageSent = false;
    pingTimer.setInterval(pingInterval);

    QObject::connect(this, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
    QObject::connect(this, SIGNAL(disconnected()), &pingTimer, SLOT(stop()));
    QObject::connect(&pingTimer, SIGNAL(timeout()), this, SLOT(sendPing()));
    QObject::connect(this, SIGNAL(connected()), this, SLOT(sendGreetingMessage())); // updating the name of user in list widget and text edit
}

void Socket::getName(const QString& name)
{
    username = name;
}

void Socket::setGreetingMessage(const QString &message)
{
    greetingMessage = message;
}

void Socket::writeMessage(const QImage& icon, const QString& from, const QString &message)
{
    if (message.isEmpty() || from.isEmpty() || icon.isNull())
        return;

    // could also save image here

    QByteArray name = from.toUtf8();
    QByteArray msg = message.toUtf8();

    QFile newDoc(QCoreApplication::applicationDirPath() + "/user.txt");

    if (newDoc.open(QIODevice::WriteOnly | QIODevice::Text))
        newDoc.write(name);

    write("MESSAGE " + QByteArray::number(msg.size()) + ' ' + msg);
    waitForBytesWritten();
    flush();
    newDoc.close();
}

void Socket::writeAnimText(const QString &msg)
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

void Socket::processReadyRead()
{
    if (state == Waiting)
    {
        if (!readProtocolHeader()) return;

        if (currentDataType != Greeting)
        {
            abort();
            return;
        }

        state = Reading;
    }

    if (state == Reading)
    {
        if (!hasEnoughData()) return;

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
            if (!readProtocolHeader())
                return;
        }

        if (!hasEnoughData())
            return;

        processData();

    } while (bytesAvailable() > 0);
}

void Socket::sendPing()
{
    if (pongTime.elapsed() > pongTimeout)
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

int Socket::readDataIntoBuffer(int maxSize)
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

        if (buffer.endsWith(separatorToken))
            break;
    }

    return buffer.size() - numBytesBeforeRead;
}

// Clears buffer of username before we write our message to it
int Socket::dataLengthForCurrentDataType()
{
    if (bytesAvailable() <= 0 || readDataIntoBuffer() <= 0 || !buffer.endsWith(separatorToken))
        return 0;

    buffer.chop(1);
    int number = buffer.toInt();
    buffer.clear();
    return number;
}

bool Socket::readProtocolHeader()
{
    if (transferTimerId)
    {
        killTimer(transferTimerId);
        transferTimerId = 0;
    }

    if (readDataIntoBuffer() <= 0)
    {
        transferTimerId = startTimer(transferTimeout);
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
    numBytesForCurrentDataType = dataLengthForCurrentDataType();
    return true;
}

bool Socket::hasEnoughData()
{
    if (transferTimerId)
    {
        QObject::killTimer(transferTimerId);
        transferTimerId = 0;
    }

    if (numBytesForCurrentDataType <= 0)
        numBytesForCurrentDataType = dataLengthForCurrentDataType();

    if (bytesAvailable() < numBytesForCurrentDataType || numBytesForCurrentDataType <= 0)
    {
        transferTimerId = startTimer(transferTimeout);
        return false;
    }

    return true;
}

// Where we insert image and text
void Socket::processData()
{
    // Reading QByteArray data
    buffer = read(numBytesForCurrentDataType);

    if (buffer.size() != numBytesForCurrentDataType)
    {
        abort();
        return;
    }

    // Open anim text. TODO: check if files exist.
    QFile animFile(QCoreApplication::applicationDirPath() + "/anim.txt");
    animFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = animFile.readAll();
    QString animType = QString::fromUtf8(data);

    if (animType.isEmpty())
        return;

    // Open username text
    QFile userFile(QCoreApplication::applicationDirPath() + "/user.txt");
    userFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray inputData = userFile.readAll();
    QString name = QString::fromUtf8(inputData);

    if (name.isEmpty())
        return;

    // Open user account picture
    QImage* imageObject = new QImage();
    imageObject->load(QCoreApplication::applicationDirPath() + "/image.png");

    QPixmap image = QPixmap::fromImage(*imageObject, Qt::AutoColor);
    picture = image.toImage();

    if (picture.isNull())
        return;

    // Execute typing detection animation
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

    // Reset data & variables
    currentDataType = Undefined;
    numBytesForCurrentDataType = 0;
    buffer.clear();
    userFile.close();
    animFile.close();
}
