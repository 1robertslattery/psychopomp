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
#include <QTextEdit>
#include <QTextCharFormat>
#include <QTextFrameFormat>
#include <QTextCursor>
#include <QFile>
#include <QTime>
#include <QPoint>
#include <QPointF>
#include <QLocale>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include "chat.h"
#include "manager.h"
#include "socket.h"

#if defined (Q_OS_WIN)
    #define OS_NAME "Windows"
#elif defined (Q_OS_OSX)
    #define OS_NAME "Mac OSX"
#elif defined (Q_OS_LINUX)
    #define OS_NAME "Linux"
#else
    #define OS_NAME "Unknown"
#endif

Chat::Chat(QWidget *parent) : QDialog(parent)
{
    // UI Setup
    ui.setupUi(this);
    ui.centralWidget->setAttribute(Qt::WA_QuitOnClose);
    ui.centralWidget->setCurrentWidget(ui.loginpage);
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    // Set location
    QLocale eng(QLocale::English, QLocale::UnitedStates);
    QLocale::setDefault(eng);

    // Set embedded fonts
    QFontDatabase::addApplicationFont(":/root/resources/Roboto-Regular.ttf");
    QFontDatabase::addApplicationFont(":/root/resources/Roboto-Bold.ttf");
    QFontDatabase::addApplicationFont(":/root/resources/Roboto-Italic.ttf");
    QFontDatabase::addApplicationFont(":/root/resources/Roboto-Black.ttf");
    QFontDatabase::addApplicationFont(":/root/resources/Roboto-Medium.ttf");
    QFontDatabase::addApplicationFont(":/root/resources/Roboto-Light.ttf");
    QFontDatabase::addApplicationFont(":/root/resources/Roboto-LightItalic.ttf");
    fontRoboto = QFont("Roboto", 10, 1);
    fontRobotoMedium = QFont("Roboto Medium", 10, 1);
    fontRobotoBlack = QFont("Roboto Black", 10, 1);

    // Set focus ui elements
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
    ui.participantsButton->setFocusPolicy(Qt::NoFocus);
    ui.colorSlider->setFocusPolicy(Qt::NoFocus);

    // Set geometry of animated ui elements
    ui.whiteBkgd->setGeometry(160, 50, 320, 391);
    ui.greyBkgd->setGeometry(140, 80, 361, 331);
    ui.loginLabel->setGeometry(242, 30, 156, 41);
    ui.loginButton->setGeometry(200, 350, 240, 41);
    ui.usernameEdit->setGeometry(200, 300, 240, 41);

    ui.textEdit->setGeometry(120, 227, 399, 0);
    ui.listWidget->setGeometry(100, 227, 441, 0);
    ui.dark_bkgd_2->setGeometry(160, 185, 320, 41);
    ui.lineEdit->setGeometry(160, 230, 320, 40);
    ui.exitButton->setGeometry(395, 190, 32, 32);
    ui.minimizeButton->setGeometry(215, 190, 32, 32);
    ui.picture->setGeometry(305, 190, 32, 32);

    // Set default icon sizes
    QSize defaultSize(0, 0);
    ui.settingsButton->setIconSize(defaultSize);
    ui.participantsButton->setIconSize(defaultSize);
    ui.saveButton->setIconSize(defaultSize);

    // Set color theme slider signals/slots
    connect(ui.colorSlider, SIGNAL(valueChanged(int)), this, SLOT(setColorTheme_Default(int)));
    connect(ui.colorSlider, SIGNAL(valueChanged(int)), this, SLOT(setColorTheme_Amber(int)));
    connect(ui.colorSlider, SIGNAL(valueChanged(int)), this, SLOT(setColorTheme_MetroBlue(int)));
    ui.colorSlider->setMinimum(0);
    ui.colorSlider->setMaximum(2);
    ui.colorSlider->setTickInterval(1);
    ui.colorSlider->setSingleStep(1);

    // Set Default Stylesheets
    ui.loginpage->setStyleSheet("background-color:#424242;");
    ui.loginButton->setStyleSheet("border:1px;border-radius:10px;background-color:#00aedb;color:#eeeeee;");
    ui.loginLabel->setStyleSheet("border:1px;border-radius:10px;background-color:#757575;color:#eeeeee;");
    ui.greyBkgd->setStyleSheet("border:1.5px;border-radius:10px;background-color:#dcdcdc;");
    ui.whiteBkgd->setStyleSheet("border:1.5px;border-radius:10px;background-color:#eeeeee;");
    ui.exitLogin->setStyleSheet("border:1px;border-radius:15px;background-color:#eeeeee;");
    ui.minimizeLogin->setStyleSheet("border:1px;border-radius:15px;background-color:#eeeeee;");
    ui.appTitle->setStyleSheet("border:1px;border-radius:10px;background-color:#424242;color:#eeeeee;");
    ui.usernameEdit->setStyleSheet("border:1.5px;border-radius:10px;background-color:#dcdcdc;color:#424242;");
    ui.acountBlack->setStyleSheet("border:1.5px;border-radius:10px;background-color:#eeeeee;");
    ui.radioButton->setStyleSheet("border:1.5px;border-radius:10px;background-color:#eeeeee;color:#616161;");

    ui.mainpage->setStyleSheet("background-color:#424242;");
    ui.textEdit->setStyleSheet("border:1.5px;border-radius:10px;background-color:#eeeeee;");
    ui.listWidget->setStyleSheet("border:1px;border-radius:10px;background-color:#dcdcdc;color:#dcdcdc;");
    ui.dark_bkgd_2->setStyleSheet("border:1.5px;border-radius:10px;background-color:#757575;");
    ui.lineEdit->setStyleSheet("border:1.5px;border-radius:10px;background-color:#dcdcdc;color:#000000;");
    ui.exitButton->setStyleSheet("border:1px;border-radius:15px;background-color:#757575;");
    ui.minimizeButton->setStyleSheet("border:1px;border-radius:15px;background-color:#757575;");
    ui.mainTitle->setStyleSheet("color:#eeeeee;");

    ui.settings->setStyleSheet("background-color:#424242;");
    ui.backButton->setStyleSheet("border:1.5px;border-radius:10px;background-color:#757575;color:#eeeeee;");
    ui.changeImageButton->setStyleSheet("border:1.5px;border-radius:10px;background-color: #757575;color:#eeeeee;");

    // Set participants window variable and slot
    isParticipantsWindow = false;
    connect(ui.participantsButton, SIGNAL(clicked()), this, SLOT(toggleWindow()));

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
    notification.setSource(QUrl::fromLocalFile(":/root/resources/sending.wav"));
    notification.setLoopCount(0);
    notification.setVolume(0.75f);
    connect(&client, SIGNAL(playSound()), this, SLOT(playMessagingSound()));

    // Set default account picture image, unless they have been saved...
    holderObject = new QImage();

    QFile defaultsFile(QCoreApplication::applicationDirPath() + "/defaults.txt");

    if (defaultsFile.exists())
    {
        if (defaultsFile.size() != 0)
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
            holderObject->load(":/root/resources/picture_holder.jpg");
            QString username = "username";
            ui.usernameEdit->setPlaceholderText(" " + username);
        }
    }
    else
    {
        holderObject->load(":/root/resources/picture_holder.jpg");
        QString username = "username";
        ui.usernameEdit->setPlaceholderText(" " + username);
    }

    holderImage = QPixmap::fromImage(*holderObject, Qt::AutoColor);
    scene = new QGraphicsScene(this);
    scene->addPixmap(holderImage.scaled(156, 156, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui.graphicsView->setScene(scene);
    ui.graphicsView->show();
    accountPicture = holderImage.toImage();

    // Set misc. variable defaults
    canAnimateLogin = false;
    canFadeOut = false;
    canFadeIn = false;
    tableFormat.setBorder(0);
    hasLoggedIn = false;
}

// Checks for internet connection
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
        qDebug() << "You are connected to the internet :)";
    else
        qDebug() << "You are not connected to the internet :(";
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
    formatName.setFontPointSize(10);
    formatMessage.setFontPointSize(10);

    // Font Color
    formatName.setForeground(QBrush(QColor(Qt::darkGray)));
    formatMessage.setForeground(QBrush(QColor(Qt::black)));

    // Fill cells.
    table->cellAt(0, 1).setFormat(formatName);
    table->cellAt(0, 2).setFormat(formatMessage);
    table->cellAt(0, 0).firstCursorPosition().insertImage(icon.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    table->cellAt(0, 1).firstCursorPosition().setBlockFormat(alignment);
    table->cellAt(0, 1).firstCursorPosition().insertText(" " + from);
    table->cellAt(0, 2).firstCursorPosition().insertText(" " + message);

    // Set Current Time with new alignment and color
    QTextBlockFormat leftAlignment;
    leftAlignment.setAlignment(Qt::AlignLeft);
    cursor.mergeBlockFormat(leftAlignment);
    ui.textEdit->setTextCursor(cursor);

    const QString timestamp = QTime::currentTime().toString("hh:mm:ss AP");

    QTextCharFormat format_2;
    format_2.setFontItalic(true);
    format_2.setFontPointSize(7.5);
    format_2.setForeground(QBrush(QColor(Qt::darkGray)));

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
            saveImage();

            // Set QLabel icon
            setIcon();

            // Send username to chat
            myNickName = text;
            newParticipant(myNickName);

            // Send username to client
            client.sendUsername(myNickName);

            // Animate and go to main page
            initLogin();
        }

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
            client.sendMessage(accountPicture, myNickName, text);

            // Sending to our chat
            appendMessage(accountPicture, myNickName, text);
        }

        // Remove typed message and focus
        ui.lineEdit->clear();
        ui.lineEdit->clearFocus();
    }
}

