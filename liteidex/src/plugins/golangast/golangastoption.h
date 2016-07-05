#ifndef GOLANGASTOPTION_H
#define GOLANGASTOPTION_H

#include "liteapi/liteapi.h"

namespace Ui {
    class GolangAstOption;
}

class GolangAstOption : public LiteApi::IOption
{
    Q_OBJECT

public:
    explicit GolangAstOption(LiteApi::IApplication *app, QObject *parent = 0);
    ~GolangAstOption();
    virtual QWidget *widget();
    virtual QString name() const;
    virtual QString mimeType() const;
    virtual void apply();
private:
    LiteApi::IApplication   *m_liteApp;
    QWidget           *m_widget;
    Ui::GolangAstOption *ui;
};

#endif // GOLANGASTOPTION_H
