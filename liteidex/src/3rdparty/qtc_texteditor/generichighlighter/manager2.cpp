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

#include "manager2.h"
#include "highlighter.h"
#include "highlightdefinition.h"
#include "highlightdefinitionhandler.h"
#include "highlighterexception.h"
#include "highlightdefinitionmetadata.h"

#include <QDir>
#include <QFileInfo>
#include <QXmlStreamReader>

using namespace TextEditor::Internal;


Manager2 *Manager2::instance()
{
    static Manager2 manager;
    return &manager;
}

QSharedPointer<HighlightDefinitionMetaData> Manager2::parseMetadata(const QFileInfo &fileInfo)
{
    static const QLatin1Char kSemiColon(';');
    static const QLatin1Char kSpace(' ');
    static const QLatin1Char kDash('-');
    static const QLatin1String kLanguage("language");
    static const QLatin1String kArtificial("text/x-artificial-");

    QFile definitionFile(fileInfo.absoluteFilePath());
    if (!definitionFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return QSharedPointer<HighlightDefinitionMetaData>();

    QSharedPointer<HighlightDefinitionMetaData> metaData(new HighlightDefinitionMetaData);

    QXmlStreamReader reader(&definitionFile);
    while (!reader.atEnd() && !reader.hasError()) {
        if (reader.readNext() == QXmlStreamReader::StartElement && reader.name() == kLanguage) {
            const QXmlStreamAttributes &atts = reader.attributes();

            metaData->setFileName(fileInfo.fileName());
            metaData->setId(fileInfo.absoluteFilePath());
            metaData->setName(atts.value(HighlightDefinitionMetaData::kName).toString());
            metaData->setVersion(atts.value(HighlightDefinitionMetaData::kVersion).toString());
            metaData->setPriority(atts.value(HighlightDefinitionMetaData::kPriority).toString()
                                  .toInt());
            metaData->setPatterns(atts.value(HighlightDefinitionMetaData::kExtensions)
                                  .toString().split(kSemiColon, QString::SkipEmptyParts));

            QStringList mimeTypes = atts.value(HighlightDefinitionMetaData::kMimeType).
                                    toString().split(kSemiColon, QString::SkipEmptyParts);
            if (mimeTypes.isEmpty()) {
                // There are definitions which do not specify a MIME type, but specify file
                // patterns. Creating an artificial MIME type is a workaround.
                QString artificialType(kArtificial);
                artificialType.append(metaData->name().trimmed().replace(kSpace, kDash));
                mimeTypes.append(artificialType);
            }
            metaData->setMimeTypes(mimeTypes);

            break;
        }
    }
    reader.clear();
    definitionFile.close();

    return metaData;
}

struct PriorityComp
{
    bool operator()(const QSharedPointer<HighlightDefinitionMetaData> &a,
                    const QSharedPointer<HighlightDefinitionMetaData> &b) {
        return a->priority() > b->priority();
    }
};

void Manager2::loadPath(const QStringList &definitionsPaths)
{
    foreach (const QString &path, definitionsPaths) {
        if (path.isEmpty())
            continue;

        QDir definitionsDir(path);
        QStringList filter(QLatin1String("*.xml"));
        definitionsDir.setNameFilters(filter);

        QList<QSharedPointer<HighlightDefinitionMetaData> > allMetaData;
        const QFileInfoList &filesInfo = definitionsDir.entryInfoList();
        foreach (const QFileInfo &fileInfo, filesInfo) {
            const QSharedPointer<HighlightDefinitionMetaData> &metaData = parseMetadata(fileInfo);
            if (!metaData.isNull())
                allMetaData.append(metaData);
        }

        // Consider definitions with higher priority first.
        qSort(allMetaData.begin(), allMetaData.end(), PriorityComp());

        foreach (const QSharedPointer<HighlightDefinitionMetaData> &metaData, allMetaData) {
            if (m_idByName.contains(metaData->name()))
                // Name already exists... This is a fallback item, do not consider it.
                continue;

            const QString &id = metaData->id();
            m_idByName.insert(metaData->name(), id);
            m_definitionsMetaData.insert(id, metaData);

            foreach (const QString &type, metaData->mimeTypes()) {
                if (m_idByMimeType.contains(type))
                    continue;

                m_idByMimeType.insert(type, id);
            }
//            static const QStringList textPlain(QLatin1String("text/plain"));

//            // A definition can specify multiple MIME types and file extensions/patterns.
//            // However, each thing is done with a single string. There is no direct way to
//            // tell which patterns belong to which MIME types nor whether a MIME type is just
//            // an alias for the other. Currently, I associate all patterns with all MIME
//            // types from a definition.
//            QList<Core::MimeGlobPattern> globPatterns;
//            foreach (const QString &type, metaData->mimeTypes()) {
//                if (m_idByMimeType.contains(type))
//                    continue;

//                m_idByMimeType.insert(type, id);
//                Core::MimeType mimeType = mimeDatabase->findByType(type);
//                if (mimeType.isNull()) {
//                    mimeType.setType(type);
//                    mimeType.setSubClassesOf(textPlain);
//                    mimeType.setComment(metaData->name());

//                    // If there's a user modification for this mime type, we want to use the
//                    // modified patterns and rule-based matchers. If not, just consider what
//                    // is specified in the definition file.
//                    QHash<QString, Core::MimeType>::const_iterator it =
//                            userModified.find(mimeType.type());
//                    if (it == userModified.end()) {
//                        if (globPatterns.isEmpty()) {
//                            foreach (const QString &pattern, metaData->patterns()) {
//                                static const QLatin1String mark("*.");
//                                if (pattern.startsWith(mark)) {
//                                    const QString &suffix = pattern.right(pattern.length() - 2);
//                                    if (!knownSuffixes.contains(suffix))
//                                        knownSuffixes.insert(suffix);
//                                    else
//                                        continue;
//                                }
//                                QRegExp regExp(pattern, Qt::CaseSensitive, QRegExp::Wildcard);
//                                globPatterns.append(Core::MimeGlobPattern(regExp, 50));
//                            }
//                        }
//                        mimeType.setGlobPatterns(globPatterns);
//                    } else {
//                        mimeType.setGlobPatterns(it.value().globPatterns());
//                        mimeType.setMagicRuleMatchers(it.value().magicRuleMatchers());
//                    }

//                    mimeDatabase->addMimeType(mimeType);
//                    future.reportResult(mimeType);
//                }
//            }
        }
    }

}

QString Manager2::definitionIdByName(const QString &name) const
{ return m_idByName.value(name); }

QString Manager2::definitionIdByMimeType(const QString &mimeType) const
{ return m_idByMimeType.value(mimeType); }

QStringList  Manager2::mimeTypes() const
{
    return m_idByMimeType.keys();
}

QString Manager2::definitionIdByAnyMimeType(const QStringList &mimeTypes) const
{
    QString definitionId;
    foreach (const QString &mimeType, mimeTypes) {
        definitionId = definitionIdByMimeType(mimeType);
        if (!definitionId.isEmpty())
            break;
    }
    return definitionId;
}

QSharedPointer<HighlightDefinition> Manager2::definition(const QString &id)
{
    if (!id.isEmpty() && !m_definitions.contains(id)) {
        QFile definitionFile(id);
        if (!definitionFile.open(QIODevice::ReadOnly | QIODevice::Text))
            return QSharedPointer<HighlightDefinition>();

        QSharedPointer<HighlightDefinition> definition(new HighlightDefinition);
        HighlightDefinitionHandler handler(definition);

        QXmlInputSource source(&definitionFile);
        QXmlSimpleReader reader;
        reader.setContentHandler(&handler);
        m_isBuilding.insert(id);
        try {
            reader.parse(source);
        } catch (HighlighterException &) {
            definition.clear();
        }
        m_isBuilding.remove(id);
        definitionFile.close();

        m_definitions.insert(id, definition);
    }

    return m_definitions.value(id);
}

QSharedPointer<HighlightDefinitionMetaData> Manager2::definitionMetaData(const QString &id) const
{ return m_definitionsMetaData.value(id); }

bool Manager2::isBuildingDefinition(const QString &id) const
{ return m_isBuilding.contains(id); }
