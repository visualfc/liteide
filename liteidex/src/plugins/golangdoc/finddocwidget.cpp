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
// Module: finddocwidget.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "finddocwidget.h"
#include "liteenvapi/liteenvapi.h"
#include "golangdoc_global.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QActionGroup>
#include <QAction>
#include <QCoreApplication>
#include <QPlainTextEdit>
#include <QTextBrowser>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

static char help[] =
"<b>Search Format</b>"
"<pre>"
"fmt.\n"
"    Extracts all fmt pkg symbol document\n"
"fmt.Println or fmt Println\n"
"    Extracts fmt.Println document\n"
"fmt.Print or fmt Print\n"
"    Extracts fmt.Print fmt.Printf fmt.Println etc\n"
"Println\n"
"    Extracts fmt.Println log.Println log.Logger.Println etc"
"</pre>"
"<b>Search Option</b>"
"<pre>"
"Match Word.\n"
"    Match whole world only\n"
"Match Case\n"
"    Match case sensitive\n"
"Use Regexp\n"
"    Use regexp for search\n"
"    example fmt p.*\n"
"</pre>"
;

class SearchEdit : public Utils::FancyLineEdit
{
    Q_DECLARE_TR_FUNCTIONS(SearchEdit)
public:
    SearchEdit(QWidget *parent = 0)
        : Utils::FancyLineEdit(parent)
    {
        QIcon icon = QIcon::fromTheme(layoutDirection() == Qt::LeftToRight ?
                         QLatin1String("edit-clear-locationbar-rtl") :
                         QLatin1String("edit-clear-locationbar-ltr"),
                         QIcon::fromTheme(QLatin1String("edit-clear"), QIcon(QLatin1String("icon:images/editclear.png"))));

        setButtonPixmap(Right, icon.pixmap(16));
        setPlaceholderText(tr("Search"));
        setButtonToolTip(Right, tr("Stop Search"));
    }
    void showStopButton(bool b)
    {
        this->setButtonVisible(Right,b);
    }
};

