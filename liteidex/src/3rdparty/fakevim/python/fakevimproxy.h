#include <fakevim/fakevimhandler.h>

#include <QObject>
#include <QTextCursor>
#include <QTextEdit>

class QString;

typedef FakeVim::Internal::ExCommand ExCommand;

class FakeVimProxy : public QObject
{
    Q_OBJECT
public:
    explicit FakeVimProxy(QObject *handler, QObject *parent = 0);

    virtual void commandBufferChanged(const QString &, int, int, int, QObject *) {}
    virtual void statusDataChanged(const QString &) {}
    virtual void extraInformationChanged(const QString &) {}
    virtual void selectionChanged(const QList<QTextEdit::ExtraSelection> &) {}
    virtual void highlightMatches(const QString &) {}
    virtual QString writeAllRequested() { return QString(); }
    virtual QTextCursor moveToMatchingParenthesis() { return QTextCursor(); }
    virtual bool checkForElectricCharacter(QChar) { return false; }
    virtual void indentRegion(int, int, QChar) {}
    virtual void completionRequested() {}
    virtual void simpleCompletionRequested(const QString &, bool) {}
    virtual void windowCommandRequested(const QString &, int) {}
    virtual void findRequested(bool) {}
    virtual void findNextRequested(bool) {}
    virtual bool handleExCommand(const ExCommand &) { return false; }
    virtual void disableBlockSelection() {}
    virtual void enableBlockSelection(const QTextCursor &) {}
    virtual QTextCursor blockSelection() { return QTextCursor(); }
    virtual bool hasBlockSelection() { return false; }
    virtual void foldToggle(int) {}
    virtual void foldAll(bool) {}
    virtual void fold(int, bool) {}
    virtual void foldGoTo(int, bool) {}
    virtual void jumpToGlobalMark(QChar, bool, const QString &) {}

private slots:
    void on_commandBufferChanged(const QString &msg, int cursorPos,
            int anchorPos, int messageLevel, QObject *eventFilter);
    void on_statusDataChanged(const QString &msg);
    void on_extraInformationChanged(const QString &msg);
    void on_selectionChanged(const QList<QTextEdit::ExtraSelection> &selection);
    void on_highlightMatches(const QString &needle);
    void on_writeAllRequested(QString *error);
    void on_moveToMatchingParenthesis(bool *moved, bool *forward, QTextCursor *cursor);
    void on_checkForElectricCharacter(bool *result, QChar c);
    void on_indentRegion(int beginLine, int endLine, QChar typedChar);
    void on_completionRequested();
    void on_simpleCompletionRequested(const QString &needle, bool forward);
    void on_windowCommandRequested(const QString &key, int count);
    void on_findRequested(bool reverse);
    void on_findNextRequested(bool reverse);
    void on_handleExCommandRequested(bool *handled, const ExCommand &cmd);
    void on_requestDisableBlockSelection();
    void on_requestSetBlockSelection(const QTextCursor&);
    void on_requestBlockSelection(QTextCursor *cursor);
    void on_requestHasBlockSelection(bool *on);
    void on_foldToggle(int depth);
    void on_foldAll(bool fold);
    void on_fold(int depth, bool fold);
    void on_foldGoTo(int count, bool current);
    void on_jumpToGlobalMark(QChar mark, bool backTickMode, const QString &fileName);
};

