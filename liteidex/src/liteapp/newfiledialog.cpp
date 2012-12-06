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
// Module: newfiledialog.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "newfiledialog.h"
#include "ui_newfiledialog.h"

#include <QStandardItemModel>
#include <QStandardItem>
#include <QStringListModel>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QTextCodec>
#include <QFileDialog>
#include <QMessageBox>
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


NewFileDialog::NewFileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewFileDialog)
{
    ui->setupUi(this);
    m_categoryModel = new QStandardItemModel(this);
    m_templateModel = new QStandardItemModel(this);
    m_pathModel = new QStringListModel(this);

    ui->categoryTreeView->setModel(m_categoryModel);
    ui->templateTreeView->setModel(m_templateModel);
    ui->pathTreeView->setModel(m_pathModel);

    ui->pathTreeView->setEditTriggers(0);
    ui->pathTreeView->setRootIsDecorated(false);
    ui->pathTreeView->setHeaderHidden(true);

    ui->categoryTreeView->setEditTriggers(0);
    ui->categoryTreeView->setRootIsDecorated(false);
    ui->categoryTreeView->setHeaderHidden(true);

    ui->templateTreeView->setEditTriggers(0);
    ui->templateTreeView->setRootIsDecorated(false);
    ui->templateTreeView->setHeaderHidden(true);

    connect(ui->categoryTreeView,SIGNAL(clicked(QModelIndex)),this,SLOT(activeCategory(QModelIndex)));
    connect(ui->templateTreeView,SIGNAL(clicked(QModelIndex)),this,SLOT(activeTemplate(QModelIndex)));
    connect(ui->nameLineEdit,SIGNAL(textChanged(QString)),this,SLOT(nameLineChanged(QString)));
    connect(ui->locationLineEdit,SIGNAL(textChanged(QString)),this,SLOT(locationLineChanged(QString)));
    connect(ui->pathTreeView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(activePath(QModelIndex)));
}

NewFileDialog::~NewFileDialog()
{
    delete ui;
}

void NewFileDialog::setPathList(const QStringList &pathList)
{
    m_pathModel->setStringList(pathList);
    if (m_gopath.isEmpty() && !pathList.isEmpty()) {
        m_gopath = pathList.first();
    }
}

void NewFileDialog::setGopath(const QString &path)
{
    m_gopath = path;
}

void NewFileDialog::setProjectLocation(const QString &path)
{
    m_projectLocation = path;
}

void NewFileDialog::setFileLocation(const QString &path)
{
    m_fileLocation = path;
}

void NewFileDialog::setTemplatePath(const QString &path)
{
    m_categoryModel->removeRows(0,m_categoryModel->rowCount());
    m_templateModel->removeRows(0,m_templateModel->rowCount());
    QDir dir(path);
    QFileInfoList infos = dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);
    foreach (QFileInfo info, infos) {
        QStandardItem *item = new QStandardItem(info.fileName());
        item->setData(info.filePath());
        m_categoryModel->appendRow(item);
    }
    if (m_categoryModel->rowCount() > 0) {
        QModelIndex index = m_categoryModel->index(0,0);
        ui->categoryTreeView->setCurrentIndex(index);
        activeCategory(index);
    }
}

void NewFileDialog::updateLocation()
{
    ui->nameLineEdit->clear();
    QModelIndex index = ui->templateTreeView->currentIndex();
    if (index.isValid()) {
        activeTemplate(index);
    }
}

