/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2016 LiteIDE Team. All rights reserved.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** In addition, as a special exception,  that plugins developed for LiteIDE,
** are allowed to remain closed sourced and can be distributed under any license .
** These rights are included in the file LGPL_EXCEPTION.txt in this package.
**
**************************************************************************/
// Module: golangcode.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golangcode.h"
#include "golangcode_global.h"
#include "fileutil/fileutil.h"
#include "processex/processex.h"
#include "../liteeditor/faketooltip.h"
#include <QProcess>
#include <QTextDocument>
#include <QAbstractItemView>
#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QTimer>
#include <QScrollBar>
#include <QDebug>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

int GolangCode::g_gocodeInstCount = 0;

GolangCode::GolangCode(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app),
    m_editor(0),
    m_completer(0),
    m_closeOnExit(true),
    m_autoUpdatePkg(false)
{
    g_gocodeInstCount++;
    m_gocodeProcess = new Process(this);
    m_gocodeSetProcess = new Process(this);
    m_importProcess = new Process(this);
    m_gocodeProcess->setWorkingDirectory(m_liteApp->applicationPath());
    m_gocodeSetProcess->setWorkingDirectory(m_liteApp->applicationPath());
    connect(m_gocodeProcess,SIGNAL(started()),this,SLOT(started()));
    connect(m_gocodeProcess,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finished(int,QProcess::ExitStatus)));
    connect(m_importProcess,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(importFinished(int,QProcess::ExitStatus)));
    m_envManager = LiteApi::getEnvManager(m_liteApp);
    if (m_envManager) {
        connect(m_envManager,SIGNAL(currentEnvChanged(LiteApi::IEnv*)),this,SLOT(currentEnvChanged(LiteApi::IEnv*)));
    }
    m_envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");
    m_golangAst = LiteApi::findExtensionObject<LiteApi::IGolangAst*>(m_liteApp,"LiteApi.IGolangAst");
    m_pkgImportTip = new ImportPkgTip(m_liteApp,this);
    connect(m_pkgImportTip,SIGNAL(import(QString,int)),this,SLOT(import(QString,int)));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));
    connect(m_liteApp->optionManager(),SIGNAL(applyOption(QString)),this,SLOT(applyOption(QString)));
    connect(m_liteApp,SIGNAL(loaded()),this,SLOT(loaded()));
    applyOption("option/golangcode");
}

void GolangCode::applyOption(QString id)
{
    if (id != "option/golangcode") return;
    m_closeOnExit = m_liteApp->settings()->value(GOLANGCODE_EXITCLOSE,true).toBool();
    m_autoUpdatePkg = m_liteApp->settings()->value(GOLANGCODE_AUTOBUILD,false).toBool();
    QStringList args;
    args << "set" << "autobuild";
    if (m_autoUpdatePkg) {
        args << "true";
    } else {
        args << "false";
    }
    if (!m_gocodeSetProcess->isStop()) {
        m_gocodeSetProcess->stopAndWait(100,2000);
    }
    m_gocodeSetProcess->start(m_gocodeCmd,args);
}

void GolangCode::loaded()
{
    loadPkgList();
}

