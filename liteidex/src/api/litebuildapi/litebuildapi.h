/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2012 LiteIDE Team. All rights reserved.
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
// Module: liteapi.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: liteapi.h,v 1.0 2011-4-22 visualfc Exp $

#ifndef __LITEBUILDAPI_H__
#define __LITEBUILDAPI_H__

#include "liteapi/liteapi.h"
#include <QProcessEnvironment>

namespace LiteApi {

class BuildAction
{  
public:
    BuildAction(): m_output(false),m_readline(false),m_separator(false) {}
    void setId(const QString &id) { m_id = id; }
    void setMenu(const QString &menu) { m_menu = menu; }
    void setKey(const QString &key) { m_key = key; }
    void setCmd(const QString &bin) { m_cmd = bin; }
    void setArgs(const QString &args) { m_args = args; }
    void setSave(const QString &save) { m_save = save; }
    void setOutput(const QString &text) {
        m_output = QVariant(text).toBool();
    }
    void setReadline(const QString &text) {
        m_readline = QVariant(text).toBool();
    }
    void setSeparator(bool b) {
        m_separator = b;
    }
    void setWork(const QString &work) { m_work = work; }
    void setCodec(const QString &codec) { m_codec = codec; }
    void setRegex(const QString &regex) { m_regex = regex; }
    void setImg(const QString &img) {m_img = img; }
    void setTask(const QStringList &task) { m_task = task; }
    QString work() const { return m_work; }
    QString id() const { return m_id; }
    QString menu() const { return m_menu; }
    QString key() const { return m_key; }
    QString cmd() const { return m_cmd; }
    QString args() const { return m_args; }
    QString save() const { return m_save; }
    bool output() const { return m_output; }
    bool readline() const {return m_readline; }
    bool isSeparator() const { return m_separator; }
    QString codec() const { return m_codec; }
    QString regex() const { return m_regex; }
    QString img() const { return m_img; }
    QStringList task() const { return m_task; }
    void clear() {
        m_id.clear();
        m_cmd.clear();
        m_key.clear();
        m_args.clear();
        m_codec.clear();
        m_regex.clear();
        m_img.clear();
        m_save.clear();
        m_task.clear();
        m_output = false;
        m_readline = false;
    }
    bool isEmpty() {
        return m_id.isEmpty();
    }

protected:
    QString m_id;
    QString m_key;
    QString m_cmd;
    QString m_args;
    QString m_codec;
    QString m_regex;
    QString m_save;
    QString m_img;
    QString m_work;
    QString m_menu;
    QStringList m_task;
    bool    m_output;
    bool    m_readline;
    bool    m_separator;
};

class BuildLookup
{
public:
    BuildLookup() : m_top(1)
    {
    }
    void setMimeType(const QString &type) {m_type=type;}
    void setFile(const QString &file) {m_file=file;}
    void setTop(const QString &top) {
        if (top.isEmpty()) {
            return;
        }
        bool ok = false;
        int value = top.toInt(&ok);
        if (ok) {
            m_top=value;
        }
    }
    QString mimeType() const {return m_type;}
    QString file() const {return m_file;}
    int top() const {return m_top;}
protected:
    QString m_type;
    QString m_file;
    int     m_top;
};

class BuildConfig
{
public:
    BuildConfig()
    {
    }
    void setId(const QString &id) { m_id = id; }
    void setName(const QString &name) { m_name = name; }
    void setValue(const QString &value) { m_value = value; }
    QString id() const { return m_id; }
    QString name() const { return m_name; }
    QString value() const { return m_value; }
protected:
    QString m_id;
    QString m_name;
    QString m_value;
};

class BuildCustom
{
public:
    BuildCustom()
    {
    }
    void setId(const QString &id) { m_id = id; }
    void setName(const QString &name) { m_name = name; }
    void setValue(const QString &value) { m_value = value; }
    QString id() const { return m_id; }
    QString name() const { return m_name; }
    QString value() const { return m_value; }
protected:
    QString m_id;
    QString m_name;
    QString m_value;
};

class BuildDebug
{
public:
    BuildDebug()
    {
    }
    void setId(const QString &id) { m_id = id; }
    void setCmd(const QString &cmd) { m_cmd = cmd; }
    void setArgs(const QString &args) { m_args = args; }
    void setWork(const QString &work) { m_work = work; }
    QString id() const { return m_id; }
    QString cmd() const { return m_cmd; }
    QString args() const { return m_args; }
    QString work() const { return m_work; }
    bool isEmpty() {
        return m_id.isEmpty();
    }
protected:
    QString m_id;
    QString m_cmd;
    QString m_args;
    QString m_work;
};

class IBuild : public QObject
{
    Q_OBJECT
public:
    IBuild(QObject *parent = 0): QObject(parent) {}
    virtual ~IBuild() {}
    virtual QString mimeType() const = 0;
    virtual QString id() const = 0;
    virtual QString work() const = 0;
    virtual QList<BuildAction*> actionList() const = 0;
    virtual QList<BuildLookup*> lookupList() const = 0;
    virtual QList<BuildConfig*> configList() const = 0;
    virtual QList<BuildCustom*> customList() const = 0;
    virtual QList<BuildDebug*>  debugList() const = 0;
    virtual BuildAction *findAction(const QString &name) = 0;
    virtual QList<QAction*> actions() = 0;
signals:
    void buildAction(LiteApi::IBuild *build, LiteApi::BuildAction *act);
};

class IBuildManager : public IManager
{
    Q_OBJECT
public:
    IBuildManager(QObject *parent = 0) : IManager(parent) {}
    virtual void addBuild(IBuild *build) = 0;
    virtual void removeBuild(IBuild *build) = 0;
    virtual IBuild *findBuild(const QString &mimeType) = 0;
    virtual QList<IBuild*> buildList() const = 0;
    virtual void setCurrentBuild(IBuild *build) = 0;
    virtual IBuild *currentBuild() const = 0;
signals:
    void buildChanged(LiteApi::IBuild*);
};

class ILiteBuild : public IObject
{
    Q_OBJECT
public:
    ILiteBuild(QObject *parent) : IObject(parent)
    {
    }
public:
    virtual QString buildFilePath() const = 0;
    virtual QString targetFilePath() const = 0;
    virtual QMap<QString,QString> buildEnvMap() const = 0;
    virtual IBuildManager *buildManager() const = 0;   
    virtual QString envValue(LiteApi::IBuild *build, const QString &value) = 0;
    virtual void appendOutput(const QString &str, const QBrush &brush, bool active, bool updateExistsTextColor = true) = 0;
    virtual void execAction(const QString &mime,const QString &id) = 0;
    virtual void executeCommand(const QString &cmd, const QString &args, const QString &workDir, bool updateExistsTextColor = true) = 0;
signals:
    void currentBuildFileChanged(const QString &filePath);
};

inline ILiteBuild *getLiteBuild(LiteApi::IApplication* app)
{
    return LiteApi::findExtensionObject<ILiteBuild*>(app,"LiteApi.ILiteBuild");
}

} //namespace LiteApi


#endif //__LITEBUILDAPI_H__

