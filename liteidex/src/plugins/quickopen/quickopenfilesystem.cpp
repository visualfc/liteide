#include "quickopenfilesystem.h"
#include "folderview/filesystemmodelex.h"
#include "folderview/dirsortfilterproxymodel.h"
#include <QFileSystemModel>

QuickOpenFileSystem::QuickOpenFileSystem(LiteApi::IApplication *app, QObject *parent) : LiteApi::IQuickOpenFileSystem(parent)
{
    m_liteApp = app;
    m_model = new FileSystemModelEx(this);
    bool proxyMode = false;
#ifdef Q_OS_MAC
    proxyMode = true;
#endif
    if (proxyMode) {
        m_proxy = new FileSystemProxyModel(this);
        m_proxy->setSourceModel(m_model);
        m_proxy->sort(0);
    } else {
        m_proxy = 0;
    }
}

QString QuickOpenFileSystem::id() const
{
    return "quickopen/filesystem";
}

QString QuickOpenFileSystem::placeholderText() const
{
    return m_placehoderText;
}

void QuickOpenFileSystem::activate()
{

}

QAbstractItemModel *QuickOpenFileSystem::model() const
{
    if (m_proxy) {
        return m_proxy;
    }
    return m_model;
}

QModelIndex QuickOpenFileSystem::rootIndex() const
{
    if (m_proxy) {
        return m_proxy->mapFromSource(m_rootIndex);
    }
    return m_rootIndex;
}

void QuickOpenFileSystem::updateModel()
{

}

QModelIndex QuickOpenFileSystem::filterChanged(const QString &text)
{
    int count = m_model->rowCount(m_rootIndex);
    for (int i = 0; i < count; i++) {
        QModelIndex index = m_model->index(i,0,m_rootIndex);
        if (m_model->fileName(index).startsWith(text)) {
            if (m_proxy) {
                return m_proxy->mapFromSource(index);
            }
            return index;
        }
    }
    return QModelIndex();
}

void QuickOpenFileSystem::cancel()
{

}

void QuickOpenFileSystem::setRootPath(const QString &path)
{
    m_rootIndex = m_model->setRootPath(path);
}

void QuickOpenFileSystem::setPlaceholderText(const QString &text)
{
    m_placehoderText = text;
}

bool QuickOpenFileSystem::selected(const QString &/*text*/, const QModelIndex &index)
{
    if (!index.isValid()) {
        return false;
    }
    QModelIndex fileIndex = index;
    if (m_proxy) {
        fileIndex = m_proxy->mapToSource(index);
    }
    if (m_model->isDir(fileIndex)) {
        return false;
    }
    QString filePath = m_model->filePath(fileIndex);
    if (!m_liteApp->fileManager()->openFile(filePath)) {
        return false;
    }
    return true;
}

void QuickOpenFileSystem::indexChanged(const QModelIndex &index)
{

}

QString QuickOpenFileSystem::info() const
{
    return tr("File System");
}
