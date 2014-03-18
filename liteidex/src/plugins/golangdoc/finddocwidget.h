#ifndef FINDDOCWIDGET_H
#define FINDDOCWIDGET_H

#include "liteapi/liteapi.h"
#include "qtc_editutil/filterlineedit.h"
#include "qtc_editutil/fancylineedit.h"
#include "qt_browser/chasewidget.h"
#include "processex/processex.h"

class SearchEdit;
class QTextBrowser;

class FindDocWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FindDocWidget(LiteApi::IApplication *app, QWidget *parent = 0);
    ~FindDocWidget();
protected slots:
    void findDoc();
    void extOutput(QByteArray,bool);    
    void extFinish(bool,int,QString);
    void abortFind();
    void stateChanged(QProcess::ProcessState);
    void openUrl(QUrl);
protected:
    QStringList parserDoc(QString findText);
    QStringList parserPkgDoc(QString findText);
    LiteApi::IApplication *m_liteApp;
    SearchEdit            *m_findEdit;
    ChaseWidget           *m_chaseWidget;
    QAction* m_useRegexpCheckAct;
    ProcessEx *m_process;
    LiteApi::IHtmlWidget *m_browser;
    QString    m_findFlag;
    QString    m_templateData;
    QString    m_htmlData;
};

#endif // FINDDOCWIDGET_H
