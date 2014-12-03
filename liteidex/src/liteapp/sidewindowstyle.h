#ifndef SIDEWINDOWSTYLE_H
#define SIDEWINDOWSTYLE_H

#include "windowstyle.h"

class ToolDockWidget;

struct SideActionState
{
    QWidget *toolBtn;
    QWidget *widget;
    QList<QAction*> widgetActions;
    QString id;
    QString  title;
};

class OutputActionBar : public QObject
{
    Q_OBJECT
public:
    OutputActionBar(QSize iconSize, QMainWindow *window, Qt::DockWidgetArea area = Qt::BottomDockWidgetArea);
    virtual ~OutputActionBar();
    ToolDockWidget *dockWidget() const;
    void addAction(QAction *action, QWidget *widget, const QString &id, const QString &title, QList<QAction*> widgetActions);
    void removeAction(QAction *action);
    void setHideToolBar(bool b);
    QAction *findToolAction(QWidget *widget);
signals:
    void moveActionTo(Qt::DockWidgetArea,QAction*);
protected slots:
    void dockVisible(bool);
    void toggledAction(bool b);
public:
    Qt::DockWidgetArea area;
    QToolBar *toolBar;
    QAction  *spacerAct;
    ToolDockWidget *dock;
    QMap<QAction*,SideActionState*> m_actionStateMap;
    bool bHideToolBar;
};

class SideWindowStyle : public IWindowStyle
{
    Q_OBJECT
public:
    SideWindowStyle(QSize iconSize, QMainWindow *window, QObject *parent = 0);
    ~SideWindowStyle();
    virtual QAction *addToolWindow(LiteApi::IApplication *app, Qt::DockWidgetArea area, QWidget *widget, const QString &id, const QString &title, bool split = false, QList<QAction*> widgetActions = QList<QAction*>());
    virtual void removeToolWindow(QAction *action);
    virtual QAction *findToolWindow(QWidget *widget);
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
    OutputActionBar *m_sideBar;
    OutputActionBar *m_outputBar;
    QStatusBar  *m_statusBar;
    QAction     *m_hideSideAct;
};

#endif // SIDEWINDOWSTYLE_H
