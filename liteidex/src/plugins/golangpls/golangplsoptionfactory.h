#ifndef GOLANGPLSOPTIONFACTORY_H
#define GOLANGPLSOPTIONFACTORY_H

#include "liteapi/liteapi.h"

class GolangPlsOptionFactory : public LiteApi::IOptionFactory
{
public:
    GolangPlsOptionFactory(LiteApi::IApplication *app, QObject *parent);
    virtual QStringList mimeTypes() const;
    virtual LiteApi::IOption *create(const QString &mimeType);
protected:
    LiteApi::IApplication *m_liteApp;
};

#endif // GOLANGPLSOPTIONFACTORY_H
