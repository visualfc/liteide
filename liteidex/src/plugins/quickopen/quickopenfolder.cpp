#include "quickopenfolder.h"
#include "quickopen_global.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QSortFilterProxyModel>
#include <QDir>
#include <QFileInfo>
#include <QTimer>
#include <QApplication>
#include <QDebug>

QuickOpenFolder::QuickOpenFolder(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IQuickOpenFolder(parent), m_liteApp(app)
{
    m_model = new QStandardItemModel(this);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
    m_matchCase = Qt::CaseInsensitive;
    m_maxCount = 100000;
    m_thread = new FindFilesThread(this);
    connect(m_thread,SIGNAL(findResult(QStringList)),this,SLOT(findResult(QStringList)));
}

QuickOpenFolder::~QuickOpenFolder()
{
    if (m_thread) {
        m_thread->stop();
        delete m_thread;
    }
}

QString QuickOpenFolder::id() const
{
    return "quickopen/folder";
}

QString QuickOpenFolder::info() const
{
    return tr("Browser Folder");
}

QString QuickOpenFolder::placeholderText() const
{
    return m_placeholderText;
}

void QuickOpenFolder::activate()
{
}

QAbstractItemModel *QuickOpenFolder::model() const
{
    return m_proxyModel;
}

void QuickOpenFolder::updateModel()
{
    m_maxCount = m_liteApp->settings()->value(QUICKOPEN_FOLDER_MAXCOUNT,100000).toInt();
    m_matchCase = m_liteApp->settings()->value(QUICKOPNE_FOLDER_MATCHCASE,false).toBool() ? Qt::CaseSensitive : Qt::CaseInsensitive;

    m_model->clear();
    m_proxyModel->setFilterFixedString("");
    m_proxyModel->setFilterKeyColumn(2);
    m_proxyModel->setFilterCaseSensitivity(m_matchCase);

    startFindThread();
}

void QuickOpenFolder::startFindThread()
{
    QSet<QString> extSet;
    foreach(LiteApi::IMimeType* type, m_liteApp->mimeTypeManager()->mimeTypeList()) {
        foreach (QString ext, type->allPatterns()) {
            if (ext.startsWith(".")) {
                extSet << ext.mid(1);
            } else if (ext.startsWith("*.")) {
                extSet << ext.mid(2);
            }
        }
    }

    int count = m_model->rowCount();
    int maxcount = count+m_liteApp->settings()->value(QUICKOPEN_FOLDER_MAXCOUNT,100000).toInt();
    QSet<QString> editorSet;

    QStringList folderList;
    folderList << m_folder;
    m_thread->setFolderList(folderList,extSet,editorSet,maxcount);

    m_thread->stop();
    m_thread->start();
}

void QuickOpenFolder::findResult(const QStringList &fileList)
{
    foreach (QString filePath, fileList) {
        m_model->appendRow(QList<QStandardItem*>() << new QStandardItem(QFileInfo(filePath).fileName()) << new QStandardItem(filePath));
    }
}

QModelIndex QuickOpenFolder::filterChanged(const QString &text)
{
    m_proxyModel->setFilterFixedString(text);

    for(int i = 0; i < m_proxyModel->rowCount(); i++) {
        QModelIndex index = m_proxyModel->index(i,0);
        QString name = index.data().toString();
        if (name.startsWith(text,m_matchCase)) {
            return index;
        }
    }
    if (m_proxyModel->rowCount() > 0)
        return m_proxyModel->index(0,0);
    return QModelIndex();
}

void QuickOpenFolder::indexChanged(const QModelIndex &/*index*/)
{
}

bool QuickOpenFolder::selected(const QString &/*text*/, const QModelIndex &index)
{
    if (!index.isValid()) {
        return false;
    }
    QString filePath = m_proxyModel->index(index.row(),1).data().toString();
    if (!m_liteApp->fileManager()->openFile(filePath)) {
        return false;
    }
    return true;
}

void QuickOpenFolder::cancel()
{
    m_thread->stop();
}

void QuickOpenFolder::setFolder(const QString &folder)
{
    m_folder = folder;
}

void QuickOpenFolder::setPlaceholderText(const QString &text)
{
    m_placeholderText = text;
}
