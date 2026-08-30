#ifndef GOPLSPLUGIN_H
#define GOPLSPLUGIN_H

#include "liteapi/liteapi.h"
#include "liteenvapi/liteenvapi.h"
#include <QHash>
#include <QSet>
#include <QTextCursor>
#include <QPoint>
#include <QtPlugin>

class GoplsClient;
class GoplsSearchResults;
class QAction;
class QEvent;
namespace LiteApi { class ICompleter; }

class GoplsPlugin : public LiteApi::IPlugin
{
    Q_OBJECT
public:
    GoplsPlugin();
    ~GoplsPlugin();
    virtual bool load(LiteApi::IApplication *app);
    virtual QStringList dependPluginList() const;

protected:
    bool eventFilter(QObject *object, QEvent *event);

private slots:
    void appLoaded();
    void clientInitialized();
    void clientStopped();
    void clientLog(const QString &message, bool error);
    void editorCreated(LiteApi::IEditor *editor);
    void currentEditorChanged(LiteApi::IEditor *editor);
    void editorAboutToClose(LiteApi::IEditor *editor);
    void editorSaved(LiteApi::IEditor *editor);
    void editorContentsChanged();
    void completionRequested(QTextCursor cursor, QString prefix, bool force);
    void clientResponse(int id, const QString &method, const QJsonValue &result, const QJsonObject &error);
    void goToDefinition();
    void findReferences();
    void findImplementations();
    void clientNotification(const QString &method, const QJsonValue &params);
    void hoverRequested(const QTextCursor &cursor, const QPoint &position, bool navigation);
    void serverRequest(int id, const QString &method, const QJsonValue &params);
    void renameSymbol();
    void formatDocument();
    void organizeImports();
    void completionAccepted(const QString &text, const QString &kind, const QString &info);
    void workspaceChanged();
    void environmentChanged(LiteApi::IEnv *environment);

private:
    QString workspaceRoot() const;
    void setLegacyCompletionEnabled(bool enabled);
    bool isGoEditor(LiteApi::IEditor *editor) const;
    QString documentUri(LiteApi::IEditor *editor) const;
    void openDocument(LiteApi::IEditor *editor);
    void changeDocument(LiteApi::IEditor *editor);
    void configureCompleter(LiteApi::IEditor *editor, bool enabled);
    QString completionKind(int kind) const;
    void handleEditResponse(int id, const QString &method, const QJsonValue &result, const QJsonObject &error);
    void handleLocationResponse(int id, const QJsonValue &result, const QJsonObject &error);
    void handleHintResponse(int id, const QString &method, const QJsonValue &result, const QJsonObject &error);
    void handleCompletionResponse(int id, const QJsonValue &result, const QJsonObject &error);
    void requestLocations(const QString &method);
    void addEditorActions(LiteApi::IEditor *editor);
    void requestSignatureHelp(LiteApi::IEditor *editor);
    void requestHover(LiteApi::IEditor *editor, const QTextCursor &cursor,
                      const QPoint &position);
    QString markupText(const QJsonValue &value) const;
    void applyWorkspaceEdit(const QJsonObject &workspaceEdit);
    void applyTextEdits(const QString &uri, QJsonArray edits);

    LiteApi::IApplication *m_liteApp;
    GoplsClient *m_client;
    LiteApi::ICompleter *m_completer;
    QHash<LiteApi::IEditor*,int> m_documentVersions;
    QSet<LiteApi::IEditor*> m_openDocuments;
    bool m_ready;
    QHash<int,LiteApi::IEditor*> m_completionEditors;
    QHash<int,QString> m_completionPrefixes;
    QHash<int,QString> m_completionRoots;
    QHash<LiteApi::IEditor*,int> m_pendingCompletions;
    QHash<int,QString> m_locationRequests;
    QHash<int,QString> m_locationSearchText;
    GoplsSearchResults *m_searchResults;
    QAction *m_definitionAction;
    QAction *m_referencesAction;
    QAction *m_implementationAction;
    QHash<int,LiteApi::IEditor*> m_hintEditors;
    QHash<int,QPoint> m_hintPositions;
    QHash<LiteApi::IEditor*,int> m_pendingHovers;
    QHash<QObject*,LiteApi::IEditor*> m_hoverViewports;
    QAction *m_renameAction;
    QAction *m_formatAction;
    QAction *m_organizeImportsAction;
    QHash<int,QString> m_editRequests;
    QHash<int,LiteApi::IEditor*> m_editRequestEditors;
    QHash<QObject*,QHash<QString,QJsonArray> > m_completionAdditionalEdits;
    bool m_appLoaded;
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
        m_info->appendDepend("plugin/litefind");
    }
};

#endif // GOPLSPLUGIN_H
