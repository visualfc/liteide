#include "foldersview.h"
#include "liteenvapi/liteenvapi.h"
#include "litebuildapi/litebuildapi.h"
#include "golangdocapi/golangdocapi.h"
#include "fileutil/fileutil.h"
#include "../../plugins/filebrowser/createfiledialog.h"
#include "../../plugins/filebrowser/createdirdialog.h"

#include <QTreeView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QDesktopServices>
#include <QProcess>
#include <QInputDialog>
#include <QLineEdit>
#include <QUrl>
#include <QToolBar>
#include <QFileSystemWatcher>
#include <QFileDialog>
#include <QHeaderView>
#include <QDebug>
#ifdef Q_OS_WIN
#include <windows.h>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end
#endif

FoldersView::FoldersView(LiteApi::IApplication *app, QWidget *parent)
    : QWidget(parent), m_liteApp(app)
{
    m_bMultiDirMode = false;
    m_tree = new SymbolTreeView;
    m_tree->setHeaderHidden(true);

#if QT_VERSION >= 0x050000
    m_tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    m_tree->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif
    m_tree->header()->setStretchLastSection(false);

    m_model = new FoldersModel(this);

    QDir::Filters filters = QDir::AllDirs | QDir::Files | QDir::Drives
                            | QDir::Readable| QDir::Writable
                            | QDir::Executable/* | QDir::Hidden*/
                            | QDir::NoDotAndDotDot;
#ifdef Q_OS_WIN // Symlinked directories can cause file watcher warnings on Win32.
    filters |= QDir::NoSymLinks;
#endif
    m_model->setFilter(filters);

    m_tree->setContextMenuPolicy(Qt::CustomContextMenu);
    m_tree->setModel(m_model);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->addWidget(m_tree);
    this->setLayout(layout);

    m_openEditorAct = new QAction(tr("Open File"),this);
    m_newFileAct = new QAction(tr("New File..."),this);
    m_newFileWizardAct = new QAction(tr("New File Wizard..."),this);
    m_renameFileAct = new QAction(tr("Rename File..."),this);
    m_removeFileAct = new QAction(tr("Delete File"),this);

    m_newFolderAct = new QAction(tr("New Folder..."),this);
    m_renameFolderAct = new QAction(tr("Rename Folder..."),this);
    m_removeFolderAct = new QAction(tr("Delete Folder"),this);

    m_openShellAct = new QAction(tr("Open Terminal Here"),this);
    m_openExplorerAct = new QAction(tr("Open Explorer Here"),this);

    m_viewGodocAct = new QAction(tr("View Godoc Here"),this);

    m_addFolderAct = new QAction(tr("Add Folder..."),this);
    m_closeFolerAct = new QAction(tr("Close Folder"),this);

    m_closeAllFoldersAct = new QAction(tr("Close All Folders"),this);

    connect(m_openEditorAct,SIGNAL(triggered()),this,SLOT(openEditor()));
    connect(m_newFileAct,SIGNAL(triggered()),this,SLOT(newFile()));
    connect(m_newFileWizardAct,SIGNAL(triggered()),this,SLOT(newFileWizard()));
    connect(m_renameFileAct,SIGNAL(triggered()),this,SLOT(renameFile()));
    connect(m_removeFileAct,SIGNAL(triggered()),this,SLOT(removeFile()));
    connect(m_newFolderAct,SIGNAL(triggered()),this,SLOT(newFolder()));
    connect(m_renameFolderAct,SIGNAL(triggered()),this,SLOT(renameFolder()));
    connect(m_removeFolderAct,SIGNAL(triggered()),this,SLOT(removeFolder()));
    connect(m_openShellAct,SIGNAL(triggered()),this,SLOT(openShell()));
    connect(m_openExplorerAct,SIGNAL(triggered()),this,SLOT(openExplorer()));
    connect(m_viewGodocAct,SIGNAL(triggered()),this,SLOT(viewGodoc()));
    connect(m_addFolderAct,SIGNAL(triggered()),this,SLOT(addFolder()));
    connect(m_closeFolerAct,SIGNAL(triggered()),this,SLOT(closeFolder()));
    connect(m_closeAllFoldersAct,SIGNAL(triggered()),this,SLOT(closeAllFolders()));

    connect(m_tree,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(treeViewContextMenuRequested(QPoint)));
    connect(m_tree,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(openPathIndex(QModelIndex)));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));
}

