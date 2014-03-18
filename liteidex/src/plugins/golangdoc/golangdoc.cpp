/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2014 LiteIDE Team. All rights reserved.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** In addition, as a special exception,  that plugins developed for LiteIDE,
** are allowed to remain closed sourced and can be distributed under any license .
** These rights are included in the file LGPL_EXCEPTION.txt in this package.
**
**************************************************************************/
// Module: golangdoc.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golangdoc.h"
#include "litebuildapi/litebuildapi.h"
#include "processex/processex.h"
#include "fileutil/fileutil.h"
#include "htmlutil/htmlutil.h"
#include "golangapi/golangapi.h"
#include "documentbrowser/documentbrowser.h"
#include "qjson/include/QJson/Parser"
#include "finddocwidget.h"
#include "findapiwidget.h"

#include <QListView>
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QToolBar>
#include <QStatusBar>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QFile>
#include <QDir>
#include <QTextBrowser>
#include <QUrl>
#if QT_VERSION >= 0x050000
#include <QUrlQuery>
#endif
#include <QPlainTextEdit>
#include <QTextCursor>
#include <QGroupBox>
#include <QToolButton>
#include <QTextCodec>
#include <QTextCursor>
#include <QDesktopServices>
#include <QDomDocument>
#include <QScrollBar>
#include <QTextBlock>
#include <QToolTip>
#include <QTimer>
#include <QDateTime>
#include <QDebug>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

void ListViewEx::currentChanged(const QModelIndex &current, const QModelIndex&)
{
    emit currentIndexChanged(current);
}