// Close & exit application
void Chat::exit()
{
    this->close();
}

// Minimize application window
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

// Mouse event overrides
void Chat::mousePressEvent(QMouseEvent *event)
{
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
    ui.lineEdit->clearFocus();

    // Detect if the click is in the view of QGraphicsView (login page)
    QPoint remapped = ui.graphicsView->mapFromParent(event->pos());

    if (ui.graphicsView->rect().contains(remapped))
    {
         uploadImage();
    }
}

// Allows user to drag screen
void Chat::mouseMoveEvent(QMouseEvent *event)
{
    move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
}

// Set account picture from login page
void Chat::uploadImage()
{
    QString imagePath = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath(), tr("JPEG (*.jpg *.jpeg);;PNG (*.png)" ));

    imageObject = new QImage();
    imageObject->load(imagePath);

    image = QPixmap::fromImage(*imageObject, Qt::AutoColor);

    QGraphicsScene *scene_2 = new QGraphicsScene(this);
    scene_2->addPixmap(image.scaled(156, 156, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui.graphicsView->setScene(scene_2);
    ui.graphicsView->show();
    accountPicture = image.toImage();
}

// This can also be saved in Connection when message is saved
void Chat::saveImage()
{
    accountPicture.save(QCoreApplication::applicationDirPath() + "/image.png");
}

// Allows user to change their account picture from the settings menu
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
    accountPicture = pixmap.toImage();

    delete imgObject;
    imgObject = nullptr;
}

