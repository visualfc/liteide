#include "imageeditor.h"
#include "imageeditorfile.h"
#include "imageeditorwidget.h"
#include <QFileInfo>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QAction>
#include <QLabel>
#include <QDebug>

ImageEditor::ImageEditor(LiteApi::IApplication *app)
    : m_liteApp(app)
{
    m_file = new ImageEditorFile(m_liteApp,this);
    m_imageWidget = new ImageEditorWidget;
    m_widget = new QWidget;
    m_toolBar = new QToolBar;
    m_mvToolBar = new QToolBar;

    m_toolBar->setIconSize(LiteApi::getToolBarIconSize(m_liteApp));
    m_mvToolBar->setIconSize(LiteApi::getToolBarIconSize(m_liteApp));

    QAction *zoomInAct = new QAction(tr("ZoomIn"),this);
    zoomInAct->setIcon(QIcon("icon:/imageeditor/images/zoomin.png"));

    QAction *zoomOutAct = new QAction(tr("ZoomOut"),this);
    zoomOutAct->setIcon(QIcon("icon:/imageeditor/images/zoomout.png"));

    QAction *resetSizeAct = new QAction(tr("Reset to original size"),this);
    resetSizeAct->setIcon(QIcon("icon:/imageeditor/images/resetsize.png"));

    QAction *fitViewAct = new QAction(tr("Fit to view"),this);
    fitViewAct->setIcon(QIcon("icon:/imageeditor/images/fitview.png"));

    QAction *playAct = new QAction(tr("Play movie"),this);
    playAct->setIcon(QIcon("icon:/imageeditor/images/play.png"));
    playAct->setCheckable(true);

    QAction *prevFrameAct = new QAction(tr("Prev frame"),this);
    prevFrameAct->setIcon(QIcon("icon:/imageeditor/images/prevframe.png"));
    connect(prevFrameAct,SIGNAL(triggered()),m_file,SLOT(jumpToPrevFrame()));

    QAction *nextFrameAct = new QAction(tr("Next frame"),this);
    nextFrameAct->setIcon(QIcon("icon:/imageeditor/images/nextframe.png"));
    connect(nextFrameAct,SIGNAL(triggered()),m_file,SLOT(jumpToNextFrame()));

    m_playAct = playAct;

    connect(zoomInAct,SIGNAL(triggered()),m_imageWidget,SLOT(zoomIn()));
    connect(zoomOutAct,SIGNAL(triggered()),m_imageWidget,SLOT(zoomOut()));
    connect(resetSizeAct,SIGNAL(triggered()),m_imageWidget,SLOT(resetSize()));
    connect(fitViewAct,SIGNAL(triggered()),m_imageWidget,SLOT(fitToView()));
    connect(playAct,SIGNAL(toggled(bool)),this,SLOT(toggledPlay(bool)));

    connect(m_imageWidget,SIGNAL(scaleFactorChanged(qreal)),this,SLOT(scaleFactorChanged(qreal)));
    connect(m_file,SIGNAL(frameChanged(int)),this,SLOT(frameChanged(int)));

    m_imageInfo = new QLabel;
    m_imageInfo->setText("32x32");

    m_scaleInfo = new QLabel;
    m_scaleInfo->setText("100.00%");

    m_frameLabel = new QLabel;
    m_frameLabel->setText("");

    m_toolBar->addAction(zoomInAct);
    m_toolBar->addAction(zoomOutAct);
    m_toolBar->addAction(resetSizeAct);
    m_toolBar->addAction(fitViewAct);
    m_toolBar->addSeparator();
    m_toolBar->addWidget(m_imageInfo);
    m_toolBar->addSeparator();
    m_toolBar->addWidget(m_scaleInfo);
    m_toolBar->addSeparator();

    m_mvToolBar->addAction(playAct);
    m_mvToolBar->addSeparator();
    m_mvToolBar->addAction(prevFrameAct);
    m_mvToolBar->addAction(nextFrameAct);
    m_mvToolBar->addSeparator();
    m_mvToolBar->addWidget(m_frameLabel);
    m_mvToolBar->addSeparator();

    QHBoxLayout *toolLayout = new QHBoxLayout;
    toolLayout->setMargin(0);
    toolLayout->setSpacing(0);
    toolLayout->addWidget(m_toolBar);
    toolLayout->addWidget(m_mvToolBar);
    toolLayout->addSpacing(0);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addLayout(toolLayout);
    layout->addWidget(m_imageWidget);
    m_widget->setLayout(layout);
}

ImageEditor::~ImageEditor()
{
    delete m_widget;
    delete m_file;
}

QWidget *ImageEditor::widget()
{
    return  m_widget;
}

QString ImageEditor::name() const
{
    return  QFileInfo(m_file->filePath()).fileName();
}

bool ImageEditor::open(const QString &filePath, const QString &mimeType)
{
    bool b = m_file->open(filePath,mimeType);
    m_imageWidget->setImageItem(m_file->graphicsItem());
    QSize sz = m_file->imageSize();
    m_imageInfo->setText(QString("%1x%2").arg(sz.width()).arg(sz.height()));
    m_mvToolBar->setVisible(m_file->isMovie());
    return  b;
}

bool ImageEditor::reload()
{
    if (!m_file->isValid()) {
        return  false;
    }
    QString filePath = m_file->filePath();
    QString mimeType = m_file->mimeType();
    m_file->clear();
    bool b = m_file->open(filePath,mimeType);
    m_imageWidget->setImageItem(m_file->graphicsItem());
    QSize sz = m_file->imageSize();
    m_imageInfo->setText(QString("%1x%2").arg(sz.width()).arg(sz.height()));
    m_mvToolBar->setVisible(m_file->isMovie());
    return b;
}

bool ImageEditor::save()
{
    return  false;
}

bool ImageEditor::saveAs(const QString &filePath)
{
    return  false;
}

void ImageEditor::setReadOnly(bool b)
{

}

bool ImageEditor::isReadOnly() const
{
    return  true;
}

bool ImageEditor::isModified() const
{
    return  false;
}

QString ImageEditor::filePath() const
{
    return  m_file->filePath();
}

QString ImageEditor::mimeType() const
{
    return  m_file->mimeType();
}

QByteArray ImageEditor::saveState() const
{
    return  QByteArray();
}

bool ImageEditor::restoreState(const QByteArray &array)
{
    return false;
}

void ImageEditor::onActive()
{

}

void ImageEditor::scaleFactorChanged(qreal factor)
{
    QString info = QString::number(factor * 100, 'f', 2) + QLatin1Char('%');
    m_scaleInfo->setText(info);
}

void ImageEditor::toggledPlay(bool checked)
{
    if (checked) {
        m_playAct->setText(tr("Pause movie"));
        m_playAct->setIcon(QIcon("icon:/imageeditor/images/pause.png"));
    } else {
        m_playAct->setText(tr("Play movie"));
        m_playAct->setIcon(QIcon("icon:/imageeditor/images/play.png"));
    }
    m_file->setPaused(!checked);
}

void ImageEditor::frameChanged(int frameNumber)
{
    m_frameLabel->setText(QString("%1/%2").arg(frameNumber+1).arg(m_file->frameCount()));
}
