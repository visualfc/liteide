#ifndef GOPLSSERVER_H
#define GOPLSSERVER_H

#include <QEventLoop>
#include <QObject>
#include <QProcess>
#include <goplstypes.h>
#include "liteapi/liteapi.h"
#include "liteeditorapi/liteeditorapi.h"

namespace GoPlsTypes {
class TextDocumentIdentifier;
}

namespace LiteApi {
class IEnv;
class IEnvManager;
}


class GoPlsServer: public QObject
{
    Q_OBJECT
    QProcess *m_process;

protected:
    void sendCommand(const QString &command, const QSharedPointer<GoPlsParams> &params, const DecodeFunc &responseFunc, bool force = false);
    void executeCommand(const GoPlsCommand &cmd);

signals:
    void responseReceived(int commandID, const QByteArray &response);
    void autocompleteResult(const QList<AutoCompletionResult> &);
    void definitionsResult(const QList<DefinitionResult> &definitions);
    void logMessage(const QString &message, bool isError);
    void formattingResults(const QList<TextEditResult> &list);
    void updateFile(const QString &filename, const QList<TextEditResult> &list);
    void hoverResult(const QList<HoverResult> &result);
    void hoverDefinitionResult(const QList<DefinitionResult> &definitions);
    void diagnosticsInfo(const QString &filename, const QList<DiagnosticResult> &diagnostics);
    void documentSymbolsResult(const QString &filename, const QList<LiteApi::Symbol> &symbols);
    void exited();

protected slots:
    void onReadyRead();
    void onStarted();
    void currentEnvChanged(LiteApi::IEnv*);

public:
    GoPlsServer(LiteApi::IApplication *app, QObject *parent = nullptr);
    virtual ~GoPlsServer();

    void quit();

    void initWorkspace(const QString &folder);
    void askAutocomplete(const QString &path, unsigned int line, unsigned int column);
    void addWorkspaceFolder(const QString &folder);
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

    void shutdown();
    void exit();

protected:
    void decodeInitialize(const QJsonObject &);
    void decodeInitialized(const QJsonObject &);
    QList<DefinitionResult> decodeDocumentDefinition(const QJsonObject &response);
    void decodeDocumentDefinitionHover(const QJsonObject &response);
    void decodeDocumentDefinitionGoTo(const QJsonObject &response);
    void decodeDocumentCompletion(const QJsonObject &response);
    void decodeDocumentFormatting(const QJsonObject &response);
    void decodeOrganizeImport(const QJsonObject &response);
    void decodeHover(const QJsonObject &response);
    void decodeDiagnostics(const QJsonObject &response);
    void decodeDocumentSymbols(const QJsonObject &response);
    void decodeDidOpened(const QJsonObject &response);
    void decodeDidClosed(const QJsonObject &response);
    void decodeDidChanged(const QJsonObject &response);
    void decodeDidSaved(const QJsonObject &response);
    void decodeShutdown(const QJsonObject &response);
    void decodeExit(const QJsonObject &response);
    void decodeAddWorkspaceFolder(const QJsonObject &response);
    void decodeCurrentEnvChanged(const QJsonObject &response);

private:
    GoPlsTypes::TextDocumentIdentifier *documentIdentifier(const QString &path) const;

    void decodeResponse(const QByteArray &payload);

    QHash<int, QString> m_idToCommands;
    QHash<int, DecodeFunc> m_idToDecodeFunc;
    QHash<QString, bool> m_openedFiles;
    QHash<QString, int> m_filesVersions;
    bool m_init;
    LiteApi::IEnvManager *m_envManager;
    QList<GoPlsCommand> m_waitingCommands;
};

#endif // GOPLSSERVER_H