GolangDoc::GolangDoc(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IGolangDoc(parent),
    m_liteApp(app)
{
    m_findProcess = new ProcessEx(this);
    m_godocProcess = new ProcessEx(this);
    //m_goapiProcess = new ProcessEx(this);
    //m_findDocProcess = new ProcessEx(this);
    m_lookupProcess = new ProcessEx(this);
    m_helpProcess = new ProcessEx(this);

    m_lastEditor = 0;
    //m_bApiLoaded = false;

//    m_widget = new QWidget;
//    m_findResultModel = new QStringListModel(this);
//    m_findFilterModel = new QSortFilterProxyModel(this);
//    m_findFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
//    m_findFilterModel->setSourceModel(m_findResultModel);

//    m_findResultListView = new ListViewEx;
//    m_findResultListView->setEditTriggers(0);
//    m_findResultListView->setModel(m_findFilterModel);

//    m_findDocEdit = new Utils::FancyLineEdit;
//    QIcon icon = QIcon::fromTheme(m_findDocEdit->layoutDirection() == Qt::LeftToRight ?
//                     QLatin1String("edit-clear-locationbar-rtl") :
//                     QLatin1String("edit-clear-locationbar-ltr"),
//                     QIcon::fromTheme(QLatin1String("edit-clear"), QIcon(QLatin1String("icon:images/editclear.png"))));

//    m_findDocEdit->setButtonPixmap(Utils::FancyLineEdit::Right, icon.pixmap(16));
//    m_findDocEdit->setButtonVisible(Utils::FancyLineEdit::Right, true);
//    m_findDocEdit->setPlaceholderText(tr("Search"));
//    m_findDocEdit->setButtonToolTip(Utils::FancyLineEdit::Right, tr("Clear Search"));
//    m_findDocEdit->setAutoHideButton(Utils::FancyLineEdit::Right, true);
//    connect(m_findDocEdit, SIGNAL(rightButtonClicked()), m_findDocEdit, SLOT(clear()));

//    m_tagInfo = new QLabel;
//    m_tagInfo->setWordWrap(true);
//    //m_tagInfo->setScaledContents(true);

//    //m_golangApiThread = new GolangApiThread(this);

//    QVBoxLayout *mainLayout = new QVBoxLayout;
//    mainLayout->setMargin(1);
//    mainLayout->setSpacing(1);

//    QHBoxLayout *findLayout = new QHBoxLayout;
//    findLayout->setMargin(2);
//    findLayout->addWidget(new QLabel("Find"));
//    findLayout->addWidget(m_findDocEdit);
//    //findLayout->addWidget(findBtn);

//    mainLayout->addLayout(findLayout);
//    mainLayout->addWidget(m_findResultListView);
//    mainLayout->addWidget(m_tagInfo);
//    m_widget->setLayout(mainLayout);

//    m_rebuildApi = new QAction(tr("Rebuild Api"),this);
//    m_configMenu = new QMenu(tr("Config"),m_widget);
//    m_configMenu->setIcon(QIcon("icon:images/config.png"));
//    m_configMenu->addAction(m_rebuildApi);

    m_findDocWidget = new FindDocWidget(m_liteApp);
    //connect(m_widget,SIGNAL(openApiUrl(QStringList)),this,SLOT(openApiUrl(QStringList)));
    //QList<QAction*> actions;
//    actions << m_configMenu->menuAction();
    m_liteApp->toolWindowManager()->addToolWindow(Qt::LeftDockWidgetArea,m_findDocWidget,"godoc/search",tr("Golang Doc Search"),true);

    m_findApiWidget = new FindApiWidget(m_liteApp);

    m_toolWindowAct = m_liteApp->toolWindowManager()->addToolWindow(Qt::LeftDockWidgetArea,m_findApiWidget,"godoc/api",tr("Golang Api Index"),true);
    connect(m_findApiWidget,SIGNAL(openApiUrl(QStringList)),this,SLOT(openApiUrl(QStringList)));

    m_docBrowser = new DocumentBrowser(m_liteApp,this);
    m_docBrowser->setName(tr("Godoc Search"));

    QStringList paths;
    paths << m_liteApp->resourcePath()+"/golangdoc";
    m_docBrowser->setSearchPaths(paths);

    m_godocFindComboBox = new QComboBox;
    m_godocFindComboBox->setEditable(true);
    m_docBrowser->toolBar()->addSeparator();
    m_docBrowser->toolBar()->addWidget(m_godocFindComboBox);

    m_browserAct = m_liteApp->editorManager()->registerBrowser(m_docBrowser);
    m_liteApp->actionManager()->insertViewMenu(LiteApi::ViewMenuBrowserPos,m_browserAct);

    LiteApi::IActionContext *actionContext = m_liteApp->actionManager()->getActionContext(this,"GoDoc");

    m_findDocAct = new QAction(tr("View Expression Information"),this);
    actionContext->regAction(m_findDocAct,"ViewInfo",QKeySequence::HelpContents);

    m_jumpDeclAct = new QAction(tr("Jump to Declaration"),this);
    actionContext->regAction(m_jumpDeclAct,"JumpToDeclaration","F2");

    connect(m_toolWindowAct,SIGNAL(toggled(bool)),this,SLOT(triggeredToolWindow(bool)));
    connect(m_findDocAct,SIGNAL(triggered()),this,SLOT(editorFindDoc()));
    connect(m_jumpDeclAct,SIGNAL(triggered()),this,SLOT(editorJumpToDecl()));
    //connect(m_golangApiThread,SIGNAL(finished()),this,SLOT(loadApiFinished()));

    connect(m_docBrowser,SIGNAL(requestUrl(QUrl)),this,SLOT(openUrl(QUrl)));
    connect(m_docBrowser,SIGNAL(linkHovered(QUrl)),this,SLOT(highlighted(QUrl)));
    connect(m_docBrowser,SIGNAL(documentLoaded()),this,SLOT(documentLoaded()));
    connect(m_docBrowser,SIGNAL(anchorChanged(QString)),this,SLOT(anchorChanged(QString)));
    connect(m_godocFindComboBox,SIGNAL(activated(QString)),this,SLOT(godocFindPackage(QString)));
    //connect(m_findEdit,SIGNAL(filterChanged(QString)),m_findFilterModel,setFilterFixedString(QString)));
    //connect(m_findEdit,SIGNAL(filterChanged(QString)),this,SLOT(filterTextChanged(QString)));//setFilterFixedString(QString)));
    //connect(m_findDocEdit,SIGNAL(returnPressed()),this,SLOT(findDoc()));
    //connect(m_findEdit,SIGNAL(activated(QString)),this,SLOT(findPackage(QString)));
    connect(m_godocProcess,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(godocOutput(QByteArray,bool)));
    connect(m_godocProcess,SIGNAL(extFinish(bool,int,QString)),this,SLOT(godocFinish(bool,int,QString)));
    //connect(m_findDocProcess,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(findDocOutput(QByteArray,bool)));
    //connect(m_findDocProcess,SIGNAL(extFinish(bool,int,QString)),this,SLOT(findDocFinish(bool,int,QString)));
    connect(m_lookupProcess,SIGNAL(started()),this,SLOT(lookupStarted()));
    connect(m_lookupProcess,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(lookupOutput(QByteArray,bool)));
    connect(m_lookupProcess,SIGNAL(extFinish(bool,int,QString)),this,SLOT(lookupFinish(bool,int,QString)));
    connect(m_helpProcess,SIGNAL(started()),this,SLOT(helpStarted()));
    connect(m_helpProcess,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(helpOutput(QByteArray,bool)));
    connect(m_helpProcess,SIGNAL(extFinish(bool,int,QString)),this,SLOT(helpFinish(bool,int,QString)));
    connect(m_findProcess,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(findOutput(QByteArray,bool)));
    connect(m_findProcess,SIGNAL(extFinish(bool,int,QString)),this,SLOT(findFinish(bool,int,QString)));
//    connect(m_findResultListView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClickListView(QModelIndex)));
//    connect(m_findResultListView,SIGNAL(currentIndexChanged(QModelIndex)),this,SLOT(currentIndexChanged(QModelIndex)));
    //connect(m_findAct,SIGNAL(triggered()),this,SLOT(findPackage()));
    //connect(m_listPkgAct,SIGNAL(triggered()),this,SLOT(listPkg()));
    //connect(m_listCmdAct,SIGNAL(triggered()),this,SLOT(listCmd()));
    connect(m_liteApp->editorManager(),SIGNAL(editorCreated(LiteApi::IEditor*)),this,SLOT(editorCreated(LiteApi::IEditor*)));
    connect(m_liteApp,SIGNAL(loaded()),this,SLOT(appLoaded()));

    m_envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");
    if (m_envManager) {
        connect(m_envManager,SIGNAL(currentEnvChanged(LiteApi::IEnv*)),this,SLOT(currentEnvChanged(LiteApi::IEnv*)));
    }

    this->loadEnv();

    m_liteApp->extension()->addObject("LiteApi.IGolangDoc",this);
    //m_liteApp->extension()->addObject("LiteApi.IGolangApi",m_golangApiThread);

    QString path = m_liteApp->resourcePath()+"/golangdoc/godoc.html";
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        m_templateData = file.readAll();
        file.close();
    }
    QString about = m_liteApp->resourcePath()+"/golangdoc/about.html";
    QFileInfo info(about);
    if(info.exists()) {
        m_templateData.replace("{about}",info.filePath());
    }

    QUrl url;
    url.setScheme("file");
    url.setPath(info.filePath());
    openUrl(url);    
}

