#ifndef JSONEDIT_H
#define JSONEDIT_H

#include <liteapi/liteapi.h>
#include <liteeditorapi/liteeditorapi.h>
#include <qjson/include/QJson/Parser>
#include <qjson/include/QJson/Serializer>

class JsonEdit : public QObject
{
    Q_OBJECT
public:
    explicit JsonEdit(LiteApi::IApplication *app, LiteApi::IEditor *editor, QObject *parent = 0);

signals:

public slots:
    void editorSaved(LiteApi::IEditor*);
    void verify();
    void indentNone();
    void indentCompact();
    void indentMinimum();
    void indentMedium();
    void indentFull();
protected:
    void format(QJson::IndentMode mode);
    LiteApi::IApplication *m_liteApp;
    LiteApi::ILiteEditor  *m_editor;
};

#endif // JSONEDIT_H
