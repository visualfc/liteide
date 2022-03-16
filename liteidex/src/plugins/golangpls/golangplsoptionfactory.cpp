#include "golangplsoptionfactory.h"
#include "golangplsoption.h"

#include "golangpls_global.h"

GolangPlsOptionFactory::GolangPlsOptionFactory(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IOptionFactory(parent),
      m_liteApp(app)
{
}

QStringList GolangPlsOptionFactory::mimeTypes() const
{
    return QStringList() << OPTION_GOLANGPLS;
}

LiteApi::IOption *GolangPlsOptionFactory::create(const QString &mimeType)
{
    if (mimeType == OPTION_GOLANGPLS) {
        return new GolangPlsOption(m_liteApp,this);
    }
    return 0;
}
