/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 visualfc. All rights reserved.
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
// Module: golangpackageoption.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golangpackageoption.h"
#include "ui_golangpackageoption.h"
#include "golangpackage_global.h"
#include "liteenvapi/liteenvapi.h"
#include "gotool.h"
#include <QFileDialog>
#include <QLineEdit>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


GolangPackageOption::GolangPackageOption(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    ui(new Ui::GolangPackageOption),
    m_widget(new QWidget)
{
    ui->setupUi(m_widget);
    ui->cmbGoModule->addItems(QStringList() << "auto" << "on" << "off");
    ui->cmbGoModule->setCurrentIndex(0);
    ui->cmbGoModule->setEnabled(false);
    ui->cmbGoProxy->setEnabled(false);
    ui->editGoPrivate->setEnabled(false);
    ui->editGoNoProxy->setEnabled(false);
    ui->editGoNoSumdb->setEnabled(false);

    ui->cmbGoProxy->addItems(QStringList()
                            << "https://proxy.golang.org,direct"
                            << "https://goproxy.io"
                            << "https://goproxy.io,direct"
                            << "https://goproxy.cn"
                            << "https://goproxy.cn,direct");
    connect(ui->browserButton,SIGNAL(clicked()),this,SLOT(browser()));
    connect(ui->clearButton,SIGNAL(clicked()),ui->litePathTextEdit,SLOT(clear()));
    connect(ui->chkUseSysGopath,SIGNAL(toggled(bool)),ui->sysPathTextEdit,SLOT(setEnabled(bool)));
    connect(ui->chkUseLiteGopath,SIGNAL(toggled(bool)),ui->litePathTextEdit,SLOT(setEnabled(bool)));
    connect(ui->chkUseGoModule,SIGNAL(toggled(bool)),ui->cmbGoModule,SLOT(setEnabled(bool)));
    connect(ui->chkUseGoProxy,SIGNAL(toggled(bool)),ui->cmbGoProxy,SLOT(setEnabled(bool)));
    connect(ui->chkUseGoPrivate,SIGNAL(toggled(bool)),ui->editGoPrivate,SLOT(setEnabled(bool)));
    connect(ui->chkUseGoNoProxy,SIGNAL(toggled(bool)),ui->editGoNoProxy,SLOT(setEnabled(bool)));
    connect(ui->chkUseGoNoSumdb,SIGNAL(toggled(bool)),ui->editGoNoSumdb,SLOT(setEnabled(bool)));
}

GolangPackageOption::~GolangPackageOption()
{
    delete ui;
}


void GolangPackageOption::setSysPathList(const QStringList &pathList)
{
    ui->sysPathTextEdit->clear();
    foreach (QString path, pathList) {
        ui->sysPathTextEdit->appendPlainText(path);
    }
}

void GolangPackageOption::setLitePathList(const QStringList &pathList)
{
    ui->litePathTextEdit->clear();
    foreach (QString path, pathList) {
        ui->litePathTextEdit->appendPlainText(path);
    }
}

void GolangPackageOption::setUseSysGopath(bool b)
{
    ui->chkUseSysGopath->setChecked(b);
}

bool GolangPackageOption::isUseSysGopath() const
{
    return ui->chkUseSysGopath->isChecked();
}

void GolangPackageOption::setUseLiteGopath(bool b)
{
    ui->chkUseLiteGopath->setChecked(b);
}

bool GolangPackageOption::isUseLiteGopath() const
{
    return ui->chkUseLiteGopath->isChecked();
}

void GolangPackageOption::setUseGoModule(bool b)
{
    ui->chkUseGoModule->setChecked(b);
}

bool GolangPackageOption::isUseGoModule() const
{
    return ui->chkUseGoModule->isChecked();
}

void GolangPackageOption::setSysGoModuleInfo(const QString &value)
{
    ui->sysGoModuleInfo->setText(value);
}

void GolangPackageOption::setGo111Module(const QString &value)
{
    for (int i = 0; i < ui->cmbGoModule->count(); i++) {
        if (ui->cmbGoModule->itemText(i) == value) {
            ui->cmbGoModule->setCurrentIndex(i);
            break;
        }
    }
}

QString GolangPackageOption::go111Module() const
{
    return ui->cmbGoModule->currentText();
}

QStringList GolangPackageOption::litePathList() const
{
    return ui->litePathTextEdit->toPlainText().split("\n",QString::SkipEmptyParts);
}

void GolangPackageOption::setUseGoProxy(bool b)
{
    ui->chkUseGoProxy->setChecked(b);
}

bool GolangPackageOption::isUseGoProxy() const
{
    return ui->chkUseGoProxy->isChecked();
}

void GolangPackageOption::setGoProxy(const QString &v)
{
    ui->cmbGoProxy->lineEdit()->setText(v);
}

QString GolangPackageOption::goProxy() const
{
    return ui->cmbGoProxy->currentText();
}

void GolangPackageOption::setUseGoPrivate(ENUM_GO_PRIVATE id, bool b)
{
    switch (id) {
    case GO_PRIVATE:
        ui->chkUseGoPrivate->setChecked(b);
        break;
    case GO_NOPROXY:
        ui->chkUseGoNoProxy->setChecked(b);
        break;
    case GO_NOSUMDB:
        ui->chkUseGoNoSumdb->setChecked(b);
        break;
    }
}

bool GolangPackageOption::isUseGoPrivate(ENUM_GO_PRIVATE id) const
{
    switch (id) {
    case GO_PRIVATE:
        return  ui->chkUseGoPrivate->isChecked();
    case GO_NOPROXY:
        return  ui->chkUseGoNoProxy->isChecked();
    case GO_NOSUMDB:
        return  ui->chkUseGoNoSumdb->isChecked();
    }
    return  false;
}

void GolangPackageOption::setGoPrivate(ENUM_GO_PRIVATE id, const QString &value)
{
    switch (id) {
    case GO_PRIVATE:
        ui->editGoPrivate->setText(value);
        break;
    case GO_NOPROXY:
        ui->editGoNoProxy->setText(value);
        break;
    case GO_NOSUMDB:
        ui->editGoNoSumdb->setText(value);
        break;
    }
}

QString GolangPackageOption::goPrivate(ENUM_GO_PRIVATE id) const
{
    switch (id) {
    case GO_PRIVATE:
        return  ui->editGoPrivate->text();
    case GO_NOPROXY:
        return  ui->editGoNoProxy->text();
    case GO_NOSUMDB:
        return  ui->editGoNoSumdb->text();
    }
    return  QString();
}

QString GolangPackageOption::name() const
{
    return "GolangPackage";
}

QString GolangPackageOption::mimeType() const
{
    return "option/golangpackage";
}

void GolangPackageOption::save()
{
    QStringList newLitePath = this->litePathList();
    //m_liteApp->sendBroadcast("golangpackage","reloadgopath");
    m_liteApp->settings()->setValue(LITEIDE_USESYSGOPATH,this->isUseSysGopath());
    m_liteApp->settings()->setValue(LITEIDE_USELITEIDEGOPATH,this->isUseLiteGopath());
    m_liteApp->settings()->setValue(LITEIDE_CUSTOMGO111MODULE,this->isUseGoModule());
    m_liteApp->settings()->setValue(LITEIDE_GO111MODULE,this->go111Module());

    m_liteApp->settings()->setValue(LITEIDE_USEGOPROXY,this->isUseGoProxy());
    m_liteApp->settings()->setValue(LITEIDE_GOPROXY,this->goProxy());

    m_liteApp->settings()->setValue(LITEIDE_USEGOPRIVATE,this->isUseGoPrivate(GO_PRIVATE));
    m_liteApp->settings()->setValue(LITEIDE_GOPRIVATE,this->goPrivate(GO_PRIVATE));

    m_liteApp->settings()->setValue(LITEIDE_USEGONOPROXY,this->isUseGoPrivate(GO_NOPROXY));
    m_liteApp->settings()->setValue(LITEIDE_GONOPROXY,this->goPrivate(GO_NOPROXY));

    m_liteApp->settings()->setValue(LITEIDE_USEGONOSUMDB,this->isUseGoPrivate(GO_NOSUMDB));
    m_liteApp->settings()->setValue(LITEIDE_GONOSUMDB,this->goPrivate(GO_NOSUMDB));
    //if (!hasSameList(orgLitePath,newLitePath)) {
    setLiteGopath(m_liteApp,newLitePath);
//    this->reloadAll();
    LiteApi::IEnvManager *env = LiteApi::getEnvManager(m_liteApp);
    if (env) {
        env->reloadCurrentEnv();
    }
}

void GolangPackageOption::load()
{
    this->setSysPathList(sysGopath(m_liteApp));
    this->setLitePathList(liteGopath(m_liteApp));
    this->setUseSysGopath(m_liteApp->settings()->value(LITEIDE_USESYSGOPATH,true).toBool());
    this->setUseLiteGopath(m_liteApp->settings()->value(LITEIDE_USELITEIDEGOPATH,true).toBool());
    this->setUseGoModule(m_liteApp->settings()->value(LITEIDE_CUSTOMGO111MODULE,false).toBool());
    this->setGo111Module(m_liteApp->settings()->value(LITEIDE_GO111MODULE,"auto").toString());
    this->setUseGoProxy(m_liteApp->settings()->value(LITEIDE_USEGOPROXY,false).toBool());
    this->setGoProxy(m_liteApp->settings()->value(LITEIDE_GOPROXY,"").toString());

    this->setUseGoPrivate(GO_PRIVATE,m_liteApp->settings()->value(LITEIDE_USEGOPRIVATE,false).toBool());
    this->setGoPrivate(GO_PRIVATE,m_liteApp->settings()->value(LITEIDE_GOPRIVATE,"").toString());

    this->setUseGoPrivate(GO_NOPROXY,m_liteApp->settings()->value(LITEIDE_USEGONOPROXY,false).toBool());
    this->setGoPrivate(GO_NOPROXY,m_liteApp->settings()->value(LITEIDE_GONOPROXY,"").toString());

    this->setUseGoPrivate(GO_NOSUMDB,m_liteApp->settings()->value(LITEIDE_USEGONOSUMDB,false).toBool());
    this->setGoPrivate(GO_NOSUMDB,m_liteApp->settings()->value(LITEIDE_GONOSUMDB,"").toString());

    QProcessEnvironment env = LiteApi::getCurrentEnvironment(m_liteApp);
    QString info = env.value("GO111MODULE");
    if (!info.isEmpty()) {
        this->setSysGoModuleInfo(QString("system GO111MODULE=%1").arg(info));
    } else {
        this->setSysGoModuleInfo("system GO111MODULE unset");
    }

}

QWidget *GolangPackageOption::widget()
{
    return m_widget;
}

void GolangPackageOption::browser()
{
    static QString last = QDir::homePath();
    QString dir = QFileDialog::getExistingDirectory(m_widget, tr("Choose directory to add to GOPATH:"),
                                                    last,
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        last = dir;
        ui->litePathTextEdit->appendPlainText(dir);
    }
}