// Sets account picture in upper left hand corner of chat window
void Chat::setIcon()
{
    ui.picture->setPixmap(QPixmap::fromImage(accountPicture.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    ui.picture->show();

    QGraphicsScene *scene_3 = new QGraphicsScene(this);
    scene_3->addPixmap(QPixmap::fromImage(accountPicture.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    ui.iconView->setScene(scene_3);
    ui.iconView->show();
}

// Settings menu - open
void Chat::openSettings()
{
    ui.centralWidget->setCurrentWidget(ui.settings);
}

// Settings menu - close
void Chat::closeSettings()
{
    accountPicture.save(QCoreApplication::applicationDirPath() + "/image.png");
    ui.picture->setPixmap(QPixmap::fromImage(accountPicture.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    ui.picture->show();
    ui.centralWidget->setCurrentWidget(ui.mainpage);
}

// Fade In Animation
void Chat::fadeInTypingDetection()
{
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect();
    ui.typeDetected->setGraphicsEffect(effect);
    QPropertyAnimation *anim = new QPropertyAnimation(effect, "opacity");
    anim->setDuration(1000);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::InQuad);

    //connect(anim, &QPropertyAnimation::finished, [=]()
    //{
        //ui.typeDetected->setText("Someone is typing...");
    //});

    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

// Fade Out Animation
void Chat::fadeOutTypingDetection()
{
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect();
    ui.typeDetected->setGraphicsEffect(effect);
    QPropertyAnimation *anim = new QPropertyAnimation(effect, "opacity");
    anim->setDuration(1000);
    anim->setStartValue(1.0);
    anim->setEndValue(0.0);
    anim->setEasingCurve(QEasingCurve::OutQuad);

    //connect(anim, &QPropertyAnimation::finished, [=]()
    //{
        //ui.typeDetected->setText("Someone is typing..."); Message Sent! || Typing stopped.
    //});

    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

// Start animation loop
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
                animationTimer->start(1000);
                isAnimating = true;
            }
        }
    }
}

// Stop animation
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
                animationTimer->stop();
                fadeOutTypingDetection();
                isAnimating = false;
            }
        }
    }
}

// Received message notification sound
void Chat::playMessagingSound()
{
    notification.play();
}

// "Remember Me" radio button
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

        if (defaultsFile.exists() && defaultsFile.size() != 0)
        {
            defaultsFile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);
            defaultsFile.waitForBytesWritten(500);
            defaultsFile.flush();
            defaultsFile.close();
        }
    }
}

// When "Remember Me" is selected
void Chat::saveDefaultUser()
{
    accountPicture.save(QCoreApplication::applicationDirPath() + "/image.png");

    QByteArray data = ui.usernameEdit->text().toUtf8();

    QFile defaultsFile(QCoreApplication::applicationDirPath() + "/defaults.txt");

    defaultsFile.open(QIODevice::ReadWrite | QIODevice::Text);
    defaultsFile.write(data);
    defaultsFile.waitForBytesWritten(500);
    defaultsFile.flush();
    defaultsFile.close();
}

