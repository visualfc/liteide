/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "fakevimhandler.h"

#include <QApplication>
#include <QFontMetrics>
#include <QMainWindow>
#include <QMessageBox>
#include <QPainter>
#include <QPlainTextEdit>
#include <QStatusBar>
#include <QTextBlock>
#include <QTextEdit>
#include <QTextStream>
#include <QTemporaryFile>
#include <QTimer>

#define EDITOR(editor, call) \
    if (QPlainTextEdit *ed = qobject_cast<QPlainTextEdit *>(editor)) { \
        (ed->call); \
    } else if (QTextEdit *ed = qobject_cast<QTextEdit *>(editor)) { \
        (ed->call); \
    }

using namespace FakeVim::Internal;

typedef QLatin1String _;

/**
 * Simple editor widget.
 * @tparam TextEdit QTextEdit or QPlainTextEdit as base class
 */
template <typename TextEdit>
class Editor : public TextEdit
{
public:
    Editor(QWidget *parent = 0) : TextEdit(parent)
    {
        TextEdit::setCursorWidth(0);
    }

    void paintEvent(QPaintEvent *e)
    {
        TextEdit::paintEvent(e);

        if ( !m_cursorRect.isNull() && e->rect().intersects(m_cursorRect) ) {
            QRect rect = m_cursorRect;
            m_cursorRect = QRect();
            TextEdit::viewport()->update(rect);
        }

        // Draw text cursor.
        QRect rect = TextEdit::cursorRect();
        if ( e->rect().intersects(rect) ) {
            QPainter painter(TextEdit::viewport());

            if ( TextEdit::overwriteMode() ) {
                QFontMetrics fm(TextEdit::font());
                const int position = TextEdit::textCursor().position();
                const QChar c = TextEdit::document()->characterAt(position);
                rect.setWidth(fm.width(c));
                painter.setPen(Qt::NoPen);
                painter.setBrush(TextEdit::palette().color(QPalette::Base));
                painter.setCompositionMode(QPainter::CompositionMode_Difference);
            } else {
                rect.setWidth(TextEdit::cursorWidth());
                painter.setPen(TextEdit::palette().color(QPalette::Text));
            }

            painter.drawRect(rect);
            m_cursorRect = rect;
        }
    }

private:
    QRect m_cursorRect;
};

class Proxy : public QObject
{
    Q_OBJECT

public:
    Proxy(QWidget *widget, QMainWindow *mw, QObject *parent = 0)
      : QObject(parent), m_widget(widget), m_mainWindow(mw)
    {
        QTimer::singleShot(0, this, SLOT(parseArguments()));
    }

    void openFile(const QString &fileName)
    {
        emit handleInput(QString(_(":r %1<CR>")).arg(fileName));
        m_fileName = fileName;
    }

signals:
    void handleInput(const QString &keys);

public slots:
    void changeStatusData(const QString &info)
    {
        m_statusData = info;
        updateStatusBar();
    }

    void highlightMatches(const QString &pattern)
    {
        QTextEdit *ed = qobject_cast<QTextEdit *>(m_widget);
        if (!ed)
            return;

        QTextCursor cur = ed->textCursor();

        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(Qt::yellow);
        selection.format.setForeground(Qt::black);

        // Highlight matches.
        QTextDocument *doc = ed->document();
        QRegExp re(pattern);
        cur = doc->find(re);

        m_searchSelection.clear();

        int a = cur.position();
        while ( !cur.isNull() ) {
            if ( cur.hasSelection() ) {
                selection.cursor = cur;
                m_searchSelection.append(selection);
            } else {
                cur.movePosition(QTextCursor::NextCharacter);
            }
            cur = doc->find(re, cur);
            int b = cur.position();
            if (a == b) {
                cur.movePosition(QTextCursor::NextCharacter);
                cur = doc->find(re, cur);
                b = cur.position();
                if (a == b) break;
            }
            a = b;
        }

        updateExtraSelections();
    }

    void changeStatusMessage(const QString &contents, int cursorPos)
    {
        m_statusMessage = cursorPos == -1 ? contents
            : contents.left(cursorPos) + QChar(10073) + contents.mid(cursorPos);
        updateStatusBar();
    }

    void changeExtraInformation(const QString &info)
    {
        QMessageBox::information(m_widget, tr("Information"), info);
    }

    void updateStatusBar()
    {
        int slack = 80 - m_statusMessage.size() - m_statusData.size();
        QString msg = m_statusMessage + QString(slack, QLatin1Char(' ')) + m_statusData;
        m_mainWindow->statusBar()->showMessage(msg);
    }

