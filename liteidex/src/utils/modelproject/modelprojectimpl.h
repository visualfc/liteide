/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 LiteIDE. All rights reserved.
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
// Module: modelprojectimpl.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef MODELPROJECTIMPL_H
#define MODELPROJECTIMPL_H

#include "liteapi/liteapi.h"
#include <QModelIndex>
#include <QPointer>

class QTreeView;
class QStandardItemModel;
class ModelFileImpl;
class ModelProjectImpl : public LiteApi::IProject
{
    Q_OBJECT
public:
    ModelProjectImpl(LiteApi::IApplication *app);
    virtual ~ModelProjectImpl();
public:
    virtual QWidget *widget();
    virtual QString name() const;
    virtual QIcon icon() const;
public:
    virtual LiteApi::IFile *file();
    virtual QString filePath() const;
    virtual QString mimeType() const;
    virtual QStringList folderList() const;
    virtual QStringList fileNameList() const;
    virtual QStringList filePathList() const;
    virtual QString fileNameToFullPath(const QString &filePath);
    virtual QMap<QString,QString> targetInfo() const;
    virtual void load();
public:
    virtual bool open(const QString &filePath, const QString &mimeType);
    void setModelFile(ModelFileImpl *file);
protected slots:
    virtual void doubleClickedTree(QModelIndex index);
    virtual void editorSaved(LiteApi::IEditor*);
protected:
    LiteApi::IApplication *m_liteApp;
    QPointer<ModelFileImpl>  m_file;
    QWidget             *m_widget;
    QTreeView           *m_tree;
    QStandardItemModel  *m_model;
};

#endif // MODELPROJECTIMPL_H
