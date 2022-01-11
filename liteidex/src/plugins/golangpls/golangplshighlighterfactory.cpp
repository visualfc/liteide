#include "golangplshighlighterfactory.h"

#include "golangplshighlighter.h"

GolangPlsHighlighterFactory::GolangPlsHighlighterFactory(QObject *parent) :
    LiteApi::IHighlighterFactory(parent)
{

}

QStringList GolangPlsHighlighterFactory::mimeTypes() const
{
    return QStringList() << "text/x-gosrc";
}

TextEditor::SyntaxHighlighter *GolangPlsHighlighterFactory::create(LiteApi::ITextEditor *editor, QTextDocument *doc, const QString &mimeType)
{
    Q_UNUSED(mimeType)

    auto highlighter = new GolangPlsHighlighter(editor, doc);
    m_editorToHighlighter[editor] = highlighter;
    return highlighter;
}

GolangPlsHighlighter *GolangPlsHighlighterFactory::getHighlighter(LiteApi::IEditor *editor) const
{
    auto textEditor = LiteApi::getTextEditor(editor);
    return m_editorToHighlighter.value(textEditor, nullptr);
}

void GolangPlsHighlighterFactory::removeHighlighterForEditor(LiteApi::IEditor *editor)
{
    auto textEditor = LiteApi::getTextEditor(editor);
    m_editorToHighlighter.remove(textEditor);
}
