%Module FakeVim

%Import QtCore/QtCoremod.sip
%Import QtGui/QtGuimod.sip

%If (Qt_5_0_0 -)
    %Import QtWidgets/QtWidgetsmod.sip
%End


int FAKEVIM_PYQT_VERSION;

%ModuleCode
#include <fakevim/fakevimhandler.h>
static const int FAKEVIM_PYQT_VERSION = FAKEVIM_PYQT_MAJOR_VERSION;
%End

enum RangeMode
{
    // Reordering first three enum items here will break
    // compatibility with clipboard format stored by Vim.
    RangeCharMode,         // v
    RangeLineMode,         // V
    RangeBlockMode,        // Ctrl-v
    RangeLineModeExclusive,
    RangeBlockAndTailMode // Ctrl-v for D and X
};

struct Range
{

%TypeHeaderCode
#include <fakevim/fakevimhandler.h>
using namespace FakeVim::Internal;
%End

    Range();
    Range(int b, int e, RangeMode m = RangeCharMode);
    QString toString() const;
    bool isValid() const;

    int beginPos;
    int endPos;
    RangeMode rangemode;
};

struct ExCommand
{

%TypeHeaderCode
#include <fakevim/fakevimhandler.h>
using namespace FakeVim::Internal;
%End

    ExCommand();
    ExCommand(const QString &cmd, const QString &args = QString(),
        const Range &range = Range());

    bool matches(const QString &min, const QString &full) const;

    QString cmd;
    bool hasBang;
    QString args;
    Range range;
    int count;
};

// message levels sorted by severity
enum MessageLevel
{
    MessageMode,    // show current mode (format "-- %1 --")
    MessageCommand, // show last Ex command or search
    MessageInfo,    // result of a command
    MessageWarning, // warning
    MessageError,   // error
    MessageShowCmd  // partial command
};

class FakeVimHandler : QObject /NoDefaultCtors/ {

%TypeHeaderCode
#include <fakevim/fakevimhandler.h>
using namespace FakeVim::Internal;
%End

public:
    explicit FakeVimHandler(QWidget *widget, QObject *parent /TransferThis/ = 0);

    ~FakeVimHandler();

    QWidget *widget();

    // call before widget is deleted
    void disconnectFromEditor();

public slots:
    void setCurrentFileName(const QString &fileName);
    QString currentFileName() const;

    void showMessage(MessageLevel level, const QString &msg);

    // This executes an "ex" style command taking context
    // information from the current widget.
    void handleCommand(const QString &cmd);
    void handleReplay(const QString &keys);
    void handleInput(const QString &keys);

    void installEventFilter();

    // Convenience
    void setupWidget();
    void restoreWidget(int tabSize);

    // Test only
    int physicalIndentation(const QString &line) const;
    int logicalIndentation(const QString &line) const;
    QString tabExpand(int n) const;

    void miniBufferTextEdited(const QString &text, int cursorPos, int anchorPos);

    // Set text cursor position. Keeps anchor if in visual mode.
    void setTextCursorPosition(int position);

    QTextCursor textCursor() const;
    void setTextCursor(const QTextCursor &cursor);

    bool jumpToLocalMark(QChar mark, bool backTickMode);

private:
    virtual bool handleExCommand(const ExCommand &cmd);
};

class FakeVimProxy : QObject /NoDefaultCtors/ {
%TypeHeaderCode
#include <fakevimproxy.h>
%End

public:
    explicit FakeVimProxy(QObject *handler, QObject *parent /TransferThis/ = 0);

    virtual void commandBufferChanged(const QString &msg, int cursorPos,
            int anchorPos, int messageLevel, QObject *eventFilter);
    virtual void statusDataChanged(const QString &msg);
    virtual void extraInformationChanged(const QString &msg);
    virtual void selectionChanged(const QList<QTextEdit::ExtraSelection> &selection);
    virtual void highlightMatches(const QString &needle);
    virtual QString writeAllRequested();
    virtual QTextCursor moveToMatchingParenthesis();
    virtual bool checkForElectricCharacter(QChar c);
    virtual void indentRegion(int beginLine, int endLine, QChar typedChar);
    virtual void completionRequested();
    virtual void simpleCompletionRequested(const QString &needle, bool forward);
    virtual void windowCommandRequested(const QString &key, int count);
    virtual void findRequested(bool reverse);
    virtual void findNextRequested(bool reverse);
    virtual bool handleExCommand(const ExCommand &);
    virtual void disableBlockSelection();
    virtual void enableBlockSelection(const QTextCursor &cursor);
    virtual QTextCursor blockSelection();
    virtual bool hasBlockSelection();
    virtual void foldToggle(int depth);
    virtual void foldAll(bool fold);
    virtual void fold(int depth, bool fold);
    virtual void foldGoTo(int count, bool current);
    virtual void jumpToGlobalMark(QChar mark, bool backTickMode, const QString &fileName);
};

