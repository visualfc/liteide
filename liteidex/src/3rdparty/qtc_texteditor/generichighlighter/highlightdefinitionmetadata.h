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

#ifndef HIGHLIGHTDEFINITIONMETADATA_H
#define HIGHLIGHTDEFINITIONMETADATA_H

#include <QtCore/QString>
#include <QtCore/QLatin1String>
#include <QtCore/QStringList>
#include <QtCore/QUrl>

namespace TextEditor {
namespace Internal {

class HighlightDefinitionMetaData
{
public:
    HighlightDefinitionMetaData();

    void setPriority(const int priority);
    int priority() const;

    void setId(const QString &id);
    const QString &id() const;

    void setName(const QString &name);
    const QString &name() const;

    void setVersion(const QString &version);
    const QString &version() const;

    void setFileName(const QString &fileName);
    const QString &fileName() const;

    void setPatterns(const QStringList &patterns);
    const QStringList &patterns() const;

    void setMimeTypes(const QStringList &mimeTypes);
    const QStringList &mimeTypes() const;

    void setUrl(const QUrl &url);
    const QUrl &url() const;

    static const QLatin1String kPriority;
    static const QLatin1String kName;
    static const QLatin1String kExtensions;
    static const QLatin1String kMimeType;
    static const QLatin1String kVersion;
    static const QLatin1String kUrl;

private:
    int m_priority;
    QString m_id;
    QString m_name;
    QString m_version;
    QString m_fileName;
    QStringList m_patterns;
    QStringList m_mimeTypes;
    QUrl m_url;
};

} // namespace Internal
} // namespace TextEditor

#endif // HIGHLIGHTDEFINITIONMETADATA_H
