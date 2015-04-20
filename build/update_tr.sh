#!/bin/sh

export BUILD_ROOT=$PWD

if [ -z $LITEIDE_ROOT ]; then
	export LITEIDE_ROOT=$PWD/../liteidex
fi

echo GOROOT=$GOROOT
echo QTDIR=$QTDIR
echo BUILD_ROOT=$BUILD_ROOT
echo LITEIDE_ROOT=$LITEIDE_ROOT
echo .

if [ -z $QTDIR ]; then
	echo 'error, QTDIR is null'
	exit 1
fi

echo compress translator files

lrelease -compress %LITEIDE_ROOT%/liteide_zh.ts -qm %LITEIDE_ROOT%/deploy/translations/liteide_zh.qm
lrelease -compress %LITEIDE_ROOT%/liteide_zh_TW.ts -qm %LITEIDE_ROOT%/deploy/translations/liteide_zh_TW.qm
lrelease -compress %LITEIDE_ROOT%/liteide_de.ts -qm %LITEIDE_ROOT%/deploy/translations/liteide_de.qm
lrelease -compress %LITEIDE_ROOT%/liteide_fr.ts -qm %LITEIDE_ROOT%/deploy/translations/liteide_fr.qm
lrelease -compress %LITEIDE_ROOT%/liteide_ja.ts -qm %LITEIDE_ROOT%/deploy/translations/liteide_ja.qm
lrelease -compress %LITEIDE_ROOT%/liteide_ru.ts -qm %LITEIDE_ROOT%/deploy/translations/liteide_ru.qm
lrelease -compress %LITEIDE_ROOT%/liteide_uk.ts -qm %LITEIDE_ROOT%/deploy/translations/liteide_uk.qm

