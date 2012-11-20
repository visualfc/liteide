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
// Module: newfiledialog.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: newfiledialog.h,v 1.0 2011-5-12 visualfc Exp $

#ifndef NEWFILEDIALOG_H
#define NEWFILEDIALOG_H

#include <QDialog>
#include <QModelIndex>

namespace Ui {
    class NewFileDialog;
}

class QStandardItemModel;
class QStringListModel;

class TemplateInfo
{
public:
    void clear()
    {
        name.clear();
        author.clear();
        type.clear();
        info.clear();
        files.clear();
        open.clear();
        scheme.clear();
    }
    bool isValid()
    {
        return !name.isEmpty() && !files.isEmpty();
    }

    QString name;
    QString author;
    QString type;
    QString info;
    QStringList files;
    QStringList open;
    QString scheme;
    QString dir;
};

class NewFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewFileDialog(QWidget *parent = 0);
    ~NewFileDialog();
    void setPathList(const QStringList &pathList);
    void setGopath(const QString &path);
    void setProjectLocation(const QString &path);
    void setFileLocation(const QString &path);
    void setTemplatePath(const QString &path);
    void updateLocation();
    virtual void accept();
    bool processFile(const QString &infile, const QString &outfile);
    QStringList openFiles() const;
    QString type() const;
    QString scheme() const;
    QString openPath() const;
public slots:
    void activePath(QModelIndex);
    void activeCategory(QModelIndex);
    void activeTemplate(QModelIndex);
    void nameLineChanged(QString);
    void locationLineChanged(QString);
private slots:
    void on_locationBrowseButton_clicked();        
private:
    Ui::NewFileDialog *ui;
    QStandardItemModel  *m_categoryModel;
    QStandardItemModel  *m_templateModel;
    QStringListModel    *m_pathModel;
    QString m_projectLocation;
    QString m_fileLocation;
    QString m_gopath;
    QString m_openPath;
    QStringList m_openFiles;
    QMap<QString,QString> m_stringMap;
    TemplateInfo    m_cur;
};

#endif // NEWFILEDIALOG_H
