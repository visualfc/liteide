#ifndef BOOKMARKMODEL_H
#define BOOKMARKMODEL_H

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>
#include <QList>
#include "liteeditorapi/liteeditorapi.h"

class BookmarkNode
{
public:
    BookmarkNode(LiteApi::IEditorMark *_mark = 0, LiteApi::IEditorMarkNode *_node = 0) : mark(_mark), node(_node)
    {
    }
public:
    QString fileName() const
    {
        return mark->fileName();
    }
    QString filePath() const
    {
        return mark->filePath();
    }
    int lineNumber() const
    {
        return node->lineNumber();
    }
    QString lineText() const
    {
        return node->block().text().simplified();
    }
    QString noteText() const
    {
        return QString();
    }
protected:
    LiteApi::IEditorMark     *mark;
    LiteApi::IEditorMarkNode *node;
};

class BookmarkModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum Roles {
        FileName = Qt::UserRole,
        LineNumber = Qt::UserRole + 1,
        FilePath = Qt::UserRole + 2,
        LineText = Qt::UserRole + 3,
        Note = Qt::UserRole + 4
    };
    void addNode(LiteApi::IEditorMark *mark, LiteApi::IEditorMarkNode *node);
    void removeNode(LiteApi::IEditorMark *mark, LiteApi::IEditorMarkNode *node);
    void updateNode(LiteApi::IEditorMark *mark, LiteApi::IEditorMarkNode *node);
    BookmarkNode *bookmarkNodeForIndex(const QModelIndex &index) const;
    BookmarkNode *findBookmarkNode(LiteApi::IEditorMark *mark, LiteApi::IEditorMarkNode *node) const;
public:
    explicit BookmarkModel(QObject *parent = 0);

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
private:
    QList<BookmarkNode*> m_nodeList;
    QMap<LiteApi::IEditorMarkNode*, BookmarkNode*> m_nodeMap;
};

class BookmarkSortProxyModel : public QSortFilterProxyModel
{
public:
    explicit BookmarkSortProxyModel(QObject *parent = 0) : QSortFilterProxyModel(parent)
    {
    }
    virtual bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
    {
        QString leftPath = source_left.data(BookmarkModel::FilePath).toString();
        QString rightPath = source_right.data(BookmarkModel::FilePath).toString();
        if (leftPath == rightPath) {
            int leftNumber = source_left.data(BookmarkModel::LineNumber).toInt();
            int rightNumber = source_right.data(BookmarkModel::LineNumber).toInt();
            return leftNumber < rightNumber;
        }
        return leftPath < rightPath;
    }
};

class BookmarkDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    BookmarkDelegate(QObject *parent = 0);

private:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void generateGradientPixmap(int width, int height, const QColor &color, bool selected) const;

    mutable QPixmap m_normalPixmap;
    mutable QPixmap m_selectedPixmap;
};


#endif // BOOKMARKMODEL_H
