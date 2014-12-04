#ifndef WINDOWSTYLE_H
#define WINDOWSTYLE_H

#include <liteapi/liteapi.h>

class IWindowStyle : public QObject
{
    Q_OBJECT
public:
    IWindowStyle(QObject *parent) : QObject(parent) {}
    virtual QAction *addToolWindow(LiteApi::IApplication *app, Qt::DockWidgetArea area, QWidget *widget, const QString &id, const QString &title, bool split = false, QList<QAction*> widgetActions = QList<QAction*>()) = 0;
    virtual void removeToolWindow(QAction *action) = 0;
    virtual QAction *findToolWindow(QWidget *wiget) = 0;
    virtual void moveToolWindow(Qt::DockWidgetArea area,QAction *action,bool split) = 0;
    virtual void saveToolState() const = 0;
    virtual void restoreToolsState() = 0;
public slots:
    virtual void hideOutputWindow() = 0;
    virtual void showOrHideToolWindow() = 0;
    virtual void hideAllToolWindows() = 0;
};

#endif // WINDOWSTYLE_H
