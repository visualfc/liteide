#include "goplsserver.h"

#include "generated.h"
#include <QFileInfo>
#include <QTimer>
#include "liteenvapi/liteenvapi.h"

#define DECODE_CALLBACK(fn) std::bind(fn, this, _1)
using namespace GoPlsTypes;
using namespace std::placeholders;

GoPlsServer::GoPlsServer(LiteApi::IApplication *app, QObject *parent)
    : QObject(parent)
    , m_init(false)
{

    m_envManager = LiteApi::getEnvManager(app);
    m_process = new QProcess(this);

    connect(m_envManager, &LiteApi::IEnvManager::currentEnvChanged, this, &GoPlsServer::currentEnvChanged);
    m_process->setProgram("gopls");
    m_process->setArguments({"-mode=stdio"});
    connect(m_process, &QProcess::started, this, &GoPlsServer::onStarted);
    connect(m_process, &QProcess::readyRead, this, &GoPlsServer::onReadyRead);
    m_process->start();
}

GoPlsServer::~GoPlsServer()
{
    m_process->kill();
    m_process->deleteLater();
}

void GoPlsServer::quit()
{
    m_process->kill();
}

void GoPlsServer::initWorkspace(const QStringList &_folders)
{
    QSharedPointer<InitializeParams> params(new InitializeParams());
    ClientCapabilities *clientCapabilites = new ClientCapabilities();
    TextDocumentClientCapabilities *documentCapabilities = new TextDocumentClientCapabilities();
    PublishDiagnosticsClientCapabilities *publishCapabilities = new PublishDiagnosticsClientCapabilities();
    publishCapabilities->setCodeDescriptionSupport(new bool(true));
    publishCapabilities->setDataSupport(new bool(true));
    publishCapabilities->setRelatedInformation(new bool(true));
    publishCapabilities->setVersionSupport(new bool(true));
    documentCapabilities->setPublishDiagnostics(publishCapabilities);
    DocumentFormattingClientCapabilities *formattingCapabilities = new DocumentFormattingClientCapabilities;
    formattingCapabilities->setDynamicRegistration(new bool(true));
    documentCapabilities->setFormatting(formattingCapabilities);
    DocumentRangeFormattingClientCapabilities *rangeFormattingCapabilites = new DocumentRangeFormattingClientCapabilities;
    rangeFormattingCapabilites->setDynamicRegistration(new bool(true));
    documentCapabilities->setRangeFormatting(rangeFormattingCapabilites);
    DocumentColorClientCapabilities *colorCapabilities = new DocumentColorClientCapabilities;
    colorCapabilities->setDynamicRegistration(new bool(true));
    documentCapabilities->setColorProvider(colorCapabilities);
    CompletionTextDocumentClientCapabilities *completionCapabilities = new CompletionTextDocumentClientCapabilities();
    CompletionTextDocumentClientCapabilitiesItem *completionCapabilitiesItem = new CompletionTextDocumentClientCapabilitiesItem();
    completionCapabilitiesItem->setCommitCharactersSupport(new bool(true));
    completionCapabilitiesItem->setInsertReplaceSupport(new bool(true));
    completionCapabilitiesItem->setSnippetSupport(new bool(true));
    completionCapabilities->setCompletionItem(completionCapabilitiesItem);
    documentCapabilities->setCompletion(completionCapabilities);
    TextDocumentSyncClientCapabilities *documentClientCapabilities = new TextDocumentSyncClientCapabilities();
    documentClientCapabilities->setDynamicRegistration(new bool(true));
    documentClientCapabilities->setWillSave(new bool(true));
    documentClientCapabilities->setWillSaveWaitUntil(new bool(true));
    documentClientCapabilities->setDidSave(new bool(true));
    documentCapabilities->setSynchronization(documentClientCapabilities);
    clientCapabilites->setTextDocument(documentCapabilities);
    params->setCapabilities(clientCapabilites);
    documentClientCapabilities->setWillSaveWaitUntil(new bool(true));
    documentClientCapabilities->setWillSave(new bool(true));
    WorkspaceClientCapabilities *workspaceCapabilities = new WorkspaceClientCapabilities();
    workspaceCapabilities->setWorkspaceFolders(new bool(true));
    DidChangeConfigurationWorkspaceClientCapabilities *changeWorkspaceCapabilites = new DidChangeConfigurationWorkspaceClientCapabilities;
    changeWorkspaceCapabilites->setDynamicRegistration(new bool(true));
    workspaceCapabilities->setDidChangeConfiguration(changeWorkspaceCapabilites);
    clientCapabilites->setWorkspace(workspaceCapabilities);
    PublishDiagnosticsClientCapabilities *diagnostics = new PublishDiagnosticsClientCapabilities();
    diagnostics->setCodeDescriptionSupport(new bool(true));
    diagnostics->setDataSupport(new bool(true));
    diagnostics->setRelatedInformation(new bool(true));
    diagnostics->setVersionSupport(new bool(true));
    documentCapabilities->setPublishDiagnostics(diagnostics);
    QList<WorkspaceFolder*> * folders = new QList<WorkspaceFolder*>();
    for(auto &f : _folders) {
        auto folder = new WorkspaceFolder();
        folder->setUri(new DocumentURI("file://"+f));
        folders->append(folder);
    }
    //params->setRootUri(new DocumentURI("file://"+wFolder));
    params->setWorkspaceFolders(folders);

    auto list = m_envManager->currentEnvironment().toStringList();
    QJsonObject env;
    for(const QString &var : list) {
        auto pos = var.indexOf("=");
        const QString name = var.left(pos);
        const QString value = var.mid(pos+1);
        env.insert(name, value);
    }

    QJsonObject settings;
    settings.insert("env", env);
    settings.insert("experimentalWorkspaceModule", true);
    settings.insert("experimentalUseInvalidMetadata", true);
    settings.insert("allowModfileModifications", true);
    settings.insert("allowImplicitNetworkAccess", true);
    settings.insert("staticcheck", true);
    settings.insert("linksInHover", false);
    settings.insert("completeUnimported", true);
    QJsonObject analyses;
    analyses.insert("nilness", true);
    analyses.insert("shadow", true);
    settings.insert("analyses", analyses);
    params->setInitializationOptions(new QJsonValue(settings));
    clientCapabilites->setExperimental(new QJsonValue(settings));
    CodeLensClientCapabilities *codeLens = new CodeLensClientCapabilities;
    codeLens->setDynamicRegistration(new bool(true));
    documentCapabilities->setCodeLens(codeLens);

    CodeActionClientCapabilities *codeAction = new CodeActionClientCapabilities;
    CodeActionClientCapabilitiesLiteralSupport *codeActionLiteral = new CodeActionClientCapabilitiesLiteralSupport;
    CodeActionClientCapabilitiesKind *codeActionLiteralKind = new CodeActionClientCapabilitiesKind;
    QList<CodeActionKind*> *actions = new QList<CodeActionKind*>();
    *actions << new CodeActionKind(SourceOrganizeImports);
    *actions << new CodeActionKind(RefactorRewrite);
    codeActionLiteralKind->setValueSet(actions);
    codeActionLiteral->setCodeActionKind(codeActionLiteralKind);
    codeAction->setCodeActionLiteralSupport(codeActionLiteral);
    documentCapabilities->setCodeAction(codeAction);

    sendCommand(MethodInitialize, params, DECODE_CALLBACK(&GoPlsServer::decodeInitialize), true);
}

