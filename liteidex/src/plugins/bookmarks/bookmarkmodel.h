#ifndef BOOKMARKMODEL_H
#define BOOKMARKMODEL_H

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>
#include <QList>
#include <QFileInfo>
#include <QDir>
#include "liteeditorapi/liteeditorapi.h"

class BookmarkNode
{
public:
    BookmarkNode() : m_lineNumber(-1)
    {
    }
public:
    void setFilePath(const QString &filePath)
    {
        m_filePath = QDir::toNativeSeparators(filePath);
        m_fileName = QFileInfo(m_filePath).fileName();
    }
    void setLineNumber(int lineNumber)
    {
        m_lineNumber = lineNumber;
    }
    void setLineText(const QString &text)
    {
        m_lineText = text;
        m_lineText.replace("\t","    ");
    }
    void setNodeText(const QString &node)
    {
        m_nodeText = node;
    }
public:
    QString fileName() const
    {
        return m_fileName;
    }
    QString filePath() const
    {
        return m_filePath;
    }
    int lineNumber() const
    {
        return m_lineNumber;
    }
    QString lineText() const
    {
        return m_lineText;
    }
    QString noteText() const
    {
        return m_nodeText;
    }
protected:
    int     m_lineNumber;
    QString m_filePath;
    QString m_fileName;
    QString m_lineText;
    QString m_nodeText;
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
    BookmarkNode *createBookmarkNode(LiteApi::IEditorMark *mark, LiteApi::IEditorMarkNode *node) const;
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
        return QSortFilterProxyModel::lessThan(source_left,source_right);
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
