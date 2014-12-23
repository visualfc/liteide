/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2015 LiteIDE Team. All rights reserved.
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
// Module: liteenvoption.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEENVOPTION_H
#define LITEENVOPTION_H

#include "liteapi/liteapi.h"
#include <QModelIndex>

namespace Ui {
    class LiteEnvOption;
}

class QFileSystemModel;
class LiteEnvOption : public LiteApi::IOption
{
    Q_OBJECT

public:
    explicit LiteEnvOption(LiteApi::IApplication *app, QObject *parent = 0);
    ~LiteEnvOption();
    virtual QWidget *widget();
    virtual QString name() const;
    virtual QString mimeType() const;
    virtual void apply();
public slots:
    void doubleClickedFile(QModelIndex);
private:
    LiteApi::IApplication   *m_liteApp;
    QWidget           *m_widget;
    Ui::LiteEnvOption *ui;
    QFileSystemModel *m_fileModel;
};

#endif // LITEENVOPTION_H
