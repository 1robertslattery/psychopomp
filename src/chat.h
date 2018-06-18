#ifndef CHAT_H
#define CHAT_H

#include "ui_mainwindow.h"
#include <QtWidgets/QtWidgets>
#include <QtWidgets/QDialog>
#include <QtGui/QTextTableFormat>
#include <QCloseEvent>
#include <QPushButton>
#include <QMouseEvent>
#include <QLineEdit>
#include <QGraphicsView>
#include <QtMultimedia/QSoundEffect>
#include "client.h"

class Chat : public QDialog
{

    Q_OBJECT

public:

    // Constructor
    Chat(QWidget *parent = 0);

    // Generated ui
    Ui::MainWindow ui;

    // Username variable
    QString myNickName;

    // Account picture variable
    QImage accountPicture;

    // Variable for typing detection animation
    bool isAnimating;

public slots:

    // New message method
    void appendMessage(const QImage& icon, const QString &from, const QString &message);

    // Animation methods
    void enableAnimation(const QString &msg);
    void disableAnimation(const QString &msg);

    // Sound method
    void playMessagingSound();

private slots:

    // Send message method
    void returnPressed();

    // User detection methods
    void newParticipant(const QString &nick);
    void participantLeft(const QString &nick);

    // Close and Minimize window methods
    void exit();
    void minimize();

    // Settings menu methods
    void openSettings();
    void closeSettings();

    // Saving user image method
    void saveImage();

    // Check internet status method
    void checkConnection();

    // Save conversation to .html/.txt method
    int saveConversation();

    // Change image from settings menu method
    void setNewImage();

    // Animation methods for type detection
    void fadeInTypingDetection();
    void fadeOutTypingDetection();

    // "Remember Me" radio button method
    void rememberMe();

    // Switch between chat and participant window
    void toggleWindow();

private:

    // User picture upload method
    void uploadImage();

    // Saving defaults method
    void saveDefaultUser();

    // Client class variable
    Client client;

    // Formatting for how message is displayed
    QTextTableFormat tableFormat;

    // Sets account picture in upper left hand corner of chat window
    void setIcon();

    // Drag window variables
    int m_nMouseClick_X_Coordinate = 0;
    int m_nMouseClick_Y_Coordinate = 0;

    // Image variables
    QPixmap image;
    QImage *imageObject;
    QGraphicsScene *scene;
    QPixmap holderImage;
    QImage* holderObject;

    // Login variable
    bool hasLoggedIn;

    // Partipants window variable
    bool isParticipantsWindow;

    // Sound
    QSoundEffect notification;

    // Variables for timers
    QTimer* pingTimer;
    QTimer* animationTimer;

protected:

    // Mouse event overrides
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

    // Filter for focus detection
    virtual bool eventFilter(QObject* obj, QEvent* event);

};

#endif
