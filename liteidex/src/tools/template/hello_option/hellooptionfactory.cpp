#include "hellooption.h"
#include "hellooptionfactory.h"

HelloOptionFactory::HelloOptionFactory(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IOptionFactory(parent),
      m_liteApp(app)
{
}

QStringList HelloOptionFactory::mimeTypes() const
{
    return QStringList() << "option/hello";
}

LiteApi::IOption *HelloOptionFactory::create(const QString &mimeType)
{
    if (mimeType == "option/hello") {
        return new HelloOption(m_liteApp,this);
    }
    return 0;
}
