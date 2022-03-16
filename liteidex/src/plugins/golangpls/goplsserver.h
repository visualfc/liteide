#ifndef GOPLSSERVER_H
#define GOPLSSERVER_H

#include <QEventLoop>
#include <QObject>
#include <QProcess>
#include <goplstypes.h>
#include "liteapi/liteapi.h"
#include "liteeditorapi/liteeditorapi.h"

namespace GoPlsTypes
{
class TextDocumentIdentifier;
class Range;
class Position;
}

class GoPlsServer: public QObject
{
    Q_OBJECT
    QProcess *m_process;

protected:
    void sendCommand(const QString &command, const QSharedPointer<GoPlsParams> &params, const DecodeFunc &responseFunc, const QString &filepath, bool force = false);
    void executeCommand(const GoPlsCommand &cmd);

signals:
    void workspaceInitialized();
    void responseReceived(int commandID, const QByteArray &response);
    void autocompleteResult(const QString &file, const QList<AutoCompletionResult> &);
    void definitionsResult(const QString &file, const QList<DefinitionResult> &definitions);
    void logMessage(const QString &message, bool isError);
    void formattingResults(const QString &file, const QList<TextEditResult> &list);
    void updateFile(const QString &filename, const QList<TextEditResult> &list);
    void hoverResult(const QString &file, const QList<HoverResult> &result);
    void hoverDefinitionResult(const QString &file, const QList<DefinitionResult> &definitions);
    void diagnosticsInfo(const QString &filename, const QList<DiagnosticResult> &diagnostics);
    void documentSymbolsResult(const QString &filename, const QList<LiteApi::Symbol> &symbols);
    void findUsageResult(const QString &file, const QList<UsageResult> &list);
    void semanticTokensResult(const QString &file, const QVariantList &list);
    void foldingRangeResult(const QString &file, const QList<FoldingRangeResult> &list);
    void exited();

protected slots:
    void onReadyRead();
    void onStarted();
    void currentEnvChanged(LiteApi::IEnv *);

public:
    explicit GoPlsServer(LiteApi::IApplication *app, QObject *parent = nullptr);
    virtual ~GoPlsServer();

    void quit();

    void initWorkspace(const QStringList &folders);
    void prepareRenameSymbol(const QString &path, unsigned int line, unsigned int startColumn);
    void askFindUsage(const QString &path, unsigned int line, unsigned int column);
    void askAutocomplete(const QString &path, unsigned int line, unsigned int column);
    void askSignatureHelp(const QString &path, unsigned int line, unsigned int column, const QString &trigger);
    void updateWorkspaceFolders(const QStringList &add, const QStringList &remove);
    void askDefinitions(const QString &file, bool hover, unsigned int line, unsigned int column);

    void formatDocument(const QString &file);
    void organizeImports(const QString &file);
    void documentHighlight(const QString &file, int startLine, int startColumn, int endLine, int endColumn);
    void fileOpened(const QString &file, const QString &content);
    void fileChanged(const QString &file, int startLine, int startPos, int endLine, int endPos, const QString &content);
    void fileSaved(const QString &file, const QString &content);
    void fileClosed(const QString &file);
    void hover(const QString &filename, int line, int column);
    void documentSymbols(const QString &filename);
    void enableStaticcheck(bool v);
    void semanticTokensFull(const QString &file);


    void shutdown();
    void exit();

protected:
    void decodeInitialize(const CommandData &data, const QJsonObject &);
    void decodeInitialized(const CommandData &data, const QJsonObject &);
    QList<DefinitionResult> decodeDocumentDefinition(const CommandData &data, const QJsonObject &response);
    void decodeDocumentDefinitionHover(const CommandData &data, const QJsonObject &response);
    void decodeDocumentDefinitionGoTo(const CommandData &data, const QJsonObject &response);
    void decodeDocumentCompletion(const CommandData &data, const QJsonObject &response);
    void decodeDocumentFormatting(const CommandData &data, const QJsonObject &response);
    void decodeOrganizeImport(const CommandData &data, const QJsonObject &response);
    void decodeHover(const CommandData &data, const QJsonObject &response);
    void decodeDiagnostics(const CommandData &data, const QJsonObject &response);
    void decodeDocumentSymbols(const CommandData &data, const QJsonObject &response);
    void decodeDidOpened(const CommandData &data, const QJsonObject &response);
    void decodeDidClosed(const CommandData &data, const QJsonObject &response);
    void decodeDidChanged(const CommandData &data, const QJsonObject &response);
    void decodeDidSaved(const CommandData &data, const QJsonObject &response);
    void decodeShutdown(const CommandData &data, const QJsonObject &response);
    void decodeExit(const CommandData &data, const QJsonObject &response);
    void decodeAddWorkspaceFolder(const CommandData &data, const QJsonObject &response);
    void decodeCurrentEnvChanged(const CommandData &data, const QJsonObject &response);
    void decodeSignatureHelp(const CommandData &data, const QJsonObject &response);
    void decodeFindUsage(const CommandData &data, const QJsonObject &response);
    void decodeSemanticTokens(const CommandData &data, const QJsonObject &response);
    void decodeFoldingRange(const CommandData &data, const QJsonObject &response);
    void printResponse(const CommandData &data, const QJsonObject &response);

private:
    GoPlsTypes::TextDocumentIdentifier *documentIdentifier(const QString &path) const;
    GoPlsTypes::Position *position(unsigned int line, unsigned int column) const;
    GoPlsTypes::Range *range(GoPlsTypes::Position *start, GoPlsTypes::Position *end) const;

    void decodeResponse(const QByteArray &payload);

    QHash<int, CommandData> m_idToData;
    QHash<int, DecodeFunc> m_idToCallback;
    QHash<QString, bool> m_openedFiles;
    QHash<QString, int> m_filesVersions;
    bool m_init;
    LiteApi::IEnvManager *m_envManager;
    QList<GoPlsCommand> m_waitingCommands;
};

#endif // GOPLSSERVER_H
