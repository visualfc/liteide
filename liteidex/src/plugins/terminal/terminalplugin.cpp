#include "terminalplugin.h"
#include <QtPlugin>

TerminalPlugin::TerminalPlugin()
{
}

bool TerminalPlugin::load(LiteApi::IApplication *app)
{
    return true;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(PluginFactory,PluginFactory)
#endif
