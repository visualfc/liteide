#include "goplsplugin.h"
#include "goplsclient.h"
#include "goplssearchresults.h"
#include "liteenvapi/liteenvapi.h"
#include "golangastapi/golangastapi.h"
#include "liteeditorapi/liteeditorapi.h"
#include "fileutil/fileutil.h"

#include <QCoreApplication>
#include <QAction>
#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QStandardItem>
#include <QUrl>

GoplsPlugin::GoplsPlugin() : m_liteApp(0), m_client(0), m_ready(false),
    m_searchResults(0), m_definitionAction(0), m_referencesAction(0), m_implementationAction(0)
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
    m_searchResults = new GoplsSearchResults(this);
    LiteApi::IFileSearchManager *searchManager = LiteApi::getFileSearchManager(app);
    if (searchManager) {
        searchManager->addFileSearch(m_searchResults);
    }
    LiteApi::IActionContext *actions = app->actionManager()->getActionContext(this,"Gopls");
    m_definitionAction = new QAction(tr("Go to Definition (gopls)"),this);
    m_referencesAction = new QAction(tr("Find References (gopls)"),this);
    m_implementationAction = new QAction(tr("Find Implementations (gopls)"),this);
    actions->regAction(m_definitionAction,"Definition","");
    actions->regAction(m_referencesAction,"References","");
    actions->regAction(m_implementationAction,"Implementation","");
    connect(m_definitionAction,SIGNAL(triggered()),this,SLOT(goToDefinition()));
    connect(m_referencesAction,SIGNAL(triggered()),this,SLOT(findReferences()));
    connect(m_implementationAction,SIGNAL(triggered()),this,SLOT(findImplementations()));
    app->extension()->addObject("LiteApi.IGoplsService",m_client);
    connect(app,SIGNAL(loaded()),this,SLOT(appLoaded()));
    connect(m_client,SIGNAL(initialized()),this,SLOT(clientInitialized()));
    connect(m_client,SIGNAL(stopped()),this,SLOT(clientStopped()));
    connect(m_client,SIGNAL(logMessage(QString,bool)),this,SLOT(clientLog(QString,bool)));
    connect(m_client,SIGNAL(response(int,QString,QJsonValue,QJsonObject)),this,SLOT(clientResponse(int,QString,QJsonValue,QJsonObject)));
    connect(m_client,SIGNAL(notification(QString,QJsonValue)),this,SLOT(clientNotification(QString,QJsonValue)));
    connect(app->editorManager(),SIGNAL(editorCreated(LiteApi::IEditor*)),this,SLOT(editorCreated(LiteApi::IEditor*)));
    connect(app->editorManager(),SIGNAL(editorAboutToClose(LiteApi::IEditor*)),this,SLOT(editorAboutToClose(LiteApi::IEditor*)));
    connect(app->editorManager(),SIGNAL(editorSaved(LiteApi::IEditor*)),this,SLOT(editorSaved(LiteApi::IEditor*)));
    return true;
}

QStringList GoplsPlugin::dependPluginList() const
{
    return QStringList() << "plugin/liteenv" << "plugin/golangast" << "plugin/litefind";
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
    foreach (LiteApi::IEditor *editor, m_completerEditors.values()) {
        configureCompleter(editor,false);
    }
    m_completionEditors.clear();
    m_completionPrefixes.clear();
    m_completionRoots.clear();
    m_pendingCompletions.clear();
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
    LiteApi::ILiteEditor *liteEditor = LiteApi::getLiteEditor(editor);
    if (liteEditor) {
        connect(liteEditor,SIGNAL(updateLink(QTextCursor,QPoint,bool)),this,SLOT(hoverRequested(QTextCursor,QPoint,bool)),Qt::UniqueConnection);
    }
    configureCompleter(editor,m_ready);
    addEditorActions(editor);
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
        LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
        if (textEditor) {
            QTextCursor cursor = textEditor->textCursor();
            int pos = cursor.position();
            QString previous = pos > 0 ? textEditor->textAt(pos-1,1) : QString();
            if (previous == "(" || previous == ",") {
                requestSignatureHelp(editor);
            }
        }
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
    int requestId = m_pendingCompletions.take(editor);
    m_completionEditors.remove(requestId);
    m_completionPrefixes.remove(requestId);
    m_completionRoots.remove(requestId);
    QObject *completer = m_completerEditors.key(editor,0);
    if (completer) {
        m_completerEditors.remove(completer);
    }
    disconnect(editor,0,this,0);
}