void GoPlsServer::askAutocomplete(const QString &path, unsigned int line, unsigned int column)
{
    QSharedPointer<CompletionParams> params(new CompletionParams());
    params->setTextDocument(documentIdentifier(path));
    Position *position = new Position();
    position->setLine(new unsigned int(line));
    position->setCharacter(new unsigned int(column));
    params->setPosition(position);
    CompletionContext *context = new CompletionContext();
    context->setTriggerCharacter(new QString("."));
    context->setTriggerKind(new CompletionTriggerKind(CompletionTriggerKindTriggerCharacter));
    params->setContext(context);
    sendCommand(MethodTextDocumentCompletion, params, DECODE_CALLBACK(&GoPlsServer::decodeDocumentCompletion));
}

void GoPlsServer::askSignatureHelp(const QString &path, unsigned int line, unsigned int column, const QString &trigger)
{
    QSharedPointer<SignatureHelpParams> params(new SignatureHelpParams);
    SignatureHelpContext *context = new SignatureHelpContext;
    context->setTriggerKind(new SignatureHelpTriggerKind(SignatureHelpTriggerKindTriggerCharacter));
    context->setTriggerCharacter(new QString(trigger));
    params->setContext(context);
    params->setTextDocument(documentIdentifier(path));
    Position *position = new Position;
    position->setLine(new unsigned int(line));
    position->setCharacter(new unsigned int(column));
    params->setPosition(position);
    sendCommand(MethodTextDocumentSignatureHelp, params, DECODE_CALLBACK(&GoPlsServer::decodeSignatureHelp));
}

