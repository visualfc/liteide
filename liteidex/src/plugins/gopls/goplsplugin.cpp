#include "goplsplugin.h"
#include "goplsclient.h"
#include "goplssearchresults.h"
#include "liteenvapi/liteenvapi.h"
#include "golangastapi/golangastapi.h"
#include "liteeditorapi/liteeditorapi.h"
#include "fileutil/fileutil.h"
#include "gopls_global.h"
#include "goplsoptionfactory.h"

#include <QCoreApplication>
#include <QApplication>
#include <QAction>
#include <QFile>
#include <QInputDialog>
#include <QDir>
#include <QFileInfo>
#include <QHelpEvent>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QStandardItem>
#include <QPlainTextEdit>
#include <QTextBlock>
#include <QTextDocument>
#include <QUrl>
#include <QRegExp>
#include <algorithm>

static QString formatGoplsInfo(const QString &info)
{
    if (!info.startsWith("type")) return info;
    QRegExp re("([\\w\\s\\.]+)\\{(.+)\\}");
    if (re.indexIn(info) == 0 && re.matchedLength() == info.length()) {
        QString formatted = re.cap(1)+" {\n";
        foreach (QString item, re.cap(2).split(";",qtSkipEmptyParts)) {
            formatted += "\t"+item.trimmed()+"\n";
        }
        return formatted+"}";
    }
    return info;
}

static QString limitGoplsInfo(const QString &info, int maxLines = 10)
{
    QStringList lines;
    foreach (const QString &line, info.split("\n")) {
        QString rest = line;
        while (rest.length() > 120) {
            int split = rest.lastIndexOf(' ', 120);
            if (split < 40) split = 120;
            lines.append(rest.left(split).trimmed());
            rest = rest.mid(split).trimmed();
        }
        lines.append(rest);
    }
    if (lines.size() <= maxLines) return info;
    return lines.mid(0,maxLines).join("\n")+"\n...";
}

GoplsPlugin::GoplsPlugin() : m_liteApp(0), m_client(0), m_completer(0), m_ready(false),
    m_searchResults(0), m_definitionAction(0), m_referencesAction(0), m_implementationAction(0)
    , m_renameAction(0), m_formatAction(0), m_organizeImportsAction(0), m_appLoaded(false), m_useFeatures(false)
{
}

GoplsPlugin::~GoplsPlugin()
{
    if (m_client) {
        disconnect(m_client,0,this,0);
        m_client->stop();
    }
}