GolangDoc::~GolangDoc()
{
    m_liteApp->settings()->setValue("golangdoc/goroot",m_goroot);
    if (m_docBrowser) {
        delete m_docBrowser;
    }
    delete m_findDocWidget;
    delete m_findApiWidget;
}

void GolangDoc::editorJumpToDecl()
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    if (!textEditor) {
        return;
    }
    //m_liteApp->editorManager()->saveEditor(editor,false);
    QString cmd = LiteApi::liteide_stub_cmd(m_liteApp);
    m_srcData = textEditor->utf8Data();
    m_lookupData.clear();
    QFileInfo info(textEditor->filePath());
    m_lookupProcess->setWorkingDirectory(info.path());
    m_lookupProcess->startEx(cmd,QString("api -cursor_std -cursor_info %1:%2 .").
                             arg(info.fileName()).
                             arg(textEditor->utf8Position()));
}

void GolangDoc::editorFindDoc()
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    if (!textEditor) {
        return;
    }
    QPlainTextEdit *ed = LiteApi::getPlainTextEdit(editor);
    if (!ed) {
        return;
    }
    //m_liteApp->editorManager()->saveEditor(editor,false);
    QString cmd = LiteApi::liteide_stub_cmd(m_liteApp);
    m_srcData = textEditor->utf8Data();
    m_lastCursor = ed->textCursor();
    m_lastEditor = editor;
    m_helpData.clear();
    QFileInfo info(textEditor->filePath());
    m_helpProcess->setWorkingDirectory(info.path());
    m_helpProcess->startEx(cmd,QString("api -cursor_std -cursor_info %1:%2 .").
                             arg(info.fileName()).
                             arg(textEditor->utf8Position()));
}

void GolangDoc::editorCreated(LiteApi::IEditor *editor)
{
    if (!editor) {
        return;
    }
    if (editor->mimeType() != "text/x-gosrc") {
        return;
    }
    QMenu *menu = LiteApi::getEditMenu(editor);
    if (menu) {
        menu->addSeparator();
        menu->addAction(m_findDocAct);
        menu->addAction(m_jumpDeclAct);
    }
    menu = LiteApi::getContextMenu(editor);
    if (menu) {
        menu->addSeparator();
        menu->addAction(m_findDocAct);
        menu->addAction(m_jumpDeclAct);
    }
}

//void GolangDoc::loadApi()
//{
//    m_bApiLoaded = true;
//    m_goapiData.clear();

//    QString cmd = LiteApi::liteide_stub_cmd(m_liteApp);
//    bool defctx = m_liteApp->settings()->value("golangapi/defctx",true).toBool();
//    bool std = m_liteApp->settings()->value("golangapi/std",false).toBool();
//    QStringList args;
//    args << "api";
//    if (defctx) {
//        args << "-default_ctx=true";
//    }
//    if (std) {
//        args << "std";
//    } else {
//        args << "all";
//    }
//    m_goapiProcess->startEx(cmd,args.join(" "));
//}

void GolangDoc::currentEnvChanged(LiteApi::IEnv*)
{
    loadEnv();
//    if (!m_bApiLoaded) {
//        return;
//    }
    //loadApi();
}

