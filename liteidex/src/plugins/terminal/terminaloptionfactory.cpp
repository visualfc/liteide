#include "terminal_global.h"
#include "terminaloption.h"
#include "terminaloptionfactory.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

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
