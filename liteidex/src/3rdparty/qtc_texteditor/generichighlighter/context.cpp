/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** No Commercial Usage
**
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**************************************************************************/

#include "context.h"
#include "rule.h"
#include "reuse.h"
#include "dynamicrule.h"
#include "highlightdefinition.h"

using namespace TextEditor;
using namespace Internal;

Context::Context() : m_fallthrough(false), m_dynamic(false)
{}

Context::Context(const Context &context) :
    m_id(context.m_id), m_name(context.m_name), m_lineBeginContext(context.m_lineBeginContext),
    m_lineEndContext(context.m_lineEndContext), m_fallthroughContext(context.m_fallthroughContext),
    m_itemData(context.m_itemData), m_fallthrough(context.m_fallthrough),
    m_dynamic(context.m_dynamic), m_instructions(context.m_instructions),
    m_definition(context.m_definition)
{
    // Rules need to be deeply copied because of dynamic contexts.
    foreach (const QSharedPointer<Rule> &rule, context.m_rules)
        m_rules.append(QSharedPointer<Rule>(rule->clone()));
}


Context::~Context()
{}