// Animation after pressing sign in
void Chat::initLogin()
{
    QTime countdown = QTime::currentTime().addSecs(3);

    while (QTime::currentTime() < countdown)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

        if (!canAnimateLogin)
        {
            // Disable buttons
            ui.exitLogin->setVisible(false);
            ui.minimizeLogin->setVisible(false);
            ui.loginButton->setVisible(false);
            ui.radioButton->setVisible(false);
            ui.graphicsView->hide();
            ui.acountBlack->setVisible(false);
            ui.sizeLabel->setVisible(false);

            // Animate white bkgd
            QPropertyAnimation *anim_3 = new QPropertyAnimation(ui.whiteBkgd, "geometry");
            QRect startRect(160, 50, 320, 391);
            QRect endRect(120, 227, 399, 0);
            anim_3->setDuration(2300);
            anim_3->setStartValue(startRect);
            anim_3->setEndValue(endRect);
            anim_3->setEasingCurve(QEasingCurve::OutQuad);
            anim_3->start(QAbstractAnimation::DeleteWhenStopped);

            // Animate grey bkgd
            QPropertyAnimation *anim_4 = new QPropertyAnimation(ui.greyBkgd, "geometry");
            QRect startRect_2(140, 80, 361, 331);
            QRect endRect_2(120, 227, 399, 0);
            anim_4->setDuration(2300);
            anim_4->setStartValue(startRect_2);
            anim_4->setEndValue(endRect_2);
            anim_4->setEasingCurve(QEasingCurve::OutQuad);
            anim_4->start(QAbstractAnimation::DeleteWhenStopped);

            // Animate "LOGIN"
            const QString welcomeTxt = "WELCOME";
            ui.loginLabel->setText(welcomeTxt);
            ui.loginLabel->setAlignment(Qt::AlignCenter);
            QPropertyAnimation *anim_5 = new QPropertyAnimation(ui.loginLabel, "geometry");
            QRect startRect_3(242, 30, 156, 41);
            QRect endRect_3(160, 185, 320, 41);
            anim_5->setDuration(2000);
            anim_5->setStartValue(startRect_3);
            anim_5->setEndValue(endRect_3);
            anim_5->setEasingCurve(QEasingCurve::OutQuad);
            anim_5->start(QAbstractAnimation::DeleteWhenStopped);

            // Animate username edit
            ui.usernameEdit->setAlignment(Qt::AlignCenter);
            ui.usernameEdit->clearFocus();
            QPropertyAnimation *anim_6 = new QPropertyAnimation(ui.usernameEdit, "geometry");
            QRect startRect_4(200, 300, 240, 41);
            QRect endRect_4(160, 230, 320, 40);
            anim_6->setDuration(2000);
            anim_6->setStartValue(startRect_4);
            anim_6->setEndValue(endRect_4);
            anim_6->setEasingCurve(QEasingCurve::OutQuad);
            anim_6->start(QAbstractAnimation::DeleteWhenStopped);

            canAnimateLogin = true;
        }
    }

    fadeOutLoginPage();
}

// Then fade out the login page
void Chat::fadeOutLoginPage()
{
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect();
    QGraphicsOpacityEffect *effect_2 = new QGraphicsOpacityEffect();

    QTime countdown = QTime::currentTime().addSecs(1);

    while (QTime::currentTime() < countdown)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

        if (!canFadeOut)
        {
            // Fade out welcome
            ui.loginLabel->setGraphicsEffect(effect);
            QPropertyAnimation *fade = new QPropertyAnimation(effect, "opacity");
            fade->setDuration(1000);
            fade->setStartValue(1.0);
            fade->setEndValue(0.0);
            fade->setEasingCurve(QEasingCurve::OutQuad);
            fade->start(QAbstractAnimation::DeleteWhenStopped);

            // Fade out username edit
            ui.usernameEdit->setGraphicsEffect(effect_2);
            QPropertyAnimation *fade_2 = new QPropertyAnimation(effect_2, "opacity");
            fade_2->setDuration(1000);
            fade_2->setStartValue(1.0);
            fade_2->setEndValue(0.0);
            fade_2->setEasingCurve(QEasingCurve::OutQuad);
            fade_2->start(QAbstractAnimation::DeleteWhenStopped);
            canFadeOut = true;
        }
    }

    fadeInChatPage();

    delete effect;
    effect = nullptr;
    ui.loginLabel->setGraphicsEffect(NULL);

    delete effect_2;
    effect_2 = nullptr;
    ui.usernameEdit->setGraphicsEffect(NULL);
}