void GoPlsServer::addWorkspaceFolder(const QString &folder)
{
    m_init = false;
    QSharedPointer<DidChangeWorkspaceFoldersParams> params(new DidChangeWorkspaceFoldersParams);
    WorkspaceFoldersChangeEvent *event = new WorkspaceFoldersChangeEvent();
    QList<WorkspaceFolder*> *folders = new QList<WorkspaceFolder*>();
    WorkspaceFolder *wFolder = new WorkspaceFolder();
    wFolder->setName(new QString(QFileInfo(folder).baseName()));
    wFolder->setUri(new QString("file://"+folder));
    folders->append(wFolder);
    event->setAdded(folders);
    params->setEvent(event);
    sendCommand(MethodWorkspaceDidChangeWorkspaceFolders, params, DECODE_CALLBACK(&GoPlsServer::decodeAddWorkspaceFolder), true);
}

void GoPlsServer::askDefinitions(const QString &file, bool hover, unsigned int line, unsigned int column)
{
    QSharedPointer<DefinitionParams> params(new DefinitionParams());
    params->setTextDocument(documentIdentifier(file));
    Position *position = new Position();
    position->setLine(new unsigned int(line));
    position->setCharacter(new unsigned int(column));
    params->setPosition(position);
    if(hover) {
        sendCommand(MethodTextDocumentDefinition, params, DECODE_CALLBACK(&GoPlsServer::decodeDocumentDefinitionHover));
    }else{
        sendCommand(MethodTextDocumentDefinition, params, DECODE_CALLBACK(&GoPlsServer::decodeDocumentDefinitionGoTo));
    }
}

void GoPlsServer::formatDocument(const QString &file)
{
    QSharedPointer<DocumentFormattingParams> params(new DocumentFormattingParams());
    params->setTextDocument(documentIdentifier(file));
    sendCommand(MethodTextDocumentFormatting, params, DECODE_CALLBACK(&GoPlsServer::decodeDocumentFormatting));
}

void GoPlsServer::organizeImports(const QString &file)
{
    QSharedPointer<CodeActionParams> params(new CodeActionParams());
    params->setTextDocument(documentIdentifier(file));
    CodeActionContext *context = new CodeActionContext;
    QList<CodeActionKind*> *actions = new QList<CodeActionKind*>();
    *actions << new CodeActionKind(SourceOrganizeImports);
    *actions << new CodeActionKind(RefactorRewrite);
    context->setOnly(actions);
    params->setContext(context);
    sendCommand(MethodTextDocumentCodeAction, params, DECODE_CALLBACK(&GoPlsServer::decodeOrganizeImport));
}

void GoPlsServer::documentHighlight(const QString &file, int startLine, int startColumn, int endLine, int endColumn)
{
    QSharedPointer<DocumentHighlightParams> params(new DocumentHighlightParams);
    sendCommand(MethodTextDocumentDocumentHighlight, params, nullptr);
}

