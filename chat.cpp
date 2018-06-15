#include <QtWidgets/QtWidgets>
#include <QtWidgets/QDialog>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QEvent>
#include <QDateTime>
#include <QLabel>
#include <QPushButton>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QPalette>
#include <QTextEdit>
#include <QPainter>
#include <QTextCharFormat>
#include <QTextFrameFormat>
#include <QTextCursor>
#include <QTime>
#include <QPixmap>
#include <QPoint>
#include <QPointF>
#include <QImage>
#include <QLocale>
#include <QBuffer>
#include <QDataStream>
#include <QTextStream>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include "chat.h"
#include "manager.h"
#include "socket.h"

Chat::Chat(QWidget *parent) : QDialog(parent)
{
    // UI Setup
    ui.setupUi(this);
    ui.centralWidget->setAttribute(Qt::WA_QuitOnClose);
    ui.centralWidget->setCurrentWidget(ui.loginpage);
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    // Set Location
    QLocale eng(QLocale::English, QLocale::UnitedStates);
    QLocale::setDefault(eng);

    // Set focus of ui elements
    ui.chatTitle->setFocusPolicy(Qt::NoFocus);
    ui.exitLogin->setFocusPolicy(Qt::NoFocus);
    ui.usernameEdit->setFocusPolicy(Qt::StrongFocus);
    ui.lineEdit->setFocusPolicy(Qt::StrongFocus);
    ui.lineEdit->installEventFilter(this); // for eventFilter
    ui.textEdit->setFocusPolicy(Qt::NoFocus);
    ui.textEdit->setReadOnly(true);
    ui.listWidget->setFocusPolicy(Qt::NoFocus);
    ui.exitButton->setFocusPolicy(Qt::NoFocus);
    ui.minimizeButton->setFocusPolicy(Qt::NoFocus);
    ui.settingsButton->setFocusPolicy(Qt::NoFocus);
    ui.saveButton->setFocusPolicy(Qt::NoFocus);

    // Set login page account picture as interactive
    ui.graphicsView->setInteractive(true);

    // Set defaults for typing detection
    fadeOutTypingDetection();

    // Set defaults for login page
    connect(ui.usernameEdit, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
    connect(ui.loginButton, SIGNAL(clicked()), this, SLOT(returnPressed()));

    // Set defaults for message box
    connect(ui.lineEdit, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
    connect(ui.lineEdit, SIGNAL(returnPressed()), this, SLOT(returnPressed())); //onEditingFinished() for typing detect?

    // Set a connection between newMessage in Client to appendMessage in Chat
    connect(&client, SIGNAL(newMessage(QImage,QString,QString)), this, SLOT(appendMessage(QImage,QString,QString)));

    // Set detection of when users enter and leave chat
    connect(&client, SIGNAL(newParticipant(QString)), this, SLOT(newParticipant(QString)));
    connect(&client, SIGNAL(participantLeft(QString)), this, SLOT(participantLeft(QString)));

    // Set buttons
    connect(ui.exitButton, SIGNAL(clicked()), this, SLOT(exit()));
    connect(ui.minimizeButton, SIGNAL(clicked()), this, SLOT(minimize()));
    connect(ui.exitLogin, SIGNAL(clicked()), this, SLOT(exit()));
    connect(ui.minimizeLogin, SIGNAL(clicked()), this, SLOT(minimize()));
    connect(ui.saveButton, SIGNAL(clicked()), this, SLOT(saveConversation()));
    connect(ui.settingsButton, SIGNAL(clicked()), this, SLOT(openSettings()));
    connect(ui.changeImageButton, SIGNAL(clicked()), this, SLOT(setNewImage()));
    connect(ui.backButton, SIGNAL(clicked()), this, SLOT(closeSettings()));

    // Set "Remember Me" radio button
    connect(ui.radioButton, SIGNAL(toggled(bool)), this, SLOT(rememberMe()));

    // Set timer for internet connection check
    tableFormat.setBorder(0);
    hasLoggedIn = false;
    pingTimer = new QTimer(this);
    QObject::connect(pingTimer, SIGNAL(timeout()), this, SLOT(checkConnection()));
    pingTimer->start(5000);

    // Set signals/slots for animating type detection
    connect(&client, SIGNAL(startAnimation(QString)), this, SLOT(enableAnimation(QString)));
    connect(&client, SIGNAL(endAnimation(QString)), this, SLOT(disableAnimation(QString)));
    animationTimer = new QTimer(this);
    QObject::connect(animationTimer, SIGNAL(timeout()), this, SLOT(fadeInTypingDetection()));
    isAnimating = false;

    // Set notification sound
    notification.setSource(QUrl::fromLocalFile(":/images/sending.wav"));
    notification.setLoopCount(0);
    notification.setVolume(0.75f);
    connect(&client, SIGNAL(playSound()), this, SLOT(playMessagingSound()));

    // Set default account picture image, unless they have been saved...
    holderObject = new QImage();

    QFile defaultsFile(QCoreApplication::applicationDirPath() + "/defaults.txt");

    if (defaultsFile.exists())
    {
        holderObject->load(QCoreApplication::applicationDirPath() + "/image.png");

        if (defaultsFile.open(QIODevice::ReadWrite | QIODevice::Text))
        {
            QByteArray data = defaultsFile.readAll();
            QString name = QString::fromUtf8(data);
            ui.usernameEdit->setText(name);
            ui.usernameEdit->text() = name;
            defaultsFile.flush();
            defaultsFile.close();
        }
    }
    else
    {
        holderObject->load(":/images/picture_holder.jpg");
        QString username = "username";
        ui.usernameEdit->setPlaceholderText(" " + username);
    }

    holderImage = QPixmap::fromImage(*holderObject, Qt::AutoColor);
    scene = new QGraphicsScene(this);
    scene->addPixmap(holderImage.scaled(156, 156, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui.graphicsView->setScene(scene);
    ui.graphicsView->show();
    ProfilePicture = holderImage.toImage();
}

Chat::~Chat()
{
    //delete holderObject;
    //delete scene;
    //delete pingTimer;
    //delete animationTimer;
    //delete imageObject;
}

void Chat::checkConnection()
{
    QNetworkAccessManager nam;
    QNetworkReply *reply = nam.get(QNetworkRequest(QUrl("http://www.google.com")));
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));

    if (!reply->isFinished())
        loop.exec();

    if (reply->bytesAvailable())
    {
        qDebug() << "You are connected to the internet :)";
    }
    else
    {
        qDebug() << "You are not connected to the internet :(";
    }
}

int Chat::saveConversation()
{
    QString FilePath = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("HTML Document (*.html);;Plain Text (*.txt)"));

    if (FilePath != "")
    {
        QFile file(FilePath);

        if (file.open(QIODevice::ReadWrite | QIODevice::Text))
        {
            QTextStream stream(&file);
            stream << ui.textEdit->toHtml();
            file.flush();
            file.close();
        }
        else
        {
            QMessageBox::critical(this, tr("Error"), tr("Could not save file!"));
            return -1;
        }
    }

    return 0;
}

// Determines how the message looks
void Chat::appendMessage(const QImage& icon, const QString &from, const QString &message)
{
    if (icon.isNull() || from.isEmpty() || message.isEmpty())
        return;

    // Rows, columns
    QTextCursor cursor(ui.textEdit->textCursor());
    cursor.movePosition(QTextCursor::End);
    QTextTable *table = cursor.insertTable(2, 3, tableFormat);

    // Alignment
    QTextCharFormat formatName;
    QTextCharFormat formatMessage;
    QTextBlockFormat alignment;
    alignment.setAlignment(Qt::AlignLeft);

    // Font Size
    formatName.setFontPointSize(11);
    formatMessage.setFontPointSize(11);

    // Font Color
    formatName.setForeground(QBrush(QColor(Qt::gray)));
    formatMessage.setForeground(QBrush(QColor(Qt::white)));

    // Fill cells.
    table->cellAt(0, 1).setFormat(formatName);
    table->cellAt(0, 2).setFormat(formatMessage);
    table->cellAt(0, 0).firstCursorPosition().insertImage(icon.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    table->cellAt(0, 1).firstCursorPosition().setBlockFormat(alignment);
    table->cellAt(0, 1).firstCursorPosition().insertText(from);
    table->cellAt(0, 2).firstCursorPosition().insertText(" " + message);

    // Set Current Time with new alignment and color
    QTextBlockFormat leftAlignment;
    leftAlignment.setAlignment(Qt::AlignLeft);
    cursor.mergeBlockFormat(leftAlignment);
    ui.textEdit->setTextCursor(cursor);

    const QString timestamp = QTime::currentTime().toString("hh:mm:ss AP");

    QTextCharFormat format_2;
    format_2.setFontItalic(true);
    format_2.setFontPointSize(8);
    format_2.setForeground(QBrush(QColor(Qt::gray)));

    table->cellAt(1, 0).setFormat(format_2);
    table->cellAt(1, 0).firstCursorPosition().setBlockFormat(leftAlignment);
    table->cellAt(1, 0).firstCursorPosition().insertText(timestamp);

    // Box Scrolling
    QScrollBar *bar = ui.textEdit->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Chat::returnPressed()
{
    if (!hasLoggedIn)
    {
        QString text = ui.usernameEdit->text();

        if (text.isEmpty())
            return;

        if (text.startsWith(QChar('/')))
        {
            QColor color = ui.textEdit->textColor();
            ui.textEdit->setTextColor(Qt::red);
            ui.textEdit->append(tr("! Unknown command: %1").arg(text.left(text.indexOf(' '))));
            ui.textEdit->setTextColor(color);
        }
        else
        {
            // Save icon
            savePressed();

            // Set QLabel icon
            SetIcon();

            // Send username to chat
            myNickName = text;
            newParticipant(myNickName);

            // Send username to client
            client.SendUsername(myNickName);

            // Go to main page
            ui.centralWidget->setCurrentWidget(ui.mainpage);
        }

        ui.usernameEdit->clear();
        hasLoggedIn = true;
    }
    else
    {
        QString text = ui.lineEdit->text();

        if (text.isEmpty()) return;

        if (text.startsWith(QChar('/')))
        {
            QColor color = ui.textEdit->textColor();
            ui.textEdit->setTextColor(Qt::red);
            ui.textEdit->append(tr("! Unknown command: %1").arg(text.left(text.indexOf(' '))));
            ui.textEdit->setTextColor(color);
        }
        else
        {
            // Where we send message to client
            client.SendText(ProfilePicture, myNickName, text);

            // Sending to our chat
            appendMessage(ProfilePicture, myNickName, text);
        }

        // Remove typed message and focus
        ui.lineEdit->clear();
        ui.lineEdit->clearFocus();
    }
}

void Chat::exit()
{
    this->close();
}

void Chat::minimize()
{
    this->setWindowState(Qt::WindowMinimized);
}

// ***** joined!
void Chat::newParticipant(const QString &nick)
{
    if (nick.isEmpty())
        return;

    QColor color = ui.textEdit->textColor();
    ui.textEdit->setTextColor(Qt::gray);
    ui.textEdit->append(tr("* %1 has joined").arg(nick));
    ui.textEdit->setTextColor(color);
    ui.listWidget->addItem(nick);
}

// ***** left!
void Chat::participantLeft(const QString &nick)
{
    if (nick.isEmpty())
        return;

    QList<QListWidgetItem*> items = ui.listWidget->findItems(nick, Qt::MatchExactly);

    if (items.isEmpty())
        return;

    delete items.at(0);
    QColor color = ui.textEdit->textColor();
    ui.textEdit->setTextColor(Qt::gray);
    ui.textEdit->append(tr("* %1 has left").arg(nick));
    ui.textEdit->setTextColor(color);
}

//void Chat::on_User1Message_received(const QString &msg)
//{
    // Rows, columns
    //QTextCursor cursor(ui.textEdit->textCursor());
    //cursor.movePosition(QTextCursor::End);
    //QTextTable *table = cursor.insertTable(2, 3, tableFormat);

//    const QString timestamp = QTime::currentTime().toString("hh:mm:ss AP");
//    const char* userText = "<font color=\"grey\"> %s </font>";
    //ui.textBrowser->setAlignment(Qt::AlignJustify);
    //ui.textBrowser->setText(QString().sprintf(userText, msg.toStdString().c_str()) + timestamp);

    // home ,forward, backward
//}

//void Chat::on_User2Message_received(const QString &msg)
//{
    //ui.textBrowser->append(QString().sprintf("<font color=\"grey\"> %s </font>",msg.toStdString().c_str()));
//}

void Chat::mousePressEvent(QMouseEvent *event)
{
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
    ui.lineEdit->clearFocus();

    // Detect if the click is in the view of QGraphicsView (login page)
    QPoint remapped = ui.graphicsView->mapFromParent(event->pos());

    if (ui.graphicsView->rect().contains(remapped))
    {
         openPressed();
    }
}

void Chat::mouseMoveEvent(QMouseEvent *event)
{
    move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
}

void Chat::openPressed()
{
    QString imagePath = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath(), tr("JPEG (*.jpg *.jpeg);;PNG (*.png)" ));

    imageObject = new QImage();
    imageObject->load(imagePath);

    image = QPixmap::fromImage(*imageObject, Qt::AutoColor);

    QGraphicsScene *scene_2 = new QGraphicsScene(this);
    scene_2->addPixmap(image.scaled(156, 156, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui.graphicsView->setScene(scene_2);
    ui.graphicsView->show();
    ProfilePicture = image.toImage();
}

// This can also be saved in Connection when message is saved
void Chat::savePressed()
{
    ProfilePicture.save(QCoreApplication::applicationDirPath() + "/image.png");
}

void Chat::setNewImage()
{
    QString imagePath = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath(), tr("JPEG (*.jpg *.jpeg);;PNG (*.png)" ));

    QImage* imgObject = new QImage();
    imgObject->load(imagePath);

    QPixmap pixmap = QPixmap::fromImage(*imgObject, Qt::AutoColor);

    QGraphicsScene *scene_4 = new QGraphicsScene(this);
    scene_4->addPixmap(pixmap.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui.iconView->setScene(scene_4);
    ui.iconView->show();
    ProfilePicture = pixmap.toImage();
}

void Chat::SetIcon()
{
    ui.picture->setPixmap(QPixmap::fromImage(ProfilePicture.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    ui.picture->show();

    QGraphicsScene *scene_3 = new QGraphicsScene(this);
    scene_3->addPixmap(QPixmap::fromImage(ProfilePicture.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    ui.iconView->setScene(scene_3);
    ui.iconView->show();
}

void Chat::openSettings()
{
    ui.centralWidget->setCurrentWidget(ui.settings);
}

void Chat::closeSettings()
{
    ProfilePicture.save(QCoreApplication::applicationDirPath() + "/image.png");
    ui.picture->setPixmap(QPixmap::fromImage(ProfilePicture.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    ui.picture->show();
    ui.centralWidget->setCurrentWidget(ui.mainpage);
}

void Chat::fadeInTypingDetection()
{
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect();
    ui.typeDetected->setGraphicsEffect(effect);
    QPropertyAnimation *anim = new QPropertyAnimation(effect,"opacity");
    anim->setDuration(1000);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::InQuad);

    connect(anim, &QPropertyAnimation::finished, [=]()
    {
        //ui.typeDetected->setText("Someone is typing...");
    });

    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void Chat::fadeOutTypingDetection()
{
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect();
    ui.typeDetected->setGraphicsEffect(effect);
    QPropertyAnimation *anim = new QPropertyAnimation(effect,"opacity");
    anim->setDuration(1000);
    anim->setStartValue(1.0);
    anim->setEndValue(0.0);
    anim->setEasingCurve(QEasingCurve::OutQuad);

    connect(anim, &QPropertyAnimation::finished, [=]()
    {
        //ui.typeDetected->setText("Someone is typing..."); // Message Sent! || Typing stopped.
    });

    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void Chat::enableAnimation(const QString &msg)
{
    if (msg.isEmpty())
        return;
    else
    {
        if (!isAnimating)
        {
            if (msg.contains("enable"))
            {
                //qDebug() << "FADE IN";

                animationTimer->start(1000);
                isAnimating = true;
            }
        }
    }
}

void Chat::disableAnimation(const QString &msg)
{
    if (msg.isEmpty())
        return;
    else
    {
        if (isAnimating)
        {
            if (msg.contains("disable"))
            {
                //qDebug() << "FADE OUT";

                animationTimer->stop();
                fadeOutTypingDetection();
                isAnimating = false;
            }
        }
    }
}

void Chat::playMessagingSound()
{
    notification.play();
}

void Chat::rememberMe()
{
    if (ui.radioButton->isChecked())
    {
        QString text = ui.usernameEdit->text();

        if (text.isEmpty())
            return;

        // set default username and image on startup, saving to file
        saveDefaultUser();
    }
    else
    {
        // delete username and image defaults
        QFile defaultsFile(QCoreApplication::applicationDirPath() + "/defaults.txt");
        //newDoc.open(QIODevice::ReadOnly | QIODevice::Text);
        if (defaultsFile.exists()) defaultsFile.remove();
    }
}

void Chat::saveDefaultUser()
{
    ProfilePicture.save(QCoreApplication::applicationDirPath() + "/image.png");

    QByteArray data = ui.usernameEdit->text().toUtf8();

    QFile defaultsFile(QCoreApplication::applicationDirPath() + "/defaults.txt");

    if (defaultsFile.open(QIODevice::WriteOnly | QIODevice::Text))
        defaultsFile.write(data);

    defaultsFile.flush();
    defaultsFile.close();
}

// Does this need to emitted by the client?
bool Chat::eventFilter(QObject* obj, QEvent* event)
{
    const QString enableTxt = "enable";
    const QString disableTxt = "disable";

    if (hasLoggedIn)
    {
        if (obj == ui.lineEdit && event->type() == QEvent::FocusIn)
        {
            client.EnableAnimation(enableTxt);
            return false;
        }
        else if (obj == ui.lineEdit && event->type() == QEvent::FocusOut)
        {
            client.DisableAnimation(disableTxt);
            return false;
        }
    }

    return false;
}