// Finally fade in the chat page (mainpage)
void Chat::fadeInChatPage()
{
    QGraphicsOpacityEffect *opacity = new QGraphicsOpacityEffect();
    QGraphicsOpacityEffect *opacity_2 = new QGraphicsOpacityEffect();
    QGraphicsOpacityEffect *opacity_3 = new QGraphicsOpacityEffect();
    QGraphicsOpacityEffect *opacity_4 = new QGraphicsOpacityEffect();
    QGraphicsOpacityEffect *opacity_5 = new QGraphicsOpacityEffect();

    QTime countdown = QTime::currentTime().addSecs(1);

    while (QTime::currentTime() < countdown)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

        if (!canFadeIn)
        {
            // Go to main page
            ui.centralWidget->setCurrentWidget(ui.mainpage);
            ui.usernameEdit->clear();

            // Fade in dark bkgd
            ui.dark_bkgd_2->setGraphicsEffect(opacity);
            QPropertyAnimation *fade = new QPropertyAnimation(opacity, "opacity");
            fade->setDuration(1000);
            fade->setStartValue(0.0);
            fade->setEndValue(1.0);
            fade->setEasingCurve(QEasingCurve::InBack);
            fade->start(QAbstractAnimation::DeleteWhenStopped);

            // Fade in picture
            ui.picture->setGraphicsEffect(opacity_2);
            QPropertyAnimation *fade_2 = new QPropertyAnimation(opacity_2, "opacity");
            fade_2->setDuration(1000);
            fade_2->setStartValue(0.0);
            fade_2->setEndValue(1.0);
            fade_2->setEasingCurve(QEasingCurve::InBack);
            fade_2->start(QAbstractAnimation::DeleteWhenStopped);

            // Fade in exit button
            ui.exitButton->setGraphicsEffect(opacity_3);
            QPropertyAnimation *fade_3 = new QPropertyAnimation(opacity_3, "opacity");
            fade_3->setDuration(1000);
            fade_3->setStartValue(0.0);
            fade_3->setEndValue(1.0);
            fade_3->setEasingCurve(QEasingCurve::InBack);
            fade_3->start(QAbstractAnimation::DeleteWhenStopped);

            // Fade in minimize button
            ui.minimizeButton->setGraphicsEffect(opacity_4);
            QPropertyAnimation *fade_4 = new QPropertyAnimation(opacity_4, "opacity");
            fade_4->setDuration(1000);
            fade_4->setStartValue(0.0);
            fade_4->setEndValue(1.0);
            fade_4->setEasingCurve(QEasingCurve::InBack);
            fade_4->start(QAbstractAnimation::DeleteWhenStopped);

            // Fade in line edit
            ui.lineEdit->setGraphicsEffect(opacity_5);
            QPropertyAnimation *fade_5 = new QPropertyAnimation(opacity_5, "opacity");
            fade_5->setDuration(1000);
            fade_5->setStartValue(0.0);
            fade_5->setEndValue(1.0);
            fade_5->setEasingCurve(QEasingCurve::InBack);
            fade_5->start(QAbstractAnimation::DeleteWhenStopped);

            canFadeIn = true;
        }
    }

    initChatWindow();

    delete opacity;
    opacity = nullptr;
    ui.dark_bkgd_2->setGraphicsEffect(NULL);

    delete opacity_2;
    opacity_2 = nullptr;
    ui.picture->setGraphicsEffect(NULL);

    delete opacity_3;
    opacity_3 = nullptr;
    ui.exitButton->setGraphicsEffect(NULL);

    delete opacity_4;
    opacity_4 = nullptr;
    ui.minimizeButton->setGraphicsEffect(NULL);

    delete opacity_5;
    opacity_5 = nullptr;
    ui.lineEdit->setGraphicsEffect(NULL);
}

