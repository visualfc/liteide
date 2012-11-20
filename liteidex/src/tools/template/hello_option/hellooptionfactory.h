#ifndef HELLOOPTIONFACTORY_H
#define HELLOOPTIONFACTORY_H

#include "liteapi.h"

class HelloOptionFactory : public LiteApi::IOptionFactory
{
public:
    HelloOptionFactory(LiteApi::IApplication *app, QObject *parent);
    virtual QStringList mimeTypes() const;
    virtual LiteApi::IOption *create(const QString &mimeType);
protected:
    LiteApi::IApplication *m_liteApp;
};

#endif // HELLOOPTIONFACTORY_H
