#include "calltip.h"
#include <QPainter>

CallTip::CallTip(QObject *parent) :
    QObject(parent)
{
    popup = new CallTipWidget;
    m_inCallTipMode = false;
    m_pos = 0;
    connect(popup,SIGNAL(mouseClick()),this,SLOT(CallTipCancel()));
}

CallTip::~CallTip()
{
    if (popup) {
        delete popup;
    }
}

QWidget *CallTip::widget()
{
    return popup;
}

void CallTip::CallTipStart(int pos, const QRect& rect, QString text, QWidget *parent)
{
    m_inCallTipMode = true;
    m_pos = pos;
    popup->setInfo(text);
    QPoint pt;
    pt = parent->mapToGlobal(rect.topLeft());
    int w = rect.width();
    int h = rect.height();
    popup->setGeometry(pt.x(), pt.y()-h, w, h);
    if (!popup->isVisible())
        popup->show();
}

void CallTip::ShowTip()
{
    m_inCallTipMode = true;
    if (!popup->isVisible())
        popup->show();
}

void CallTip::CallTipCancel()
{
    m_inCallTipMode = false;
    if (popup->isVisible()) {
        popup->hide();
    }
}

void CallTip::showPopup(const QRect& rect, QWidget *widget)
{
    const QRect screen = QApplication::desktop()->availableGeometry(widget);
    Qt::LayoutDirection dir = widget->layoutDirection();
    QPoint pos;
    int rh, w;
    int h = 100;
    if (rect.isValid()) {
        rh = rect.height();
        w = rect.width();
        pos = widget->mapToGlobal(dir == Qt::RightToLeft ? rect.bottomRight() : rect.bottomLeft());
    } else {
        rh = widget->height();
        pos = widget->mapToGlobal(QPoint(0, widget->height() - 2));
        w = widget->width();
    }

    if (w > screen.width())
        w = screen.width();
    if ((pos.x() + w) > (screen.x() + screen.width()))
        pos.setX(screen.x() + screen.width() - w);
    if (pos.x() < screen.x())
        pos.setX(screen.x());

    int top = pos.y() - rh - screen.top() + 2;
    int bottom = screen.bottom() - pos.y();
    h = qMax(h, popup->minimumHeight());
    if (h > bottom) {
        h = qMin(qMax(top, bottom), h);

        if (top > bottom)
            pos.setY(pos.y() - h - rh + 2);
    }

    popup->setGeometry(pos.x(), pos.y(), w, h);

    if (!popup->isVisible())
        popup->show();
}


CallTipWidget::CallTipWidget(QWidget *parent) : QWidget(parent,Qt::ToolTip)
{

}

CallTipWidget::~CallTipWidget()
{

}

void CallTipWidget::setInfo(QString info)
{
    m_info = info;
}

void CallTipWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    QRect rc = this->rect();
    painter.fillRect(rc,Qt::white);
    painter.drawText(rc,m_info,Qt::AlignLeft|Qt::AlignVCenter);
}

void CallTipWidget::mousePressEvent(QMouseEvent *event)
{
    emit mouseClick();
}
