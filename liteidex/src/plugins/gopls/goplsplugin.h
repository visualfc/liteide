#ifndef GOPLSPLUGIN_H
#define GOPLSPLUGIN_H

#include "liteapi/liteapi.h"
#include <QtPlugin>

class GoplsClient;

class GoplsPlugin : public LiteApi::IPlugin
{
    Q_OBJECT
public:
    GoplsPlugin();
    ~GoplsPlugin();
    virtual bool load(LiteApi::IApplication *app);
    virtual QStringList dependPluginList() const;

private slots:
    void appLoaded();
    void clientInitialized();
    void clientStopped();
    void clientLog(const QString &message, bool error);

private:
    QString workspaceRoot() const;
    void setLegacyCompletionEnabled(bool enabled);

    LiteApi::IApplication *m_liteApp;
    GoplsClient *m_client;
};

class PluginFactory : public LiteApi::PluginFactoryT<GoplsPlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "liteidex.GoplsPlugin")
#endif
public:
    PluginFactory() {
        m_info->setId("plugin/gopls");
        m_info->setName("Gopls");
        m_info->setAuthor("visualfc");
        m_info->setVer("X38.5");
        m_info->setInfo("Go language server support");
        m_info->appendDepend("plugin/liteenv");
        m_info->appendDepend("plugin/golangast");
    }
};

#endif // GOPLSPLUGIN_H
