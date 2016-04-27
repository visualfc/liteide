#!/usr/bin/env python
from FakeVim import *
import sys
import os

if FAKEVIM_PYQT_VERSION == 5:
    from PyQt5.QtCore import *
    from PyQt5.QtGui import *
    from PyQt5.QtWidgets import *
else:
    from PyQt4.QtCore import *
    from PyQt4.QtGui import *

def overrides(interface_class):
    """ Decorator for overriden methods. """
    def overrider(method):
        assert(method.__name__ in dir(interface_class))
        return method
    return overrider

class Proxy (FakeVimProxy):
    """ Used by FakeVim to modify or retrieve editor state. """
    def __init__(self, window, editor, handler):
        super(Proxy, self).__init__(handler)
        self.__handler = handler
        self.__window = window
        self.__editor = editor

        self.__statusMessage = ""
        self.__statusData = ""
        self.__cursorPosition =  -1
        self.__cursorAnchor =  -1
        self.__eventFilter = None

        self.__fileName = ""
        self.__lastSavePath = ""

    def openFile(self, filePath):
        filePath = os.path.expanduser(filePath)

        try:
            if os.path.isfile(filePath):
                with open(filePath, 'r') as f:
                    self.__fileName = filePath
                    self.__editor.setPlainText(f.read())
            else:
                with open(filePath, 'w') as f:
                    self.__fileName = filePath
        except:
            self.__handler.showMessage( MessageError,
                    self.tr('Cannot open file "{filePath}"')
                    .format(filePath = filePath) )

    def needSave(self):
        return self.__editor.document().isModified()

    def maybeCloseEditor(self):
        if self.needSave():
            self.__handler.showMessage( MessageError,
                    self.tr("No write since last change (add ! to override)") )
            self.__updateStatusBar()

            return False

        return True

    def commandQuit(self):
        qApp.exit()

    def commandWrite(self, filePath = None):
        filePath = filePath and os.path.expanduser(filePath) or self.__fileName

        if not filePath:
            filePath = QFileDialog.getSaveFileName(self.__window, self.tr("Save File ..."), self.__lastSavePath)
            if filePath:
                self.__fileName = filePath
                self.__lastSavePath = filePath

        if not filePath:
            return False

        try:
            with open(filePath, 'w') as f:
                buffer = self.__editor.toPlainText()
                f.write(buffer)
                self.__handler.showMessage( MessageInfo, self.tr('"{filePath}" {lines}L, {characters}C written')
                        .format(
                            filePath = filePath,
                            lines = buffer.count('\n'),
                            characters = len(buffer)) )
        except:
            self.__handler.showMessage( MessageError,
                    self.tr('Failed to save file "{filePath}"')
                    .format(filePath = filePath) )

        self.__editor.document().setModified(False)

        return True

    @overrides(FakeVimProxy)
    def handleExCommand(self, cmd):
        if cmd.matches("e", "edit"):
            if cmd.hasBang or self.maybeCloseEditor():
                self.openFile(cmd.args)
        elif cmd.matches("q", "quit"):
            if cmd.hasBang or self.maybeCloseEditor():
                self.commandQuit()
        elif cmd.matches("w", "write"):
            self.commandWrite(cmd.args)
        elif cmd == "wq":
            self.commandWrite() and self.commandQuit()
        else:
            return False
        return True

    @overrides(FakeVimProxy)
    def enableBlockSelection(self, cursor):
        self.__editor.setTextCursor(cursor)
        self.__editor.setBlockSelection(True)

    @overrides(FakeVimProxy)
    def disableBlockSelection(self):
        self.__editor.setBlockSelection(False)

    @overrides(FakeVimProxy)
    def blockSelection(self):
        self.__editor.setBlockSelection(True)
        return self.__editor.textCursor()

    @overrides(FakeVimProxy)
    def hasBlockSelection(self):
        return self.__editor.hasBlockSelection()

    @overrides(FakeVimProxy)
    def commandBufferChanged(self, msg, cursorPosition, cursorAnchor, messageLevel, eventFilter):
        self.__cursorPosition = cursorPosition
        self.__cursorAnchor = cursorAnchor
        self.__statusMessage = msg
        self.__updateStatusBar();
        self.__eventFilter = eventFilter

    @overrides(FakeVimProxy)
    def statusDataChanged(self, msg):
        self.__statusData = msg
        self.__updateStatusBar()

    @overrides(FakeVimProxy)
    def extraInformationChanged(self, msg):
        QMessageBox.information(self.__window, self.tr("Information"), msg)

    @overrides(FakeVimProxy)
    def highlightMatches(self, pattern):
        self.__editor.highlightMatches(pattern)

    def __updateStatusBar(self):
        self.__window.statusBar().setStatus(
                self.__statusMessage, self.__statusData,
                self.__cursorPosition, self.__cursorAnchor, self.__eventFilter)


