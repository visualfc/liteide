#include "terminal_global.h"
#include "terminaloption.h"
#include "terminaloptionfactory.h"

TerminalOptionFactory::TerminalOptionFactory(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IOptionFactory(parent),
      m_liteApp(app)
{
}

QStringList TerminalOptionFactory::mimeTypes() const
{
    return QStringList() << OPTION_TERMIANL;
}

LiteApi::IOption *TerminalOptionFactory::create(const QString &mimeType)
{
    if (mimeType == OPTION_TERMIANL) {
        return new TerminalOption(m_liteApp,this);
    }
    return 0;
}