bool GoplsPlugin::load(LiteApi::IApplication *app)
{
    m_liteApp = app;
    m_useFeatures = app->settings()->value(GOPLS_USE_FEATURES,false).toBool();
    app->optionManager()->addFactory(new GoplsOptionFactory(app,this));
    m_client = new GoplsClient(this);
    m_searchResults = new GoplsSearchResults(this);
    LiteApi::IFileSearchManager *searchManager = LiteApi::getFileSearchManager(app);
    if (searchManager) {
        searchManager->addFileSearch(m_searchResults);
    }
    LiteApi::IActionContext *actions = app->actionManager()->getActionContext(this,"Gopls");
    m_definitionAction = new QAction(tr("Go to Definition (gopls)"),this);
    m_referencesAction = new QAction(tr("Find All References (gopls)"),this);
    m_implementationAction = new QAction(tr("Find All Implementations (gopls)"),this);
    m_renameAction = new QAction(tr("Rename Symbol (gopls)"),this);
    m_formatAction = new QAction(tr("Format Document (gopls)"),this);
    m_organizeImportsAction = new QAction(tr("Organize Imports (gopls)"),this);
    m_definitionAction->setEnabled(m_useFeatures);
    m_referencesAction->setEnabled(m_useFeatures);
    m_implementationAction->setEnabled(m_useFeatures);
    actions->regAction(m_definitionAction,"Definition","");
    actions->regAction(m_referencesAction,"References","");
    actions->regAction(m_implementationAction,"Implementation","");
    actions->regAction(m_renameAction,"Rename","");
    actions->regAction(m_formatAction,"Format","");
    actions->regAction(m_organizeImportsAction,"OrganizeImports","");
    connect(m_definitionAction,SIGNAL(triggered()),this,SLOT(goToDefinition()));
    connect(m_referencesAction,SIGNAL(triggered()),this,SLOT(findReferences()));
    connect(m_implementationAction,SIGNAL(triggered()),this,SLOT(findImplementations()));
    connect(m_renameAction,SIGNAL(triggered()),this,SLOT(renameSymbol()));
    connect(m_formatAction,SIGNAL(triggered()),this,SLOT(formatDocument()));
    connect(m_organizeImportsAction,SIGNAL(triggered()),this,SLOT(organizeImports()));
    app->extension()->addObject("LiteApi.IGoplsService",m_client);
    connect(app,SIGNAL(loaded()),this,SLOT(appLoaded()));
    connect(app->optionManager(),SIGNAL(applyOption(QString)),this,SLOT(applyOption(QString)));
    connect(m_client,SIGNAL(initialized()),this,SLOT(clientInitialized()));
    connect(m_client,SIGNAL(stopped()),this,SLOT(clientStopped()));
    connect(m_client,SIGNAL(logMessage(QString,bool)),this,SLOT(clientLog(QString,bool)));
    connect(m_client,SIGNAL(response(int,QString,QJsonValue,QJsonObject)),this,SLOT(clientResponse(int,QString,QJsonValue,QJsonObject)));
    connect(m_client,SIGNAL(notification(QString,QJsonValue)),this,SLOT(clientNotification(QString,QJsonValue)));
    connect(m_client,SIGNAL(serverRequest(int,QString,QJsonValue)),this,SLOT(serverRequest(int,QString,QJsonValue)));
    connect(app->editorManager(),SIGNAL(editorCreated(LiteApi::IEditor*)),this,SLOT(editorCreated(LiteApi::IEditor*)));
    connect(app->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));
    connect(app->editorManager(),SIGNAL(editorAboutToClose(LiteApi::IEditor*)),this,SLOT(editorAboutToClose(LiteApi::IEditor*)));
    connect(app->editorManager(),SIGNAL(editorSaved(LiteApi::IEditor*)),this,SLOT(editorSaved(LiteApi::IEditor*)));
    connect(app->projectManager(),SIGNAL(currentProjectChanged(LiteApi::IProject*)),this,SLOT(workspaceChanged()));
    LiteApi::IEnvManager *envManager = LiteApi::getEnvManager(app);
    if (envManager) {
        connect(envManager,SIGNAL(currentEnvChanged(LiteApi::IEnv*)),this,SLOT(environmentChanged(LiteApi::IEnv*)));
    }
    return true;
}

QStringList GoplsPlugin::dependPluginList() const
{
    return QStringList() << "plugin/liteenv" << "plugin/golangast" << "plugin/litefind";
}

bool GoplsPlugin::eventFilter(QObject *object, QEvent *event)
{
    LiteApi::IEditor *editor = m_hoverViewports.value(object);
    if (!m_useFeatures || !m_ready || !editor || event->type() != QEvent::ToolTip) {
        return LiteApi::IPlugin::eventFilter(object,event);
    }
    if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
        return LiteApi::IPlugin::eventFilter(object,event);
    }
    QPlainTextEdit *plainTextEdit = LiteApi::getPlainTextEdit(editor);
    if (!plainTextEdit) {
        return LiteApi::IPlugin::eventFilter(object,event);
    }
    QHelpEvent *helpEvent = static_cast<QHelpEvent*>(event);
    QTextCursor cursor = plainTextEdit->cursorForPosition(helpEvent->pos());
    LiteApi::selectWordUnderCursor(cursor);
    requestHover(editor,cursor,helpEvent->globalPos());
    return true;
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
    m_appLoaded = true;
    if (!m_useFeatures) {
        return;
    }
    if (m_client->isRunning()) {
        return;
    }

    QProcessEnvironment environment = LiteApi::getGoEnvironment(m_liteApp);
    QString program = FileUtil::lookupGoBin("gopls",m_liteApp,environment,true);
    if (program.isEmpty()) {
        // gopls is optional; the plugin restores legacy gocode completion when it is unavailable.
        clientLog(tr("gopls was not found on system PATH (hint: go install golang.org/x/tools/gopls@latest)"),true);
        return;
    } else {
        clientLog(QString("Found gopls at %1").arg(program),false);
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
    foreach (LiteApi::IEditor *editor, m_liteApp->editorManager()->editorList()) {
        editorCreated(editor);
        openDocument(editor);
    }
    currentEditorChanged(m_liteApp->editorManager()->currentEditor());
}

