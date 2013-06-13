#include "litettyplugin.h"
#include <QtPlugin>

LiteTtyPlugin::LiteTtyPlugin()
{
}

bool LiteTtyPlugin::load(LiteApi::IApplication *app)
{
    return true;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(PluginFactory,PluginFactory)
#endif
