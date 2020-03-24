/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2017 LiteIDE. All rights reserved.
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
// Module: goremovetagsdialog.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "goremovetagsdialog.h"
#include "ui_goremovetagsdialog.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

GoRemoveTagsDialog::GoRemoveTagsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GoRemoveTagsDialog)
{
    ui->setupUi(this);
    connect(ui->clearAllTagsRadioButton,SIGNAL(toggled(bool)),this,SLOT(updateArguments()));
    connect(ui->clearAllOptionsRadioButton,SIGNAL(toggled(bool)),this,SLOT(updateArguments()));

    connect(ui->removeJsonTagRadioButton,SIGNAL(toggled(bool)),this,SLOT(updateArguments()));
    connect(ui->removeXmlOptionRadioButton,SIGNAL(toggled(bool)),this,SLOT(updateArguments()));
    connect(ui->removeCustomTagRadioButton,SIGNAL(toggled(bool)),this,SLOT(updateArguments()));

    connect(ui->removeJsonOptionRadioButton,SIGNAL(toggled(bool)),this,SLOT(updateArguments()));
    connect(ui->removeXmlOptionRadioButton,SIGNAL(toggled(bool)),this,SLOT(updateArguments()));
    connect(ui->removeCustomOptionRadioButton,SIGNAL(toggled(bool)),this,SLOT(updateArguments()));

    connect(ui->customTaglineEdit,SIGNAL(textChanged(QString)),this,SLOT(updateArguments()));
    connect(ui->jsonOptionLineEdit,SIGNAL(textChanged(QString)),this,SLOT(updateArguments()));
    connect(ui->xmlOptionLineEdit,SIGNAL(textChanged(QString)),this,SLOT(updateArguments()));
    connect(ui->customOptionLineEdit,SIGNAL(textChanged(QString)),this,SLOT(updateArguments()));
}

GoRemoveTagsDialog::~GoRemoveTagsDialog()
{
    delete ui;
}

void GoRemoveTagsDialog::setInfo(const QString &info)
{
    ui->infoLabel->setText(info);
}

QString GoRemoveTagsDialog::arguments() const
{
    return ui->argumentsEdit->toPlainText().trimmed();
}

static QString removeHead(const QString &text, const QString &head)
{
    if (text.startsWith(head)) {
        return text.mid(head.length());
    }
    return text;
}

void GoRemoveTagsDialog::updateArguments()
{
    QString args;
    if (ui->clearAllTagsRadioButton->isChecked()) {
        args = "-clear-tags";
    } else if (ui->clearAllOptionsRadioButton->isChecked()) {
        args = "-clear-options";
    } else if (ui->removeJsonTagRadioButton->isChecked()) {
        args = "-remove-tags json";
    } else if (ui->removeXmlTagRadioButton->isChecked()) {
        args = "-remove-tags xml";
    } else if (ui->removeCustomTagRadioButton->isChecked()) {
        QString tag = ui->customTaglineEdit->text().trimmed();
        if (!tag.isEmpty()) {
            args = "-remove-tags "+tag;
        }
    } else if (ui->removeJsonOptionRadioButton->isChecked()) {
        QStringList optList = ui->jsonOptionLineEdit->text().trimmed().split(",",QString::SkipEmptyParts);
        QStringList options;
        foreach (QString opt, optList) {
            options << "json="+opt;
        }
        if (!options.isEmpty()) {
            args = "-remove-options "+options.join(",");
        }
    } else if (ui->removeXmlOptionRadioButton->isChecked()) {
        QStringList optList = ui->xmlOptionLineEdit->text().trimmed().split(",",QString::SkipEmptyParts);
        QStringList options;
        foreach (QString opt, optList) {
            options << "xml="+opt;
        }
        if (!options.isEmpty()) {
            args = "-remove-options "+options.join(",");
        }
    } else if(ui->removeCustomOptionRadioButton->isChecked()) {
        QString opt = ui->customOptionLineEdit->text().trimmed();
        if (opt == ui->customOptionLineEdit->placeholderText()) {
            if (ui->customOptionLineEdit->cursorPosition() == 0) {
                opt.clear();
            }
        }
        if (opt.contains("=")) {
            args = "-remove-options "+opt;
        }
    }

    ui->argumentsEdit->setPlainText(args);
}
