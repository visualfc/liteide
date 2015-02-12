/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "searchresultwidget.h"
#include "searchresulttreeview.h"
#include "searchresulttreemodel.h"
#include "searchresulttreeitems.h"
#include "searchresulttreeitemroles.h"

//#include "treeviewfind.h"

//#include <aggregation/aggregate.h>

#include <QDir>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>

static const int SEARCHRESULT_WARNING_LIMIT = 200000;
static const char SIZE_WARNING_ID[] = "sizeWarningLabel";

namespace Find {
namespace Internal {

class WideEnoughLineEdit : public QLineEdit {
    Q_OBJECT
public:
    WideEnoughLineEdit(QWidget *parent):QLineEdit(parent){
        connect(this, SIGNAL(textChanged(QString)),
                this, SLOT(updateGeometry()));
    }
    ~WideEnoughLineEdit(){}
    QSize sizeHint() const {
        QSize sh = QLineEdit::minimumSizeHint();
        sh.rwidth() += qMax(25 * fontMetrics().width(QLatin1Char('x')),
                            fontMetrics().width(text()));
        return sh;
    }
public slots:
    void updateGeometry() { QLineEdit::updateGeometry(); }
};

} // Internal
} // Find

using namespace Find;
using namespace Find::Internal;

SearchResultWidget::SearchResultWidget(QWidget *parent) :
    QWidget(parent),
    m_count(0),
    m_isShowingReplaceUI(false),
    m_searchAgainSupported(false),
    m_preserveCaseSupported(false)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(2);
    layout->setSpacing(1);
    setLayout(layout);

    QFrame *topWidget = new QFrame;
//    QPalette pal = topWidget->palette();
//    pal.setColor(QPalette::Window, QColor(255, 255, 225));
//    pal.setColor(QPalette::WindowText, Qt::black);
//    topWidget->setPalette(pal);
    topWidget->setFrameStyle(QFrame::Panel);// | QFrame::Raised);
    topWidget->setLineWidth(0);
    //topWidget->setAutoFillBackground(true);
    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
    topLayout->setMargin(0);
    topWidget->setLayout(topLayout);
    layout->addWidget(topWidget);

    m_infoWidget = new QFrame;
//    pal = m_infoWidget->palette();
//    pal.setColor(QPalette::Window, QColor(255, 255, 225));
//    pal.setColor(QPalette::WindowText, Qt::black);
//    m_infoWidget->setPalette(pal);
    m_infoWidget->setFrameStyle(QFrame::Panel);// | QFrame::Raised);
    m_infoWidget->setLineWidth(0);
    //m_infoWidget->setAutoFillBackground(true);

    QHBoxLayout *infoLayout = new QHBoxLayout(m_infoWidget);
    infoLayout->setMargin(2);
    m_infoLabel = new QLabel();
    infoLayout->addWidget(m_infoLabel);
    layout->addWidget(m_infoWidget);


    m_messageWidget = new QFrame;
//    pal.setColor(QPalette::Window, QColor(255, 255, 225));
//    pal.setColor(QPalette::WindowText, Qt::red);
//    m_messageWidget->setPalette(pal);
    m_messageWidget->setFrameStyle(QFrame::Panel);// | QFrame::Raised);
    m_messageWidget->setLineWidth(0);
    //m_messageWidget->setAutoFillBackground(true);
    QHBoxLayout *messageLayout = new QHBoxLayout(m_messageWidget);
    messageLayout->setMargin(2);
    m_messageWidget->setLayout(messageLayout);
    QLabel *messageLabel = new QLabel(tr("Search was canceled."));
    //messageLabel->setPalette(pal);
    messageLayout->addWidget(messageLabel);
    layout->addWidget(m_messageWidget);
    m_messageWidget->setVisible(false);

    m_searchResultTreeView = new Internal::SearchResultTreeView(this);
    //m_searchResultTreeView->setFrameStyle(QFrame::NoFrame);
    //m_searchResultTreeView->setAttribute(Qt::WA_MacShowFocusRect, false);
