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

#ifndef MANAGER_H
#define MANAGER_H

#include "highlightdefinitionmetadata.h"

#include <QtCore/QString>
#include <QtCore/QHash>
#include <QtCore/QSet>
#include <QtCore/QUrl>
#include <QtCore/QList>
#include <QtCore/QSharedPointer>
#include <QtCore/QFutureWatcher>
//#include <QtNetwork/QNetworkAccessManager>

QT_BEGIN_NAMESPACE
class QFileInfo;
class QStringList;
class QIODevice;
template <class> class QFutureInterface;
template <class> class QSharedPointer;
QT_END_NAMESPACE

namespace TextEditor {
namespace Internal {

class HighlightDefinition;
class DefinitionDownloader;

// This is the generic highlighter manager. It is not thread-safe.

class Manager : public QObject
{
    Q_OBJECT
public:
    virtual ~Manager();
    static Manager *instance();

    QString definitionIdByName(const QString &name) const;
    QString definitionIdByMimeType(const QString &mimeType) const;
    QString definitionIdByAnyMimeType(const QStringList &mimeTypes) const;

    bool isBuildingDefinition(const QString &id) const;

    QSharedPointer<HighlightDefinition> definition(const QString &id);
    QSharedPointer<HighlightDefinitionMetaData> definitionMetaData(const QString &id) const;

    void downloadAvailableDefinitionsMetaData();
    void downloadDefinitions(const QList<QUrl> &urls, const QString &savePath);
    bool isDownloadingDefinitions() const;

    static QSharedPointer<HighlightDefinitionMetaData> parseMetadata(const QFileInfo &fileInfo);
    void loadFromPath(const QStringList & definitionsPaths);
public slots:
    void registerMimeTypes();

private slots:
    void registerMimeType(int index) const;
    void registerMimeTypesFinished();
    void downloadAvailableDefinitionsListFinished();
    void downloadDefinitionsFinished();

signals:
    void mimeTypesRegistered();

private:
    Manager();
    Q_DISABLE_COPY(Manager)

//    void gatherDefinitionsMimeTypes(QFutureInterface<Core::MimeType> &future);
    QList<HighlightDefinitionMetaData> parseAvailableDefinitionsList(QIODevice *device) const;
    void clear();

    bool m_downloadingDefinitions;
    bool m_registeringMimeTypes;
    int m_queuedMimeTypeRegistrations;

    QHash<QString, QString> m_idByName;
    QHash<QString, QString> m_idByMimeType;
    QHash<QString, QSharedPointer<HighlightDefinition> > m_definitions;
    QHash<QString, QSharedPointer<HighlightDefinitionMetaData> > m_definitionsMetaData;
    QSet<QString> m_isBuilding;

    //QList<DefinitionDownloader *> m_downloaders;
    //QNetworkAccessManager m_networkManager;

    QFutureWatcher<void> m_downloadWatcher;
    //QFutureWatcher<Core::MimeType> m_mimeTypeWatcher;

    struct PriorityComp
    {
        bool operator()(const QSharedPointer<HighlightDefinitionMetaData> &a,
                        const QSharedPointer<HighlightDefinitionMetaData> &b) {
            return a->priority() > b->priority();
        }
    };
signals:
    void definitionsMetaDataReady(const QList<Internal::HighlightDefinitionMetaData>&);
    void errorDownloadingDefinitionsMetaData();
};

} // namespace Internal
} // namespace TextEditor

#endif // MANAGER_H