void GolangCode::import(const QString &import, int startPos)
{
    QPlainTextEdit *ed = LiteApi::getPlainTextEdit(m_editor);
    if (!ed) {
        return;
    }
    QTextBlock block = ed->document()->firstBlock();
    int pos1 = -1;
    int pos2 = -1;
    int pos3 = -1;
    int pos4 = -1;
    int offset = 0;
    while (block.isValid()) {
        QString text = block.text();
        if (text.startsWith("/*")) {
            block = block.next();
            while(block.isValid()) {
                if (block.text().endsWith("*/")) {
                    break;
                }
                block = block.next();
            }
            if (!block.isValid()) {
                break;
            }
        } else if (text.startsWith("var")) {
            break;
        } else if (text.startsWith("func")) {
            break;
        } else if (text.startsWith("package ")) {
            pos1 = block.position()+block.length();
        } else if (pos1 != -1 && text.startsWith("import (")) {
            pos2 = block.position()+block.length();
            break;
        } else if (pos1 != -1 && text.startsWith("import ")) {
            QString path = text.right(text.length()-7).trimmed();
            if (!path.startsWith("\"C\"")) {
                pos3 = block.position()+ 7;
                pos4 = block.position()+block.length();
                break;
            }
        }
        block = block.next();
    }
    if (pos1 < 0) {
        return;
    }
    QString text = "\t\""+import+"\"\n";
    QTextCursor cur = ed->textCursor();
    int orgPos = cur.position();
    cur.beginEditBlock();
    if (pos2 < 0) {
        if (pos3 < 0) {
            pos2 = pos1;
            text = "\nimport (\n\t\""+import+"\"\n)\n";
        } else {
            cur.setPosition(pos3);
            cur.insertText("(\n\t");
            pos2 = pos4+3;
            offset += 3;
            text = "\t\""+import+"\"\n)\n";
        }
    }
    cur.setPosition(pos2);
    cur.insertText(text);
    cur.setPosition(orgPos+text.length()+offset);
    cur.endEditBlock();
    ed->setTextCursor(cur);
    if (orgPos == startPos) {
        prefixChanged(cur,m_lastPrefix,true);
    }
}

bool check_import(const QString &path, const QString &id)
{
    int start = path.indexOf("\"");
    if (start >= 0) {
        int end = path.indexOf("\"",start+1);
        if (end > 0) {
            QString name = path.left(start).trimmed();
            if (!name.isEmpty()) {
                if (name == id) {
                    return true;
                }
            } else {
                QString tmp = path.mid(start+1,end-start-1);
                if (tmp == id) {
                    return true;
                }
                if (tmp.endsWith("/"+id)) {
                    return true;
                }
            }
        }
    }
    return  false;
}

bool GolangCode::findImport(const QString &id)
{
    QPlainTextEdit *ed = LiteApi::getPlainTextEdit(m_editor);
    if (!ed) {
        return false;
    }
    QTextBlock block = ed->document()->firstBlock();
    int pos1 = -1;
    while (block.isValid()) {
        QString text = block.text().trimmed();
        if (text.startsWith("/*")) {
            block = block.next();
            while(block.isValid()) {
                if (block.text().endsWith("*/")) {
                    break;
                }
                block = block.next();
            }
            if (!block.isValid()) {
                break;
            }
        } else if (text.startsWith("var")) {
            break;
        } else if (text.startsWith("func")) {
            break;
        } else if (text.startsWith("package ")) {
            pos1 = block.position()+block.length();
        } else if (pos1 != -1 && text.startsWith("import (")) {
            block = block.next();
            while(block.isValid()) {
                QString text = block.text().trimmed();
                if (text.startsWith(")")) {
                    break;
                }
                //skip
                if (text.startsWith("/*")) {
                    block = block.next();
                    while(block.isValid()) {
                        if (block.text().endsWith("*/")) {
                            break;
                        }
                        block = block.next();
                    }
                    if (!block.isValid()) {
                        break;
                    }
                }
                if (text.startsWith("//")) {
                    block = block.next();
                    continue;
                }
                if (check_import(text,id)) {
                    return true;
                }
                block = block.next();
            }
        } else if (pos1 != -1 && text.startsWith("import ")) {
            QString path = text.right(text.length()-7);
            if (check_import(path,id)) {
                return true;
            }
        }
        block = block.next();
    }
    return false;
}

void GolangCode::broadcast(QString /*module*/,QString /*id*/,QString)
{
//    if (module == "golangpackage" && id == "reloadgopath") {
//        resetGocode();
//    }
}

GolangCode::~GolangCode()
{
    g_gocodeInstCount--;
    if (g_gocodeInstCount == 0 && m_closeOnExit && !m_gocodeCmd.isEmpty()) {
        ProcessEx::startDetachedEx(m_gocodeCmd,QStringList() << "close");
    }
    delete m_gocodeProcess;
    delete m_gocodeSetProcess;
    delete m_importProcess;
}

