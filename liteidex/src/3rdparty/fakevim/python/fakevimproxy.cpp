#include "python/fakevimproxy.h"

#include <QMetaMethod>
#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QTextCursor>
#include <QTextEdit>

FakeVimProxy::FakeVimProxy(QObject *handler, QObject *parent)
    : QObject(parent)
{
    const QMetaObject *mo = metaObject();

    for (int i = 0; i < mo->methodCount(); ++i) {
        QMetaMethod method = mo->method(i);
        QByteArray signature =
#if QT_VERSION < 0x050000
            method.signature();
#else
            method.methodSignature();
#endif

        if (method.methodType() == QMetaMethod::Slot && signature.startsWith("on_")) {
            const QByteArray signalSignature = signature.mid(3);
            const int signalIndex = handler->metaObject()->indexOfSignal(signalSignature);
            Q_ASSERT(signalIndex != -1);
            connect(handler, handler->metaObject()->method(signalIndex), this, method);
        }
    }

}

void FakeVimProxy::on_commandBufferChanged(const QString &msg, int cursorPos,
        int anchorPos, int messageLevel, QObject *eventFilter)
{
    commandBufferChanged(msg, cursorPos, anchorPos, messageLevel, eventFilter);
}

void FakeVimProxy::on_statusDataChanged(const QString &msg)
{
    statusDataChanged(msg);
}


void FakeVimProxy::on_extraInformationChanged(const QString &msg)
{
    extraInformationChanged(msg);
}

void FakeVimProxy::on_selectionChanged(const QList<QTextEdit::ExtraSelection> &selection)
{
    selectionChanged(selection);
}

void FakeVimProxy::on_highlightMatches(const QString &needle)
{
    highlightMatches(needle);
}

void FakeVimProxy::on_writeAllRequested(QString *error)
{
    *error = writeAllRequested();
}

void FakeVimProxy::on_moveToMatchingParenthesis(bool *moved, bool *forward, QTextCursor *cursor)
{
    const QTextCursor cursor2 = moveToMatchingParenthesis();
    *moved = cursor2.isNull();
    if (*moved) {
        *moved = true;
        *forward = cursor2.position() > cursor->position();
        *cursor = cursor2;
    }
}

void FakeVimProxy::on_checkForElectricCharacter(bool *result, QChar c)
{
    *result = checkForElectricCharacter(c);
}

void FakeVimProxy::on_indentRegion(int beginLine, int endLine, QChar typedChar)
{
    indentRegion(beginLine, endLine, typedChar);
}

void FakeVimProxy::on_completionRequested()
{
    completionRequested();
}

void FakeVimProxy::on_simpleCompletionRequested(const QString &needle, bool forward)
{
    simpleCompletionRequested(needle, forward);
}

void FakeVimProxy::on_windowCommandRequested(const QString &key, int count)
{
    windowCommandRequested(key, count);
}

void FakeVimProxy::on_findRequested(bool reverse)
{
    findRequested(reverse);
}

void FakeVimProxy::on_findNextRequested(bool reverse)
{
    findNextRequested(reverse);
}

void FakeVimProxy::on_handleExCommandRequested(bool *handled, const FakeVim::Internal::ExCommand &cmd)
{
    *handled = handleExCommand(cmd);
}

void FakeVimProxy::on_requestDisableBlockSelection()
{
    disableBlockSelection();
}

void FakeVimProxy::on_requestSetBlockSelection(const QTextCursor &cursor)
{
    enableBlockSelection(cursor);
}

void FakeVimProxy::on_requestBlockSelection(QTextCursor *cursor)
{
    *cursor = blockSelection();
}

void FakeVimProxy::on_requestHasBlockSelection(bool *on)
{
    *on = hasBlockSelection();
}

void FakeVimProxy::on_foldToggle(int depth)
{
    foldToggle(depth);
}

void FakeVimProxy::on_foldAll(bool fold)
{
    foldAll(fold);
}

void FakeVimProxy::on_fold(int depth, bool fold)
{
    this->fold(depth, fold);
}

void FakeVimProxy::on_foldGoTo(int count, bool current)
{
    foldGoTo(count, current);
}

void FakeVimProxy::on_jumpToGlobalMark(QChar mark, bool backTickMode, const QString &fileName)
{
    jumpToGlobalMark(mark, backTickMode, fileName);
}

