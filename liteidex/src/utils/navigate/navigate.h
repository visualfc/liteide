#ifndef NAVIGATE_H
#define NAVIGATE_H

#include "liteapi/liteapi.h"

class NavigateBar : public QObject
{
    Q_OBJECT
public:
    NavigateBar(LiteApi::IApplication *app, const QString &title, QObject *parent);
    virtual ~NavigateBar();
    void LoadPath(const QString &path);
    QToolBar *toolBar() const {
        return  m_toolBar;
    }
public slots:
    void pathLinkActivated(const QString &path);
    void quickPathLinkActivated(const QString &path);
protected:
    QToolBar *createNavToolBar(QWidget *parent);
protected:
    LiteApi::IApplication *m_liteApp;
    QToolBar *m_toolBar;
    QAction *m_navHeadAct;
    QString m_filePath;
};

#endif // NAVIGATE_H