void GolangCode::resetGocode()
{
    if (m_gocodeCmd.isEmpty()) {
        return;
    }
    QProcessEnvironment env = LiteApi::getGoEnvironment(m_liteApp);
    m_gocodeProcess->setEnvironment(env.toStringList());
    m_gocodeSetProcess->setEnvironment(env.toStringList());
    m_gocodeSetProcess->start(m_gocodeCmd,QStringList() << "set" << "lib-path" << env.value("GOPATH"));
}

void GolangCode::cgoComplete()
{
    QStandardItem *root= m_completer->findRoot(m_preWord);
    QStringList types;
    types << "int" << "uint"
          << "short" << "ushort"
          << "char" << "schar" << "uchar"
          << "long" << "ulong"
          << "longlong" << "ulonglong"
          << "float" << "double";
    QIcon icon = m_golangAst->iconFromTagEnum(LiteApi::TagType,true);
    foreach(QString item, types) {
        m_completer->appendChildItem(root,item,"type","",icon,true);
    }
    icon = m_golangAst->iconFromTagEnum(LiteApi::TagFunc,true);
    m_completer->appendChildItem(root,"CString","func","func(string) *C.char",icon,true);
    m_completer->appendChildItem(root,"GoString","func","func(*C.char) string",icon,true);
    m_completer->appendChildItem(root,"GoStringN","func","func(*C.char, C.int) string",icon,true);
    m_completer->appendChildItem(root,"GoBytes","func","func(unsafe.Pointer, C.int) []byte",icon,true);
    m_completer->updateCompleterModel();
    m_completer->showPopup();
}

void GolangCode::loadPkgList()
{
    QString path = m_liteApp->resourcePath()+("/packages/go/pkglist");
    QFile file(path);
    if (file.open(QFile::ReadOnly)) {
        QByteArray data = file.readAll();
        QString ar = QString::fromUtf8(data);
        ar.replace("\r\n","\n");
        foreach(QString line, ar.split("\n")) {
            line = line.trimmed();
            if (line.isEmpty()) {
                continue;
            }
            QStringList pathList = line.split("/");
            m_pkgListMap.insert(pathList.last(),line);
            m_importList.append(line);
        }
    }
    m_importList.removeDuplicates();
    m_importList << "github.com/"
                 << "golang.org/x/";
    m_allImportList = m_importList;
}

void GolangCode::loadImportsList()
{
    if (!m_importProcess->isStop()) {
        m_importProcess->stopAndWait(100,2000);
    }

    QString cmd = LiteApi::getGotools(m_liteApp);
    if (cmd.isEmpty()) {
        return;
    }
    QStringList args;
    args << "pkgs" << "-list" << "-pkg" << "-skip_goroot";
    m_importProcess->start(cmd,args);
}

void GolangCode::currentEnvChanged(LiteApi::IEnv*)
{    
    QProcessEnvironment env = LiteApi::getGoEnvironment(m_liteApp);
//    if (!LiteApi::hasGoEnv(env)) {
//        return;
//    }
    m_liteApp->appendLog("GolangCode","go environment changed");
    m_gocodeCmd = FileUtil::lookupGoBin("gocode",m_liteApp,true);
    m_gobinCmd = FileUtil::lookupGoBin("go",m_liteApp,false);
    m_gocodeProcess->setProcessEnvironment(env);
    m_importProcess->setProcessEnvironment(env);

    if (m_gocodeCmd.isEmpty()) {
         m_liteApp->appendLog("GolangCode","Could not find gocode (hint: is gocode installed?)",true);
    } else {
         m_liteApp->appendLog("GolangCode",QString("Found gocode at %1").arg(m_gocodeCmd));
    }
    resetGocode();
    loadImportsList();
}