void GoPlsServer::fileOpened(const QString &file, const QString &content)
{
        m_openedFiles.insert(file, true);
        QSharedPointer<DidOpenTextDocumentParams> params(new DidOpenTextDocumentParams());
        TextDocumentItem *item = new TextDocumentItem();
        item->setUri(new DocumentURI("file://"+file));
        item->setLanguageId(new LanguageIdentifier(GoLanguage));
        item->setText(new QString(content));
        params->setTextDocument(item);
        sendCommand(MethodTextDocumentDidOpen, params, DECODE_CALLBACK(&GoPlsServer::decodeDidOpened));
}

void GoPlsServer::fileClosed(const QString &file)
{
    m_openedFiles.remove(file);
    m_filesVersions.remove(file);

    QSharedPointer<DidCloseTextDocumentParams> params(new DidCloseTextDocumentParams());
    params->setTextDocument(documentIdentifier(file));
    sendCommand(MethodTextDocumentDidClose, params, DECODE_CALLBACK(&GoPlsServer::decodeDidClosed));
}

void GoPlsServer::hover(const QString &filename, int line, int column)
{
    QSharedPointer<HoverParams> params(new HoverParams);
    params->setTextDocument(documentIdentifier(filename));
    Position *pos = new Position;
    pos->setCharacter(new unsigned int(column));
    pos->setLine(new unsigned int(line));
    params->setPosition(pos);
    sendCommand(MethodTextDocumentHover, params, DECODE_CALLBACK(&GoPlsServer::decodeHover));
}

void GoPlsServer::documentSymbols(const QString &filename)
{
    QSharedPointer<DocumentSymbolParams> params(new DocumentSymbolParams);
    params->setTextDocument(documentIdentifier(filename));
    sendCommand(MethodTextDocumentDocumentSymbol, params, DECODE_CALLBACK(&GoPlsServer::decodeDocumentSymbols));
}

void GoPlsServer::enableStaticcheck(bool v)
{

    QSharedPointer<DidChangeConfigurationParams> params(new DidChangeConfigurationParams());
    QJsonObject settings;
    settings.insert("staticcheck", v);
    params->setSettings(new QJsonValue(settings));
    sendCommand(MethodWorkspaceDidChangeConfiguration, params, DECODE_CALLBACK(&GoPlsServer::decodeCurrentEnvChanged));
}

void GoPlsServer::shutdown()
{
    sendCommand(MethodShutdown, nullptr, DECODE_CALLBACK(&GoPlsServer::decodeShutdown));
}

void GoPlsServer::exit()
{
    sendCommand(MethodExit, nullptr, DECODE_CALLBACK(&GoPlsServer::decodeExit));
}

void GoPlsServer::decodeInitialize(const QJsonObject &resp)
{
    QSharedPointer<InitializedParams> params(new InitializedParams());
    sendCommand(MethodInitialized, params, DECODE_CALLBACK(&GoPlsServer::decodeInitialized), true);
}

void GoPlsServer::decodeInitialized(const QJsonObject &resp)
{
    m_init = true;
    if(!m_waitingCommands.isEmpty()) {
        executeCommand(m_waitingCommands.takeFirst());
    }
}

QList<DefinitionResult> GoPlsServer::decodeDocumentDefinition(const QJsonObject &jsonObject)
{
    QJsonArray locations = jsonObject.value("result").toArray();
    QList<DefinitionResult> list;
    for(const QJsonValue &value : locations) {
        Location *location = new Location();
        location->fromJson(value.toObject());
        DefinitionResult result;
        result.path = location->getUri()->replace("file://", "");
        const auto pos = location->getRange()->getEnd();
        result.line = *pos->getLine();
        result.column = *pos->getCharacter();
        list << result;
    }
    return list;
}

void GoPlsServer::decodeDocumentDefinitionHover(const QJsonObject &response)
{
    auto list = decodeDocumentDefinition(response);
    emit hoverDefinitionResult(list);
}

void GoPlsServer::decodeDocumentDefinitionGoTo(const QJsonObject &response)
{
    auto list = decodeDocumentDefinition(response);
    emit definitionsResult(list);
}

