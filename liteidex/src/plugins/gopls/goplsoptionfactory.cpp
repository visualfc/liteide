#include "goplsoption.h"
#include "goplsoptionfactory.h"
#include "gopls_global.h"

GoplsOptionFactory::GoplsOptionFactory(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IOptionFactory(parent), m_liteApp(app) {}

QStringList GoplsOptionFactory::mimeTypes() const { return QStringList() << OPTION_GOPLS; }

LiteApi::IOption *GoplsOptionFactory::create(const QString &mimeType)
{
    return mimeType == OPTION_GOPLS ? new GoplsOption(m_liteApp, this) : 0;
}
