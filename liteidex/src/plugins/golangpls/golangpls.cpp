#include "golangpls.h"

#include <QDebug>
#include "goplsserver.h"
#include "quickopenapi/quickopenapi.h"
#include "golangpls_global.h"
#include "../liteeditor/liteeditor.h"
#include "golangplshighlighterfactory.h"
#include "golangplshighlighter.h"

GolangPls::GolangPls(LiteApi::IApplication *app, QObject *parent)
    : QObject(parent)
    , m_liteApp(app)
    , m_completer(nullptr)
    , m_server(new GoPlsServer(app))
    , m_isWorkspaceInitialized(false)
{
    LiteApi::IActionContext *actionContext = m_liteApp->actionManager()->getActionContext(this, "GolangPls");

    m_jumpDeclAct = new QAction(tr("Jump to Declaration"), this);
    actionContext->regAction(m_jumpDeclAct, "JumpToDeclaration!", "CTRL+SHIFT+J;F2");
    connect(m_jumpDeclAct, &QAction::triggered, this, &GolangPls::editorJumpToDecl);

    m_refactorAct = new QAction(tr("Rename symbol"), this);
    actionContext->regAction(m_refactorAct, "RenameSymbol!", "CTRL+SHIFT+R");
    connect(m_refactorAct, &QAction::triggered, this, &GolangPls::renameSymbol);

    m_usageAct = new QAction(tr("Find usages"), this);
    actionContext->regAction(m_usageAct, "FindUsages!", "CTRL+SHIFT+U");
    connect(m_usageAct, &QAction::triggered, this, &GolangPls::findUsage);

    connect(m_liteApp, &LiteApi::IApplication::sessionListChanged, this, &GolangPls::appLoaded);
    connect(m_server, &GoPlsServer::workspaceInitialized, this, &GolangPls::onWorkspaceInitialized);
    connect(m_server, &GoPlsServer::logMessage, this, &GolangPls::onLogMessage);
    connect(m_server, &GoPlsServer::autocompleteResult, this, &GolangPls::onAutoCompletion);
    connect(m_server, &GoPlsServer::definitionsResult, this, &GolangPls::onDefinitionResult);
    connect(m_server, &GoPlsServer::formattingResults, this, &GolangPls::onFormattingResults);
    connect(m_server, &GoPlsServer::updateFile, this, &GolangPls::onUpdateFile);
    connect(m_server, &GoPlsServer::hoverResult, this, &GolangPls::onHoverResult);
    connect(m_server, &GoPlsServer::hoverDefinitionResult, this, &GolangPls::onHoverDefinitionResult);
    connect(m_server, &GoPlsServer::diagnosticsInfo, this, &GolangPls::onDiagnosticsInfo);
    connect(m_server, &GoPlsServer::documentSymbolsResult, this, &GolangPls::onDocumentSymbolsResult);
    connect(m_server, &GoPlsServer::semanticTokensResult, this, &GolangPls::onSemanticTokensResult);
    connect(m_server, &GoPlsServer::foldingRangeResult, this, &GolangPls::onFoldingRangeResult);

    connect(m_liteApp->editorManager(), &LiteApi::IEditorManager::currentEditorChanged, this, &GolangPls::currentEditorChanged);
    connect(m_liteApp->editorManager(), &LiteApi::IEditorManager::editorCreated, this, &GolangPls::editorCreated);
    connect(m_liteApp->editorManager(), &LiteApi::IEditorManager::editorAboutToClose, this, &GolangPls::editorClosed);
    connect(m_liteApp->editorManager(), &LiteApi::IEditorManager::editorAboutToSave, this, &GolangPls::editorSaved);

    connect(m_liteApp->optionManager(), SIGNAL(applyOption(QString)), this, SLOT(applyOption(QString)));
    m_liteApp->optionManager()->emitApplyOption(OPTION_GOLANGPLS);
    //connect(m_liteApp->editorManager(), &LiteApi::IEditorManager::editorModifyChanged, this, &GolangPls::editorChanged);

    //connect(m_liteApp->optionManager(),&LiteApi::IOptionManager::applyOption,this, &GolangPls::applyOption);
    m_fileSearch = new GolangPlsUsage(m_liteApp);
    LiteApi::IFileSearchManager *manager = LiteApi::getFileSearchManager(app);
    if (manager) {
        manager->addFileSearch(m_fileSearch);
        connect(m_server, &GoPlsServer::findUsageResult, m_fileSearch, &GolangPlsUsage::loadResults);
    }
}

