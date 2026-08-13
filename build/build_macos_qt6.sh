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

qmake "$LITEIDE_ROOT" -spec macx-clang "CONFIG+=release" \
	'QMAKE_LIBS_OPENGL=-framework OpenGL' \
	'QMAKE_CXXFLAGS+=-include arm_acle.h'
make

cd "$LITEIDE_ROOT"
./make_tools.sh
go run src/tools/exportqrc/main.go -root .

cd "$BUILD_ROOT"
rm -rf liteide
mkdir -p liteide
cp -R "$LITEIDE_ROOT/liteide/bin/LiteIDE.app" liteide/
rm -f liteide/LiteIDE.app/Contents/PlugIns/*.a
cp -R "$LITEIDE_ROOT/src/liteide/Info.plist" liteide/LiteIDE.app/Contents/
cp "$LITEIDE_ROOT/LICENSE.LGPL" "$LITEIDE_ROOT/LGPL_EXCEPTION.TXT" liteide/
cp "$LITEIDE_ROOT/../README.md" "$LITEIDE_ROOT/../CONTRIBUTORS" liteide/
cp "$LITEIDE_ROOT/bin/gocode" "$LITEIDE_ROOT/bin/gotools" \
	"$LITEIDE_ROOT/bin/gomodifytags" "$LITEIDE_ROOT/bin/gopls" \
	liteide/LiteIDE.app/Contents/MacOS/
cp -R "$LITEIDE_ROOT/deploy/." liteide/LiteIDE.app/Contents/Resources/
cp -R "$LITEIDE_ROOT/os_deploy/macosx/." liteide/LiteIDE.app/Contents/Resources/
