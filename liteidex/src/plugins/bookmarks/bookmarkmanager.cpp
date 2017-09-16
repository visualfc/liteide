#include "bookmarkmanager.h"
#include <QDebug>

BookmarkManager::BookmarkManager(QObject *parent)
    : LiteApi::IManager(parent)
{

}

bool BookmarkManager::initWithApp(LiteApi::IApplication *app)
{
    if (!IManager::initWithApp(app)) {
        return false;
    }

    LiteApi::IEditorMarkTypeManager *manager = LiteApi::getEditorMarkTypeManager(app);
    if (!manager) {
        return false;
    }
    manager->registerMark(BookMarkType,QIcon("icon:bookmarks/images/bookmark16.png"));

    m_toggleBookmarkAct = new QAction(tr("Toggle Bookmark"),this);

    LiteApi::IActionContext *actionContext = m_liteApp->actionManager()->getActionContext(this,"Bookmarks");
    actionContext->regAction(m_toggleBookmarkAct,"ToggleBookmark","CTRL+M");

    connect(m_toggleBookmarkAct,SIGNAL(triggered()),this,SLOT(toggledBookmark()));

    connect(m_liteApp->editorManager(),SIGNAL(editorCreated(LiteApi::IEditor*)),this,SLOT(editorCreated(LiteApi::IEditor*)));
    connect(m_liteApp->editorManager(),SIGNAL(editorAboutToClose(LiteApi::IEditor*)),this,SLOT(editorAboutToClose(LiteApi::IEditor*)));

    return true;
}

void BookmarkManager::editorCreated(LiteApi::IEditor *editor)
{
    if (!editor) {
        return;
    }
    LiteApi::IEditorMark *mark = LiteApi::getEditorMark(editor);
    if (!mark) {
        return;
    }
    connect(mark,SIGNAL(markListChanged(int)),this,SLOT(markListChanged(int)));
    m_editorMarkList.push_back(mark);

    QMenu *menu = LiteApi::getEditMenu(editor);
    if (menu) {
        menu->addSeparator();
        menu->addAction(m_toggleBookmarkAct);
    }
    menu = LiteApi::getContextMenu(editor);
    if (menu) {
        menu->addSeparator();
        menu->addAction(m_toggleBookmarkAct);
    }
    bool ok;
    QString key = QString("bookmarks/%1").arg(editor->filePath());
    foreach(QString bp, m_liteApp->settings()->value(key).toStringList()) {
        int i = bp.toInt(&ok);
        if (ok) {
            mark->addMark(i,BookMarkType);
        }
    }
}

void BookmarkManager::editorAboutToClose(LiteApi::IEditor *editor)
{
    if (!editor) {
        return;
    }
    LiteApi::IEditorMark *mark = LiteApi::getEditorMark(editor);
    if (!mark) {
        return;
    }
    m_editorMarkList.removeAll(mark);
    QList<int> bpList = mark->markLinesByType(BookMarkType);
    QStringList save;
    foreach(int bp, bpList) {
        save.append(QString("%1").arg(bp));
    }
    QString key = QString("bookmarks/%1").arg(editor->filePath());
    if (save.isEmpty()) {
        m_liteApp->settings()->remove(key);
    } else {
        m_liteApp->settings()->setValue(key,save);
    }
}

void BookmarkManager::markListChanged(int type)
{
    //qDebug() << type;
}

void BookmarkManager::toggledBookmark()
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    LiteApi::IEditorMark *mark = LiteApi::getEditorMark(editor);
    if (!mark) {
        return;
    }
    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    int line = textEditor->line();
    QList<int> types = mark->markTypesByLine(line);
    if (types.contains(BookMarkType)) {
        mark->removeMark(line,BookMarkType);
    } else {
        mark->addMark(line,BookMarkType);
    }
}
