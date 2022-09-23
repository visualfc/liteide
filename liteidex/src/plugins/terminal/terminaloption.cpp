#include "terminaloption.h"
#include "ui_terminaloption.h"
#include "terminal_global.h"

TerminalOption::TerminalOption(LiteApi::IApplication *app,QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::TermianlOption)
{
    ui->setupUi(m_widget);
}

TerminalOption::~TerminalOption()
{
    delete m_widget;
    delete ui;
}

QWidget *TerminalOption::widget()
{
    return m_widget;
}

QString TerminalOption::name() const
{
    return "Terminal";
}

QString TerminalOption::mimeType() const
{
    return OPTION_TERMIANL;
}

void TerminalOption::load()
{

}

void TerminalOption::save()
{

}
