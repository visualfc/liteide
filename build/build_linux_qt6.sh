#!/bin/sh

set -e

BUILD_ROOT=$PWD
LITEIDE_ROOT=${LITEIDE_ROOT:-$PWD/../liteidex}

if [ -n "$QTDIR" ]; then
	PATH="$QTDIR/bin:$PATH"
	export PATH
fi

command -v qmake >/dev/null 2>&1 || { echo 'error, qmake not found in PATH'; exit 1; }
qmake -query QT_VERSION | grep -q '^6\.' || { echo 'error, qmake is not Qt 6'; exit 1; }

qmake "$LITEIDE_ROOT" -spec linux-g++ "CONFIG+=release"
make

cd "$LITEIDE_ROOT"
./make_tools.sh
go run src/tools/exportqrc/main.go -root .

cd "$BUILD_ROOT"
rm -rf liteide
mkdir -p liteide/bin liteide/share/liteide liteide/lib/liteide/plugins

cp -a "$LITEIDE_ROOT/LICENSE.LGPL" "$LITEIDE_ROOT/LGPL_EXCEPTION.TXT" liteide/
cp -a "$LITEIDE_ROOT/../README.md" "$LITEIDE_ROOT/../CONTRIBUTORS" liteide/
cp -a "$LITEIDE_ROOT/liteide/bin/liteide" liteide/bin/
cp -a "$LITEIDE_ROOT/bin/gocode" "$LITEIDE_ROOT/bin/gotools" \
	"$LITEIDE_ROOT/bin/gomodifytags" "$LITEIDE_ROOT/bin/gopls" liteide/bin/
cp -a "$LITEIDE_ROOT/liteide/lib/liteide/libliteapp.so" liteide/lib/liteide/
cp -a "$LITEIDE_ROOT/liteide/lib/liteide/plugins/"*.so liteide/lib/liteide/plugins/
cp -R "$LITEIDE_ROOT/deploy/." liteide/share/liteide/
cp -R "$LITEIDE_ROOT/os_deploy/linux/." liteide/share/liteide/
