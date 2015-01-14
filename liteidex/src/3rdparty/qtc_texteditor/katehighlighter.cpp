#include "katehighlighter.h"
#include "generichighlighter/highlighter.h"
#include "generichighlighter/highlightdefinition.h"
#include "generichighlighter/highlightdefinitionhandler.h"
#include "generichighlighter/highlighterexception.h"
#include "generichighlighter/highlightdefinitionmetadata.h"
#include "generichighlighter/manager2.h"
#include "texteditorconstants.h"

#include <QDir>
#include <QFileInfo>
#include <QXmlStreamReader>

#include "colorstyle/colorstyle.h"

using namespace TextEditor::Internal;
using namespace TextEditor::Constants;
using namespace TextEditor;

KateHighlighter::KateHighlighter(QObject *parent) :
    QObject(parent)
{
}

void KateHighlighter::loadPath(const QString &definitionsPaths)
{
    Manager2::instance()->loadPath(QStringList(definitionsPaths));
}

QStringList KateHighlighter::mimeTypes() const
{
    return Manager2::instance()->mimeTypes();
}

QString KateHighlighter::mimeTypeName(const QString &mimeType) const
{
    QString id = Manager2::instance()->definitionIdByMimeType(mimeType);
    QSharedPointer<HighlightDefinitionMetaData> data = Manager2::instance()->definitionMetaData(id);
    if (data) {
        return data->name();
    }
    return QString();
}

QStringList KateHighlighter::mimeTypePatterns(const QString &mimeType) const
{
    QString id = Manager2::instance()->definitionIdByMimeType(mimeType);
    QSharedPointer<HighlightDefinitionMetaData> data = Manager2::instance()->definitionMetaData(id);
    if (data) {
        return data->patterns();
    }
    return QStringList();
}

TextEditor::SyntaxHighlighter *KateHighlighter::create(QTextDocument *doc, const QString &mimeType)
{
    Highlighter *h = new Highlighter(doc);
    //reset(h);
    QString id = Manager2::instance()->definitionIdByMimeType(mimeType);
    QSharedPointer<HighlightDefinition> def = Manager2::instance()->definition(id);
    if (def) {
        SyntaxComment comment;
        comment.singleLineComment = def->singleLineComment();
        comment.multiLineCommentStart = def->multiLineCommentStart();
        comment.multiLineCommentEnd = def->multiLineCommentEnd();
        comment.isCommentAfterWhiteSpaces = def->isCommentAfterWhiteSpaces();
        h->setupComment(comment);
        h->setDefaultContext(def->initialContext());
    }
    return h;
}