void GoplsPlugin::clientStopped()
{
    m_ready = false;
    m_openDocuments.clear();
    if (m_completer) {
        LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
        if (editor) {
            configureCompleter(editor,false);
            configureGocodeCompletion(editor,true);
        }
        m_completer = 0;
    }
    m_completionEditors.clear();
    m_completionPrefixes.clear();
    m_completionRoots.clear();
    m_pendingCompletions.clear();
    m_pendingHovers.clear();
}

void GoplsPlugin::clientLog(const QString &message, bool error)
{
    m_liteApp->appendLog("Gopls",message,error);
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
    QPlainTextEdit *plainTextEdit = LiteApi::getPlainTextEdit(editor);
    if (plainTextEdit && !m_hoverViewports.contains(plainTextEdit->viewport())) {
        plainTextEdit->viewport()->installEventFilter(this);
        m_hoverViewports.insert(plainTextEdit->viewport(),editor);
    }
    addEditorActions(editor);
    if (m_ready) {
        openDocument(editor);
    }
}

void GoplsPlugin::currentEditorChanged(LiteApi::IEditor *editor)
{
    if (m_completer) {
        disconnect(m_completer,0,this,0);
        m_completer->setSearchSeparator(true);
        m_completer->setExternalMode(false);
        m_completer = 0;
    }
    if (!m_ready || !isGoEditor(editor)) {
        return;
    }
    m_completer = LiteApi::findExtensionObject<LiteApi::ICompleter*>(editor,"LiteApi.ICompleter");
    configureCompleter(editor,m_useFeatures && m_completer != 0);
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
    if (editor == m_liteApp->editorManager()->currentEditor()) {
        m_completer = 0;
    }
    int requestId = m_pendingCompletions.take(editor);
    m_completionEditors.remove(requestId);
    m_completionPrefixes.remove(requestId);
    m_completionRoots.remove(requestId);
    int hoverId = m_pendingHovers.take(editor);
    if (hoverId) {
        m_client->notify("$/cancelRequest",QJsonObject{{"id",hoverId}});
        m_hintEditors.remove(hoverId);
        m_hintPositions.remove(hoverId);
    }
    QObject *viewport = m_hoverViewports.key(editor,0);
    if (viewport) {
        viewport->removeEventFilter(this);
        m_hoverViewports.remove(viewport);
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
    disconnect(completer,SIGNAL(wordCompleted(QString,QString,QString)),this,SLOT(completionAccepted(QString,QString,QString)));
    if (enabled) {
        configureGocodeCompletion(editor,false);
        completer->setSearchSeparator(false);
        completer->setExternalMode(true);
        connect(completer,SIGNAL(prefixChanged(QTextCursor,QString,bool)),this,SLOT(completionRequested(QTextCursor,QString,bool)),Qt::UniqueConnection);
        connect(completer,SIGNAL(wordCompleted(QString,QString,QString)),this,SLOT(completionAccepted(QString,QString,QString)),Qt::UniqueConnection);
    } else {
        completer->setSearchSeparator(true);
        completer->setExternalMode(false);
    }
}

void GoplsPlugin::configureGocodeCompletion(LiteApi::IEditor *editor, bool enabled)
{
    if (!editor || !m_liteApp || !m_liteApp->extension()) return;
    QObject *golangCode = m_liteApp->extension()->findObject("LiteApi.GolangCode");
    if (!golangCode) return;
    LiteApi::ICompleter *completer = LiteApi::findExtensionObject<LiteApi::ICompleter*>(editor,"LiteApi.ICompleter");
    if (!completer) return;
    if (enabled) {
        QMetaObject::invokeMethod(golangCode,"currentEditorChanged",Qt::DirectConnection,
                                  Q_ARG(LiteApi::IEditor*,editor));
    } else {
        QObject::disconnect(completer,SIGNAL(prefixChanged(QTextCursor,QString,bool)),
                            golangCode,SLOT(prefixChanged(QTextCursor,QString,bool)));
        QObject::disconnect(completer,SIGNAL(wordCompleted(QString,QString,QString)),
                            golangCode,SLOT(wordCompleted(QString,QString,QString)));
    }
}

void GoplsPlugin::applyOption(const QString &option)
{
    if (option != OPTION_GOPLS) return;
    bool enabled = m_liteApp->settings()->value(GOPLS_USE_FEATURES,false).toBool();
    if (enabled == m_useFeatures) return;
    m_useFeatures = enabled;
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (editor) {
        configureCompleter(editor, enabled && m_completer != 0);
        if (!enabled) configureGocodeCompletion(editor,true);
    }
    if (!enabled) {
        foreach (int id, m_pendingCompletions) {
            m_client->notify("$/cancelRequest",QJsonObject{{"id",id}});
        }
        m_pendingCompletions.clear();
        m_completionEditors.clear();
        m_completionPrefixes.clear();
        m_completionRoots.clear();
        foreach (int id, m_pendingHovers) {
            m_client->notify("$/cancelRequest",QJsonObject{{"id",id}});
        }
        m_pendingHovers.clear();
        m_hintEditors.clear();
        m_hintPositions.clear();
    }
    m_definitionAction->setEnabled(enabled);
    m_referencesAction->setEnabled(enabled);
    m_implementationAction->setEnabled(enabled);
    if (enabled && !m_client->isRunning()) {
        appLoaded();
    } else if (!enabled && m_client->isRunning()) {
        m_client->stop();
    }
}

void GoplsPlugin::completionRequested(QTextCursor cursor, QString prefix, bool force)
{
    Q_UNUSED(cursor);
    if (!m_useFeatures || !m_ready) {
        return;
    }
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor || !m_openDocuments.contains(editor) || m_completer->completionContext() != LiteApi::CompleterCodeContext) {
        return;
    }
    if (!force && !prefix.endsWith('.') && prefix.length() != m_completer->prefixMin()) {
        return;
    }

    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    if (!textEditor) {
        return;
    }
    changeDocument(editor);
    cursor = textEditor->textCursor();

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
    if (method == "textDocument/rename" || method == "textDocument/formatting" || method == "textDocument/codeAction") {
        handleEditResponse(id,method,result,error);
        return;
    }
    if (method == "textDocument/definition" || method == "textDocument/references" || method == "textDocument/implementation") {
        handleLocationResponse(id,result,error);
        return;
    }
    if (method == "textDocument/hover" || method == "textDocument/signatureHelp") {
        handleHintResponse(id,method,result,error);
        return;
    }
    if (method == "textDocument/completion") {
        handleCompletionResponse(id,result,error);
    }
}

