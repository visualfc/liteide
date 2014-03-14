#include "finddocwidget.h"
#include "liteenvapi/liteenvapi.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QActionGroup>
#include <QAction>

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

    mainLayout->addLayout(findLayout);

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
    args << "doc" << "-src";
    if (m_useRegexpCheckAct->isChecked()) {
        args << "-r";
    }
    if (!findFlag.isEmpty()) {
        args << findFlag;
    }
    args << text;

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
}

void FindDocWidget::extFinish(bool, int, QString)
{

}