void NewFileDialog::accept()
{
    if (!m_cur.isValid()) {
        return;
    }

    QString location = ui->locationLineEdit->text();
    QString name = ui->nameLineEdit->text();

    if (name.isEmpty()) {
        return;
    }
    QDir dir(location);
    if (!dir.exists()) {
        if (!dir.mkpath(location)) {
            QMessageBox::warning(this,"Warning",tr("MakePath false:")+location);
            return;
        }
    } else {
        if (m_cur.type != "file" && !dir.entryList(QDir::Files).isEmpty()) {
            int ret = QMessageBox::warning(this,"Warning",QString(tr("Location %1 is not empty, continue?")).arg(location),
                                 QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,QMessageBox::Cancel);
            if (ret != QMessageBox::Yes) {
                return;
            }
        }
    }


    m_stringMap.clear();
    m_stringMap.insert("$ROOT$",QFileInfo(name).fileName());
    m_openFiles.clear();

    m_openPath = location;

    QStringList ofiles;
    foreach (QString file, m_cur.files) {
        QString infile = QFileInfo(m_cur.dir,file).absoluteFilePath();
        QString ofile = file;
        QFileInfo oi(name);
        QString base = oi.fileName();
        if (!oi.suffix().isEmpty() &&
            (oi.suffix() == QFileInfo(infile).suffix())) {
            ofile.replace("root",base.left(base.length()-oi.suffix().length()-1));
        } else {
            ofile.replace("root",base);
        }

        QString outfile = QFileInfo(location,ofile).absoluteFilePath();
        if (QFile::exists(outfile)) {
            int ret = QMessageBox::warning(this,"Warning",QString(tr("%1 exists, continue to rewrite or abort?").arg(outfile)),
                                 QMessageBox::Yes|QMessageBox::No|QMessageBox::Abort,QMessageBox::Abort);
            if (ret == QMessageBox::Abort) {
                return;
            } else if (ret == QMessageBox::No) {
                continue;
            }
        }
        if (processFile(infile,outfile)) {
            ofiles.append(outfile);
            if (m_cur.open.contains(file)) {
                m_openFiles.append(QDir::toNativeSeparators(outfile));
            }
        }
    }
    if (ofiles.isEmpty()) {
        QMessageBox::warning(this,"Warning",QString(tr("%1 create false!")).arg(name));
        return;
    }
    QDialog::accept();
}

void NewFileDialog::activePath(QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }
    if (m_cur.type == "project") {
        m_projectLocation = index.data(Qt::DisplayRole).toString();
    } else if (m_cur.type == "file"){
        m_fileLocation = index.data(Qt::DisplayRole).toString();
    } else if (m_cur.type == "gopath") {
        m_gopath = index.data(Qt::DisplayRole).toString();
    }
    QModelIndex i = ui->templateTreeView->currentIndex();
    if (i.isValid()) {
        activeTemplate(i);
    }
}

void  NewFileDialog::activeCategory(QModelIndex index)
{
    m_templateModel->clear();
    m_cur.clear();
    ui->nameLineEdit->clear();
    ui->locationLineEdit->clear();

    if (!index.isValid()) {
        return;
    }

    QString root = index.data(Qt::UserRole+1).toString();
    QStringList files;
    files << "file.sub" << "project.sub";
    foreach (QString file, files) {
        QFile f(root+"/"+file);
        if (!f.open(QIODevice::ReadOnly)) {
            continue;
        }
        while (!f.atEnd()) {
            QString line = f.readLine().trimmed();
            QString setup = root+"/"+line+"/setup.inf";
            QSettings  set(setup,QSettings::IniFormat);
            if (set.status() != QSettings::NoError) {
                continue;
            }
            QString name = set.value("SETUP/NAME").toString();
            QString type = set.value("SETUP/type").toString();
            if (!name.isEmpty()) {
                QStandardItem *item = new QStandardItem(name);
                item->setData(setup);
                QFont font = item->font();
                if (type == "file") {
                    font.setItalic(true);
                } else {
                    font.setBold(true);
                }
                item->setFont(font);
                m_templateModel->appendRow(item);
            }
        }
        f.close();
    }
    if (m_templateModel->rowCount() > 0) {
        QModelIndex index = m_templateModel->index(0,0);
        ui->templateTreeView->setCurrentIndex(index);
        activeTemplate(index);
    }
}

