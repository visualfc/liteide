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

static bool setTextCharStyle(QTextCharFormat &fmt, const QString &name, const ColorStyleScheme *scheme)
{
    const ColorStyle *style = scheme->findStyle(name);
    if (!style) {
        return false;
    }
    QColor fore = style->foregound();
    if (fore.isValid()) {
        fmt.setForeground(fore);
    }
    QColor back = style->background();
    if (back.isValid()) {
        fmt.setBackground(back);
    }
    fmt.setFontItalic(style->italic());
    if (style->bold()) {
        fmt.setFontWeight(QFont::Bold);
    } else {
        fmt.setFontWeight(QFont::Normal);
    }
    return true;
}

void KateHighlighter::setColorStyle(TextEditor::SyntaxHighlighter *h,const ColorStyleScheme *scheme)
{
    Highlighter *highlighter = static_cast<Highlighter*>(h);
    if (!highlighter) {
        return;
    }

    QTextCharFormat fmt1;
    if (!setTextCharStyle(fmt1,"VisualWhitespace",scheme)) {
        fmt1.setForeground(Qt::lightGray);
    }
    highlighter->configureFormat(Highlighter::VisualWhitespace, fmt1);

    QTextCharFormat fmt2;
    if (!setTextCharStyle(fmt2,"Keyword",scheme)) {
        fmt2.setForeground(Qt::darkBlue);
        fmt2.setFontWeight(QFont::Bold);
    }
    highlighter->configureFormat(Highlighter::Keyword, fmt2);

    QTextCharFormat fmt3;
    if (!setTextCharStyle(fmt3,"DataType",scheme)) {
        fmt3.setForeground(Qt::darkBlue);//Qt::darkMagenta);
    }
    highlighter->configureFormat(Highlighter::DataType, fmt3);

    QTextCharFormat fmt4;
    if (!setTextCharStyle(fmt4,"Function",scheme)) {
        fmt4.setForeground(Qt::blue);
    }
    highlighter->configureFormat(Highlighter::Function,fmt4);

    QTextCharFormat fmt5;
    if (!setTextCharStyle(fmt5,"Comment",scheme)) {
        fmt5.setForeground(Qt::darkGreen);
    }
    highlighter->configureFormat(Highlighter::Comment, fmt5);

    QTextCharFormat fmt6;
    if (!setTextCharStyle(fmt6,"Number",scheme)) {
        fmt6.setForeground(Qt::darkMagenta);
    }
    // Using C_NUMBER for all kinds of numbers.
    highlighter->configureFormat(Highlighter::Decimal, fmt6);
    highlighter->configureFormat(Highlighter::BaseN, fmt6);
    highlighter->configureFormat(Highlighter::Float, fmt6);


    QTextCharFormat fmt7;
    if (!setTextCharStyle(fmt7,"String",scheme)) {
        fmt7.setForeground(Qt::darkGreen);
    }
    // Using C_STRING for strings and chars.
    highlighter->configureFormat(Highlighter::Char, fmt7);
    highlighter->configureFormat(Highlighter::String, fmt7);


    QTextCharFormat fmt8;
    if (!setTextCharStyle(fmt8,"Error",scheme)) {
        fmt8.setForeground(Qt::red);
    }
    highlighter->configureFormat(Highlighter::Error,fmt8);

    highlighter->rehighlight();
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
        h->setDefaultContext(def->initialContext());
    }
    return h;
}
