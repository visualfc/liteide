#ifndef QUICKOPENFILESYSTEM_H
#define QUICKOPENFILESYSTEM_H

#include "quickopenapi/quickopenapi.h"

class QFileSystemModel;
class QSortFilterProxyModel;
class FileSystemModelEx;
class QuickOpenFileSystem : public LiteApi::IQuickOpenFileSystem
{
public:
    QuickOpenFileSystem(LiteApi::IApplication *app, QObject *parent);
    virtual QString id() const;
    virtual QString info() const;
    virtual QString placeholderText() const;
    virtual void activate();
    virtual QAbstractItemModel *model() const;
    virtual QModelIndex rootIndex() const;
    virtual void updateModel();
    virtual QModelIndex filterChanged(const QString &text);
    virtual void indexChanged(const QModelIndex &index);
    virtual bool selected(const QString &text, const QModelIndex &index);
    virtual void cancel();
    virtual void setRootPath(const QString &path);
    virtual void setPlaceholderText(const QString &text);
protected:
    LiteApi::IApplication *m_liteApp;
    FileSystemModelEx *m_model;
    QSortFilterProxyModel *m_proxy;
    QModelIndex m_rootIndex;
    QString m_placehoderText;
};

#endif // QUICKOPENFILESYSTEM_H