void GoPlsServer::decodeDocumentCompletion(const QJsonObject &jsonObject)
{
    CompletionList list;
    list.fromJson(jsonObject.value("result").toObject());
    QList<AutoCompletionResult> completions;
    if(list.getItems()) {
        for(auto it = list.getItems()->cbegin(); it != list.getItems()->cend(); ++it) {
            AutoCompletionResult result;
            auto label = (*it)->getLabel();
            auto detail = (*it)->getDetail();
            auto kind = (*it)->getKind();
            if(label) {
                result.label = *label;
            }
            if(detail) {
                result.detail = *detail;
            }
            if(kind) {
                switch (int(*kind)) {
                case CompletionItemKindValue:
                case CompletionItemKindConstant:
                    result.type = "const";
                    break;
                case CompletionItemKindFunction:
                    result.type = "func";
                    break;
                case CompletionItemKindInterface:
                case CompletionItemKindStruct:
                    result.type = "struct";
                    break;
                case CompletionItemKindVariable:
                    result.type = "var";
                    break;
                case CompletionItemKindModule:
                    result.type = "package";
                    break;
                default:
                    result.type = "type";
                    break;
                }
            }
            completions << result;
        }
    }
    emit autocompleteResult(completions);
}

QList<TextEditResult> convertTextEditResult(QJsonArray array) {
    QList<TextEditResult> list;
    for(const auto it : array) {
        TextEdit edit;
        edit.fromJson(it.toObject());
        TextEditResult result;
        result.text = *edit.getNewText();
        result.startLine = *edit.getRange()->getStart()->getLine();
        result.startColumn = *edit.getRange()->getStart()->getCharacter();
        result.endLine = *edit.getRange()->getEnd()->getLine();
        result.endColumn = *edit.getRange()->getEnd()->getCharacter();
        list << result;
    }
    return list;
}

void GoPlsServer::decodeDocumentFormatting(const QJsonObject &jsonObject)
{
    const QJsonArray array = jsonObject.value("result").toArray();
    emit formattingResults(convertTextEditResult(array));
}

void GoPlsServer::decodeOrganizeImport(const QJsonObject &jsonObject)
{
    QJsonArray results = jsonObject.value("result").toArray();
    for(const auto &result : results) {
        QJsonArray edits = result.toObject().value("edit").toObject().value("documentChanges").toArray();
        for(const auto &edit : edits) {
            const QJsonObject obj = edit.toObject();
            const QString filename = obj.value("textDocument").toObject().value("uri").toString().mid(7);
            const QList<TextEditResult> modifications(convertTextEditResult(obj.value("edits").toArray()));
            emit updateFile(filename, modifications);
        }
    }
}

void GoPlsServer::decodeHover(const QJsonObject &response)
{
    if(response.value("result").isNull()){
        return;
    }
    auto contents = response.value("result");
    if(contents.isArray()) {

    } else {
        Hover hover;
        hover.fromJson(contents.toObject());
        auto range = hover.getRange();
        HoverResult result;
        result.startLine = *range->getStart()->getLine();
        result.startColumn = *range->getStart()->getCharacter();
        result.endLine = *range->getEnd()->getLine();
        result.endColumn = *range->getEnd()->getCharacter();
        result.info = *hover.getContents()->getValue();
        emit hoverResult({result});
    }
}

void GoPlsServer::decodeDiagnostics(const QJsonObject &response)
{
    PublishDiagnosticsParams result;
    result.fromJson(response.value("params").toObject());
    QList<DiagnosticResult> list;
    auto diags = *result.getDiagnostics();
    for(auto it : diags) {
        DiagnosticResult diag;
        diag.message = *it->getMessage();
        if(it->getSource()) {
            diag.code = *it->getSource();
        }
        diag.line = *it->getRange()->getStart()->getLine()+1;

        /*
        DiagnosticSeverityError DiagnosticSeverity = 1

        // DiagnosticSeverityWarning reports a warning.
        DiagnosticSeverityWarning DiagnosticSeverity = 2

        // DiagnosticSeverityInformation reports an information.
        DiagnosticSeverityInformation DiagnosticSeverity = 3

        // DiagnosticSeverityHint reports a hint.
        DiagnosticSeverityHint DiagnosticSeverity = 4
         */
        switch (int(*it->getSeverity())) {
        case 1:
            diag.level = "error";
            break;
        case 2:
            diag.level = "warning";
            break;
        case 3:
            diag.level = "info";
            break;
        case 4:
            diag.level = "hint";
            break;
        }
        list << diag;
    }
    emit diagnosticsInfo(result.getUri()->mid(7), list);
}

