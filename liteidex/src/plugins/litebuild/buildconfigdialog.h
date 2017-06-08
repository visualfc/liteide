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
// Module: buildconfigdialog.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef BUILDCONFIGDIALOG_H
#define BUILDCONFIGDIALOG_H

#include <QDialog>
#include <QModelIndex>
#include "liteapi/liteapi.h"
#include "litebuildapi/litebuildapi.h"

namespace Ui {
    class BuildConfigDialog;
}

class QAbstractItemModel;
class QTableView;
class QStandardItemModel;
class BuildConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BuildConfigDialog(LiteApi::IApplication *app, QWidget *parent = 0);
    virtual ~BuildConfigDialog();
    void setBuild(LiteApi::IBuild *build, const QString &buildPath, const QMap<QString,QString> &liteEnvMap);
    void saveCustomGopath();
    void saveCustomModel();
    void updateBuildConfigHelp(LiteApi::IBuild *build, const QString &buildRootPath, const QMap<QString,QString> &liteEnvMap, QStandardItemModel *liteideModel, QStandardItemModel *configModel, QStandardItemModel *customModel, QStandardItemModel *actionModel);
public slots:
    void editCustomeTabView(QModelIndex);
protected:
    void resizeTableView(QTableView *tableView);
private slots:
    void on_customGopathBrowserButton_clicked();

    void on_customGopathClear_clicked();

    void on_customResetAllButton_clicked();

private:
    LiteApi::IApplication *m_liteApp;
    Ui::BuildConfigDialog *ui;
    QString                m_buildPath;
    QStandardItemModel *m_liteideModel;
    QStandardItemModel *m_configModel;
    QStandardItemModel *m_customModel;
    QStandardItemModel *m_actionModel;
};

#endif // BUILDCONFIGDIALOG_H
