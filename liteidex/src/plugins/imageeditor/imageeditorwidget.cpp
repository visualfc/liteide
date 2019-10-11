#include "imageeditorwidget.h"
#include <QPainter>
#include <QGraphicsItem>
#include <QWheelEvent>
#include <math.h>
#include <QDebug>

ImageEditorWidget::ImageEditorWidget()
{
    setScene(new QGraphicsScene(this));
    setTransformationAnchor(AnchorUnderMouse);
    setDragMode(ScrollHandDrag);
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
}

ImageEditorWidget::~ImageEditorWidget()
{
    if (m_imageItem) {
        this->scene()->removeItem(m_imageItem);
    }
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
   // emitScaleFactor();
    QGraphicsPixmapItem *pix = dynamic_cast<QGraphicsPixmapItem*>(m_imageItem);
    if (pix) {
        pix->setTransformationMode(
                    transform().m11() < 1 ? Qt::SmoothTransformation : Qt::FastTransformation);
    }
}

void ImageEditorWidget::wheelEvent(QWheelEvent *event)
{
    qreal factor = pow(m_scaleFactor, event->delta() / 240.0);
    doScale(factor);
    event->accept();
}

void ImageEditorWidget::zoomIn()
{
    doScale(m_scaleFactor);
}

void ImageEditorWidget::zoomOut()
{
    doScale(1.0 / m_scaleFactor);
}