void GoplsPlugin::configureCompleter(LiteApi::IEditor *editor, bool enabled)
{
    LiteApi::ICompleter *completer = LiteApi::findExtensionObject<LiteApi::ICompleter*>(editor,"LiteApi.ICompleter");
    if (!completer) {
        return;
    }
    disconnect(completer,SIGNAL(prefixChanged(QTextCursor,QString,bool)),this,SLOT(completionRequested(QTextCursor,QString,bool)));
    m_completerEditors.remove(completer);
    if (enabled) {
        completer->setSearchSeparator(false);
        completer->setExternalMode(true);
        connect(completer,SIGNAL(prefixChanged(QTextCursor,QString,bool)),this,SLOT(completionRequested(QTextCursor,QString,bool)),Qt::UniqueConnection);
        m_completerEditors.insert(completer,editor);
    }
}

void GoplsPlugin::completionRequested(QTextCursor cursor, QString prefix, bool force)
{
    if (!m_ready || prefix.isEmpty()) {
        return;
    }
    LiteApi::ICompleter *completer = qobject_cast<LiteApi::ICompleter*>(sender());
    LiteApi::IEditor *editor = m_completerEditors.value(completer,0);
    if (!editor || !m_openDocuments.contains(editor) || completer->completionContext() != LiteApi::CompleterCodeContext) {
        return;
    }
    if (!force && !prefix.endsWith('.')) {
        return;
    }

    int oldId = m_pendingCompletions.value(editor,0);
    if (oldId) {
        m_client->notify("$/cancelRequest",QJsonObject{{"id",oldId}});
        m_completionEditors.remove(oldId);
        m_completionPrefixes.remove(oldId);
        m_completionRoots.remove(oldId);
    }

    QString root;
    int dot = prefix.lastIndexOf('.');
    if (dot >= 0) {
        root = prefix.left(dot+1);
    }
    QJsonObject position{{"line",cursor.blockNumber()},{"character",cursor.positionInBlock()}};
    QJsonObject params{{"textDocument",QJsonObject{{"uri",documentUri(editor)}}},
                       {"position",position},
                       {"context",QJsonObject{{"triggerKind",prefix.endsWith('.') ? 2 : 1},
                                               {"triggerCharacter",prefix.endsWith('.') ? "." : ""}}}};
    int id = m_client->request("textDocument/completion",params);
    m_pendingCompletions.insert(editor,id);
    m_completionEditors.insert(id,editor);
    m_completionPrefixes.insert(id,prefix);
    m_completionRoots.insert(id,root);
}

QString GoplsPlugin::completionKind(int kind) const
{
    switch (kind) {
    case 2: case 3: case 4: return "func";
    case 5: return "field";
    case 6: return "var";
    case 7: case 22: return "struct";
    case 8: return "interface";
    case 9: return "package";
    case 12: return "value";
    case 13: return "type";
    case 14: return "keyword";
    case 20: case 21: return "const";
    case 25: return "type";
    default: return QString();
    }
}

