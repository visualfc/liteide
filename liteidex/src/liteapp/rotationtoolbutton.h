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
// Module: rotationtoolbutton.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-9-12
// $Id: rotationtoolbutton.h,v 1.0 2012-9-12 visualfc Exp $

#ifndef ROTATIONTOOLBUTTON_H
#define ROTATIONTOOLBUTTON_H

#include <QToolButton>

class RotationToolButton : public QToolButton
{
    Q_OBJECT
public:
    enum Rotation {
        NoRotation  = 0,
        UpsideDown  = 180,
        Clockwise  = 90,
        CounterClockwise = 270
    };
    explicit RotationToolButton(QWidget *parent = 0);
    void setRotation(Rotation rotation);
    Rotation rotation() const;
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
protected:
    void paintEvent(QPaintEvent *);
protected:
    Rotation  rot;
};

#endif // ROTATIONTOOLBUTTON_H