void  NewFileDialog::activeTemplate(QModelIndex index)
{
    m_cur.clear();
    if (index.isValid()) {
        QString fileName = index.data(Qt::UserRole+1).toString();
        QSettings  set(fileName,QSettings::IniFormat);
        if (set.status() != QSettings::NoError) {
            return;
        }
        m_cur.dir = QFileInfo(fileName).absolutePath();
        m_cur.name = set.value("SETUP/NAME").toString();
        m_cur.author = set.value("SETUP/AUTHOR").toString();
        m_cur.info = set.value("SETUP/INFO").toString();
        m_cur.type = set.value("SETUP/TYPE").toString().toLower();
        m_cur.files = set.value("SETUP/FILES").toString().trimmed().split(" ",QString::SkipEmptyParts);
        m_cur.open = set.value("SETUP/OPEN").toString().trimmed().split(" ",QString::SkipEmptyParts);
        m_cur.scheme = set.value("SETUP/SCHEME").toString().toLower();
        if (m_cur.open.isEmpty() && m_cur.files.count() > 0) {
            m_cur.open.append(m_cur.files.at(0));
        }
        if (m_cur.type.isEmpty()) {
            m_cur.type = "file";
        }

    }
    if (m_cur.type == "file") {
        ui->typeLabel->setText(tr("New file wizard:"));
    } else {
        ui->typeLabel->setText(tr("New project wizard:"));
    }
    QStringList infos;
    if (!m_cur.name.isEmpty()) {
        infos.append("Name = "+m_cur.name);
    }
    if (!m_cur.author.isEmpty()) {
        infos.append("Author = "+m_cur.author);
    }
    if (!m_cur.info.isEmpty()) {
        infos.append("Info = "+m_cur.info);
    }
    ui->infoLabel->setText(infos.join("\n"));
    QString location;
    bool b = true;
    if (m_cur.type == "gopath") {
        location = QFileInfo(m_gopath,"src").filePath();
        b = false;
    } else if (m_cur.type == "project") {
        location = m_projectLocation;
    } else {
        location = m_fileLocation;
    }
    ui->locationLineEdit->setEnabled(b);
    ui->locationBrowseButton->setEnabled(b);
    ui->locationLineEdit->setText(QDir::toNativeSeparators(location));
    ui->nameLineEdit->clear();
}

bool NewFileDialog::processFile(const QString &infile, const QString &outfile)
{
    QFile in(infile);
    if (!in.open(QIODevice::ReadOnly|QIODevice::Text)) {
        return false;
    }
    QFile out(outfile);
    if (!out.open(QIODevice::WriteOnly|QIODevice::Text)) {
        return false;
    }
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString all = codec->toUnicode(in.readAll());

    QMapIterator<QString,QString> i(m_stringMap);
    while (i.hasNext()) {
        i.next();
        all.replace(i.key(),i.value());
    }

    out.write(codec->fromUnicode(all));

    in.close();
    out.close();
    return true;
}

QStringList NewFileDialog::openFiles() const
{
    return m_openFiles;
}

QString NewFileDialog::type() const
{
    return m_cur.type;
}

QString NewFileDialog::scheme() const
{
    return m_cur.scheme;
}

QString NewFileDialog::openPath() const
{
    return m_openPath;
}

void NewFileDialog::on_locationBrowseButton_clicked()
{
    QString location = ui->locationLineEdit->text();
    location = QFileDialog::getExistingDirectory(this,tr("Browser Location"),location);
    if (location.isEmpty()) {
        return;
    }
    ui->locationLineEdit->setText(location);

    if (m_cur.type == "project") {
        m_projectLocation = location;
    } else if (m_cur.type == "file"){
        m_fileLocation = location;
    }    
}

void NewFileDialog::locationLineChanged(QString)
{
    QString location = ui->locationLineEdit->text();
    if (location.isEmpty()) {
        return;
    }
    if (m_cur.type == "project") {
        m_projectLocation = location;
    } else if (m_cur.type == "file"){
        m_fileLocation = location;
    }
}

void NewFileDialog::nameLineChanged(QString)
{
    if (m_cur.type == "file") {
        return;
    }
    QString name = ui->nameLineEdit->text();
    QString location;
    if (m_cur.type == "gopath") {
        location = QFileInfo(m_gopath,"src").filePath();
    } else if (m_cur.type == "project") {
        location = m_projectLocation;
    } else {
        location = m_fileLocation;
    }
    location = QFileInfo(location,name).absoluteFilePath();
    ui->locationLineEdit->blockSignals(true);
    ui->locationLineEdit->setText(QDir::toNativeSeparators(location));
    ui->locationLineEdit->blockSignals(false);
}
