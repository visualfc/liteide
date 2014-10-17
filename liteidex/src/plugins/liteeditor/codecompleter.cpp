#include "codecompleter.h"
#include <QListView>
#include <QStandardItemModel>
#include <QKeyEvent>

CodeCompleter::CodeCompleter(QObject *parent) :
    QCompleter(parent)
{
}

CodeCompleter::~CodeCompleter()
{
}

void CodeCompleter::setSeparator(const QString &separator)
{
    m_seperator = separator;
}

QString CodeCompleter::separator() const
{
    return m_seperator;
}

QStringList CodeCompleter::splitPath(const QString &path) const
{
    if (m_seperator.isNull()) {
        return QCompleter::splitPath(path);
    }
    return path.split(m_seperator);
}

QString CodeCompleter::pathFromIndex(const QModelIndex &index) const
{
    if (m_seperator.isNull()) {
        return QCompleter::pathFromIndex(index);
    }

    // navigate up and accumulate data
    QStringList dataList;
    for (QModelIndex i = index; i.isValid(); i = i.parent()) {
        dataList.prepend(model()->data(i, completionRole()).toString());
    }
    return dataList.join(m_seperator);
}

