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
// Module: liteeditorfile.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "liteeditorfile.h"
#include "liteeditor_global.h"
#include <QFile>
#include <QTextDocument>
#include <QTextCodec>
#include <QTextStream>
#include <QMessageBox>
#include <QDir>
#include <QDebug>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


LiteEditorFile::LiteEditorFile(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IFile(parent),
      m_liteApp(app)
{
    //m_codec = QTextCodec::codecForLocale();
    m_codec = QTextCodec::codecForName("utf-8");
    m_hasDecodingError = false;
    m_bReadOnly = false;
    m_lineTerminatorMode = NativeLineTerminator;
}

QString LiteEditorFile::filePath() const
{
    return m_fileName;
}

bool LiteEditorFile::isReadOnly() const
{
    if (m_hasDecodingError) {
        return true;
    }
    return m_bReadOnly;
}

bool LiteEditorFile::saveText(const QString &fileName, const QString &text)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QIODevice::Truncate)) {
        return false;
    }
    m_fileName = fileName;

    QString saveText = text;
    if (m_lineTerminatorMode == CRLFLineTerminator) {
        saveText.replace(QLatin1Char('\n'), QLatin1String("\r\n"));
    }

    if (m_codec) {
        file.write(m_codec->fromUnicode(saveText));
    } else {
        file.write(saveText.toLocal8Bit());
    }
    return true;
}

bool LiteEditorFile::reloadTextByCodec(const QString &codecName, QString &outText)
{
    setTextCodec(codecName);
    return loadFileHelper(m_fileName,m_mimeType,false,outText);
}

bool LiteEditorFile::reloadText(QString &outText)
{
    return loadText(m_fileName,m_mimeType,outText);
}

QString LiteEditorFile::mimeType() const
{
    return m_mimeType;
}

void LiteEditorFile::setTextCodec(const QString &name)
{
    QTextCodec *codec = QTextCodec::codecForName(name.toLatin1());
    if (codec) {
        m_codec = codec;
    }
}

QString LiteEditorFile::textCodec() const
{
    return m_codec->name();
}

bool LiteEditorFile::loadFileHelper(const QString &fileName, const QString &mimeType, bool bCheckCodec, QString &outText)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        return false;
    }
    const QFileInfo fi(fileName);
    m_bReadOnly = !fi.isWritable();

    m_mimeType = mimeType;
    m_fileName = fileName;

    QByteArray buf = file.readAll();
    m_hasDecodingError = false;

    if (bCheckCodec) {
        if (mimeType == "text/html" || mimeType == "text/xml") {
            m_codec = QTextCodec::codecForHtml(buf,QTextCodec::codecForName("utf-8"));
        } else {
            LiteApi::IMimeType *im = m_liteApp->mimeTypeManager()->findMimeType(mimeType);
            if (im) {
                QString codecName = im->codec();
                if (!codecName.isEmpty()) {
                    m_codec = QTextCodec::codecForName(codecName.toLatin1());
                }
            }
            int bytesRead = buf.size();
            QTextCodec *codec = m_codec;
            // code taken from qtextstream
            if (bytesRead >= 4 && ((uchar(buf[0]) == 0xff && uchar(buf[1]) == 0xfe && uchar(buf[2]) == 0 && uchar(buf[3]) == 0)
                                   || (uchar(buf[0]) == 0 && uchar(buf[1]) == 0 && uchar(buf[2]) == 0xfe && uchar(buf[3]) == 0xff))) {
                codec = QTextCodec::codecForName("UTF-32");
            } else if (bytesRead >= 2 && ((uchar(buf[0]) == 0xff && uchar(buf[1]) == 0xfe)
                                          || (uchar(buf[0]) == 0xfe && uchar(buf[1]) == 0xff))) {
                codec = QTextCodec::codecForName("UTF-16");
            } else if (bytesRead >= 3 && uchar(buf[0]) == 0xef && uchar(buf[1]) == 0xbb && uchar(buf[2])== 0xbf) {
                codec = QTextCodec::codecForName("UTF-8");
            } else if (!codec) {
                codec = QTextCodec::codecForLocale();
            }
            // end code taken from qtextstream
            m_codec = codec;
        }
    }


    QTextCodec::ConverterState state;
    outText = m_codec->toUnicode(buf,buf.size(),&state);
    if (state.invalidChars > 0 || state.remainingChars > 0) {
        m_hasDecodingError = true;
    }
    //qDebug() << state.invalidChars << state.remainingChars;

/*
    QByteArray verifyBuf = m_codec->fromUnicode(text); // slow
    // the minSize trick lets us ignore unicode headers
    int minSize = qMin(verifyBuf.size(), buf.size());
    m_hasDecodingError = (minSize < buf.size()- 4
                          || memcmp(verifyBuf.constData() + verifyBuf.size() - minSize,
                                    buf.constData() + buf.size() - minSize, minSize));
*/
    /*
    if (text.length()*2+4 < buf.length()) {
        m_hasDecodingError = true;
    }
    */

    int lf = outText.indexOf('\n');
    if (lf < 0) {
        m_lineTerminatorMode = NativeLineTerminator;
    } else if (lf == 0) {
        m_lineTerminatorMode = LFLineTerminator;
    } else {
        lf = outText.indexOf(QRegExp("[^\r]\n"),lf-1);
        if (lf >= 0) {
            m_lineTerminatorMode = LFLineTerminator;
        } else {
            m_lineTerminatorMode = CRLFLineTerminator;
        }
    }

    bool noprintCheck = m_liteApp->settings()->value(EDITOR_NOPRINTCHECK,true).toBool();
    if (noprintCheck && !LiteApi::mimeIsText(mimeType)) {
        for (int i = 0; i < outText.length(); i++) {
            if (!outText[i].isPrint() && !outText[i].isSpace() && outText[i] != '\r' && outText[i] != '\n') {
                outText[i] = '.';
                m_hasDecodingError = true;
            }
        }
    }

    return true;
}

bool LiteEditorFile::setLineEndUnix(bool b)
{
    if (this->isLineEndUnix() == b) {
        return false;
    }
    if (b) {
        m_lineTerminatorMode = LFLineTerminator;
    } else {
        m_lineTerminatorMode = CRLFLineTerminator;
    }
    return true;
}

bool LiteEditorFile::isLineEndUnix() const
{
    return m_lineTerminatorMode == LFLineTerminator;
}

bool LiteEditorFile::isLineEndWindow() const
{
    return m_lineTerminatorMode == CRLFLineTerminator;
}

void LiteEditorFile::setMimeType(const QString &mimeType)
{
    m_mimeType = mimeType;
}

bool LiteEditorFile::loadText(const QString &fileName, const QString &mimeType, QString &outText)
{    
    return loadFileHelper(fileName,mimeType,true,outText);
}
