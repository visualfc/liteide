#include "gdbdebuggeroption.h"
#include "gdbdebuggeroptionfactory.h"
#include "gdbdebugger_global.h"

GdbDebuggerOptionFactory::GdbDebuggerOptionFactory(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IOptionFactory(parent),
      m_liteApp(app)
{
}

QStringList GdbDebuggerOptionFactory::mimeTypes() const
{
    return QStringList() << OPTION_GDBDEBUGGER;
}

LiteApi::IOption *GdbDebuggerOptionFactory::create(const QString &mimeType)
{
    if (mimeType == OPTION_GDBDEBUGGER) {
        return new GdbDebuggerOption(m_liteApp,this);
    }
    return 0;
}
