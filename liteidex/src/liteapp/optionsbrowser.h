/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2014 LiteIDE Team. All rights reserved.
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
// Module: optionsbrowser.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef OPTIONSBROWSER_H
#define OPTIONSBROWSER_H

#include <QWidget>
#include <QDialog>

#include "liteapi/liteapi.h"

namespace Ui {
    class OptionsWidget;
}

class QListWidgetItem;

class OptionsBrowser : public QDialog//LiteApi::IBrowserEditor
{
    Q_OBJECT
public:
    explicit OptionsBrowser(LiteApi::IApplication *app, QWidget *parent = 0);
    ~OptionsBrowser();
    virtual QString name() const;
    virtual QString mimeType() const;
    void addOption(LiteApi::IOption *opt);
    int execute();
signals:
    void applyOption(QString);
protected slots:
    void itemSelectionChanged();
private slots:
    void applayButton();

private:
    LiteApi::IApplication   *m_liteApp;
    Ui::OptionsWidget *ui;
    QMap<QListWidgetItem*,LiteApi::IOption*>    m_widgetOptionMap;
};

#endif // OPTIONSBROWSER_H
