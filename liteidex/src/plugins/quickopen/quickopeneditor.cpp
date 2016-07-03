#include "quickopeneditor.h"
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QStandardItem>

QuickOpenEditor::QuickOpenEditor(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IQuickOpen(parent), m_liteApp(app)
{
    m_model = new QStandardItemModel(this);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
}

QString QuickOpenEditor::id() const
{
    return "quickopen/editor";
}

QString QuickOpenEditor::info() const
{
    return tr("Open Editor by Name");
}

QAbstractItemModel *QuickOpenEditor::model() const
{
    return m_proxyModel;
}

void QuickOpenEditor::updateModel()
{
    m_model->clear();
    m_proxyModel->setFilterFixedString("");
    m_proxyModel->setFilterKeyColumn(2);

    foreach(LiteApi::IEditor *editor, m_liteApp->editorManager()->editorList()) {
        if (editor->filePath().isEmpty()) {
            continue;
        }
        m_model->appendRow(QList<QStandardItem*>() << new QStandardItem(editor->name()) << new QStandardItem(editor->filePath()) );
    }
    m_proxyModel->sort(0);
}

QModelIndex QuickOpenEditor::filter(const QString &text)
{
    m_proxyModel->setFilterFixedString(text);

    for(int i = 0; i < m_proxyModel->rowCount(); i++) {
        QModelIndex index = m_proxyModel->index(i,1);
        QString name = index.data().toString();
        if (name.contains(text)) {
            return index;
        }
    }
    if (m_proxyModel->rowCount() > 0)
        return m_proxyModel->index(0,0);
    return QModelIndex();
}

bool QuickOpenEditor::selected(const QString &text, const QModelIndex &index)
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
