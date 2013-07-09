#ifndef GDBDEBUGGEROPTIONFACTORY_H
#define GDBDEBUGGEROPTIONFACTORY_H

#include "liteapi/liteapi.h"

class GdbDebuggerOptionFactory : public LiteApi::IOptionFactory
{
public:
    GdbDebuggerOptionFactory(LiteApi::IApplication *app, QObject *parent);
    virtual QStringList mimeTypes() const;
    virtual LiteApi::IOption *create(const QString &mimeType);
protected:
    LiteApi::IApplication *m_liteApp;
};

#endif // GDBDEBUGGEROPTIONFACTORY_H
