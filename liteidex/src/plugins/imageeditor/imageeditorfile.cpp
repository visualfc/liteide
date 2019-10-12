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
        m_movie->start();
        //m_isPaused = false;
        //setPaused(true);
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
