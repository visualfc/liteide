#ifndef RUSTEDITPLUGIN_H
#define RUSTEDITPLUGIN_H

#include "rustedit_global.h"
#include "liteapi/liteapi.h"

class RustEditPlugin : public LiteApi::IPlugin
{
public:
    RustEditPlugin();
    virtual bool load(LiteApi::IApplication *app);
};

class PluginFactory : public LiteApi::PluginFactoryT<RustEditPlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "liteidex.RustEditPlugin")
#endif
public:
    PluginFactory() {
        m_info->setId("plugin/RustEdit");
        m_info->setVer("x26.1");
        m_info->setName("RustEdit");
        m_info->setAuthor("visualfc");
        m_info->setInfo("RustEdit");
        m_info->appendDepend("plugin/liteeditor");
    }
};


#endif // RUSTEDITPLUGIN_H
