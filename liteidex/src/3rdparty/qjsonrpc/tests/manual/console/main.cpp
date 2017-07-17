/*
 * Copyright (C) 2012-2013 Matt Broadstone
 * Contact: http://bitbucket.org/devonit/qjsonrpc
 *
 * This file is part of the QJsonRpc Library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 */
#include <qscriptengine.h>

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QStringList>
#include <QCoreApplication>

#include <stdlib.h>

#include "interface.h"

static bool wantsToQuit;

static QScriptValue qtscript_quit(QScriptContext *ctx, QScriptEngine *eng)
{
    Q_UNUSED(ctx);
    wantsToQuit = true;
    return eng->undefinedValue();
}

static void interactive(QScriptEngine *eng)
{
    QScriptValue global = eng->globalObject();
    QScriptValue quitFunction = eng->newFunction(qtscript_quit);
    if (!global.property(QLatin1String("exit")).isValid())
        global.setProperty(QLatin1String("exit"), quitFunction);
    if (!global.property(QLatin1String("quit")).isValid())
        global.setProperty(QLatin1String("quit"), quitFunction);
    wantsToQuit = false;

    QTextStream qin(stdin, QFile::ReadOnly);

    const char *qscript_prompt = "console> ";
    const char *dot_prompt = ".... ";
    const char *prompt = qscript_prompt;

    QString code;

    forever {
        QString line;

        printf("%s", prompt);
        fflush(stdout);

        line = qin.readLine();
        if (line.isNull())
            break;

        code += line;
        code += QLatin1Char('\n');

        if (line.trimmed().isEmpty()) {
            continue;

        } else if (! eng->canEvaluate(code)) {
            prompt = dot_prompt;

        } else {
            QScriptValue result = eng->evaluate(code, QLatin1String("typein"));

            code.clear();
            prompt = qscript_prompt;

            if (! result.isUndefined())
                fprintf(stderr, "%s\n", qPrintable(result.toString()));

            if (wantsToQuit)
                break;
        }
    }
}

static QScriptValue importExtension(QScriptContext *context, QScriptEngine *engine)
{
    return engine->importExtension(context->argument(0).toString());
}

static QScriptValue loadScripts(QScriptContext *context, QScriptEngine *engine)
{
    for (int i = 0; i < context->argumentCount(); ++i) {
        QString fileName = context->argument(0).toString();
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly))
            return context->throwError(QString::fromLatin1("could not open %0 for reading").arg(fileName));
        QTextStream ts(&file);
        QString contents = ts.readAll();
        file.close();
        QScriptContext *pc = context->parentContext();
        context->setActivationObject(pc->activationObject());
        context->setThisObject(pc->thisObject());
        QScriptValue ret = engine->evaluate(contents);
        if (engine->hasUncaughtException())
            return ret;
    }
    return engine->undefinedValue();
}

int main(int argc, char *argv[])
{
    QCoreApplication *app;
    if (argc >= 2 && !qstrcmp(argv[1], "-tty")) {
        ++argv;
       --argc;
        app = new QCoreApplication(argc, argv);
    } else {
        app = new QCoreApplication(argc, argv);
    }

    QScriptEngine *eng = new QScriptEngine();

    QScriptValue globalObject = eng->globalObject();

    globalObject.setProperty("load", eng->newFunction(loadScripts, /*length=*/1));

    {
        if (!globalObject.property("qt").isObject())
            globalObject.setProperty("qt", eng->newObject());            
        QScriptValue qscript = eng->newObject();
        qscript.setProperty("importExtension", eng->newFunction(importExtension));
        globalObject.property("qt").setProperty("script", qscript);
    }

    //    ByteArrayClass *byteArrayClass = new ByteArrayClass(eng);
    //    globalObject.setProperty("ByteArray", byteArrayClass->constructor());

    QScriptValue rpcObject = eng->newQObject(new QJsonRpcServiceSocketPrototype);
    globalObject.setProperty("qjsonrpc", rpcObject);

    if (! *++argv) {
        interactive(eng);
        return EXIT_SUCCESS;
    }

    while (const char *arg = *argv++) {
        QString fn = QString::fromLocal8Bit(arg);

        if (fn == QLatin1String("-i")) {
            interactive(eng);
            break;
        }

        QString contents;
        int lineNumber = 1;

        if (fn == QLatin1String("-")) {
            QTextStream stream(stdin, QFile::ReadOnly);
            contents = stream.readAll();
        }

        else {
            QFile file(fn);

            if (file.open(QFile::ReadOnly)) {
                QTextStream stream(&file);
                contents = stream.readAll();
                file.close();

                // strip off #!/usr/bin/env qscript line
                if (contents.startsWith("#!")) {
                    contents.remove(0, contents.indexOf("\n"));
                    ++lineNumber;
                }
            }
        }

        if (contents.isEmpty())
            continue;

        QScriptValue r = eng->evaluate(contents, fn, lineNumber);
        if (eng->hasUncaughtException()) {
            QStringList backtrace = eng->uncaughtExceptionBacktrace();
            fprintf (stderr, "    %s\n%s\n\n", qPrintable(r.toString()),
                     qPrintable(backtrace.join("\n")));
            return EXIT_FAILURE;
        }
    }

    delete eng;
    delete app;

    return EXIT_SUCCESS;
}