GolangPls::~GolangPls()
{
    QEventLoop loop;
    connect(m_server, &GoPlsServer::exited, &loop, &QEventLoop::quit);
    m_server->shutdown();
    loop.exec();
    m_server->quit();
}

void GolangPls::setCompleter(LiteApi::ICompleter *completer)
{
    if (m_completer) {
        disconnect(m_completer, 0, this, 0);
    }
    m_completer = completer;
    if (m_completer) {
        m_completer->setSearchSeparator(false);
        m_completer->setExternalMode(true);
        connect(m_completer, SIGNAL(prefixChanged(QTextCursor, QString, bool)), this, SLOT(prefixChanged(QTextCursor, QString, bool)));
        connect(m_completer, SIGNAL(wordCompleted(QString, QString, QString)), this, SLOT(wordCompleted(QString, QString, QString)));
    }
}

void GolangPls::setHighlightFactory(GolangPlsHighlighterFactory *factory)
{
    m_highlightFactory = factory;
}

void GolangPls::currentEditorChanged(LiteApi::IEditor *editor)
{
    if (!editor) {
        this->setCompleter(0);
        return;
    }

    auto liteEditor = LiteApi::getLiteEditor(editor);
    if (liteEditor) {
        liteEditor->clearAnnotations("staticcheck");
    }

    if (editor->mimeType() == "text/x-gosrc") {
        LiteApi::ICompleter *completer = LiteApi::findExtensionObject<LiteApi::ICompleter *>(editor, "LiteApi.ICompleter");
        this->setCompleter(completer);
    } else if (editor->mimeType() == "browser/goplay") {
        LiteApi::IEditor *pedit = LiteApi::findExtensionObject<LiteApi::IEditor *>(m_liteApp->extension(), "LiteApi.Goplay.IEditor");
        if (pedit && pedit->mimeType() == "text/x-gosrc") {
            editor = pedit;
            LiteApi::ICompleter *completer = LiteApi::findExtensionObject<LiteApi::ICompleter *>(editor, "LiteApi.ICompleter");
            this->setCompleter(completer);
        }
    } else {
        this->setCompleter(0);
        return;
        if (m_currentFile != "") {
            m_server->fileClosed(m_currentFile);
        }
    }

    m_editor = LiteApi::getTextEditor(editor);
    if (!m_editor) {
        return;
    }

    m_currentFile = m_editor->filePath();
    QPlainTextEdit *plainTextEditor = LiteApi::getPlainTextEdit(m_editor);
    m_plainText = plainTextEditor->toPlainText();
}

void GolangPls::editorCreated(LiteApi::IEditor *editor)
{
    if (editor->mimeType() != "text/x-gosrc") {
        return;
    }

    //editor->widget()->addAction(m_commentAct);
    QMenu *menu = LiteApi::getEditMenu(editor);
    if (menu) {
        menu->addAction(m_jumpDeclAct);
        menu->addAction(m_refactorAct);
        menu->addAction(m_usageAct);
    }

    menu = LiteApi::getContextMenu(editor);
    if (menu) {
        menu->addAction(m_jumpDeclAct);
        menu->addAction(m_refactorAct);
        menu->addAction(m_usageAct);
    }

    QString filePath = editor->filePath();
    if (filePath.isEmpty()) {
        return;
    }
    connect(editor, &LiteApi::IEditor::contentsChanged, this, &GolangPls::editorChanged);
    connect(editor, SIGNAL(updateLink(QTextCursor, QPoint, bool)), this, SLOT(onUpdateLink(QTextCursor, QPoint, bool)));

    QPlainTextEdit *plainTextEditor = LiteApi::getPlainTextEdit(editor);
    if (m_isWorkspaceInitialized) {
        auto workspaceDirectory = findModulePath(filePath);
        if (!m_opendWorkspace.contains(workspaceDirectory) && workspaceDirectory != "/") {
            m_server->updateWorkspaceFolders({workspaceDirectory}, {});
            m_opendWorkspace.insert(workspaceDirectory, true);
        }
        m_server->fileOpened(editor->filePath(), plainTextEditor->toPlainText());
        m_server->documentSymbols(editor->filePath());
    }

    const QString filepath = editor->filePath();
    m_plainText = plainTextEditor->toPlainText();
    connect(plainTextEditor, &QPlainTextEdit::textChanged, [this, filepath, plainTextEditor]() {
        if (m_plainText == plainTextEditor->toPlainText()) {
            return;
        }
        int startLine, startPos, endLine, endPos;
        QString content;
        computeModifications(m_plainText, plainTextEditor->toPlainText(), startLine, startPos, endLine, endPos, content);
        qDebug() << startLine << endLine;
        m_server->fileChanged(filepath, startLine, startPos, endLine, endPos, content);
        m_plainText = plainTextEditor->toPlainText();
    });
}

