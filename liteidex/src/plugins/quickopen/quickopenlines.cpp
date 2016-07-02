#include "quickopenlines.h"
#include <QStandardItemModel>
#include <QTextDocument>
#include <QDebug>

QuickOpenLines::QuickOpenLines(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IQuickOpen(parent), m_liteApp(app)
{
    m_model = new QStandardItemModel(this);
}

QString QuickOpenLines::id() const
{
    return "quickopen/lines";
}

QAbstractItemModel *QuickOpenLines::model() const
{
    return m_model;
}

bool QuickOpenLines::update(const QString &text)
{
    m_model->clear();
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        m_model->appendRow(new QStandardItem("editor not support go to lines"));
        return false;
    }
    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    if (!textEditor) {
        m_model->appendRow(new QStandardItem("editor not support go to lines"));
        return false;
    }
    int maxcount = textEditor->document()->blockCount();
    bool ok = false;
    int line = 0;
    if (!text.isEmpty()) {
        line = text.toInt(&ok);
    }
    if (line <= 0 || line > maxcount) {
        ok = false;
    }
    if (!ok) {
        m_model->appendRow(new QStandardItem(QString(tr("please input line range %1 - %2")).arg(1).arg(maxcount)));
        return false;
    }
    m_model->appendRow(new QStandardItem(QString(tr("go to line %1")).arg(line)));
    return true;
}

void QuickOpenLines::updateModel()
{
    update("");
}

QModelIndex QuickOpenLines::filter(const QString &text)
{
    update(text);
    return m_model->index(0,0);
}

bool QuickOpenLines::selected(const QString &text, const QModelIndex &index)
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return false;
    }
    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    if (!textEditor) {
        return false;
    }
    int maxcount = textEditor->document()->blockCount();
    bool ok = false;
    int line = 0;
    if (!text.isEmpty()) {
        line = text.toInt(&ok);
    }
    if (!ok) {
        return false;
    }
    if (line <= 0 || line > maxcount) {
        return false;
    }
    if (line-1 != textEditor->line()) {
        m_liteApp->editorManager()->addNavigationHistory();
    }
    textEditor->gotoLine(line-1,0,true);
    return true;
}
