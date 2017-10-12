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
// Module: liteeditorfile.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEEDITORFILE_H
#define LITEEDITORFILE_H

#include "liteapi/liteapi.h"
#include "editorutil/libucd.h"

class QTextDocument;
class LiteEditorFile : public LiteApi::IFile
{
    Q_OBJECT
public:
    LiteEditorFile(LiteApi::IApplication *app, QObject *parent = 0);
    virtual bool loadText(const QString &filePath, const QString &mimeType, QString &outText);
    virtual bool reloadText(QString &outText);
    virtual bool reloadTextByCodec(const QString &codecName, QString &outText);
    virtual bool saveText(const QString &filePath, const QString &text);
    virtual bool isReadOnly() const;
    virtual QString filePath() const;
    virtual void setMimeType(const QString &mimeType);
    virtual QString mimeType() const;
public:
    void setTextCodec(const QString &name);
    QString textCodec() const;
    bool loadFileHelper(const QString &filePath, const QString &mimeType, bool bCheckCodec, QString &outText);
    bool isLineEndUnix() const;
    bool isLineEndWindow() const;
    bool setLineEndUnix(bool b);
protected:
    enum LineTerminatorMode {
        LFLineTerminator = 0,
        CRLFLineTerminator,
        NativeLineTerminator =
#if defined (Q_OS_WIN)
        CRLFLineTerminator
#else
        LFLineTerminator
#endif
    };
    LineTerminatorMode m_lineTerminatorMode;
protected:
    bool m_hasDecodingError;
    bool m_bReadOnly;
    LiteApi::IApplication *m_liteApp;
    QString        m_fileName;
    QString        m_mimeType;
    QTextCodec    *m_codec;
    bool           m_hasUtf8Bom;
    LibUcd         m_libucd;
};

#endif //LITEEDITORFILE_H
