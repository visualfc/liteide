#ifndef RUSTEDIT_H
#define RUSTEDIT_H

#include <liteapi/liteapi.h>
#include <liteeditorapi/liteeditorapi.h>
#include <liteenvapi/liteenvapi.h>

class RustEdit : public QObject
{
    Q_OBJECT
public:
    explicit RustEdit(LiteApi::IApplication *app, QObject *parent = 0);

    void setCompleter(LiteApi::ICompleter *completer);
signals:

public slots:
    void currentEnvChanged(LiteApi::IEnv *env);
    void editorCreated(LiteApi::IEditor *editor);
    void currentEditorChanged(LiteApi::IEditor *editor);
    void prefixChanged(const QTextCursor &cur,const QString &pre,bool force);
    void wordCompleted(QString,QString,QString);
    void finished(int code,QProcess::ExitStatus);
protected:
    LiteApi::IApplication *m_liteApp;
    LiteApi::ICompleter   *m_completer;
    LiteApi::ILiteEditor  *m_editor;
    QProcess    *m_process;
    QString     m_racerCmd;
    QString     m_preWord;
    QString     m_prefix;
    QString     m_lastPrefix;
};

#endif // RUSTEDIT_H
