#ifndef GOLANGASTOPTIONFACTORY_H
#define GOLANGASTOPTIONFACTORY_H

#include "liteapi/liteapi.h"

class GolangAstOptionFactory : public LiteApi::IOptionFactory
{
public:
    GolangAstOptionFactory(LiteApi::IApplication *app, QObject *parent);
    virtual QStringList mimeTypes() const;
    virtual LiteApi::IOption *create(const QString &mimeType);
protected:
    LiteApi::IApplication *m_liteApp;
};

#endif // GOLANGASTOPTIONFACTORY_H
