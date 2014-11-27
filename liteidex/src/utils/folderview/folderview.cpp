#include "folderview.h"
#include <QFileSystemModel>
#include <QMessageBox>
class MyFileSystemModel : public QFileSystemModel
{
public:
    MyFileSystemModel(QObject *parent) :
        QFileSystemModel(parent)
    {
    }
    virtual int columnCount(const QModelIndex &parent) const
    {
        return 1;
    }
};

FolderView::FolderView(LiteApi::IApplication *app, QWidget *parent) :
    BaseFolderView(app,parent)
{
    m_model = new MyFileSystemModel(this);
    this->setModel(m_model);
    this->setHeaderHidden(true);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenuRequested(QPoint)));
}

void FolderView::setRootPath(const QString &path)
{
    QModelIndex index = m_model->setRootPath(path);
    this->setRootIndex(index);
}

QString FolderView::rootPath() const
{
    return m_model->rootPath();
}

QFileSystemModel *FolderView::model() const
{
    return m_model;
}

void FolderView::removeFile()
{
    QFileInfo info = m_contextInfo;
    if (!info.isFile()) {
        return;
    }

    int ret = QMessageBox::question(m_liteApp->mainWindow(),tr("Delete File"),
                          tr("Are you sure that you want to permanently delete this file?"),
                          QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        if (!m_model->remove(this->currentIndex())) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Delete File"),
                                     tr("Failed to delete the file!"));
        }
    }
}

void FolderView::removeFolder()
{
    QFileInfo info = m_contextInfo;
    if (!info.isDir()) {
        return;
    }

    int ret = QMessageBox::warning(m_liteApp->mainWindow(),tr("Delete Folder"),
                          tr("Are you sure that you want to permanently delete this folder and all of its contents?"),
                          QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        if (!m_model->remove(this->currentIndex())) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Delete Folder"),
                                     tr("Failed to delete the folder!"));
        }
    }
}

void FolderView::customContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this);
    LiteApi::FILESYSTEM_CONTEXT_FLAG flag = LiteApi::FILESYSTEM_ROOT;
    QModelIndex index = this->indexAt(pos);
    if (index.isValid()) {
        m_contextInfo = m_model->fileInfo(index);
        m_contextIndex = index;
        if (m_model->isDir(index)) {
            flag = LiteApi::FILESYSTEM_FOLDER;
        } else {
            flag = LiteApi::FILESYSTEM_FILES;
        }
    } else {
        m_contextIndex = this->rootIndex();
        m_contextInfo = m_model->fileInfo(m_contextIndex);
        flag = LiteApi::FILESYSTEM_ROOTFOLDER;
    }
    bool hasGo = false;
    foreach(QFileInfo info, contextDir().entryInfoList(QDir::Files)) {
        if (info.suffix() == "go") {
            hasGo = true;
        }
    }
    //root folder
    if (flag == LiteApi::FILESYSTEM_ROOT) {
        menu.addAction(m_newFileAct);
        menu.addAction(m_newFileWizardAct);
        menu.addAction(m_newFolderAct);
        menu.addSeparator();
        if (hasGo) {
            menu.addAction(m_viewGodocAct);
            menu.addSeparator();
        }
        menu.addAction(m_openShellAct);
        menu.addAction(m_openExplorerAct);
    } else if (flag == LiteApi::FILESYSTEM_ROOTFOLDER) {
        menu.addAction(m_newFileAct);
        menu.addAction(m_newFileWizardAct);
        menu.addAction(m_newFolderAct);
        menu.addAction(m_renameFolderAct);
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
