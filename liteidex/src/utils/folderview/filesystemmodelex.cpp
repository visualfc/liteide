#include "filesystemmodelex.h"
#include <QDateTime>

QString FileSystemModelEx::fileSize(qint64 bytes)
{
    // According to the Si standard KB is 1000 bytes, KiB is 1024
    // but on windows sizes are calculated by dividing by 1024 so we do what they do.
    const qint64 kb = 1024;
    const qint64 mb = 1024 * kb;
    const qint64 gb = 1024 * mb;
    const qint64 tb = 1024 * gb;
    if (bytes >= tb)
        return QFileSystemModel::tr("%1 TB").arg(QLocale().toString(qreal(bytes) / tb, 'f', 3));
    if (bytes >= gb)
        return QFileSystemModel::tr("%1 GB").arg(QLocale().toString(qreal(bytes) / gb, 'f', 2));
    if (bytes >= mb)
        return QFileSystemModel::tr("%1 MB").arg(QLocale().toString(qreal(bytes) / mb, 'f', 1));
    if (bytes >= kb)
        return QFileSystemModel::tr("%1 KB").arg(QLocale().toString(bytes / kb));
    return QFileSystemModel::tr("%1 bytes").arg(QLocale().toString(bytes));
}

FileSystemModelEx::FileSystemModelEx(QObject *parent) :
    QFileSystemModel(parent)
{
}

int FileSystemModelEx::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
}
QVariant FileSystemModelEx::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ToolTipRole) {
        if (this->isDir(index)) {
            return QDir::toNativeSeparators(this->filePath(index));
        } else {
            QFileInfo info = this->fileInfo(index);
            return QString("%1\n%2\n%3")
                    .arg(QDir::toNativeSeparators(info.filePath()))
                    .arg(fileSize(info.size()))
                    .arg(info.lastModified().toString(Qt::SystemLocaleDate));
        }
    }
    return QFileSystemModel::data(index,role);
}