void GolangPls::editorClosed(LiteApi::IEditor *editor)
{
    auto folders = activeWorkspaces();
    QStringList remove;
    for (const auto &folder : m_opendWorkspace.keys()) {
        if (!folders.contains(folder)) {
            remove << folder;
            m_opendWorkspace.remove(folder);
        }
    }
    m_server->updateWorkspaceFolders({}, remove);
    // editor is the new one, get the last one
    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    if (textEditor) {
        m_server->fileClosed(textEditor->filePath());
    }
}

void GolangPls::editorSaved(LiteApi::IEditor *editor)
{
    LiteApi::ITextEditor *currentEditor = LiteApi::getTextEditor(editor);
    //    m_server->organizeImports(currentEditor->filePath());
    m_server->formatDocument(currentEditor->filePath());
    m_server->fileSaved(currentEditor->filePath(), LiteApi::getPlainTextEdit(currentEditor)->toPlainText());
    m_liteApp->editorManager()->saveEditor(editor, false);
}

void GolangPls::editorChanged()
{
    QObject *source = QObject::sender();
    LiteApi::IEditor *editor = dynamic_cast<LiteApi::IEditor *>(source);
    if (editor) {
        m_server->documentSymbols(editor->filePath());
    }
}

void GolangPls::prefixChanged(QTextCursor cur, QString pre, bool force)
{
    m_preWord.clear();
    if (pre.endsWith('.')) {
        m_preWord = pre;
    }
    if (!m_preWord.isEmpty()) {
        m_completer->clearItemChilds(m_preWord);
    }

    const QPoint pos = cursorPosition(cur);
    m_server->askAutocomplete(m_editor->filePath(), pos.y(), pos.x());
}

void GolangPls::wordCompleted(QString, QString, QString)
{
    m_preWord.clear();
}

void GolangPls::onWorkspaceInitialized()
{
    qDebug() << "EVENT RECEIVED!";
    m_isWorkspaceInitialized = true;

    for (auto editor : m_liteApp->editorManager()->editorList()) {
        QPlainTextEdit *plainTextEditor = LiteApi::getPlainTextEdit(editor);
        m_server->fileOpened(editor->filePath(), plainTextEditor->toPlainText());
        m_server->documentSymbols(editor->filePath());
    }
}

void GolangPls::onLogMessage(const QString &message, bool isError)
{
    m_liteApp->appendLog("GolangPls", message, isError);
}

void GolangPls::onAutoCompletion(const QString &filename, const QList<AutoCompletionResult> &result)
{
    QStandardItem *root = m_completer->findRoot(m_preWord);
    for (const auto &item : result) {
        m_completer->appendChildItem(root, item.label, item.type, item.detail, QIcon(), true);
    }
    m_completer->updateCompleterModel();
    m_completer->showPopup();
}

void GolangPls::onDefinitionResult(const QString &filename, const QList<DefinitionResult> &definitions)
{
    if (definitions.isEmpty()) {
        return;
    }

    DefinitionResult result = definitions.first();
    QString fileName = result.path;
    LiteApi::gotoLine(m_liteApp, fileName, result.line, result.column, true, true);
}

