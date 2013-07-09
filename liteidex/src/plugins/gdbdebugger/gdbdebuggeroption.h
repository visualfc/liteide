#ifndef GDBDEBUGGEROPTION_H
#define GDBDEBUGGEROPTION_H

#include "liteapi/liteapi.h"

namespace Ui {
    class GdbDebuggerOption;
}

class GdbDebuggerOption : public LiteApi::IOption
{
    Q_OBJECT

public:
    explicit GdbDebuggerOption(LiteApi::IApplication *app, QObject *parent = 0);
    ~GdbDebuggerOption();
    virtual QWidget *widget();
    virtual QString name() const;
    virtual QString mimeType() const;
    virtual void apply();
private:
    LiteApi::IApplication   *m_liteApp;
    QWidget           *m_widget;
    Ui::GdbDebuggerOption *ui;
};

bool isGdbDebuggerUseTty(LiteApi::IApplication *app);

#endif // GDBDEBUGGEROPTION_H