void FoldersView::showHideFiles(bool b)
{
    if (isShowHideFiles() == b) {
        return;
    }
    QDir::Filters filters = m_model->filter();
    if (b) {
        filters |= QDir::Hidden;
    } else {
        filters ^= QDir::Hidden;
    }
    m_model->setFilter(filters);
}

bool FoldersView::isShowHideFiles() const
{
    return m_model->filter() & QDir::Hidden;
}

QDir FoldersView::contextDir() const
{
    if (m_contextInfo.isDir()) {
        return m_contextInfo.filePath();
    }
    return m_contextInfo.dir();
}

QFileInfo FoldersView::contextFileInfo() const
{
    return m_contextInfo;
}

void FoldersView::openEditor()
{
    if (m_contextInfo.isFile()) {
        m_liteApp->fileManager()->openEditor(m_contextInfo.filePath());
    }
}

void FoldersView::newFile()
{
    QDir dir = contextDir();

    CreateFileDialog dlg;
    dlg.setDirectory(dir.path());
    if (dlg.exec() == QDialog::Rejected) {
        return;
    }
    QString fileName = dlg.getFileName();
    if (!fileName.isEmpty()) {
        QString filePath = QFileInfo(dir,fileName).filePath();
        if (QFile::exists(filePath)) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Create File"),
                                     tr("A file with that name already exists!"));
        } else {
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly)) {
                file.close();
                if (dlg.isOpenEditor()) {
                    m_liteApp->fileManager()->openEditor(filePath,true);
                }
            } else {
                QMessageBox::information(m_liteApp->mainWindow(),tr("Create File"),
                                         tr("Failed to create the file!"));
            }
        }
    }
}

void FoldersView::newFileWizard()
{
    QString filePath;
    QString projPath;
    QFileInfo info = contextFileInfo();
    QDir dir = contextDir();
    if (!info.isFile()) {
        filePath = dir.absolutePath();
        projPath = dir.absolutePath();
    } else {
        filePath = dir.absolutePath();
        dir.cdUp();
        projPath = dir.absolutePath();
    }
    m_liteApp->fileManager()->execFileWizard(projPath,filePath);
}

void FoldersView::renameFile()
{
    QFileInfo info = contextFileInfo();
    if (!info.isFile()) {
        return;
    }
    QString fileName = QInputDialog::getText(m_liteApp->mainWindow(),
                                             tr("Rename File"),tr("New Name:"),
                                             QLineEdit::Normal,info.fileName());
    if (!fileName.isEmpty() && fileName != info.fileName()) {
        QDir dir = contextDir();
#ifdef Q_OS_WIN
        if (!MoveFileW(info.filePath().toStdWString().c_str(),QFileInfo(dir,fileName).filePath().toStdWString().c_str())) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Rename File"),
                                     tr("Failed to rename the file!"));
        }
#else
        if (!QFile::rename(info.filePath(),QFileInfo(dir,fileName).filePath())) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Rename File"),
                                     tr("Failed to rename the file!"));
        }
#endif
    }
}

void FoldersView::removeFile()
{
    QFileInfo info = contextFileInfo();
    if (!info.isFile()) {
        return;
    }

    int ret = QMessageBox::question(m_liteApp->mainWindow(),tr("Delete File"),
                          tr("Are you sure that you want to permanently delete this file?"),
                          QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        if (!QFile::remove(info.filePath())) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Delete File"),
                                     tr("Failed to delete the file!"));
        }
    }
}

void FoldersView::newFolder()
{
    QDir dir = contextDir();

    CreateDirDialog dlg;
    dlg.setDirectory(dir.path());
    if (dlg.exec() == QDialog::Rejected) {
        return;
    }

    QString folderName = dlg.getDirPath();
    if (!folderName.isEmpty()) {
        if (!dir.entryList(QStringList() << folderName,QDir::Dirs).isEmpty()) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Create Folder"),
                                     tr("A folder with that name already exists!"));
        } else if (!dir.mkpath(folderName)) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Create Folder"),
                                     tr("Failed to create the folder!"));
        }
    }
}

