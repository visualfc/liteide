#ifndef GOLANGPLSHIGHLIGHTERFACTORY_H
#define GOLANGPLSHIGHLIGHTERFACTORY_H

#include "liteeditorapi/liteeditorapi.h"

class GolangPlsHighlighter;

class GolangPlsHighlighterFactory : public LiteApi::IHighlighterFactory
{
    Q_OBJECT
public:
    GolangPlsHighlighterFactory(QObject *parent = 0);

    QStringList mimeTypes() const;
    TextEditor::SyntaxHighlighter *create(LiteApi::ITextEditor *editor, QTextDocument *doc, const QString &mimeType);

    GolangPlsHighlighter *getHighlighter(LiteApi::IEditor *editor) const;

public slots:
    void removeHighlighterForEditor(LiteApi::IEditor *editor);

private:
    QHash<LiteApi::ITextEditor *, GolangPlsHighlighter *> m_editorToHighlighter;
};

#endif // GOLANGPLSHIGHLIGHTERFACTORY_H
