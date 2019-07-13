/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 visualfc. All rights reserved.
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

    m_findDocWidget = new FindDocWidget(m_liteApp);
    m_docSearchWindowAct = m_liteApp->toolWindowManager()->addToolWindow(Qt::BottomDockWidgetArea,m_findDocWidget,"GoDocSearch",tr("Go Doc Search"),true);

    m_findApiWidget = new FindApiWidget(m_liteApp);

    m_apiSearchWindowAct = m_liteApp->toolWindowManager()->addToolWindow(Qt::BottomDockWidgetArea,m_findApiWidget,"GoApiIndex",tr("Go Api Index"),true);
    connect(m_findApiWidget,SIGNAL(openApiUrl(QStringList)),this,SLOT(openApiUrl(QStringList)));

    m_docBrowser = new DocumentBrowser(m_liteApp,this);
    m_docBrowser->setName(tr("Godoc Search"));

    QStringList paths;
    paths << m_liteApp->resourcePath()+"/packages/go/godoc";
    m_docBrowser->setSearchPaths(paths);

    m_godocFindComboBox = new QComboBox;
    m_godocFindComboBox->setMinimumWidth(100);
    m_godocFindComboBox->setEditable(true);
    //m_godocFindComboBox->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    m_docBrowser->toolBar()->addSeparator();
    m_docBrowser->toolBar()->addWidget(new QLabel(tr("Find Package:")));
    m_docBrowser->toolBar()->addWidget(m_godocFindComboBox);

    m_browserAct = m_liteApp->editorManager()->registerBrowser(m_docBrowser);
    m_liteApp->actionManager()->insertViewMenu(LiteApi::ViewMenuBrowserPos,m_browserAct);

    connect(m_apiSearchWindowAct,SIGNAL(toggled(bool)),this,SLOT(toggledApiSearchWindow(bool)));
    connect(m_docSearchWindowAct,SIGNAL(toggled(bool)),this,SLOT(toggledDocSearchWindow(bool)));

    connect(m_docBrowser,SIGNAL(requestUrl(QUrl)),this,SLOT(openUrl(QUrl)));
    connect(m_docBrowser,SIGNAL(linkHovered(QUrl)),this,SLOT(highlighted(QUrl)));
    connect(m_docBrowser,SIGNAL(documentLoaded()),this,SLOT(documentLoaded()));
    connect(m_docBrowser,SIGNAL(anchorChanged(QString)),this,SLOT(anchorChanged(QString)));
    connect(m_godocFindComboBox,SIGNAL(activated(QString)),this,SLOT(godocFindPackage(QString)));
    connect(m_godocProcess,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(godocOutput(QByteArray,bool)));
    connect(m_godocProcess,SIGNAL(extFinish(bool,int,QString)),this,SLOT(godocFinish(bool,int,QString)));
    connect(m_findProcess,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(findOutput(QByteArray,bool)));
    connect(m_findProcess,SIGNAL(extFinish(bool,int,QString)),this,SLOT(findFinish(bool,int,QString)));
    connect(m_liteApp,SIGNAL(loaded()),this,SLOT(appLoaded()));

    m_envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");
    if (m_envManager) {
        connect(m_envManager,SIGNAL(currentEnvChanged(LiteApi::IEnv*)),this,SLOT(currentEnvChanged(LiteApi::IEnv*)));
    }

    m_liteApp->extension()->addObject("LiteApi.IGolangDoc",this);
    //m_liteApp->extension()->addObject("LiteApi.IGolangApi",m_golangApiThread);

    QString path = m_liteApp->resourcePath()+"/packages/go/godoc/godoc.html";
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        m_templateData = file.readAll();
        file.close();
    }
    QString about = m_liteApp->resourcePath()+"/packages/go/godoc/about.html";
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
//    if (!LiteApi::hasGoEnv(env)) {
//        return;
//    }
//    m_godocCmd = FileUtil::lookupGoBin("godoc",m_liteApp,env,false);