class Editor (QTextEdit):
    """ Editor widget driven by FakeVim. """
    def __init__(self, parent = None):
        sup = super(Editor, self)
        sup.__init__(parent)

        self.__context = QAbstractTextDocumentLayout.PaintContext()
        self.__lastCursorRect = QRect()
        self.__hasBlockSelection = False
        self.__selection = []
        self.__searchSelection = []

        self.viewport().installEventFilter(self)

        self.selectionChanged.connect(self.__onSelectionChanged)
        self.cursorPositionChanged.connect(self.__onSelectionChanged)

    def setBlockSelection(self, enabled):
        self.__hasBlockSelection = enabled
        self.__selection.clear()
        self.__updateSelections()

    def hasBlockSelection(self):
        return self.__hasBlockSelection

    def highlightMatches(self, pattern):
        cur = self.textCursor()

        re = QRegExp(pattern)
        cur = self.document().find(re)
        a = cur.position()

        self.__searchSelection.clear()

        while not cur.isNull():
            if cur.hasSelection():
                selection = QAbstractTextDocumentLayout.Selection()
                selection.format.setBackground(Qt.yellow)
                selection.format.setForeground(Qt.black)
                selection.cursor = cur
                self.__searchSelection.append(selection)
            else:
                cur.movePosition(QTextCursor.NextCharacter)

            cur = self.document().find(re, cur)
            b = cur.position()

            if a == b:
                cur.movePosition(QTextCursor.NextCharacter)
                cur = self.document().find(re, cur)
                b = cur.position()

                if (a == b):
                    break
            a = b

        self.__updateSelections()

    def eventFilter(self, viewport, paintEvent):
        """ Handle paint event from text editor. """
        if viewport != self.viewport() or paintEvent.type() != QEvent.Paint:
            return False

        tc = self.textCursor()

        self.__context.cursorPosition = -1
        self.__context.palette = self.palette()

        h = self.__horizontalOffset()
        v = self.__verticalOffset()
        self.__context.clip = QRectF( paintEvent.rect().translated(h, v) )

        painter = QPainter(viewport)

        painter.save()

        # Draw base and text.
        painter.translate(-h, -v)
        self.__paintDocument(painter)

        # Draw block selection.
        if self.hasBlockSelection():
            rect = self.cursorRect(tc)
            tc2 = QTextCursor(tc)
            tc2.setPosition(tc.anchor())
            rect = rect.united( self.cursorRect(tc2) )

            self.__context.palette.setColor( QPalette.Base,
                    self.__context.palette.color(QPalette.Highlight) )
            self.__context.palette.setColor( QPalette.Text,
                    self.__context.palette.color(QPalette.HighlightedText) )

            self.__context.clip = QRectF( rect.translated(h, v) )

            self.__paintDocument(painter)

        painter.restore()

        # Draw text cursor.
        rect = self.cursorRect()

        if self.overwriteMode() or self.hasBlockSelection():
            fm = self.fontMetrics()
            c = self.document().characterAt( tc.position() )
            rect.setWidth( fm.width(c) )
            if rect.width() == 0:
                rect.setWidth( fm.averageCharWidth() )
        else:
            rect.setWidth(2)
            rect.adjust(-1, 0, 0, 0)

        if self.hasBlockSelection():
            startColumn = self.__columnForPosition(tc.anchor())
            endColumn = self.__columnForPosition(tc.position())

            if startColumn < endColumn:
                rect.moveLeft(rect.left() - rect.width())

        painter.setCompositionMode(QPainter.CompositionMode_Difference)
        painter.fillRect(rect, Qt.white)

        if not self.hasBlockSelection() \
           and self.__lastCursorRect.width() != rect.width():
            viewport.update()

        self.__lastCursorRect = rect

        return True

    def __columnForPosition(self, position):
        return position - self.document().findBlock(position).position()

    def __paintDocument(self, painter):
        painter.setClipRect(self.__context.clip)
        painter.fillRect( self.__context.clip, self.__context.palette.base() )
        self.document().documentLayout().draw(painter, self.__context)

    def __horizontalOffset(self):
        hbar = self.horizontalScrollBar()

        if self.isRightToLeft():
            return hbar.maximum() - hbar.value()

        return hbar.value()

    def __verticalOffset(self):
        return self.verticalScrollBar().value()

    def __updateSelections(self):
        self.__context.selections = self.__searchSelection + self.__selection
        self.viewport().update()

    def __onSelectionChanged(self):
        self.__hasBlockSelection = False
        self.__selection.clear()

        selection = QAbstractTextDocumentLayout.Selection()
        selection.cursor = self.textCursor()

        if selection.cursor.hasSelection():
            pal = self.palette()
            selection.format.setBackground( pal.color(QPalette.Highlight) )
            selection.format.setForeground( pal.color(QPalette.HighlightedText) )
            self.__selection.append(selection)

        self.__updateSelections()


