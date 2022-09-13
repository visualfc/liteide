/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2020 LiteIDE. All rights reserved.
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
// Module: vtermwidget.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "vtermwidget.h"
#include <QProcess>
#include <QResizeEvent>
#include <QScrollBar>
#include <QDebug>
#include <signal.h>
#include <QKeySequence>
#include <QClipboard>
#include <QApplication>
#include <QMenu>
#include <QAction>

#if defined(Q_OS_MAC)
# define TermControlModifier Qt::MetaModifier
#else
# define TermControlModifier Qt::ControlModifier
#endif


VTermWidget::VTermWidget(LiteApi::IApplication *app,QWidget *parent) : VTermWidgetBase(app,24,80,parent),m_liteApp(app)
{
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    m_process = PtyQt::createPtyProcess(IPtyProcess::AutoPty);
    m_contextMenu = new QMenu(this);
    m_bStarted = false;

    m_copy = new QAction(tr("Copy"),this);
    m_copy->setShortcut(QKeySequence::Copy);
    m_copy->setShortcutContext(Qt::WidgetShortcut);

    m_paste = new QAction(tr("Paste"),this);
    m_paste->setShortcut(QKeySequence::Paste);
    m_paste->setShortcutContext(Qt::WidgetShortcut);

    m_selectAll = new QAction(tr("Select All"),this);
    m_selectAll->setShortcut(QKeySequence::SelectAll);
    m_selectAll->setShortcutContext(Qt::WidgetShortcut);

    m_contextMenu->addAction(m_copy);
    m_contextMenu->addAction(m_paste);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_selectAll);

    connect(m_process,SIGNAL(started()),this,SIGNAL(started()));
    connect(m_process,SIGNAL(exited()),this,SIGNAL(exited()));
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(contextMenuRequested(QPoint)));
    connect(m_copy,SIGNAL(triggered()),this,SLOT(copy()));
    connect(m_paste,SIGNAL(triggered()),this,SLOT(paste()));
    connect(m_selectAll,SIGNAL(triggered()),this,SLOT(selectAll()));
}

VTermWidget::~VTermWidget()
{
    delete m_process;
}

bool VTermWidget::isAvailable() const
{
    return m_process->isAvailable();
}

void VTermWidget::start(const QString &program, const QStringList &arguments, const QString &workingDirectory, QStringList env)
{
    m_bStarted = false;
    if (!m_process->isAvailable()) {
        qDebug() << "pty process invalid";
        return;
    }
    bool b = m_process->startProcess(program,arguments,workingDirectory,env,qint16(m_rows),qint16(m_cols));
    if (!b) {
        qDebug() << m_process->lastError();
        return;
    }
    m_bStarted = true;
    connect(m_process->notifier(),SIGNAL(readyRead()),this,SLOT(readyRead()));
    connect(this,SIGNAL(sizeChanged(int,int)),this,SLOT(resizePty(int,int)));
}

bool VTermWidget::isStarted() const
{
    return m_bStarted;
}

IPtyProcess *VTermWidget::process() const
{
    return m_process;
}

void VTermWidget::copy()
{
    QString text = selectedText();
    if (!text.isEmpty()) {
        qApp->clipboard()->setText(text);
    }
}

void VTermWidget::paste()
{
    QString text = qApp->clipboard()->text();
    if (!text.isEmpty()) {
        m_process->write(text.toUtf8());
    }
}

void VTermWidget::readyRead()
{
    QByteArray data = m_process->readAll();
    if (data.isEmpty()) {
        return;
    }
    this->inputWrite(data);
}

void VTermWidget::resizeEvent(QResizeEvent *e)
{
    VTermWidgetBase::resizeEvent(e);
}

void VTermWidget::keyPressEvent(QKeyEvent *e)
{    
    if (!m_bStarted) {
        return;
    }
//#ifdef Q_OS_WIN
    // WINDOWS copy & clear selection
    if (hasSelection() && (e == QKeySequence::Copy || e->key() == Qt::Key_Return)) {
        QString text = selectedText();
        if (!text.isEmpty()) {
            qApp->clipboard()->setText(text);
            clearSelection();
            return;
        }
    }
//#else
//    if (e == QKeySequence::Copy) {
//        QString text = selectedText();
//        if (!text.isEmpty()) {
//            qApp->clipboard()->setText(text);
//        }
//        return;
//    }
//#endif
    else if (e == QKeySequence::Paste) {
        QString text = qApp->clipboard()->text();
        if (!text.isEmpty()) {
            m_process->write(text.toUtf8());
        }
        return;
    } else if (e == QKeySequence::SelectAll) {
        selectAll();
        return;
    }
    if ((e->modifiers() & TermControlModifier) ) {
        QChar c(e->key());
        char asciiVal = c.toUpper().toLatin1();
        QByteArray array;
        if (asciiVal >= 0x41 && asciiVal <= 0x5f) {
            array.push_back(asciiVal-0x40);
            m_process->write(array);
            return;
        }
    }
    VTermWidgetBase::keyPressEvent(e);
}

void VTermWidget::resizePty(int rows, int cols)
{
    if (!m_bStarted) {
        return;
    }
    m_process->resize(cols,rows);
}

void VTermWidget::contextMenuRequested(const QPoint &pt)
{
    m_copy->setEnabled(this->hasSelection());
    m_paste->setEnabled(!qApp->clipboard()->text().isEmpty());
    QPoint globalPos = this->mapToGlobal(pt);
    m_contextMenu->popup(globalPos);
}

void VTermWidget::write_data(const char *buf, int len)
{
    if (!m_bStarted) {
        return;
    }
    m_process->write(QByteArray(buf,len));
}
