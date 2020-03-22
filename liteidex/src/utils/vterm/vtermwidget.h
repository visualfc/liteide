#ifndef VTERMWIDGET_H
#define VTERMWIDGET_H

#include "vtermwidgetbase.h"
#include "ptyqt/core/ptyqt.h"

class QProcess;
class VTermWidget : public VTermWidgetBase
{
    Q_OBJECT
public:
    explicit VTermWidget(QWidget *parent);
    virtual ~VTermWidget();
    void start(const QString &program, const QStringList &arguments, const QString &workingDirectory, QStringList env);
signals:
    void exited();
public slots:
    void readyRead();
    void resizePty(int rows,int cols);
protected:
    virtual void write_data(const char *buf, int len);
    virtual void resizeEvent(QResizeEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
protected:
    IPtyProcess *m_process;
};

#endif // VTERMWIDGET_H
