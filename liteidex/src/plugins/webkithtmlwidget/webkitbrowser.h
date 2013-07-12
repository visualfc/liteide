#ifndef WEBKITBROWSER_H
#define WEBKITBROWSER_H

#include "liteapi/liteapi.h"
#include <QWidget>
#include <QUrl>

class QWebView;
class QToolBar;
class QLineEdit;
class WebkitBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit WebkitBrowser(LiteApi::IApplication *app, QWidget *parent = 0);
public slots:
    void changeLocation();
    void adjustLocation();
    void loadFinished(bool);
    void linkClicked(QUrl);
    void loadUrl(const QUrl &url);
    void linkHovered(const QString & link, const QString & title, const QString & textContent);
    void statusBarMessage(const QString &msg);
protected:
    LiteApi::IApplication *m_liteApp;
    QToolBar *m_toolBar;
    QLineEdit *m_locationEdit;
    QWebView *m_view;
};

#endif // WEBKITBROWSER_H
