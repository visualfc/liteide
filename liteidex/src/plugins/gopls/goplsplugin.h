#ifndef GOPLSPLUGIN_H
#define GOPLSPLUGIN_H

#include "liteapi/liteapi.h"
#include <QHash>
#include <QSet>
#include <QTextCursor>
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
    void editorCreated(LiteApi::IEditor *editor);
    void editorAboutToClose(LiteApi::IEditor *editor);
    void editorSaved(LiteApi::IEditor *editor);
    void editorContentsChanged();
    void completionRequested(QTextCursor cursor, QString prefix, bool force);
    void clientResponse(int id, const QString &method, const QJsonValue &result, const QJsonObject &error);

private:
    QString workspaceRoot() const;
    void setLegacyCompletionEnabled(bool enabled);
    bool isGoEditor(LiteApi::IEditor *editor) const;
    QString documentUri(LiteApi::IEditor *editor) const;
    void openDocument(LiteApi::IEditor *editor);
    void changeDocument(LiteApi::IEditor *editor);
    void configureCompleter(LiteApi::IEditor *editor, bool enabled);
    QString completionKind(int kind) const;

    LiteApi::IApplication *m_liteApp;
    GoplsClient *m_client;
    QHash<LiteApi::IEditor*,int> m_documentVersions;
    QSet<LiteApi::IEditor*> m_openDocuments;
    bool m_ready;
    QHash<QObject*,LiteApi::IEditor*> m_completerEditors;
    QHash<int,LiteApi::IEditor*> m_completionEditors;
    QHash<int,QString> m_completionPrefixes;
    QHash<int,QString> m_completionRoots;
    QHash<LiteApi::IEditor*,int> m_pendingCompletions;
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
