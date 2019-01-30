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
// Module: litebuildoption.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEBUILDOPTION_H
#define LITEBUILDOPTION_H

#include "liteapi/liteapi.h"
#include <QModelIndex>

namespace Ui {
    class LiteBuildOption;
}

class QFileSystemModel;
class LiteBuildOption : public LiteApi::IOption
{
    Q_OBJECT

public:
    explicit LiteBuildOption(LiteApi::IApplication *app,QObject *parent = 0);
    ~LiteBuildOption();
    virtual QWidget *widget();
    virtual QString name() const;
    virtual QString mimeType() const;
    virtual void apply();
public slots:
    void doubleClickedFile(QModelIndex);
private:
    LiteApi::IApplication   *m_liteApp;
    QWidget *m_widget;
    Ui::LiteBuildOption *ui;
    QFileSystemModel *m_fileModel;
};

#endif // LITEBUILDOPTION_H