void FoldersView::renameFolder()
{
    QFileInfo info = contextFileInfo();
    if (!info.isDir()) {
        return;
    }

    QString folderName = QInputDialog::getText(m_liteApp->mainWindow(),
                                               tr("Rename Folder"),tr("Folder Name"),
                                               QLineEdit::Normal,info.fileName());
    if (!folderName.isEmpty() && folderName != info.fileName()) {
        QDir dir = contextDir();
        dir.cdUp();
#ifdef Q_OS_WIN
        QString _old = info.filePath();
        QString _new = dir.path()+"/"+folderName;
        if (!MoveFileW(_old.toStdWString().c_str(),_new.toStdWString().c_str())) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Rename Folder"),
                                     tr("Failed to rename the folder!"));
        }
#else
        if (!dir.rename(info.fileName(),folderName)) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Rename Folder"),
                                     tr("Failed to rename the folder!"));
        }
#endif
    }
}

void FoldersView::removeFolder()
{
    QFileInfo info = contextFileInfo();
    if (!info.isDir()) {
        return;
    }

    int ret = QMessageBox::warning(m_liteApp->mainWindow(),tr("Delete Folder"),
                          tr("Are you sure that you want to permanently delete this folder and all of its contents?"),
                          QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        QDir dir = info.dir();
        if (!dir.rmdir(info.fileName())) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Delete Folder"),
                                     tr("Failed to delete the folder!"));
        }
    }
}

void FoldersView::openExplorer()
{
    QDir dir = contextDir();
    QDesktopServices::openUrl(QUrl::fromLocalFile(dir.path()));
}

void FoldersView::viewGodoc()
{
    QDir dir = contextDir();
    LiteApi::IGolangDoc *doc = LiteApi::findExtensionObject<LiteApi::IGolangDoc*>(m_liteApp,"LiteApi.IGolangDoc");
    if (doc) {
        QUrl url;
        url.setScheme("pdoc");
        url.setPath(dir.path());
        doc->openUrl(url);
        doc->activeBrowser();
    }
}

void FoldersView::addFolder()
{
#if QT_VERSION >= 0x050000
        static QString home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#else
        static QString home = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
#endif
    QString folder = QFileDialog::getExistingDirectory(this,tr("Add Folder"),home);
    if (folder.isEmpty()) {
        return;
    }
    m_model->addRootPath(folder);
    QDir dir(folder);
    if (dir.cdUp()) {
        home = dir.path();
    }
}

void FoldersView::closeFolder()
{
//    if (m_contextInfo.exists() && !m_contextInfo.isDir()) {
//        return;
//    }
    this->m_model->removeRootPath(m_contextInfo.filePath());
}

void FoldersView::closeAllFolders()
{
    this->m_model->clear();
}

void FoldersView::openShell()
{
    QDir dir = contextDir();
    QProcessEnvironment env = LiteApi::getCurrentEnvironment(m_liteApp);
    QString shell = env.value("LITEIDE_SHELL");
    if (!shell.isEmpty()) {
        foreach (QString info, shell.split(";",QString::SkipEmptyParts)) {
            QStringList ar = info.split(" ",QString::SkipEmptyParts);
            if (ar.size() >= 1) {
                QString cmd = FileUtil::lookPath(ar[0],LiteApi::getCurrentEnvironment(m_liteApp),false);
                if (!cmd.isEmpty()) {
                    QString path = dir.path();
                    ar.pop_front();
#ifdef Q_OS_MAC
                    ar.push_back(path);
#endif
#ifdef Q_OS_WIN
    if (path.length() == 2 && path.right(1) == ":") {
        path += "/";
    }
#endif
                    QProcess::startDetached(cmd,ar,path);
                    return;
                }
            }
        }
        return;
    }
    QString cmd = env.value("LITEIDE_TERM");
    QStringList args = env.value("LITEIDE_TERMARGS").split(" ",QString::SkipEmptyParts);
    qDebug() << cmd;
    QString path = dir.path();
#ifdef Q_OS_MAC
    args.append(path);
#endif
#ifdef Q_OS_WIN
    if (path.length() == 2 && path.right(1) == ":") {
        path += "/";
    }
#endif
    QProcess::startDetached(cmd,args,path);
}

QModelIndex FoldersView::rootIndex() const
{
    if (m_model->rowCount() == 0) {
        return QModelIndex();
    }
    return m_model->index(0,0);
}