void GolangDoc::loadEnv()
{    
    QProcessEnvironment env = LiteApi::getGoEnvironment(m_liteApp);//m_envManager->currentEnvironment();
    m_goroot = env.value("GOROOT");

    m_godocCmd = FileUtil::lookupGoBin("godoc",m_liteApp,false);

    m_findProcess->setEnvironment(env.toStringList());
    m_godocProcess->setEnvironment(env.toStringList());
    //m_goapiProcess->setEnvironment(env.toStringList());
    m_lookupProcess->setEnvironment(env.toStringList());
    m_helpProcess->setEnvironment(env.toStringList());

    if (!m_godocCmd.isEmpty()) {
        m_liteApp->appendLog("GolangDoc",QString("found godoc at %1").arg(m_godocCmd),false);
    } else {
        m_liteApp->appendLog("GolangDoc",QString("Could not find godoc, (hint: is godoc installed?)"),true);
    }

    m_pathFileMap.clear();
    QDir dir(m_goroot);
    if (dir.exists() && dir.cd("doc")) {
        foreach(QFileInfo info, dir.entryInfoList(QStringList()<<"*.html",QDir::Files)) {
            QFile f(info.filePath());
            if (f.open(QFile::ReadOnly)) {
                QByteArray line = f.read(1024);
                int start = line.indexOf("<!--");
                if (start == 0 ) {
                    int end = line.indexOf("-->");
                    if (end > start) {
                        QByteArray jsonData = line.mid(start+4,end-start-4);
                        QJson::Parser parser;
                        bool ok = false;
                        QVariant json = parser.parse(jsonData, &ok).toMap();
                        if (ok) {
                            QVariantMap jsonMap = json.toMap();
                            if (jsonMap.contains("Path")) {
                                m_pathFileMap.insert(jsonMap.value("Path").toString(),info.filePath());
                            }
                        }
                    }
                }
            }
        }
    }
}

void GolangDoc::activeBrowser()
{
    m_liteApp->editorManager()->activeBrowser(m_docBrowser);
}

void GolangDoc::rebuildApiData()
{
    if (!m_liteApp->globalCookie().value("golangdoc.goapi.rebuild").toBool()) {
        QFileInfo info(m_findApiWidget->apiDataFile());
        if (!info.exists()) {
            m_findApiWidget->rebuildApiData();
        }
        QDateTime dt = info.lastModified();
        if (dt.toLocalTime().date() < QDate::currentDate()) {
            m_findApiWidget->rebuildApiData();
        }
        m_liteApp->globalCookie().value("golangdoc.goapi.rebuild",true);
    }
}

void GolangDoc::listPkg()
{
    QString cmd = LiteApi::liteide_stub_cmd(m_liteApp);
    QStringList args;
    args << "docview" << "-mode=lite" << "-list=pkg";
    m_findData.clear();
    m_findProcess->start(cmd,args);
}

void GolangDoc::listCmd()
{
    QString cmd = LiteApi::liteide_stub_cmd(m_liteApp);
    QStringList args;
    args << "docview" << "-mode" << "lite" << "-list"<<"cmd";
    m_findData.clear();
    m_findProcess->start(cmd,args);
}

void GolangDoc::godocFindPackage(QString pkgname)
{
    if (pkgname.isEmpty()) {
        pkgname = m_godocFindComboBox->currentText();
    }
    if (pkgname.isEmpty()) {
        return;
    }
    QUrl url;
    url.setScheme("find");
    url.setPath(pkgname);
    openUrl(url);
}

//void GolangDoc::findPackage(QString pkgname)
//{
//    if (pkgname.isEmpty()) {
//        pkgname = m_findDocEdit->text();
//    }
//    if (pkgname.isEmpty()) {
//        return;
//    }
//    QString cmd = LiteApi::liteide_stub_cmd(m_liteApp);
//    QStringList args;
//    args << "docview" << "-mode=lite" << "-find" << pkgname;
//    m_findData.clear();
//    m_findProcess->start(cmd,args);
//}

void GolangDoc::findOutput(QByteArray data,bool bStderr)
{
    if (bStderr) {
        return;
    }
    m_findData.append(data);
}

void GolangDoc::findFinish(bool error,int code,QString /*msg*/)
{
    if (!error && code == 0) {
        QStringList array = QString(m_findData.trimmed()).split(',');
        if (array.size() >= 2 && array.at(0) == "$find") {
            array.removeFirst();
            QString best = array.at(0);
            if (best.isEmpty()) {
                array.removeFirst();
            } else {
                activeBrowser();
                QUrl url;
                url.setScheme("pdoc");
                url.setPath(best);
                openUrl(url);
            }
//            if (array.isEmpty()) {
//                m_findResultModel->setStringList(QStringList() << "<nofind>");
//            } else {
//                m_findResultModel->setStringList(array);
//            }
        }/* else if (array.size() >= 1 && array.at(0) == "$list") {
            //array.removeFirst();
            m_findResultModel->setStringList(array);
        }*/
    } /*else {
        m_findResultModel->setStringList(QStringList() << "<error>");
    }*/
}

