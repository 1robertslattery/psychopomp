# Psychopomp
A Client/Server Chat Application

Psychopomp is an open-source chat application.  Its purpose is to allow multiple client PCs on a 
Local Area Network (LAN) to send messages to each other.  It is written in C++, using Qt. 
It is released under the terms of the GNU Lesser General Public License (LGPL) version 3, in line
with the Qt Open Source Edition, permitting the use of Qt's proprietary library for research and/or
commercial purposes.

![Alt text](/screenshots/login_1.PNG?raw=true)
![Alt text](/screenshots/chat_screen.PNG?raw=true)
![Alt text](/screenshots/login_2.PNG?raw=true)

## PROJECT DETAILS

The Psychopomp project aims to be customizable and easy to use. Psychopomp derives from Greek, meaning the "guide of souls".

**Features**

- Custom username and picture
- Custom color themes
- Typing detection across network
- Notification sounds
- Saving conversations to file (.html, .txt)

To generate the ui_mainwindow.h file, you will have to run the Qt User Interface Compiler (uic.exe) from the .ui file directory.

**An example of how to do this from command line:**
```
cd C:\psychopomp
C:\Qt\5.10.1\msvc2015_64\bin\uic mainwindow.ui -o ui_mainwindow.h
```
A NSIS installer file is available for Windows in the \installation folder. It uses the [UltraModernUI plugin](http://ultramodernui.sourceforge.net/).

Visit http://doc.qt.io/qt-5/qtnetwork-programming.html for more information on Qt's Networking Framework.

Getting started with Qt: http://doc.qt.io/qt-5/gettingstarted.html

Setting up a generic project in Qt: http://doc.qt.io/qtcreator/creator-project-generic.html

## TRY IT OUT

Available for Windows 7/8.1/10 (x64).  An installer for v1.2.0 is available in the [release](https://github.com/1robertslattery/psychopomp/releases) tab.

## PREREQUISITES

- Qt 5.10.1 or later
- MinGW with g++ 4.9 or Visual Studio 2015 or later
- [Visual C++ 2015 Redistributable (x64)](https://www.microsoft.com/en-us/download/details.aspx?id=52685)

**Psychopomp runs on the following .dll:**

- Qt5Core.dll
- Qt5Gui.dll
- Qt5Multimedia.dll
- Qt5Network.dll
- Qt5Widgets.dll

## NOTES

Custom color themes can be changed to your preference in chat .h/.cpp.  This software will listen for ANY network connection 
enabled on your PC and may discover virtual machine guest connections, which will lead to errors.  There is a known issue with the QTcpSocket timeout() that may interfere with messages. It is being worked out. 
Psychopomp is shipped as-is without warranty.


