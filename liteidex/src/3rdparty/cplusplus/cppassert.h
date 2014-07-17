/****************************************************************************
 * **
 * ** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
 * ** Contact: http://www.qt-project.org/legal
 * **
 * ** This file is part of Qt Creator.
 * **
 * ** Commercial License Usage
 * ** Licensees holding valid commercial Qt licenses may use this file in
 * ** accordance with the commercial license agreement provided with the
 * ** Software or, alternatively, in accordance with the terms contained in
 * ** a written agreement between you and Digia.  For licensing terms and
 * ** conditions see http://qt.digia.com/licensing.  For further information
 * ** use the contact form at http://qt.digia.com/contact-us.
 * **
 * ** GNU Lesser General Public License Usage
 * ** Alternatively, this file may be used under the terms of the GNU Lesser
 * ** General Public License version 2.1 as published by the Free Software
 * ** Foundation and appearing in the file LICENSE.LGPL included in the
 * ** packaging of this file.  Please review the following information to
 * ** ensure the GNU Lesser General Public License version 2.1 requirements
 * ** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 * **
 * ** In addition, as a special exception, Digia gives you certain additional
 * ** rights.  These rights are described in the Digia Qt LGPL Exception
 * ** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
 * **
 * ****************************************************************************/

#ifndef SOFT_ASSERT_H
#define SOFT_ASSERT_H

#include <iostream>

#define CPP_ASSERT_STRINGIFY_HELPER(x) #x
#define CPP_ASSERT_STRINGIFY(x) CPP_ASSERT_STRINGIFY_HELPER(x)
#define CPP_ASSERT_STRING(cond) std::cerr \
    << "SOFT ASSERT: \"" cond"\" in file " __FILE__ ", line " CPP_ASSERT_STRINGIFY(__LINE__) \
    << std::endl;

#define CPP_ASSERT(cond, action) if (cond) {} else { CPP_ASSERT_STRING(#cond); action; } do {} while (0)
#define CPP_CHECK(cond) if (cond) {} else { CPP_ASSERT_STRING(#cond); } do {} while (0)

#endif // SOFT_ASSERT_H