// The animation that plays when chat window opens
void Chat::initChatWindow()
{
    QTime countdown = QTime::currentTime().addSecs(1);

    while (QTime::currentTime() < countdown)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    // Animate dark bkgd
    QPropertyAnimation *animation = new QPropertyAnimation(ui.dark_bkgd_2, "geometry");
    QRect startRect(160, 185, 320, 41);
    QRect endRect(205, 15, 230, 41);
    animation->setDuration(2500);
    animation->setStartValue(startRect);
    animation->setEndValue(endRect);
    animation->setEasingCurve(QEasingCurve::OutQuad);
    animation->start(QAbstractAnimation::DeleteWhenStopped);

    // Animate line edit
    QPropertyAnimation *animation_2 = new QPropertyAnimation(ui.lineEdit, "geometry");
    QRect startRect_2(160, 230, 320, 40);
    QRect endRect_2(160, 403, 320, 40);
    animation_2->setDuration(2500);
    animation_2->setStartValue(startRect_2);
    animation_2->setEndValue(endRect_2);
    animation_2->setEasingCurve(QEasingCurve::OutQuad);
    animation_2->start(QAbstractAnimation::DeleteWhenStopped);

    // Animate text edit
    QPropertyAnimation *animation_3 = new QPropertyAnimation(ui.textEdit, "geometry");
    QRect startRect_3(120, 227, 399, 0);
    QRect endRect_3(120, 50, 399, 359);
    animation_3->setDuration(2500);
    animation_3->setStartValue(startRect_3);
    animation_3->setEndValue(endRect_3);
    animation_3->setEasingCurve(QEasingCurve::OutQuad);
    animation_3->start(QAbstractAnimation::DeleteWhenStopped);

    // Animate list widget
    QPropertyAnimation *animation_4 = new QPropertyAnimation(ui.listWidget, "geometry");
    QRect startRect_4(120, 227, 399, 0);
    QRect endRect_4(100, 70, 441, 321);
    animation_4->setDuration(2500);
    animation_4->setStartValue(startRect_4);
    animation_4->setEndValue(endRect_4);
    animation_4->setEasingCurve(QEasingCurve::OutQuad);
    animation_4->start(QAbstractAnimation::DeleteWhenStopped);

    // Animate picture
    QPropertyAnimation *animation_5 = new QPropertyAnimation(ui.picture, "geometry");
    QRect startRect_5(305, 190, 32, 32);
    QRect endRect_5(305, 20, 32, 32);
    animation_5->setDuration(2500);
    animation_5->setStartValue(startRect_5);
    animation_5->setEndValue(endRect_5);
    animation_5->setEasingCurve(QEasingCurve::OutQuad);
    animation_5->start(QAbstractAnimation::DeleteWhenStopped);

    // Animate minimize button
    QPropertyAnimation *animation_6 = new QPropertyAnimation(ui.minimizeButton, "geometry");
    QRect startRect_6(215, 190, 32, 32);
    QRect endRect_6(215, 20, 32, 32);
    animation_6->setDuration(2500);
    animation_6->setStartValue(startRect_6);
    animation_6->setEndValue(endRect_6);
    animation_6->setEasingCurve(QEasingCurve::OutQuad);
    animation_6->start(QAbstractAnimation::DeleteWhenStopped);

    // Animate exit button
    QPropertyAnimation *animation_7 = new QPropertyAnimation(ui.exitButton, "geometry");
    QRect startRect_7(395, 190, 32, 32);
    QRect endRect_7(395, 20, 32, 32);
    animation_7->setDuration(2500);
    animation_7->setStartValue(startRect_7);
    animation_7->setEndValue(endRect_7);
    animation_7->setEasingCurve(QEasingCurve::OutQuad);
    animation_7->start(QAbstractAnimation::DeleteWhenStopped);

    // Animate settings button
    QPropertyAnimation *animation_8 = new QPropertyAnimation(ui.settingsButton, "iconSize");
    QSize startRect_8(0, 0);
    QSize endRect_8(24, 24);
    animation_8->setDuration(2500);
    animation_8->setStartValue(startRect_8);
    animation_8->setEndValue(endRect_8);
    animation_8->setEasingCurve(QEasingCurve::OutQuad);
    animation_8->start(QAbstractAnimation::DeleteWhenStopped);

    // Animate participants button
    QPropertyAnimation *animation_9 = new QPropertyAnimation(ui.participantsButton, "iconSize");
    QSize startRect_9(0, 0);
    QSize endRect_9(24, 24);
    animation_9->setDuration(2500);
    animation_9->setStartValue(startRect_9);
    animation_9->setEndValue(endRect_9);
    animation_9->setEasingCurve(QEasingCurve::OutQuad);
    animation_9->start(QAbstractAnimation::DeleteWhenStopped);

    // Animate save button
    QPropertyAnimation *animation_10 = new QPropertyAnimation(ui.saveButton, "iconSize");
    QSize startRect_10(0, 0);
    QSize endRect_10(24, 24);
    animation_10->setDuration(2500);
    animation_10->setStartValue(startRect_10);
    animation_10->setEndValue(endRect_10);
    animation_10->setEasingCurve(QEasingCurve::OutQuad);
    animation_10->start(QAbstractAnimation::DeleteWhenStopped);

    delete holderObject;
    holderObject = nullptr;

    delete scene;
    scene = nullptr;
}

// Switch between chat and participant window
void Chat::toggleWindow()
{
    if (!isParticipantsWindow)
    {
        // Animate textEdit
        QPropertyAnimation *animation = new QPropertyAnimation(ui.textEdit, "geometry");
        QRect startRect(120, 50, 399, 359);
        QRect endRect(100, 70, 441, 321);
        animation->setDuration(1500);
        animation->setStartValue(startRect);
        animation->setEndValue(endRect);
        animation->setEasingCurve(QEasingCurve::OutQuad);
        animation->start(QAbstractAnimation::DeleteWhenStopped);

        // Animate listWidget
        QPropertyAnimation *animation_2 = new QPropertyAnimation(ui.listWidget, "geometry");
        QRect startRect_2(100, 70, 441, 321);
        QRect endRect_2(120, 50, 399, 359);
        animation_2->setDuration(1500);
        animation_2->setStartValue(startRect_2);
        animation_2->setEndValue(endRect_2);
        animation_2->setEasingCurve(QEasingCurve::OutQuad);
        animation_2->start(QAbstractAnimation::DeleteWhenStopped);

        // Change colors and layers
        ui.listWidget->setStyleSheet("border:1px;border-radius:10px;background-color:#eeeeee;color: #000000;");
        ui.textEdit->setStyleSheet("border:1.5px;border-radius:10px;background-color:#dcdcdc;");
        ui.listWidget->raise();
        ui.listWidget->activateWindow();
        ui.dark_bkgd_2->raise();
        ui.dark_bkgd_2->activateWindow();
        ui.picture->raise();
        ui.picture->activateWindow();
        ui.exitButton->raise();
        ui.exitButton->activateWindow();
        ui.minimizeButton->raise();
        ui.minimizeButton->activateWindow();
        ui.lineEdit->raise();
        ui.lineEdit->activateWindow();

        isParticipantsWindow = true;
    }
    else
    {
        // Animate textEdit
        QPropertyAnimation *animation = new QPropertyAnimation(ui.textEdit, "geometry");
        QRect startRect(100, 70, 441, 321);
        QRect endRect(120, 50, 399, 359);
        animation->setDuration(1500);
        animation->setStartValue(startRect);
        animation->setEndValue(endRect);
        animation->setEasingCurve(QEasingCurve::OutQuad);
        animation->start(QAbstractAnimation::DeleteWhenStopped);

        // Animate listWidget
        QPropertyAnimation *animation_2 = new QPropertyAnimation(ui.listWidget, "geometry");
        QRect startRect_2(120, 50, 399, 359);
        QRect endRect_2(100, 70, 441, 321);
        animation_2->setDuration(1500);
        animation_2->setStartValue(startRect_2);
        animation_2->setEndValue(endRect_2);
        animation_2->setEasingCurve(QEasingCurve::OutQuad);
        animation_2->start(QAbstractAnimation::DeleteWhenStopped);

        // Change colors and layers
        ui.listWidget->lower();
        ui.listWidget->setStyleSheet("border:1px;border-radius:10px;background-color:#dcdcdc;color: #dcdcdc;");
        ui.textEdit->setStyleSheet("border:1.5px;border-radius:10px;background-color:#eeeeee;");

        isParticipantsWindow = false;
    }
}

