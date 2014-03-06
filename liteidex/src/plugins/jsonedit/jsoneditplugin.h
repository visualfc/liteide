#ifndef JSONEDITPLUGIN_H
#define JSONEDITPLUGIN_H

#include "jsonedit_global.h"
#include "liteapi/liteapi.h"

class JsonEditPlugin : public LiteApi::IPlugin
{
    Q_OBJECT
public:
    JsonEditPlugin();
    virtual bool load(LiteApi::IApplication *app);
public slots:
    void editorCreated(LiteApi::IEditor*);
protected:
    LiteApi::IApplication *m_liteApp;
};

class PluginFactory : public LiteApi::PluginFactoryT<JsonEditPlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "liteidex.JsonEditPlugin")
#endif
public:
    PluginFactory() {
        m_info->setId("plugin/JsonEdit");
        m_info->setVer("x21");
        m_info->setName("JsonEdit");
        m_info->setAuthor("visualfc");
        m_info->setInfo("JsonEdit");
        m_info->appendDepend("plugin/liteeditor");
    }
};


#endif // JSONEDITPLUGIN_H