void GoplsPlugin::handleEditResponse(int id, const QString &method, const QJsonValue &result, const QJsonObject &error)
{
    QString operation = m_editRequests.take(id);
    LiteApi::IEditor *editor = m_editRequestEditors.take(id);
    if (!error.isEmpty()) {
        clientLog(error.value("message").toString(),true);
        return;
    }
    if (method == "textDocument/rename") {
        applyWorkspaceEdit(result.toObject());
    } else if (method == "textDocument/formatting") {
        if (editor) applyTextEdits(documentUri(editor),result.toArray());
    } else {
        QJsonArray actions = result.toArray();
        foreach (QJsonValue value, actions) {
            QJsonObject action = value.toObject();
            if (action.contains("edit")) {
                applyWorkspaceEdit(action.value("edit").toObject());
                break;
            }
            QJsonObject command = action.value("command").toObject();
            if (command.isEmpty() && action.contains("command") && action.value("command").isString()) {
                command = action;
            }
            if (!command.isEmpty()) {
                m_client->request("workspace/executeCommand",QJsonObject{{"command",command.value("command")},
                                                                         {"arguments",command.value("arguments")}});
                break;
            }
        }
    }
    Q_UNUSED(operation);
}

void GoplsPlugin::handleLocationResponse(int id, const QJsonValue &result, const QJsonObject &error)
{
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
            bool references = requestMethod == "textDocument/references";
            m_searchResults->setReplaceMode(false,!references);
            manager->setCurrentSearch(m_searchResults);
            m_searchResults->showLocations(references ? tr("All References") : tr("Implementations"),
                                           searchText,locations,false,!references);
        }
    }
}

