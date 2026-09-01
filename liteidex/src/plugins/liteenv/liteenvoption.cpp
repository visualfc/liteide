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
// Module: liteenvoption.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "liteenvoption.h"
#include "ui_liteenvoption.h"
#include "liteenvapi/liteenvapi.h"
#include "envmanager.h"
#include <QDir>
#include <QFileInfo>
#include <QStandardItemModel>
#include <QFile>
#include <QMessageBox>
#include <QInputDialog>
#include <QDialog>
#include <QTimer>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

LiteEnvOption::LiteEnvOption(LiteApi::IApplication *app,QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::LiteEnvOption)
{
    ui->setupUi(m_widget);

    m_fileModel = new QStandardItemModel(this);
    m_fileModel->setHorizontalHeaderLabels(QStringList() << tr("Environment"));
    ui->fileTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->fileTreeView->setModel(m_fileModel);
#if QT_VERSION >= 0x050000
    ui->fileTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    ui->fileTreeView->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif
    connect(ui->fileTreeView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClickedFile(QModelIndex)));
    connect(ui->newButton,SIGNAL(clicked()),this,SLOT(newUserTemplate()));
    connect(ui->deleteButton,SIGNAL(clicked()),this,SLOT(deleteUserTemplate()));
    connect(ui->renameButton,SIGNAL(clicked()),this,SLOT(renameUserTemplate()));
    refreshFiles();
}

LiteEnvOption::~LiteEnvOption()
{
    delete m_widget;
    delete ui;
}

QWidget *LiteEnvOption::widget()
{
    return m_widget;
}

QString LiteEnvOption::name() const
{
    return "LiteEnv";
}

QString LiteEnvOption::mimeType() const
{
    return "option/liteenv";
}

void LiteEnvOption::load()
{
    refreshFiles();
}

void LiteEnvOption::save()
{
}

void LiteEnvOption::doubleClickedFile(QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }
    QString path = index.data(Qt::UserRole).toString();
    if (!path.isEmpty()) {
        const QString templateRoot = QDir(m_liteApp->resourcePath()+"/liteenv").absolutePath();
        if (QDir::cleanPath(QFileInfo(path).absolutePath()) == QDir::cleanPath(templateRoot)) {
            path = QDir(m_liteApp->storagePath()+"/liteenv").absoluteFilePath(QFileInfo(path).fileName());
            QDir userDir = QFileInfo(path).dir();
            if (!userDir.exists() && !QDir().mkpath(userDir.absolutePath())) {
                return;
            }
            if (!QFileInfo::exists(path) && !QFile::copy(index.data(Qt::UserRole).toString(),path)) {
                return;
            }
        }
        if (m_liteApp->fileManager()->openEditor(path)) {
            QDialog *dialog = qobject_cast<QDialog*>(m_widget->window());
            if (dialog) {
                dialog->accept();
            }
        }
    }
}

void LiteEnvOption::refreshFiles()
{
    m_fileModel->clear();
    m_fileModel->setHorizontalHeaderLabels(QStringList() << tr("Environment"));
    const QDir templateDir(m_liteApp->resourcePath()+"/liteenv");
    const QDir userDir(m_liteApp->storagePath()+"/liteenv");
    const QStringList templates = templateDir.entryList(QStringList() << "*.env", QDir::Files | QDir::NoSymLinks, QDir::Name);
    foreach (const QString &fileName, templates) {
        QStandardItem *item = new QStandardItem(tr("[System] %1").arg(fileName));
        item->setData(templateDir.absoluteFilePath(fileName),Qt::UserRole);
        item->setData(false,Qt::UserRole + 1);
        m_fileModel->appendRow(item);
    }
    const QStringList users = userDir.entryList(QStringList() << "*.env", QDir::Files | QDir::NoSymLinks, QDir::Name);
    foreach (const QString &fileName, users) {
        QStandardItem *item = new QStandardItem(tr("[User] %1").arg(fileName));
        item->setData(userDir.absoluteFilePath(fileName),Qt::UserRole);
        item->setData(true,Qt::UserRole + 1);
        m_fileModel->appendRow(item);
    }
    ui->fileTreeView->resizeColumnToContents(0);
}

