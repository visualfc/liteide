#include "boxfolderwindow.h"
#include "symboltreeview/symboltreeview.h"
#include "liteapp_global.h"
#include <QTreeView>
#include <QStackedWidget>
#include <QSplitter>
#include <QDir>
#include <QAction>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QMenu>
#include <QDebug>

BoxFolderWindow::BoxFolderWindow(IApplication *app, QObject *parent)
    : IFolderWindow(parent), m_liteApp(app)
{
    m_spliter = new QSplitter(Qt::Vertical);

    m_tree = new BoxFolderView(app);
    m_tree->setHeaderHidden(true);
    m_tree->setRootIsDecorated(false);

    m_stacked = new QStackedWidget();
    m_spliter->addWidget(m_tree);
    m_spliter->addWidget(m_stacked);
    m_spliter->setStretchFactor(0,0);
    m_spliter->setStretchFactor(1,1);

    QDir::Filters filters = QDir::AllDirs | QDir::Files | QDir::Drives
                            | QDir::Readable| QDir::Writable
                            | QDir::Executable /*| QDir::Hidden*/
                            | QDir::NoDotAndDotDot;

    bool bShowHiddenFiles = m_liteApp->settings()->value(LITEAPP_FOLDERSHOWHIDENFILES,false).toBool();
    if (bShowHiddenFiles) {
        filters |= QDir::Hidden;
    }

    m_filters = filters;

    m_showHideFilesAct = new QAction(tr("Show Hidden Files"),this);
    m_showHideFilesAct->setCheckable(true);
    if (bShowHiddenFiles) {
        m_showHideFilesAct->setChecked(true);
    }
    connect(m_showHideFilesAct,SIGNAL(triggered(bool)),this,SLOT(showHideFiles(bool)));

    m_bShowDetails = m_liteApp->settings()->value(LITEAPP_FOLDERSHOWDETAILS,false).toBool();
    m_showDetailsAct = new QAction(tr("Show Details"),this);
    m_showDetailsAct->setCheckable(true);
    if (m_bShowDetails) {
        m_showDetailsAct->setChecked(true);
    }
    connect(m_showDetailsAct,SIGNAL(triggered(bool)),this,SLOT(setShowDetails(bool)));

    m_syncEditorAct = new QAction(QIcon("icon:images/sync.png"),tr("Synchronize with editor"),this);
    m_syncEditorAct->setCheckable(true);

    QList<QAction*> actions;
    m_filterMenu = new QMenu(tr("Filter"));
    m_filterMenu->setIcon(QIcon("icon:images/filter.png"));
    m_filterMenu->addAction(m_showHideFilesAct);
    m_filterMenu->addAction(m_showDetailsAct);
    actions << m_filterMenu->menuAction() << m_syncEditorAct;

    m_toolWindowAct = m_liteApp->toolWindowManager()->addToolWindow(Qt::LeftDockWidgetArea,m_spliter,"Folders",tr("Folders"),false,actions);

    connect(m_tree,SIGNAL(currentIndexChanged(QModelIndex,QModelIndex)),this,SLOT(currentIndexChanged(QModelIndex,QModelIndex)));
    connect(m_tree,SIGNAL(aboutToShowContextMenu(QMenu*,LiteApi::FILESYSTEM_CONTEXT_FLAG,QFileInfo)),m_liteApp->fileManager(),SIGNAL(aboutToShowFolderContextMenu(QMenu*,LiteApi::FILESYSTEM_CONTEXT_FLAG,QFileInfo)));
    connect(m_tree,SIGNAL(closeFolderIndex(QModelIndex)),this,SLOT(closeFolderIndex(QModelIndex)));
    connect(m_tree,SIGNAL(reloadFolderIndex(QModelIndex)),this,SLOT(reloadFolderIndex(QModelIndex)));

    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));

    connect(m_syncEditorAct,SIGNAL(triggered(bool)),this,SLOT(triggeredSyncEditor(bool)));
    bool b = m_liteApp->settings()->value(LITEAPP_FOLDERSSYNCEDITOR,false).toBool();
    if (b) {
        m_syncEditorAct->setChecked(true);
    }

    QByteArray state = m_liteApp->settings()->value("LiteApp/BoxFolderSplitter").toByteArray();
    m_spliter->restoreState(state);
}

