#ifndef HELLOOPTION_H
#define HELLOOPTION_H

#include "liteapi/liteapi.h"

namespace Ui {
    class HelloOption;
}

class HelloOption : public LiteApi::IOption
{
    Q_OBJECT

public:
    explicit HelloOption(LiteApi::IApplication *app, QObject *parent = 0);
    ~HelloOption();
    virtual QWidget *widget();
    virtual QString name() const;
    virtual QString mimeType() const;
    virtual void apply();
private:
    LiteApi::IApplication   *m_liteApp;
    QWidget           *m_widget;
    Ui::HelloOption *ui;
};

#endif // HELLOOPTION_H
