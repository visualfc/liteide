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
// Module: imageeditorfile.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "imageeditorfile.h"
#include <QGraphicsItem>
#include <QImageReader>
#include <QGraphicsPixmapItem>
#include <QMovie>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QPixmap>

#ifndef QT_NO_SVG
#include <QGraphicsSvgItem>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end
#endif

ImageEditorFile::ImageEditorFile(LiteApi::IApplication *app, QObject *parent)
    : QObject(parent), m_liteApp(app)
{
    m_movie = 0;
    m_item = 0;
    m_type = Invalid;
    m_isPaused = true;
}

ImageEditorFile::~ImageEditorFile()
{
    clear();
}

bool ImageEditorFile::open(const QString &filePath, const QString &mimeType)
{
    QByteArray format = QImageReader::imageFormat(filePath);
    if (format.isEmpty()) {
        m_liteApp->appendLog("ImageViewer", QString("Image format not supported. %1").arg(filePath));
        return  false;
    }
#ifndef QT_NO_SVG
    else if (format.startsWith("svg")) {
        QGraphicsItem *item = new QGraphicsSvgItem(filePath);
        if (item->boundingRect().isEmpty()) {
            delete  item;
            m_liteApp->appendLog("ImageViewer", QString("SVG file invalid. %1").arg(filePath));
            return  false;
        }
        m_item = item;
        m_type = Svg;
    }
#endif
     else if (QMovie::supportedFormats().contains(format)) {
        m_movie = new QMovie(filePath,QByteArray(),this);
        if (!m_movie->isValid()) {
            delete  m_movie;
            m_liteApp->appendLog("ImageViewer", QString("Movie file invalid. %1").arg(filePath));
            return  false;
        }
        m_movie->setCacheMode(QMovie::CacheAll);
        m_item = new GraphicsMovieItem(m_movie);
        m_type = Movie;
        connect(m_movie,SIGNAL(finished()),m_movie,SLOT(start()));
        connect(m_movie,SIGNAL(frameChanged(int)),this,SIGNAL(frameChanged(int)));
        m_movie->start();
        m_isPaused = false;
        setPaused(true);
    } else {
        QPixmap pixmap(filePath);
        if (pixmap.isNull()) {
            m_liteApp->appendLog("ImageViewer", QString("Pixmap file invalid. %1").arg(filePath));
            return  false;
        }
        m_type = Pixmap;
        m_item = new QGraphicsPixmapItem(pixmap);
        ((QGraphicsPixmapItem*)m_item)->setTransformationMode(Qt::SmoothTransformation);
    }
    m_mimeType = mimeType;
    m_filePath = filePath;
    return  true;
}

void ImageEditorFile::setPaused(bool paused)
{
    if (!m_movie || m_isPaused == paused)
        return;
    m_isPaused = paused;
    m_movie->setPaused(paused);
    emit isPausedChanged(m_isPaused);
}

QSize ImageEditorFile::imageSize() const
{
    if (m_item) {
        QRectF sz = m_item->boundingRect();
        return  QSize(sz.width(),sz.height());
    }
    return QSize();
}

int ImageEditorFile::frameCount() const
{
    if (m_type != Movie) {
        return 1;
    }
    return  m_movie->frameCount();
}

int ImageEditorFile::currentFrame() const
{
    if (m_type != Movie) {
        return  0;
    }
    return  m_movie->currentFrameNumber();
}

bool ImageEditorFile::jumpToNextFrame()
{
    if (m_type != Movie) {
        return  false;
    }
    int cur = m_movie->currentFrameNumber();
    cur++;
    if (cur < m_movie->frameCount()) {
        return  m_movie->jumpToFrame(cur);
    }
    return  false;
}

bool ImageEditorFile::jumpToPrevFrame()
{
    if (m_type != Movie) {
        return  false;
    }
    int cur = m_movie->currentFrameNumber();
    cur--;
    if (cur >= 0) {
        return m_movie->jumpToFrame(cur);
    }
    return  false;
}

void ImageEditorFile::clear()
{
    if (m_movie) {
        delete  m_movie;
    }
    if (m_item) {
        delete  m_item;
    }
    m_filePath.clear();
    m_type = Invalid;
}

GraphicsMovieItem::GraphicsMovieItem(QMovie *movie)
    : m_movie(movie)
{
    setPixmap(m_movie->currentPixmap());
    connect(m_movie,SIGNAL(updated(QRect)),this,SLOT(movieUpdate(QRect)));
}

void GraphicsMovieItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    const bool smoothTransform = painter->worldTransform().m11() < 1;
    painter->setRenderHint(QPainter::SmoothPixmapTransform, smoothTransform);
    painter->drawPixmap(offset(), m_movie->currentPixmap());
}

QRectF GraphicsMovieItem::boundingRect() const {
    QRect rc = m_movie->frameRect();
    return  QRectF(rc);
}

void GraphicsMovieItem::movieUpdate(const QRect &rc)
{
    QGraphicsPixmapItem::update(rc);
}