void GolangDoc::godocOutput(QByteArray data,bool bStderr)
{
    if (bStderr) {
        QTextCodec *codec = QTextCodec::codecForName("utf8");
        m_liteApp->appendLog("GolangDoc",codec->toUnicode(data),true);
        return;
    }
    m_godocData.append(data);
}

void GolangDoc::godocFinish(bool error,int code,QString /*msg*/)
{
    if (!error && code == 0 && m_docBrowser != 0) {
        bool nav = true;
        QString header;
        if (m_openUrl.scheme() == "list") {
            nav = false;
            header = "Package List";
        } else if (m_openUrl.scheme() == "find") {
            nav = false;
            header = "Find Package "+m_openUrl.path();
        } else if (m_openUrl.scheme() == "pdoc") {
            nav = false;
            header = "Package "+m_openUrl.path();
        }
        updateHtmlDoc(m_openUrl,m_godocData,header,nav);
    }
}

void GolangDoc::updateTextDoc(const QUrl &url, const QByteArray &ba, const QString &header)
{
    m_lastUrl = url;
    QTextCodec *codec = QTextCodec::codecForUtfText(ba,QTextCodec::codecForName("utf-8"));
#if QT_VERSION >= 0x050000
    QString html = codec->toUnicode(ba).toHtmlEscaped();
#else
    QString html = Qt::escape(codec->toUnicode(ba));
#endif
    QString data = m_templateData;
    data.replace("{header}",header);
    data.replace("{nav}","");
    data.replace("{content}",QString("<pre>%1</pre>").arg(html));
    m_docBrowser->setUrlHtml(url,data);

}

void GolangDoc::updateHtmlDoc(const QUrl &url, const QByteArray &ba, const QString &header, bool toNav)
{
    m_lastUrl = url;
    if (m_lastUrl.scheme() == "pdoc") {
        m_targetList.clear();
        QString pkgname = m_lastUrl.path();
        QString goroot = LiteApi::getGoroot(m_liteApp);
        QFileInfo i1(QFileInfo(goroot,"src/cmd").filePath(),pkgname);
        if (i1.exists()) {
            m_targetList.append(i1.filePath());
        }
        QFileInfo i2(QFileInfo(goroot,"src/pkg").filePath(),pkgname);
        if (i2.exists()) {
            m_targetList.append(i2.filePath());
        }
        foreach(QString path,LiteApi::getGopathList(m_liteApp,false)) {
            QFileInfo info(QFileInfo(path,"src").filePath(),pkgname);
            if (info.exists()) {
                m_targetList.append(info.filePath());
            }
        }
    }

    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString genHeader;
    QString nav;

    QString content = HtmlUtil::docToNavdoc(codec->toUnicode(ba),genHeader,nav);
    QString data = m_templateData;

    if (genHeader.isEmpty()) {
        data.replace("{header}",header);
    } else {
        data.replace("{header}",genHeader);
    }
    if (toNav) {
        data.replace("{nav}",nav);
    } else {
        data.replace("{nav}","");
    }
    data.replace("{content}",content);
    data.replace("#pkg-constants","#constants");
    data.replace("#pkg-variables","#variables");
    data.replace("id=\"pkg-constants\"","id=\"constants\"");
    data.replace("id=\"pkg-variables\"","id=\"variables\"");
    m_docBrowser->setUrlHtml(url,data);
}

void GolangDoc::openUrlList(const QUrl &url)
{
    if (url.scheme() != "list") {
        return;
    }
    QString cmd = LiteApi::liteide_stub_cmd(m_liteApp);
    QStringList args;
    args << "docview" << "-mode=html"<< QString("-list=%1").arg(url.path());
    m_godocData.clear();
    m_godocProcess->start(cmd,args);
}

void GolangDoc::openUrlFind(const QUrl &url)
{
    if (url.scheme() != "find") {
        return;
    }
    QString cmd = LiteApi::liteide_stub_cmd(m_liteApp);
    QStringList args;
    args << "docview" << "-mode=html" << "-find" << url.path();
    m_godocData.clear();
    m_godocProcess->start(cmd,args);
    return;
}

void GolangDoc::openUrlPdoc(const QUrl &url)
{
    if (url.scheme() != "pdoc") {
        return;
    }
    if (m_godocCmd.isEmpty()) {
        m_godocCmd = FileUtil::lookupGoBin("godoc",m_liteApp,false);
    }
    if (m_godocCmd.isEmpty()) {
        m_liteApp->appendLog("GolangDoc",QString("not lookup godoc in PATH"),true);
        return;
    }
    if (m_godocProcess->isRunning()) {
        m_godocProcess->waitForFinished(200);
    }
    m_godocData.clear();
    QStringList args;
    //check additional path
    bool local = false;
    if (QDir(url.path()).exists()) {
        local = true;
        if (url.path().length() >= 2 && url.path().right(2) == "..") {
            local = false;
        }
    }

    if (local) {
        m_godocProcess->setWorkingDirectory(url.path());
        args << "-html=true" << ".";
    } else {
        m_godocProcess->setWorkingDirectory(m_goroot);
        args << "-html=true" << url.path();
    }
    m_godocProcess->setEnvironment(LiteApi::getGoEnvironment(m_liteApp).toStringList());
    m_godocProcess->start(m_godocCmd,args);
}

