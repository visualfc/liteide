/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2012 LiteIDE Team. All rights reserved.
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
// Module: liteeditoroption.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "liteeditoroption.h"
#include "ui_liteeditoroption.h"
#include "liteeditor_global.h"
#include <QFontDatabase>
#include <QDir>
#include <QFileInfo>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


LiteEditorOption::LiteEditorOption(LiteApi::IApplication *app,QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::LiteEditorOption)
{
    ui->setupUi(m_widget);

    connect(ui->editPushButton,SIGNAL(clicked()),this,SLOT(editStyleFile()));
    connect(ui->rightLineVisibleCheckBox,SIGNAL(toggled(bool)),ui->rightLineWidthSpinBox,SLOT(setEnabled(bool)));    
}

QWidget *LiteEditorOption::widget()
{
    return m_widget;
}

QString LiteEditorOption::name() const
{
    return "LiteEditor";
}

QString LiteEditorOption::mimeType() const
{
    return OPTION_LITEEDITOR;
}

void LiteEditorOption::apply()
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
    m_liteApp->settings()->setValue(EDITOR_FONTZOOM,fontZoom);

    QString style = ui->styleComboBox->currentText();
    bool noprintCheck = ui->noprintCheckBox->isChecked();
    bool autoIndent = ui->autoIndentCheckBox->isChecked();
    bool autoBraces0 = ui->autoBraces0CheckBox->isChecked();
    bool autoBraces1 = ui->autoBraces1CheckBox->isChecked();
    bool autoBraces2 = ui->autoBraces2CheckBox->isChecked();
    bool autoBraces3 = ui->autoBraces3CheckBox->isChecked();
    bool autoBraces4 = ui->autoBraces4CheckBox->isChecked();
    bool lineNumberVisible = ui->lineNumberVisibleCheckBox->isChecked();
    bool caseSensitive = ui->completerCaseSensitiveCheckBox->isChecked();
    bool antialias = ui->antialiasCheckBox->isChecked();
    bool rightLineVisible = ui->rightLineVisibleCheckBox->isChecked();
    int rightLineWidth = ui->rightLineWidthSpinBox->value();
    int min = ui->preMinLineEdit->text().toInt();
    if (min < 0 || min > 10) {
        min = 1;
    }

    m_liteApp->settings()->setValue(EDITOR_NOPRINTCHECK,noprintCheck);
    m_liteApp->settings()->setValue(EDITOR_FAMILY,m_fontFamily);
    m_liteApp->settings()->setValue(EDITOR_FONTSIZE,m_fontSize);
    m_liteApp->settings()->setValue(EDITOR_ANTIALIAS,antialias);
    m_liteApp->settings()->setValue(EDITOR_STYLE,style);
    m_liteApp->settings()->setValue(EDITOR_AUTOINDENT,autoIndent);
    m_liteApp->settings()->setValue(EDITOR_AUTOBRACE0,autoBraces0);
    m_liteApp->settings()->setValue(EDITOR_AUTOBRACE1,autoBraces1);
    m_liteApp->settings()->setValue(EDITOR_AUTOBRACE2,autoBraces2);
    m_liteApp->settings()->setValue(EDITOR_AUTOBRACE3,autoBraces3);
    m_liteApp->settings()->setValue(EDITOR_AUTOBRACE4,autoBraces4);
    m_liteApp->settings()->setValue(EDITOR_LINENUMBERVISIBLE,lineNumberVisible);
    m_liteApp->settings()->setValue(EDITOR_COMPLETER_CASESENSITIVE,caseSensitive);
    m_liteApp->settings()->setValue(EDITOR_PREFIXLENGTH,min);
    m_liteApp->settings()->setValue(EDITOR_RIGHTLINEVISIBLE,rightLineVisible);
    if (rightLineVisible) {
        m_liteApp->settings()->setValue(EDITOR_RIGHTLINEWIDTH,rightLineWidth);
    }
    int tab = ui->tabWidthLineEdit->text().toInt();
    if (tab < 0 || tab > 10) {
        tab = 4;
    }
    m_liteApp->settings()->setValue(EDITOR_TABWIDTH,tab);
}

