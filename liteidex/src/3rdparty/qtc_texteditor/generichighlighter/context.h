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

#ifndef CONTEXT_H
#define CONTEXT_H

#include "includerulesinstruction.h"
#include "reuse.h"
#include "dynamicrule.h"
#include "highlightdefinition.h"
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QSharedPointer>

namespace TextEditor {
namespace Internal {

class Context
{
public:
    Context();
    Context(const Context &context);
    ~Context();
    const Context &operator=(Context copy)
    {
        swap(copy);
        return *this;
    }
    void swap(Context &context)
    {
        qSwap(m_id, context.m_id);
        qSwap(m_name, context.m_name);
        qSwap(m_lineBeginContext, context.m_lineBeginContext);
        qSwap(m_lineEndContext, context.m_lineEndContext);
        qSwap(m_fallthroughContext, context.m_fallthroughContext);
        qSwap(m_itemData, context.m_itemData);
        qSwap(m_fallthrough, context.m_fallthrough);
        qSwap(m_dynamic, context.m_dynamic);
        qSwap(m_rules, context.m_rules);
        qSwap(m_instructions, context.m_instructions);
        qSwap(m_definition, context.m_definition);
    }

    void configureId(const int unique)
    { m_id.append(QString::number(unique)); }

    const QString &id() const
    { return m_id; }

    void setName(const QString &name)
    {
        m_name = name;
        m_id = name;
    }

    const QString &name() const
    { return m_name; }

    void setLineBeginContext(const QString &context)
    { m_lineBeginContext = context; }

    const QString &lineBeginContext() const
    { return m_lineBeginContext; }

    void setLineEndContext(const QString &context)
    { m_lineEndContext = context; }

    const QString &lineEndContext() const
    { return m_lineEndContext; }

    void setFallthroughContext(const QString &context)
    { m_fallthroughContext = context; }

    const QString &fallthroughContext() const
    { return m_fallthroughContext; }

    void setItemData(const QString &itemData)
    { m_itemData = itemData; }

    const QString &itemData() const
    { return m_itemData; }

    void setFallthrough(const QString &fallthrough)
    { m_fallthrough = toBool(fallthrough); }

    bool isFallthrough() const
    { return m_fallthrough; }

    void setDynamic(const QString &dynamic)
    { m_dynamic = toBool(dynamic); }

    bool isDynamic() const
    { return m_dynamic; }

    void updateDynamicRules(const QStringList &captures) const
    {
        TextEditor::Internal::updateDynamicRules(m_rules, captures);
    }

    void addRule(const QSharedPointer<Rule> &rule)
    { m_rules.append(rule); }

    void addRule(const QSharedPointer<Rule> &rule, int index)
    { m_rules.insert(index, rule); }

    const QList<QSharedPointer<Rule> > & rules() const
    { return m_rules; }

    void addIncludeRulesInstruction(const IncludeRulesInstruction &instruction)
    { m_instructions.append(instruction); }

    const QList<IncludeRulesInstruction> &includeRulesInstructions() const
    { return m_instructions; }

    void clearIncludeRulesInstructions()
    { m_instructions.clear(); }

    void setDefinition(const QSharedPointer<HighlightDefinition> &definition)
    { m_definition = definition; }

    const QSharedPointer<HighlightDefinition> &definition() const
    { return m_definition; }

protected:
    QString m_id;
    QString m_name;
    QString m_lineBeginContext;
    QString m_lineEndContext;
    QString m_fallthroughContext;
    QString m_itemData;
    bool m_fallthrough;
    bool m_dynamic;

    QList<QSharedPointer<Rule> > m_rules;
    QList<IncludeRulesInstruction> m_instructions;

    QSharedPointer<HighlightDefinition> m_definition;
};


} // namespace Internal
} // namespace TextEditor

#endif // CONTEXT_H
