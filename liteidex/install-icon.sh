#!/bin/bash

SCRIPT=$(readlink -f $0)
SCRIPTPATH=`dirname $SCRIPT`


echo "
[Desktop Entry]
Type=Application
Name=LiteIDE
Comment=IDE for editing and building projects written in the Go programming language
Exec=${SCRIPTPATH}/bin/liteide
Icon=${SCRIPTPATH}/share/liteide/welcome/images/liteide128.xpm
Terminal=false
StartupNotify=false
Categories=Development;
" > ./liteide.desktop

# must run with current user instead of root
cp ./liteide.desktop ~/Desktop/
chmod u+x ~/Desktop/liteide.desktop

#desktop-file-validate ./liteide.desktop
#desktop-file-install ./liteide.desktop