FindDocWidget::FindDocWidget(LiteApi::IApplication *app, QWidget *parent) :
    QWidget(parent), m_liteApp(app)
{
    m_findEdit = new SearchEdit;
    m_findEdit->setPlaceholderText(tr("Search"));

    m_chaseWidget = new ChaseWidget;
    m_chaseWidget->setMinimumSize(QSize(16,16));
    m_chaseWidget->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);

    QToolButton *findBtn = new QToolButton;
    findBtn->setPopupMode(QToolButton::InstantPopup);
    findBtn->setText(tr("Find"));

    QHBoxLayout *findLayout = new QHBoxLayout;
    findLayout->setMargin(2);
    findLayout->addWidget(m_findEdit);
    findLayout->addWidget(findBtn);
    findLayout->addWidget(m_chaseWidget);

    m_browser = m_liteApp->htmlWidgetManager()->createByName(this,"QTextBrowser");
    QStringList paths;
    paths << m_liteApp->resourcePath()+"/packages/go/godoc";
    m_browser->setSearchPaths(paths);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(1);
    mainLayout->setSpacing(1);
    mainLayout->addLayout(findLayout);
    mainLayout->addWidget(m_browser->widget());

    QAction *findAll = new QAction(tr("Find All"),this);
    QAction *findConst = new QAction(tr("Find const"),this);
    findConst->setData("const");
    QAction *findFunc = new QAction(tr("Find func"),this);
    findFunc->setData("func");
    QAction *findInterface = new QAction(tr("Find interface"),this);
    findInterface->setData("interface");
    QAction *findPkg = new QAction(tr("Find pkg"),this);
    findPkg->setData("pkg");
    QAction *findStruct = new QAction(tr("Find struct"),this);
    findStruct->setData("struct");
    QAction *findType = new QAction(tr("Find type"),this);
    findType->setData("type");
    QAction *findVar = new QAction(tr("Find var"),this);
    findVar->setData("var");
    m_useRegexpCheckAct = new QAction(tr("Use Regexp"),this);
    m_useRegexpCheckAct->setCheckable(true);
    m_matchCaseCheckAct = new QAction(tr("Match Case"),this);
    m_matchCaseCheckAct->setCheckable(true);
    m_matchWordCheckAct = new QAction(tr("Match Word"),this);
    m_matchWordCheckAct->setCheckable(true);

    m_useRegexpCheckAct->setChecked(m_liteApp->settings()->value(GODOCFIND_USEREGEXP,false).toBool());
    m_matchCaseCheckAct->setChecked(m_liteApp->settings()->value(GODOCFIND_MATCHCASE,true).toBool());
    m_matchWordCheckAct->setChecked(m_liteApp->settings()->value(GODOCFIND_MATCHWORD,false).toBool());

    QMenu *menu = new QMenu(findBtn);
    menu->addActions(QList<QAction*>()
                     << findAll
                     //<< findPkg
                     );
    menu->addSeparator();
    menu->addActions(QList<QAction*>()
                     << findInterface
                     << findStruct
                     << findType
                     << findFunc
                     << findConst
                     << findVar
                     );
    menu->addSeparator();
    menu->addAction(m_matchWordCheckAct);
    menu->addAction(m_matchCaseCheckAct);
    menu->addAction(m_useRegexpCheckAct);
    findBtn->setMenu(menu);

    QAction *helpAct = new QAction(tr("Help"),this);
    menu->addSeparator();
    menu->addAction(helpAct);
    connect(helpAct,SIGNAL(triggered()),this,SLOT(showHelp()));

    this->setLayout(mainLayout);    


    connect(findAll,SIGNAL(triggered()),this,SLOT(findDoc()));
    connect(findConst,SIGNAL(triggered()),this,SLOT(findDoc()));
    connect(findFunc,SIGNAL(triggered()),this,SLOT(findDoc()));
    connect(findInterface,SIGNAL(triggered()),this,SLOT(findDoc()));
    connect(findPkg,SIGNAL(triggered()),this,SLOT(findDoc()));
    connect(findStruct,SIGNAL(triggered()),this,SLOT(findDoc()));
    connect(findType,SIGNAL(triggered()),this,SLOT(findDoc()));
    connect(findVar,SIGNAL(triggered()),this,SLOT(findDoc()));

    m_process = new ProcessEx(this);
    connect(m_process,SIGNAL(stateChanged(QProcess::ProcessState)),this,SLOT(stateChanged(QProcess::ProcessState)));
    connect(m_process,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(extOutput(QByteArray,bool)));
    connect(m_process,SIGNAL(extFinish(bool,int,QString)),this,SLOT(extFinish(bool,int,QString)));
    connect(m_findEdit,SIGNAL(returnPressed()),findAll,SIGNAL(triggered()));
    connect(m_findEdit,SIGNAL(rightButtonClicked()),this,SLOT(abortFind()));
    connect(m_browser,SIGNAL(linkClicked(QUrl)),this,SLOT(openUrl(QUrl)));


    QString path = m_liteApp->resourcePath()+"/packages/go/godoc/finddoc.html";
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        m_templateData = file.readAll();
        file.close();
    }

    //QFont font = m_browser->widget()->font();
    //font.setPointSize(12);
    //m_browser->widget()->setFont(font);

    showHelp();
}

FindDocWidget::~FindDocWidget()
{
    m_liteApp->settings()->setValue(GODOCFIND_MATCHCASE,m_matchCaseCheckAct->isChecked());
    m_liteApp->settings()->setValue(GODOCFIND_MATCHWORD,m_matchWordCheckAct->isChecked());
    m_liteApp->settings()->setValue(GODOCFIND_USEREGEXP,m_useRegexpCheckAct->isChecked());

    abortFind();
    delete m_process;
}

void FindDocWidget::findDoc()
{
    QAction *act = (QAction*)sender();

    QString text = m_findEdit->text().trimmed();
    if (text.isEmpty()) {
        return;
    }

    QString findFlag = act->data().toString();

    abortFind();

    QStringList args;
    args << "finddoc" << "-urltag" << "<liteide_doc>";
    if (m_matchWordCheckAct->isChecked()) {
       args << "-word";
    }
    if (m_matchCaseCheckAct->isChecked()) {
        args << "-case";
    }
    if (m_useRegexpCheckAct->isChecked()) {
        args << "-r";
    }
    if (!findFlag.isEmpty()) {
        args << "-"+findFlag;
    }
    args << text.split(" ");

    m_browser->clear();
    m_findFlag = findFlag;
    m_htmlData.clear();
    QString cmd = LiteApi::getGotools(m_liteApp);
    m_process->setEnvironment(LiteApi::getGoEnvironment(m_liteApp).toStringList());
    m_process->start(cmd,args);
}

