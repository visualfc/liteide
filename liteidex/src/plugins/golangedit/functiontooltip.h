#ifndef FUNCTIONTOOLTIP_H
#define FUNCTIONTOOLTIP_H

#include "liteapi/liteapi.h"
#include <QWidget>
#include <QLabel>


struct Function
{
    Function(const QString &arg) {
        args.push_back(arg);
    }

    QStringList args;
};

class FakeToolTipFrame : public QWidget
{
public:
    FakeToolTipFrame(QWidget *parent = 0) :
        QWidget(parent, Qt::ToolTip | Qt::WindowStaysOnTopHint)
    {
        setFocusPolicy(Qt::NoFocus);
        //setAttribute(Qt::WA_DeleteOnClose);

        // Set the window and button text to the tooltip text color, since this
        // widget draws the background as a tooltip.
        QPalette p = palette();
        const QColor toolTipTextColor = p.color(QPalette::Inactive, QPalette::ToolTipText);
        p.setColor(QPalette::Inactive, QPalette::WindowText, toolTipTextColor);
        p.setColor(QPalette::Inactive, QPalette::ButtonText, toolTipTextColor);
        setPalette(p);
    }

protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
};

class FunctionArgumentWidget : public QLabel
{
    Q_OBJECT

public:
    FunctionArgumentWidget(LiteApi::ITextEditor *editor);
    void showFunctionHint(QList<Function> functionSymbols,
                          int startPosition);

protected:
    bool eventFilter(QObject *obj, QEvent *e);

private slots:
    void nextPage();
    void previousPage();

private:
    void updateArgumentHighlight();
    void updateHintText();

    Function currentFunction() const
    { return m_items.at(m_current); }

    int m_startpos;
    int m_currentarg;
    int m_current;
    bool m_escapePressed;

    LiteApi::ITextEditor *m_editor;

    QWidget *m_pager;
    QLabel *m_numberLabel;
    FakeToolTipFrame *m_popupFrame;
    QList<Function> m_items;
};


#endif // FUNCTIONTOOLTIP_H
