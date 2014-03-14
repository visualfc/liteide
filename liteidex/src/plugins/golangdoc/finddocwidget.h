#ifndef FINDDOCWIDGET_H
#define FINDDOCWIDGET_H

#include "liteapi/liteapi.h"
#include "qtc_editutil/filterlineedit.h"
#include "processex/processex.h"

class QPlainTextEdit;
class FindDocWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FindDocWidget(LiteApi::IApplication *app, QWidget *parent = 0);
    ~FindDocWidget();
signals:

protected slots:
    void findDoc();
    void extOutput(QByteArray,bool);
    void extFinish(bool,int,QString);
protected:
    LiteApi::IApplication *m_liteApp;
    Utils::FilterLineEdit  *m_findEdit;
    bool    m_bUseRegexp;
    QAction* m_useRegexpCheckAct;
    ProcessEx *m_process;
    QPlainTextEdit *m_browser;
};

#endif // FINDDOCWIDGET_H
