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

int BuildConfigDialog::s_lastViewIndex = 4;

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

    ui->actionTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->actionTableView->resizeColumnsToContents();
    ui->actionTableView->verticalHeader()->hide();

    m_liteideModel = new QStandardItemModel(0,2,this);
    m_liteideModel->setHeaderData(0,Qt::Horizontal,tr("Name"));
    m_liteideModel->setHeaderData(1,Qt::Horizontal,tr("Value"));

    m_configModel = new QStandardItemModel(0,2,this);
    m_configModel->setHeaderData(0,Qt::Horizontal,tr("Name"));
    m_configModel->setHeaderData(1,Qt::Horizontal,tr("Value"));

    m_customModel = new QStandardItemModel(0,3,this);
    m_customModel->setHeaderData(0,Qt::Horizontal,tr("Name"));
    m_customModel->setHeaderData(1,Qt::Horizontal,tr("Value"));
    m_customModel->setHeaderData(2,Qt::Horizontal,tr("SharedValue"));

    m_actionModel = new QStandardItemModel(0,2,this);
    m_actionModel->setHeaderData(0,Qt::Horizontal,tr("Id"));
    m_actionModel->setHeaderData(1,Qt::Horizontal,tr("Cmd"));

    ui->liteideTableView->setModel(m_liteideModel);
    ui->configTableView->setModel(m_configModel);
    ui->customTableView->setModel(m_customModel);
    ui->actionTableView->setModel(m_actionModel);
    ui->actionTableView->setTextElideMode(Qt::ElideNone);

    connect(ui->customTableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(editCustomeTabView(QModelIndex)));

    ui->tabWidget->setCurrentIndex(s_lastViewIndex);

    connect(ui->buttonBox,SIGNAL(clicked(QAbstractButton*)),this,SLOT(buttonBoxClicked(QAbstractButton*)));
}