// Dark color theme
void Chat::setColorTheme_Default(int value)
{
    if (ui.colorSlider->value() == 0)
    {
        value = ui.colorSlider->value();

        ui.loginpage->setStyleSheet("background-color:#424242;");
        ui.loginButton->setStyleSheet("border:1px;border-radius:10px;background-color:#00aedb;color:#eeeeee;");
        ui.loginLabel->setStyleSheet("border:1px;border-radius:10px;background-color:#757575;color:#eeeeee;");
        ui.greyBkgd->setStyleSheet("border:1.5px;border-radius:10px;background-color:#dcdcdc;");
        ui.whiteBkgd->setStyleSheet("border:1.5px;border-radius:10px;background-color:#eeeeee;");
        ui.exitLogin->setStyleSheet("border:1px;border-radius:15px;background-color:#eeeeee;");
        ui.minimizeLogin->setStyleSheet("border:1px;border-radius:15px;background-color:#eeeeee;");
        ui.appTitle->setStyleSheet("border:1px;border-radius:10px;background-color:#424242;color:#eeeeee;");
        ui.usernameEdit->setStyleSheet("border:1.5px;border-radius:10px;background-color:#dcdcdc;color:#424242;");
        ui.acountBlack->setStyleSheet("border:1.5px;border-radius:10px;background-color:#eeeeee;");
        ui.radioButton->setStyleSheet("border:1.5px;border-radius:10px;background-color:#eeeeee;color:#616161;");

        ui.mainpage->setStyleSheet("background-color:#424242;");
        ui.textEdit->setStyleSheet("border:1.5px;border-radius:10px;background-color:#eeeeee;");
        ui.listWidget->setStyleSheet("border:1px;border-radius:10px;background-color:#dcdcdc;color:#dcdcdc;");
        ui.dark_bkgd_2->setStyleSheet("border:1.5px;border-radius:10px;background-color:#757575;");
        ui.lineEdit->setStyleSheet("border:1.5px;border-radius:10px;background-color:#dcdcdc;color:#000000;");
        ui.exitButton->setStyleSheet("border:1px;border-radius:15px;background-color:#757575;");
        ui.minimizeButton->setStyleSheet("border:1px;border-radius:15px;background-color:#757575;");
        ui.mainTitle->setStyleSheet("color:#eeeeee;");

        ui.settings->setStyleSheet("background-color:#424242;");
        ui.backButton->setStyleSheet("border:1.5px;border-radius:10px;background-color:#757575;color:#eeeeee;");
        ui.changeImageButton->setStyleSheet("border:1.5px;border-radius:10px;background-color: #757575;color:#eeeeee;");
    }
}

