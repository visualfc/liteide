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
// Module: rotationtoolbutton.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "rotationtoolbutton.h"
#include <QStylePainter>
#include <QStyleOptionToolButton>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

static const int Vertical_Mask = 0x02;

RotationToolButton::RotationToolButton(QWidget *parent) :
    QToolButton(parent), rot(NoRotation)
{
}

void RotationToolButton::setRotation(Rotation rotation)
{
    if (rot != rotation)
    {
        rot = rotation;
        switch (rotation)
        {
        case NoRotation:
        case UpsideDown:
            setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
            break;

        case Clockwise:
        case CounterClockwise:
            setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
            break;
        default:
            break;
        }
        updateGeometry();
        update();
    }
}

RotationToolButton::Rotation RotationToolButton::rotation() const
{
    return rot;
}

QSize RotationToolButton::sizeHint() const
{
    QSize size;
    size = QToolButton::sizeHint();

    if (rot & Vertical_Mask)
        size.transpose();
    return size;
}

QSize RotationToolButton::minimumSizeHint() const
{
    return sizeHint();
}

void RotationToolButton::paintEvent(QPaintEvent*)
{
    QStylePainter painter(this);
    painter.rotate(rot);
    switch (rot)
    {
    case UpsideDown:
        painter.translate(-width(), -height());
        break;

    case Clockwise:
        painter.translate(0, -width());
        break;

    case CounterClockwise:
        painter.translate(-height(), 0);
        break;

    default:
        // nothing to do
        break;
    }

    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    if (rot & Vertical_Mask)
    {
        QSize size = opt.rect.size();
        size.transpose();
        opt.rect.setSize(size);
    }
    painter.drawComplexControl(QStyle::CC_ToolButton, opt);
}