void GoplsPlugin::clientResponse(int id, const QString &method, const QJsonValue &result, const QJsonObject &error)
{
    if (method == "textDocument/definition" || method == "textDocument/references" || method == "textDocument/implementation") {
        QString requestMethod = m_locationRequests.take(id);
        QString searchText = m_locationSearchText.take(id);
        if (!error.isEmpty()) {
            clientLog(error.value("message").toString(),true);
            return;
        }
        QJsonArray locations = result.isArray() ? result.toArray() : QJsonArray{result};
        if (requestMethod == "textDocument/definition") {
            if (!locations.isEmpty()) {
                QJsonObject location = locations.first().toObject();
                QString uri = location.value("uri").toString();
                QJsonObject range = location.value("range").toObject();
                if (uri.isEmpty()) {
                    uri = location.value("targetUri").toString();
                    range = location.value("targetSelectionRange").toObject();
                }
                QJsonObject start = range.value("start").toObject();
                LiteApi::gotoLine(m_liteApp,QUrl(uri).toLocalFile(),start.value("line").toInt(),start.value("character").toInt(),true,true);
            }
        } else {
            LiteApi::IFileSearchManager *manager = LiteApi::getFileSearchManager(m_liteApp);
            if (manager) {
                manager->setCurrentSearch(m_searchResults);
                m_searchResults->showLocations(requestMethod == "textDocument/references" ? tr("References") : tr("Implementations"),searchText,locations);
            }
        }
        return;
    }
    if (method == "textDocument/hover" || method == "textDocument/signatureHelp") {
        LiteApi::IEditor *editor = m_hintEditors.take(id);
        QPoint position = m_hintPositions.take(id);
        if (!editor || !error.isEmpty() || result.isNull()) {
            return;
        }
        QString text;
        if (method == "textDocument/hover") {
            text = markupText(result.toObject().value("contents"));
        } else {
            QJsonArray signatures = result.toObject().value("signatures").toArray();
            if (!signatures.isEmpty()) {
                QJsonObject signature = signatures.first().toObject();
                text = signature.value("label").toString();
                QString documentation = markupText(signature.value("documentation"));
                if (!documentation.isEmpty()) {
                    text += "\n"+documentation;
                }
            }
        }
        LiteApi::ILiteEditor *liteEditor = LiteApi::getLiteEditor(editor);
        if (liteEditor && !text.isEmpty()) {
            liteEditor->showToolTipInfo(position,text);
        }
        return;
    }
    if (method != "textDocument/completion") {
        return;
    }
    LiteApi::IEditor *editor = m_completionEditors.take(id);
    QString prefix = m_completionPrefixes.take(id);
    QString rootName = m_completionRoots.take(id);
    if (!editor || m_pendingCompletions.value(editor) != id) {
        return;
    }
    m_pendingCompletions.remove(editor);
    if (!error.isEmpty() || m_liteApp->editorManager()->currentEditor() != editor) {
        return;
    }
    LiteApi::ICompleter *completer = LiteApi::findExtensionObject<LiteApi::ICompleter*>(editor,"LiteApi.ICompleter");
    if (!completer || completer->completionPrefix() != prefix) {
        return;
    }
    QJsonArray items = result.isArray() ? result.toArray() : result.toObject().value("items").toArray();
    QStandardItem *root = completer->findRoot(rootName);
    completer->clearChildItem(root);
    LiteApi::IGolangAst *ast = LiteApi::findExtensionObject<LiteApi::IGolangAst*>(m_liteApp,"LiteApi.IGolangAst");
    int count = 0;
    foreach (QJsonValue value, items) {
        QJsonObject item = value.toObject();
        QString label = item.value("label").toString();
        QString kind = completionKind(item.value("kind").toInt());
        QString detail = item.value("detail").toString();
        if (label.isEmpty()) {
            continue;
        }
        QIcon icon;
        if (ast) {
            LiteApi::ASTTAG_ENUM tag = LiteApi::TagNone;
            if (kind == "func") tag = LiteApi::TagFunc;
            else if (kind == "var" || kind == "field" || kind == "value") tag = LiteApi::TagValue;
            else if (kind == "const") tag = LiteApi::TagConst;
            else if (kind == "struct") tag = LiteApi::TagStruct;
            else if (kind == "interface") tag = LiteApi::TagInterface;
            else if (kind == "package") tag = LiteApi::TagPackage;
            else if (kind == "type") tag = LiteApi::TagType;
            icon = ast->iconFromTagEnum(tag,true);
        }
        completer->appendChildItem(root,label,kind,detail,icon,true);
        count++;
    }
    if (count) {
        completer->updateCompleterModel();
        completer->showPopup();
    }
}

void GoplsPlugin::addEditorActions(LiteApi::IEditor *editor)
{
    if (!isGoEditor(editor)) {
        return;
    }
    QMenu *editMenu = LiteApi::getEditMenu(editor);
    QMenu *contextMenu = LiteApi::getContextMenu(editor);
    foreach (QMenu *menu, QList<QMenu*>() << editMenu << contextMenu) {
        if (menu) {
            menu->addSeparator();
            menu->addAction(m_definitionAction);
            menu->addAction(m_referencesAction);
            menu->addAction(m_implementationAction);
        }
    }
}