// Colorful color theme
void Chat::setColorTheme_Amber(int value)
{
    if (ui.colorSlider->value() == 1)
    {
        value = ui.colorSlider->value();

        // violet: #571845
        // dark red: #900c3e
        // red: #c70039
        // orange: #ff5733
        // gold: #ffc300

        ui.loginpage->setStyleSheet("background-color:#900c3e;");
        ui.loginButton->setStyleSheet("border:1px;border-radius:10px;background-color:#00aedb;color:#eeeeee;");
        ui.loginLabel->setStyleSheet("border:1px;border-radius:10px;background-color:#757575;color:#eeeeee;");
        ui.greyBkgd->setStyleSheet("border:1.5px;border-radius:10px;background-color:#dcdcdc;");
        ui.whiteBkgd->setStyleSheet("border:1.5px;border-radius:10px;background-color:#eeeeee;");
        ui.exitLogin->setStyleSheet("border:1px;border-radius:15px;background-color:#eeeeee;");
        ui.minimizeLogin->setStyleSheet("border:1px;border-radius:15px;background-color:#eeeeee;");
        ui.appTitle->setStyleSheet("border:1px;border-radius:10px;background-color:#424242;color:#eeeeee;");
        ui.usernameEdit->setStyleSheet("border:1.5px;border-radius:10px;background-color:#dcdcdc;color:#424242;");
        ui.acountBlack->setStyleSheet("border:1.5px;border-radius:10px;background-color:#eeeeee;");
        ui.radioButton->setStyleSheet("border:1.5px;border-radius:10px;background-color:#eeeeee;color:#616161;");

        ui.mainpage->setStyleSheet("background-color:#900c3e;");
        ui.textEdit->setStyleSheet("border:1.5px;border-radius:10px;background-color:#eeeeee;");
        ui.listWidget->setStyleSheet("border:1px;border-radius:10px;background-color:#dcdcdc;color:#dcdcdc;");
        ui.dark_bkgd_2->setStyleSheet("border:1.5px;border-radius:10px;background-color:#757575;");
        ui.lineEdit->setStyleSheet("border:1.5px;border-radius:10px;background-color:#dcdcdc;color:#000000;");
        ui.exitButton->setStyleSheet("border:1px;border-radius:15px;background-color:#757575;");
        ui.minimizeButton->setStyleSheet("border:1px;border-radius:15px;background-color:#757575;");
        ui.mainTitle->setStyleSheet("color:#eeeeee;");

        ui.settings->setStyleSheet("background-color:#900c3e;");
        ui.backButton->setStyleSheet("border:1.5px;border-radius:10px;background-color:#757575;color:#eeeeee;");
        ui.changeImageButton->setStyleSheet("border:1.5px;border-radius:10px;background-color: #757575;color:#eeeeee;");
    }
}

// Blue color theme
void Chat::setColorTheme_MetroBlue(int value)
{
    if (ui.colorSlider->value() == 2)
    {
        value = ui.colorSlider->value();

        ui.loginpage->setStyleSheet("background-color:#04396c;");
        ui.loginButton->setStyleSheet("border:1px;border-radius:10px;background-color:#00aedb;color:#eeeeee;");
        ui.loginLabel->setStyleSheet("border:1px;border-radius:10px;background-color:#757575;color:#eeeeee;");
        ui.greyBkgd->setStyleSheet("border:1.5px;border-radius:10px;background-color:#dcdcdc;");
        ui.whiteBkgd->setStyleSheet("border:1.5px;border-radius:10px;background-color:#eeeeee;");
        ui.exitLogin->setStyleSheet("border:1px;border-radius:15px;background-color:#eeeeee;");
        ui.minimizeLogin->setStyleSheet("border:1px;border-radius:15px;background-color:#eeeeee;");
        ui.appTitle->setStyleSheet("border:1px;border-radius:10px;background-color:#424242;color:#eeeeee;");
        ui.usernameEdit->setStyleSheet("border:1.5px;border-radius:10px;background-color:#dcdcdc;color:#424242;");
        ui.acountBlack->setStyleSheet("border:1.5px;border-radius:10px;background-color:#eeeeee;");
        ui.radioButton->setStyleSheet("border:1.5px;border-radius:10px;background-color:#eeeeee;color:#616161;");

        ui.mainpage->setStyleSheet("background-color:#04396c;");
        ui.textEdit->setStyleSheet("border:1.5px;border-radius:10px;background-color:#eeeeee;");
        ui.listWidget->setStyleSheet("border:1px;border-radius:10px;background-color:#dcdcdc;color:#dcdcdc;");
        ui.dark_bkgd_2->setStyleSheet("border:1.5px;border-radius:10px;background-color:#757575;");
        ui.lineEdit->setStyleSheet("border:1.5px;border-radius:10px;background-color:#dcdcdc;color:#000000;");
        ui.exitButton->setStyleSheet("border:1px;border-radius:15px;background-color:#757575;");
        ui.minimizeButton->setStyleSheet("border:1px;border-radius:15px;background-color:#757575;");
        ui.mainTitle->setStyleSheet("color:#eeeeee;");

        ui.settings->setStyleSheet("background-color:#04396c;");
        ui.backButton->setStyleSheet("border:1.5px;border-radius:10px;background-color:#757575;color:#eeeeee;");
        ui.changeImageButton->setStyleSheet("border:1.5px;border-radius:10px;background-color: #757575;color:#eeeeee;");
    }
}

// Event filter for typing detection animation
bool Chat::eventFilter(QObject* obj, QEvent* event)
{
    const QString enableTxt = "enable";
    const QString disableTxt = "disable";

    if (hasLoggedIn)
    {
        if (obj == ui.lineEdit && event->type() == QEvent::FocusIn)
        {
            client.enableAnimation(enableTxt);
            return false;
        }
        else if (obj == ui.lineEdit && event->type() == QEvent::FocusOut)
        {
            client.disableAnimation(disableTxt);
            return false;
        }
    }

    return false;
}