void GolangPls::onFormattingResults(const QString &filename, const QList<TextEditResult> &list)
{
    auto editor = m_liteApp->editorManager()->findEditor(filename, false);
    editText(editor, list);

    m_server->organizeImports(m_currentFile);
}

void GolangPls::onUpdateFile(const QString &filename, const QList<TextEditResult> &list)
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->findEditor(filename, true);
    if (!editor) {
        QString mimeType = m_liteApp->mimeTypeManager()->findMimeTypeByFile(filename);
        editor = m_liteApp->editorManager()->openEditor(filename, mimeType);
    }

    if (!editor) {
        return;
    }

    editText(editor, list);
}

void GolangPls::onHoverResult(const QString &filename, const QList<HoverResult> &result)
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->findEditor(filename, true);
    if (editor) {
        for (auto &res : result) {
            m_lastLink.showTip = true;
            m_lastLink.targetInfo = res.info;
            fromLineAndColumnToPos(editor, res.startLine, res.startColumn, m_lastLink.linkTextStart);
            fromLineAndColumnToPos(editor, res.endLine, res.endColumn, m_lastLink.linkTextEnd);

            auto liteEditor = LiteApi::getLiteEditor(editor);
            if (liteEditor) {
                liteEditor->showLink(m_lastLink);
            }
            break;
        }
    }
}

void GolangPls::onHoverDefinitionResult(const QString &filename, const QList<DefinitionResult> &definitions)
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->findEditor(filename, true);
    if (editor) {
        for (auto def : definitions) {
            m_lastLink.targetFileName = def.path;
            m_lastLink.targetLine = def.line;
            m_lastLink.targetColumn = def.column;
            m_lastLink.showNav = true;

            auto liteEditor = LiteApi::getLiteEditor(editor);
            if (liteEditor) {
                liteEditor->showLink(m_lastLink);
            }
            break;
        }
    }
}

void GolangPls::onDiagnosticsInfo(const QString &filename, const QList<DiagnosticResult> &diagnostics)
{
    auto editor = m_liteApp->editorManager()->findEditor(filename, true);

    if (editor) {
        auto liteEditor = LiteApi::getLiteEditor(editor);
        if (liteEditor) {
            liteEditor->clearAnnotations("staticcheck");
            for (const auto &diag : diagnostics) {
                bool disabled = true;
                for (const auto &check : qAsConst(m_staticcheckEnables)) {
                    if (diag.code.startsWith(check)) {
                        disabled = false;
                        break;
                    }
                }
                qDebug() << "DIAGNOSTIC(enabled:" << !disabled << "):" << filename << ": [" << diag.code << "]" << diag.line << ": " << diag.message;
                if (disabled) {
                    continue;
                }
                LiteApi::Annotation annotation;
                annotation.content = diag.message;
                annotation.level = diag.level;
                annotation.from = "staticcheck";

                LiteApi::getEditorMark(m_editor)->addMark(diag.line-1, 1000);
                liteEditor->addAnnotation(diag.line, annotation);
            }
        }
    }
}

void GolangPls::onDocumentSymbolsResult(const QString &filename, const QList<LiteApi::Symbol> &symbols)
{
    auto editor = m_liteApp->editorManager()->findEditor(filename, false);
    if (editor) {
        auto liteEditor = LiteApi::getLiteEditor(editor);
        if (liteEditor) {
            liteEditor->loadSymbols(symbols);
        }
    }
}

void GolangPls::onSemanticTokensResult(const QString &filename, const QVariantList &list)
{
    auto editor = m_liteApp->editorManager()->findEditor(filename, false);
    qDebug() << "EDITOR=" << editor << ", factory=" << m_highlightFactory;
    if (editor && m_highlightFactory) {
        auto highlighter = m_highlightFactory->getHighlighter(editor);
        if (highlighter) {
            highlighter->onHighlightResults(list);
        }
    }
}

