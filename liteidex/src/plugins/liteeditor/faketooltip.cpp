#include "faketooltip.h"
#include <QStylePainter>
#include <QStyleOptionFrame>

FakeToolTip::FakeToolTip(QWidget *parent) :
    QWidget(parent, Qt::ToolTip | Qt::WindowStaysOnTopHint)
{
    setFocusPolicy(Qt::NoFocus);
    //setAttribute(Qt::WA_DeleteOnClose);

//    QPalette p = palette();
//    const QColor toolTipTextColor = p.color(QPalette::Inactive, QPalette::ToolTipText);
//    p.setColor(QPalette::Inactive, QPalette::WindowText, toolTipTextColor);
//    p.setColor(QPalette::Inactive, QPalette::ButtonText, toolTipTextColor);
//    setPalette(p);

    const int margin = 1 + style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, 0, this);
    setContentsMargins(margin + 1, margin, margin, margin);
    //setWindowOpacity(style()->styleHint(QStyle::SH_ToolTipLabel_Opacity, 0, this) / 255.0);
}

void FakeToolTip::paintEvent(QPaintEvent *)
{
    QStylePainter p(this);
    QStyleOptionFrame opt;
    opt.init(this);
    p.drawPrimitive(QStyle::PE_PanelTipLabel, opt);
    p.end();
}

void FakeToolTip::resizeEvent(QResizeEvent *)
{
    QStyleHintReturnMask frameMask;
    QStyleOption option;
    option.init(this);
    if (style()->styleHint(QStyle::SH_ToolTip_Mask, &option, this, &frameMask))
        setMask(frameMask.region);
}
