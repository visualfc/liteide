#ifndef SIDEWINDOWSTYLE_H
#define SIDEWINDOWSTYLE_H

#include "windowstyle.h"

class SideDockWidget;
class OutputDockWidget;
class QToolButton;

struct SideActionState
{
    QWidget *toolBtn;
    QWidget *widget;
    QList<QAction*> widgetActions;
    QString id;
    QString title;
};

class SideActionBar : public QObject
{
    Q_OBJECT
public:
    SideActionBar(QSize iconSize, QMainWindow *window, Qt::DockWidgetArea area = Qt::BottomDockWidgetArea);
    virtual ~SideActionBar();
    void addAction(QAction *action, QWidget *widget, const QString &id, const QString &title, QList<QAction*> widgetActions);
    void removeAction(QAction *action);
    QAction *findToolAction(QWidget *widget);
signals:
    void moveActionTo(Qt::DockWidgetArea,QAction*);
protected slots:
    void setHideToolBar(bool b);
    void dockVisible(bool);
    void toggledAction(bool b);
    void currenActionChanged(QAction *org, QAction *act);
public:
    QSize iconSize;
    QMainWindow *window;
    Qt::DockWidgetArea area;
    QToolBar *toolBar;
    QAction  *spacerAct;
    QList<SideDockWidget*> m_dockList;
    QMap<QAction*,SideActionState*> m_actionStateMap;
    bool bHideToolBar;
    void updateAction(QAction *action);
};

struct OutputActionState
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
    OutputDockWidget *dockWidget() const;
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
    OutputDockWidget *dock;
    QMap<QAction*,OutputActionState*> m_actionStateMap;
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
    virtual void saveToolState();
    virtual void restoreToolsState();
    void restoreHideToolWindows();
    void restoreHideSideToolWindows();
    void hideSideToolWindows();
public slots:
    virtual void hideToolWindow(Qt::DockWidgetArea area = Qt::BottomDockWidgetArea);
    virtual void hideOutputWindow();
    virtual void showOrHideToolWindow();
    virtual void hideAllToolWindows();
    void hideSideBar(bool);
    void toggledSideBar(bool);
protected:
    QMainWindow *m_mainWindow;
    SideActionBar *m_sideBar;
    OutputActionBar *m_outputBar;
    QStatusBar  *m_statusBar;
    QAction     *m_hideSideAct;
    QList<QAction*> m_hideActionList;
    QList<QAction*> m_hideSideActionList;
};

#endif // SIDEWINDOWSTYLE_H
