#ifndef FINDDOCWIDGET_H
#define FINDDOCWIDGET_H

#include "liteapi/liteapi.h"
#include "qtc_editutil/filterlineedit.h"

class FindDocWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FindDocWidget(LiteApi::IApplication *app, QWidget *parent = 0);

signals:

public slots:
protected:
    LiteApi::IApplication *m_liteApp;
    Utils::FilterLineEdit  *m_findEdit;
};

#endif // FINDDOCWIDGET_H