void LiteEditorOption::active()
{
    QFontDatabase db;
    const QStringList families = db.families();
    ui->familyComboBox->addItems(families);

    m_fontFamily = m_liteApp->settings()->value(EDITOR_FAMILY,"Courier").toString();
    m_fontSize = m_liteApp->settings()->value(EDITOR_FONTSIZE,12).toInt();

    int fontZoom = m_liteApp->settings()->value(EDITOR_FONTZOOM,100).toInt();

    bool antialias = m_liteApp->settings()->value(EDITOR_ANTIALIAS,true).toBool();
    ui->antialiasCheckBox->setChecked(antialias);

    const int idx = families.indexOf(m_fontFamily);
    ui->familyComboBox->setCurrentIndex(idx);

    updatePointSizes();

    ui->fontZoomSpinBox->setValue(fontZoom);

    QString styleName = m_liteApp->settings()->value(EDITOR_STYLE,"default.xml").toString();
    QString stylePath = m_liteApp->resourcePath()+"/liteeditor/color";
    QDir dir(stylePath);
    int index = -1;
    foreach(QFileInfo info, dir.entryInfoList(QStringList() << "*.xml")) {
        ui->styleComboBox->addItem(info.fileName());
        if (info.fileName() == styleName) {
            index = ui->styleComboBox->count()-1;
        }
    }
    if (index >= 0 && index < ui->styleComboBox->count()) {
        ui->styleComboBox->setCurrentIndex(index);
    }
    bool noprintCheck = m_liteApp->settings()->value(EDITOR_NOPRINTCHECK,true).toBool();
    bool autoIndent = m_liteApp->settings()->value(EDITOR_AUTOINDENT,true).toBool();
    bool autoBraces0 = m_liteApp->settings()->value(EDITOR_AUTOBRACE0,true).toBool();
    bool autoBraces1 = m_liteApp->settings()->value(EDITOR_AUTOBRACE1,true).toBool();
    bool autoBraces2 = m_liteApp->settings()->value(EDITOR_AUTOBRACE2,true).toBool();
    bool autoBraces3 = m_liteApp->settings()->value(EDITOR_AUTOBRACE3,true).toBool();
    bool autoBraces4 = m_liteApp->settings()->value(EDITOR_AUTOBRACE4,true).toBool();
    bool caseSensitive = m_liteApp->settings()->value(EDITOR_COMPLETER_CASESENSITIVE,true).toBool();
    bool lineNumberVisible = m_liteApp->settings()->value(EDITOR_LINENUMBERVISIBLE,true).toBool();
    bool rightLineVisible = m_liteApp->settings()->value(EDITOR_RIGHTLINEVISIBLE,true).toBool();
    int rightLineWidth = m_liteApp->settings()->value(EDITOR_RIGHTLINEWIDTH,80).toInt();

    int min = m_liteApp->settings()->value(EDITOR_PREFIXLENGTH,1).toInt();
    int tab = m_liteApp->settings()->value(EDITOR_TABWIDTH,4).toInt();

    ui->noprintCheckBox->setChecked(noprintCheck);;
    ui->autoIndentCheckBox->setChecked(autoIndent);
    ui->autoBraces0CheckBox->setChecked(autoBraces0);
    ui->autoBraces1CheckBox->setChecked(autoBraces1);
    ui->autoBraces2CheckBox->setChecked(autoBraces2);
    ui->autoBraces3CheckBox->setChecked(autoBraces3);
    ui->autoBraces4CheckBox->setChecked(autoBraces4);
    ui->lineNumberVisibleCheckBox->setChecked(lineNumberVisible);
    ui->completerCaseSensitiveCheckBox->setChecked(caseSensitive);
    ui->preMinLineEdit->setText(QString("%1").arg(min));
    ui->rightLineVisibleCheckBox->setChecked(rightLineVisible);
    ui->rightLineWidthSpinBox->setValue(rightLineWidth);
    ui->tabWidthLineEdit->setText(QString("%1").arg(tab));
}

LiteEditorOption::~LiteEditorOption()
{
    delete m_widget;
    delete ui;
}

void LiteEditorOption::updatePointSizes()
{
    // Update point sizes
    const int oldSize = m_fontSize;
    if (ui->sizeComboBox->count()) {
        const QString curSize = ui->sizeComboBox->currentText();
        bool ok = true;
        int oldSize = curSize.toInt(&ok);
        if (!ok)
            oldSize = m_fontSize;
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

QList<int> LiteEditorOption::pointSizesForSelectedFont() const
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

void LiteEditorOption::editStyleFile()
{
    QString fileName = ui->styleComboBox->currentText();
    if (fileName.isEmpty()) {
        return;
    }
    QString filePath = m_liteApp->resourcePath()+"/liteeditor/color/"+fileName;
    m_liteApp->fileManager()->openEditor(filePath);
}
