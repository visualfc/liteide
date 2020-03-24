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
// Module: imageeditor.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "imageeditor.h"
#include "imageeditorfile.h"
#include "imageeditorwidget.h"
#include <QFileInfo>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QAction>
#include <QLabel>
#include <QDebug>

#define EDITOR_TOOLBAR_VISIBLE "editor/toolbar_visible"
#define EDITOR_NAVBAR_VISIBLE "editor/navbar_visible"

ImageEditor::ImageEditor(LiteApi::IApplication *app)
    : m_liteApp(app)
{
    m_file = new ImageEditorFile(m_liteApp,this);
    m_imageWidget = new ImageEditorWidget;
    m_widget = new QWidget;
    m_toolBar = new QToolBar;
    m_mvToolBar = new QToolBar;
    m_navBar = new NavigateBar(app,this);
    m_navBar->createToolBar("editor.nav",m_widget);
    m_navBar->toolBar()->setVisible(m_liteApp->settings()->value(EDITOR_NAVBAR_VISIBLE,true).toBool());

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
    toolLayout->addWidget(m_navBar->toolBar());
    toolLayout->addSpacing(0);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addLayout(toolLayout);
    layout->addWidget(m_imageWidget);
    m_widget->setLayout(layout);

    connect(m_liteApp,SIGNAL(broadcast(QString,QString,QVariant)),this,SLOT(broadcast(QString,QString,QVariant)));
}

ImageEditor::~ImageEditor()
{
    delete m_widget;
    delete m_file;
}

void ImageEditor::broadcast(const QString &module, const QString &id, const QVariant &param)
{
    if (module == "liteeditor" && id == "font" && param != this->filePath()) {
    } else if (module == "liteeditor" && id == EDITOR_NAVBAR_VISIBLE) {
        m_navBar->toolBar()->setVisible(param.toBool());
    } else if (module == "liteeditor" && id == EDITOR_TOOLBAR_VISIBLE) {
    }
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
    m_navBar->LoadPath(filePath);
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
