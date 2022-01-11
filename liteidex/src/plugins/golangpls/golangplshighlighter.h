#ifndef GOLANGPLSHIGHLIGHTER_H
#define GOLANGPLSHIGHLIGHTER_H

#include "qtc_texteditor/syntaxhighlighter.h"
#include "liteapi/liteapi.h"

class GolangPlsHighlighter: public TextEditor::SyntaxHighlighter
{
    Q_OBJECT
public:
    GolangPlsHighlighter(LiteApi::ITextEditor *editor, QTextDocument *document = 0);

    static SyntaxHighlighter::TextFormatId plsToFormat(unsigned int tokenType);
    // SyntaxHighlighter interface
protected:
    void highlightBlock(const QTextBlock &block);

public slots:
    void onHighlightResults(const QVariantList &list);

protected:
    LiteApi::ITextEditor *m_editor;
    struct Highlight {
        int pos;
        int length;
        TextFormatId format;
    };
    // line -> formats
    QHash<int, QList<Highlight>> m_highlights;
};

#endif // GOLANGPLSHIGHLIGHTER_H
