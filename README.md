# Psychopomp
A Client/Server Chat Application

![alt text](https://github.com/1robertslattery/psychopomp/blob/master/screenshots/login_1.png?raw=true "Screenshot")
![Alt text](/screenshots/chat_screen.png?raw=true "Optional Title")
![Alt text](/screenshots/login_2.png?raw=true "Optional Title")

Psychopomp is an open-source chat application.  Its purpose is to allow multiple client PCs on a 
Local Area Network (LAN) to send messages to each other.  It is written in C++, using Qt. 
It is released under the terms of the GNU Lesser General Public License (LGPL) version 3, in line
with the Qt Open Source Edition, permitting the use of Qt's proprietary library for research and/or
commercial purposes.

## PROJECT DETAILS

The Psychompomp project aims to be customizable and easy to use.

**Features**

- Custom username and picture
- Typing detection across network
- Saving conversations to file (.html, .txt)

To generate the ui_mainwindow.h file, you will have to run the Qt User Interface Compiler (uic.exe) from the .ui file directory.

**An example of how to do this from command line:**
```
cd C:\psychopomp
C:\Qt\5.10.1\msvc2015_64\bin\uic mainwindow.ui -o ui_mainwindow.h
```
A NSIS installer file is available for Windows in the \nsis folder. It uses the [UltraModernUI plugin](http://ultramodernui.sourceforge.net/).

Visit http://doc.qt.io/qt-5/qtnetwork-programming.html for more information on Qt's Networking Framework.

Getting started with Qt: http://doc.qt.io/qt-5/gettingstarted.html

Setting up a generic project in Qt: http://doc.qt.io/qtcreator/creator-project-generic.html

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

Available for Windows 7/8.1/10 (x64).  Custom color themes are not yet available, however.  This software will listen for ANY network connection 
enabled on your PC and may discover virtual machine guest connections, which will lead to errors.  Bugs and bottlenecks are being worked out. 
Psychopomp is shipped as-is without warranty.