void GoplsPlugin::handleHintResponse(int id, const QString &method, const QJsonValue &result, const QJsonObject &error)
{
    LiteApi::IEditor *editor = m_hintEditors.take(id);
    QPoint position = m_hintPositions.take(id);
    if (method == "textDocument/hover") {
        if (!editor || m_pendingHovers.value(editor) != id) {
            return;
        }
        m_pendingHovers.remove(editor);
    }
    if (!editor || !error.isEmpty() || result.isNull()) {
        return;
    }
    QString text;
    if (method == "textDocument/hover") {
        text = limitGoplsInfo(formatGoplsInfo(markupText(result.toObject().value("contents"))));
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
}

void GoplsPlugin::handleCompletionResponse(int id, const QJsonValue &result, const QJsonObject &error)
{
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
    m_completionAdditionalEdits[completer].clear();
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
        QJsonArray additionalEdits = item.value("additionalTextEdits").toArray();
        if (!additionalEdits.isEmpty()) {
            m_completionAdditionalEdits[completer].insert(label,additionalEdits);
        }
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
            menu->addSeparator();
            menu->addAction(m_renameAction);
            menu->addAction(m_formatAction);
            menu->addAction(m_organizeImportsAction);
        }
    }
}

void GoplsPlugin::requestLocations(const QString &method)
{
    if (!m_useFeatures) return;
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    if (!m_ready || !isGoEditor(editor) || !textEditor) {
        return;
    }
    if (method == "textDocument/references") {
        m_liteApp->editorManager()->saveAllEditors(false);
    }
    changeDocument(editor);
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
        QString text = object.value("value").toString();
        if (object.value("kind").toString() == "markdown") {
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
            QTextDocument document;
            document.setMarkdown(text);
            text = document.toPlainText();
#else
            text.replace(QRegExp("```[A-Za-z0-9_+.-]*\\n"),QString());
            text.replace("```",QString());
#endif
        }
        return text.trimmed();
    }
    if (value.isArray()) {
        QStringList parts;
        foreach (QJsonValue part, value.toArray()) {
            QString text = markupText(part);
            if (!text.isEmpty()) parts.append(text);
        }
        return parts.join("\n\n");
    }
    return QString();
}

void GoplsPlugin::hoverRequested(const QTextCursor &cursor, const QPoint &position, bool navigation)
{
    if (!m_useFeatures || !m_ready || navigation) {
        return;
    }
    LiteApi::ILiteEditor *liteEditor = qobject_cast<LiteApi::ILiteEditor*>(sender());
    LiteApi::IEditor *editor = liteEditor;
    QPlainTextEdit *plainTextEdit = LiteApi::getPlainTextEdit(editor);
    QPoint globalPosition = position;
    if (plainTextEdit) {
        globalPosition = plainTextEdit->mapToGlobal(position);
    }
    requestHover(editor,cursor,globalPosition);
}