void GolangDoc::openUrlFile(const QUrl &url)
{
    QFileInfo info(url.toLocalFile());
    if (!info.exists()) {
        info.setFile(url.path());
    }
    QString ext = info.suffix().toLower();
    if (ext == "html") {
        QFile file(info.filePath());
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray ba = file.readAll();
            file.close();
            if (info.fileName().compare("docs.html",Qt::CaseInsensitive) == 0) {
                updateHtmlDoc(url,ba,info.fileName(),false);
            } else {
                updateHtmlDoc(url,ba,info.fileName(),true);
            }
        }
    } else if (ext == "go") {
        LiteApi::IEditor *editor = m_liteApp->editorManager()->findEditor(info.filePath(),true);
        if (!editor) {
            editor = m_liteApp->fileManager()->openEditor(info.filePath(),true);
            editor->setReadOnly(true);
        } else {
            m_liteApp->editorManager()->setCurrentEditor(editor);
        }
        if (editor) {
            QPlainTextEdit *ed = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
#if QT_VERSION >= 0x050000
            if (ed && QUrlQuery(url).hasQueryItem("s")) {
                QStringList pos =  QUrlQuery(url).queryItemValue("s").split(":");
#else
            if (ed && url.hasQueryItem("s")) {
                QStringList pos = url.queryItemValue("s").split(":");
#endif
                if (pos.length() == 2) {
                    bool ok = false;
                    int begin = pos.at(0).toInt(&ok);
                    if (ok) {
                        QTextCursor cur = ed->textCursor();
                        cur.setPosition(begin);
                        ed->setTextCursor(cur);
                        ed->centerCursor();
                    }
                }
            }
        }
    } else if (ext == "pdf") {
        QDesktopServices::openUrl(info.filePath());
    } else {
        QFile file(info.filePath());
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray ba = file.readAll();
            updateTextDoc(url,ba,info.fileName());
        }
    }
}

QUrl GolangDoc::parserUrl(const QUrl &_url)
{
    QUrl url = _url;
    if (url.path().isEmpty() && !url.fragment().isEmpty()) {
        return url;
    }
#ifdef Q_OS_WIN
    //fix windows "f:/hg/zmq" -> scheme="f" path="/hg/zmq"
    if (url.scheme().length() == 1) {
        QString path = QDir::fromNativeSeparators(url.toString());
        QFileInfo info(path);
        url.setScheme("");
        url.setPath(path);
        if (info.exists()) {
            if (info.isFile()) {
                url.setScheme("file");
            } else if (info.isDir()) {
                url.setScheme("pdoc");
            }
        }
        return url;
    }
#endif
    if (!url.scheme().isEmpty()) {
        return url;
    }
    if (url.isRelative() && !url.path().isEmpty()) {
        if (m_pathFileMap.contains(url.path())) {
            url.setScheme("file");
            url.setPath(m_pathFileMap.value(url.path()));
        } else if (url.path().compare("/src/pkg/") == 0 || url.path().compare("/pkg/") == 0) {
            url.setScheme("list");
            url.setPath("pkg");
        } else if (url.path().compare("/src/cmd/") == 0  || url.path().compare("/cmd/") == 0){
            url.setScheme("list");
            url.setPath("cmd");
        } else if (url.path().indexOf("/pkg/") == 0) {
            url.setScheme("pdoc");
            if (url.path().at(url.path().length()-1) == '/') {
                url.setPath(url.path().mid(5,url.path().length()-6));
            } else {
                url.setPath(url.path().right(url.path().length()-5));
            }
        } else if (url.path().indexOf("/cmd/") == 0) {
            url.setScheme("pdoc");
            if (url.path().at(url.path().length()-1) == '/') {
                url.setPath(url.path().mid(5,url.path().length()-6));
            } else {
                url.setPath(url.path().right(url.path().length()-5));
            }
        } else if (url.path() == "..") {
            // check ".." is root
            if (m_lastUrl.scheme() == "pdoc") {
                QString path = QDir::cleanPath(m_lastUrl.path()+"/"+url.path());
                if (path != "..") {
                    url.setScheme("pdoc");
                    url.setPath(path);
                }
            }
        } else if (url.path().indexOf("/target/") == 0 && m_lastUrl.scheme() == "pdoc") {
            QString name = url.path().right(url.path().length()-8);
            foreach (QString path, m_targetList) {
                QFileInfo info(path,name);
                if (info.exists()) {
                    url.setScheme("file");
                    url.setPath(info.filePath());
                    break;
                }
            }
        } else {
            QFileInfo info;
            info.setFile(url.path());
            if (!info.exists()) {
                info.setFile(url.toLocalFile());
            }
            if (!info.exists()) {
                QString path = url.path();

                if (path.at(0) == '/') {
                    info.setFile(QDir(m_goroot),path.right(path.length()-1));
                } else if (m_lastUrl.scheme() == "file") {
                    info.setFile(QFileInfo(m_lastUrl.toLocalFile()).absoluteDir(),path);
                }
                if (!info.exists()) {
                    //fix godoc path="f:/hg/zmq/gozmq" "href=/gozmq/gozmq.go"
                    if (m_lastUrl.scheme() == "pdoc") {
                        QDir dir(m_lastUrl.path());
                        if (dir.exists()) {
                            if (path.indexOf(dir.dirName()) == 1) {
                                path.remove(0,dir.dirName().length()+2);
                            }
                            info.setFile(dir,path);
                        }
                    }
                }
            }
            //check index.html
            if (info.exists() && info.isDir()) {
                QFileInfo test(info.dir(),"index.html");
                if (test.exists()) {
                    info = test;
                }
            }

            if (info.exists() && info.isFile()) {
                url.setScheme("file");
                url.setPath(QDir::cleanPath(info.filePath()));
            } else if(url.path().at(url.path().length()-1) != '/') {
                url.setScheme("pdoc");
                if (info.exists()) {
                    url.setPath(info.filePath());
                } else {
                    if (m_lastUrl.scheme() == "pdoc") {
                        url.setPath(QDir::cleanPath(m_lastUrl.path()+"/"+url.path()));
                    } else {
                        url.setPath(url.path());
                    }
                }
            }
        }
    }
    return url;
}

