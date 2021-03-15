#include "golangplsoption.h"

#include <QWidget>

GolangPlsOption::GolangPlsOption(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IOption(parent)
    , m_liteApp(app)
{

}


QWidget *GolangPlsOption::widget()
{
    return new QWidget();
}

QString GolangPlsOption::name() const
{
    return "GolangPls";
}

QString GolangPlsOption::mimeType() const
{
    return "option/golangpls";
}

void GolangPlsOption::save()
{
}

void GolangPlsOption::load()
{
}
