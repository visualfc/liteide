#ifndef HELLOPLUGIN_H
#define HELLOPLUGIN_H

#include "hello_global.h"
#include "liteapi/liteapi.h"

class HelloPlugin : public LiteApi::IPlugin
{
public:
    HelloPlugin();
    virtual bool load(LiteApi::IApplication *app);
};

class PluginFactory : public LiteApi::PluginFactoryT<HelloPlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "liteidex.HelloPlugin")
#endif
public:
    PluginFactory() {
        m_info->setId("plugin/Hello");
        m_info->setVer("x18");
        m_info->setName("Hello");
        m_info->setAnchor("visualfc");
        m_info->setInfo("Hello");
        //m_info->appendDepend("plugin/liteenv");
    }
};


#endif // HELLOPLUGIN_H
