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
// Module: imageeditorfile.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef IMAGEEDITORFILE_H
#define IMAGEEDITORFILE_H

#include "liteapi/liteapi.h"
#include <QGraphicsPixmapItem>

class QGraphicsItem;
class QPixmap;
class QMovie;
class ImageEditorFile : public QObject
{
    Q_OBJECT
public:
    enum Type {
        Invalid = 0,
        Svg = 1,
        Movie = 2,
        Pixmap = 3,
    };
public:
    ImageEditorFile(LiteApi::IApplication *app, QObject *parent);
    virtual ~ImageEditorFile();
    bool open(const QString &filePath, const QString &mimeType);
    bool isValid() const {
        return  m_type != Invalid;
    }
    bool isMovie() const {
        return m_type == Movie;
    }
    virtual QGraphicsItem *graphicsItem() const {
        return  m_item;
    }
    virtual QString mimeType() const {
        return  m_mimeType;
    }
    void setPaused(bool paused);
    bool isPaused() const {
        return  m_isPaused;
    }
    Type type() const {
        return  m_type;
    }
    QString filePath() const {
        return  m_filePath;
    }
    QSize imageSize() const;
    void clear();
public:
    int frameCount() const;
    int currentFrame() const;
public slots:
    bool jumpToNextFrame();
    bool jumpToPrevFrame();
signals:
    void isPausedChanged(bool);
    void frameChanged(int frameNumber);
protected:
    LiteApi::IApplication *m_liteApp;
    QString m_mimeType;
    QGraphicsItem *m_item;
    QPixmap *m_pixmap;
    QMovie *m_movie;
    Type    m_type;
    QString m_filePath;
    bool    m_isPaused;
};

class GraphicsMovieItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    GraphicsMovieItem(QMovie *movie);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
    QRectF boundingRect() const;
public slots:
    void movieUpdate(const QRect &rc);
protected:
    QMovie *m_movie;
};

#endif // IMAGEEDITORFILE_H
