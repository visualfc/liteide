#ifndef SEARCHRESULTCOLOR_H
#define SEARCHRESULTCOLOR_H

#include <QColor>

namespace Find {
namespace Internal {

class SearchResultColor{
public:
    QColor textBackground;
    QColor textForeground;
    QColor highlightBackground;
    QColor highlightForeground;
};

} // namespace Internal
} // namespace Find

#endif // SEARCHRESULTCOLOR_H
