#ifndef GOPLSSEARCHRESULTS_H
#define GOPLSSEARCHRESULTS_H

#include "litefindapi/litefindapi.h"
#include <QJsonArray>

class GoplsSearchResults : public LiteApi::IFileSearch
{
    Q_OBJECT
public:
    explicit GoplsSearchResults(QObject *parent = 0);
    virtual QString mimeType() const;
    virtual QString displayName() const;
    virtual QWidget *widget() const;
    virtual void start();
    virtual void cancel();
    virtual void activate();
    virtual QString searchText() const;
    virtual bool replaceMode() const;
    virtual bool readOnly() const;
    virtual bool canCancel() const;
    virtual void setSearchInfo(const QString &, const QString &, const QString &);

    void showLocations(const QString &title, const QString &searchText, const QJsonArray &locations);

private:
    QString m_title;
    QString m_searchText;
};

#endif // GOPLSSEARCHRESULTS_H