void GoPlsServer::decodeDocumentSymbols(const QJsonObject &response)
{
    const auto symbols = response.value("result").toArray();
    QHash<QString, QList<LiteApi::Symbol>> result;
    for(auto it : symbols) {
        SymbolInformation symbol;
        symbol.fromJson(it.toObject());
        LiteApi::Symbol res;
        res.startLine = *symbol.getLocation()->getRange()->getStart()->getLine();
        res.endLine = *symbol.getLocation()->getRange()->getEnd()->getLine();
        res.name = *symbol.getName();

        const QString filename = symbol.getLocation()->getUri()->mid(7);
        auto list = result[filename];
        list << res;
        result[filename] = list;
    }
    for(auto it = result.cbegin(); it != result.cend(); ++it){
        emit documentSymbolsResult(it.key(), it.value());
    }
}

void GoPlsServer::decodeDidChanged(const QJsonObject &response)
{
}

void GoPlsServer::decodeDidSaved(const QJsonObject &response)
{
}

void GoPlsServer::decodeShutdown(const QJsonObject &response)
{
    exit();
}

void GoPlsServer::decodeExit(const QJsonObject &response)
{

}

void GoPlsServer::decodeAddWorkspaceFolder(const QJsonObject &response)
{
    m_init = true;
    if(!m_waitingCommands.isEmpty()) {
        executeCommand(m_waitingCommands.takeFirst());
    }
}

void GoPlsServer::decodeCurrentEnvChanged(const QJsonObject &response)
{

}

void GoPlsServer::decodeSignatureHelp(const QJsonObject &response)
{
    printResponse(response);
}

void GoPlsServer::printResponse(const QJsonObject &response)
{
    qDebug().noquote() << response;
}

void GoPlsServer::decodeDidOpened(const QJsonObject &response)
{
}

void GoPlsServer::decodeDidClosed(const QJsonObject &response)
{
}

void GoPlsServer::fileSaved(const QString &file, const QString &content)
{
    QSharedPointer<DidSaveTextDocumentParams> params(new DidSaveTextDocumentParams());
    params->setTextDocument(documentIdentifier(file));
    params->setText(new QString(content));
    sendCommand(MethodTextDocumentDidSave, params, DECODE_CALLBACK(&GoPlsServer::decodeDidSaved));
}

void GoPlsServer::fileChanged(const QString &file, int startLine, int startPos, int endLine, int endPos, const QString &content)
{
    QSharedPointer<DidChangeTextDocumentParams> params(new DidChangeTextDocumentParams());
    VersionedTextDocumentIdentifier *item = new VersionedTextDocumentIdentifier();
    item->setUri(new DocumentURI("file://"+file));
    const int version = m_filesVersions.value(file);
    m_filesVersions[file] = version+1;
    item->setVersion(new int(version));
    params->setTextDocument(item);
    QList<TextDocumentContentChangeEvent *> *list = new QList<TextDocumentContentChangeEvent *>();
    TextDocumentContentChangeEvent *text = new TextDocumentContentChangeEvent();
    Range *range = new Range();
    Position *start = new Position();
    start->setCharacter(new unsigned int(startPos));
    start->setLine(new unsigned int(startLine));
    range->setStart(start);
    Position *end = new Position();
    end->setCharacter(new unsigned int(endPos));
    end->setLine(new unsigned int(endLine));
    range->setEnd(end);
    text->setRange(range);
    text->setText(new QString(content));
    list->append(text);
    params->setContentChanges(list);
    sendCommand(MethodTextDocumentDidChange, params, DECODE_CALLBACK(&GoPlsServer::decodeDidChanged));
}

TextDocumentIdentifier *GoPlsServer::documentIdentifier(const QString &path) const
{
    TextDocumentIdentifier *identifier = new TextDocumentIdentifier();
    identifier->setUri(new DocumentURI("file://"+path));
    return identifier;
}