void GolangCode::currentEditorChanged(LiteApi::IEditor *editor)
{
    if (!editor) {
        this->setCompleter(0);
        return;
    }

    if (editor->mimeType() == "text/x-gosrc") {
        LiteApi::ICompleter *completer = LiteApi::findExtensionObject<LiteApi::ICompleter*>(editor,"LiteApi.ICompleter");
        this->setCompleter(completer);
    } else if (editor->mimeType() == "browser/goplay") {
        LiteApi::IEditor* editor = LiteApi::findExtensionObject<LiteApi::IEditor*>(m_liteApp->extension(),"LiteApi.Goplay.IEditor");
        if (editor && editor->mimeType() == "text/x-gosrc") {
            LiteApi::ICompleter *completer = LiteApi::findExtensionObject<LiteApi::ICompleter*>(editor,"LiteApi.ICompleter");
            this->setCompleter(completer);
        }
    } else {
        this->setCompleter(0);
        return;
    }

    m_editor = LiteApi::getTextEditor(editor);
    if (!m_editor) {
        return;
    }
    m_pkgImportTip->setWidget(editor->widget());
    QString filePath = m_editor->filePath();
    if (filePath.isEmpty()) {
        return;
    }
    m_fileInfo.setFile(filePath);
    m_gocodeProcess->setWorkingDirectory(m_fileInfo.absolutePath());
}

void GolangCode::setCompleter(LiteApi::ICompleter *completer)
{
    if (m_completer) {
        disconnect(m_completer,0,this,0);
    }
    m_completer = completer;
    if (m_completer) {
        m_completer->setImportList(m_allImportList);
        if (!m_gocodeCmd.isEmpty()) {
            m_completer->setSearchSeparator(false);
            m_completer->setExternalMode(true);
            connect(m_completer,SIGNAL(prefixChanged(QTextCursor,QString,bool)),this,SLOT(prefixChanged(QTextCursor,QString,bool)));
            connect(m_completer,SIGNAL(wordCompleted(QString,QString,QString)),this,SLOT(wordCompleted(QString,QString,QString)));
        } else {
            m_completer->setSearchSeparator(true);
            m_completer->setExternalMode(false);
        }
    }
}

void GolangCode::prefixChanged(QTextCursor cur,QString pre,bool force)
{
    if (m_completer->completionContext() != LiteApi::CompleterCodeContext) {
        return;
    }

    if (m_gocodeCmd.isEmpty()) {
        return;
    }

//    if (m_completer->completer()->completionPrefix().startsWith(pre)) {
//       // qDebug() << pre << m_completer->completer()->completionPrefix();
//       // return;
//    }
    if (!m_gocodeProcess->isStop()) {
        return;
    }
    int offset = -1;
    if (pre.endsWith('.')) {
        m_preWord = pre;
        offset = 0;
    } else if (pre.length() == m_completer->prefixMin()) {
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
    }

    m_prefix = pre;
    m_lastPrefix = m_prefix;

    if (!m_preWord.isEmpty()) {
        m_completer->clearItemChilds(m_preWord);
    }

    if (m_preWord == "C.") {
        cgoComplete();
        return;
    }
    if (m_prefix.lastIndexOf("..") > 0) {
        m_pkgImportTip->hide();
        return;
    }

    QString src = cur.document()->toPlainText();
    src = src.replace("\r\n","\n");
    m_writeData = src.left(cur.position()).toUtf8();
    QStringList args;
    args << "-in" << "" << "-f" << "csv" << "autocomplete" << m_fileInfo.fileName() << QString::number(m_writeData.length()+offset);
    m_writeData = src.toUtf8();
    m_gocodeProcess->setWorkingDirectory(m_fileInfo.absolutePath());
    m_gocodeProcess->start(m_gocodeCmd,args);
}

void GolangCode::wordCompleted(QString,QString,QString)
{
    m_prefix.clear();
}

void GolangCode::started()
{
    if (m_writeData.isEmpty()) {
        m_gocodeProcess->closeWriteChannel();
        return;
    }
    m_gocodeProcess->write(m_writeData);
    m_gocodeProcess->closeWriteChannel();
    m_writeData.clear();
}

