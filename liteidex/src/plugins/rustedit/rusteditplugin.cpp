#include "rusteditplugin.h"
#include "rustedit.h"
#include <QtPlugin>

RustEditPlugin::RustEditPlugin()
{
}

bool RustEditPlugin::load(LiteApi::IApplication *app)
{
    new RustEdit(app,this);
    return true;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(PluginFactory,PluginFactory)
#endif
