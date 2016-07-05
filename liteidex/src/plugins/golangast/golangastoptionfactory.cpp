#include "golangastoption.h"
#include "golangastoptionfactory.h"
#include "golangast_global.h"

GolangAstOptionFactory::GolangAstOptionFactory(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IOptionFactory(parent),
      m_liteApp(app)
{
}

QStringList GolangAstOptionFactory::mimeTypes() const
{
    return QStringList() << OPTION_GOLANGAST;
}

LiteApi::IOption *GolangAstOptionFactory::create(const QString &mimeType)
{
    if (mimeType == OPTION_GOLANGAST) {
        return new GolangAstOption(m_liteApp,this);
    }
    return 0;
}
