#ifndef FINDAPIWIDGET_H
#define FINDAPIWIDGET_H

#include "liteapi/liteapi.h"
#include "qtc_editutil/fancylineedit.h"
#include "qt_browser/chasewidget.h"
#include "processex/processex.h"
#include <QWidget>
#include <QThread>
#include <QModelIndex>

class QListView;
class QStandardItemModel;
class FindApiThread : public QThread
{
    Q_OBJECT
public:
    FindApiThread(QObject *parent);
    ~FindApiThread();
signals:
    void findApiOut(const QString &api, const QString &text, const QStringList &url);
public:
    void setFileName(const QString &fileName);
    void findApi(const QString &text);
    void stopFind();
    void setMatchCase(bool b);
protected:
    virtual void	run ();
    QString m_text;
    QString m_fileName;    
    bool    m_bMatchCase;
};

class FindApiEdit : public Utils::FancyLineEdit
{
public:
    FindApiEdit(QWidget *parent = 0)
        : Utils::FancyLineEdit(parent)
    {
        QIcon icon = QIcon::fromTheme(layoutDirection() == Qt::LeftToRight ?
                         QLatin1String("edit-clear-locationbar-rtl") :
                         QLatin1String("edit-clear-locationbar-ltr"),
                         QIcon::fromTheme(QLatin1String("edit-clear"), QIcon(QLatin1String("icon:images/editclear.png"))));

        setButtonPixmap(Right, icon.pixmap(16));
        setPlaceholderText(tr("Search"));
        setButtonToolTip(Right, tr("Stop Search"));
    }
    void showStopButton(bool b)
    {
        this->setButtonVisible(Right,b);
    }
};

class FindApiWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FindApiWidget(LiteApi::IApplication *app,QWidget *parent = 0);
    ~FindApiWidget();
signals:
    void openApiUrl(QStringList);
public slots:
    void findApi();
    void findApiOut(QString,QString,QStringList);
    void findApiFinished();
    void findApiTerminated();
    void doubleClickedApi(QModelIndex);
    void rebuildApiData();
public:
    QString apiDataFile() const;
protected:
    FindApiThread *m_findThread;
    LiteApi::IApplication *m_liteApp;
    FindApiEdit           *m_findEdit;
    ChaseWidget           *m_chaseWidget;
    QListView             *m_listView;
    QStandardItemModel    *m_model;
    ProcessEx             *m_rebuildThread;
    QAction               *m_findAct;
    QAction               *m_caseCheckAct;
    QAction               *m_rebuildAct;
};

#endif // FINDAPIWIDGET_H
