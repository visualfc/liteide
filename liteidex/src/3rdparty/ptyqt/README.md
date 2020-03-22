# Pty-Qt - C++ library for work with PseudoTerminals

Pty-Qt is small library for access to console applications by pseudo-terminal interface on Mac, Linux and Windows. On Mac and Linux you can use standard PseudoTerminal API and on Windows you can use WinPty or ConPty.

### CI Status

Ubuntu / MacOS X: [![Build Status](https://travis-ci.org/kafeg/ptyqt.svg?branch=master)](https://travis-ci.org/kafeg/ptyqt)

Windows: [![Build Status](https://ci.appveyor.com/api/projects/status/github/kafeg/ptyqt?svg=true)](https://ci.appveyor.com/project/kafeg/ptyqt)

## Pre-Requirements and build
  - ConPty part works only on Windows 10 >= 1903 (build > 18309) and can be built only with Windows SDK >= 10.0.18346.0 (maybe >= 17134, but not sure)
  - WinPty part requires winpty sdk for build and winpty.dll with winpty-agent.exe for deployment with target application. WinPty can work on Windows XP and later (depended on used build SDK: vc140 / vc140_xp). You can't link WinPty libraries inside your App, because it use cygwin for build.
  - UnixPty part can work on both Linux/Mac versions, because it based on standard POSIX pseudo terminals API
  - Еarget platforms: x86 or x64
  - Required Qt >= 5.10
  - On Windows should be installed: Git for Windows, Visual Studio >=2015

### Build on Windows (cmd.exe)
```sh
set VCPKG_ROOT=c:/dev/ptyqtroot/vcpkg
set GIT_PATH="c:\Program Files\Git\bin\git.exe"
set CMAKE_PATH=%VCPKG_ROOT%/downloads/tools/cmake-3.12.4-windows/cmake-3.12.4-win32-x86/bin/cmake.exe
set TRIPLET=x64-windows

mkdir %VCPKG_ROOT%
cd %VCPKG_ROOT%
%GIT_PATH% clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg.exe integrate install
.\vcpkg.exe env
cd %VCPKG_ROOT%
.\vcpkg.exe install qt5-base:%TRIPLET% qt5-websockets:%TRIPLET%
cd ..
%GIT_PATH% clone https://github.com/kafeg/ptyqt.git ptyqt
mkdir ptyqt-build
cd ptyqt-build

%CMAKE_PATH% ../ptyqt "-DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake" "-DVCPKG_TARGET_TRIPLET=%TRIPLET%" "-DNO_BUILD_EXAMPLES=1"
%CMAKE_PATH% --build . --target winpty
%CMAKE_PATH% --build .
```

### Build on Ubuntu
```sh
sudo apt-get install qtbase5-dev cmake libqt5websockets5-dev
git clone https://github.com/kafeg/ptyqt.git ptyqt
mkdir ptyqt-build
cd ptyqt-build
cmake ../ptyqt
cmake --build .
```

### Build on Mac
```sh
brew install qt cmake
git clone https://github.com/kafeg/ptyqt.git ptyqt
mkdir ptyqt-build
cd ptyqt-build
cmake ../ptyqt "-DCMAKE_PREFIX_PATH=/usr/local/Cellar/qt/5.12.1"
cmake --build .
```

## Usage
Standard way: build and install library then link it to your project and check examples for sample code.

For example, this code snipped works on Windows, Linux and Mac and make interface from Pty to XTermJS:
```cpp
#include <QCoreApplication>
#include <QWebSocketServer>
#include <QWebSocket>
#include "ptyqt.h"
#include <QTimer>
#include <QProcessEnvironment>
#include <QSysInfo>

#define PORT 4242

#define COLS 87
#define ROWS 26

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    //start WebSockets server for receive connections from xterm.js
    QWebSocketServer wsServer("TestServer", QWebSocketServer::NonSecureMode);
    if (!wsServer.listen(QHostAddress::Any, PORT))
        return 1;

    QMap<QWebSocket *, IPtyProcess *> sessions;

    //create new session on new connection
    QObject::connect(&wsServer, &QWebSocketServer::newConnection, [&wsServer, &sessions]()
    {
        //handle new connection
        QWebSocket *wSocket = wsServer.nextPendingConnection();

        //use cmd.exe or bash, depends on target platform
        IPtyProcess::PtyType ptyType = IPtyProcess::WinPty;
        qint32 buildNumber = QSysInfo::kernelVersion().split(".").last().toInt();
        if (buildNumber >= CONPTY_MINIMAL_WINDOWS_VERSION)
        {
            qDebug() << "Use ConPty except of WinPty";
            ptyType = IPtyProcess::ConPty;
        }

        QString shellPath = "c:\\Windows\\system32\\cmd.exe";
        //shellPath = "C:\\Program\ Files\\Git\\bin\\bash.exe";
#ifdef Q_OS_UNIX
        shellPath = "/bin/sh";
        ptyType = IPtyProcess::UnixPty;
#endif

        //create new Pty instance
        IPtyProcess *pty = PtyQt::createPtyProcess(ptyType);

        qDebug() << "New connection" << wSocket->peerAddress() << wSocket->peerPort() << pty->pid();

        //start Pty process ()
        pty->startProcess(shellPath, QProcessEnvironment::systemEnvironment().toStringList(), COLS, ROWS);

        //connect read channel from Pty process to write channel on websocket
        QObject::connect(pty->notifier(), &QIODevice::readyRead, [wSocket, pty]()
        {
            wSocket->sendTextMessage(pty->readAll());
        });

        //connect read channel of Websocket to write channel of Pty process
        QObject::connect(wSocket, &QWebSocket::textMessageReceived, [wSocket, pty](const QString &message)
        {
            pty->write(message.toLatin1());
        });

        //...
        //for example handle disconnections, process crashes and stuff like that...
        //...

        //add connection to list of active connections
        sessions.insert(wSocket, pty);

        qDebug() << pty->size();
    });

    //stop eventloop if needed
    //QTimer::singleShot(5000, [](){ qApp->quit(); });

    //exec eventloop
    bool res = app.exec();

    QMapIterator<QWebSocket *, IPtyProcess *> it(sessions);
    while (it.hasNext())
    {
        it.next();

        it.key()->deleteLater();
        delete it.value();
    }
    sessions.clear();

    return res;
}
```

## Examples
### XtermJS
- build and run example from cmd.exe
- install nodejs (your prefer way)
- open console and run:
```sh
cd ptyqt/examples/xtermjs
npm install xterm
npm install http-server -g
http-server ./
```
- open http://127.0.0.1:8080/ in Web browser
- use your terminal, for example install and run 'Midnight Commander' or 'Far' for test pseduo-graphic interface

**IMPORTANT**
- do not use Git Bash for run 'xtermjs_sample.exe' on Windows, it has some issues: https://github.com/git-for-windows/git/wiki/FAQ#some-native-console-programs-dont-work-when-run-from-git-bash-how-to-fix-it
- Only Far manager >= 3.0 supported by XTermJS, all old versioans are unsupported
- ConPty requires to run your application from existing terminal session, in another case it just not work. For example in Qt Creator on Windows check "Run in Terminal" in project run settings before run examples or tests

### QVTerminal
This is very-very-very basic implementation of VT100 terminal + Pty-Qt.
It's just forked repository from https://github.com/sebcaux/QVTerminal and used for this example with connection to real PTY.
So, you can see basic sample howto make VT100-like terminal from scratch.
Just build project and run executable of this example.

Also, you can find for example projects like https://github.com/lxqt/qterminal, they all based on QTermWidget and they all not-crossplatform and support only UNIX. But QTermWidget is support full VT100 protocol because it's fork from Linux/KDE/Konsole application.

## More information
Resources used to develop this library:
  - https://github.com/Microsoft/node-pty
  - https://github.com/Microsoft/console
  - https://github.com/rprichard/winpty
  - https://github.com/xtermjs/xterm.js
  - https://github.com/lxqt/qterminal
  - https://devblogs.microsoft.com/commandline/windows-command-line-introducing-the-windows-pseudo-console-conpty/
  - https://devblogs.microsoft.com/commandline/windows-command-line-backgrounder/
  - https://github.com/sebcaux/QVTerminal

## XtermJS + PTY-Qt + C++ sample screenshots

![Midnight Commander](https://github.com/kafeg/ptyqt/raw/master/examples/xtermjs/screens/midnight_commander_bash_unix.png)

![Far Manager](https://github.com/kafeg/ptyqt/raw/master/examples/xtermjs/screens/far_manager_cmd_windows.png)
