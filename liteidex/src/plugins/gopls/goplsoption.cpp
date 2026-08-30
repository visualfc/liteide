#include "goplsoption.h"
#include "gopls_global.h"
#include "ui_goplsoption.h"

GoplsOption::GoplsOption(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IOption(parent), m_liteApp(app), m_widget(new QWidget), ui(new Ui::GoplsOption)
{
    ui->setupUi(m_widget);
}

GoplsOption::~GoplsOption()
{
    delete ui;
    delete m_widget;
}

QWidget *GoplsOption::widget() { return m_widget; }
QString GoplsOption::name() const { return "Gopls"; }
QString GoplsOption::mimeType() const { return OPTION_GOPLS; }

void GoplsOption::load()
{
    ui->enableCheckBox->setChecked(m_liteApp->settings()->value(GOPLS_USE_FEATURES, false).toBool());
}

void GoplsOption::save()
{
    m_liteApp->settings()->setValue(GOPLS_USE_FEATURES, ui->enableCheckBox->isChecked());
}
