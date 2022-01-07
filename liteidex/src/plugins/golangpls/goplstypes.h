#ifndef GOPLSTYPES_H
#define GOPLSTYPES_H

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QVariant>
#include <QPoint>

class GoPlsParams
{
public:
    virtual QJsonObject toJson() const = 0;
    virtual void fromJson(const QJsonObject &json) = 0;
};

class EmptyResults: virtual public GoPlsParams {
public:
    virtual QJsonObject toJson() const { return QJsonObject(); }
    virtual void fromJson(const QJsonObject &) {}
};

typedef std::function<void(const QJsonObject &result)> DecodeFunc;

class GoPlsCommand
{
    QString m_method;
    QSharedPointer<GoPlsParams> m_params;
    static int s_commandID;
    int m_commandID;
    DecodeFunc m_decodeFunc;

public:
    GoPlsCommand(const QString &method, const  QSharedPointer<GoPlsParams> &params, const DecodeFunc &responseFunc);
    virtual ~GoPlsCommand(){}
    virtual QByteArray toJson() const;
    int commandID() const;
    QString method() const;
    DecodeFunc decodeFunc() const;
};

struct AutoCompletionResult {
    QString label;
    QString detail;
    QString type;
};

struct DefinitionResult {
    QString path;
    unsigned int line;
    unsigned int column;
};

struct TextEditResult {
    unsigned int startLine;
    unsigned int startColumn;
    unsigned int endLine;
    unsigned int endColumn;
    QString text;
};

struct HoverResult {
    unsigned int startLine;
    unsigned int startColumn;
    unsigned int endLine;
    unsigned int endColumn;
    QString info;
};

struct DiagnosticResult {
    unsigned int line;
    QString level;
    QString message;
    QString code;
};

struct UsageResult {
    QString filepath;
    unsigned int startLine;
    unsigned int startColumn;
    unsigned int endLine;
    unsigned int endColumn;
};

#endif // GOPLSTYPES_H
