#ifndef CODECOMPLETER_H
#define CODECOMPLETER_H

#include <QCompleter>
#include "liteapi/liteapi.h"

class QListView;
class QKeyEvent;
class QStandardItemModel;
class QStandardItem;
class QSortFilterProxyModel;
class CodeCompleter : public QCompleter
{
    Q_OBJECT
public:
    explicit CodeCompleter(QObject *parent = 0);
    virtual ~CodeCompleter();
    void setModel(QAbstractItemModel *c);
    QString separator() const;
    void setSeparator(const QString &separator);
    void setCompletionPrefix(const QString &prefix);
    QString completionPrefix() const;
    void updateFilter();
protected:
    virtual bool eventFilter(QObject *o, QEvent *e);
    virtual QStringList splitPath(const QString &path) const;
    virtual QString pathFromIndex(const QModelIndex &index) const;
private:
    QSortFilterProxyModel *m_proxy;
    QListView *m_popup;
    QString m_seperator;
    QString m_prefix;
};

class CodeCompleterListView;

namespace LiteApi {
    enum CaseSensitivity {
        CaseInsensitive,
        CaseSensitive,
        FirstLetterCaseSensitive
    };
}


class CodeCompleterProxyModel : public QAbstractListModel
{
public:
    CodeCompleterProxyModel(QObject *parent = 0);
    virtual int rowCount(const QModelIndex & index = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    void setSourceModel(QStandardItemModel *sourceModel);
    QStandardItemModel *sourceModel() const;
    QStandardItem* item(const QModelIndex &index) const;
    int filter(const QString &filter, int cs = LiteApi::CaseInsensitive);
    void setSeparator(const QString &separator);
    QString separator() const;
protected:
    bool splitFilter(const QString &filter, QModelIndex &parent, QString &prefix);
    QList<QStandardItem*> m_items;
    QStandardItemModel *m_model;
    QString             m_seperator;
};

class CodeCompleterEx : public QObject
{
    Q_OBJECT
public:
    CodeCompleterEx(QObject *parent = 0);
    void setModel(QStandardItemModel *c);
    QAbstractItemModel *model() const;
    void setSeparator(const QString &separator);
    QString separator() const;
    void setCompletionPrefix(const QString &prefix);
    QString completionPrefix() const;
    void updateFilter();
    void complete(const QRect& rect = QRect());
    QWidget *widget() const;
    void setWidget(QWidget *widget);
    QModelIndex currentIndex() const;
    QString currentCompletion() const;
    void setCaseSensitivity(Qt::CaseSensitivity cs);
    Qt::CaseSensitivity caseSensitivity() const;
    QAbstractItemView *popup() const;
    QAbstractItemModel *completionModel() const;
    void setWrapAround(bool wrap);
    bool wrapAround() const;
signals:
    void activated(QModelIndex);
public slots:
    void completerActivated(QModelIndex);
protected:
    virtual bool eventFilter(QObject *o, QEvent *e);
    QWidget              *m_widget;
    CodeCompleterListView     *m_popup;
    CodeCompleterProxyModel   *m_proxy;
    QString               m_prefix;
    Qt::CaseSensitivity  m_cs;
    int                 maxVisibleItems;
    bool                m_eatFocusOut;
    bool                m_hiddenBecauseNoMatch;
    bool                m_wrap;
};

#endif // CODECOMPLETER_H
