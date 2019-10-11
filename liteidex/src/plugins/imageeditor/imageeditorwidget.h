#ifndef IMAGEEDITORWIDGET_H
#define IMAGEEDITORWIDGET_H

#include <QGraphicsView>

class ImageEditorWidget : public QGraphicsView
{
public:
    ImageEditorWidget();
    virtual ~ImageEditorWidget();
    void setImageItem(QGraphicsItem *item);
    void doScale(qreal factor);
    void zoomIn();
    void zoomOut();
protected:
    void drawBackground(QPainter *p, const QRectF &);
    void wheelEvent(QWheelEvent *event);
protected:
    QGraphicsItem *m_imageItem;
    double m_scaleFactor;
};

#endif // IMAGEEDITORWIDGET_H
