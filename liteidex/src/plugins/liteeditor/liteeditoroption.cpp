/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011 LiteIDE Team. All rights reserved.
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
// date: 2011-3-26
// $Id: liteeditoroption.cpp,v 1.0 2011-7-26 visualfc Exp $

#include "liteeditoroption.h"
#include "ui_liteeditoroption.h"
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
    QFontDatabase db;
    const QStringList families = db.families();
    ui->familyComboBox->addItems(families);

    m_fontFamily = m_liteApp->settings()->value("editor/family","Courier").toString();
    m_fontSize = m_liteApp->settings()->value("editor/fontsize",12).toInt();

    bool antialias = m_liteApp->settings()->value("editor/antialias",true).toBool();
    ui->antialiasCheckBox->setChecked(antialias);

    const int idx = families.indexOf(m_fontFamily);
    ui->familyComboBox->setCurrentIndex(idx);

    updatePointSizes();

    QString styleName = m_liteApp->settings()->value("editor/style","default.xml").toString();
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
    bool noprintCheck = m_liteApp->settings()->value("editor/noprintcheck",true).toBool();
    bool autoIndent = m_liteApp->settings()->value("editor/autoindent",true).toBool();
    bool autoBraces0 = m_liteApp->settings()->value("editor/autobraces0",true).toBool();
    bool autoBraces1 = m_liteApp->settings()->value("editor/autobraces1",true).toBool();
    bool autoBraces2 = m_liteApp->settings()->value("editor/autobraces2",true).toBool();
    bool autoBraces3 = m_liteApp->settings()->value("editor/autobraces3",true).toBool();
    bool autoBraces4 = m_liteApp->settings()->value("editor/autobraces4",true).toBool();
    bool caseSensitive = m_liteApp->settings()->value("editor/ComplererCaseSensitive",true).toBool();
    bool lineNumberVisible = m_liteApp->settings()->value("editor/linenumbervisible",true).toBool();

    int min = m_liteApp->settings()->value("editor/prefixlength",1).toInt();

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

    connect(ui->editPushButton,SIGNAL(clicked()),this,SLOT(editStyleFile()));
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
    return "option/liteeditor";
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
    int min = ui->preMinLineEdit->text().toInt();
    if (min < 0 || min > 10) {
        min = 1;
    }

    m_liteApp->settings()->setValue("editor/noprintcheck",noprintCheck);
    m_liteApp->settings()->setValue("editor/family",m_fontFamily);
    m_liteApp->settings()->setValue("editor/fontsize",m_fontSize);
    m_liteApp->settings()->setValue("editor/antialias",antialias);
    m_liteApp->settings()->setValue("editor/style",style);
    m_liteApp->settings()->setValue("editor/autoindent",autoIndent);
    m_liteApp->settings()->setValue("editor/autobraces0",autoBraces0);
    m_liteApp->settings()->setValue("editor/autobraces1",autoBraces1);
    m_liteApp->settings()->setValue("editor/autobraces2",autoBraces2);
    m_liteApp->settings()->setValue("editor/autobraces3",autoBraces3);
    m_liteApp->settings()->setValue("editor/autobraces4",autoBraces4);
    m_liteApp->settings()->setValue("editor/linenumbervisible",lineNumberVisible);
    m_liteApp->settings()->setValue("editor/ComplererCaseSensitive",caseSensitive);
    m_liteApp->settings()->setValue("editor/prefixlength",min);
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