//    Aggregation::Aggregate * agg = new Aggregation::Aggregate;
//    agg->add(m_searchResultTreeView);
//    agg->add(new TreeViewFind(m_searchResultTreeView,
//                              ItemDataRoles::ResultLineRole));
    layout->addWidget(m_searchResultTreeView);

//    m_infoBarDisplay.setTarget(layout, 2);
//    m_infoBarDisplay.setInfoBar(&m_infoBar);

    m_descriptionContainer = new QWidget(topWidget);
    QHBoxLayout *descriptionLayout = new QHBoxLayout(m_descriptionContainer);
    m_descriptionContainer->setLayout(descriptionLayout);
    descriptionLayout->setMargin(0);
    m_descriptionContainer->setMinimumWidth(200);
    m_descriptionContainer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_label = new QLabel(m_descriptionContainer);
    m_label->setVisible(false);
    m_searchTerm = new QLabel(m_descriptionContainer);
    m_searchTerm->setVisible(false);
    descriptionLayout->addWidget(m_label);
    descriptionLayout->addWidget(m_searchTerm);
    m_cancelButton = new QToolButton(topWidget);
    m_cancelButton->setText(tr("Cancel"));
    m_cancelButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    m_searchAgainButton = new QToolButton(topWidget);
    m_searchAgainButton->setToolTip(tr("Repeat the search with same parameters"));
    m_searchAgainButton->setText(tr("Search again"));
    m_searchAgainButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    m_searchAgainButton->setVisible(false);
    connect(m_searchAgainButton, SIGNAL(clicked()), this, SLOT(searchAgain()));

    m_showReplaceModeButton = new QToolButton(topWidget);
    m_showReplaceModeButton->setToolTip(tr("Set show replace mode ui"));
    m_showReplaceModeButton->setText(tr("Show Replace"));
    m_showReplaceModeButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    m_showReplaceModeButton->setVisible(false);
    connect(m_showReplaceModeButton,SIGNAL(clicked()),this,SLOT(showReplaceMode()));

    m_replaceLabel = new QLabel(tr("Replace with:"), topWidget);
    m_replaceTextEdit = new WideEnoughLineEdit(topWidget);
    m_replaceTextEdit->setMinimumWidth(120);
    m_replaceTextEdit->setEnabled(false);
    m_replaceTextEdit->setTabOrder(m_replaceTextEdit, m_searchResultTreeView);
    m_replaceButton = new QToolButton(topWidget);
    m_replaceButton->setToolTip(tr("Replace all occurrences"));
    m_replaceButton->setText(tr("Replace"));
    m_replaceButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    m_replaceButton->setEnabled(false);
    m_preserveCaseCheck = new QCheckBox(topWidget);
    m_preserveCaseCheck->setText(tr("Preserve case"));
    m_preserveCaseCheck->setEnabled(false);

//    if (FindPlugin * plugin = FindPlugin::instance()) {
//        m_preserveCaseCheck->setChecked(plugin->hasFindFlag(FindPreserveCase));
//        connect(m_preserveCaseCheck, SIGNAL(clicked(bool)), plugin, SLOT(setPreserveCase(bool)));
//    }

    m_matchesFoundLabel = new QLabel(topWidget);
    updateMatchesFoundLabel();

    topLayout->addWidget(m_descriptionContainer);
    topLayout->addWidget(m_cancelButton);
    topLayout->addWidget(m_searchAgainButton);
    topLayout->addWidget(m_showReplaceModeButton);
    topLayout->addWidget(m_replaceLabel);
    topLayout->addWidget(m_replaceTextEdit);
    topLayout->addWidget(m_replaceButton);
    topLayout->addWidget(m_preserveCaseCheck);
    topLayout->addStretch(2);
    topLayout->addWidget(m_matchesFoundLabel);
    topWidget->setMinimumHeight(m_cancelButton->sizeHint().height()
                                + topLayout->contentsMargins().top() + topLayout->contentsMargins().bottom()
                                + topWidget->lineWidth());
    setShowReplaceUI(false);

    connect(m_searchResultTreeView, SIGNAL(jumpToSearchResult(SearchResultItem)),
            this, SLOT(handleJumpToSearchResult(SearchResultItem)));
    connect(m_replaceTextEdit, SIGNAL(returnPressed()), this, SLOT(handleReplaceButton()));
    connect(m_replaceButton, SIGNAL(clicked()), this, SLOT(handleReplaceButton()));
}

