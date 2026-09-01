#include "goplsclient.h"

#include <QJsonDocument>

GoplsClient::GoplsClient(QObject *parent) :
    QObject(parent), m_process(new QProcess(this)), m_nextId(1),
    m_initializeId(0), m_stopping(false)
{
    connect(m_process,SIGNAL(readyReadStandardOutput()),this,SLOT(readOutput()));
    connect(m_process,SIGNAL(readyReadStandardError()),this,SLOT(readError()));
    connect(m_process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(processFinished(int,QProcess::ExitStatus)));
}

GoplsClient::~GoplsClient()
{
    stop();
}

bool GoplsClient::start(const QString &program, const QString &workingDirectory,
                        const QProcessEnvironment &environment, const QJsonObject &initializeParams)
{
    if (isRunning()) {
        return true;
    }
    m_buffer.clear();
    m_requests.clear();
    m_stopping = false;
    m_process->setWorkingDirectory(workingDirectory);
    m_process->setProcessEnvironment(environment);
    m_process->start(program,QStringList() << "serve");
    if (!m_process->waitForStarted(3000)) {
        emit logMessage(QString("Failed to start gopls: %1").arg(m_process->errorString()),true);
        return false;
    }
    m_initializeId = request("initialize",initializeParams);
    return true;
}

void GoplsClient::stop()
{
    if (!isRunning()) {
        return;
    }
    m_stopping = true;
    request("shutdown",QJsonValue());
    notify("exit");
    m_process->closeWriteChannel();
    if (!m_process->waitForFinished(500)) {
        m_process->terminate();
        if (!m_process->waitForFinished(500)) {
            m_process->kill();
            m_process->waitForFinished(500);
        }
    }
}

bool GoplsClient::isRunning() const
{
    return m_process->state() != QProcess::NotRunning;
}

int GoplsClient::request(const QString &method, const QJsonValue &params)
{
    int id = m_nextId++;
    QJsonObject message;
    message.insert("jsonrpc","2.0");
    message.insert("id",id);
    message.insert("method",method);
    if (!params.isUndefined()) {
        message.insert("params",params);
    }
    m_requests.insert(id,method);
    writeMessage(message);
    return id;
}

void GoplsClient::notify(const QString &method, const QJsonValue &params)
{
    QJsonObject message;
    message.insert("jsonrpc","2.0");
    message.insert("method",method);
    if (!params.isUndefined()) {
        message.insert("params",params);
    }
    writeMessage(message);
}

void GoplsClient::reply(int id, const QJsonValue &result)
{
    QJsonObject message;
    message.insert("jsonrpc","2.0");
    message.insert("id",id);
    message.insert("result",result);
    writeMessage(message);
}

void GoplsClient::writeMessage(const QJsonObject &message)
{
    if (!isRunning()) {
        return;
    }
    QByteArray body = QJsonDocument(message).toJson(QJsonDocument::Compact);
    QByteArray header = "Content-Length: " + QByteArray::number(body.size()) + "\r\n\r\n";
    m_process->write(header);
    m_process->write(body);
}

void GoplsClient::readOutput()
{
    m_buffer += m_process->readAllStandardOutput();
    for (;;) {
        int headerEnd = m_buffer.indexOf("\r\n\r\n");
        if (headerEnd < 0) {
            return;
        }
        QByteArray header = m_buffer.left(headerEnd);
        int contentLength = -1;
        foreach (QByteArray line, header.split('\n')) {
            line = line.trimmed();
            if (line.toLower().startsWith("content-length:")) {
                contentLength = line.mid(15).trimmed().toInt();
                break;
            }
        }
        if (contentLength < 0) {
            emit logMessage("Invalid LSP response header",true);
            m_buffer.remove(0,headerEnd+4);
            continue;
        }
        if (m_buffer.size() < headerEnd+4+contentLength) {
            return;
        }
        QByteArray body = m_buffer.mid(headerEnd+4,contentLength);
        m_buffer.remove(0,headerEnd+4+contentLength);
        QJsonParseError parseError;
        QJsonDocument document = QJsonDocument::fromJson(body,&parseError);
        if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
            emit logMessage(QString("Invalid JSON from gopls: %1").arg(parseError.errorString()),true);
            continue;
        }
        dispatch(document.object());
    }
}

void GoplsClient::dispatch(const QJsonObject &message)
{
    if (message.contains("method")) {
        if (message.contains("id")) {
            emit serverRequest(message.value("id").toInt(),message.value("method").toString(),message.value("params"));
        } else {
            emit notification(message.value("method").toString(),message.value("params"));
        }
        return;
    }
    int id = message.value("id").toInt();
    QString method = m_requests.take(id);
    if (id == m_initializeId && !message.contains("error")) {
        notify("initialized",QJsonObject());
        emit initialized();
    }
    emit response(id,method,message.value("result"),message.value("error").toObject());
}

void GoplsClient::readError()
{
    QString message = QString::fromUtf8(m_process->readAllStandardError()).trimmed();
    if (!message.isEmpty()) {
        emit logMessage(message,false);
    }
}

void GoplsClient::processFinished(int, QProcess::ExitStatus)
{
    m_requests.clear();
    if (!m_stopping) {
        emit logMessage("gopls stopped unexpectedly",true);
    }
    emit stopped();
}
