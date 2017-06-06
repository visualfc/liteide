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
// Module: buildconfigdialog.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "buildconfigdialog.h"
#include "ui_buildconfigdialog.h"
#include "liteenvapi/liteenvapi.h"
#include "liteapi/liteutil.h"

#include <QAbstractItemModel>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDebug>
#include <QFileDialog>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

BuildConfigDialog::BuildConfigDialog(LiteApi::IApplication *app, QWidget *parent) :
    QDialog(parent),
    m_liteApp(app),
    ui(new Ui::BuildConfigDialog)
{
    ui->setupUi(this);
    ui->liteideTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->liteideTableView->resizeColumnsToContents();
    ui->liteideTableView->verticalHeader()->hide();

    ui->configTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->configTableView->resizeColumnsToContents();
    ui->configTableView->verticalHeader()->hide();

    ui->customTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->customTableView->resizeColumnsToContents();
    ui->customTableView->verticalHeader()->hide();

    ui->sysGopathEdit->setReadOnly(true);
    ui->liteGopathEdit->setReadOnly(true);

    connect(ui->customTableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(editCustomeTabView(QModelIndex)));

    ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
}

BuildConfigDialog::~BuildConfigDialog()
{
    delete ui;
}

void BuildConfigDialog::editCustomeTabView(QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }
    if (index.column() == 1) {
        ui->customTableView->edit(index);
    }
}

void BuildConfigDialog::resizeTableView(QTableView *tableView)
{
    QAbstractItemModel *model = tableView->model();
    if (model && model->columnCount() >= 2) {
        tableView->resizeColumnToContents(0);
#if QT_VERSION >= 0x050000
        tableView->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Interactive);
        for (int i = 1; i < model->columnCount(); i++) {
             tableView->horizontalHeader()->setSectionResizeMode(i,QHeaderView::Stretch);
        }
#else
        tableView->horizontalHeader()->setResizeMode(0,QHeaderView::Interactive);
        for (int i = 1; i < model->columnCount(); i++) {
            tableView->horizontalHeader()->setResizeMode(i,QHeaderView::Stretch);
        }
#endif
    }
}

void BuildConfigDialog::setBuild(const QString &buildId, const QString &buildFile)
{
    m_buildFile = buildFile;

    ui->buildIdLabel->setText(buildId);
    ui->buildFileLabel->setText(buildFile);

    QString customKey = "litebuild-custom/"+buildFile;

    bool use_custom_gopath = m_liteApp->settings()->value(customKey+"#use_custom_gopath",false).toBool();
    ui->useCustomGopathGroupBox->setChecked(use_custom_gopath);

    bool inherit_sys_gopath = m_liteApp->settings()->value(customKey+"#inherit_sys_gopath",true).toBool();
    bool inherit_lite_gopath = m_liteApp->settings()->value(customKey+"#inherit_lite_gopath",true).toBool();
    bool custom_gopath = m_liteApp->settings()->value(customKey+"#custom_gopath",false).toBool();

    ui->inheritSysGopathCheckBox->setChecked(inherit_sys_gopath);
    ui->inheritLiteGopathCheckBox->setChecked(inherit_lite_gopath);
    ui->customGopathCheckBox->setChecked(custom_gopath);

    QProcessEnvironment env = LiteApi::getSysEnvironment(m_liteApp);
#ifdef Q_OS_WIN
    QString sep = ";";
#else
    QString sep = ":";
#endif

    QStringList pathList;
    foreach (QString path, env.value("GOPATH").split(sep,QString::SkipEmptyParts)) {
        pathList.append(QDir::toNativeSeparators(path));
    }
    pathList.removeDuplicates();
    ui->sysGopathEdit->setPlainText(pathList.join("\n"));

    pathList.clear();
    foreach (QString path, m_liteApp->settings()->value("liteide/gopath").toStringList()) {
          pathList.append(QDir::toNativeSeparators(path));
    }
    pathList.removeDuplicates();
    ui->liteGopathEdit->setPlainText(pathList.join("\n"));

    pathList.clear();
    foreach (QString path, m_liteApp->settings()->value(customKey+"#gopath").toStringList()) {
          pathList.append(QDir::toNativeSeparators(path));
    }
    pathList.removeDuplicates();
    ui->customGopathEdit->setPlainText(pathList.join("\n"));
}

void BuildConfigDialog::saveCustomGopath()
{
    if (m_buildFile.isEmpty()) {
        return;
    }

    QString customKey = "litebuild-custom/"+m_buildFile;

    bool use_custom_gopath = ui->useCustomGopathGroupBox->isChecked();
    bool inherit_sys_gopath = ui->inheritSysGopathCheckBox->isChecked();
    bool inherit_lite_gopath = ui->inheritLiteGopathCheckBox->isChecked();
    bool custom_gopath = ui->customGopathCheckBox->isChecked();

//    m_liteApp->settings()->setValue(customKey+"#use_custom_gopath",use_custom_gopath);
//    m_liteApp->settings()->setValue(customKey+"#inherit_sys_gopath",inherit_sys_gopath);
//    m_liteApp->settings()->setValue(customKey+"#inherit_lite_gopath",inherit_lite_gopath);
//    m_liteApp->settings()->setValue(customKey+"#custom_gopath",custom_gopath);
//    m_liteApp->settings()->setValue(customKey+"#gopath",ui->customGopathEdit->toPlainText().split("\n"));
    LiteApi::updateAppSetting(m_liteApp,customKey+"#use_custom_gopath",use_custom_gopath,false);
    LiteApi::updateAppSetting(m_liteApp,customKey+"#inherit_sys_gopath",inherit_sys_gopath,true);
    LiteApi::updateAppSetting(m_liteApp,customKey+"#inherit_lite_gopath",inherit_lite_gopath,true);
    LiteApi::updateAppSetting(m_liteApp,customKey+"#custom_gopath",custom_gopath,false);
    LiteApi::updateAppSetting(m_liteApp,customKey+"#gopath",ui->customGopathEdit->toPlainText().split("\n"),"");
}

void BuildConfigDialog::setModel(QAbstractItemModel * liteide,QAbstractItemModel * config, QAbstractItemModel * custom)
{
    ui->liteideTableView->setModel(liteide);
    ui->configTableView->setModel(config);
    ui->customTableView->setModel(custom);
    resizeTableView(ui->liteideTableView);
    resizeTableView(ui->configTableView);
    resizeTableView(ui->customTableView);
}

void BuildConfigDialog::on_customGopathBrowserButton_clicked()
{
    static QString last = QDir::homePath();
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose directory to add to GOPATH:"),
                                                    last,
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        last = dir;
        ui->liteGopathEdit->appendPlainText(dir);
    }
}

void BuildConfigDialog::on_customGopathClear_clicked()
{
    ui->liteGopathEdit->clear();
}

void BuildConfigDialog::on_customResetAllButton_clicked()
{
    QStandardItemModel *model = (QStandardItemModel*)(ui->customTableView->model());
    for (int i = 0; i < model->rowCount(); i++) {
       // QStandardItem *name = model->item(i,0);
        QStandardItem *value = model->item(i,1);
        QStandardItem *sharedValue = model->item(i,2);
        value->setText(value->data().toString());
        if (sharedValue->data().toBool()) {
            sharedValue->setCheckState(Qt::Checked);
        }
    }
}
