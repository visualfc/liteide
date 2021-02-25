/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 visualfc. All rights reserved.
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
// Module: fileutil_mac.mm
// Creator: visualfc <visualfc@gmail.com>

#include "fileutil.h"
#include <QUrl>
#include <QFileInfo>
#include <CoreFoundation/CoreFoundation.h>
#include <Foundation/Foundation.h>


bool FileUtil::hasTrash()
{
#ifdef Q_OS_MACOS
    return true;
#else
    return false;
#endif
}


bool FileUtil::moveToTrash(const QString &fileName)
{
#ifdef Q_OS_MACOS // desktop macOS has a trash can
    QMacAutoReleasePool pool;

    QFileInfo info(fileName);
    NSString *filepath = info.filePath().toNSString();
    NSURL *fileurl = [NSURL fileURLWithPath:filepath isDirectory:info.isDir()];
    NSURL *resultingUrl = nil;
    NSError *nserror = nil;
    NSFileManager *fm = [NSFileManager defaultManager];
    if ([fm trashItemAtURL:fileurl resultingItemURL:&resultingUrl error:&nserror] != YES) {
        return false;
    }
    return true;
#else
    return false;
#endif
}
