#include "hellooption.h"
#include "ui_hellooption.h"
#include "hello_global.h"

HelloOption::HelloOption(LiteApi::IApplication *app,QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::HelloOption)
{
    ui->setupUi(m_widget);
}

HelloOption::~HelloOption()
{
    delete m_widget;
    delete ui;
}

QWidget *HelloOption::widget()
{
    return m_widget;
}

QString HelloOption::name() const
{
    return "Hello";
}

QString HelloOption::mimeType() const
{
    return OPTION_HELLO;
}
void HelloOption::apply()
{
}