void GoplsPlugin::requestLocations(const QString &method)
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    if (!m_ready || !isGoEditor(editor) || !textEditor) {
        return;
    }
    QTextCursor cursor = textEditor->textCursor();
    QString searchText = LiteApi::wordUnderCursor(cursor);
    QJsonObject params{{"textDocument",QJsonObject{{"uri",documentUri(editor)}}},
                       {"position",QJsonObject{{"line",cursor.blockNumber()},{"character",cursor.positionInBlock()}}}};
    if (method == "textDocument/references") {
        params.insert("context",QJsonObject{{"includeDeclaration",true}});
    }
    int id = m_client->request(method,params);
    m_locationRequests.insert(id,method);
    m_locationSearchText.insert(id,searchText);
}

void GoplsPlugin::goToDefinition() { requestLocations("textDocument/definition"); }
void GoplsPlugin::findReferences() { requestLocations("textDocument/references"); }
void GoplsPlugin::findImplementations() { requestLocations("textDocument/implementation"); }

QString GoplsPlugin::markupText(const QJsonValue &value) const
{
    if (value.isString()) {
        return value.toString();
    }
    if (value.isObject()) {
        QJsonObject object = value.toObject();
        return object.value("value").toString(object.value("language").toString());
    }
    if (value.isArray()) {
        QStringList parts;
        foreach (QJsonValue part, value.toArray()) {
            QString text = markupText(part);
            if (!text.isEmpty()) parts.append(text);
        }
        return parts.join("\n");
    }
    return QString();
}

void GoplsPlugin::hoverRequested(const QTextCursor &cursor, const QPoint &position, bool navigation)
{
    if (!m_ready || navigation) {
        return;
    }
    LiteApi::ILiteEditor *liteEditor = qobject_cast<LiteApi::ILiteEditor*>(sender());
    LiteApi::IEditor *editor = liteEditor;
    if (!isGoEditor(editor) || cursor.selectedText().trimmed().isEmpty()) {
        return;
    }
    QTextCursor requestCursor = cursor;
    requestCursor.setPosition(cursor.selectionStart());
    QJsonObject params{{"textDocument",QJsonObject{{"uri",documentUri(editor)}}},
                       {"position",QJsonObject{{"line",requestCursor.blockNumber()},{"character",requestCursor.positionInBlock()}}}};
    int id = m_client->request("textDocument/hover",params);
    m_hintEditors.insert(id,editor);
    m_hintPositions.insert(id,position);
}

void GoplsPlugin::requestSignatureHelp(LiteApi::IEditor *editor)
{
    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    if (!textEditor) return;
    QTextCursor cursor = textEditor->textCursor();
    QJsonObject params{{"textDocument",QJsonObject{{"uri",documentUri(editor)}}},
                       {"position",QJsonObject{{"line",cursor.blockNumber()},{"character",cursor.positionInBlock()}}},
                       {"context",QJsonObject{{"triggerKind",2}}}};
    int id = m_client->request("textDocument/signatureHelp",params);
    QPlainTextEdit *plainTextEdit = LiteApi::getPlainTextEdit(editor);
    QPoint position;
    if (plainTextEdit) {
        position = plainTextEdit->mapToGlobal(plainTextEdit->cursorRect().bottomLeft());
    }
    m_hintEditors.insert(id,editor);
    m_hintPositions.insert(id,position);
}

void GoplsPlugin::clientNotification(const QString &method, const QJsonValue &paramsValue)
{
    if (method != "textDocument/publishDiagnostics") {
        return;
    }
    QJsonObject params = paramsValue.toObject();
    QString fileName = QUrl(params.value("uri").toString()).toLocalFile();
    LiteApi::IEditor *editor = m_liteApp->editorManager()->findEditor(fileName,true);
    LiteApi::ILiteEditor *liteEditor = LiteApi::getLiteEditor(editor);
    if (!liteEditor) {
        return;
    }
    liteEditor->clearAllNavigateMark(LiteApi::EditorNavigateBad,"gopls");
    foreach (QJsonValue value, params.value("diagnostics").toArray()) {
        QJsonObject diagnostic = value.toObject();
        QJsonObject start = diagnostic.value("range").toObject().value("start").toObject();
        int severity = diagnostic.value("severity").toInt(2);
        LiteApi::EditorNaviagteType type = severity == 1 ? LiteApi::EditorNavigateError : LiteApi::EditorNavigateWarning;
        liteEditor->insertNavigateMark(start.value("line").toInt(),type,diagnostic.value("message").toString(),"gopls");
    }
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(PluginFactory,PluginFactory)
#endif
