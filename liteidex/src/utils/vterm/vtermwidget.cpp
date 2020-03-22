#include "vtermwidget.h"
#include <QProcess>
#include <QResizeEvent>
#include <QScrollBar>
#include <QDebug>
#include <signal.h>
#include <QKeySequence>
#include <QClipboard>
#include <QApplication>

#if defined(Q_OS_MAC)
# define TermControlModifier Qt::MetaModifier
#else
# define TermControlModifier Qt::ControlModifier
#endif


VTermWidget::VTermWidget(QWidget *parent) : VTermWidgetBase(24,80,parent)
{
    m_process = PtyQt::createPtyProcess(IPtyProcess::AutoPty);
    connect(this,SIGNAL(sizeChanged(int,int)),this,SLOT(resizePty(int,int)));
    connect(m_process,SIGNAL(exited()),this,SIGNAL(exited()));
}

VTermWidget::~VTermWidget()
{
    delete m_process;
}

void VTermWidget::start(const QString &program, const QStringList &arguments, const QString &workingDirectory, QStringList env)
{
    m_process->startProcess(program,arguments,workingDirectory,env,qint16(m_rows),qint16(m_cols));
    connect(m_process->notifier(),SIGNAL(readyRead()),this,SLOT(readyRead()));
}

void VTermWidget::readyRead()
{
    QByteArray data = m_process->readAll();
    if (data.isEmpty()) {
        return;
    }
    this->inputWrite(data);
}

void VTermWidget::resizeEvent(QResizeEvent *e)
{
    VTermWidgetBase::resizeEvent(e);
}

void VTermWidget::keyPressEvent(QKeyEvent *e)
{    
//#ifdef Q_OS_WIN
    // WINDOWS copy & clear selection
    if (hasSelection() && (e == QKeySequence::Copy || e->key() == Qt::Key_Return)) {
        QString text = selectedText();
        if (!text.isEmpty()) {
            qApp->clipboard()->setText(text);
            clearSelection();
            return;
        }
    }
//#else
//    if (e == QKeySequence::Copy) {
//        QString text = selectedText();
//        if (!text.isEmpty()) {
//            qApp->clipboard()->setText(text);
//        }
//        return;
//    }
//#endif
    else if (e == QKeySequence::Paste) {
        QString text = qApp->clipboard()->text();
        if (!text.isEmpty()) {
            m_process->write(text.toUtf8());
        }
        return;
    } else if (e == QKeySequence::SelectAll) {
        selectAll();
        return;
    }
    if ((e->modifiers() & TermControlModifier) ) {
        QChar c(e->key());
        char asciiVal = c.toUpper().toLatin1();
        QByteArray array;
        if (asciiVal >= 0x41 && asciiVal <= 0x5f) {
            array.push_back(asciiVal-0x40);
            m_process->write(array);
            return;
        }
    }
    VTermWidgetBase::keyPressEvent(e);
}

void VTermWidget::resizePty(int rows, int cols)
{
    m_process->resize(cols,rows);
}

void VTermWidget::write_data(const char *buf, int len)
{
    m_process->write(QByteArray(buf,len));
}
