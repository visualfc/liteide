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
// Module: imageeditorwidget.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "imageeditorwidget.h"
#include <QPainter>
#include <QGraphicsItem>
#include <QWheelEvent>
#include <QGestureEvent>
#include <QPinchGesture>
#include <QPanGesture>
#include <QScrollBar>
#include <math.h>
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

ImageEditorWidget::ImageEditorWidget()
{
    setScene(new QGraphicsScene(this));
    setTransformationAnchor(AnchorUnderMouse);
    setDragMode(RubberBandDrag);
    setViewportUpdateMode(FullViewportUpdate);
    setFrameShape(QFrame::NoFrame);
    setRenderHint(QPainter::SmoothPixmapTransform);

    QPixmap tilePixmap(20, 20);
    tilePixmap.fill(QColor(229,229,229));
    QPainter tilePainter(&tilePixmap);
    QColor color(177, 177, 177);
    tilePainter.fillRect(0, 0, 10, 10, color);
    tilePainter.fillRect(10, 10, 10, 10, color);
    tilePainter.end();

    setBackgroundBrush(tilePixmap);
    m_scaleFactor = 1.2;
    m_imageItem = 0;
    grabGesture(Qt::PinchGesture);
   // grabGesture(Qt::PanGesture);
}

ImageEditorWidget::~ImageEditorWidget()
{
    if (m_imageItem) {
        this->scene()->removeItem(m_imageItem);
    }
    ungrabGesture(Qt::PinchGesture);
    //ungrabGesture(Qt::PanGesture);
}

void ImageEditorWidget::setImageItem(QGraphicsItem *item)
{
    QGraphicsScene *s = this->scene();
    s->clear();
    m_imageItem = item;
    if (item) {
        item->setZValue(0);
        s->addItem(item);
    }
    this->resetSize();
}


void ImageEditorWidget::drawBackground(QPainter *p, const QRectF &)
{
    p->save();
    p->resetTransform();
    p->fillRect(viewport()->rect(),QColor(236,236,236));
    if (m_imageItem) {
        p->setClipRegion(this->mapFromScene(this->sceneRect()));
    }
    p->drawTiledPixmap(viewport()->rect(), backgroundBrush().texture());
    p->restore();
}

void ImageEditorWidget::doScale(qreal factor)
{
    qreal currentScale = transform().m11();
    qreal newScale = currentScale * factor;
    qreal actualFactor = factor;
    // cap to 0.001 - 1000
    if (newScale > 1000)
        actualFactor = 1000./currentScale;
    else if (newScale < 0.001)
        actualFactor = 0.001/currentScale;

    scale(actualFactor, actualFactor);
    emitScaleFactor();
    QGraphicsPixmapItem *pix = dynamic_cast<QGraphicsPixmapItem*>(m_imageItem);
    if (pix) {
        pix->setTransformationMode(
                    transform().m11() < 1 ? Qt::SmoothTransformation : Qt::FastTransformation);
    }
}

void ImageEditorWidget::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        const int delta = event->delta();
        if (delta < 0)
            zoomOut();
        else if (delta > 0)
            zoomIn();
        return;
    }
    if (event->delta() != 0) {
        int delta = event->delta();
#if QT_VERSION >= 0x050700
        if (event->inverted()) {
            delta = -event->delta();
        }
#endif
        if (event->orientation() == Qt::Horizontal) {
            this->horizontalScrollBar()->setValue(this->horizontalScrollBar()->value()-delta);
        } else {
            this->verticalScrollBar()->setValue(this->verticalScrollBar()->value()-delta);
        }
    }
    event->accept();
}

bool ImageEditorWidget::gestureEvent(QGestureEvent *event)
{
    if (QGesture *pinch = event->gesture(Qt::PinchGesture))
        pinchTriggered(static_cast<QPinchGesture *>(pinch));

    return true;
}

void ImageEditorWidget::pinchTriggered(QPinchGesture *gesture)
{
    QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
    if (changeFlags & QPinchGesture::ScaleFactorChanged)
    {
        qreal value = gesture->property("scaleFactor").toReal();
        doScale(value);
    }
}

bool ImageEditorWidget::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture)
    {
        return gestureEvent(static_cast<QGestureEvent*>(event));
    }
    return QGraphicsView::event(event);
}

void ImageEditorWidget::zoomIn()
{
    doScale(m_scaleFactor);
}

void ImageEditorWidget::zoomOut()
{
    doScale(1.0 / m_scaleFactor);
}

void ImageEditorWidget::resetSize()
{
    resetTransform();
    emitScaleFactor();
}

void ImageEditorWidget::fitToView()
{
    //fitInView(m_imageItem, Qt::KeepAspectRatio);
    QRect viewRect = this->viewport()->rect();
    QRect sceneRect = this->mapFromScene(this->sceneRect()).boundingRect();
    qreal x = qreal(viewRect.width())/sceneRect.width();
    qreal y = qreal(viewRect.height())/sceneRect.height();
    //scale(x,y);
    qreal v = qMin(x,y);
    scale(v,v);
    emitScaleFactor();
}

void ImageEditorWidget::emitScaleFactor()
{
    qreal factor = transform().m11();
    emit scaleFactorChanged(factor);
}

qreal ImageEditorWidget::scaleFactor() const
{
    return transform().m11();
}