//    m_findProcess->setEnvironment(env.toStringList());
//    m_godocProcess->setEnvironment(env.toStringList());

//    if (!m_godocCmd.isEmpty()) {
//        m_liteApp->appendLog("GolangDoc",QString("Found godoc at %1").arg(m_godocCmd),false);
//    } else {
//        m_liteApp->appendLog("GolangDoc",QString("Could not find godoc, (hint: is godoc installed?)"),true);
//    }

    m_pathFileMap.clear();
    loadGoroot();
}

void GolangDoc::loadGoroot()
{
    m_goroot = LiteApi::getGOROOT(m_liteApp);
    if (m_goroot.isEmpty()) {
        return;
    }
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

//void GolangDoc::rebuildApiData()
//{
//    if (!m_liteApp->globalCookie().value("golangdoc.goapi.rebuild").toBool()) {
//        QFileInfo info(m_findApiWidget->apiDataFile());
//        if (!info.exists()) {
//            m_findApiWidget->rebuildApiData();
//        }
//        QDateTime dt = info.lastModified();
//        if (dt.toLocalTime().date() < QDate::currentDate()) {
//            m_findApiWidget->rebuildApiData();
//        }
//        m_liteApp->globalCookie().value("golangdoc.goapi.rebuild",true);
//    }
//}

void GolangDoc::listPkg()
{
    QString cmd = LiteApi::getGotools(m_liteApp);
    QStringList args;
    args << "docview" << "-mode=lite" << "-list=pkg";
    m_findData.clear();
    m_findProcess->start(cmd,args);
}

void GolangDoc::listCmd()
{
    QString cmd = LiteApi::getGotools(m_liteApp);
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
        m_liteApp->appendLog("GolangDoc",codec->toUnicode(data),false);
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
        if (m_godocData.indexOf("<!--") == 0) {
            updateHtmlDoc(m_openUrl,m_godocData,header,nav);
        } else {
            updateTextDoc(m_openUrl,m_godocData,header);
        }
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
        QString goroot = LiteApi::getGOROOT(m_liteApp);
        QFileInfo i1(QFileInfo(goroot,"src/cmd").filePath(),pkgname);
        if (i1.exists()) {
            m_targetList.append(i1.filePath());
        }
        QFileInfo i2(QFileInfo(goroot,"src/pkg").filePath(),pkgname);
        if (i2.exists()) {
            m_targetList.append(i2.filePath());
        } else {
            QFileInfo i3(QFileInfo(goroot,"src").filePath(),pkgname);
            m_targetList.append(i3.filePath());
        }
        foreach(QString path,LiteApi::getGOPATH(m_liteApp,false)) {
            QFileInfo info(QFileInfo(path,"src").filePath(),pkgname);
            if (info.exists()) {
                m_targetList.append(info.filePath());
            }
        }
    }

    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString genHeader;
    QString nav;

    if (ba.contains("{{if $.GoogleCN}}")) {
    }

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

    //hard code, clean go1.11 docs.html
    if (data.contains("{{if $.GoogleCN}}")) {
        data.replace("{{if $.GoogleCN}}","");
        int i = data.indexOf(" A Tour of Go");
        if (i > 0) {
            data.replace(i,13,"");
        }
        data.replace("{{if not $.GoogleCN}}","");
        data.replace("{{else}}","");
        data.replace("{{end}}","");
    }

    if (!m_openUrlAddin.isNull()) {
        QString realPath = m_openUrlAddin.toString();
        if (!realPath.isEmpty()) {
            data.replace(QString("import \"%1\"").arg(m_lastUrl.path()),QString("import \"%1\"").arg(realPath));
            if (data.contains("import \".\"")) {
                QDir dir(m_lastUrl.path());
                if (dir.exists()) {
                    data.replace(QString("import \".\""),QString("import \"%1\"").arg(realPath));
                }
            }
        }
    }

    m_docBrowser->setUrlHtml(url,data);
}

