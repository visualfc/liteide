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

#include "managedefinitionsdialog.h"
#include "manager.h"

#include <QtCore/QUrl>
#include <QtCore/QIODevice>
#include <QtCore/QXmlStreamReader>
#include <QtCore/QXmlStreamAttributes>
#include <QtCore/QFuture>
#include <QtCore/QFutureWatcher>
#include <QtCore/QtConcurrentMap>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtGui/QMessageBox>

#include <QtCore/QDebug>

using namespace TextEditor;
using namespace Internal;

ManageDefinitionsDialog::ManageDefinitionsDialog(
        const QList<HighlightDefinitionMetaData> &metaDataList,
        const QString &path,
        QWidget *parent) :
    QDialog(parent),
    m_definitionsMetaData(metaDataList),
    m_path(path)
{
    ui.setupUi(this);    
    ui.definitionsTable->setHorizontalHeaderLabels(
        QStringList() << tr("Name") << tr("Installed") << tr("Available"));
    ui.definitionsTable->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);

    setWindowTitle(tr("Download Definitions"));

    populateDefinitionsWidget();

    connect(ui.downloadButton, SIGNAL(clicked()), this, SLOT(downloadDefinitions()));
    connect(ui.allButton, SIGNAL(clicked()), this, SLOT(selectAll()));
    connect(ui.clearButton, SIGNAL(clicked()), this, SLOT(clearSelection()));
    connect(ui.invertButton, SIGNAL(clicked()), this, SLOT(invertSelection()));
}

void ManageDefinitionsDialog::populateDefinitionsWidget()
{
    const int size = m_definitionsMetaData.size();
    ui.definitionsTable->setRowCount(size);
    for (int i = 0; i < size; ++i) {
        const HighlightDefinitionMetaData &downloadData = m_definitionsMetaData.at(i);

        // Look for this definition in the current path specified by the user, not the one
        // stored in the settings. So the manager should not be queried for this information.
        QString dirVersion;
        QFileInfo fi(m_path + downloadData.fileName());
        QFile definitionFile(fi.absoluteFilePath());
        if (definitionFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            const QSharedPointer<HighlightDefinitionMetaData> &data = Manager::parseMetadata(fi);
            if (!data.isNull())
                dirVersion = data->version();
        }

        for (int j = 0; j < 3; ++j) {
            QTableWidgetItem *item = new QTableWidgetItem;
            if (j == 0)
                item->setText(downloadData.name());
            else if (j == 1) {
                item->setText(dirVersion);
                item->setTextAlignment(Qt::AlignCenter);
            } else if (j == 2) {
                item->setText(downloadData.version());
                item->setTextAlignment(Qt::AlignCenter);
            }
            ui.definitionsTable->setItem(i, j, item);
        }
    }
}

void ManageDefinitionsDialog::downloadDefinitions()
{
    if (Manager::instance()->isDownloadingDefinitions()) {
        QMessageBox::information(
            this,
            tr("Download Information"),
            tr("There is already one download in progress. Please wait until it is finished."));
        return;
    }

    QList<QUrl> urls;
    foreach (const QModelIndex &index, ui.definitionsTable->selectionModel()->selectedRows())
        urls.append(m_definitionsMetaData.at(index.row()).url());
    Manager::instance()->downloadDefinitions(urls, m_path);
    accept();
}

void ManageDefinitionsDialog::selectAll()
{
    ui.definitionsTable->selectAll();
    ui.definitionsTable->setFocus();
}

void ManageDefinitionsDialog::clearSelection()
{
    ui.definitionsTable->clearSelection();
}

void ManageDefinitionsDialog::invertSelection()
{
    const QModelIndex &topLeft = ui.definitionsTable->model()->index(0, 0);
    const QModelIndex &bottomRight =
        ui.definitionsTable->model()->index(ui.definitionsTable->rowCount() - 1,
                                            ui.definitionsTable->columnCount() - 1);
    QItemSelection itemSelection(topLeft, bottomRight);
    ui.definitionsTable->selectionModel()->select(itemSelection, QItemSelectionModel::Toggle);
    ui.definitionsTable->setFocus();
}

void ManageDefinitionsDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui.retranslateUi(this);
        break;
    default:
        break;
    }
}
