#include "webkitbrowser.h"
#include <QWebView>
#include <QToolBar>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QDebug>

WebkitBrowser::WebkitBrowser(QWidget *parent) :
    QWidget(parent)
{        
    m_view = new QWebView;

    m_locationEdit = new QLineEdit(this);
    m_locationEdit->setSizePolicy(QSizePolicy::Expanding, m_locationEdit->sizePolicy().verticalPolicy());
    connect(m_locationEdit, SIGNAL(returnPressed()), SLOT(changeLocation()));
    connect(m_view, SIGNAL(loadFinished(bool)), SLOT(adjustLocation()));

    QToolBar *toolBar = new QToolBar(tr("Navigation"));
    toolBar->setIconSize(QSize(16,16));
    toolBar->addAction(m_view->pageAction(QWebPage::Back));
    toolBar->addAction(m_view->pageAction(QWebPage::Forward));
    toolBar->addAction(m_view->pageAction(QWebPage::Reload));
    toolBar->addAction(m_view->pageAction(QWebPage::Stop));
    toolBar->addWidget(m_locationEdit);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addWidget(toolBar);
    layout->addWidget(m_view);

    this->setLayout(layout);
}


void WebkitBrowser::changeLocation()
{
    QString text = m_locationEdit->text();
    if (text.isEmpty()) {
        return;
    }
    if (!text.startsWith("http://") &&
            !text.startsWith("file://") &&
            !text.startsWith("https://")) {
        text = "http://"+text;
    }
    QUrl url = QUrl(text);
    m_view->load(url);
    m_view->setFocus();
}

void WebkitBrowser::adjustLocation()
{
    m_locationEdit->setText(m_view->url().toString());
}
