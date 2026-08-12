#include "goplssearchresults.h"
#include "liteapi/liteqt.h"

#include <QFile>
#include <QJsonObject>
#include <QTextStream>
#include <QUrl>

GoplsSearchResults::GoplsSearchResults(QObject *parent) : LiteApi::IFileSearch(parent),
    m_replaceMode(false), m_readOnly(true)
{
}

QString GoplsSearchResults::mimeType() const { return "find/gopls-lsp"; }
QString GoplsSearchResults::displayName() const { return m_title; }
QWidget *GoplsSearchResults::widget() const { return 0; }
void GoplsSearchResults::start() {}
void GoplsSearchResults::cancel() {}
void GoplsSearchResults::activate() {}
QString GoplsSearchResults::searchText() const { return m_searchText; }
bool GoplsSearchResults::replaceMode() const { return m_replaceMode; }
bool GoplsSearchResults::readOnly() const { return m_readOnly; }
bool GoplsSearchResults::canCancel() const { return false; }
void GoplsSearchResults::setSearchInfo(const QString &, const QString &, const QString &) {}

void GoplsSearchResults::setReplaceMode(bool replaceMode, bool readOnly)
{
    m_replaceMode = replaceMode;
    m_readOnly = readOnly;
}

void GoplsSearchResults::showLocations(const QString &title, const QString &searchText, const QJsonArray &locations,
                                       bool replaceMode, bool readOnly)
{
    m_title = title;
    m_searchText = searchText;
    setReplaceMode(replaceMode,readOnly);
    emit findStarted();
    foreach (QJsonValue value, locations) {
        QJsonObject location = value.toObject();
        QString uri = location.value("uri").toString();
        QJsonObject range = location.value("range").toObject();
        if (uri.isEmpty()) {
            uri = location.value("targetUri").toString();
            range = location.value("targetSelectionRange").toObject();
        }
        QJsonObject start = range.value("start").toObject();
        QJsonObject end = range.value("end").toObject();
        QString fileName = QUrl(uri).toLocalFile();
        int lineNumber = start.value("line").toInt();
        int column = start.value("character").toInt();
        int length = m_readOnly ? qMax(1,end.value("character").toInt()-column)
                                : qMax(1,m_searchText.length());
        QString lineText;
        QFile file(fileName);
        if (file.open(QFile::ReadOnly|QFile::Text)) {
            QTextStream stream(&file);
            qtSetUtf8Encoding(stream);
            for (int line = 0; !stream.atEnd(); ++line) {
                QString text = stream.readLine();
                if (line == lineNumber) {
                    lineText = text;
                    break;
                }
            }
        }
        emit findResult(LiteApi::FileSearchResult(fileName,lineText,lineNumber,column,length));
    }
    emit findFinished(true);
}