void GolangDoc::openUrlList(const QUrl &url)
{
    if (url.scheme() != "list") {
        return;
    }
    QString cmd = LiteApi::getGotools(m_liteApp);
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
    QString cmd = LiteApi::getGotools(m_liteApp);
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
//    if (m_godocCmd.isEmpty()) {
//        QProcessEnvironment env = LiteApi::getGoEnvironment(m_liteApp);
//        m_godocCmd = FileUtil::lookupGoBin("godoc",m_liteApp,env,false);
//    }
//    if (m_godocCmd.isEmpty()) {
//        m_liteApp->appendLog("GolangDoc",QString("not lookup godoc in PATH"),true);
//        return;
//    }
    m_godocProcess->stopAndWait(100,2000);
    m_godocData.clear();

    QString godocCmd = LiteApi::getGotools(m_liteApp);
    QStringList args;
    args << "godoc";

    //check additional path
    bool local = false;
    QDir dir(url.path());
    if (dir.exists()) {
        QStringList nameFilter("*.go");
        if (!dir.entryList(nameFilter).isEmpty()) {
            local = true;
        }
    }
    if (local) {
        QStringList gopathList = LiteApi::getGOPATH(m_liteApp,true);
        QStringList pkgList;
        foreach (QString gopath, gopathList) {
            gopath = QDir::fromNativeSeparators(QDir::cleanPath(gopath));
            QString urlpath = QDir::fromNativeSeparators(QDir::cleanPath(url.path()));
            if (urlpath.startsWith(gopath+"/src/")) {
                pkgList << urlpath.mid(gopath.length()+5);
            }
        }
        if (pkgList.size() == 1) {
            m_godocProcess->setWorkingDirectory(m_goroot);
            m_openUrl.setPath(pkgList.at(0));
            args /*<< "-html=true"*/ << pkgList.at(0);
        } else {
            m_godocProcess->setWorkingDirectory(url.path());
            args /*<< "-html=true"*/ << ".";
        }
    } else {
        m_godocProcess->setWorkingDirectory(m_goroot);
        args /*<< "-html=true"*/ << url.path();
    }
    m_godocProcess->setEnvironment(LiteApi::getGoEnvironment(m_liteApp).toStringList());
    m_godocProcess->start(godocCmd,args);
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
    if (m_goroot.isEmpty()) {
        loadGoroot();
    }
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
        } else if (url.path().indexOf("/src/pkg/target/") == 0 && m_lastUrl.scheme() == "pdoc") {
            QString name = url.path().right(url.path().length()-16);
            foreach (QString path, m_targetList) {
                QFileInfo info(path,name);
                if (info.exists()) {
                    url.setScheme("file");
                    url.setPath(info.filePath());
                    break;
                }
            }
        } else if (url.path().indexOf("/src/target/") == 0 && m_lastUrl.scheme() == "pdoc") {
            QString name = url.path().right(url.path().length()-12);
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
                if (path.startsWith("/")) {
                    if (path.endsWith(".go")) {
                        QStringList gopathList = LiteApi::getGOPATH(m_liteApp,true);
                        foreach (QString gopath, gopathList) {
                            QFileInfo _info(gopath+path);
                            if (_info.exists()) {
                                info.setFile(_info.filePath());
                                break;
                            }
                        }
                    } else {
                        info.setFile(QDir(m_goroot),path.right(path.length()-1));
                    }
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
            } else {
                url.setScheme("pdoc");
                if (info.exists()) {
                    url.setPath(info.filePath());
                } else {
                    if (m_lastUrl.scheme() == "pdoc" && url.path().endsWith('/')) {
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

void GolangDoc::openUrl(const QUrl &_url, const QVariant &addin)
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
    m_openUrlAddin = addin;
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

void GolangDoc::appLoaded()
{
}

void GolangDoc::toggledApiSearchWindow(bool /*b*/)
{
//    if (b) {
//        rebuildApiData();
//    }
}

void GolangDoc::toggledDocSearchWindow(bool /*b*/)
{
}
