
#ifndef GOLANGPLSUSAGE_H
#define GOLANGPLSUSAGE_H

#include "liteenvapi/liteenvapi.h"
#include "litefindapi/litefindapi.h"
#include "liteeditorapi/liteeditorapi.h"
#include "processex/processex.h"
#include "goplstypes.h"

class GolangPlsUsage : public LiteApi::IFileSearch
{
    Q_OBJECT
public:
    explicit GolangPlsUsage(QObject *parent = 0);
    virtual ~GolangPlsUsage();

    QString mimeType() const;
    QString displayName() const;
    QWidget *widget() const;
    void start();
    void cancel();
    void activate();
    QString searchText() const;
    bool replaceMode() const;
    bool canCancel() const;
    void setSearchInfo(const QString &text, const QString &filter, const QString &path);
public slots:
    void loadResults(const QString &filename, const QList<UsageResult> &list);

protected:
    QString m_searchText;
};

#endif // GOLANGPLSUSAGE_H
