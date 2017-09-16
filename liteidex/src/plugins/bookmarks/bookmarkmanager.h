#ifndef BOOKMARKMANAGER_H
#define BOOKMARKMANAGER_H

#include "liteapi/liteapi.h"
#include "liteeditorapi/liteeditorapi.h"

#define BookMarkType 3000

class BookmarkManager : public LiteApi::IManager
{
    Q_OBJECT
public:
    BookmarkManager(QObject *parent = 0);
    virtual bool initWithApp(LiteApi::IApplication *app);
public slots:
    void editorCreated(LiteApi::IEditor *editor);
    void editorAboutToClose(LiteApi::IEditor *editor);
    void markListChanged(int type);
    void toggledBookmark();
protected:
    QAction *m_toggleBookmarkAct;
    QList<LiteApi::IEditorMark*> m_editorMarkList;
};

#endif // BOOKMARKMANAGER_H
