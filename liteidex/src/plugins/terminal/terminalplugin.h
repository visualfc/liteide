#ifndef TERMINALPLUGIN_H
#define TERMINALPLUGIN_H

#include "terminal_global.h"
#include "liteapi/liteapi.h"

class TerminalPlugin : public LiteApi::IPlugin
{
public:
    TerminalPlugin();
    virtual bool load(LiteApi::IApplication *app);
};

class PluginFactory : public LiteApi::PluginFactoryT<TerminalPlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "liteidex.TerminalPlugin")
#endif
public:
    PluginFactory() {
        m_info->setId("plugin/Terminal");
        m_info->setVer("X37");
        m_info->setName("Terminal");
        m_info->setAuthor("visualfc");
        m_info->setInfo("Terminal");
        m_info->appendDepend("plugin/liteenv");
    }
};


#endif // TERMINALPLUGIN_H
