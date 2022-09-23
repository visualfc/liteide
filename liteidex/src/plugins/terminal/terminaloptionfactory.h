#ifndef TERMINALOPTIONFACTORY_H
#define TERMINALOPTIONFACTORY_H

#include "liteapi/liteapi.h"

class TerminalOptionFactory : public LiteApi::IOptionFactory
{
public:
    TerminalOptionFactory(LiteApi::IApplication *app, QObject *parent);
    virtual QStringList mimeTypes() const;
    virtual LiteApi::IOption *create(const QString &mimeType);
protected:
    LiteApi::IApplication *m_liteApp;
};

#endif // TERMINALOPTIONFACTORY_H