SearchResultWidget::~SearchResultWidget()
{
//    if (m_infoBar.containsInfo(Core::Id(SIZE_WARNING_ID)))
//        cancelAfterSizeWarning();
}

void SearchResultWidget::setInfo(const QString &label, const QString &toolTip, const QString &term)
{
    m_label->setText(label);
    m_label->setVisible(!label.isEmpty());
    m_descriptionContainer->setToolTip(toolTip);
    m_searchTerm->setText(term);
    m_searchTerm->setVisible(!term.isEmpty());
}

void SearchResultWidget::addResult(const QString &fileName, int lineNumber, const QString &rowText,
    int searchTermStart, int searchTermLength, const QVariant &userData)
{
    SearchResultItem item;
    item.path = QStringList() << QDir::toNativeSeparators(fileName);
    item.lineNumber = lineNumber;
    item.text = rowText;
    item.textMarkPos = searchTermStart;
    item.textMarkLength = searchTermLength;
    item.useTextEditorFont = true;
    item.userData = userData;
    addResults(QList<SearchResultItem>() << item, AddOrdered);
}

void SearchResultWidget::addResults(const QList<SearchResultItem> &items, AddMode mode)
{
    bool firstItems = (m_count == 0);
    m_count += items.size();
    m_searchResultTreeView->addResults(items, mode);
    updateMatchesFoundLabel();
    if (firstItems) {
//        if (!m_dontAskAgainGroup.isEmpty()) {
//            Core::Id undoWarningId = Core::Id("warninglabel/").withSuffix(m_dontAskAgainGroup);
//            if (m_infoBar.canInfoBeAdded(undoWarningId)) {
//                Core::InfoBarEntry info(undoWarningId, tr("This change cannot be undone."),
//                                        Core::InfoBarEntry::GlobalSuppressionEnabled);
//                m_infoBar.addInfo(info);
//            }
//        }

        m_replaceTextEdit->setEnabled(true);
        // We didn't have an item before, set the focus to the search widget or replace text edit
        if (m_isShowingReplaceUI) {
            m_replaceTextEdit->setFocus();
            m_replaceTextEdit->selectAll();
        } else {
            m_searchResultTreeView->setFocus();
        }
        m_searchResultTreeView->selectionModel()->select(m_searchResultTreeView->model()->index(0, 0, QModelIndex()), QItemSelectionModel::Select);
        emit navigateStateChanged();
    } else if (m_count <= SEARCHRESULT_WARNING_LIMIT) {
        return;
    } else {
//        Core::Id sizeWarningId(SIZE_WARNING_ID);
//        if (!m_infoBar.canInfoBeAdded(sizeWarningId))
//            return;
//        emit paused(true);
//        Core::InfoBarEntry info(sizeWarningId,
//                                tr("The search resulted in more than %n items, do you still want to continue?",
//                                0, SEARCHRESULT_WARNING_LIMIT));
//        info.setCancelButtonInfo(tr("Cancel"), this, SLOT(cancelAfterSizeWarning()));
//        info.setCustomButtonInfo(tr("Continue"), this, SLOT(continueAfterSizeWarning()));
//        m_infoBar.addInfo(info);
//        emit requestPopup(false/*no focus*/);
    }
}



int SearchResultWidget::count() const
{
    return m_count;
}

QString SearchResultWidget::dontAskAgainGroup() const
{
    return m_dontAskAgainGroup;
}

void SearchResultWidget::setDontAskAgainGroup(const QString &group)
{
    m_dontAskAgainGroup = group;
}


void SearchResultWidget::setTextToReplace(const QString &textToReplace)
{
    m_replaceTextEdit->setText(textToReplace);
}

QString SearchResultWidget::textToReplace() const
{
    return m_replaceTextEdit->text();
}

