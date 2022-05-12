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
// Module: goaddtagsdialog.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "goaddtagsdialog.h"
#include "ui_goaddtagsdialog.h"
#include "liteapi/liteqt.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

GoAddTagsDialog::GoAddTagsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GoAddTagsDialog)
{
    ui->setupUi(this);

    connect(ui->jsonCheckBox,SIGNAL(toggled(bool)),this,SLOT(updateaAguments()));
    connect(ui->jsonCheckBox,SIGNAL(toggled(bool)),ui->jsonGroupBox,SLOT(setEnabled(bool)));

    connect(ui->jsonOptionsCheckBox,SIGNAL(toggled(bool)),this,SLOT(updateaAguments()));
    connect(ui->jsonOptionsLineEdit,SIGNAL(textChanged(QString)),this,SLOT(updateaAguments()));

    connect(ui->xmlCheckBox,SIGNAL(toggled(bool)),this,SLOT(updateaAguments()));
    connect(ui->xmlCheckBox,SIGNAL(toggled(bool)),ui->xmlGroupBox,SLOT(setEnabled(bool)));

    connect(ui->xmlOptionsCheckBox,SIGNAL(toggled(bool)),this,SLOT(updateaAguments()));
    connect(ui->xmlOptionsLineEdit,SIGNAL(textChanged(QString)),this,SLOT(updateaAguments()));

    connect(ui->customCheckBox,SIGNAL(toggled(bool)),this,SLOT(updateaAguments()));
    connect(ui->customCheckBox,SIGNAL(toggled(bool)),ui->customGroupBox,SLOT(setEnabled(bool)));

    connect(ui->customTagNameLineEdit1,SIGNAL(textChanged(QString)),this,SLOT(updateaAguments()));
    connect(ui->customTagOptionLineEdit1,SIGNAL(textChanged(QString)),this,SLOT(updateaAguments()));
    connect(ui->customTagNameLineEdit2,SIGNAL(textChanged(QString)),this,SLOT(updateaAguments()));
    connect(ui->customTagOptionlineEdit2,SIGNAL(textChanged(QString)),this,SLOT(updateaAguments()));

    connect(ui->snakeCaseRadioButton,SIGNAL(toggled(bool)),this,SLOT(updateaAguments()));
    connect(ui->camelCaseRadioButton,SIGNAL(toggled(bool)),this,SLOT(updateaAguments()));
    connect(ui->lispCaseRadioButton,SIGNAL(toggled(bool)),this,SLOT(updateaAguments()));

    connect(ui->overrideCheckBox,SIGNAL(toggled(bool)),this,SLOT(updateaAguments()));
    connect(ui->sortCheckBox,SIGNAL(toggled(bool)),this,SLOT(updateaAguments()));

    ui->snakeCaseRadioButton->setChecked(true);

    ui->jsonGroupBox->setEnabled(false);
    ui->xmlGroupBox->setEnabled(false);
    ui->customGroupBox->setEnabled(false);
}

GoAddTagsDialog::~GoAddTagsDialog()
{
    delete ui;
}

void GoAddTagsDialog::setInfo(const QString &info)
{
    ui->infoLabel->setText(info);
}

QString GoAddTagsDialog::arguments() const
{
    return ui->argumentsEdit->toPlainText().trimmed();
}

static QString parserTag(const QString &tag)
{
   int pos = tag.indexOf(":");
   if (pos == -1) {
       return tag;
   }
   return tag.left(pos);
}
void GoAddTagsDialog::updateaAguments()
{
    QStringList tagList;
    QStringList optList;
    QString tranform;
    if (ui->jsonCheckBox->isChecked()) {
        tagList << "json";
        if (ui->jsonOptionsCheckBox->isChecked()) {
            QStringList opt = ui->jsonOptionsLineEdit->text().trimmed().split(",",qtSkipEmptyParts);
            foreach (QString o, opt) {
                optList << "json="+o;
            }
        }
    }
    if (ui->xmlCheckBox->isChecked()) {
        tagList << "xml";
        if (ui->xmlOptionsCheckBox->isChecked()) {
            QStringList opt = ui->xmlOptionsLineEdit->text().trimmed().split(",",qtSkipEmptyParts);
            foreach (QString o, opt) {
                optList << "xml="+o;
            }
        }
    }
    if (ui->customCheckBox->isChecked()) {
        QString tag1 = ui->customTagNameLineEdit1->text().trimmed();
        QStringList opt1 = ui->customTagOptionLineEdit1->text().trimmed().split(",",qtSkipEmptyParts);
        QString tag2 = ui->customTagNameLineEdit2->text().trimmed();
        QStringList opt2 = ui->customTagOptionlineEdit2->text().trimmed().split(",",qtSkipEmptyParts);
        if (!tag1.isEmpty()) {
            tagList << tag1;
            foreach (QString o, opt1) {
                optList << parserTag(tag1)+"="+o;
            }
        }
        if (!tag2.isEmpty()) {
            tagList << tag2;
            foreach (QString o, opt2) {
                optList << parserTag(tag2)+"="+o;
            }
        }
    }
    QString info;
    if (ui->snakeCaseRadioButton->isChecked()) {
        tranform = "snakecase";
        info = "BaseDomain -> base_domain";
    } else if (ui->camelCaseRadioButton->isChecked()) {
        tranform = "camelcase";
        info = "BaseDomain -> baseDomain";
    } else if (ui->lispCaseRadioButton->isChecked()) {
        tranform = "lispcase";
        info = "BaseDomain -> base-domain";
    }
    if (ui->transformInfo->text() != info) {
        ui->transformInfo->setText(info);
    }

    QString command;
    if (!tagList.isEmpty()) {
        command += "-add-tags "+tagList.join(",");
        if (!optList.isEmpty()) {
            command += " -add-options "+optList.join(",");
        }
        command += " -transform "+tranform;
        if (ui->overrideCheckBox->isChecked()) {
            command += " -override";
        }
        if (ui->sortCheckBox->isChecked()) {
            command += " -sort";
        }
    }
    ui->argumentsEdit->setPlainText(command);
}
