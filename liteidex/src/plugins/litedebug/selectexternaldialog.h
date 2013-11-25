/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2013 LiteIDE Team. All rights reserved.
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
// Module: selectexternaldialog.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef SELECTEXTERNALDIALOG_H
#define SELECTEXTERNALDIALOG_H

#include <QDialog>
#include "liteapi/liteapi.h"

namespace Ui {
class SelectExternalDialog;
}

class SelectExternalDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SelectExternalDialog(LiteApi::IApplication *app, QWidget *parent = 0);
    ~SelectExternalDialog();
    QString getCmd() const;
    QString getArgs() const;
    QString getWork() const;
    void loadSetting();
    void saveSetting();
private slots:
    void on_cmbPushButton_clicked();
    void on_workPushButton_clicked();
private:
    LiteApi::IApplication *m_liteApp;
    Ui::SelectExternalDialog *ui;
};

#endif // SELECTEXTERNALDIALOG_H