void GolangCode::finished(int code,QProcess::ExitStatus)
{
    if (code != 0) {
        return;
    }

    if (m_prefix.isEmpty()) {
        return;
    }

    if (m_prefix != m_lastPrefix) {
        m_prefix.clear();
        return;
    }

    QByteArray read = m_gocodeProcess->readAllStandardOutput();

    QList<QByteArray> all = read.split('\n');
    //func,,Fprint,,func(w io.Writer, a ...interface{}) (n int, error os.Error)
    //type,,Formatter,,interface
    //const,,ModeExclusive,,
    //var,,Args,,[]string
    int n = 0;
    QIcon icon;
    QStandardItem *root= m_completer->findRoot(m_preWord);
    foreach (QByteArray bs, all) {
        QStringList word = QString::fromUtf8(bs,bs.size()).split(",,");
        if (word.count() != 3) {
            continue;
        }
        if (word.at(0) == "PANIC") {
            continue;
        }
        LiteApi::ASTTAG_ENUM tag = LiteApi::TagNone;
        QString kind = word.at(0);
        QString info = word.at(2);
        if (kind == "package") {
            tag = LiteApi::TagPackage;
        } else if (kind == "func") {
            tag = LiteApi::TagFunc;
        } else if (kind == "var") {
            tag = LiteApi::TagValue;
        } else if (kind == "const") {
            tag = LiteApi::TagConst;
        } else if (kind == "type") {
            if (info == "interface") {
                tag = LiteApi::TagInterface;
            } else if (info == "struct") {
                tag = LiteApi::TagStruct;
            } else {
                tag = LiteApi::TagType;
            }
        }

        if (m_golangAst) {
            icon = m_golangAst->iconFromTagEnum(tag,true);
        }
        //m_completer->appendItemEx(m_preWord+word.at(1),kind,info,icon,true);
        m_completer->appendChildItem(root,word.at(1),kind,info,icon,true);
        n++;
    }
    m_lastPrefix = m_prefix;
    m_prefix.clear();
    if (n >= 1) {
        m_completer->updateCompleterModel();
        m_completer->showPopup();
    }
    if (n == 0 && m_lastPrefix.endsWith(".")) {
        QString id = m_lastPrefix.left(m_lastPrefix.length()-1);
        QStringList pkgs = m_pkgListMap.values(id);
        if (!pkgs.isEmpty() && !findImport(id)) {
            QPlainTextEdit *ed = LiteApi::getPlainTextEdit(m_editor);
            if (ed) {
                int pos = ed->textCursor().position();
                pkgs.sort();
                m_pkgImportTip->showPkgHint(pos,pkgs,ed);
            }
        }
    }
}

void GolangCode::importFinished(int code,QProcess::ExitStatus)
{
    if (code != 0) {
        return;
    }
    QByteArray read = m_importProcess->readAllStandardOutput();
    QString data = QString::fromUtf8(read);
    QStringList importList = data.split('\n');
    importList.removeDuplicates();
    importList.sort();
    m_allImportList = m_importList;
    m_allImportList.append(importList);
    m_allImportList.removeDuplicates();
    if (m_completer) {
        m_completer->setImportList(m_allImportList);
    }
}

ImportPkgTip::ImportPkgTip(LiteApi::IApplication *app, QObject *parent)
    : QObject(parent), m_liteApp(app)
{
    m_editWidget = 0;
    m_startPos = 0;
    m_pkgIndex = 0;
    m_escapePressed = false;
    m_enterPressed = false;
    m_popup = new FakeToolTip();
    //m_popup->setFocusPolicy(Qt::NoFocus);
    m_infoLabel = new QLabel;
    m_pkgLabel = new QLabel;
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->addWidget(m_infoLabel);
    layout->addWidget(m_pkgLabel);
    m_popup->setLayout(layout);

    qApp->installEventFilter(this);
}

ImportPkgTip::~ImportPkgTip()
{
    delete m_popup;
}