void SearchResultWidget::setShowReplaceUI(bool visible)
{
    m_searchResultTreeView->model()->setShowReplaceUI(visible);
    m_replaceLabel->setVisible(visible);
    m_replaceTextEdit->setVisible(visible);
    m_replaceButton->setVisible(visible);
    m_preserveCaseCheck->setVisible(m_preserveCaseSupported && visible);
    m_isShowingReplaceUI = visible;
    m_infoWidget->setVisible(visible);
    m_showReplaceModeButton->setVisible(!visible);
}

void SearchResultWidget::setInfoWidgetLabel(const QString &infoText)
{
    m_infoLabel->setText(infoText);
}

bool SearchResultWidget::hasFocusInternally() const
{
    return m_searchResultTreeView->hasFocus() || (m_isShowingReplaceUI && m_replaceTextEdit->hasFocus());
}

void SearchResultWidget::setFocusInternally()
{
    if (m_count > 0) {
        if (m_isShowingReplaceUI) {
            if (!focusWidget() || focusWidget() == m_replaceTextEdit) {
                m_replaceTextEdit->setFocus();
                m_replaceTextEdit->selectAll();
            } else {
                m_searchResultTreeView->setFocus();
            }
        } else {
            m_searchResultTreeView->setFocus();
        }
    }
}

bool SearchResultWidget::canFocusInternally() const
{
    return m_count > 0;
}

void SearchResultWidget::notifyVisibilityChanged(bool visible)
{
    emit visibilityChanged(visible);
}

void SearchResultWidget::setTextEditorFont(const QFont &font, const SearchResultColor color)
{
    m_searchResultTreeView->setTextEditorFont(font, color);
}

void SearchResultWidget::setAutoExpandResults(bool expand)
{
    m_searchResultTreeView->setAutoExpandResults(expand);
}

void SearchResultWidget::expandAll()
{
    m_searchResultTreeView->expandAll();
}

void SearchResultWidget::collapseAll()
{
    m_searchResultTreeView->collapseAll();
}

void SearchResultWidget::goToNext()
{
    if (m_count == 0)
        return;
    QModelIndex idx = m_searchResultTreeView->model()->next(m_searchResultTreeView->currentIndex());
    if (idx.isValid()) {
        m_searchResultTreeView->setCurrentIndex(idx);
        m_searchResultTreeView->emitJumpToSearchResult(idx);
    }
}

void SearchResultWidget::goToPrevious()
{
    if (!m_searchResultTreeView->model()->rowCount())
        return;
    QModelIndex idx = m_searchResultTreeView->model()->prev(m_searchResultTreeView->currentIndex());
    if (idx.isValid()) {
        m_searchResultTreeView->setCurrentIndex(idx);
        m_searchResultTreeView->emitJumpToSearchResult(idx);
    }
}

void SearchResultWidget::restart()
{
    m_replaceTextEdit->setEnabled(false);
    m_replaceButton->setEnabled(false);
    m_searchResultTreeView->clear();
    m_count = 0;
//    Core::Id sizeWarningId(SIZE_WARNING_ID);
//    m_infoBar.removeInfo(sizeWarningId);
//    m_infoBar.enableInfo(sizeWarningId);
    m_cancelButton->setVisible(true);
    m_searchAgainButton->setVisible(false);
    m_messageWidget->setVisible(false);
    //updateMatchesFoundLabel();
    initMatchesFoundLabel();
    emit restarted();
}

void SearchResultWidget::setSearchAgainSupported(bool supported)
{
    m_searchAgainSupported = supported;
    m_searchAgainButton->setVisible(supported && !m_cancelButton->isVisible());
}

void SearchResultWidget::setSearchAgainEnabled(bool enabled)
{
    m_searchAgainButton->setEnabled(enabled);
}

void SearchResultWidget::setPreserveCaseSupported(bool supported)
{
    m_preserveCaseSupported = supported;
    m_preserveCaseCheck->setVisible(supported);
}

