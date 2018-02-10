#ifndef BOXFOLDERWINDOW_H
#define BOXFOLDERWINDOW_H

#include "filemanager.h"
#include "folderview/folderview.h"

class QTreeView;
class QStackedWidget;
class QTreeWidget;
class QSplitter;
class QStandardItemModel;
class BoxFolderView;
class BoxFolderWindow : public IFolderWindow
{
    Q_OBJECT
public:
    BoxFolderWindow(LiteApi::IApplication *app, QObject *parent = 0);
    virtual ~BoxFolderWindow();
    virtual QStringList folderList() const;
    virtual void setFolderList(const QStringList &folders);
    virtual void addFolderList(const QString &folder);
    virtual void closeAllFolders();
public slots:
    void currentIndexChanged(const QModelIndex &index,const QModelIndex &prev);
    void closeFolderIndex(const QModelIndex &index);
    void reloadFolderIndex(const QModelIndex &index);
    void showHideFiles(bool b);
    void setShowDetails(bool b);
    void currentEditorChanged(LiteApi::IEditor *editor);
    void triggeredSyncEditor(bool b);
    void doubleClickedFolderView(const QModelIndex &index);
    void enterKeyPressedFolderView(const QModelIndex &index);
protected:
    LiteApi::IApplication *m_liteApp;
    QSplitter *m_spliter;
    BoxFolderView *m_tree;
    QStandardItemModel *m_model;
    QStackedWidget *m_stacked;
    QStringList m_folderList;
    QAction *m_toolWindowAct;
    QMenu*       m_filterMenu;
    QAction*     m_showHideFilesAct;
    QAction*     m_showDetailsAct;
    QAction*     m_syncEditorAct;
    QDir::Filters m_filters;
    bool m_bShowDetails;
protected:
    void addFolderImpl(const QString &folder);
    int findInStacked(const QModelIndex &index);
};

class BoxFolderView : public BaseFolderView
{
    Q_OBJECT
public:
    BoxFolderView(LiteApi::IApplication *app, QWidget *parent = 0);
    void addRootPath(const QString &folder);
    void clear();
signals:
    void closeFolderIndex(const QModelIndex &index);
    void reloadFolderIndex(const QModelIndex &index);
public slots:
    void customContextMenuRequested(const QPoint &pos);
    virtual void openFolder();
    virtual void closeFolder();
    virtual void reloadFolder();
protected:
    QStandardItemModel *m_model;
    QMenu *m_contextMenu;
};

#endif // BOXFOLDERWINDOW_H
