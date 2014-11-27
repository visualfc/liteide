#ifndef FOLDERSVIEW_H
#define FOLDERSVIEW_H

#include "liteapi/liteapi.h"
#include "foldersmodel.h"
#include "symboltreeview/symboltreeview.h"

class FoldersView : public QWidget
{
    Q_OBJECT
public:
    FoldersView(LiteApi::IApplication *app, QWidget *parent = 0);
public slots:
    void showHideFiles(bool b);
    bool isShowHideFiles() const;
    void pathIndexChanged(const QModelIndex & index);
    void openPathIndex(const QModelIndex &index);
    void currentEditorChanged(LiteApi::IEditor*);
    void treeViewContextMenuRequested(const QPoint &pos);
    void openEditor();
    void newFile();
    void newFileWizard();
    void renameFile();
    void removeFile();
    void newFolder();
    void renameFolder();
    void removeFolder();
    void openShell();
    void openExplorer();
    void viewGodoc();
    void addFolder();
    void closeFolder();
    void closeAllFolders();
    void syncEditor(bool);
signals:
    void aboutToShowContextMenu(QMenu *menu, LiteApi::FILESYSTEM_CONTEXT_FLAG flag, const QFileInfo &info);
public:
    SymbolTreeView *treeView() const;
    FoldersModel   *model() const;
    QFileInfo contextFileInfo() const;
    QDir contextDir() const;
    QModelIndex rootIndex() const;
    bool addRootPath(const QString &path);
    void setRootPathList(const QStringList &pathList);
    void setRootPath(const QString &path);
    QString rootPath() const;
    QStringList rootPathList() const;
protected:
    LiteApi::IApplication *m_liteApp;
    SymbolTreeView  *m_tree;
    FoldersModel    *m_model;
    bool m_bMultiDirMode;
private:
    QFileInfo m_contextInfo;
    QModelIndex m_contextIndex;
    QAction *m_openEditorAct;
    QAction *m_newFileAct;
    QAction *m_newFileWizardAct;
    QAction *m_removeFileAct;
    QAction *m_renameFileAct;
    QAction *m_newFolderAct;
    QAction *m_removeFolderAct;
    QAction *m_renameFolderAct;
    QAction *m_openShellAct;
    QAction *m_openExplorerAct;
    QAction *m_viewGodocAct;
    QAction *m_addFolderAct;
    QAction *m_closeFolerAct;
    QAction *m_closeAllFoldersAct;
};

#endif // FOLDERSVIEW_H
