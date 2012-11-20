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

#include "highlightdefinitionmetadata.h"

using namespace TextEditor;
using namespace Internal;

const QLatin1String HighlightDefinitionMetaData::kPriority("priority");
const QLatin1String HighlightDefinitionMetaData::kName("name");
const QLatin1String HighlightDefinitionMetaData::kExtensions("extensions");
const QLatin1String HighlightDefinitionMetaData::kMimeType("mimetype");
const QLatin1String HighlightDefinitionMetaData::kVersion("version");
const QLatin1String HighlightDefinitionMetaData::kUrl("url");

HighlightDefinitionMetaData::HighlightDefinitionMetaData() : m_priority(0)
{}

void HighlightDefinitionMetaData::setPriority(const int priority)
{ m_priority = priority; }

int HighlightDefinitionMetaData::priority() const
{ return m_priority; }

void HighlightDefinitionMetaData::setId(const QString &id)
{ m_id = id; }

const QString &HighlightDefinitionMetaData::id() const
{ return m_id; }

void HighlightDefinitionMetaData::setName(const QString &name)
{ m_name = name; }

const QString &HighlightDefinitionMetaData::name() const
{ return m_name; }

void HighlightDefinitionMetaData::setVersion(const QString &version)
{ m_version = version; }

const QString &HighlightDefinitionMetaData::version() const
{ return m_version; }

void HighlightDefinitionMetaData::setFileName(const QString &fileName)
{ m_fileName = fileName; }

const QString &HighlightDefinitionMetaData::fileName() const
{ return m_fileName; }

void HighlightDefinitionMetaData::setPatterns(const QStringList &patterns)
{ m_patterns = patterns; }

const QStringList &HighlightDefinitionMetaData::patterns() const
{ return m_patterns; }

void HighlightDefinitionMetaData::setMimeTypes(const QStringList &mimeTypes)
{ m_mimeTypes = mimeTypes; }

const QStringList &HighlightDefinitionMetaData::mimeTypes() const
{ return m_mimeTypes; }

void HighlightDefinitionMetaData::setUrl(const QUrl &url)
{ m_url = url; }

const QUrl &HighlightDefinitionMetaData::url() const
{ return m_url; }
