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
// Module: setupgopathdialog.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef SETUPGOPATHDIALOG_H
#define SETUPGOPATHDIALOG_H

#include <QDialog>

namespace Ui {
class SetupGopathDialog;
}

class SetupGopathDialog : public QDialog
{
    Q_OBJECT    
public:
    explicit SetupGopathDialog(QWidget *parent = 0);
    ~SetupGopathDialog();
    void setSysPathList(const QStringList &litePathList);
    void setLitePathList(const QStringList &litePathList);
    void setUseSysGopath(bool b);
    bool isUseSysGopath() const;
    void setUseLiteGopath(bool b);
    bool isUseLiteGopath() const;
    void setUseGoModule(bool b);
    bool isUseGoModule() const;
    void setSysGoModuleInfo(const QString &value);
    void setGo111Module(const QString &value);
    QString go111Module() const;
    QStringList litePathList() const;
protected slots:
    void browser();

private:
    Ui::SetupGopathDialog *ui;
};

#endif // SETUPGOPATHDIALOG_H