void GolangDoc::highlighted(const QUrl &_url)
{
    QUrl url = parserUrl(_url);
    m_liteApp->mainWindow()->statusBar()->showMessage(url.toString());
//    if (_url.isEmpty()) {
//        m_docBrowser->statusBar()->showMessage(QString("GOROOT=%1").arg(m_goroot));
//    } else {
//        QUrl url = parserUrl(_url);
//        m_docBrowser->statusBar()->showMessage(QString("%1 [%2]").arg(_url.toString()).arg(url.toString()));
//    }
}

void GolangDoc::openUrl(const QUrl &_url)
{ 
    m_liteApp->mainWindow()->statusBar()->clearMessage();
    QUrl url = parserUrl(_url);
    /*
    if (url.scheme() != "file") {
        if (url.path().isEmpty() && !url.fragment().isEmpty()) {
            m_docBrowser->scrollToAnchor(url.fragment());
            return;
        }
        if ( (m_openUrl.scheme() == url.scheme()) &&
             m_openUrl.path() == url.path()) {
            m_docBrowser->scrollToAnchor(url.fragment());
            m_openUrl = url;
            return;
        }
    }
    */
    m_openUrl = url;
    if (url.path().isEmpty() && !url.fragment().isEmpty()) {
        m_docBrowser->scrollToAnchor(url.fragment());
    } else if (url.scheme() == "find") {
        openUrlFind(url);
    } else if (url.scheme() == "pdoc") {
        openUrlPdoc(url);
    } else if (url.scheme() == "list") {
        openUrlList(url);
    } else if (url.scheme() == "file") {
        openUrlFile(url);
    } else {
        QDesktopServices::openUrl(url);
    }
}

//void GolangDoc::findTag(const QString &tag)
//{
//    if (!tag.isEmpty()){
//        QStringList urlList = m_golangApiThread->api()->findDocUrl(tag);
//        if (!urlList.isEmpty()) {
//            if (urlList.size() >= 2) {
//                m_docFind = urlList.at(1);
//            } else {
//                m_docFind.clear();
//            }
//            QString text = urlList.at(0);
//            if (!text.isEmpty()) {
//                activeBrowser();
//                QUrl url(QString("pdoc:%1").arg(text));
//                openUrl(url);
//            }
//        }
//    }
//}

//void GolangDoc::doubleClickListView(QModelIndex index)
//{
//    //TODO
//    if (!index.isValid()) {
//        return;
//    }
//    QModelIndex src =  m_findFilterModel->mapToSource(index);
//    if (!src.isValid()) {
//        return;
//    }
//    QString tag = m_findResultModel->data(src,Qt::DisplayRole).toString();
//    if (!tag.isEmpty()){
//        QStringList urlList;// = m_golangApiThread->api()->findDocUrl(tag);
//        if (!urlList.isEmpty()) {
//            if (urlList.size() >= 2) {
//                m_docFind = urlList.at(1);
//            } else {
//                m_docFind.clear();
//            }
//            QString text = urlList.at(0);
//            if (!text.isEmpty()) {
//                activeBrowser();
//                QUrl url(QString("pdoc:%1").arg(text));
//                openUrl(url);
//            }
//        }
//    }
//}