void GoplsPlugin::requestHover(LiteApi::IEditor *editor, const QTextCursor &cursor,
                               const QPoint &position)
{
    if (!m_useFeatures) return;
    if (!isGoEditor(editor) || cursor.selectedText().trimmed().isEmpty()) {
        int oldId = m_pendingHovers.take(editor);
        if (oldId) {
            m_client->notify("$/cancelRequest",QJsonObject{{"id",oldId}});
            m_hintEditors.remove(oldId);
            m_hintPositions.remove(oldId);
        }
        return;
    }
    QTextCursor requestCursor = cursor;
    requestCursor.setPosition(cursor.selectionStart());
    QPoint displayPosition = position;
    QPlainTextEdit *plainTextEdit = LiteApi::getPlainTextEdit(editor);
    if (plainTextEdit) {
        displayPosition = plainTextEdit->mapToGlobal(plainTextEdit->cursorRect(requestCursor).topRight());
    }
    QJsonObject params{{"textDocument",QJsonObject{{"uri",documentUri(editor)}}},
                       {"position",QJsonObject{{"line",requestCursor.blockNumber()},{"character",requestCursor.positionInBlock()}}}};
    changeDocument(editor);
    int oldId = m_pendingHovers.value(editor,0);
    if (oldId) {
        m_client->notify("$/cancelRequest",QJsonObject{{"id",oldId}});
        m_hintEditors.remove(oldId);
        m_hintPositions.remove(oldId);
    }
    int id = m_client->request("textDocument/hover",params);
    m_hintEditors.insert(id,editor);
    m_hintPositions.insert(id,displayPosition);
    m_pendingHovers.insert(editor,id);
}

void GoplsPlugin::requestSignatureHelp(LiteApi::IEditor *editor)
{
    if (!m_useFeatures) return;
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

struct GoplsTextEditGreater
{
    bool operator()(const QJsonValue &leftValue, const QJsonValue &rightValue) const {
        QJsonObject left = leftValue.toObject().value("range").toObject().value("start").toObject();
        QJsonObject right = rightValue.toObject().value("range").toObject().value("start").toObject();
        int leftLine = left.value("line").toInt();
        int rightLine = right.value("line").toInt();
        return leftLine == rightLine ? left.value("character").toInt() > right.value("character").toInt() : leftLine > rightLine;
    }
};

void GoplsPlugin::applyTextEdits(const QString &uri, QJsonArray edits)
{
    if (edits.isEmpty()) return;
    QList<QJsonValue> sortedEdits;
    foreach (QJsonValue edit, edits) sortedEdits.append(edit);
    std::sort(sortedEdits.begin(),sortedEdits.end(),GoplsTextEditGreater());
    QString fileName = QUrl(uri).toLocalFile();
    LiteApi::IEditor *editor = m_liteApp->editorManager()->findEditor(fileName,true);
    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    if (textEditor) {
        QTextCursor cursor(textEditor->document());
        cursor.beginEditBlock();
        foreach (QJsonValue value, sortedEdits) {
            QJsonObject edit = value.toObject();
            QJsonObject range = edit.value("range").toObject();
            QJsonObject start = range.value("start").toObject();
            QJsonObject end = range.value("end").toObject();
            QTextBlock startBlock = textEditor->document()->findBlockByNumber(start.value("line").toInt());
            QTextBlock endBlock = textEditor->document()->findBlockByNumber(end.value("line").toInt());
            if (!startBlock.isValid() || !endBlock.isValid()) continue;
            cursor.setPosition(startBlock.position()+start.value("character").toInt());
            cursor.setPosition(endBlock.position()+end.value("character").toInt(),QTextCursor::KeepAnchor);
            cursor.insertText(edit.value("newText").toString());
        }
        cursor.endEditBlock();
        if (m_openDocuments.contains(editor)) {
            changeDocument(editor);
        }
        return;
    }

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) return;
    QString text = QString::fromUtf8(file.readAll());
    file.close();
    foreach (QJsonValue value, sortedEdits) {
        QJsonObject edit = value.toObject();
        QJsonObject range = edit.value("range").toObject();
        QJsonObject start = range.value("start").toObject();
        QJsonObject end = range.value("end").toObject();
        QStringList lines = text.split('\n',Qt::KeepEmptyParts);
        int startOffset = 0;
        int endOffset = 0;
        for (int line = 0; line < start.value("line").toInt() && line < lines.size(); ++line) startOffset += lines.at(line).size()+1;
        for (int line = 0; line < end.value("line").toInt() && line < lines.size(); ++line) endOffset += lines.at(line).size()+1;
        startOffset += start.value("character").toInt();
        endOffset += end.value("character").toInt();
        text.replace(startOffset,endOffset-startOffset,edit.value("newText").toString());
    }
    if (file.open(QFile::WriteOnly|QFile::Truncate)) {
        file.write(text.toUtf8());
    }
}

