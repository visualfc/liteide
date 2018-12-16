/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2016 LiteIDE Team. All rights reserved.
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
#include "liteapi/liteutil.h"
#include <QFontDatabase>
#include <QDir>
#include <QFileInfo>
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

static QString defaultFontFamily()
{
#if defined(Q_OS_WIN)
    return "Courier";
#elif defined(Q_OS_LINUX)
    return "Monospace";
#elif defined(Q_OS_MAC)
    return "Menlo";
#else
    return "Monospace";
#endif
}

LiteEditorOption::LiteEditorOption(LiteApi::IApplication *app,QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::LiteEditorOption)
{
    ui->setupUi(m_widget);

//    QFontDatabase db;
//    const QStringList families = db.families();

    m_fontFamily = m_liteApp->settings()->value(EDITOR_FAMILY,defaultFontFamily()).toString();

    ui->fontComboBox->setCurrentFont(QFont(m_fontFamily));

    m_fontSize = m_liteApp->settings()->value(EDITOR_FONTSIZE,12).toInt();

    int fontZoom = m_liteApp->settings()->value(EDITOR_FONTZOOM,100).toInt();

    bool antialias = m_liteApp->settings()->value(EDITOR_ANTIALIAS,true).toBool();
    ui->antialiasCheckBox->setChecked(antialias);

    updatePointSizes();

    ui->fontZoomSpinBox->setValue(fontZoom);

    QString styleName = m_liteApp->settings()->value(EDITOR_STYLE,"default.xml").toString();
    QString stylePath = m_liteApp->resourcePath()+"/liteeditor/color";
    QDir dir(stylePath);
    int index = -1;
    if (!QFileInfo(stylePath,styleName).exists()) {
        styleName = "default.xml";
    }
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
    bool autoBraces5 = m_liteApp->settings()->value(EDITOR_AUTOBRACE5,true).toBool();
    bool caseSensitive = m_liteApp->settings()->value(EDITOR_COMPLETER_CASESENSITIVE,false).toBool();
    bool fuzzyCompleter = m_liteApp->settings()->value(EDITOR_COMPLETER_FUZZY,true).toBool();
    bool cleanWhitespaceOnSave = m_liteApp->settings()->value(EDITOR_CLEANWHITESPACEONSAVE,false).toBool();
    bool lineNumberVisible = m_liteApp->settings()->value(EDITOR_LINENUMBERVISIBLE,true).toBool();
    bool codeFoldVisible = m_liteApp->settings()->value(EDITOR_CODEFOLDVISIBLE,true).toBool();
    bool rightLineVisible = m_liteApp->settings()->value(EDITOR_RIGHTLINEVISIBLE,true).toBool();
    bool eofVisible = m_liteApp->settings()->value(EDITOR_EOFVISIBLE,false).toBool();
    bool defaultWordWrap = m_liteApp->settings()->value(EDITOR_DEFAULTWORDWRAP,false).toBool();
    bool indentLineVisible = m_liteApp->settings()->value(EDITOR_INDENTLINEVISIBLE,true).toBool();
    bool wheelZoom = m_liteApp->settings()->value(EDITOR_WHEEL_SCROLL,true).toBool();
    bool offsetVisible = m_liteApp->settings()->value(EDITOR_OFFSETVISIBLE,false).toBool();
    bool visualizeWhitespace = m_liteApp->settings()->value(EDITOR_VISUALIZEWHITESPACE,false).toBool();
    int rightLineWidth = m_liteApp->settings()->value(EDITOR_RIGHTLINEWIDTH,80).toInt();
    bool cleanComplerCache = m_liteApp->settings()->value(EDITOR_CLEAN_COMPLETERCACHE_SAVE,false).toBool();
    bool copyToHtml = m_liteApp->settings()->value(EDITOR_ENABLE_COPYTOHTML,true).toBool();

    int min = m_liteApp->settings()->value(EDITOR_PREFIXLENGTH,1).toInt();

    ui->noprintCheckBox->setChecked(noprintCheck);;
    ui->autoIndentCheckBox->setChecked(autoIndent);
    ui->autoBraces0CheckBox->setChecked(autoBraces0);
    ui->autoBraces1CheckBox->setChecked(autoBraces1);
    ui->autoBraces2CheckBox->setChecked(autoBraces2);
    ui->autoBraces3CheckBox->setChecked(autoBraces3);
    ui->autoBraces4CheckBox->setChecked(autoBraces4);
    ui->autoBraces5CheckBox->setChecked(autoBraces5);
    ui->lineNumberVisibleCheckBox->setChecked(lineNumberVisible);
    ui->visualizeWhitespaceCheckBox->setChecked(visualizeWhitespace);
    ui->codeFoldVisibleCheckBox->setChecked(codeFoldVisible);
    ui->completerCaseSensitiveCheckBox->setChecked(caseSensitive);
    ui->completerFuzzyCheckBox->setChecked(fuzzyCompleter);
    ui->preMinLineEdit->setText(QString("%1").arg(min));
    ui->cleanWhitespaceOnSaveCheckBox->setChecked(cleanWhitespaceOnSave);
    ui->rightLineVisibleCheckBox->setChecked(rightLineVisible);
    ui->rightLineWidthSpinBox->setValue(rightLineWidth);
    ui->eofVisibleCheckBox->setChecked(eofVisible);
    ui->defaultWordWrapCheckBox->setChecked(defaultWordWrap);
    ui->indentLineCheckBox->setChecked(indentLineVisible);
    ui->wheelZoomingCheckBox->setChecked(wheelZoom);
    ui->offsetCheckBox->setChecked(offsetVisible);
    ui->cleanCompleterCacheCheckBox->setChecked(cleanComplerCache);
    ui->enableCopyToHtmlCheckBox->setChecked(copyToHtml);

    connect(ui->editPushButton,SIGNAL(clicked()),this,SLOT(editStyleFile()));
    connect(ui->rightLineVisibleCheckBox,SIGNAL(toggled(bool)),ui->rightLineWidthSpinBox,SLOT(setEnabled(bool)));
    connect(ui->restoreDefaultFontButton,SIGNAL(clicked()),this,SLOT(restoreDefaultFont()));
    connect(ui->monospaceFontCheckBox,SIGNAL(toggled(bool)),this,SLOT(filterMonospaceFont(bool)));

    m_mimeModel = new QStandardItemModel(0,5,this);
    m_mimeModel->setHeaderData(0,Qt::Horizontal,tr("MIME Type"));
    m_mimeModel->setHeaderData(1,Qt::Horizontal,tr("Tab Width"));
    m_mimeModel->setHeaderData(2,Qt::Horizontal,tr("Tab To Spaces"));
    m_mimeModel->setHeaderData(3,Qt::Horizontal,tr("Custom Extensions"));
    m_mimeModel->setHeaderData(4,Qt::Horizontal,tr("File Extensions"));
    connect(m_mimeModel,SIGNAL(itemChanged(QStandardItem*)),this,SLOT(mimeItemChanged(QStandardItem*)));

    ui->mimeTreeView->setModel(m_mimeModel);
    ui->mimeTreeView->setRootIsDecorated(false);

    QStringList mimeTypes = m_liteApp->editorManager()->mimeTypeList();
    qStableSort(mimeTypes);
    foreach(QString mime, mimeTypes) {
        if (mime.startsWith("text/") || mime.startsWith("application/")) {
            QStandardItem *item = new QStandardItem(mime);
            item->setEditable(false);

            bool tabToSpace = true;
            int tabWidth = 4;
            LiteApi::IMimeType *im = m_liteApp->mimeTypeManager()->findMimeType(mime);
            if (im) {
                tabToSpace = im->tabToSpace();
                tabWidth = im->tabWidth();
            }

            tabToSpace = m_liteApp->settings()->value(MIMETYPE_TABTOSPACE+mime,tabToSpace).toBool();
            tabWidth = m_liteApp->settings()->value(MIMETYPE_TABWIDTH+mime,tabWidth).toInt();

            QStandardItem *tab = new QStandardItem(QString("%1").arg(tabWidth));
            QStandardItem *useSpace = new QStandardItem();
            useSpace->setCheckable(true);
            useSpace->setCheckState(tabToSpace?Qt::Checked:Qt::Unchecked);
            useSpace->setEditable(false);
            QStandardItem *ext = new QStandardItem;
            ext->setEditable(false);
            if (im) {
                ext->setText(im->globPatterns().join(";"));
            }
            QString custom = m_liteApp->settings()->value(EDITOR_CUSTOMEXTENSION+mime,"").toString();
            QStandardItem *cus = new QStandardItem(custom);
            m_mimeModel->appendRow(QList<QStandardItem*>()
                                  << item
                                  << tab
                                  << useSpace
                                  << cus
                                  << ext);
        }
    }

#if QT_VERSION >= 0x050000
    ui->mimeTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    ui->mimeTreeView->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif

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
    m_fontFamily = ui->fontComboBox->currentFont().family();
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
    if (style != m_liteApp->settings()->value(EDITOR_STYLE,"default.xml").toString()) {
        m_liteApp->settings()->setValue(EDITOR_STYLE,style);
        QString styleFile = m_liteApp->resourcePath()+"/liteeditor/color/"+style;
        m_liteApp->editorManager()->loadColorStyleScheme(styleFile);
    }

    bool noprintCheck = ui->noprintCheckBox->isChecked();
    bool autoIndent = ui->autoIndentCheckBox->isChecked();
    bool autoBraces0 = ui->autoBraces0CheckBox->isChecked();
    bool autoBraces1 = ui->autoBraces1CheckBox->isChecked();
    bool autoBraces2 = ui->autoBraces2CheckBox->isChecked();
    bool autoBraces3 = ui->autoBraces3CheckBox->isChecked();
    bool autoBraces4 = ui->autoBraces4CheckBox->isChecked();
    bool autoBraces5 = ui->autoBraces5CheckBox->isChecked();
    bool lineNumberVisible = ui->lineNumberVisibleCheckBox->isChecked();
    bool codeFoldVisible = ui->codeFoldVisibleCheckBox->isChecked();
    bool caseSensitive = ui->completerCaseSensitiveCheckBox->isChecked();
    bool fuzzyCompleter = ui->completerFuzzyCheckBox->isChecked();
    bool cleanWhitespaceOnSave = ui->cleanWhitespaceOnSaveCheckBox->isChecked();
    bool antialias = ui->antialiasCheckBox->isChecked();
    bool rightLineVisible = ui->rightLineVisibleCheckBox->isChecked();
    bool eofVisible = ui->eofVisibleCheckBox->isChecked();
    bool defaultWordWrap = ui->defaultWordWrapCheckBox->isChecked();
    bool indentLineVisible = ui->indentLineCheckBox->isChecked();
    bool wheelZoom = ui->wheelZoomingCheckBox->isChecked();
    bool visualizeWhitespace = ui->visualizeWhitespaceCheckBox->isChecked();
    int rightLineWidth = ui->rightLineWidthSpinBox->value();
    bool cleanCompleterCache = ui->cleanCompleterCacheCheckBox->isChecked();
    bool copyToHtml = ui->enableCopyToHtmlCheckBox->isChecked();
    int min = ui->preMinLineEdit->text().toInt();
    if (min < 0 || min > 10) {
        min = 1;
    }
    bool offsetVisible = ui->offsetCheckBox->isChecked();

    m_liteApp->settings()->setValue(EDITOR_NOPRINTCHECK,noprintCheck);
    m_liteApp->settings()->setValue(EDITOR_FAMILY,m_fontFamily);
    m_liteApp->settings()->setValue(EDITOR_FONTSIZE,m_fontSize);
    m_liteApp->settings()->setValue(EDITOR_ANTIALIAS,antialias);
    m_liteApp->settings()->setValue(EDITOR_AUTOINDENT,autoIndent);
    m_liteApp->settings()->setValue(EDITOR_AUTOBRACE0,autoBraces0);
    m_liteApp->settings()->setValue(EDITOR_AUTOBRACE1,autoBraces1);
    m_liteApp->settings()->setValue(EDITOR_AUTOBRACE2,autoBraces2);
    m_liteApp->settings()->setValue(EDITOR_AUTOBRACE3,autoBraces3);
    m_liteApp->settings()->setValue(EDITOR_AUTOBRACE4,autoBraces4);
    m_liteApp->settings()->setValue(EDITOR_AUTOBRACE5,autoBraces5);
    m_liteApp->settings()->setValue(EDITOR_LINENUMBERVISIBLE,lineNumberVisible);
    m_liteApp->settings()->setValue(EDITOR_CODEFOLDVISIBLE,codeFoldVisible);
    m_liteApp->settings()->setValue(EDITOR_EOFVISIBLE,eofVisible);
    m_liteApp->settings()->setValue(EDITOR_DEFAULTWORDWRAP,defaultWordWrap);
    m_liteApp->settings()->setValue(EDITOR_INDENTLINEVISIBLE,indentLineVisible);
    m_liteApp->settings()->setValue(EDITOR_COMPLETER_CASESENSITIVE,caseSensitive);
    m_liteApp->settings()->setValue(EDITOR_COMPLETER_FUZZY,fuzzyCompleter);
    m_liteApp->settings()->setValue(EDITOR_PREFIXLENGTH,min);
    m_liteApp->settings()->setValue(EDITOR_CLEANWHITESPACEONSAVE,cleanWhitespaceOnSave);
    m_liteApp->settings()->setValue(EDITOR_RIGHTLINEVISIBLE,rightLineVisible);
    m_liteApp->settings()->setValue(EDITOR_WHEEL_SCROLL,wheelZoom);
    m_liteApp->settings()->setValue(EDITOR_OFFSETVISIBLE,offsetVisible);
    m_liteApp->settings()->setValue(EDITOR_VISUALIZEWHITESPACE,visualizeWhitespace);
    m_liteApp->settings()->setValue(EDITOR_CLEAN_COMPLETERCACHE_SAVE,cleanCompleterCache);
    m_liteApp->settings()->setValue(EDITOR_ENABLE_COPYTOHTML,copyToHtml);
    if (rightLineVisible) {
        m_liteApp->settings()->setValue(EDITOR_RIGHTLINEWIDTH,rightLineWidth);
    }
    for (int i = 0; i < m_mimeModel->rowCount(); i++) {
        QString mime = m_mimeModel->item(i,0)->text();
        QString tab = m_mimeModel->item(i,1)->text();
        QString custom = m_mimeModel->item(i,3)->text();
        bool ok;
        int n = tab.toInt(&ok);
        if (ok && n > 0 && n < 20) {
            //m_liteApp->settings()->setValue(MIMETYPE_TABWIDTH+mime,n);
            LiteApi::updateAppSetting(m_liteApp,MIMETYPE_TABWIDTH+mime,n,4);
        }
        bool b = m_mimeModel->item(i,2)->checkState() == Qt::Checked;        
        //m_liteApp->settings()->setValue(MIMETYPE_TABTOSPACE+mime,b);
        LiteApi::updateAppSetting(m_liteApp,MIMETYPE_TABTOSPACE+mime,b,true);
        //m_liteApp->settings()->setValue(EDITOR_CUSTOMEXTENSION+mime,custom);
        LiteApi::updateAppSetting(m_liteApp,EDITOR_CUSTOMEXTENSION+mime,custom,"");
        LiteApi::IMimeType *imt = m_liteApp->mimeTypeManager()->findMimeType(mime);
        if (imt) {
            imt->setCustomPatterns(custom.split(";"));
        }
    }
}

void LiteEditorOption::active()
{
    int fontZoom = m_liteApp->settings()->value(EDITOR_FONTZOOM,100).toInt();
    ui->fontZoomSpinBox->setValue(fontZoom);
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
        //const QString curSize = ui->sizeComboBox->currentText();
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
    static QFontDatabase db;
    const QString familyName = ui->fontComboBox->currentFont().family();
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

void LiteEditorOption::mimeItemChanged(QStandardItem *item)
{
    if (item->column() == 1) {
        bool ok;
        int n = item->text().toInt(&ok);
        if (!ok || n <= 0 || n >= 20) {
            item->setText("4");
        }
    }
}


void LiteEditorOption::restoreDefaultFont()
{
    m_fontFamily = defaultFontFamily();
    ui->fontComboBox->setCurrentFont(QFont(m_fontFamily));

    m_fontSize = 12;

    ui->fontZoomSpinBox->setValue(100);
    ui->antialiasCheckBox->setChecked(true);

    updatePointSizes();
}

void LiteEditorOption::filterMonospaceFont(bool b)
{
    ui->fontComboBox->setFontFilters(b ?QFontComboBox::MonospacedFonts :  QFontComboBox::AllFonts);
    ui->fontComboBox->updateGeometry();
}