//void GolangDoc::currentIndexChanged(QModelIndex index)
//{
//    if (!index.isValid()) {
//        return;
//    }
//    QModelIndex src =  m_findFilterModel->mapToSource(index);
//    if (!src.isValid()) {
//        return;
//    }
//    QString tag = m_findResultModel->data(src,Qt::DisplayRole).toString();
//    if (!tag.isEmpty()){
//        //m_tagInfo->setText(m_golangApiThread->api()->findDocInfo(tag));
//    }
//}

void GolangDoc::documentLoaded()
{
}

void GolangDoc::anchorChanged(const QString &/*anchor*/)
{
    if (!m_docFind.isEmpty()) {
        m_docBrowser->htmlWidget()->findText(m_docFind,QTextDocument::FindCaseSensitively|QTextDocument::FindWholeWords);
        m_docFind.clear();
    }
}

void GolangDoc::openApiUrl(QStringList urlList)
{
    if (urlList.isEmpty()) {
        return;
    }
    if (urlList.size() >= 2) {
        m_docFind = urlList.at(1);
    } else {
        m_docFind.clear();
    }
    QString text = urlList.at(0);
    if (!text.isEmpty()) {
        activeBrowser();
        QUrl url(QString("pdoc:%1").arg(text));
        openUrl(url);
    }
}

//void GolangDoc::filterTextChanged(QString str)
//{
//    if (str.isEmpty()) {
//        return;
//    }
//    m_findFilterModel->setFilterFixedString(str);
//    m_findResultListView->verticalScrollBar()->setValue(0);
//    if (m_findFilterModel->rowCount() > 0) {
//        m_findResultListView->setCurrentIndex(m_findFilterModel->index(0,0));
//    }
//}

//void GolangDoc::findDoc()
//{

//}

//void GolangDoc::goapiOutput(QByteArray data,bool bError)
//{
//    if (bError) {
//        return;
//    }
//    m_goapiData.append(data);
//}

//void GolangDoc::goapiFinish(bool error,int code,QString)
//{
//    if (!error && code == 0) {
//        m_liteApp->globalCookie().insert("goalngdoc.goapi.data",m_goapiData);
//        m_golangApiThread->loadData(m_goapiData);
//    }
//}

//void GolangDoc::loadApiFinished()
//{
//    m_findResultModel->setStringList(m_golangApiThread->all());
//}

void GolangDoc::lookupStarted()
{
    m_lookupProcess->write(m_srcData);
    m_lookupProcess->closeWriteChannel();
}

void GolangDoc::lookupOutput(QByteArray data, bool bStdErr)
{
    if (!bStdErr) {
        m_lookupData.append(data);
    }
}

void GolangDoc::lookupFinish(bool error, int code, QString)
{
    if (!error && code == 0) {
        QTextStream s(&m_lookupData);
        while (!s.atEnd()) {
            QString line = s.readLine();
            if (line.startsWith("pos,")) {
                QString info = line.mid(4).trimmed();
                QRegExp reg(":(\\d+):(\\d+)");
                int pos = reg.lastIndexIn(info);
                if (pos >= 0) {
                    QString fileName = info.left(pos);
                    int line = reg.cap(1).toInt();
                    int col = reg.cap(2).toInt();                    
                    LiteApi::gotoLine(m_liteApp,fileName,line-1,col-1);
                }
            }
        }
    }
}

void GolangDoc::helpStarted()
{
    m_helpProcess->write(m_srcData);
    m_helpProcess->closeWriteChannel();
}

void GolangDoc::helpOutput(QByteArray data, bool bStdErr)
{
    if (!bStdErr) {
        m_helpData.append(data);
    }
}

void GolangDoc::helpFinish(bool error, int code, QString)
{
    if (!error && code == 0) {
        QTextStream s(&m_helpData);
        QString info;
        while (!s.atEnd()) {
            QString line = s.readLine();
            if (line.startsWith("help,")) {
                //m_toolAct->setChecked(true);
                QString help = line.mid(5).trimmed();
                //TODO FIXME
                //m_findDocEdit->setText(help);
            } else if (line.startsWith("info,")) {
                info = line.mid(5).trimmed();
            }
        }
        if (!info.isEmpty()) {
            LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
            if (editor && editor == m_lastEditor) {
                QPlainTextEdit *ed = LiteApi::getPlainTextEdit(editor);
                if (ed && ed->textCursor() == m_lastCursor) {
                    QRect rc = ed->cursorRect(m_lastCursor);
                    QPoint pt = ed->mapToGlobal(rc.topRight());
                    QToolTip::showText(pt,info,ed);
                }
            }
        }
    }
}

void GolangDoc::appLoaded()
{
}

void GolangDoc::triggeredToolWindow(bool b)
{
    if (b) {
        rebuildApiData();
    }
}
