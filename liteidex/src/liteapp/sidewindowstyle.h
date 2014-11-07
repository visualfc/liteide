#ifndef SIDEWINDOWSTYLE_H
#define SIDEWINDOWSTYLE_H

#include "windowstyle.h"

class SideWindowStyle : public IWindowStyle
{
    Q_OBJECT
public:
    SideWindowStyle(QSize iconSize, QMainWindow *window, QObject *parent = 0);
    ~SideWindowStyle();
    virtual QAction *addToolWindow(LiteApi::IApplication *app, Qt::DockWidgetArea area, QWidget *widget, const QString &id, const QString &title, bool split = false, QList<QAction*> widgetActions = QList<QAction*>());
    virtual void removeToolWindow(QAction *action);
    virtual QAction *findToolWindow(QWidget *wiget);
    virtual void moveToolWindow(Qt::DockWidgetArea area,QAction *action,bool split);
    virtual QByteArray saveToolState(int version = 0) const;
    virtual bool loadInitToolState(const QByteArray &state, int version = 0);
    virtual bool restoreState(const QByteArray &state, int version = 0);
public slots:
    virtual void hideToolWindow(Qt::DockWidgetArea area = Qt::BottomDockWidgetArea);
    virtual void showOrHideToolWindow();
    virtual void hideAllToolWindows();
protected:
    QMainWindow *m_mainWindow;
};

#endif // SIDEWINDOWSTYLE_H
