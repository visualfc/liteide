#ifndef WEBKITBROWSER_H
#define WEBKITBROWSER_H

#include <QWidget>
#include <QUrl>

class QWebView;
class QToolBar;
class QLineEdit;
class WebkitBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit WebkitBrowser(QWidget *parent = 0);
    void setUrl(const QUrl &url);
signals:
    
public slots:
    void changeLocation();
    void adjustLocation();
protected:
    QToolBar *m_toolBar;
    QLineEdit *m_locationEdit;
    QWebView *m_view;
};

#endif // WEBKITBROWSER_H
