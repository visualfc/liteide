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
// Module: modelfileimpl.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef MODELFILEIMPL_H
#define MODELFILEIMPL_H

#include "liteapi/liteapi.h"

class QStandardItemModel;
class ModelFileImpl : public LiteApi::IFile
{
    Q_OBJECT
public:
    ModelFileImpl(LiteApi::IApplication *app, QObject *parent);
    enum ITEM_TYPE{
        ItemRoot = 1,
        ItemFolder,
        ItemProFile,
        ItemFile
    };
public:
    virtual bool loadText(const QString &filePath, const QString &mimeType, QString &outText);
    virtual bool reloadText(QString &outText);
    virtual bool saveText(const QString &filePath, const QString &text);
    virtual bool isReadOnly() const;
    virtual bool isBinary() const;
    virtual QString filePath() const;
    virtual QString mimeType() const;
public:
    virtual void updateModel() = 0;
protected:
    virtual bool loadFile(const QString &filePath) = 0;
public:
    virtual QString target() const = 0;
    virtual QString targetPath() const = 0;
    virtual QString workPath() const;
    virtual QMap<QString,QString> targetInfo() const = 0;
    QStandardItemModel *model() const;
    QString value(const QString &key) const;
    QStringList values(const QString &key) const;
    QStringList fileNameList() const;
    QStringList filePathList() const;
    QString fileNameToFullPath(const QString &filePath);
protected:
    LiteApi::IApplication *m_liteApp;
    QStandardItemModel       *m_model;
    QString     m_rootPath;
    QMap<QString,QStringList>   m_context;
    QStringList m_fileNameList;
    QStringList m_filePathList;
    QString     m_mimeType;
    QString     m_fileName;
    bool        m_bReadOnly;
};

#endif // MODELFILEIMPL_H
