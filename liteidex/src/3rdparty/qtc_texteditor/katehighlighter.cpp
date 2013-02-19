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

    QTextCharFormat fmt_vw;
    if (!setTextCharStyle(fmt_vw,"VisualWhitespace",scheme)) {
        fmt_vw.setForeground(Qt::lightGray);
    }
    highlighter->configureFormat(Highlighter::VisualWhitespace, fmt_vw);

    QTextCharFormat fmt_kw;
    if (!setTextCharStyle(fmt_kw,"Keyword",scheme)) {
        fmt_kw.setForeground(Qt::darkBlue);
        fmt_kw.setFontWeight(QFont::Bold);
    }
    highlighter->configureFormat(Highlighter::Keyword, fmt_kw);

    QTextCharFormat fmt_dt;
    if (!setTextCharStyle(fmt_dt,"DataType",scheme)) {
        fmt_dt.setForeground(Qt::darkBlue);//Qt::darkMagenta);
    }
    highlighter->configureFormat(Highlighter::DataType, fmt_dt);

    QTextCharFormat fmt_fn;
    if (!setTextCharStyle(fmt_fn,"Function",scheme)) {
        fmt_fn.setForeground(Qt::blue);
    }
    highlighter->configureFormat(Highlighter::Function,fmt_fn);

    QTextCharFormat fmt_cmn;
    if (!setTextCharStyle(fmt_cmn,"Comment",scheme)) {
        fmt_cmn.setForeground(Qt::darkGreen);
    }
    highlighter->configureFormat(Highlighter::Comment, fmt_cmn);

    QTextCharFormat fmt_dd;
    if (!setTextCharStyle(fmt_dd,"Decimal",scheme)) {
        fmt_dd.setForeground(Qt::darkMagenta);
    }
    // Using C_NUMBER for all kinds of numbers.
    highlighter->configureFormat(Highlighter::Decimal, fmt_dd);

    QTextCharFormat fmt_db;
    if (!setTextCharStyle(fmt_db,"BaseN",scheme)) {
        fmt_db.setForeground(Qt::darkMagenta);
    }
    highlighter->configureFormat(Highlighter::BaseN, fmt_db);

    QTextCharFormat fmt_df;
    if (!setTextCharStyle(fmt_df,"Float",scheme)) {
        fmt_df.setForeground(Qt::darkMagenta);
    }
    highlighter->configureFormat(Highlighter::Float, fmt_df);


    QTextCharFormat fmt_ch;
    if (!setTextCharStyle(fmt_ch,"Char",scheme)) {
        fmt_ch.setForeground(Qt::darkGreen);
    }
    // Using C_STRING for strings and chars.
    highlighter->configureFormat(Highlighter::Char, fmt_ch);

    QTextCharFormat fmt_cs;
    if (!setTextCharStyle(fmt_cs,"String",scheme)) {
        fmt_cs.setForeground(Qt::darkGreen);
    }
    highlighter->configureFormat(Highlighter::String, fmt_cs);

    QTextCharFormat fmt_rm;
    if (!setTextCharStyle(fmt_rm,"RegionMarker",scheme)) {
        fmt_rm.setForeground(Qt::yellow);
    }
    highlighter->configureFormat(Highlighter::RegionMarker,fmt_rm);

    QTextCharFormat fmt_alert;
    if (!setTextCharStyle(fmt_alert,"Alert",scheme)) {
        fmt_alert.setForeground(Qt::red);
    }
    highlighter->configureFormat(Highlighter::Alert,fmt_alert);

    QTextCharFormat fmt_err;
    if (!setTextCharStyle(fmt_err,"Error",scheme)) {
        fmt_err.setForeground(Qt::red);
    }
    highlighter->configureFormat(Highlighter::Error,fmt_err);

    QTextCharFormat fmt_sym;
    if (!setTextCharStyle(fmt_sym,"Symbol",scheme)) {
        fmt_sym.setForeground(Qt::red);
    }
    highlighter->configureFormat(Highlighter::Symbol,fmt_sym);

    QTextCharFormat fmt_bf;
    if (!setTextCharStyle(fmt_bf,"BuiltinFunc",scheme)) {
        fmt_bf.setForeground(Qt::blue);
    }
    highlighter->configureFormat(Highlighter::BuiltinFunc,fmt_bf);

    QTextCharFormat fmt_pre;
    if (!setTextCharStyle(fmt_pre,"Predeclared",scheme)) {
        fmt_pre.setForeground(Qt::blue);
    }
    highlighter->configureFormat(Highlighter::Predeclared,fmt_pre);

    QTextCharFormat fmt_fc;
    if (!setTextCharStyle(fmt_fc,"FuncDecl",scheme)) {
        fmt_fc.setForeground(Qt::blue);
    }
    highlighter->configureFormat(Highlighter::FuncDecl,fmt_fc);

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
