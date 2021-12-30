#ifndef GOLANPLSPLUGIN_H
#define GOLANPLSPLUGIN_H

#include "liteapi/liteapi.h"
#include <QtPlugin>

class GolangPls;
class GolanPlsPlugin : public LiteApi::IPlugin
{
    Q_OBJECT
public:
    GolanPlsPlugin();
    virtual bool load(LiteApi::IApplication *app);
    virtual QStringList dependPluginList() const;
protected slots:
    void editorCreated(LiteApi::IEditor*editor);
    void currentEditorChanged(LiteApi::IEditor*editor);
protected:
    LiteApi::IApplication *m_liteApp;
    GolangPls *m_pls;
};

class PluginFactory : public LiteApi::PluginFactoryT<GolanPlsPlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "liteidex.GolangPlsPlugin")
#endif
public:
    PluginFactory() {
        m_info->setId("plugin/golangpls");
        m_info->setName("GolangPls");
        m_info->setAuthor("JL");
        m_info->setVer("X36");
        m_info->setInfo("Golang PLS Support");
    }
};
#endif // GOLANPLSPLUGIN_H