struct doc_comment {
    QString     url;
    QString     file;
    QStringList comment;
};

void FindDocWidget::extOutput(QByteArray data, bool error)
{
    if (error) {
        m_liteApp->appendLog("FindDoc",QString::fromUtf8(data),false);
        return;
    }
/*
http://golang.org/pkg\fmt\#Println
c:\go\src\pkg\log\log.go:169:
// Println calls l.Output to print to the logger.
// Arguments are handled in the manner of fmt.Println.
func (l *Logger) Println(v ...interface{})

http://godoc.org\code.google.com\p\go.tools\cmd\vet\#Println
F:\vfc\liteide-git\liteidex\src\code.google.com\p\go.tools\cmd\vet\main.go:375:
// Println is fmt.Println guarded by -v.
func Println(args ...interface{})
*/
    QList<doc_comment> dc_array;
    doc_comment dc;
    int flag = 0;
    foreach (QString line, QString::fromUtf8(data).split("\n")) {
        if (line.startsWith("<liteide_doc>")) {
            flag = 1;
            if (!dc.url.isEmpty()) {
                dc_array.push_back(dc);
            }
            dc.url = line.mid(13);
            dc.file.clear();
            dc.comment.clear();
            continue;
        }
        if (flag == 1) {
            dc.file = line;
            flag = 2;
            continue;
        }
        if (flag == 2) {
            dc.comment.push_back(line);
        }
    }
    if (!dc.url.isEmpty()) {
        dc_array.push_back(dc);
    }
    QStringList array;
    foreach (doc_comment dc, dc_array) {
        array.append(docToHtml(dc.url,dc.file,dc.comment));
    }
 //   qDebug() << array.join("\n");
//    if (m_findFlag == "pkg") {
//        array = parserPkgDoc(QString::fromUtf8(data));
//    } else {
//        array = parserDoc(QString::fromUtf8(data));
//    }

    m_htmlData.append(array.join("\n"));
    QString html = m_templateData;
    html.replace("{content}",m_htmlData);
    int pos = m_browser->scrollBarValue(Qt::Vertical);
    m_browser->setHtml(html,QUrl());
    m_browser->setScrollBarValue(Qt::Vertical,pos);
}

void FindDocWidget::extFinish(bool, int, QString)
{
    if (m_htmlData.isEmpty()) {
        QString html = m_templateData;
        html.replace("{content}","<b>Not found!</b>");
        m_browser->setHtml(html,QUrl());
    }
    m_htmlData.clear();
}

void FindDocWidget::abortFind()
{
    m_process->stop(100);
}

void FindDocWidget::stateChanged(QProcess::ProcessState state)
{
    m_chaseWidget->setAnimated(state == QProcess::Running);
    m_findEdit->showStopButton(state == QProcess::Running);
}

void FindDocWidget::openUrl(QUrl url)
{
    if (!url.isLocalFile()) {
        return;
    }
    QString text = url.toLocalFile();
    QRegExp rep("(\\w?\\:?[\\w\\d\\_\\-\\\\/\\.]+):(\\d+):");
    int index = rep.indexIn(text);
    if (index < 0)
        return;
    QStringList capList = rep.capturedTexts();

    if (capList.count() < 3)
        return;
    QString fileName = capList[1];
    QString fileLine = capList[2];

    bool ok = false;
    int line = fileLine.toInt(&ok);
    if (!ok)
        return;

    LiteApi::IEditor *editor = m_liteApp->fileManager()->openEditor(fileName,true);
    if (editor) {
        LiteApi::ITextEditor *textEditor =  LiteApi::getTextEditor(editor);
        if (textEditor) {
            textEditor->gotoLine(line-1,0,true);
        }
    }
}

