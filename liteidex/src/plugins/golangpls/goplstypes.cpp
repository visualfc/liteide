#include <goplstypes.h>

int GoPlsCommand::s_commandID = 1;
int GoPlsCommand::commandID() const
{
    return m_commandID;
}

QString GoPlsCommand::method() const
{
    return m_method;
}

DecodeFunc GoPlsCommand::decodeFunc() const
{
    return m_decodeFunc;
}

QString GoPlsCommand::filepath() const
{
    return m_filepath;
}

GoPlsCommand::GoPlsCommand(const QString &method, const QSharedPointer<GoPlsParams> &params, const DecodeFunc &responseFunc, const QString &filepath)
    : m_method(method)
    , m_params(params)
    , m_decodeFunc(responseFunc)
    , m_filepath(filepath)
{
    m_commandID = s_commandID++;
}

QByteArray GoPlsCommand::toJson() const
{
    QVariantHash hash;
    hash["id"] = m_commandID;
    hash["jsonrpc"] = "2.0";
    hash["method"] = m_method;
    if(m_params) {
        hash["params"] = m_params->toJson();
    }
    QJsonDocument doc = QJsonDocument::fromVariant(hash);
    return doc.toJson();
}

