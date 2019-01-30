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
// Module: fakevimeditoption.h
// Creator: jsuppe <jon.suppe@gmail.com>

#ifndef FAKEVIMEDITOPTION_H
#define FAKEVIMEDITOPTION_H

#include "liteapi/liteapi.h"

namespace Ui {
    class FakeVimEditOption;
}

class FakeVimEditOption : public LiteApi::IOption
{
    Q_OBJECT

public:
    explicit FakeVimEditOption(LiteApi::IApplication *app, QObject *parent = 0);
    ~FakeVimEditOption();
    virtual QWidget *widget();
    virtual QString name() const;
    virtual QString mimeType() const;
    virtual void apply();
private slots:
    void on_resetCommandsButton_clicked();

private:
    LiteApi::IApplication   *m_liteApp;
    QWidget           *m_widget;
    Ui::FakeVimEditOption *ui;
};

#endif // FAKEVIMEDITOPTION_H
