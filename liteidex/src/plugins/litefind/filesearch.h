/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2016 LiteIDE Team. All rights reserved.
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
// Module: filesearch.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef FILESEARCH_H
#define FILESEARCH_H

#include "liteapi/liteapi.h"
#include "litefindapi/litefindapi.h"
#include "textoutput/terminaledit.h"
#include "textoutput/textoutput.h"
#include <QStringList>
#include <QThread>

class FindThread : public QThread
{
Q_OBJECT
public:
    FindThread(QObject *parent = 0);
    virtual void run();
public slots:
    void stop();
protected:
    void findDir(const QRegExp &reg, const QString &path);
    void findFile(const QRegExp &reg, const QString &fileName);
signals:
    void findResult(const LiteApi::FileSearchResult &result);
public:
    bool useRegExp;
    bool matchWord;
    bool matchCase;
    bool findSub;
    QString findText;
    QString findPath;
    QStringList nameFilter;
    volatile bool finding;
};

class QTabWidget;
class QLineEdit;
class QComboBox;
class QCheckBox;
class QPushButton;
class QPlainTextEdit;

class ResultTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    ResultTextEdit(QWidget *parent = 0);
signals:
    void dbclickEvent(const QTextCursor &cur);
public slots:
    void slotCursorPositionChanged();
protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
};

class FileSearch : public LiteApi::IFileSearch
{
    Q_OBJECT
public:
    FileSearch(LiteApi::IApplication *app, QObject *parent = 0);
    ~FileSearch();
    void setVisible(bool b);
    virtual QString mimeType() const;
    virtual QString displayName() const;
    virtual QWidget* widget() const;
    virtual void start();
    virtual void cancel();
    virtual void activate();
    virtual QString searchText() const;
    virtual bool replaceMode() const { return false; }
    virtual bool canCancel() const { return true; }
    virtual void setSearchInfo(const QString &text, const QString &filter, const QString &path);
public slots:
    void findInFiles();
    void browser();
    void currentDir();
protected:
    LiteApi::IApplication *m_liteApp;
    FindThread *m_thread;
    QWidget     *m_findWidget;
    QComboBox   *m_findCombo;
    QComboBox   *m_findPathCombo;
    QComboBox   *m_filterCombo;
    QCheckBox   *m_findSubCheckBox;
    QCheckBox   *m_matchWordCheckBox;
    QCheckBox   *m_matchCaseCheckBox;
    QCheckBox   *m_useRegexCheckBox;
    QCheckBox   *m_autoSwitchPathCheckBox;
    QPushButton *m_findButton;
    QPushButton *m_stopButton;
};

//static QList<FileSearchResult> findInFile(const QString &text, bool useRegExp, bool matchWord, bool matchCase, const QString &fileName);

#endif // FILESEARCH_H
