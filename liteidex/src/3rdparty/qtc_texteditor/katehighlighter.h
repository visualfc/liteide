#ifndef KATEHIGHLIGHTER_H
#define KATEHIGHLIGHTER_H

#include "texteditor_global.h"

#include <QObject>
#include <QTextDocument>
#include <QSharedPointer>
#include <QFileInfo>
#include "syntaxhighlighter.h"
#include "colorscheme.h"

class ColorStyleScheme;
class TEXTEDITOR_EXPORT KateHighlighter : public QObject
{
    Q_OBJECT
public:
    explicit KateHighlighter(QObject *parent = 0);
    static void setColorStyle(TextEditor::SyntaxHighlighter *h,const ColorStyleScheme*);
    static void setTabSize(TextEditor::SyntaxHighlighter *h, int tabSize);
public:
    void loadPath(const QString &definitionsPaths);
    QStringList mimeTypes() const;
    QStringList mimeTypePatterns(const QString &mimeType) const;
    QString mimeTypeName(const QString &mimeType) const;
    TextEditor::SyntaxHighlighter *create(QTextDocument *doc, const QString &mimeType);
    QTextCharFormat toTextCharFormat(const QString &name);
};

#endif // KATEHIGHLIGHTER_H