void GolangPls::onFoldingRangeResult(const QString &filename, const QList<FoldingRangeResult> &list)
{
    auto editor = m_liteApp->editorManager()->findEditor(filename, false);
    qDebug() << "EDITOR=" << editor << ", factory=" << m_highlightFactory;
    if (editor && m_highlightFactory) {
        auto highlighter = m_highlightFactory->getHighlighter(editor);
        if (highlighter) {
            highlighter->onFoldingResults(list);
        }
    }
}

void GolangPls::editorJumpToDecl()
{
    if (!m_editor) {
        return;
    }

    const QPoint pos = cursorPosition(m_editor->textCursor());
    m_server->askDefinitions(m_editor->filePath(), false, pos.y(), pos.x());
}

void GolangPls::renameSymbol()
{
    auto cursor = m_editor->textCursor();
    int line = 0, column = 0;
    fromPosToLineAndColumn(m_editor, m_editor->textCursor().position(), line, column);
    m_server->prepareRenameSymbol(m_editor->filePath(), line, column);
}

void GolangPls::findUsage()
{
    m_fileSearch->start();
    auto cursor = m_editor->textCursor();
    m_server->askFindUsage(m_editor->filePath(), cursor.blockNumber(), cursor.position() - cursor.block().position());
}

void GolangPls::editorFindUsages()
{

}

void GolangPls::editText(LiteApi::IEditor *liteEditor, const QList<TextEditResult> &list, bool reverse)
{
    QByteArray state = liteEditor->saveState();
    auto editor = LiteApi::getPlainTextEdit(liteEditor);
    auto cursor = editor->textCursor();
    QString text = editor->toPlainText();
    QList<TextEditResult> edits;
    for (auto it = list.crbegin(); it != list.crend(); ++it) {
        auto changes = *it;
        int startPos;
        fromLineAndColumnToPos(liteEditor, changes.startLine, changes.startColumn, startPos);

        int endPos;
        fromLineAndColumnToPos(liteEditor, changes.endLine, changes.endColumn, endPos);

        cursor.setPosition(startPos);
        cursor.setPosition(endPos, QTextCursor::KeepAnchor);
        cursor.insertText(changes.text);
    }
    liteEditor->restoreState(state);
}

void GolangPls::onUpdateLink(const QTextCursor &cursor, const QPoint &curPos, bool nav)
{
    auto editor = dynamic_cast<LiteApi::IEditor *>(sender());
    if (!editor) {
        return;
    }

    int pos = cursor.position();
    const QString text = LiteApi::getPlainTextEdit(editor)->toPlainText();
    int line = 0;
    int column = 0;
    fromPosToLineAndColumn(editor, pos, line, column);
    m_lastLink.clear();
    m_lastLink.cursorPos = curPos;
    m_server->hover(editor->filePath(), line, column);
    m_server->askDefinitions(editor->filePath(), true, line, column);
}

void GolangPls::computeModifications(const QString &original, const QString &current, int &startLine, int &startPos, int &endLine, int &endPos, QString &content)
{
    int start = 0;
    int end = 0;
    content = "";
    while (true) {
        if (start < original.length() && start < current.length()) {
            if (original.at(start) != current.at(start)) {
                break;
            }
            start++;
        } else {
            break;
        }
    }
    int fromEnd = 0;
    while (true) {
        if (original.length() - fromEnd > start && current.length() - fromEnd > start) {
            if (original.at(original.length() - fromEnd - 1) != current.at(current.length() - fromEnd - 1)) {
                break;
            }
            fromEnd++;
        } else {
            break;
        }
    }
    end = original.length() - fromEnd;
    content = current.mid(start, current.length() - fromEnd - start);
    const QString startString = original.left(start);
    const QString endString = original.left(end);
    startLine = startString.count("\n");
    startPos = start - startString.lastIndexOf("\n") - 1;
    endLine = endString.count("\n");
    endPos = end - endString.lastIndexOf("\n") - 1;
}

void GolangPls::fromLineAndColumnToPos(LiteApi::IEditor *editor, int line, int column, int &pos)
{
    auto liteEditor = LiteApi::getLiteEditor(editor);
    if (liteEditor) {
        auto doc = liteEditor->document();
        auto block = doc->findBlockByLineNumber(line);
        pos = block.position() + column;
    }
}

