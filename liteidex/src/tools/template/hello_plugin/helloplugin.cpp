#include "helloplugin.h"
#include <QtPlugin>

HelloPlugin::HelloPlugin()
{
    m_info->setId("plugin/Hello");
    m_info->setName("Hello");
    m_info->setAnchor("visualfc");
    m_info->setInfo("Hello Plugin");
}

bool HelloPlugin::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IPlugin::initWithApp(app)) {
        return false;
    }
    return true;
}

Q_EXPORT_PLUGIN(PluginFactory)
