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
// Module: pluginsdialog.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef PLUGINSDIALOG_H
#define PLUGINSDIALOG_H

#include <QDialog>
#include "liteapi/liteapi.h"

namespace Ui {
    class PluginsDialog;
}

class QStandardItemModel;
class QStandardItem;

class PluginManager;
class PluginsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PluginsDialog(LiteApi::IApplication *app, QWidget *parent = 0);
    ~PluginsDialog();
    void appendInfo(const LiteApi::PluginInfo *info);
public slots:
    void itemChanged(QStandardItem*);
private:
    LiteApi::IApplication *m_liteApp;
    Ui::PluginsDialog *ui;
    QStandardItemModel  *m_model;
};

#endif // PLUGINSDIALOG_H