void GolangPls::fromPosToLineAndColumn(LiteApi::IEditor *editor, int pos, int &line, int &column)
{
    auto liteEditor = LiteApi::getLiteEditor(editor);
    if (liteEditor) {
        auto doc = liteEditor->document();
        auto block = doc->findBlock(pos);
        line = block.blockNumber();
        column = pos - block.position();
    }
}

void GolangPls::applyOption(QString id)
{
    if (id != OPTION_GOLANGPLS) {
        return;
    }

    bool sa1 = m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_SA1, true).toBool();
    bool sa2 = m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_SA2, true).toBool();
    bool sa3 = m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_SA3, true).toBool();
    bool sa4 = m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_SA4, true).toBool();
    bool sa5 = m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_SA5, true).toBool();
    bool sa6 = m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_SA6, true).toBool();
    bool sa9 = m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_SA9, true).toBool();
    bool s1 = m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_S1, true).toBool();
    bool st1 = m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_ST1, true).toBool();
    bool qf1 = m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_QF1, true).toBool();
    bool unreachable = m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_UNREACHABLE, true).toBool();
    bool compiler = m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_COMPILER, true).toBool();
    bool shadow = m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_SHADOW, true).toBool();
    bool syntax = m_liteApp->settings()->value(GOLANGPLS_STATICCHECK_SYNTAX, true).toBool();

    m_staticcheckEnables.clear();
    if (sa1) {
        m_staticcheckEnables << "SA1";
    }
    if (sa2) {
        m_staticcheckEnables << "SA2";
    }
    if (sa3) {
        m_staticcheckEnables << "SA3";
    }
    if (sa4) {
        m_staticcheckEnables << "SA4";
    }
    if (sa5) {
        m_staticcheckEnables << "SA5";
    }
    if (sa6) {
        m_staticcheckEnables << "SA6";
    }
    if (sa9) {
        m_staticcheckEnables << "SA9";
    }
    if (s1) {
        m_staticcheckEnables << "S1";
    }
    if (st1) {
        m_staticcheckEnables << "ST1";
    }
    if (qf1) {
        m_staticcheckEnables << "QF1";
    }
    if (unreachable) {
        m_staticcheckEnables << "unreachable";
    }
    if (compiler) {
        m_staticcheckEnables << "compiler";
    }
    if (shadow) {
        m_staticcheckEnables << "shadow";
    }
    if (syntax) {
        m_staticcheckEnables << "syntax";
    }
    qDebug() << "STATIC CHECK ENABLED" << m_staticcheckEnables;
}

void GolangPls::appLoaded()
{
    qDebug() << "APP LOADED!!!!";
    for (auto folder : activeWorkspaces()) {
        m_opendWorkspace[folder] = true;
    }

    m_server->initWorkspace(m_opendWorkspace.keys());
}

QString GolangPls::findModulePath(const QString &filepath) const
{
    QString workspaceDirectory = QFileInfo(filepath).absoluteDir().absolutePath();
    while (workspaceDirectory != "/") {
        if (QFile::exists(workspaceDirectory + "/go.mod")) {
            break;
        }
        workspaceDirectory = QFileInfo(workspaceDirectory).absoluteDir().absolutePath();
    }
    return workspaceDirectory;
}

QStringList GolangPls::activeWorkspaces() const
{
    QHash<QString, bool> folders;
    for (auto editor : m_liteApp->editorManager()->editorList()) {
        auto path = findModulePath(editor->filePath());
        if (path == "/") {
            continue;
        }
        folders[path] = true;
    }

    return folders.keys();
}

QPoint GolangPls::cursorPosition(QTextCursor cur) const
{
    const int column = cur.columnNumber();
    cur.movePosition(QTextCursor::StartOfLine);

    int lines = 1;
    while (cur.positionInBlock() > 0) {
        cur.movePosition(QTextCursor::Up);
        lines++;
    }
    QTextBlock block = cur.block().previous();

    while (block.isValid()) {
        lines += block.lineCount();
        block = block.previous();
    }
    return QPoint(column, lines - 1);
}
