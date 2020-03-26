#ifndef VTERMCOLOR_H
#define VTERMCOLOR_H

#include <QString>
#include <QStringList>

enum TERM_COLOR {
    TERM_COLOR_DEFAULT = -1,
    TERM_COLOR_BLACK = 0,
    TERM_COLOR_RED = 1,
    TERM_COLOR_GREEN = 2,
    TERM_COLOR_YELLOW = 3,
    TERM_COLOR_BLUE = 4,
    TERM_COLOR_PURPLE = 5,
    TERM_COLOR_CYAN = 6,
    TERM_COLOR_WHITE = 7,
};

enum TERM_ATTR {
    TERM_ATTR_NORMAL = 0,
    TERM_ATTR_BOLD = 0x00000001,
    TERM_ATTR_NOBOLD = 0x00000002,
    TERM_ATTR_ITALIC = 0x00000004,
    TERM_ATTR_NOITALIC = 0x00000008,
    TERM_ATTR_UNDERLINE = 0x00000010,
    TERM_ATTR_NOUNDERLINE = 0x000020,
    TERM_ATTR_BLINK = 0x00000040,
    TERM_ATTR_NOBLINK = 0x00000080,
    TERM_ATTR_REVERSE = 0x00000100,
    TERM_ATTR_NOREVERSE = 0x00000200,
    TERM_ATTR_HALF = 0x00000400,
    TERM_ATTR_NOHALF = TERM_ATTR_NOBOLD,
    TERM_ATTR_LIGHT = TERM_ATTR_BOLD,
    TERM_ATTR_NOLIGHT = TERM_ATTR_NOBOLD,
    TERM_ATTR_HIDE = 0x00000800,

};

inline QString colored(const QString &text, TERM_COLOR fg = TERM_COLOR_DEFAULT,  TERM_COLOR bg = TERM_COLOR_DEFAULT, int attr = TERM_ATTR_NORMAL, bool endResetDefault = true)
{
    if (fg == TERM_COLOR_DEFAULT && bg == TERM_COLOR_DEFAULT && attr == TERM_ATTR_NORMAL) {
        return text;
    }
    QStringList attrs;
    if (fg != TERM_COLOR_DEFAULT) {
        attrs << QString("3%1").arg(fg);
    }
    if (bg != TERM_COLOR_DEFAULT) {
        attrs << QString("4%1").arg(bg);
    }
    if ((attr & TERM_ATTR_BOLD) == TERM_ATTR_BOLD) {
        attrs << "1";
    }
    if ((attr & TERM_ATTR_HALF) == TERM_ATTR_HALF) {
        attrs << "2";
    }
    if ((attr & TERM_ATTR_NOBOLD) == TERM_ATTR_NOBOLD) {
        attrs << "22";
    }
    if ((attr & TERM_ATTR_ITALIC) == TERM_ATTR_ITALIC) {
        attrs << "3";
    }
    if ((attr & TERM_ATTR_NOITALIC) == TERM_ATTR_NOITALIC) {
        attrs << "23";
    }
    if ((attr & TERM_ATTR_UNDERLINE) == TERM_ATTR_UNDERLINE) {
        attrs << "4";
    }
    if ((attr & TERM_ATTR_NOUNDERLINE) == TERM_ATTR_NOUNDERLINE) {
        attrs << "24";
    }
    if ((attr & TERM_ATTR_BLINK) == TERM_ATTR_BLINK) {
        attrs << "5";
    }
    if ((attr & TERM_ATTR_NOBLINK) == TERM_ATTR_NOBLINK) {
        attrs << "25";
    }
    if ((attr & TERM_ATTR_REVERSE) == TERM_ATTR_REVERSE) {
        attrs << "7";
    }
    if ((attr & TERM_ATTR_NOREVERSE) == TERM_ATTR_NOREVERSE) {
        attrs << "27";
    }
    if (endResetDefault) {
        return QString("\033[%1m%2\033[0m").arg(attrs.join(";")).arg(text);
    }
    return  QString("\033[%1m%2").arg(attrs.join(";")).arg(text);
}


#endif // VTERMCOLOR_H