void FoldersView::treeViewContextMenuRequested(const QPoint &pos)
{
    QMenu menu(m_tree);
    LiteApi::FILESYSTEM_CONTEXT_FLAG flag = LiteApi::FILESYSTEM_ROOT;
    QModelIndex index = m_tree->indexAt(pos);
    if (index.isValid()) {
        m_contextInfo = m_model->fileInfo(index);
        m_contextIndex = index;
        if (m_model->isRootIndex(index)) {
            flag = LiteApi::FILESYSTEM_ROOTFOLDER;
        } else if (m_model->isDir(index)) {
            flag = LiteApi::FILESYSTEM_FOLDER;
        } else {
            flag = LiteApi::FILESYSTEM_FILES;
        }
    } else if (!m_bMultiDirMode) {
        m_contextIndex = this->rootIndex();
        m_contextInfo = m_model->fileInfo(m_contextIndex);
        flag = LiteApi::FILESYSTEM_ROOTFOLDER;
    }
    bool hasGo = false;
    if (!m_bMultiDirMode || (flag != LiteApi::FILESYSTEM_ROOT)) {
        foreach(QFileInfo info, contextDir().entryInfoList(QDir::Files)) {
            if (info.suffix() == "go") {
                hasGo = true;
            }
        }
    }
    //root folder
    if (flag == LiteApi::FILESYSTEM_ROOT) {
        if (m_bMultiDirMode) {
            menu.addAction(m_addFolderAct);
        } else {
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
        }
    } else if (flag == LiteApi::FILESYSTEM_ROOTFOLDER) {
        menu.addAction(m_newFileAct);
        menu.addAction(m_newFileWizardAct);
        menu.addAction(m_newFolderAct);
        menu.addAction(m_renameFolderAct);
        menu.addAction(m_closeFolerAct);
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
    menu.exec(m_tree->mapToGlobal(pos));
}

bool FoldersView::addRootPath(const QString &path)
{
    QModelIndex index = m_model->addRootPath(path);
    if (!index.isValid()) {
        return false;
    }
    m_liteApp->fileManager()->addRecentFile(path,"folder");
    m_tree->expand(index);
    return true;
}

void FoldersView::setRootPathList(const QStringList &pathList)
{
    m_model->clear();
    foreach (QString path, pathList) {
        m_model->addRootPath(path);
    }
    //currentEditorChanged(m_liteApp->editorManager()->currentEditor());
}

void FoldersView::setRootPath(const QString &path)
{
    m_model->clear();
    m_model->addRootPath(path);
//    m_model->setRootPath(path);
//    if (m_bHideRoot) {
//        m_tree->expand(this->rootIndex());
//    } else {
//        m_tree->expandToDepth(0);
//    }
}

QString FoldersView::rootPath() const
{
    QStringList paths = m_model->rootPathList();
    if (!paths.isEmpty()) {
        return paths.first();
    }
    return QString();
}

QStringList FoldersView::rootPathList() const
{
    return m_model->rootPathList();
}

SymbolTreeView *FoldersView::treeView() const
{
    return m_tree;
}

FoldersModel *FoldersView::model() const
{
    return m_model;
}

void FoldersView::openPathIndex(const QModelIndex &index)
{
    QFileInfo info = m_model->fileInfo(index);
    if (!info.isFile()) {
        return;
    }
    QString mimeType = m_liteApp->mimeTypeManager()->findMimeTypeByFile(info.filePath());
    if (mimeType.startsWith("text/") || mimeType.startsWith("application/")) {
        m_liteApp->fileManager()->openEditor(info.filePath());
    }
}

void FoldersView::currentEditorChanged(LiteApi::IEditor* /*editor*/)
{
    /*
    if (!m_syncEditor->isChecked()) {
        return;
    }
    if (editor && !editor->filePath().isEmpty()) {
        QModelIndex index = m_model->findPath(editor->filePath());
        if (index.isValid()) {
            m_tree->setCurrentIndex(index);
            m_tree->scrollTo(index,QAbstractItemView::EnsureVisible);
            if (m_syncProject->isChecked()) {
                setStartIndex(index.parent());
            }
        }
    }
    */
}

void FoldersView::syncEditor(bool b)
{
    if (!b) {
       // m_syncProject->setChecked(false);
    }
}
