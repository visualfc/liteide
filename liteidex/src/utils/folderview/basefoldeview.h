#ifndef BASEFOLDERVIEW_H
#define BASEFOLDERVIEW_H

#include "liteapi/liteapi.h"
#include <QTreeView>

class BaseFolderView : public QTreeView
{
    Q_OBJECT
public:
    explicit BaseFolderView(LiteApi::IApplication *app,QWidget *parent = 0);
    QDir contextDir() const;
    QFileInfo contextFileInfo() const;
signals:
    void aboutToShowContextMenu(QMenu *menu, LiteApi::FILESYSTEM_CONTEXT_FLAG flag, const QFileInfo &info);
public slots:
    virtual void openEditor();
    virtual void newFile();
    virtual void newFileWizard();
    virtual void renameFile();
    virtual void removeFile();
    virtual void newFolder();
    virtual void renameFolder();
    virtual void removeFolder();
    virtual void openShell();
    virtual void openExplorer();
    virtual void viewGodoc();
    virtual void addFolder();
    virtual void closeFolder();
    virtual void closeAllFolders();
protected:
    LiteApi::IApplication *m_liteApp;
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

#endif // BASEFOLDERVIEW_H
