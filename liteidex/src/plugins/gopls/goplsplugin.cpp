#include "goplsplugin.h"
#include "goplsclient.h"
#include "liteenvapi/liteenvapi.h"
#include "fileutil/fileutil.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QUrl>

GoplsPlugin::GoplsPlugin() : m_liteApp(0), m_client(0), m_ready(false)
{
}

GoplsPlugin::~GoplsPlugin()
{
    if (m_liteApp && m_liteApp->extension()) {
        m_liteApp->extension()->removeObject("LiteApi.IGoplsService");
    }
    setLegacyCompletionEnabled(true);
}

bool GoplsPlugin::load(LiteApi::IApplication *app)
{
    m_liteApp = app;
    m_client = new GoplsClient(this);
    app->extension()->addObject("LiteApi.IGoplsService",m_client);
    connect(app,SIGNAL(loaded()),this,SLOT(appLoaded()));
    connect(m_client,SIGNAL(initialized()),this,SLOT(clientInitialized()));
    connect(m_client,SIGNAL(stopped()),this,SLOT(clientStopped()));
    connect(m_client,SIGNAL(logMessage(QString,bool)),this,SLOT(clientLog(QString,bool)));
    connect(app->editorManager(),SIGNAL(editorCreated(LiteApi::IEditor*)),this,SLOT(editorCreated(LiteApi::IEditor*)));
    connect(app->editorManager(),SIGNAL(editorAboutToClose(LiteApi::IEditor*)),this,SLOT(editorAboutToClose(LiteApi::IEditor*)));
    connect(app->editorManager(),SIGNAL(editorSaved(LiteApi::IEditor*)),this,SLOT(editorSaved(LiteApi::IEditor*)));
    return true;
}

QStringList GoplsPlugin::dependPluginList() const
{
    return QStringList() << "plugin/liteenv" << "plugin/golangast";
}

QString GoplsPlugin::workspaceRoot() const
{
    LiteApi::IProject *project = m_liteApp->projectManager()->currentProject();
    if (project) {
        QStringList folders = project->folderList();
        if (!folders.isEmpty()) {
            return QFileInfo(folders.first()).absoluteFilePath();
        }
        if (!project->filePath().isEmpty()) {
            QFileInfo info(project->filePath());
            return info.isDir() ? info.absoluteFilePath() : info.absolutePath();
        }
    }
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (editor && !editor->filePath().isEmpty()) {
        return QFileInfo(editor->filePath()).absolutePath();
    }
    return QDir::currentPath();
}

void GoplsPlugin::appLoaded()
{
    QProcessEnvironment environment = LiteApi::getGoEnvironment(m_liteApp);
    QString program = FileUtil::lookupGoBin("gopls",m_liteApp,environment,true);
    if (program.isEmpty()) {
        clientLog(tr("gopls was not found on system PATH (hint: go install golang.org/x/tools/gopls@latest)"),true);
        return;
    }

    QString root = workspaceRoot();
    QJsonObject capabilities;
    QJsonObject textDocument;
    QJsonObject completion;
    QJsonObject completionItem;
    completionItem.insert("snippetSupport",false);
    completion.insert("completionItem",completionItem);
    textDocument.insert("completion",completion);
    textDocument.insert("synchronization",QJsonObject());
    capabilities.insert("textDocument",textDocument);

    QJsonObject params;
    params.insert("processId",qint64(QCoreApplication::applicationPid()));
    params.insert("clientInfo",QJsonObject{{"name","LiteIDE"},{"version",m_liteApp->ideVersion()}});
    params.insert("rootUri",QUrl::fromLocalFile(root).toString());
    params.insert("capabilities",capabilities);
    params.insert("workspaceFolders",QJsonArray{QJsonObject{{"uri",QUrl::fromLocalFile(root).toString()},{"name",QFileInfo(root).fileName()}}});
    m_client->start(program,root,LiteApi::getCustomGoEnvironment(m_liteApp,m_liteApp->editorManager()->currentEditor()),params);
}