    void handleExCommand(bool *handled, const ExCommand &cmd)
    {
        if ( wantSaveAndQuit(cmd) ) {
            // :wq
            if (save())
                cancel();
        } else if ( wantSave(cmd) ) {
            save(); // :w
        } else if ( wantQuit(cmd) ) {
            if (cmd.hasBang)
                invalidate(); // :q!
            else
                cancel(); // :q
        } else {
            *handled = false;
            return;
        }

        *handled = true;
    }

    void requestSetBlockSelection(bool on = false)
    {
        QTextEdit *ed = qobject_cast<QTextEdit *>(m_widget);
        if (!ed)
            return;

        QPalette pal = ed->parentWidget() != NULL ? ed->parentWidget()->palette()
                                                  : QApplication::palette();

        m_blockSelection.clear();
        m_clearSelection.clear();

        if (on) {
            QTextCursor cur = ed->textCursor();

            QTextEdit::ExtraSelection selection;
            selection.format.setBackground( pal.color(QPalette::Base) );
            selection.format.setForeground( pal.color(QPalette::Text) );
            selection.cursor = cur;
            m_clearSelection.append(selection);

            selection.format.setBackground( pal.color(QPalette::Highlight) );
            selection.format.setForeground( pal.color(QPalette::HighlightedText) );

            int from = cur.positionInBlock();
            int to = cur.anchor() - cur.document()->findBlock(cur.anchor()).position();
            const int min = qMin(cur.position(), cur.anchor());
            const int max = qMax(cur.position(), cur.anchor());
            for ( QTextBlock block = cur.document()->findBlock(min);
                  block.isValid() && block.position() < max; block = block.next() ) {
                cur.setPosition( block.position() + qMin(from, block.length()) );
                cur.setPosition( block.position() + qMin(to, block.length()), QTextCursor::KeepAnchor );
                selection.cursor = cur;
                m_blockSelection.append(selection);
            }

            connect( ed, SIGNAL(selectionChanged()),
                     SLOT(requestSetBlockSelection()), Qt::UniqueConnection );

            QPalette pal2 = ed->palette();
            pal2.setColor(QPalette::Highlight, Qt::transparent);
            pal2.setColor(QPalette::HighlightedText, Qt::transparent);
            ed->setPalette(pal2);

        } else {
            ed->setPalette(pal);

            disconnect( ed, SIGNAL(selectionChanged()),
                        this, SLOT(requestSetBlockSelection()) );
        }

        updateExtraSelections();
    }

    void requestHasBlockSelection(bool *on)
    {
        *on = !m_blockSelection.isEmpty();
    }

private slots:
    void parseArguments()
    {
        QStringList args = qApp->arguments();

        const QString editFileName = args.value(1, QString(_("/usr/share/vim/vim74/tutor/tutor")));
        openFile(editFileName);

        foreach (const QString &cmd, args.mid(2))
            emit handleInput(cmd);
    }

private:
    void updateExtraSelections()
    {
        QTextEdit *ed = qobject_cast<QTextEdit *>(m_widget);
        if (ed)
            ed->setExtraSelections(m_clearSelection + m_searchSelection + m_blockSelection);
    }

    bool wantSaveAndQuit(const ExCommand &cmd)
    {
        return cmd.cmd == "wq";
    }

    bool wantSave(const ExCommand &cmd)
    {
        return cmd.matches("w", "write") || cmd.matches("wa", "wall");
    }

    bool wantQuit(const ExCommand &cmd)
    {
        return cmd.matches("q", "quit") || cmd.matches("qa", "qall");
    }

    bool save()
    {
        if (!hasChanges())
            return true;

        QTemporaryFile tmpFile;
        if (!tmpFile.open()) {
            QMessageBox::critical(m_widget, tr("FakeVim Error"),
                                  tr("Cannot create temporary file: %1").arg(tmpFile.errorString()));
            return false;
        }

        QTextStream ts(&tmpFile);
        ts << content();
        ts.flush();

        QFile::remove(m_fileName);
        if (!QFile::copy(tmpFile.fileName(), m_fileName)) {
            QMessageBox::critical(m_widget, tr("FakeVim Error"),
                                  tr("Cannot write to file \"%1\"").arg(m_fileName));
            return false;
        }

        return true;
    }

    void cancel()
    {
        if (hasChanges()) {
            QMessageBox::critical(m_widget, tr("FakeVim Warning"),
                                  tr("File \"%1\" was changed").arg(m_fileName));
        } else {
            invalidate();
        }
    }

    void invalidate()
    {
        QApplication::quit();
    }

    bool hasChanges()
    {
        if (m_fileName.isEmpty() && content().isEmpty())
            return false;

        QFile f(m_fileName);
        if (!f.open(QIODevice::ReadOnly))
            return true;

        QTextStream ts(&f);
        return content() != ts.readAll();
    }

