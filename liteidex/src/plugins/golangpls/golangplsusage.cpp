#include "golangplsusage.h"

GolangPlsUsage::GolangPlsUsage(QObject *parent)
    : LiteApi::IFileSearch(parent)
{

}

GolangPlsUsage::~GolangPlsUsage()
{

}

QString GolangPlsUsage::mimeType() const
{
    return "find/golangplsusage";
}

QString GolangPlsUsage::displayName() const
{
    return tr("Golang Find Usages");
}

QWidget *GolangPlsUsage::widget() const
{
    return nullptr;
}

void GolangPlsUsage::start()
{
    emit findStarted();
}

void GolangPlsUsage::cancel()
{
}

void GolangPlsUsage::activate()
{
}

QString GolangPlsUsage::searchText() const
{
    return m_searchText;
}

bool GolangPlsUsage::replaceMode() const
{
    return false;
}

bool GolangPlsUsage::canCancel() const
{
    return false;
}

void GolangPlsUsage::setSearchInfo(const QString &text, const QString &filter, const QString &path)
{
}

void GolangPlsUsage::loadResults(const QList<UsageResult> &list)
{
    emit findFinished(false);
    int previousLine = 0;
    for(auto it : list) {
        QFile file(it.filepath);
        file.open(QIODevice::ReadOnly);
        QString line;
        for(int i = previousLine; i <= it.startLine; i++) {
            char buf[1024];
            qint64 lineLength = file.readLine(buf, sizeof(buf));
            if (lineLength != -1) {
                line = QString::fromLatin1(buf);
            }
        }
        emit findResult(LiteApi::FileSearchResult(it.filepath,line,it.startLine+1,it.startColumn,it.endColumn-it.startColumn));
        file.close();
    }
}