BoxFolderWindow::~BoxFolderWindow()
{
    m_liteApp->settings()->setValue(LITEAPP_FOLDERSSYNCEDITOR,m_syncEditorAct->isChecked());
    m_liteApp->settings()->setValue(LITEAPP_FOLDERSHOWHIDENFILES,m_showHideFilesAct->isChecked());
    m_liteApp->settings()->setValue(LITEAPP_FOLDERSHOWDETAILS,m_showDetailsAct->isChecked());

    m_liteApp->settings()->setValue("LiteApp/BoxFolderSplitter",m_spliter->saveState());

    delete m_filterMenu;

    delete m_spliter;
}

QStringList BoxFolderWindow::folderList() const
{
    return m_folderList;
}

void BoxFolderWindow::setFolderList(const QStringList &folders)
{
    foreach (QString folder, folders) {
       addFolderImpl(folder);
    }
}

void BoxFolderWindow::addFolderList(const QString &folder)
{
    addFolderImpl(folder);
}

void BoxFolderWindow::closeAllFolders()
{
    m_folderList.clear();
    m_tree->clear();
    int count = m_stacked->count();
    while (count) {
        count--;
        QWidget *widget = m_stacked->widget(count);
        m_stacked->removeWidget(widget);
        delete widget;
    }
}

void BoxFolderWindow::currentIndexChanged(const QModelIndex &index, const QModelIndex &/*prev*/)
{
    int row = findInStacked(index);
    if (row == -1) {
        return;
    }
    QString folder = index.data(Qt::UserRole+1).toString();
    if (!QFileInfo(folder).exists()) {
        m_tree->closeFolder();
        return;
    }
    FolderView *widget = (FolderView*)m_stacked->widget(row);
    if (widget->filter() != m_filters) {
        widget->setFilter(m_filters);
    }
    if (widget->isShowDetails() != m_bShowDetails) {
        widget->setShowDetails(m_bShowDetails);
    }
    m_stacked->setCurrentIndex(row);
}

void BoxFolderWindow::closeFolderIndex(const QModelIndex &index)
{
    int row = findInStacked(index);
    if (row == -1) {
        return;
    }
    QWidget *widget = m_stacked->widget(row);
    m_stacked->removeWidget(widget);
    delete widget;
    m_folderList.removeAt(row);
}

void BoxFolderWindow::reloadFolderIndex(const QModelIndex &index)
{
    int row = findInStacked(index);
    if (row == -1) {
        return;
    }
    FolderView *widget = (FolderView*)m_stacked->widget(row);
    widget->reload();
}

void BoxFolderWindow::showHideFiles(bool b)
{
    if (b) {
        m_filters |= QDir::Hidden;
    } else {
        m_filters ^= QDir::Hidden;
    }
    FolderView *widget = (FolderView*)m_stacked->currentWidget();
    if (!widget) {
        return;
    }
    if (widget->filter() != m_filters) {
        widget->setFilter(m_filters);
    }
}

void BoxFolderWindow::setShowDetails(bool b)
{
    m_bShowDetails = b;
    FolderView *widget = (FolderView*)m_stacked->currentWidget();
    if (!widget) {
        return;
    }
    if (widget->isShowDetails() != b) {
        widget->setShowDetails(b);
    }
}

void BoxFolderWindow::currentEditorChanged(IEditor *editor)
{
    if (!m_syncEditorAct->isChecked()) {
        return;
    }
    if (!editor) {
        return;
    }
    QString filePath = editor->filePath();
    if (filePath.isEmpty()) {
        return;
    }
    filePath = QDir::toNativeSeparators(filePath);

    FolderView *widget = (FolderView*)m_stacked->currentWidget();
    if (widget) {
        QModelIndex index = widget->indexForPath(filePath);
        if (index.isValid()) {
            widget->scrollTo(index);
            widget->setCurrentIndex(index);
            return;
        }
    }
    int count = m_folderList.count();
    for (int i = 0; i < count; i++) {
        QString folder = m_folderList.at(i);
        if (!QFileInfo(folder).exists()) {
            continue;
        }
        FolderView *widget = (FolderView*)m_stacked->widget(i);
        QModelIndex index = widget->indexForPath(filePath);
        if (index.isValid()) {
            widget->scrollTo(index);
            widget->setCurrentIndex(index);
            m_tree->setCurrentIndex(m_tree->model()->index(i,0));
            m_stacked->setCurrentIndex(i);
            return;
        }
    }
}

