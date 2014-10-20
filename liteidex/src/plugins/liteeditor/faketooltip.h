#ifndef FAKETOOLTIP_H
#define FAKETOOLTIP_H

#include <QWidget>

class FakeToolTip : public QWidget
{
    Q_OBJECT
public:
    explicit FakeToolTip(QWidget *parent = 0);
protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
};

#endif // FAKETOOLTIP_H