BuildConfigDialog::~BuildConfigDialog()
{
    s_lastViewIndex = ui->tabWidget->currentIndex();
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

void BuildConfigDialog::buttonBoxClicked(QAbstractButton *button)
{
    QDialogButtonBox::ButtonRole role = ui->buttonBox->buttonRole(button);
    if (role == QDialogButtonBox::AcceptRole) {
        this->saveBuild();
    } else if (role == QDialogButtonBox::RejectRole) {
        this->reject();
    } else if (role == QDialogButtonBox::ApplyRole) {
        this->saveBuild();
        this->updat_gopath_info();
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

void BuildConfigDialog::setBuild(LiteApi::IBuild *build, const QString &buildPath, const QMap<QString, QString> &liteEnvMap)
{    
    m_buildPath = buildPath;

    updateBuildConfigHelp(build,buildPath,liteEnvMap,m_liteideModel,m_configModel,m_customModel,m_actionModel);

    resizeTableView(ui->liteideTableView);
    resizeTableView(ui->configTableView);
    resizeTableView(ui->actionTableView);
    resizeTableView(ui->customTableView);

#if QT_VERSION >= 0x050000
    ui->customTableView->horizontalHeader()->setSectionResizeMode(2,QHeaderView::ResizeToContents);
#else
    ui->customTableView->horizontalHeader()->setResizeMode(2,QHeaderView::ResizeToContents);
#endif

    ui->buildIdLabel->setText(build->id());
    ui->buildFileLabel->setText(buildPath);

    QString customKey = "litebuild-custom/"+buildPath;

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
    foreach (QString path, env.value("GOPATH").split(sep,qtSkipEmptyParts)) {
        pathList.append(QDir::toNativeSeparators(path));
    }
    pathList.removeDuplicates();
    ui->sysGopathLabel->setText(pathList.join("\n"));

    pathList.clear();
    foreach (QString path, m_liteApp->settings()->value("liteide/gopath").toStringList()) {
          pathList.append(QDir::toNativeSeparators(path));
    }
    pathList.removeDuplicates();
    ui->liteGopathLabel->setText(pathList.join("\n"));

    pathList.clear();
    foreach (QString path, m_liteApp->settings()->value(customKey+"#gopath").toStringList()) {
          pathList.append(QDir::toNativeSeparators(path));
    }
    pathList.removeDuplicates();
    ui->customGopathEdit->setPlainText(pathList.join("\n"));

//    QStringList saveAutoRun = m_liteApp->settings()->value(customKey+"#saveautorun").toStringList();
//    ui->saveAutoRunEdit->setText(saveAutoRun.join(";"));

    updat_gopath_info();
}

void BuildConfigDialog::saveBuild()
{
    saveGopath();
    saveCustom();
    saveAction();
}

void BuildConfigDialog::saveGopath()
{
    if (m_buildPath.isEmpty()) {
        return;
    }

    QString customKey = "litebuild-custom/"+m_buildPath;

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

    LiteApi::IGoEnvManger *goEnv = LiteApi::getGoEnvManager(m_liteApp);
    if (goEnv) {
        goEnv->updateCustomGOPATH(m_buildPath);
    }
}

void BuildConfigDialog::saveCustom()
{
    if (m_buildPath.isEmpty()) {
        return;
    }
    QString key;
    key = "litebuild-custom/"+m_buildPath;

    for (int i = 0; i < m_customModel->rowCount(); i++) {
        QStandardItem *name = m_customModel->item(i,0);
        QStandardItem *value = m_customModel->item(i,1);
        QStandardItem *sharedValue = m_customModel->item(i,2);
        QString id = name->data().toString();
        if (!key.isEmpty()) {
            QString defValue = value->data().toString();
            bool defShared = sharedValue->data().toBool();
            LiteApi::updateAppSetting(m_liteApp,key+"#"+id,value->text(),defValue);
            LiteApi::updateAppSetting(m_liteApp,key+"#"+id+"#shared",sharedValue->checkState() == Qt::Checked ? true:false,defShared);
        }
    }
}

void BuildConfigDialog::saveAction()
{
//    if (m_buildPath.isEmpty()) {
//        return;
//    }
//    QString key;
//    key = "litebuild-custom/"+m_buildPath;
//    QStringList task;
//    foreach (QString id, ui->saveAutoRunEdit->text().split(";")) {
//        id = id.trimmed();
//        if (!id.isEmpty()) {
//            task.push_back(id);
//        }
//    }
//    LiteApi::updateAppSetting(m_liteApp,key+"#saveautorun",task,QStringList());
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
        ui->customGopathEdit->appendPlainText(dir);
    }
}

void BuildConfigDialog::on_customGopathClear_clicked()
{
    ui->customGopathEdit->clear();
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

void BuildConfigDialog::updat_gopath_info()
{
#ifdef Q_OS_WIN
    QString sep = ";";
#else
    QString sep = ":";
#endif
    QString customPath;
    QProcessEnvironment env = LiteApi::getCustomGoEnvironment(m_liteApp,m_buildPath,&customPath);

    QString infoHead;
    if (!customPath.isEmpty()) {
        infoHead = QString("Use custom GOPATH for build path <b>%1</b>").arg(QDir::toNativeSeparators(customPath));
    } else {
        infoHead = "Use LiteIDE Global GOPATH";
    }
    QStringList gopathList = env.value("GOPATH").split(sep);

    ui->gopathInfoHeadLabel->setText(infoHead);
    ui->gopathInfoLabel->setText(gopathList.join("\n"));
}

void BuildConfigDialog::updateBuildConfigHelp(LiteApi::IBuild *build, const QString &buildRootPath, const QMap<QString,QString> &liteEnvMap, QStandardItemModel *liteideModel, QStandardItemModel *configModel, QStandardItemModel *customModel, QStandardItemModel *actionModel)
{
    liteideModel->removeRows(0,liteideModel->rowCount());
    QMapIterator<QString,QString> i(liteEnvMap);
    while (i.hasNext()) {
        i.next();
        liteideModel->appendRow(QList<QStandardItem*>()
                                 << new QStandardItem(i.key())
                                 << new QStandardItem(i.value()));
    }
    if (build) {
        configModel->removeRows(0,configModel->rowCount());
        customModel->removeRows(0,customModel->rowCount());
        actionModel->removeRows(0,actionModel->rowCount());
        QString customkey;
        if (!buildRootPath.isEmpty()) {
            customkey = "litebuild-custom/"+buildRootPath;
        }
        QString configkey = "litebuild-config/"+build->id();
        foreach(LiteApi::BuildCustom *cf, build->customList()) {
            QString name = cf->name();
            QString value = cf->value();
            QString sharedValue = cf->sharedValue();
            bool sharedChecked = cf->hasShared();
            if (!customkey.isEmpty()) {
                value = m_liteApp->settings()->value(customkey+"#"+cf->id(),value).toString();
                sharedChecked = m_liteApp->settings()->value(customkey+"#"+cf->id()+"#shared",true).toBool();
            }
            QStandardItem *nameItem = new QStandardItem(name);
            QStandardItem *valueItem = new QStandardItem(value);
            if (cf->isReadOnly()) {
                valueItem->setEnabled(false);
            }
            QStandardItem *sharedItem = new QStandardItem(sharedValue);
            sharedItem->setEnabled(cf->hasShared());
            if (cf->hasShared()) {
                sharedItem->setCheckable(true);
                sharedItem->setCheckState(sharedChecked ? Qt::Checked : Qt::Unchecked);
            }
            nameItem->setData(cf->id());
            valueItem->setData(cf->value());
            sharedItem->setData(cf->hasShared());
            customModel->appendRow(QList<QStandardItem*>()
                                     << nameItem
                                     << valueItem
                                     << sharedItem );

        }
        foreach(LiteApi::BuildConfig *cf, build->configList()) {
            QString name = cf->name();
            QString value = cf->value();
            if (!configkey.isEmpty()) {
                value = m_liteApp->settings()->value(configkey+"#"+cf->id(),value).toString();
            }
            QStandardItem *item = new QStandardItem(name);
            item->setData(cf->id());
            configModel->appendRow(QList<QStandardItem*>()
                                     << item
                                     << new QStandardItem(value));
        }
        foreach (LiteApi::BuildAction *ba, build->actionList()) {
            QString id = ba->id();
            QString cmd = ba->cmd();
            QString args = ba->args();
            QStringList task = ba->task();
            if (task.isEmpty() && cmd.isEmpty()) {
                continue;
            }
            QStandardItem *item = new QStandardItem(id);
            QString value;
            if (!task.isEmpty()) {
                value = task.join(";");
            } else {
                value = cmd+" "+args;
            }
            actionModel->appendRow(QList<QStandardItem*>()
                                   << item
                                   << new QStandardItem(value) );
        }
    }
}
