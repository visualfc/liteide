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
// Module: optionsbrowser.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef OPTIONSBROWSER_H
#define OPTIONSBROWSER_H

#include <QWidget>

#include "liteapi/liteapi.h"

namespace Ui {
    class OptionsWidget;
}

class QListWidgetItem;
class OptionsBrowser : public LiteApi::IBrowserEditor
{
    Q_OBJECT
public:
    explicit OptionsBrowser(LiteApi::IApplication *app, QObject *parent);
    ~OptionsBrowser();
    virtual QWidget *widget();
    virtual QString name() const;
    virtual QString mimeType() const;
    void addOption(LiteApi::IOption *opt);
signals:
    void applyOption(QString);
protected slots:
    void itemSelectionChanged();
private slots:
    void applayButton();

private:
    LiteApi::IApplication   *m_liteApp;
    QWidget *m_widget;
    Ui::OptionsWidget *ui;
    QMap<QListWidgetItem*,LiteApi::IOption*>    m_widgetOptionMap;
};

#endif // OPTIONSBROWSER_H
