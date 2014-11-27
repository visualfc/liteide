#ifndef FOLDERVIEW_H
#define FOLDERVIEW_H

#include <QTreeView>
#include <QFileSystemModel>
#include "basefoldeview.h"

class FolderView : public BaseFolderView
{
    Q_OBJECT
public:
    explicit FolderView(LiteApi::IApplication *app, QWidget *parent = 0);
    void setRootPath(const QString &path);
    QString rootPath() const;
    QFileSystemModel *model() const;
public slots:
    void customContextMenuRequested(const QPoint &pos);    
    virtual void removeFolder();
    virtual void removeFile();
protected:
    QFileSystemModel *m_model;
};

#endif // FOLDERVIEW_H