void FindDocWidget::showHelp()
{
    QString data = m_templateData;
    data.replace("{content}",help);
    m_browser->setHtml(data,QUrl());
}

static QString escape(const QString &text) {
#if QT_VERSION >= 0x050000
    return QString(text).toHtmlEscaped();
#else
    return Qt::escape(text);
#endif
}

QStringList FindDocWidget::docToHtml(const QString &url, const QString &file, const QStringList &comment)
{
    QString sz;
    QString pkgName;
    QString findName;
    if (url.startsWith("http://golang.org/pkg")) {
        sz = url.mid(21);
    } else if (url.startsWith("http://golang.org/cmd")) {
        sz = url.mid(21);
    } else if (url.startsWith("http://godoc.org")) {
        sz = url.mid(16);
    }
    //\code.google.com\p\go.tools\cmd\vet\#Println
    int pos = sz.indexOf("#");
    if (pos != -1) {
        pkgName = QDir::fromNativeSeparators(sz.left(pos));
        if (pkgName.startsWith("/")) {
            pkgName = pkgName.mid(1);
        }
        if (pkgName.endsWith("/")) {
            pkgName = pkgName.left(pkgName.length()-1);
        }
        findName = sz.mid(pos+1);
    }
    QStringList array;
    array.push_back(QString("<h4><b>%2</b>&nbsp;&nbsp;<a href=\"file:%1\">%3</a></h4>")
                    .arg(QDir::fromNativeSeparators(escape(file))).arg(pkgName).arg(findName));
    if (!comment.isEmpty() &&
            (  comment.first().startsWith("const (") ||
               comment.first().startsWith("var (")) ) {
        array.push_back("<pre>");
        QString head = "const ";
        if (comment.first().startsWith("var (")) {
            head = "var ";
        }
        QStringList incmd;
        foreach (QString sz, comment) {
            if (sz.trimmed().startsWith("//")) {
                incmd.push_back(escape(sz.trimmed()));
            } else if (sz.indexOf(findName) >= 0) {
                array.append(incmd);
                array.push_back(escape(head+sz.replace("\t"," ").trimmed()));
            } else {
                incmd.clear();
            }
        }
        array.push_back("</pre>");
        return array;
    }

    int flag = 0;
    QString lastTag;
    foreach (QString sz, comment) {
        if (sz.startsWith("//")) {
            if (flag != 1) {
                if (!lastTag.isEmpty()) array.push_back(lastTag);
                array.push_back("<p>");
                lastTag = "</p>";
            }
            flag = 1;
            if (sz.mid(2).trimmed().isEmpty()) {
                array.push_back("</p><p>");
            } else {
                array.push_back(escape(sz.mid(2)));
            }
        } else {
            if (sz.trimmed().isEmpty()) {
                continue;
            }
            if (flag != 3) {
                if (!lastTag.isEmpty()) array.push_back(lastTag);
                array.push_back("<pre>");
                lastTag = "</pre>";
            }
            flag = 3;
            array.push_back(escape(sz.replace("\t","    ")));
        }
    }
    if (!lastTag.isEmpty()) array.push_back(lastTag);
    array.push_back("<p></p>");
    return array;
}

