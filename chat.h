#ifndef CHAT_H
#define CHAT_H

#include "ui_mainwindow.h"
#include <QtWidgets/QtWidgets>
#include <QtWidgets/QDialog>
#include <QtGui/QTextTableFormat>
#include <QCloseEvent>
#include <QPushButton>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QLineEdit>
#include <QGraphicsView>
#include <QtMultimedia/QSoundEffect>
#include <QImage>
#include "client.h"

class Chat : public QDialog
{

    Q_OBJECT

public:

    Chat(QWidget *parent = 0);

    ~Chat();

    Ui::MainWindow ui;

    // Displayed name
    QString myNickName;

    // User Icon
    QImage ProfilePicture;

    bool isAnimating;

public slots:

    void appendMessage(const QImage& icon, const QString &from, const QString &message);
    void enableAnimation(const QString &msg);
    void disableAnimation(const QString &msg);
    void playMessagingSound();

private slots:

    // Write in chat
    void returnPressed();
    void newParticipant(const QString &nick);
    void participantLeft(const QString &nick);

    void exit();
    void minimize();
    void openSettings();
    void closeSettings();
    void savePressed();
    void checkConnection();
    int saveConversation();
    void setNewImage();

    void fadeInTypingDetection();
    void fadeOutTypingDetection();

    void rememberMe();

private:

    void openPressed();
    void saveDefaultUser();
    Client client;
    QTimer* pingTimer;
    QTimer* animationTimer;
    QTextTableFormat tableFormat;

    void SetIcon();

    int m_nMouseClick_X_Coordinate;
    int m_nMouseClick_Y_Coordinate;

    QPixmap image;
    QImage *imageObject;
    QGraphicsScene *scene;

    QPixmap holderImage;
    QImage* holderObject;

    bool hasLoggedIn;

    QSoundEffect notification;

protected:

    // Drag window
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

    virtual bool eventFilter(QObject* obj, QEvent* event);

};

#endif
