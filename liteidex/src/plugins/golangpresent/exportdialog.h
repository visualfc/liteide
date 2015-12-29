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
// Module: exportdialog.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>

namespace Ui {
class ExportDialog;
}

class ExportDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ExportDialog(QWidget *parent = 0);
    ~ExportDialog();
    QString fileName() const;
    void setFileName(const QString &text);
    bool isExportAndView() const { return m_bView; }
private slots:
    void on_exportButton_clicked();
    void on_exportAndViewButton_clicked();
private:
    Ui::ExportDialog *ui;
    bool m_bView;
};

#endif // EXPORTDIALOG_H
