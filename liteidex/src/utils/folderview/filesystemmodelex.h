#ifndef FILESYSTEMMODELEX_H
#define FILESYSTEMMODELEX_H

#include <QFileSystemModel>

class FolderListModel;
class FileSystemModelEx : public QFileSystemModel
{
    Q_OBJECT
public:
    explicit FileSystemModelEx(QObject *parent = 0);
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &index, int role) const;
    friend class FolderListModel;
    static QString fileSize(qint64 bytes);
};

#endif // FILESYSTEMMODELEX_H
