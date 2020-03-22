#include <QApplication>
#include <QTimer>
#include "qvterminal.h"
#include "ptydevice.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QVTerminal vtTerm;

    PtyDevice ptyDevice;

    vtTerm.setIODevice(&ptyDevice);
#ifdef Q_OS_WIN
    vtTerm.setCrlf(true);
#endif

    vtTerm.show();
    vtTerm.resize(640, 480);
    ptyDevice.start();

    //exec eventloop
    bool res = app.exec();

    return res;
}
