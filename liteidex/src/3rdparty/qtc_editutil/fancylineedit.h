/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
**************************************************************************/

#ifndef FANCYLINEEDIT_H
#define FANCYLINEEDIT_H

#include <QtGui/QLineEdit>
#include <QtGui/QAbstractButton>

namespace Utils {

class FancyLineEditPrivate;

class IconButton: public QAbstractButton
{
    Q_OBJECT
    Q_PROPERTY(float iconOpacity READ iconOpacity WRITE setIconOpacity)
    Q_PROPERTY(bool autoHide READ hasAutoHide WRITE setAutoHide)
    Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap)
public:
    explicit IconButton(QWidget *parent = 0);
    void paintEvent(QPaintEvent *event);
    void setPixmap(const QPixmap &pixmap) { m_pixmap = pixmap; update(); }
    QPixmap pixmap() const { return m_pixmap; }
    float iconOpacity() { return m_iconOpacity; }
    void setIconOpacity(float value) { m_iconOpacity = value; update(); }
    void animateShow(bool visible);

    void setAutoHide(bool hide) { m_autoHide = hide; }
    bool hasAutoHide() const { return m_autoHide; }
private:
    float m_iconOpacity;
    bool m_autoHide;
    QPixmap m_pixmap;
};


/* A line edit with an embedded pixmap on one side that is connected to
 * a menu. Additionally, it can display a grayed hintText (like "Type Here to")
 * when not focused and empty. When connecting to the changed signals and
 * querying text, one has to be aware that the text is set to that hint
 * text if isShowingHintText() returns true (that is, does not contain
 * valid user input).
 */
class FancyLineEdit : public QLineEdit
{
    Q_DISABLE_COPY(FancyLineEdit)
    Q_OBJECT
    Q_ENUMS(Side)

public:
    enum Side {Left = 0, Right = 1};

    explicit FancyLineEdit(QWidget *parent = 0);
    ~FancyLineEdit();

    QPixmap buttonPixmap(Side side) const;
    void setButtonPixmap(Side side, const QPixmap &pixmap);

    QMenu *buttonMenu(Side side) const;
    void setButtonMenu(Side side, QMenu *menu);

    void setButtonVisible(Side side, bool visible);
    bool isButtonVisible(Side side) const;

    void setButtonToolTip(Side side, const QString &);
    void setButtonFocusPolicy(Side side, Qt::FocusPolicy policy);

    // Set whether tabbing in will trigger the menu.
    void setMenuTabFocusTrigger(Side side, bool v);
    bool hasMenuTabFocusTrigger(Side side) const;

    // Set if icon should be hidden when text is empty
    void setAutoHideButton(Side side, bool h);
    bool hasAutoHideButton(Side side) const;

signals:
    void buttonClicked(Utils::FancyLineEdit::Side side);
    void leftButtonClicked();
    void rightButtonClicked();

private slots:
    void checkButtons(const QString &);
    void iconClicked();

protected:
    virtual void resizeEvent(QResizeEvent *e);

private:
    void updateMargins();
    void updateButtonPositions();
    friend class Utils::FancyLineEditPrivate;

    FancyLineEditPrivate *m_d;
    QString m_oldText;
};

} // namespace Utils

#endif // FANCYLINEEDIT_H