void ImportPkgTip::showPkgHint(int startpos, const QStringList &pkg, QPlainTextEdit *ed)
{
    const QDesktopWidget *desktop = QApplication::desktop();
#ifdef Q_WS_MAC
    const QRect screen = desktop->availableGeometry(desktop->screenNumber(ed));
#else
    const QRect screen = desktop->screenGeometry(desktop->screenNumber(ed));
#endif
    m_pkg = pkg;
    m_startPos = startpos;
    m_enterPressed = false;
    m_escapePressed = false;
    m_pkgIndex = 0;
    const QSize sz = m_popup->minimumSizeHint();
    QTextCursor cur = ed->textCursor();
    cur.setPosition(startpos);
    QPoint pos = ed->cursorRect(cur).topLeft();
    pos.setY(pos.y() - sz.height() - 1);
    pos = ed->mapToGlobal(pos);
    if (pos.x() + sz.width() > screen.right())
        pos.setX(screen.right() - sz.width());
    m_infoLabel->setText(tr("warning, pkg not find, please enter to import :"));
    if (m_pkg.size() == 1) {
        m_pkgLabel->setText(m_pkg[0]);
    } else {
        m_pkgLabel->setText(QString("[%1/%2] \"%3\"").arg(m_pkgIndex+1).arg(m_pkg.size()).arg(m_pkg[m_pkgIndex]));
    }
    m_popup->move(pos);
    if (!m_popup->isVisible()) {
        m_popup->show();
    }
}

void ImportPkgTip::hide()
{
    m_popup->hide();
}

void ImportPkgTip::setWidget(QWidget *widget)
{
    hide();
    m_editWidget = widget;
}

bool ImportPkgTip::eventFilter(QObject *obj, QEvent *e)
{
    if (!m_popup->isVisible()) {
        return QObject::eventFilter(obj,e);
    }
    switch (e->type()) {
    case QEvent::ShortcutOverride:
        if (m_popup->isVisible() && static_cast<QKeyEvent*>(e)->key() == Qt::Key_Escape) {
            m_escapePressed = true;
            e->accept();
        } else if (static_cast<QKeyEvent*>(e)->modifiers() & Qt::ControlModifier) {
            m_popup->hide();
        }
        break;
    case QEvent::KeyPress: {
            QKeyEvent *ke = static_cast<QKeyEvent*>(e);
            if (ke->key() == Qt::Key_Escape) {
                m_escapePressed = true;
            } else if (ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter) {
                m_enterPressed = true;
                e->accept();
                return true;
            } else if (ke->key() == Qt::Key_Up) {
                if (m_pkg.size() > 1) {
                    e->accept();
                    m_pkgIndex--;
                    if (m_pkgIndex < 0) {
                        m_pkgIndex = m_pkg.size()-1;
                    }
                    m_pkgLabel->setText(QString("[%1/%2] \"%3\"").arg(m_pkgIndex+1).arg(m_pkg.size()).arg(m_pkg[m_pkgIndex]));
                }
                return true;
            } else if (ke->key() == Qt::Key_Down) {
                if (m_pkg.size() > 1) {
                    e->accept();
                    m_pkgIndex++;
                    if (m_pkgIndex >= m_pkg.size()) {
                        m_pkgIndex = 0;
                    }
                    m_pkgLabel->setText(QString("[%1/%2] \"%3\"").arg(m_pkgIndex+1).arg(m_pkg.size()).arg(m_pkg[m_pkgIndex]));
                }
                return true;
            }
        }
        break;
    case QEvent::KeyRelease: {
            QKeyEvent *ke = static_cast<QKeyEvent*>(e);
            if (ke->key() == Qt::Key_Escape && m_escapePressed) {
                hide();
            } else if ( (ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter) &&
                        m_enterPressed)            {
                e->accept();
                m_enterPressed = false;
                hide();
                emit import(m_pkg[m_pkgIndex],m_startPos);
            } else if (ke->key() == Qt::Key_Up) {
                return true;
            } else if (ke->key() == Qt::Key_Down) {
                return true;
            } else if (ke->text() != "."){
                hide();
            }
        }
        break;
    case QEvent::FocusOut:
    case QEvent::WindowDeactivate:
    case QEvent::Resize:
        if (obj != m_editWidget)
            break;
        hide();
        break;
    case QEvent::Move:
        if (obj != m_liteApp->mainWindow())
            break;
        hide();
        break;
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::Wheel: {
            hide();
        }
        break;
    default:
        break;
    }
    return false;
}
