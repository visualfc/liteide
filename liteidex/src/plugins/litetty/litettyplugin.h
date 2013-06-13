#ifndef LITETTYPLUGIN_H
#define LITETTYPLUGIN_H

#include "litetty_global.h"
#include "liteapi/liteapi.h"

class LiteTtyPlugin : public LiteApi::IPlugin
{
public:
    LiteTtyPlugin();
    virtual bool load(LiteApi::IApplication *app);
};

class PluginFactory : public LiteApi::PluginFactoryT<LiteTtyPlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "liteidex.LiteTtyPlugin")
#endif
public:
    PluginFactory() {
        m_info->setId("plugin/LiteTty");
        m_info->setVer("x18");
        m_info->setName("LiteTty");
        m_info->setAnchor("visualfc");
        m_info->setInfo("LiteIDE tty Util");
        //m_info->appendDepend("plugin/liteenv");
    }
};


#endif // LITETTYPLUGIN_H
