#!/bin/sh

cp -v LICENSE.LGPL liteide
cp -v LGPL_EXCEPTION.TXT liteide
cp -v ../README.md liteide
mkdir -p liteide/share/liteide/
cp bin/liteide_stub liteide/bin
cp bin/gocode liteide/bin
cp -r -v deploy/* liteide/share/liteide/
cp -r -v os_deploy/linux/* liteide/share/liteide/

