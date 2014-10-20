#include "codecompleter.h"
#include <QListView>
#include <QStandardItemModel>
#include <QKeyEvent>
#include <QDebug>

CodeCompleter::CodeCompleter(QObject *parent) :
    QCompleter(parent)
{
    m_popup = new QListView;
    m_popup->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_popup->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_popup->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_popup->setSelectionMode(QAbstractItemView::SingleSelection);
    m_popup->setModelColumn(0);
    this->setPopup(m_popup);
}

CodeCompleter::~CodeCompleter()
{
}

void CodeCompleter::setSeparator(const QString &separator)
{
    m_seperator = separator;
}

bool CodeCompleter::eventFilter(QObject *o, QEvent *e)
{
    switch (e->type()) {
    case QEvent::KeyPress: {
        QKeyEvent *ke = static_cast<QKeyEvent *>(e);
        switch (ke->key()) {
        case Qt::Key_Up:
            if ( this->popup() && this->popup()->isVisible()) {
                QModelIndex index = this->popup()->currentIndex();
                if (index.isValid() && (index.row() == 0)) {
                    this->popup()->setCurrentIndex(this->popup()->model()->index(this->popup()->model()->rowCount()-1,0));
                    return true;
                }
            }
            break;
        case Qt::Key_Down:
            if (this->popup() && this->popup()->isVisible()) {
                QModelIndex index = this->popup()->currentIndex();
                if (index.isValid() && (index.row() == this->popup()->model()->rowCount()-1)) {
                    this->popup()->setCurrentIndex(this->popup()->model()->index(0,0));
                    return true;
                }
            }
            break;
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
    return QCompleter::eventFilter(o,e);
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

