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
// Module: qsqleditor.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-25
// $Id: qsqleditor.h,v 1.0 2011-7-25 visualfc Exp $

#ifndef QSQLEDITOR_H
#define QSQLEDITOR_H

#include "liteapi.h"
#include "qsqldbfile.h"
#include <QModelIndex>
#include <QtSql/qsql.h>

namespace Ui {
    class QSqlEditor;
}

class QStandardItemModel;

class QSqlEditor : public LiteApi::IEditor
{
    Q_OBJECT
public:
    explicit QSqlEditor(LiteApi::IApplication *app);
    ~QSqlEditor();
public:
    virtual QWidget *widget();
    virtual QString name() const;
public:
    virtual bool open(const QString &fileName,const QString &mimeType);
    virtual bool reload();
    virtual bool save();
    virtual bool saveAs(const QString &fileName);
    virtual void setReadOnly(bool b);
    virtual bool isReadOnly() const;
    virtual bool isModified() const;
    virtual QString fileName() const;
    virtual QString mimeType() const;
    virtual LiteApi::IFile *file();
public:
    void setFile(QSqlDbFile *file);
    void loadDatabase();
protected slots:
    void dbTreeContextMenuRequested(const QPoint& pt);
    void editorTable();
protected:
    void appendTableItems(QSql::TableType type);
    QString tableTypeNames(int type) const;
    QString tableTypeName(int type) const;
protected:
    LiteApi::IApplication *m_liteApp;
    QSqlDbFile  *m_file;
    QWidget     *m_widget;
    Ui::QSqlEditor  *ui;
    bool    m_bReadOnly;
    QStandardItemModel *m_dbModel;
    QMenu   *m_tableMenu;
    QAction *m_infoAct;
    QAction *m_editorAct;
    QModelIndex m_contextIndex;
};

#endif // QSQLEDITOR_H
