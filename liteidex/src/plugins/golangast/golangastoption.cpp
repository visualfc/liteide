#include "golangastoption.h"
#include "ui_golangastoption.h"
#include "golangast_global.h"

GolangAstOption::GolangAstOption(LiteApi::IApplication *app,QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::GolangAstOption)
{
    ui->setupUi(m_widget);
    ui->checkQuickSymbolImportPath->setChecked(m_liteApp->settings()->value(GOLANGAST_QUICKOPNE_SYMBOL_IMPORTPATH,true).toBool());
    ui->checkQuickSymbolMatchCase->setChecked(m_liteApp->settings()->value(GOLANGAST_QUICKOPEN_SYMBOL_MATCHCASE,false).toBool());
}

GolangAstOption::~GolangAstOption()
{
    delete m_widget;
    delete ui;
}

QWidget *GolangAstOption::widget()
{
    return m_widget;
}

QString GolangAstOption::name() const
{
    return "GolangAst";
}

QString GolangAstOption::mimeType() const
{
    return OPTION_GOLANGAST;
}
void GolangAstOption::apply()
{
    m_liteApp->settings()->setValue(GOLANGAST_QUICKOPNE_SYMBOL_IMPORTPATH,ui->checkQuickSymbolImportPath->isChecked());
    m_liteApp->settings()->setValue(GOLANGAST_QUICKOPEN_SYMBOL_MATCHCASE,ui->checkQuickSymbolMatchCase->isChecked());
}
