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
// Module: liteappoption.cpp
// Creator: visualfc <visualfc@gmail.com>



#include "liteappoption.h"
#include "liteapp_global.h"
#include "ui_liteappoption.h"
#include <QDir>
#include <QFileInfo>
#include <QLocale>
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

LiteAppOption::LiteAppOption(LiteApi::IApplication *app,QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::LiteAppOption)
{
    ui->setupUi(m_widget);

    const QString &liteideTrPath = m_liteApp->resourcePath()+"/translations";
    QLocale eng(QLocale::English);
    ui->langComboBox->addItem(QLocale::languageToString(QLocale::English),eng.name());
    QDir dir(liteideTrPath);
    if (dir.exists()) {
        foreach (QFileInfo info,dir.entryInfoList(QStringList() << "liteide_*.qm")) {
            QString base = info.baseName();
            QLocale lc(base.right(base.length()-8));
            if (lc.name().isEmpty()) {
                continue;
            }
            QLocale::Language lang = lc.language();
            QString text = QString("%1 (%2)").arg(QLocale::languageToString(lang)).arg(lc.name());
            ui->langComboBox->addItem(text,lc.name());
        }
    }
    QString locale = QLocale::system().name();
    locale = m_liteApp->settings()->value(LITEAPP_LANGUAGE,locale).toString();
    if (!locale.isEmpty()) {
        for (int i = 0; i < ui->langComboBox->count(); i++) {
            if (locale == ui->langComboBox->itemData(i).toString()) {
                ui->langComboBox->setCurrentIndex(i);
                break;
            }
        }
    }
    int max = m_liteApp->settings()->value(LITEAPP_MAXRECENTFILES,16).toInt();
    ui->maxRecentLineEdit->setText(QString("%1").arg(max));
    bool b = m_liteApp->settings()->value(LITEAPP_AUTOCLOSEPROEJCTFILES,true).toBool();
    ui->autoCloseProjecEditorsCheckBox->setChecked(b);
    bool b1 = m_liteApp->settings()->value(LITEAPP_AUTOLOADLASTSESSION,true).toBool();
    ui->autoLoadLastSessionCheckBox->setChecked(b1);
    bool b2 = m_liteApp->settings()->value(LITEAPP_SPLASHVISIBLE,true).toBool();
    ui->splashVisibleCheckBox->setChecked(b2);
    bool b3 = m_liteApp->settings()->value(LITEAPP_WELCOMEPAGEVISIBLE,true).toBool();
    ui->welcomeVisibleCheckBox->setChecked(b3);

    bool b4 = m_liteApp->settings()->value(LITEAPP_EDITTABSCLOSABLE,true).toBool();
    ui->editorTabsClosableCheckBox->setChecked(b4);

    int id = m_liteApp->settings()->value(LITEAPP_TOOLBARICONSIZE,TOOLBAR_ICONSIZE_18).toInt();
    if (id >= 0 && id < ui->buttonGroup->buttons().size()) {
        ui->buttonGroup->buttons().at(id)->setChecked(true);
    }
}

LiteAppOption::~LiteAppOption()
{
    delete m_widget;
    delete ui;
}

QWidget *LiteAppOption::widget()
{
    return m_widget;
}

QString LiteAppOption::name() const
{
    return "LiteApp";
}

QString LiteAppOption::mimeType() const
{
    return OPTION_LITEAPP;
}
void LiteAppOption::apply()
{
    int index = ui->langComboBox->currentIndex();
    if (index < 0) {
        return;
    }
    QString lc = ui->langComboBox->itemData(index).toString();
    m_liteApp->settings()->setValue(LITEAPP_LANGUAGE,lc);
    QString max = ui->maxRecentLineEdit->text();
    m_liteApp->settings()->setValue(LITEAPP_MAXRECENTFILES,max);
    bool b = ui->autoCloseProjecEditorsCheckBox->isChecked();
    m_liteApp->settings()->setValue(LITEAPP_AUTOCLOSEPROEJCTFILES,b);
    bool b1 = ui->autoLoadLastSessionCheckBox->isChecked();
    m_liteApp->settings()->setValue(LITEAPP_AUTOLOADLASTSESSION,b1);
    bool b2 = ui->splashVisibleCheckBox->isChecked();
    m_liteApp->settings()->setValue(LITEAPP_SPLASHVISIBLE,b2);
    bool b3 = ui->welcomeVisibleCheckBox->isChecked();
    m_liteApp->settings()->setValue(LITEAPP_WELCOMEPAGEVISIBLE,b3);
    bool b4 = ui->editorTabsClosableCheckBox->isChecked();
    m_liteApp->settings()->setValue(LITEAPP_EDITTABSCLOSABLE,b4);

    int size = ui->buttonGroup->buttons().size();
    for (int i = 0; i < size; i++) {
        if (ui->buttonGroup->buttons().at(i)->isChecked()) {
            m_liteApp->settings()->setValue(LITEAPP_TOOLBARICONSIZE,i);
            break;
        }
    }
}
