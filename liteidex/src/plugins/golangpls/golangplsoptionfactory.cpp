#include "golangplsoptionfactory.h"
#include "golangplsoption.h"

GolangPlsOptionFactory::GolangPlsOptionFactory(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IOptionFactory(parent)
    , m_liteApp(app)
{

}

QStringList GolangPlsOptionFactory::mimeTypes() const
{
    return QStringList() << "option/golangpls";
}

LiteApi::IOption *GolangPlsOptionFactory::create(const QString &mimeType)
{
    if (mimeType == "option/golangpls") {
        return new GolangPlsOption(m_liteApp,this);
    }
    return 0;
}