bool LiteEnvOption::closeEditor(const QString &path)
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->findEditor(path, false);
    return !editor || m_liteApp->editorManager()->closeEditor(editor);
}

void LiteEnvOption::newUserTemplate()
{
    QModelIndex current = ui->fileTreeView->currentIndex();
    QString source = current.data(Qt::UserRole).toString();
    if (source.isEmpty() || !QFileInfo::exists(source)) {
        source = QDir(m_liteApp->resourcePath() + "/liteenv").absoluteFilePath("system.env");
    }
    const QString sourceName = QFileInfo(source).fileName();
    bool ok = false;
    QString id = QInputDialog::getText(m_widget,tr("New User Environment"),tr("Environment ID (based on %1):").arg(sourceName),QLineEdit::Normal,QString(),&ok).trimmed();
    if (!ok || id.isEmpty() || id.contains('/') || id.contains('\\') || id.endsWith(".env")) {
        return;
    }
    const QString fileName = id + ".env";
    const QString path = QDir(m_liteApp->storagePath()+"/liteenv").absoluteFilePath(fileName);
    if (QFileInfo::exists(path)) {
        return;
    }
    QDir userDir = QFileInfo(path).dir();
    if (!userDir.exists() && !QDir().mkpath(userDir.absolutePath())) {
        return;
    }
    if (QFileInfo::exists(source)) {
        if (!QFile::copy(source, path)) {
            return;
        }
    } else {
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly)) {
            return;
        }
        file.close();
    }
    refreshFiles();
    EnvManager *envManager = qobject_cast<EnvManager*>(LiteApi::getEnvManager(m_liteApp));
    if (envManager) {
        envManager->addUserEnvFile(path);
    }
    QDialog *dialog = qobject_cast<QDialog*>(m_widget->window());
    if (dialog) {
        dialog->accept();
    }
    QTimer::singleShot(0, m_liteApp->mainWindow(), [this, path]() {
        m_liteApp->fileManager()->openEditor(path,true);
    });
}

void LiteEnvOption::deleteUserTemplate()
{
    QModelIndex index = ui->fileTreeView->currentIndex();
    if (!index.isValid() || !index.data(Qt::UserRole + 1).toBool()) {
        return;
    }
    const QString path = index.data(Qt::UserRole).toString();
    if (QMessageBox::question(m_widget,tr("Delete User Environment"),tr("Delete %1?").arg(QFileInfo(path).fileName()),QMessageBox::Yes | QMessageBox::No,QMessageBox::No) != QMessageBox::Yes) {
        return;
    }
    if (!closeEditor(path)) {
        return;
    }
    if (QFile::remove(path)) {
        refreshFiles();
        LiteApi::IEnvManager *envManager = LiteApi::getEnvManager(m_liteApp);
        EnvManager *manager = qobject_cast<EnvManager*>(envManager);
        if (manager) {
            manager->removeUserEnvFile(path);
        }
    }
}

void LiteEnvOption::renameUserTemplate()
{
    QModelIndex index = ui->fileTreeView->currentIndex();
    if (!index.isValid() || !index.data(Qt::UserRole + 1).toBool()) {
        return;
    }
    const QString oldPath = index.data(Qt::UserRole).toString();
    bool ok = false;
    QString id = QInputDialog::getText(m_widget,tr("Rename User Environment"),tr("Environment ID:"),QLineEdit::Normal,QFileInfo(oldPath).completeBaseName(),&ok).trimmed();
    if (!ok || id.isEmpty() || id.contains('/') || id.contains('\\') || id.endsWith(".env")) {
        return;
    }
    const QString newPath = QFileInfo(oldPath).dir().absoluteFilePath(id + ".env");
    if (QFileInfo::exists(newPath) || !closeEditor(oldPath) || !QFile::rename(oldPath,newPath)) {
        return;
    }
    EnvManager *envManager = qobject_cast<EnvManager*>(LiteApi::getEnvManager(m_liteApp));
    if (envManager) {
        envManager->renameUserEnvFile(oldPath,newPath);
    }
    refreshFiles();
}
