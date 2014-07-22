#include "functiontooltip.h"
#include "cplusplus/SimpleLexer.h"

#include <QDesktopWidget>
#include <QApplication>
#include <QStylePainter>
#include <QStyleOptionFrame>
#include <QToolButton>

using namespace CPlusPlus;

void FakeToolTipFrame::paintEvent(QPaintEvent *)
{
    QStylePainter p(this);
    QStyleOptionFrame opt;
    opt.init(this);
    p.drawPrimitive(QStyle::PE_PanelTipLabel, opt);
    p.end();
}

void FakeToolTipFrame::resizeEvent(QResizeEvent *)
{
    QStyleHintReturnMask frameMask;
    QStyleOption option;
    option.init(this);
    if (style()->styleHint(QStyle::SH_ToolTip_Mask, &option, this, &frameMask))
        setMask(frameMask.region);
}


FunctionArgumentWidget::FunctionArgumentWidget(LiteApi::ITextEditor *editor):
    m_startpos(-1),
    m_current(0),
    m_escapePressed(false)
{
    m_editor = editor;

    m_popupFrame = new FakeToolTipFrame(m_editor->widget());

    QToolButton *downArrow = new QToolButton;
    downArrow->setArrowType(Qt::DownArrow);
    downArrow->setFixedSize(16, 16);
    downArrow->setAutoRaise(true);

    QToolButton *upArrow = new QToolButton;
    upArrow->setArrowType(Qt::UpArrow);
    upArrow->setFixedSize(16, 16);
    upArrow->setAutoRaise(true);

    setParent(m_popupFrame);
    setFocusPolicy(Qt::NoFocus);

    m_pager = new QWidget;
    QHBoxLayout *hbox = new QHBoxLayout(m_pager);
    hbox->setMargin(0);
    hbox->setSpacing(0);
    hbox->addWidget(upArrow);
    m_numberLabel = new QLabel;
    hbox->addWidget(m_numberLabel);
    hbox->addWidget(downArrow);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_pager);
    layout->addWidget(this);
    m_popupFrame->setLayout(layout);

    connect(upArrow, SIGNAL(clicked()), SLOT(previousPage()));
    connect(downArrow, SIGNAL(clicked()), SLOT(nextPage()));

    setTextFormat(Qt::RichText);
    setMargin(1);

    qApp->installEventFilter(this);
}

void FunctionArgumentWidget::showFunctionHint(QList<Function> functionSymbols,
                                              int startPosition)
{
    Q_ASSERT(!functionSymbols.isEmpty());

    if (m_startpos == startPosition)
        return;

    m_pager->setVisible(functionSymbols.size() > 1);

    m_items = functionSymbols;
    m_startpos = startPosition;
    m_current = 0;
    m_escapePressed = false;

    // update the text
    m_currentarg = -1;
    updateArgumentHighlight();

    m_popupFrame->show();
}

void FunctionArgumentWidget::nextPage()
{
    m_current = (m_current + 1) % m_items.size();
    updateHintText();
}

void FunctionArgumentWidget::previousPage()
{
    if (m_current == 0)
        m_current = m_items.size() - 1;
    else
        --m_current;

    updateHintText();
}

void FunctionArgumentWidget::updateArgumentHighlight()
{
    int curpos = m_editor->position();
    if (curpos < m_startpos) {
        m_popupFrame->close();
        return;
    }
    QString str = m_editor->textAt(m_startpos, curpos - m_startpos);
    int argnr = 0;
    int parcount = 0;

    LanguageFeatures features;
    features.golangEnable = true;

    SimpleLexer tokenize;
    tokenize.setLanguageFeatures(features);
    QList<Token> tokens = tokenize(str);
    for (int i = 0; i < tokens.count(); ++i) {
        const Token &tk = tokens.at(i);
        if (tk.is(T_LPAREN))
            ++parcount;
        else if (tk.is(T_RPAREN))
            --parcount;
        else if (! parcount && tk.is(T_COMMA))
            ++argnr;
    }

    if (m_currentarg != argnr) {
        m_currentarg = argnr;
        updateHintText();
    }

    if (parcount < 0)
        m_popupFrame->close();
}

bool FunctionArgumentWidget::eventFilter(QObject *obj, QEvent *e)
{
    if (!m_popupFrame->isVisible()) {
        return false;
    }
    switch (e->type()) {
    case QEvent::ShortcutOverride:
        if (static_cast<QKeyEvent*>(e)->key() == Qt::Key_Escape) {
            m_escapePressed = true;
        }
        break;
    case QEvent::KeyPress:
        if (static_cast<QKeyEvent*>(e)->key() == Qt::Key_Escape) {
            m_escapePressed = true;
        }
        if (m_items.size() > 1) {
            QKeyEvent *ke = static_cast<QKeyEvent*>(e);
            if (ke->key() == Qt::Key_Up) {
                previousPage();
                return true;
            } else if (ke->key() == Qt::Key_Down) {
                nextPage();
                return true;
            }
            return false;
        }
        break;
    case QEvent::KeyRelease:
        if (static_cast<QKeyEvent*>(e)->key() == Qt::Key_Escape && m_escapePressed) {
            m_popupFrame->close();
            return false;
        }
        if (static_cast<QKeyEvent*>(e)->key() == Qt::Key_Return) {
            return false;
        }
        updateArgumentHighlight();
        break;
    case QEvent::WindowDeactivate:
    case QEvent::FocusOut:
    case QEvent::Resize:
    case QEvent::Move:
        if (obj != m_editor->widget())
            break;
        m_popupFrame->close();
        break;
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::Wheel: {
            QWidget *widget = qobject_cast<QWidget *>(obj);
            if (! (widget == this || m_popupFrame->isAncestorOf(widget))) {
                m_popupFrame->close();
            }
        }
        break;
    default:
        break;
    }
    return false;
}

void FunctionArgumentWidget::updateHintText()
{
//    Overview overview;
//    overview.setShowReturnTypes(true);
//    overview.setShowArgumentNames(true);
//    overview.setMarkedArgument(m_currentarg + 1);
//    Function *f = currentFunction();

//    const QString prettyMethod = overview(f->type(), f->name());
//    const int begin = overview.markedArgumentBegin();
//    const int end = overview.markedArgumentEnd();

    Function fn = currentFunction();

    QString hintText;
    hintText += fn.args.join(",");
//    hintText += Qt::escape(prettyMethod.left(begin));
//    hintText += "<b>";
//    hintText += Qt::escape(prettyMethod.mid(begin, end - begin));
//    hintText += "</b>";
//    hintText += Qt::escape(prettyMethod.mid(end));
    setText(hintText);

    m_numberLabel->setText(tr("%1 of %2").arg(m_current + 1).arg(m_items.size()));

    m_popupFrame->setFixedWidth(m_popupFrame->minimumSizeHint().width());

    const QDesktopWidget *desktop = QApplication::desktop();
#ifdef Q_WS_MAC
    const QRect screen = desktop->availableGeometry(desktop->screenNumber(m_editor->widget()));
#else
    const QRect screen = desktop->screenGeometry(desktop->screenNumber(m_editor->widget()));
#endif

    const QSize sz = m_popupFrame->sizeHint();
    QPoint pos = m_editor->cursorRect(m_startpos).topLeft();
    pos.setY(pos.y() - sz.height() - 1);

    if (pos.x() + sz.width() > screen.right())
        pos.setX(screen.right() - sz.width());

    m_popupFrame->move(pos);
}
