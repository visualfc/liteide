#ifndef GOPLSOPTIONFACTORY_H
#define GOPLSOPTIONFACTORY_H

#include "liteapi/liteapi.h"

class GoplsOptionFactory : public LiteApi::IOptionFactory
{
    Q_OBJECT
public:
    explicit GoplsOptionFactory(LiteApi::IApplication *app, QObject *parent = 0);
    QStringList mimeTypes() const;
    LiteApi::IOption *create(const QString &mimeType);
private:
    LiteApi::IApplication *m_liteApp;
};

#endif // GOPLSOPTIONFACTORY_H