void BoxFolderWindow::triggeredSyncEditor(bool b)
{
    if (b) {
        this->currentEditorChanged(m_liteApp->editorManager()->currentEditor());
    }
}

void BoxFolderWindow::doubleClickedFolderView(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    FolderView *view = (FolderView*)(sender());
    QFileInfo info = view->fileInfo(index);
    if (info.isFile()) {
       m_liteApp->fileManager()->openEditor(info.filePath());
    }
}

void BoxFolderWindow::enterKeyPressedFolderView(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    FolderView *view = (FolderView*)(sender());
    QFileInfo info = view->fileInfo(index);
    if (info.isFile()) {
        m_liteApp->fileManager()->openEditor(info.filePath());
    } else {
        view->setExpanded(index,!view->isExpanded(index));
    }
}

void BoxFolderWindow::addFolderImpl(const QString &_folder)
{
    QString folder = QDir::toNativeSeparators(_folder);
    if (m_folderList.contains(folder)) {
        return;
    }
    if (!QDir(folder).exists()) {
        return;
    }
    FolderView *view = new FolderView(false,m_liteApp);
    view->setFilter(m_filters);
    view->setShowDetails(m_bShowDetails);
    view->setRootPath(folder);
    connect(view,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClickedFolderView(QModelIndex)));
    connect(view,SIGNAL(enterKeyPressed(QModelIndex)),this,SLOT(enterKeyPressedFolderView(QModelIndex)));

    m_stacked->addWidget(view);
    m_folderList.append(folder);
    m_tree->addRootPath(folder);
    m_liteApp->recentManager()->addRecent(folder,"folder");
}

int BoxFolderWindow::findInStacked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return -1;
    }
    QString folder = index.data(Qt::UserRole+1).toString();
    return m_folderList.indexOf(folder);
}


BoxFolderView::BoxFolderView(IApplication *app, QWidget *parent)
    : BaseFolderView(app,parent)
{
    m_model = new QStandardItemModel(this);
    this->setModel(m_model);
    m_contextMenu = new QMenu();

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenuRequested(QPoint)));
}

void BoxFolderView::addRootPath(const QString &folder)
{
    QStandardItem *item = new QStandardItem(folder);
    item->setData(folder,Qt::UserRole+1);
    item->setToolTip(folder);

    m_model->appendRow(item);
    this->setCurrentIndex(m_model->indexFromItem(item));
}

void BoxFolderView::clear()
{
    m_model->clear();
}

void BoxFolderView::openFolder()
{
    m_liteApp->fileManager()->openFolder();
}

void BoxFolderView::closeFolder()
{
    QModelIndex index = this->currentIndex();
    if (!index.isValid()) {
        return;
    }
    emit closeFolderIndex(index);
    m_model->removeRow(index.row());
}

void BoxFolderView::reloadFolder()
{
    QModelIndex index = this->currentIndex();
    if (!index.isValid()) {
        return;
    }
    emit reloadFolderIndex(index);
}

void BoxFolderView::customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = this->currentIndex();
    if (!index.isValid()) {
        return;
    }
    QString dir = index.data(Qt::UserRole+1).toString();
    QFileInfo fileInfo(dir);

    m_contextMenu->clear();
    m_contextInfo = fileInfo;

    LiteApi::FILESYSTEM_CONTEXT_FLAG flag = LiteApi::FILESYSTEM_ROOTFOLDER;
    bool hasGo = false;
    foreach(QFileInfo info, QDir(dir).entryInfoList(QDir::Files)) {
        if (info.suffix() == "go") {
            hasGo = true;
        }
    }
    m_contextMenu->addAction(m_openInNewWindowAct);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_newFileAct);
    m_contextMenu->addAction(m_newFileWizardAct);
    m_contextMenu->addAction(m_newFolderAct);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_openFolderAct);
    m_contextMenu->addAction(m_reloadFolderAct);
    m_contextMenu->addAction(m_closeFolderAct);
    m_contextMenu->addSeparator();
    if (hasGo) {
        m_contextMenu->addAction(m_viewGodocAct);
        m_contextMenu->addSeparator();
    }
    m_contextMenu->addAction(m_openExplorerAct);
    m_contextMenu->addAction(m_openShellAct);
    emit aboutToShowContextMenu(m_contextMenu,flag,m_contextInfo);
    m_contextMenu->exec(this->mapToGlobal(pos));
}