void GoplsPlugin::applyWorkspaceEdit(const QJsonObject &workspaceEdit)
{
    QJsonObject changes = workspaceEdit.value("changes").toObject();
    foreach (QString uri, changes.keys()) applyTextEdits(uri,changes.value(uri).toArray());
    foreach (QJsonValue value, workspaceEdit.value("documentChanges").toArray()) {
        QJsonObject change = value.toObject();
        if (change.contains("edits")) {
            applyTextEdits(change.value("textDocument").toObject().value("uri").toString(),change.value("edits").toArray());
        }
    }
}

void GoplsPlugin::serverRequest(int id, const QString &method, const QJsonValue &params)
{
    if (method == "workspace/applyEdit") {
        applyWorkspaceEdit(params.toObject().value("edit").toObject());
        m_client->reply(id,QJsonObject{{"applied",true}});
    } else if (method == "workspace/configuration") {
        QJsonArray result;
        foreach (QJsonValue item, params.toObject().value("items").toArray()) {
            Q_UNUSED(item);
            result.append(QJsonObject());
        }
        m_client->reply(id,result);
    } else {
        m_client->reply(id,QJsonValue());
    }
}

void GoplsPlugin::renameSymbol()
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    if (!m_ready || !isGoEditor(editor) || !textEditor) return;
    bool ok = false;
    QString oldName = LiteApi::wordUnderCursor(textEditor->textCursor());
    QString newName = QInputDialog::getText(m_liteApp->mainWindow(),tr("Rename Symbol"),tr("New name:"),QLineEdit::Normal,oldName,&ok);
    if (!ok || newName.isEmpty() || newName == oldName) return;
    changeDocument(editor);
    QTextCursor cursor = textEditor->textCursor();
    QJsonObject params{{"textDocument",QJsonObject{{"uri",documentUri(editor)}}},
                       {"position",QJsonObject{{"line",cursor.blockNumber()},{"character",cursor.positionInBlock()}}},
                       {"newName",newName}};
    int id = m_client->request("textDocument/rename",params);
    m_editRequests.insert(id,"rename");
    m_editRequestEditors.insert(id,editor);
}

void GoplsPlugin::formatDocument()
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    if (!m_ready || !isGoEditor(editor) || !textEditor) return;
    changeDocument(editor);
    QJsonObject params{{"textDocument",QJsonObject{{"uri",documentUri(editor)}}},
                       {"options",QJsonObject{{"tabSize",4},{"insertSpaces",false}}}};
    int id = m_client->request("textDocument/formatting",params);
    m_editRequests.insert(id,"format");
    m_editRequestEditors.insert(id,editor);
}

void GoplsPlugin::organizeImports()
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    if (!m_ready || !isGoEditor(editor) || !textEditor) return;
    changeDocument(editor);
    QJsonObject zero{{"line",0},{"character",0}};
    QJsonArray only;
    only.append("source.organizeImports");
    QJsonObject context{{"diagnostics",QJsonArray()},{"only",only}};
    QJsonObject params{{"textDocument",QJsonObject{{"uri",documentUri(editor)}}},
                       {"range",QJsonObject{{"start",zero},{"end",zero}}},
                       {"context",context}};
    int id = m_client->request("textDocument/codeAction",params);
    m_editRequests.insert(id,"organizeImports");
    m_editRequestEditors.insert(id,editor);
}

void GoplsPlugin::completionAccepted(const QString &text, const QString &, const QString &)
{
    QObject *completer = sender();
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    QJsonArray edits = m_completionAdditionalEdits[completer].take(text);
    if (editor && completer == m_completer && !edits.isEmpty()) applyTextEdits(documentUri(editor),edits);
}

void GoplsPlugin::workspaceChanged()
{
    //if (!m_appLoaded) return;
    m_ready = false;
    m_openDocuments.clear();
    m_client->stop();
    appLoaded();
}

void GoplsPlugin::environmentChanged(LiteApi::IEnv *)
{
    workspaceChanged();
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(PluginFactory,PluginFactory)
#endif
