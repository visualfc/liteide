#!/bin/sh

cp -v LICENSE.LGPL liteide
cp -v LGPL_EXCEPTION.TXT liteide
cp -v ../README.md liteide
mkdir -p liteide/share/liteide/
cp bin/gotools liteide/bin
cp bin/gocode liteide/bin
cp bin/gomodifytags liteide/bin
cp bin/libgopher.dylib liteide/lib/liteide/libgopher.so
cp -r -v deploy/* liteide/share/liteide/
cp -r -v os_deploy/linux/* liteide/share/liteide/

