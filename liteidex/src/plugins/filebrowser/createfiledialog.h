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
// Module: createfiledialog.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-8-12
// $Id: createfiledialog.h,v 1.0 2011-8-12 visualfc Exp $

#ifndef CREATEFILEDIALOG_H
#define CREATEFILEDIALOG_H

#include <QDialog>

namespace Ui {
    class CreateFileDialog;
}

class CreateFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateFileDialog(QWidget *parent = 0);
    ~CreateFileDialog();
    void setDirectory(const QString &path);
    QString getFileName() const;
    bool isOpenEditor() const { return m_bOpenEditor; }
private slots:
    void on_createButton_clicked();
    void on_editButton_clicked();
    void on_cancelButton_clicked();
private:
    Ui::CreateFileDialog *ui;
    bool    m_bOpenEditor;
};

#endif // CREATEFILEDIALOG_H
