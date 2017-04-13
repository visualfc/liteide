#include "multifolderview.h"
#include <QMessageBox>

MultiFolderView::MultiFolderView(LiteApi::IApplication *app, QWidget *parent)
    : BaseFolderView(app,parent)
{
    m_model = new MultiFolderModel(this);

#ifdef Q_OS_MAC
    m_model->sort(0);
    m_model->setSorting(QDir::DirsFirst|QDir::Name);
#endif

    this->setModel(m_model);
    this->setHeaderHidden(false);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenuRequested(QPoint)));
}

void MultiFolderView::setFilter(QDir::Filters filters)
{
    m_model->setFilter(filters);
}

QDir::Filters MultiFolderView::filter() const
{
    return m_model->filter();
}

QFileInfo MultiFolderView::fileInfo(const QModelIndex &index) const
{
    return m_model->fileInfo(index);
}

bool MultiFolderView::addRootPath(const QString &path)
{
    if (m_model->isRootPath(path)) {
        return true;
    }
    QModelIndex index = m_model->addRootPath(path);
    return index.isValid();
}

void MultiFolderView::setRootPathList(const QStringList &pathList)
{
    m_model->clearAll();
    foreach (QString path, pathList) {
        m_model->addRootPath(path);
    }
}

QStringList MultiFolderView::rootPathList() const
{
    return m_model->rootPathList();
}

void MultiFolderView::clear()
{
    m_model->clearAll();
}

void MultiFolderView::expandFolder(const QString &path, bool expand)
{
    QList<QModelIndex> indexList = m_model->indexForPath(path);
    foreach (QModelIndex sourceIndex, indexList) {
        QModelIndex index = sourceIndex;
        if (expand) {
            this->expand(index);
        } else {
            this->collapse(index);
        }
    }
}

QList<QModelIndex> MultiFolderView::indexForPath(const QString &path) const
{
    return m_model->indexForPath(path);
}

void MultiFolderView::customContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this);
    LiteApi::FILESYSTEM_CONTEXT_FLAG flag = LiteApi::FILESYSTEM_ROOT;
    QModelIndex index = this->indexAt(pos);
    if (index.isValid()) {
        m_contextIndex = index;
        QModelIndex sourceIndex = index;
        m_contextInfo = m_model->fileInfo(sourceIndex);
        if (m_model->isRootIndex(sourceIndex)) {
            flag = LiteApi::FILESYSTEM_ROOTFOLDER;
        } else if (m_model->isDir(sourceIndex)) {
            flag = LiteApi::FILESYSTEM_FOLDER;
        } else {
            flag = LiteApi::FILESYSTEM_FILES;
        }
    } else {
        m_contextIndex = this->rootIndex();
        m_contextInfo = QFileInfo();
    }
    bool hasGo = false;
    if (flag != LiteApi::FILESYSTEM_ROOT) {
        foreach(QFileInfo info, contextDir().entryInfoList(QDir::Files)) {
            if (info.suffix() == "go") {
                hasGo = true;
            }
        }
    }
    //root folder
    if (flag == LiteApi::FILESYSTEM_ROOT) {
        menu.addAction(m_openFolderAct);
    } else if (flag == LiteApi::FILESYSTEM_ROOTFOLDER) {
        menu.addAction(m_newFileAct);
        menu.addAction(m_newFileWizardAct);
        menu.addAction(m_newFolderAct);
        menu.addSeparator();
        menu.addAction(m_reloadFolderAct);
        menu.addAction(m_closeFolderAct);
        menu.addSeparator();
        if (hasGo) {
            menu.addAction(m_viewGodocAct);
            menu.addSeparator();
        }
        menu.addAction(m_openShellAct);
        menu.addAction(m_openExplorerAct);
    } else if (flag == LiteApi::FILESYSTEM_FOLDER) {
        menu.addAction(m_newFileAct);
        menu.addAction(m_newFileWizardAct);
        menu.addAction(m_newFolderAct);
        menu.addAction(m_renameFolderAct);
        menu.addAction(m_removeFolderAct);
        menu.addSeparator();
        if (hasGo) {
            menu.addAction(m_viewGodocAct);
            menu.addSeparator();
        }
        menu.addAction(m_openShellAct);
        menu.addAction(m_openExplorerAct);
    } else if (flag == LiteApi::FILESYSTEM_FILES) {
        menu.addAction(m_openEditorAct);
        menu.addSeparator();
        menu.addAction(m_newFileAct);
        menu.addAction(m_newFileWizardAct);
        menu.addAction(m_renameFileAct);
        menu.addAction(m_removeFileAct);
        menu.addSeparator();
        if (hasGo) {
            menu.addAction(m_viewGodocAct);
            menu.addSeparator();
        }
        menu.addAction(m_openShellAct);
        menu.addAction(m_openExplorerAct);

    }
    emit aboutToShowContextMenu(&menu,flag,m_contextInfo);
    menu.exec(this->mapToGlobal(pos));
}

void MultiFolderView::removeFolder()
{
    QFileInfo info = m_contextInfo;
    if (!info.isDir()) {
        return;
    }

    int ret = QMessageBox::warning(m_liteApp->mainWindow(),tr("Delete Folder"),
                          tr("Are you sure that you want to permanently delete this folder and all of its contents?")
                          +"\n"+info.filePath(),
                          QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        QModelIndex index = this->currentIndex();
        if (!m_model->remove(index)) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Delete Folder"),
                                     tr("Failed to delete the folder!"));
        }
    }
}

void MultiFolderView::removeFile()
{
    QFileInfo info = m_contextInfo;
    if (!info.isFile()) {
        return;
    }

    int ret = QMessageBox::question(m_liteApp->mainWindow(),tr("Delete File"),
                          tr("Are you sure that you want to permanently delete this file?")
                          +"\n"+info.filePath(),
                          QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        QModelIndex index = this->currentIndex();
        if (!m_model->remove(index)) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Delete File"),
                                     tr("Failed to delete the file!"));
        }
    }
}

void MultiFolderView::openFolder()
{
    m_liteApp->fileManager()->openFolder();
}

void MultiFolderView::closeFolder()
{
    QModelIndex index = this->currentIndex();
    m_model->removeRoot(index);
}

void MultiFolderView::reloadFolder()
{
    m_model->reloadAll();
}

void MultiFolderView::closeAllFolders()
{
    m_model->clearAll();
}

