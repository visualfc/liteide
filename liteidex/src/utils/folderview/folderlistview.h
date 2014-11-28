#ifndef FOLDERLISTVIEW_H
#define FOLDERLISTVIEW_H

#include "basefoldeview.h"
#include "folderlistmodel.h"

class FolderListView : public BaseFolderView
{
    Q_OBJECT
public:
    explicit FolderListView(LiteApi::IApplication *app, QWidget *parent = 0);
    FolderListModel *model() const;
public:
    void addRootPath(const QString &path);
    void setRootPathList(const QStringList &pathList);
    QStringList rootPathList() const;
    void clear();
public slots:
    void customContextMenuRequested(const QPoint &pos);
    virtual void removeFolder();
    virtual void removeFile();
    virtual void addFolder();
    virtual void closeFolder();
    virtual void closeAllFolders();
protected:
    FolderListModel *m_model;
};

#endif // FOLDERLISTVIEW_H