class StatusBar (QStatusBar):
    def __init__(self, window):
        super(QStatusBar, self).__init__(window)

        self.__statusMessageLabel = QLabel(self)
        self.__statusDataLabel = QLabel(self)
        self.__commandLine = QLineEdit(self)

        self.addPermanentWidget(self.__statusMessageLabel, 1)
        self.addPermanentWidget(self.__commandLine, 1)
        self.addPermanentWidget(self.__statusDataLabel)

        self.__commandLine.hide()
        window.setStatusBar(self)

    def setStatus(self, statusMessage, statusData, cursorPosition, cursorAnchor, eventFilter):
        commandMode = cursorPosition != -1
        self.__commandLine.setVisible(commandMode)
        self.__statusMessageLabel.setVisible(not commandMode)

        if commandMode:
            self.__commandLine.installEventFilter(eventFilter)
            self.__commandLine.setFocus()
            self.__commandLine.setText(statusMessage)
            self.__commandLine.setSelection(cursorPosition, cursorAnchor - cursorPosition)
        else:
            self.__statusMessageLabel.setText(statusMessage)

        self.__statusDataLabel.setText(statusData)

class MainWindow (QMainWindow):
    def __init__(self, parent = None):
        super(MainWindow, self).__init__(parent)
        self.__editor = Editor(self)
        font = self.__editor.font()
        font.setFamily("Monospace")
        self.__editor.setFont(font)

        self.setCentralWidget(self.__editor)

        self.move(0, 0)
        self.resize(600, 600)

        self.__statusBar = StatusBar(self)

        self.__handler = FakeVimHandler(self.__editor)
        self.__proxy = Proxy(self, self.__editor, self.__handler)

        self.__handler.installEventFilter()
        self.__handler.setupWidget()
        self.__handler.handleCommand(
                'source {home}/.vimrc'.format(home = os.path.expanduser("~")))

    def openFile(self, filePath):
        self.__proxy.openFile(filePath)

    @overrides(QMainWindow)
    def closeEvent(self, event):
        event.ignore()
        if self.__proxy.maybeCloseEditor():
            self.__proxy.commandQuit()

if __name__ == "__main__":
    app = QApplication(sys.argv)

    window = MainWindow()
    if len(sys.argv) > 1:
        window.openFile(sys.argv[1])
    window.show()

    sys.exit(app.exec_())

