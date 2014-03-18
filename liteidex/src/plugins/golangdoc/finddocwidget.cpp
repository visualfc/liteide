#include "finddocwidget.h"
#include "liteenvapi/liteenvapi.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QActionGroup>
#include <QAction>
#include <QPlainTextEdit>
#include <QTextBrowser>

class SearchEdit : public Utils::FancyLineEdit
{
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

    m_browser =  m_liteApp->htmlWidgetManager()->createByName(this,"QTextBrowser");
    QStringList paths;
    paths << m_liteApp->resourcePath()+"/golangdoc";
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

    QMenu *menu = new QMenu(findBtn);
    menu->addActions(QList<QAction*>()
                     << findAll
                     << findPkg
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
    menu->addAction(m_useRegexpCheckAct);
    findBtn->setMenu(menu);
    //findBtn->setDefaultAction(findAll);

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


    QString path = m_liteApp->resourcePath()+"/golangdoc/finddoc.html";
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        m_templateData = file.readAll();
        file.close();
    }
}

FindDocWidget::~FindDocWidget()
{
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
    args << "doc";
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
    QString cmd = LiteApi::liteide_stub_cmd(m_liteApp);
    m_process->setEnvironment(LiteApi::getGoEnvironment(m_liteApp).toStringList());
    m_process->start(cmd,args);
}

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
    QStringList array;
    if (m_findFlag == "pkg") {
        array = parserPkgDoc(QString::fromUtf8(data));
    } else {
        array = parserDoc(QString::fromUtf8(data));
    }

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
    if (m_process->isRunning()) {
        m_process->kill();
        m_process->waitForFinished(1000);
    }
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