QStringList FindDocWidget::parserDoc(QString findText)
{
    QStringList array;
    int lastFlag = 0;
    QString findName;
    QString findPos;
    QString findComment;
    foreach (QString sz, findText.split('\n')) {
        int flag = 0;
        if (sz.startsWith("http://golang.org/pkg")) {
            flag = 1;
            sz = sz.mid(21);
        } else if (sz.startsWith("http://golang.org/cmd")) {
            flag = 1;
            sz = sz.mid(21);
        } else if (sz.startsWith("http://godoc.org")) {
            flag = 1;
            sz = sz.mid(16);
        } else if (sz.startsWith("//")) {
            flag = 2;
            sz = sz.mid(2);
        } else if (sz.isEmpty()) {
            flag = 4;
        } else {
            flag = 3;
        }

        if (flag == 1) {
            //\code.google.com\p\go.tools\cmd\vet\#Println
            int pos = sz.indexOf("#");
            if (pos != -1) {
                QString pkg = sz.left(pos);
                pkg = QDir::fromNativeSeparators(pkg);
                if (pkg.startsWith("/")) {
                    pkg = pkg.mid(1);
                }
                if (pkg.endsWith("/")) {
                    pkg = pkg.left(pkg.length()-1);
                }
                sz = pkg+sz.mid(pos);
                findName = sz;
            } else {
                QString pkg = sz;
                pkg = QDir::fromNativeSeparators(pkg);
                if (pkg.startsWith("/")) {
                    pkg = pkg.mid(1);
                }
                if (pkg.endsWith("/")) {
                    pkg = pkg.left(pkg.length()-1);
                }
                findName = pkg;
            }
        } else if (flag == 3) {
            if (lastFlag == 1) {
                findPos = "file:"+sz;
                array.push_back(QString("<h3><a href=\"%1\">%2</a></h3>").arg(findPos).arg(findName));
            }  else {
                array.push_back(QString("<b>%1</b>").arg(sz));
                if (!findComment.isEmpty()) {
                    array.push_back(QString("<p>%1</p>").arg(findComment));
                }
                findComment.clear();
            }
        } else if (flag == 2) {
            findComment += sz.trimmed();
        } else if (flag == 4) {
        }
        lastFlag = flag;
    }

    return array;
}

QStringList FindDocWidget::parserPkgDoc(QString findText)
{
    QStringList array;
    int lastFlag = 0;
    QString findName;
    QString findPos;
    QString findComment;
    bool bHead = false;
    foreach (QString sz, findText.split('\n')) {
        int flag = 0;
        if (sz.startsWith("http://golang.org/pkg")) {
            flag = 1;
            sz = sz.mid(21);
        } else if (sz.startsWith("http://golang.org/cmd")) {
            flag = 1;
            sz = sz.mid(21);
        } else if (sz.startsWith("http://godoc.org")) {
            flag = 1;
            sz = sz.mid(16);
        } else {
            flag = 3;
        }

        if (flag == 1) {
            bHead = false;
        }

        if (bHead) {
//            findComment.append(sz+"\n");
            if (sz.startsWith("\t\t")) {
                flag = 5;
            } else if (sz.trimmed().isEmpty()){
                flag = 6;
            } else {
                flag = 7;
            }
            sz.replace("\t"," ");
            if (lastFlag != flag && !findComment.isEmpty()) {
                if (lastFlag == 5) {
                    array.push_back(QString("<pre>%1</pre>").arg(findComment));
                } else {
                    array.push_back(QString("<p>%1</p>").arg(findComment));
                }
                findComment.clear();
            }
            if (flag == 5) {
                findComment += sz.trimmed()+"\n";
            } else {
                findComment += sz.trimmed();
            }
            lastFlag = flag;
            continue;
        }

        if (flag == 1) {
            //\code.google.com\p\go.tools\cmd\vet\#Println
            int pos = sz.indexOf("#");
            if (pos != -1) {
                QString pkg = sz.left(pos);
                pkg = QDir::fromNativeSeparators(pkg);
                if (pkg.startsWith("/")) {
                    pkg = pkg.mid(1);
                }
                if (pkg.endsWith("/")) {
                    pkg = pkg.left(pkg.length()-1);
                }
                sz = pkg+sz.mid(pos);
                findName = sz;
            } else {
                QString pkg = sz;
                pkg = QDir::fromNativeSeparators(pkg);
                if (pkg.startsWith("/")) {
                    pkg = pkg.mid(1);
                }
                if (pkg.endsWith("/")) {
                    pkg = pkg.left(pkg.length()-1);
                }
                findName = pkg;
            }
        } else if (flag == 3) {
            if (lastFlag == 1) {
                findPos = "file:"+sz;
                array.push_back(QString("<h3><a href=\"%1\">%2</a></h3>").arg(findPos).arg(findName));
                bHead = true;
            }
        } else if (flag == 2) {
            findComment += sz.trimmed();
        }
        lastFlag = flag;
    }
    if (!findComment.isEmpty()) {
        if (lastFlag == 5) {
            array.push_back(QString("<pre>%1</pre>").arg(findComment));
        } else {
            array.push_back(QString("<p>%1</p>").arg(findComment));
        }
    }
    return array;
}


