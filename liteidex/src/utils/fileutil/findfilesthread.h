#ifndef FINDFILESTHREAD_H
#define FINDFILESTHREAD_H

#include <QThread>
#include <QString>
#include <QSet>

class FindFilesThread : public QThread
{
    Q_OBJECT
public:
    FindFilesThread(QObject *parent);
    void setFolderList(const QStringList &folderLis, const QSet<QString> &extSet, const QSet<QString> &exceptFiles, int maxCount);
    void stop(int time = 10);
protected:
    virtual void run();
    void findFolder(QString folder);
    void sendResult(const QStringList &fileList);
signals:
    void findResult(const QStringList &fileList);
protected:
    QStringList m_folderList;
    QSet<QString> m_exceptFiles;
    QSet<QString> m_extSet;
    QSet<QString> m_processFolderSet;
    int           m_maxFileCount;
    int           m_filesCount;
    int           m_maxBlockSendCount;
    bool          m_cancel;
};

#endif // FINDFILESTHREAD_H
