#ifndef CALLTIP_H
#define CALLTIP_H

#include <QObject>
#include <QWidget>

class CallTipWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CallTipWidget(QWidget *parent = 0);
    ~CallTipWidget();
    void setInfo(QString info);
    void showPopup(QRect rc);
signals:
    void mouseClick();
protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual void mousePressEvent(QMouseEvent *event);
protected:
    QString m_info;
};

class CallTip : public QObject
{
    Q_OBJECT
public:
    explicit CallTip(QObject *parent = 0);
    ~CallTip();
    QWidget* widget();
    void CallTipStart(int pos, const QRect& rect, QString text, QWidget *parent);
    void ShowTip();
    bool InCallTipMode() const { return m_inCallTipMode; }
    int posStartCallTip() const { return m_pos; }
    void showPopup(const QRect& rect, QWidget *parent);
signals:
public slots:
    void CallTipCancel();
protected:
    CallTipWidget *popup;
    bool     m_inCallTipMode;
    int      m_pos;
};

#endif // CALLTIP_H
