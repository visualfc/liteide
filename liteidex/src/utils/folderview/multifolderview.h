#ifndef MULTIFOLDERVIEW_H
#define MULTIFOLDERVIEW_H

#include "basefoldeview.h"
#include "multifoldermodel.h"

class MultiFolderView : public BaseFolderView
{
public:
    Q_OBJECT
public:
    explicit MultiFolderView(LiteApi::IApplication *app, QWidget *parent = 0);
    void setFilter(QDir::Filters filters);
    QDir::Filters filter() const;
    QFileInfo fileInfo(const QModelIndex &index) const;
public:
    bool addRootPath(const QString &path);
    void setRootPathList(const QStringList &pathList);
    QStringList rootPathList() const;
    void clear();
    void expandFolder(const QString &path, bool expand);
    QList<QModelIndex> indexForPath(const QString &path) const;
public slots:
    void customContextMenuRequested(const QPoint &pos);
    virtual void removeFolder();
    virtual void removeFile();
    virtual void openFolder();
    virtual void closeFolder();
    virtual void reloadFolder();
    virtual void closeAllFolders();
protected:
    MultiFolderModel *m_model;
};

#endif // MULTIFOLDERVIEW_H
