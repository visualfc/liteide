/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 visualfc. All rights reserved.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** In addition, as a special exception,  that plugins developed for LiteIDE,
** are allowed to remain closed sourced and can be distributed under any license .
** These rights are included in the file LGPL_EXCEPTION.txt in this package.
**
**************************************************************************/
// Module: basefoldeview.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef BASEFOLDERVIEW_H
#define BASEFOLDERVIEW_H

#include "liteapi/liteapi.h"
#include "symboltreeview/symboltreeview.h"
#include <QTreeView>

class BaseFolderView : public SymbolTreeView
{
    Q_OBJECT
public:
    explicit BaseFolderView(LiteApi::IApplication *app,QWidget *parent = 0);
    QDir contextDir() const;
    QFileInfo contextFileInfo() const;
signals:
    void aboutToShowContextMenu(QMenu *menu, LiteApi::FILESYSTEM_CONTEXT_FLAG flag, const QFileInfo &info);
public slots:
    virtual void openBundle();
    virtual void openInNewWindow();
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
    virtual void openFolder();
    virtual void closeFolder();
    virtual void reloadFolder();
    virtual void closeAllFolders();
    virtual void copyFile();
    virtual void pasteFile();
    virtual bool canPasteFile();
protected:
    LiteApi::IApplication *m_liteApp;
    QFileInfo m_contextInfo;
    QModelIndex m_contextIndex;
    QAction *m_openInNewWindowAct;
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
    QAction *m_openFolderAct;
    QAction *m_closeFolderAct;
    QAction *m_reloadFolderAct;
    QAction *m_closeAllFoldersAct;
    QAction *m_openBundleAct;
    QAction *m_copyFileAct;
    QAction *m_pasteFileAct;
    bool copy_dir(const QString &src, const QString &dest_root);
};

#endif // BASEFOLDERVIEW_H
