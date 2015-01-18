#include "rustedit.h"
#include "fileutil/fileutil.h"
#include <QProcess>
#include <QTemporaryFile>
#include <QFileInfo>
#include <QDir>
#include <QFile>

RustEdit::RustEdit(LiteApi::IApplication *app, QObject *parent)
    : QObject(parent), m_liteApp(app)
{
    m_completer = 0;
    m_editor = 0;
    m_racerCmd = "racer";
    m_process = new QProcess(this);
    connect(m_liteApp->editorManager(),SIGNAL(editorCreated(LiteApi::IEditor*)),this,SLOT(editorCreated(LiteApi::IEditor*)));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));
    connect(m_process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finished(int,QProcess::ExitStatus)));
    LiteApi::IEnvManager *env = LiteApi::getEnvManager(m_liteApp);
    if (env) {
        connect(env,SIGNAL(currentEnvChanged(LiteApi::IEnv*)),this,SLOT(currentEnvChanged(LiteApi::IEnv*)));
        currentEnvChanged(env->currentEnv());
    }
}

void RustEdit::editorCreated(LiteApi::IEditor *editor)
{
    if (editor->mimeType() != "text/x-rust") {
        return;
    }
}

void RustEdit::currentEditorChanged(LiteApi::IEditor *editor)
{
    if (!editor) {
        return;
    }
    if (editor->mimeType() != "text/x-rust") {
        return;
    }
    LiteApi::ICompleter *completer = LiteApi::findExtensionObject<LiteApi::ICompleter*>(editor,"LiteApi.ICompleter");
    if (completer) {
        setCompleter(completer);
    }
    m_editor = LiteApi::getLiteEditor(editor);
}

void RustEdit::prefixChanged(const QTextCursor &cur, const QString &pre, bool force)
{
    if (m_completer->completionContext() != LiteApi::CompleterCodeContext) {
        return;
    }

    if (m_racerCmd.isEmpty()) {
        return;
    }

    if (m_process->state() != QProcess::NotRunning) {
        return;
    }
    /*
    int offset = -1;
    if (pre.endsWith('.')) {
        m_preWord = pre;
        offset = 0;
    } else if (pre.length() == 1) {
        m_preWord.clear();
    } else {
        if (!force) {
            return;
        }
        m_preWord.clear();
        int index = pre.lastIndexOf(".");
        if (index != -1) {
            m_preWord = pre.left(index);
        }
    }*/
    if (!pre.endsWith("::")) {
        return;
    }
    m_prefix = pre;
    m_lastPrefix = m_prefix;

    QStringList args;
    args << "complete";
    args << QString("%1").arg(m_editor->line()+1);
    args << QString("%1").arg(m_editor->column());
    args << "racer_temp.rs";
    QString path = QDir::tempPath();
    QFile file(path+"//racer_temp.rs");
    if (file.open(QFile::WriteOnly|QFile::Truncate)) {
        file.write(m_editor->utf8Data());
        file.close();
    }
    m_process->setWorkingDirectory(path);
    m_process->start(m_racerCmd,args);
}

void RustEdit::wordCompleted(QString, QString, QString)
{
    //m_completer->clearTemp();
}

void RustEdit::finished(int code, QProcess::ExitStatus)
{
    if (code != 0) {
        return;
    }
    QByteArray data = m_process->readAllStandardOutput();
    int count = 0;
    foreach (QString line, QString::fromUtf8(data).split("\n")) {
        if (line.startsWith("MATCH")) {
            QStringList array = line.mid(5).split(",",QString::SkipEmptyParts);
            if (array.size() > 0) {
                count++;
                m_completer->appendItem(m_prefix+array[0].trimmed(),QIcon(),true);
            }
        }
    }
    if (count > 0) {
        m_completer->updateCompleterModel();
        m_completer->showPopup();
    }
}

void RustEdit::setCompleter(LiteApi::ICompleter *completer)
{
    if (m_completer) {
        disconnect(m_completer,0,this,0);
    }
    m_completer = completer;
    if (m_completer) {
        if (!m_racerCmd.isEmpty()) {
            m_completer->setSearchSeparator(false);
            m_completer->setExternalMode(false);
            m_completer->setSeparator("::");
            connect(m_completer,SIGNAL(prefixChanged(QTextCursor,QString,bool)),this,SLOT(prefixChanged(QTextCursor,QString,bool)));
            connect(m_completer,SIGNAL(wordCompleted(QString,QString,QString)),this,SLOT(wordCompleted(QString,QString,QString)));
        } else {
            m_completer->setSearchSeparator(true);
            m_completer->setExternalMode(false);
        }
    }
}

void RustEdit::currentEnvChanged(LiteApi::IEnv*)
{
    QProcessEnvironment env = LiteApi::getCurrentEnvironment(m_liteApp);
    m_racerCmd = FileUtil::lookPath("racer",env,false);
    if (m_racerCmd.isEmpty()) {
#ifdef Q_OS_WIN
        m_racerCmd = "c:\\rust\\bin\\racer.exe";
#endif
    }
    m_process->setProcessEnvironment(env);
}
