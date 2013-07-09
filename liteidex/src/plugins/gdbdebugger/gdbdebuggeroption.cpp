#include "gdbdebuggeroption.h"
#include "ui_gdbdebuggeroption.h"
#include "gdbdebugger_global.h"

GdbDebuggerOption::GdbDebuggerOption(LiteApi::IApplication *app,QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::GdbDebuggerOption)
{
    ui->setupUi(m_widget);
    ui->useTtyCheckBox->setChecked(isGdbDebuggerUseTty);
}

GdbDebuggerOption::~GdbDebuggerOption()
{
    delete m_widget;
    delete ui;
}

QWidget *GdbDebuggerOption::widget()
{
    return m_widget;
}

QString GdbDebuggerOption::name() const
{
    return "GdbDebugger";
}

QString GdbDebuggerOption::mimeType() const
{
    return OPTION_GDBDEBUGGER;
}
void GdbDebuggerOption::apply()
{
    m_liteApp->settings()->setValue(GDBDEBUGGER_USETTY,ui->useTtyCheckBox->isChecked());
}


bool isGdbDebuggerUseTty(LiteApi::IApplication *app)
{
#ifdef Q_OS_WIN
    return app->settings()->value(GDBDEBUGGER_USETTY,true).toBool();
#else
    return app->settings()->value(GDBDEBUGGER_USETTY,false).toBool();
#endif
}
