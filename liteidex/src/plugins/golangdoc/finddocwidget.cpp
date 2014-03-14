#include "finddocwidget.h"
#include "liteenvapi/liteenvapi.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QActionGroup>
#include <QAction>
#include <QPlainTextEdit>

FindDocWidget::FindDocWidget(LiteApi::IApplication *app, QWidget *parent) :
    QWidget(parent), m_liteApp(app)
{
    m_findEdit = new Utils::FilterLineEdit(500);
    m_findEdit->setPlaceholderText(tr("Search"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(1);
    mainLayout->setSpacing(1);

    QHBoxLayout *findLayout = new QHBoxLayout;
    findLayout->setMargin(2);
    findLayout->addWidget(m_findEdit);

    QToolButton *findBtn = new QToolButton;
    findBtn->setPopupMode(QToolButton::MenuButtonPopup);
    findBtn->setText("Find");
    findLayout->addWidget(findBtn);

    m_browser = new QPlainTextEdit;
    m_browser->setReadOnly(true);

    mainLayout->addLayout(findLayout);
    mainLayout->addWidget(m_browser);

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
                     << findConst
                     << findFunc
                     << findInterface
                     << findPkg
                     << findStruct
                     << findType
                     << findVar
                     );
    menu->addSeparator();
    menu->addAction(m_useRegexpCheckAct);
    findBtn->setMenu(menu);
    findBtn->setDefaultAction(findAll);

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
    connect(m_process,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(extOutput(QByteArray,bool)));
    connect(m_process,SIGNAL(extFinish(bool,int,QString)),this,SLOT(extFinish(bool,int,QString)));
    connect(m_findEdit,SIGNAL(returnPressed()),findAll,SIGNAL(triggered()));
}

FindDocWidget::~FindDocWidget()
{
    if (m_process->isRunning()) {
        m_process->kill();
    }
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

    if (m_process->isRunning()) {
        m_process->kill();
        m_process->waitForFinished(1000);
    }

    m_process->setEnvironment(LiteApi::getGoEnvironment(m_liteApp).toStringList());
    QStringList args;
    args << "doc";
    if (m_useRegexpCheckAct->isChecked()) {
        args << "-r";
    }
    if (!findFlag.isEmpty()) {
        args << "-"+findFlag;
    }
    args << text;

    m_browser->clear();
    QString cmd = LiteApi::liteide_stub_cmd(m_liteApp);
    m_process->start(cmd,args);
}

void FindDocWidget::extOutput(QByteArray data, bool error)
{
    if (error) {
        m_liteApp->appendLog("FindDoc",QString::fromUtf8(data),false);
        return;
    }
    QString findText = QString::fromUtf8(data);
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
    int lastFlag = 0;
    QString findName;
    QString findPos;
    QString findComment;
    foreach (QString sz, findText.split('\n')) {
        int flag = 0;
        if (sz.isEmpty()) {
            flag = 4;
        } else if (sz.startsWith("http://golang.org/pkg")) {
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
                findPos = sz;
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
    m_browser->appendHtml(array.join("\n"));
}

void FindDocWidget::extFinish(bool, int, QString)
{
}
