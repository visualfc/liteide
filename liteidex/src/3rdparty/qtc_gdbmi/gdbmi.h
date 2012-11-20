/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (info@qt.nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at info@qt.nokia.com.
**
**************************************************************************/

#ifndef GDBMI_H
#define GDBMI_H

#include <QByteArray>
#include <QList>
#include <QVariant>

class GdbMiValue
{
public:
    GdbMiValue() : m_type(Invalid) {}

    QByteArray m_name;
    QByteArray m_data;
    QList<GdbMiValue> m_children;

    enum Type {
        Invalid,
        Const,
        Tuple,
        List
    };

    Type m_type;

    inline Type type() const { return m_type; }
    inline QByteArray name() const { return m_name; }
    inline bool hasName(const char *name) const { return m_name == name; }

    inline bool isValid() const { return m_type != Invalid; }
    inline bool isConst() const { return m_type == Const; }
    inline bool isTuple() const { return m_type == Tuple; }
    inline bool isList() const { return m_type == List; }


    inline QByteArray data() const { return m_data; }
    inline const QList<GdbMiValue> &children() const { return m_children; }
    inline int childCount() const { return m_children.size(); }

    const GdbMiValue &childAt(int index) const { return m_children[index]; }
    GdbMiValue &childAt(int index) { return m_children[index]; }
    GdbMiValue findChild(const char *name) const;

    QByteArray toString(bool multiline = false, int indent = 0) const;
    qulonglong toAddress() const;
    void fromString(const QByteArray &str);
    void fromStringMultiple(const QByteArray &str);

public:
    static QByteArray parseCString(const char *&from, const char *to);
    static QByteArray escapeCString(const QByteArray &ba);
    static QString escapeCString(const QString &ba);
    void parseResultOrValue(const char *&from, const char *to);
    void parseValue(const char *&from, const char *to);
    void parseTuple(const char *&from, const char *to);
    void parseTuple_helper(const char *&from, const char *to);
    void parseList(const char *&from, const char *to);

    void dumpChildren(QByteArray *str, bool multiline, int indent) const;
};

enum GdbResultClass
{
    // "done" | "running" | "connected" | "error" | "exit"
    GdbResultUnknown,
    GdbResultDone,
    GdbResultRunning,
    GdbResultConnected,
    GdbResultError,
    GdbResultExit
};

class GdbResponse
{
public:
    GdbResponse() : token(-1), resultClass(GdbResultUnknown) {}
    QByteArray toString() const;
    static QByteArray stringFromResultClass(GdbResultClass resultClass);

    int            token;
    GdbResultClass resultClass;
    GdbMiValue     data;
    QVariant       cookie;
    QByteArray     logStreamOutput;
    QByteArray     consoleStreamOutput;
};


#endif // GDBMI_H
