#include "webkitbrowser.h"
#include "sundown/mdtohtml.h"
#include <QWebView>
#include <QNetworkProxyFactory>
#include <QToolBar>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QFile>
#include <QFileInfo>
#include <QStatusBar>
#include <QProgressBar>
#include <QRegExp>
#include <QDebug>

WebkitBrowser::WebkitBrowser(LiteApi::IApplication *app, QWidget *parent) :
    QWidget(parent), m_liteApp(app)
{        
    QNetworkProxyFactory::setUseSystemConfiguration(true);

    m_view = new QWebView;
    m_view->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

    m_locationEdit = new QLineEdit(this);
    m_locationEdit->setSizePolicy(QSizePolicy::Expanding, m_locationEdit->sizePolicy().verticalPolicy());
    connect(m_locationEdit, SIGNAL(returnPressed()), this,SLOT(changeLocation()));
    connect(m_view, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
    connect(m_view, SIGNAL(linkClicked(QUrl)),this, SLOT(linkClicked(QUrl)));
    connect(m_view->page(), SIGNAL(linkHovered(QString,QString,QString)),this,SLOT(linkHovered(QString,QString,QString)));
    connect(m_view,SIGNAL(statusBarMessage(QString)),this,SLOT(statusBarMessage(QString)));
    connect(m_view,SIGNAL(loadStarted()),this,SLOT(loadStarted()));
    connect(m_view,SIGNAL(loadProgress(int)),this,SLOT(loadProgress(int)));

    QToolBar *toolBar = new QToolBar(tr("Navigation"));
    toolBar->setIconSize(QSize(16,16));
    toolBar->addAction(m_view->pageAction(QWebPage::Back));
    toolBar->addAction(m_view->pageAction(QWebPage::Forward));
    toolBar->addAction(m_view->pageAction(QWebPage::Reload));
    toolBar->addAction(m_view->pageAction(QWebPage::Stop));
    toolBar->addWidget(m_locationEdit);

    m_progressBar = new QProgressBar;
    m_progressBar->hide();
    m_progressBar->setRange(0,100);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addWidget(toolBar);
    layout->addWidget(m_view);
    layout->addWidget(m_progressBar);

    this->setLayout(layout);
}

static QByteArray html_data =
"<html>"
"<head>"
"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>"
"<title>__HTML_TITLE__</title>"
"</head>"
"<body>"
"__HTML_CONTENT__"
"</body>"
"</html>";

void WebkitBrowser::changeLocation()
{
    QString text = m_locationEdit->text();
    if (text.isEmpty()) {
        return;
    }
    if (!text.startsWith("http:") &&
            !text.startsWith("file:") &&
            !text.startsWith("https:")) {
        text = "http://"+text;
    }
    loadUrl(text);
}

void WebkitBrowser::adjustLocation()
{
    m_locationEdit->setText(m_view->url().toString());
}

void WebkitBrowser::loadFinished(bool b)
{
    m_progressBar->hide();
    QString url = m_view->url().toString();
    if (b) {
        m_locationEdit->setText(url);
    } else {
        QString context = QString(tr("False load %1 !")).arg(url);
    #if QT_VERSION >= 0x050000
        QString html = context.toHtmlEscaped();
    #else
        QString html = Qt::escape(context);
    #endif
        QString data = html_data;
        data.replace("__HTML_TITLE__","LoadFalse");
        data.replace("__HTML_CONTENT__",QString("<pre>%1</pre>").arg(html));
        m_view->setHtml(data,url);
    }
}

void WebkitBrowser::linkClicked(QUrl url)
{
    this->loadUrl(url);
}

void WebkitBrowser::loadUrl(const QUrl &url)
{
    m_liteApp->mainWindow()->statusBar()->clearMessage();
    if (url.scheme() == "http" || url.scheme() == "https") {
        m_view->setUrl(url);
    } else if (url.scheme() == "file") {
        QFileInfo info(url.toLocalFile());
#ifdef Q_OS_WIN
        QString file = info.filePath();
        if (file.length() > 4 && file[0] == '/' && file[1] == '/' &&
                file[2].isLetter() && file[3] == '/') {
            info.setFile(file[2]+":"+file.right(file.length()-3));
        }
#endif
        if (info.exists()) {
            QFile file(info.filePath());
            if (file.open(QFile::ReadOnly)) {
                QByteArray ba = file.readAll();
                QString ext = info.suffix().toLower();
                if (ext == "html" || ext == "htm") {
                    QTextCodec *codec = QTextCodec::codecForHtml(ba,QTextCodec::codecForName("utf-8"));
                    m_view->setHtml(codec->toUnicode(ba),url);
                } else if (ext == "md") {
                    QString data = html_data;
                    data.replace("__HTML_TITLE__",info.fileName());
                    data.replace("__HTML_CONTENT__",QString::fromUtf8(md2html(ba)));
                    m_view->setHtml(data,url);
                } else {
                    QTextCodec *codec = QTextCodec::codecForUtfText(ba,QTextCodec::codecForName("utf-8"));
                #if QT_VERSION >= 0x050000
                    QString html = codec->toUnicode(ba).toHtmlEscaped();
                #else
                    QString html = Qt::escape(codec->toUnicode(ba));
                #endif
                    QString data = html_data;
                    data.replace("__HTML_TITLE__",info.fileName());
                    data.replace("__HTML_CONTENT__",QString("<pre>%1</pre>").arg(html));
                    m_view->setHtml(data,url);
                }
            }
        }
    }
    m_view->setFocus();
}

void WebkitBrowser::linkHovered(const QString & link, const QString & /*title*/, const QString & /*textContent*/)
{
   m_liteApp->mainWindow()->statusBar()->showMessage(link);
}

void WebkitBrowser::statusBarMessage(const QString &msg)
{
    m_liteApp->mainWindow()->statusBar()->showMessage(msg);
}

void WebkitBrowser::loadStarted()
{
    m_progressBar->show();
}

void WebkitBrowser::loadProgress(int value)
{
    m_progressBar->setValue(value);
}
