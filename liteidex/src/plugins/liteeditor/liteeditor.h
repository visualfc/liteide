/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011 LiteIDE Team. All rights reserved.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** In addition, as a special exception,  that plugins developed for LiteIDE,
** are allowed to remain closed sourced and can be distributed under any license .
** These rights are included in the file LGPL_EXCEPTION.txt in this package.
**
**************************************************************************/
// Module: liteeditor.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: liteeditor.h,v 1.0 2011-7-13 visualfc Exp $

#ifndef LITEEDITOR_H
#define LITEEDITOR_H

#include "liteapi/liteapi.h"
#include "liteeditorapi/liteeditorapi.h"
#include "extension/extension.h"
#include "elidedlabel/elidedlabel.h"
#include <QSet>
#include <QHash>
#include <QStack>
#include <QTextBlock>
#include <QLabel>

//#define LITEEDITOR_FIND

class TreeModelCompleter;
class QAbstractItemModel;
class QStandardItemModel;
class QStandardItem;
class LiteEditorWidget;
class LiteEditorFile;
class QComboBox;
class QLabel;
class QToolButton;
class LiteCompleter;
class ColorStyleScheme;

class QLabelEx : public QLabel
{
    Q_OBJECT
public:
    QLabelEx(const QString &text, QWidget *parent = 0);
signals:
    void doubleClickEvent();
protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
};

class LiteEditor : public LiteApi::ITextEditor
{
    Q_OBJECT
public:
    LiteEditor(LiteApi::IApplication *app);
    virtual ~LiteEditor();
    void setCompleter(LiteCompleter *complter);
    void setEditorMark(LiteApi::IEditorMark *mark);
    void createActions();
    void createToolBars();
    void createContextMenu();
    virtual LiteApi::IExtension *extension();
    virtual QWidget *widget();
    virtual QString name() const;
    virtual QIcon icon() const;

    virtual bool createNew(const QString &contents, const QString &mimeType);
    virtual bool open(const QString &filePath, const QString &mimeType);
    virtual bool reload();
    virtual bool save();
    virtual bool saveAs(const QString &filePath);
    virtual void setReadOnly(bool b);
    virtual bool isReadOnly() const;
    virtual bool isModified() const;
    virtual QString filePath() const;
    virtual QString mimeType() const;
    virtual LiteApi::IFile *file();
    virtual int line() const;
    virtual int column() const;
    virtual int utf8Position() const;
    virtual QByteArray utf8Data() const;
    virtual void gotoLine(int line, int column, bool center);
    virtual QString textCodec() const;
    virtual void setTextCodec(const QString &codec);
    virtual QMenu *contextMenu() const;
    virtual QByteArray saveState() const;
    virtual bool restoreState(const QByteArray &state);
    virtual void onActive();

    const ColorStyleScheme *colorStyleScheme() const;
    LiteEditorWidget *editorWidget() const;
signals:
    void colorStyleChanged();
public slots:
    void applyOption(QString);
    void clipbordDataChanged();
#ifdef LITEEDITOR_FIND
    void findNextText();
#endif
    void updateTip(QString,QString);
    void exportPdf();
    void exportHtml();
    void filePrint();
    void filePrintPreview();
    void printPreview(QPrinter *printer);
    void codecComboBoxChanged(QString);
    void editPositionChanged();
    void navigationStateChanged(const QByteArray &state);
    void gotoLine();
    void selectNextParam();
public:
    void findCodecs();
    ColorStyleScheme    *m_colorStyleScheme;
    QList<QTextCodec *> m_codecs;
    LiteApi::IApplication *m_liteApp;
    Extension   *m_extension;
    QWidget *m_widget;
    QToolBar *m_toolBar;
    LiteEditorWidget    *m_editorWidget;
    LiteCompleter   *m_completer;
    QAction *m_undoAct;
    QAction *m_redoAct;
    QAction *m_cutAct;
    QAction *m_copyAct;
    QAction *m_pasteAct;
    QAction *m_selectAllAct;
    QAction *m_exportHtmlAct;
    QAction *m_exportPdfAct;
    QAction *m_filePrintAct;
    QAction *m_filePrintPreviewAct;
#ifdef LITEEDITOR_FIND
    QComboBox *m_findComboBox;
#endif
    QAction *m_gotoPrevBlockAct;
    QAction *m_gotoNextBlockAct;
    QAction *m_gotoMatchBraceAct;
    QAction *m_selectBlockAct;
    QAction *m_gotoLineAct;
    QAction *m_lineInfoAct;
    QAction *m_duplicateAct;
    QAction *m_foldAct;
    QAction *m_unfoldAct;
    QAction *m_foldAllAct;
    QAction *m_unfoldAllAct;
    LiteEditorFile *m_file;
    QMenu   *m_contextMenu;
    QString  m_colorStyle;
    QPalette m_defPalette;
    bool       m_bReadOnly;
    QLabelEx  *m_lineInfo;
    QWidget *m_spacerWidget;
};

#endif //LITEEDITOR_H