    QTextDocument *document() const
    {
        QTextDocument *doc = NULL;
        if (QPlainTextEdit *ed = qobject_cast<QPlainTextEdit *>(m_widget))
            doc = ed->document();
        else if (QTextEdit *ed = qobject_cast<QTextEdit *>(m_widget))
            doc = ed->document();
        return doc;
    }

    QString content() const
    {
        return document()->toPlainText();
    }

    QWidget *m_widget;
    QMainWindow *m_mainWindow;
    QString m_statusMessage;
    QString m_statusData;
    QString m_fileName;

    QList<QTextEdit::ExtraSelection> m_searchSelection;
    QList<QTextEdit::ExtraSelection> m_clearSelection;
    QList<QTextEdit::ExtraSelection> m_blockSelection;
};

QWidget *createEditorWidget(bool usePlainTextEdit)
{
    QWidget *editor = 0;
    if (usePlainTextEdit) {
        Editor<QPlainTextEdit> *w = new Editor<QPlainTextEdit>;
        w->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        editor = w;
    } else {
        Editor<QTextEdit> *w = new Editor<QTextEdit>;
        w->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        editor = w;
    }
    editor->setObjectName(_("Editor"));
    editor->setFocus();

    return editor;
}

void initHandler(FakeVimHandler &handler)
{
    handler.handleCommand(_("set nopasskeys"));
    handler.handleCommand(_("set nopasscontrolkey"));

    // Set some Vim options.
    handler.handleCommand(_("set expandtab"));
    handler.handleCommand(_("set shiftwidth=8"));
    handler.handleCommand(_("set tabstop=16"));
    handler.handleCommand(_("set autoindent"));

    // Try to source file "fakevimrc" from current directory.
    handler.handleCommand(_("source fakevimrc"));

    handler.installEventFilter();
    handler.setupWidget();
}

void initMainWindow(QMainWindow &mainWindow, QWidget *centralWidget, const QString &title)
{
    mainWindow.setWindowTitle(QString(_("FakeVim (%1)")).arg(title));
    mainWindow.setCentralWidget(centralWidget);
    mainWindow.resize(600, 650);
    mainWindow.move(0, 0);
    mainWindow.show();

    // Set monospace font for editor and status bar.
    QFont font = QApplication::font();
    font.setFamily(_("Monospace"));
    centralWidget->setFont(font);
    mainWindow.statusBar()->setFont(font);
}

void clearUndoRedo(QWidget *editor)
{
    EDITOR(editor, setUndoRedoEnabled(false));
    EDITOR(editor, setUndoRedoEnabled(true));
}

void connectSignals(FakeVimHandler &handler, Proxy &proxy)
{
    QObject::connect(&handler, SIGNAL(commandBufferChanged(QString,int,int,int,QObject*)),
        &proxy, SLOT(changeStatusMessage(QString,int)));
    QObject::connect(&handler, SIGNAL(extraInformationChanged(QString)),
        &proxy, SLOT(changeExtraInformation(QString)));
    QObject::connect(&handler, SIGNAL(statusDataChanged(QString)),
        &proxy, SLOT(changeStatusData(QString)));
    QObject::connect(&handler, SIGNAL(highlightMatches(QString)),
        &proxy, SLOT(highlightMatches(QString)));
    QObject::connect(&handler, SIGNAL(handleExCommandRequested(bool*,ExCommand)),
        &proxy, SLOT(handleExCommand(bool*,ExCommand)));
    QObject::connect(&handler, SIGNAL(requestSetBlockSelection(bool)),
        &proxy, SLOT(requestSetBlockSelection(bool)));
    QObject::connect(&handler, SIGNAL(requestHasBlockSelection(bool*)),
        &proxy, SLOT(requestHasBlockSelection(bool*)));

    QObject::connect(&proxy, SIGNAL(handleInput(QString)),
        &handler, SLOT(handleInput(QString)));
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QStringList args = app.arguments();

    // If FAKEVIM_PLAIN_TEXT_EDIT environment variable is 1 use QPlainTextEdit instead on QTextEdit;
    bool usePlainTextEdit = qgetenv("FAKEVIM_PLAIN_TEXT_EDIT") == "1";

    // Create editor widget.
    QWidget *editor = createEditorWidget(usePlainTextEdit);

    // Create FakeVimHandler instance which will emulate Vim behavior in editor widget.
    FakeVimHandler handler(editor, 0);

    // Create main window.
    QMainWindow mainWindow;
    initMainWindow(mainWindow, editor, usePlainTextEdit ? _("QPlainTextEdit") : _("QTextEdit"));

    // Connect slots to FakeVimHandler signals.
    Proxy proxy(editor, &mainWindow);
    connectSignals(handler, proxy);

    // Initialize FakeVimHandler.
    initHandler(handler);

    // Clear undo and redo queues.
    clearUndoRedo(editor);

    return app.exec();
}

#include "main.moc"