void GoplsPlugin::clientInitialized()
{
    m_ready = true;
    m_liteApp->appendLog("Gopls",tr("gopls initialized"));
    setLegacyCompletionEnabled(false);
    foreach (LiteApi::IEditor *editor, m_liteApp->editorManager()->editorList()) {
        editorCreated(editor);
        openDocument(editor);
    }
}

void GoplsPlugin::clientStopped()
{
    m_ready = false;
    m_openDocuments.clear();
    setLegacyCompletionEnabled(true);
}

void GoplsPlugin::clientLog(const QString &message, bool error)
{
    m_liteApp->appendLog("Gopls",message,error);
}

void GoplsPlugin::setLegacyCompletionEnabled(bool enabled)
{
    if (!m_liteApp || !m_liteApp->extension()) {
        return;
    }
    QObject *golangCode = m_liteApp->extension()->findObject("LiteApi.GolangCode");
    if (golangCode) {
        QMetaObject::invokeMethod(golangCode,"setGocodeEnabled",Qt::DirectConnection,Q_ARG(bool,enabled));
    }
}

bool GoplsPlugin::isGoEditor(LiteApi::IEditor *editor) const
{
    return editor && editor->mimeType() == "text/x-gosrc" && !editor->filePath().isEmpty();
}

QString GoplsPlugin::documentUri(LiteApi::IEditor *editor) const
{
    return QUrl::fromLocalFile(QFileInfo(editor->filePath()).absoluteFilePath()).toString();
}

void GoplsPlugin::editorCreated(LiteApi::IEditor *editor)
{
    if (!isGoEditor(editor)) {
        return;
    }
    connect(editor,SIGNAL(contentsChanged()),this,SLOT(editorContentsChanged()),Qt::UniqueConnection);
    if (m_ready) {
        openDocument(editor);
    }
}

void GoplsPlugin::openDocument(LiteApi::IEditor *editor)
{
    if (!isGoEditor(editor) || m_openDocuments.contains(editor)) {
        return;
    }
    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    if (!textEditor) {
        return;
    }
    int version = 1;
    m_documentVersions.insert(editor,version);
    QJsonObject textDocument;
    textDocument.insert("uri",documentUri(editor));
    textDocument.insert("languageId","go");
    textDocument.insert("version",version);
    textDocument.insert("text",QString::fromUtf8(textEditor->utf8Data()));
    m_client->notify("textDocument/didOpen",QJsonObject{{"textDocument",textDocument}});
    m_openDocuments.insert(editor);
}

void GoplsPlugin::changeDocument(LiteApi::IEditor *editor)
{
    if (!m_openDocuments.contains(editor)) {
        openDocument(editor);
        return;
    }
    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    if (!textEditor) {
        return;
    }
    int version = m_documentVersions.value(editor)+1;
    m_documentVersions.insert(editor,version);
    QJsonObject identifier;
    identifier.insert("uri",documentUri(editor));
    identifier.insert("version",version);
    QJsonArray changes;
    changes.append(QJsonObject{{"text",QString::fromUtf8(textEditor->utf8Data())}});
    m_client->notify("textDocument/didChange",QJsonObject{{"textDocument",identifier},{"contentChanges",changes}});
}

void GoplsPlugin::editorContentsChanged()
{
    LiteApi::IEditor *editor = qobject_cast<LiteApi::IEditor*>(sender());
    if (editor && m_ready) {
        changeDocument(editor);
    }
}

void GoplsPlugin::editorSaved(LiteApi::IEditor *editor)
{
    if (!m_openDocuments.contains(editor)) {
        return;
    }
    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    QJsonObject params;
    params.insert("textDocument",QJsonObject{{"uri",documentUri(editor)}});
    if (textEditor) {
        params.insert("text",QString::fromUtf8(textEditor->utf8Data()));
    }
    m_client->notify("textDocument/didSave",params);
}

void GoplsPlugin::editorAboutToClose(LiteApi::IEditor *editor)
{
    if (m_openDocuments.remove(editor)) {
        m_client->notify("textDocument/didClose",QJsonObject{{"textDocument",QJsonObject{{"uri",documentUri(editor)}}}});
    }
    m_documentVersions.remove(editor);
    disconnect(editor,0,this,0);
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(PluginFactory,PluginFactory)
#endif
