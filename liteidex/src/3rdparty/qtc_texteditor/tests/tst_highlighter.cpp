#include <QtTest>
#include <QTextDocument>
#include "generichighlighter/highlighter.h"
#include "generichighlighter/highlightdefinition.h"
#include "generichighlighter/specificrules.h"
#include "generichighlighter/itemdata.h"

using namespace TextEditor::Internal;

class TestHighlighter : public Highlighter
{
public:
    TestHighlighter(QTextDocument *document) : Highlighter(document) {}
    bool isBroken() const { return m_isBroken; }
    bool hasContexts() const { return !m_contexts.isEmpty(); }
};

static QSharedPointer<Context> addContext(const QSharedPointer<HighlightDefinition> &definition,
                                        const QString &name, bool initial = false)
{
    QSharedPointer<Context> context = definition->createContext(name, initial);
    context->setDefinition(definition);
    context->setItemData("Normal");
    context->setLineEndContext("#stay");
    return context;
}

static QSharedPointer<DetectCharRule> addRule(const QSharedPointer<Context> &context,
                                            const QString &target, bool lookAhead = true)
{
    QSharedPointer<DetectCharRule> rule(new DetectCharRule);
    rule->setChar("x");
    rule->setContext(target);
    rule->setLookAhead(lookAhead ? "true" : "false");
    rule->setItemData("Normal");
    rule->setDefinition(context->definition());
    context->addRule(rule);
    return rule;
}

class HighlighterTest : public QObject
{
    Q_OBJECT
private slots:
    void ruleLoops_data()
    {
        QTest::addColumn<QString>("mode");
        QTest::addColumn<bool>("broken");
        QTest::newRow("lookahead-stay") << QString("stay") << true;
        QTest::newRow("lookahead-base-pop") << QString("pop") << true;
        QTest::newRow("empty-string-match") << QString("empty") << true;
        QTest::newRow("lookahead-context-cycle") << QString("cycle") << true;
        QTest::newRow("fallthrough-stay") << QString("fallthrough") << true;
        QTest::newRow("fallthrough-context-cycle") << QString("fallthrough-cycle") << true;
        QTest::newRow("recursive-children") << QString("children") << true;
        QTest::newRow("consuming-rule") << QString("consume") << false;
        QTest::newRow("valid-lookahead") << QString("valid") << false;
    }

    void ruleLoops()
    {
        QFETCH(QString, mode);
        QFETCH(bool, broken);
        QSharedPointer<HighlightDefinition> definition(new HighlightDefinition);
        definition->createItemData("Normal")->setStyle("dsNormal");
        definition->itemData("Normal")->setColor("#ff0000");
        QSharedPointer<Context> base = addContext(definition, "base", true);
        if (mode == "fallthrough" || mode == "fallthrough-cycle") {
            base->setFallthrough("true");
            base->setFallthroughContext(mode == "fallthrough" ? "#stay" : "other");
            if (mode == "fallthrough-cycle") {
                QSharedPointer<Context> other = addContext(definition, "other");
                other->setFallthrough("true");
                other->setFallthroughContext("#pop");
            }
        } else if (mode == "cycle" || mode == "valid") {
            QSharedPointer<Context> other = addContext(definition, "other");
            addRule(base, "other");
            addRule(other, "#pop", mode == "cycle");
        } else if (mode == "empty") {
            QSharedPointer<StringDetectRule> rule(new StringDetectRule);
            rule->setString("");
            rule->setInsensitive("false");
            rule->setContext("#stay");
            rule->setDefinition(definition);
            base->addRule(rule);
        } else if (mode == "children") {
            QSharedPointer<DetectCharRule> parent = addRule(base, "#stay");
            for (int i = 0; i < 140; ++i) {
                QSharedPointer<DetectCharRule> child(new DetectCharRule);
                child->setChar("x");
                child->setLookAhead("true");
                child->setDefinition(definition);
                parent->addChild(child);
                parent = child;
            }
        } else {
            addRule(base, mode == "pop" ? "#pop" : "#stay", mode != "consume");
        }

        QTextDocument document;
        // More characters than the no-progress budget verifies that consuming
        // matches reset the budget rather than disabling normal long lines.
        const QString text = QString(4096, QLatin1Char('x')) + "\nx";
        document.setPlainText(text);
        TestHighlighter highlighter(&document);
        highlighter.setDefaultContext(base);
        highlighter.rehighlight();
        QCOMPARE(highlighter.isBroken(), broken);
        QVERIFY(!highlighter.hasContexts());
        QCOMPARE(document.toPlainText(), text);
        if (!broken) {
            const auto formats = document.begin().layout()->formats();
            QVERIFY(!formats.isEmpty());
            QCOMPARE(formats.first().start, 0);
            QCOMPARE(formats.first().length, 4096);
            QCOMPARE(formats.first().format.foreground().color(), QColor(Qt::red));
        }
        highlighter.rehighlight();
        QCOMPARE(highlighter.isBroken(), broken);
    }
};

QTEST_MAIN(HighlighterTest)
#include "tst_highlighter.moc"
