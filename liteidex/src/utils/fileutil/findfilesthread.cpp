#include "findfilesthread.h"
#include <QDir>
#include <QFileInfo>

FindFilesThread::FindFilesThread(QObject *parent) : QThread(parent)
{
    m_cancel = false;
    m_maxFileCount = 10000;
    m_filesCount = 0;
    m_maxBlockSendCount = 10;
}

void FindFilesThread::setFolderList(const QStringList &folderList, const QSet<QString> &extSet, const QSet<QString> &exceptFiles, int maxCount)
{
    m_folderList = folderList;
    m_extSet = extSet;
    m_exceptFiles = exceptFiles;
    m_maxFileCount = maxCount;
    m_filesCount = 0;
    m_processFolderSet.clear();
    m_cancel = false;
}

void FindFilesThread::stop(int time)
{
    m_cancel = true;
    if (this->isRunning()) {
        if (!this->wait(time))
            this->terminate();
    }
}

void FindFilesThread::run()
{
    m_cancel = false;
    foreach (QString folder, m_folderList) {
        findFolder(folder);
    }
}

void FindFilesThread::sendResult(const QStringList &fileList)
{
    emit findResult(fileList);
}

void FindFilesThread::findFolder(QString folder)
{
    if (m_cancel) {
        return;
    }
    if (m_processFolderSet.contains(folder)) {
        return;
    }
    m_processFolderSet.insert(folder);
    QDir dir(folder);
    QList<QString> fileList;
    foreach (QFileInfo info, dir.entryInfoList(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot)) {
        if (m_cancel) {
            return;
        }
        QString filePath = info.filePath();
        if (info.isDir()) {
            findFolder(filePath);
        } else if (info.isFile()) {
            if (m_extSet.contains(info.suffix()) && !m_exceptFiles.contains(filePath) ) {
                m_filesCount++;
                if (m_filesCount > m_maxFileCount) {
                    return;
                }
                fileList << filePath;
            }
        }
        if (fileList.size() >= m_maxBlockSendCount) {
            sendResult(fileList);
            fileList.clear();
        }
    }
    if (!fileList.isEmpty()) {
        sendResult(fileList);
    }
}
