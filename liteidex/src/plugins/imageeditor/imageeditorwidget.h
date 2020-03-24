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
// Module: imageeditorwidget.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef IMAGEEDITORWIDGET_H
#define IMAGEEDITORWIDGET_H

#include <QGraphicsView>

class QGestureEvent;
class QPinchGesture;
class QPanGesture;
class ImageEditorWidget : public QGraphicsView
{
    Q_OBJECT
public:
    ImageEditorWidget();
    virtual ~ImageEditorWidget();
    void setImageItem(QGraphicsItem *item);
    void doScale(qreal factor);
    qreal scaleFactor() const;
public slots:
    void zoomIn();
    void zoomOut();
    void resetSize();
    void fitToView();
signals:
    void scaleFactorChanged(qreal factory);
protected:
    void drawBackground(QPainter *p, const QRectF &);
    void wheelEvent(QWheelEvent *event);
    bool event(QEvent *event);
    bool gestureEvent(QGestureEvent *event);
    void pinchTriggered(QPinchGesture *gesture);
    void emitScaleFactor();
protected:
    QGraphicsItem *m_imageItem;
    double m_scaleFactor;
};

#endif // IMAGEEDITORWIDGET_H
