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
// Module: findapiwidget.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef FINDAPIWIDGET_H
#define FINDAPIWIDGET_H

#include "liteapi/liteapi.h"
#include "qtc_editutil/fancylineedit.h"
#include "qt_browser/chasewidget.h"
#include "processex/processex.h"
#include <QWidget>
#include <QThread>
#include <QModelIndex>
#include <QSet>

class QListView;
class QStandardItemModel;
class FindApiThread : public QThread
{
    Q_OBJECT
public:
    FindApiThread(QObject *parent);
    ~FindApiThread();
signals:
    void findApiOut(const QString &api, const QString &text, const QStringList &url);
public:
    void setRootPath(const QString &rootPath);
    void findApi(const QString &text);
    void stopFind();
    void setMatchCase(bool b);
    void findInFile(const QString &fileName);
protected:
    virtual void	run ();
    QString m_text;
    QString m_rootPath;
    bool    m_bMatchCase;
};

class FindApiEdit : public Utils::FancyLineEdit
{
    Q_OBJECT
public:
    FindApiEdit(QWidget *parent = 0)
        : Utils::FancyLineEdit(parent)
    {
        QIcon icon = QIcon::fromTheme(layoutDirection() == Qt::LeftToRight ?
                         QLatin1String("edit-clear-locationbar-rtl") :
                         QLatin1String("edit-clear-locationbar-ltr"),
                         QIcon::fromTheme(QLatin1String("edit-clear"), QIcon(QLatin1String("icon:images/editclear.png"))));

        setButtonPixmap(Right, icon.pixmap(16));
        setPlaceholderText(tr("Search"));
        setButtonToolTip(Right, tr("Stop Search"));
    }
    void showStopButton(bool b)
    {
        this->setButtonVisible(Right,b);
    }
};

class FindApiWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FindApiWidget(LiteApi::IApplication *app,QWidget *parent = 0);
    ~FindApiWidget();
signals:
    void openApiUrl(QStringList);
public slots:
    void findApi();
    void findApiOut(QString,QString,QStringList);
    void findApiFinished();
    void doubleClickedApi(QModelIndex);
   // void rebuildApiData();
    //QString apiDataFile() const;
protected:
    FindApiThread *m_findThread;
    LiteApi::IApplication *m_liteApp;
    FindApiEdit           *m_findEdit;
    ChaseWidget           *m_chaseWidget;
    QListView             *m_listView;
    QStandardItemModel    *m_model;
    ProcessEx             *m_rebuildThread;
    QAction               *m_findAct;
    QAction               *m_caseCheckAct;
    //QAction               *m_rebuildAct;
};

#endif // FINDAPIWIDGET_H
