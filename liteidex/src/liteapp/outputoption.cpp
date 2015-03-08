/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2015 LiteIDE Team. All rights reserved.
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
// Module: outputoption.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "outputoption.h"
#include "ui_outputoption.h"
#include "liteapp_global.h"
#include <QFontDatabase>
#include <QStandardItemModel>
#include <QStandardItem>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

OutputOption::OutputOption(LiteApi::IApplication *app,QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::OutputOption)
{
    ui->setupUi(m_widget);

    QFontDatabase db;
    const QStringList families = db.families();
    ui->familyComboBox->addItems(families);

#if defined(Q_OS_WIN)
    m_fontFamily = m_liteApp->settings()->value(OUTPUT_FAMILY,"Courier").toString();
#elif defined(Q_OS_LINUX)
    m_fontFamily = m_liteApp->settings()->value(OUTPUT_FAMILY,"Monospace").toString();
#elif defined(Q_OS_MAC)
    m_fontFamily = m_liteApp->settings()->value(OUTPUT_FAMILY,"Menlo").toString();
#else
    m_fontFamily = m_liteApp->settings()->value(OUTPUT_FAMILY,"Monospace").toString();
#endif
    m_fontSize = m_liteApp->settings()->value(OUTPUT_FONTSIZE,12).toInt();

    int fontZoom = m_liteApp->settings()->value(OUTPUT_FONTZOOM,100).toInt();

    bool antialias = m_liteApp->settings()->value(OUTPUT_ANTIALIAS,true).toBool();
    ui->antialiasCheckBox->setChecked(antialias);

    const int idx = families.indexOf(m_fontFamily);
    ui->familyComboBox->setCurrentIndex(idx);

    updatePointSizes();

    ui->fontZoomSpinBox->setValue(fontZoom);

    bool useColorScheme = m_liteApp->settings()->value(OUTPUT_USECOLORSCHEME,true).toBool();
    ui->useColorSchemeCheckBox->setChecked(useColorScheme);

    bool wrap = m_liteApp->settings()->value(OUTPUT_LINEWRAP,false).toBool();
    ui->lineWrapCheckBox->setChecked(wrap);
}

OutputOption::~OutputOption()
{
    delete m_widget;
    delete ui;
}

QWidget *OutputOption::widget()
{
    return m_widget;
}

QString OutputOption::name() const
{
    return "Output";
}

QString OutputOption::mimeType() const
{
    return OPTION_OUTPUT;
}

void OutputOption::apply()
{
    m_fontFamily = ui->familyComboBox->currentText();
    if (ui->sizeComboBox->count()) {
        const QString curSize = ui->sizeComboBox->currentText();
        bool ok = true;
        int size = curSize.toInt(&ok);
        if (ok) {
            m_fontSize = size;
        }
    }

    int fontZoom = ui->fontZoomSpinBox->value();
    if (fontZoom <= 10) {
        fontZoom = 10;
    }
    m_liteApp->settings()->setValue(OUTPUT_FAMILY,m_fontFamily);
    m_liteApp->settings()->setValue(OUTPUT_FONTSIZE,m_fontSize);
    m_liteApp->settings()->setValue(OUTPUT_FONTZOOM,fontZoom);

    bool colorScheme = ui->useColorSchemeCheckBox->isChecked();
    bool lineWrap = ui->lineWrapCheckBox->isChecked();

    m_liteApp->settings()->setValue(OUTPUT_USECOLORSCHEME,colorScheme);
    m_liteApp->settings()->setValue(OUTPUT_LINEWRAP,lineWrap);
}

void OutputOption::updatePointSizes()
{
    // Update point sizes
    const int oldSize = m_fontSize;
    if (ui->sizeComboBox->count()) {
        ui->sizeComboBox->clear();
    }
    const QList<int> sizeLst = pointSizesForSelectedFont();
    int idx = 0;
    int i = 0;
    for (; i < sizeLst.count(); ++i) {
        if (idx == 0 && sizeLst.at(i) >= oldSize)
            idx = i;
        ui->sizeComboBox->addItem(QString::number(sizeLst.at(i)));
    }
    if (ui->sizeComboBox->count())
        ui->sizeComboBox->setCurrentIndex(idx);
}

QList<int> OutputOption::pointSizesForSelectedFont() const
{
    QFontDatabase db;
    const QString familyName = ui->familyComboBox->currentText();
    QList<int> sizeLst = db.pointSizes(familyName);
    if (!sizeLst.isEmpty())
        return sizeLst;

    QStringList styles = db.styles(familyName);
    if (!styles.isEmpty())
        sizeLst = db.pointSizes(familyName, styles.first());
    if (sizeLst.isEmpty())
        sizeLst = QFontDatabase::standardSizes();

    return sizeLst;
}