void SearchResultWidget::finishSearch(bool canceled)
{
    this->updateMatchesFoundLabel();
    m_replaceTextEdit->setEnabled(m_count > 0);
    m_replaceButton->setEnabled(m_count > 0);
    m_preserveCaseCheck->setEnabled(m_count > 0);
    m_cancelButton->setVisible(false);
    m_messageWidget->setVisible(canceled);
    m_searchAgainButton->setVisible(m_searchAgainSupported);
}

void SearchResultWidget::sendRequestPopup()
{
    emit requestPopup(true/*focus*/);
}

void SearchResultWidget::continueAfterSizeWarning()
{
    //m_infoBar.suppressInfo(Core::Id(SIZE_WARNING_ID));
    emit paused(false);
}

void SearchResultWidget::cancelAfterSizeWarning()
{
    //m_infoBar.suppressInfo(Core::Id(SIZE_WARNING_ID));
    emit cancelled();
    emit paused(false);
}

void SearchResultWidget::handleJumpToSearchResult(const SearchResultItem &item)
{
    emit activated(item);
}

void SearchResultWidget::handleReplaceButton()
{
    // check if button is actually enabled, because this is also triggered
    // by pressing return in replace line edit
    if (m_replaceButton->isEnabled()) {
        //m_infoBar.clear();
        emit replaceButtonClicked(m_replaceTextEdit->text(), checkedItems(), m_preserveCaseCheck->isChecked());
    }
}

void SearchResultWidget::cancel()
{
    m_cancelButton->setVisible(false);
//    if (m_infoBar.containsInfo(Core::Id(SIZE_WARNING_ID)))
//        cancelAfterSizeWarning();
//    else
        emit cancelled();
}

void SearchResultWidget::searchAgain()
{
    emit searchAgainRequested();
}

void SearchResultWidget::showReplaceMode()
{
    this->setShowReplaceUI(true);
    Internal::SearchResultTreeModel *model = m_searchResultTreeView->model();
    const int fileCount = model->rowCount(QModelIndex());
    for (int i = 0; i < fileCount; ++i) {
        QModelIndex fileIndex = model->index(i, 0, QModelIndex());
        Internal::SearchResultTreeItem *fileItem = static_cast<Internal::SearchResultTreeItem *>(fileIndex.internalPointer());
        fileItem->setIsUserCheckable(true);
        fileItem->setCheckState(Qt::Checked);
        for (int rowIndex = 0; rowIndex < fileItem->childrenCount(); ++rowIndex) {
            QModelIndex textIndex = model->index(rowIndex, 0, fileIndex);
            Internal::SearchResultTreeItem *rowItem = static_cast<Internal::SearchResultTreeItem *>(textIndex.internalPointer());
            rowItem->setIsUserCheckable(true);
            rowItem->setCheckState(Qt::Checked);
        }
    }
    this->m_replaceTextEdit->setFocus();
}

QList<SearchResultItem> SearchResultWidget::checkedItems() const
{
    QList<SearchResultItem> result;
    Internal::SearchResultTreeModel *model = m_searchResultTreeView->model();
    const int fileCount = model->rowCount(QModelIndex());
    for (int i = 0; i < fileCount; ++i) {
        QModelIndex fileIndex = model->index(i, 0, QModelIndex());
        Internal::SearchResultTreeItem *fileItem = static_cast<Internal::SearchResultTreeItem *>(fileIndex.internalPointer());
        Q_ASSERT(fileItem != 0);
        for (int rowIndex = 0; rowIndex < fileItem->childrenCount(); ++rowIndex) {
            QModelIndex textIndex = model->index(rowIndex, 0, fileIndex);
            Internal::SearchResultTreeItem *rowItem = static_cast<Internal::SearchResultTreeItem *>(textIndex.internalPointer());
            if (rowItem->checkState())
                result << rowItem->item;
        }
    }
    return result;
}

void SearchResultWidget::updateMatchesFoundLabel()
{
    if (m_count == 0)
        m_matchesFoundLabel->setText(tr("No matches found."));
    else
        m_matchesFoundLabel->setText(tr("%n matches found.", 0, m_count));
}

void SearchResultWidget::initMatchesFoundLabel()
{
    m_matchesFoundLabel->setText(tr("searching ..."));
}

#include "searchresultwidget.moc"
