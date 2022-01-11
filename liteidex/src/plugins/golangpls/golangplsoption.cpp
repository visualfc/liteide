#include "golangplsoption.h"
#include "ui_golangplsoption.h"

#include "golangpls_global.h"

GolangPlsOption::GolangPlsOption(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::GolangPlsOption)
{
    ui->setupUi(m_widget);
}

GolangPlsOption::~GolangPlsOption()
{
    delete ui;
}

QWidget *GolangPlsOption::widget()
{
    return m_widget;
}

QString GolangPlsOption::name() const
{
    return "GolangPls";
}

QString GolangPlsOption::mimeType() const
{
    return OPTION_GOLANGPLS;
}

void GolangPlsOption::save()
{
    m_liteApp->settings()->setValue(GOLANGPLS_STATICCHECK_SA1, ui->sa1CheckBox->isChecked());
    m_liteApp->settings()->setValue(GOLANGPLS_STATICCHECK_SA2, ui->sa2CheckBox->isChecked());
    m_liteApp->settings()->setValue(GOLANGPLS_STATICCHECK_SA3, ui->sa3CheckBox->isChecked());
    m_liteApp->settings()->setValue(GOLANGPLS_STATICCHECK_SA4, ui->sa4CheckBox->isChecked());
    m_liteApp->settings()->setValue(GOLANGPLS_STATICCHECK_SA5, ui->sa5CheckBox->isChecked());
    m_liteApp->settings()->setValue(GOLANGPLS_STATICCHECK_SA6, ui->sa6CheckBox->isChecked());
    m_liteApp->settings()->setValue(GOLANGPLS_STATICCHECK_SA9, ui->sa9CheckBox->isChecked());
    m_liteApp->settings()->setValue(GOLANGPLS_STATICCHECK_S1, ui->s1CheckBox->isChecked());
    m_liteApp->settings()->setValue(GOLANGPLS_STATICCHECK_ST1, ui->st1CheckBox->isChecked());
    m_liteApp->settings()->setValue(GOLANGPLS_STATICCHECK_QF1, ui->qf1CheckBox->isChecked());
    m_liteApp->settings()->setValue(GOLANGPLS_STATICCHECK_UNREACHABLE, ui->unreachableCheckBox->isChecked());
    m_liteApp->settings()->setValue(GOLANGPLS_STATICCHECK_COMPILER, ui->compilerCheckBox->isChecked());
    m_liteApp->settings()->setValue(GOLANGPLS_STATICCHECK_SYNTAX, ui->syntaxCheckBox->isChecked());
    m_liteApp->settings()->setValue(GOLANGPLS_STATICCHECK_SHADOW, ui->shadowCheckBox->isChecked());
}

void GolangPlsOption::load()
{
    ui->sa1CheckBox->setChecked(m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_SA1, true).toBool());
    ui->sa2CheckBox->setChecked(m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_SA2, true).toBool());
    ui->sa3CheckBox->setChecked(m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_SA3, true).toBool());
    ui->sa4CheckBox->setChecked(m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_SA4, true).toBool());
    ui->sa5CheckBox->setChecked(m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_SA5, true).toBool());
    ui->sa6CheckBox->setChecked(m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_SA6, true).toBool());
    ui->sa9CheckBox->setChecked(m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_SA9, true).toBool());
    ui->s1CheckBox->setChecked(m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_S1, true).toBool());
    ui->st1CheckBox->setChecked(m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_ST1, true).toBool());
    ui->qf1CheckBox->setChecked(m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_QF1, true).toBool());
    ui->unreachableCheckBox->setChecked(m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_UNREACHABLE, true).toBool());
    ui->compilerCheckBox->setChecked(m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_COMPILER, true).toBool());
    ui->syntaxCheckBox->setChecked(m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_SYNTAX, true).toBool());
    ui->shadowCheckBox->setChecked(m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_SHADOW, true).toBool());
}
