#ifndef CODECOMPLETER_H
#define CODECOMPLETER_H

#include <QCompleter>

class QListView;
class QKeyEvent;
class QStandardItemModel;
class CodeCompleter : public QCompleter
{
    Q_OBJECT
public:
    explicit CodeCompleter(QObject *parent = 0);
    virtual ~CodeCompleter();
    QString separator() const;
    void setSeparator(const QString &separator);
protected:
    virtual QStringList splitPath(const QString &path) const;
    virtual QString pathFromIndex(const QModelIndex &index) const;
private:
    QString m_seperator;
};

#endif // CODECOMPLETER_H