void GoPlsServer::decodeResponse(const QByteArray &payload)
{
    if(payload.isEmpty()) {
        return;
    }
    QJsonObject jsonObject = QJsonDocument::fromJson(payload).object();
    const int commandID = jsonObject.value("id").toInt();

    if (commandID == 0 ){
        const QString method = jsonObject.value("method").toString();
        if(method == MethodWindowLogMessage || method == MethodWindowShowMessage){
            LogMessageParams message;
            message.fromJson(jsonObject.value("params").toObject());
            if(message.getMessage()) {
                bool isError = message.getType() && *message.getType() == MessageTypeError;
                emit logMessage(*message.getMessage(), isError);
            }
        }else if(method == MethodTextDocumentPublishDiagnostics) {
            decodeDiagnostics(QJsonDocument::fromJson(payload).object());
        }else if(!payload.isEmpty()){
            qDebug().noquote() << payload;
        }
    }else{
        const QString requested = m_idToCommands.take(commandID);
        DecodeFunc decodeFunc = m_idToDecodeFunc.take(commandID);
        if(decodeFunc) {
            decodeFunc(jsonObject);
        }else{
            qDebug().noquote() << requested << payload;
        }
    }
    if(!m_waitingCommands.empty() && m_init) {
        executeCommand(m_waitingCommands.takeFirst());
    }
}

void GoPlsServer::sendCommand(const QString &command, const QSharedPointer<GoPlsParams> &params, const DecodeFunc &responseFunc, bool force)
{
    GoPlsCommand cmd(command, params, responseFunc);
    if(!m_init && !force) {
        m_waitingCommands << cmd;
    }else{
        executeCommand(cmd);
    }
}

void GoPlsServer::executeCommand(const GoPlsCommand &cmd)
{
    auto cmdID = cmd.commandID();
    m_idToCommands.insert(cmdID, cmd.method());
    m_idToDecodeFunc.insert(cmdID, cmd.decodeFunc());
    QTimer::singleShot(1000, [this, cmdID]{
        m_idToCommands.take(cmdID);
        m_idToDecodeFunc.take(cmdID);
    });
    const QByteArray raw = cmd.toJson();
    const QString payload = QString("Content-Length: %1\r\n\r\n%2").arg(raw.length()).arg(QString::fromUtf8(raw));
    m_process->write(payload.toUtf8());
}

void GoPlsServer::onReadyRead()
{
    const QByteArray payload = m_process->readAll();
    if(payload.isEmpty()) {
        return;
    }
    int contentLength = 0;
    int offset = 0;
    if(payload.length() == 0) {
        return;
    }
    if(payload.indexOf("\r\n") < 0) {
        decodeResponse(payload);
        return;
    }
    while(true) {
        const int next = payload.indexOf("\r\n", offset);
        if (next != offset) {
            QString header = payload.mid(offset, next-offset);
            if(header.startsWith("Content-Length:", Qt::CaseInsensitive)) {
                QString size = header.remove("Content-Length:", Qt::CaseInsensitive).trimmed();
                contentLength = size.toInt();
            }
            offset = next+2;
        }else{
            const QByteArray body = payload.mid(offset+2, contentLength);
            decodeResponse(body);
            offset += contentLength+2;
        }
        if(offset >= payload.length()) {
            break;
        }
    }
}

void GoPlsServer::onStarted()
{
}

void GoPlsServer::currentEnvChanged(LiteApi::IEnv *env)
{
    if(env) {
        auto list = env->environment().toStringList();
        QJsonObject obj;

        for(const QString &var : list) {
            auto pos = var.indexOf("=");
            const QString name = var.left(pos);
            const QString value = var.mid(pos+1);
            obj.insert(name, value);
        }

        QSharedPointer<DidChangeConfigurationParams> params(new DidChangeConfigurationParams());
        QJsonObject settings;
        settings.insert("env", obj);
        params->setSettings(new QJsonValue(settings));
        sendCommand(MethodWorkspaceDidChangeConfiguration, params, DECODE_CALLBACK(&GoPlsServer::decodeCurrentEnvChanged));
    }
}
