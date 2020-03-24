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
// Module: golangpackageoption.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef SETUPGOPATHDIALOG_H
#define SETUPGOPATHDIALOG_H

#include <QDialog>
#include "liteapi/liteapi.h"

namespace Ui {
class GolangPackageOption;
}

enum ENUM_GO_PRIVATE{
    GO_PRIVATE = 0,
    GO_NOPROXY = 1,
    GO_NOSUMDB = 2,
};

class GolangPackageOption : public LiteApi::IOption
{
    Q_OBJECT    
public:
    explicit GolangPackageOption(LiteApi::IApplication *app, QObject *parent = 0);
    ~GolangPackageOption();
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
    void setUseGoProxy(bool b);
    bool isUseGoProxy() const;
    void setGoProxy(const QString &v);
    QString goProxy() const;
    void setUseGoPrivate(ENUM_GO_PRIVATE id, bool b);
    bool isUseGoPrivate(ENUM_GO_PRIVATE id) const;
    void setGoPrivate(ENUM_GO_PRIVATE id, const QString &value);
    QString goPrivate(ENUM_GO_PRIVATE id) const;
public:
    virtual QString name() const;
    virtual QString mimeType() const;
    virtual void save();
    virtual void load();
    virtual QWidget *widget();
protected slots:
    void browser();

private:
    LiteApi::IApplication *m_liteApp;
    Ui::GolangPackageOption *ui;
    QWidget *m_widget;
};

#endif // SETUPGOPATHDIALOG_H
