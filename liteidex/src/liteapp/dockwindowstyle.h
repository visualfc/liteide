#ifndef DOCKWINDOWSTYLE_H
#define DOCKWINDOWSTYLE_H

#include "windowstyle.h"

#include <QMap>
#include <QPointer>
#include <QAction>

class ActionGroup;
class QSplitter;
class RotationToolButton;
class ToolDockWidget;

class ActionGroup : public QObject
{
    Q_OBJECT
public:
    ActionGroup(QObject *parent);
    QList<QAction *> actions() const;
    void addAction(QAction *act);
    void removeAction(QAction *act);
    QAction * checkedAction () const;
protected slots:
    void actionChanged();
protected:
    QList<QAction *> m_actions;
    QPointer<QAction> current;
};

class ActionToolBar : public QObject
{
    Q_OBJECT
public:
    ActionToolBar(QSize iconSize, QWidget *parent, Qt::DockWidgetArea area);
    ToolDockWidget *dock(bool split) const;
    void addAction(QAction *action, const QString &title, bool split);
    void removeAction(QAction *action, bool split);
    void setHideToolBar(bool b);
signals:
    void moveActionTo(Qt::DockWidgetArea,QAction*,bool);
protected slots:
    void dock1Visible(bool);
    void dock2Visible(bool);
public:
    Qt::DockWidgetArea area;
    QToolBar *toolBar;
    QAction  *spacerAct;
    ToolDockWidget *dock1;
    ToolDockWidget *dock2;
    QMap<QAction*,QWidget*> m_actionWidgetMap;
    bool bHideToolBar;
};

struct ActionState
{
    QWidget *widget;
    QList<QAction*> widgetActions;
    Qt::DockWidgetArea area;
    bool    split;
    QString id;
    QString  title;
};

struct InitToolSate
{
    Qt::DockWidgetArea area;
    bool               split;
    bool               checked;
};


class DockWindowStyle : public IWindowStyle
{
    Q_OBJECT
public:
    DockWindowStyle(QSize iconSize, QMainWindow *window, QObject *parent = 0);
    ~DockWindowStyle();
    QAction *addToolWindow(LiteApi::IApplication *app, Qt::DockWidgetArea area, QWidget *widget, const QString &id, const QString &title, bool split = false, QList<QAction*> widgetActions = QList<QAction*>());
    void removeToolWindow(QAction *action);
    QAction *findToolWindow(QWidget *wiget);
    QByteArray saveToolState(int version = 0) const;
    bool loadInitToolState(const QByteArray &state, int version = 0);
    bool restoreState(const QByteArray &state, int version = 0);
public slots:
    void hideToolWindow(Qt::DockWidgetArea area = Qt::BottomDockWidgetArea);
    void showOrHideToolWindow();
    void hideAllToolWindows();
    void restoreToolWindows();
    void hideSideBar(bool b);
    void moveToolWindow(Qt::DockWidgetArea area, QAction *action,bool split = false);
protected slots:
    void toggledAction(bool);
protected:
    QMainWindow *m_mainWindow;
    QMap<Qt::DockWidgetArea,ActionToolBar*> m_areaToolBar;
    QMap<QAction*,ActionState*> m_actStateMap;
    QMap<QString,InitToolSate> m_initIdStateMap;
    QList<QString> m_initCheckedList;
    QStatusBar  *m_statusBar;
    QAction     *m_hideSideAct;
    QList<QAction*> m_hideActionList;
};

#endif // DOCKWINDOWSTYLE_H
