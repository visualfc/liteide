#ifndef QUICKOPENFOLDER_H
#define QUICKOPENFOLDER_H

#include "quickopenapi/quickopenapi.h"
#include "fileutil/findfilesthread.h"

class QStandardItemModel;
class QSortFilterProxyModel;

class QuickOpenFolder : public LiteApi::IQuickOpenFolder
{
    Q_OBJECT
public:
    QuickOpenFolder(LiteApi::IApplication *app, QObject *parent = 0);
    virtual ~QuickOpenFolder();
    virtual QString id() const;
    virtual QString info() const;
    virtual QString placeholderText() const;
    virtual void activate();
    virtual QAbstractItemModel *model() const;
    virtual void updateModel();
    virtual QModelIndex filterChanged(const QString &text);
    virtual void indexChanged(const QModelIndex &index);
    virtual bool selected(const QString &text, const QModelIndex &index);
    virtual void cancel();
    virtual void setFolder(const QString &folder);
    virtual void setPlaceholderText(const QString &text);
public slots:
    void findResult(const QStringList &fileList);
protected:
    void startFindThread();
    LiteApi::IApplication *m_liteApp;
    FindFilesThread *m_thread;
    QStandardItemModel *m_model;
    QSortFilterProxyModel *m_proxyModel;
    Qt::CaseSensitivity m_matchCase;
    int                 m_maxCount;
    QString             m_folder;
    QString             m_placeholderText;
};

#endif // QUICKOPENFOLDER_H
