@echo on
copy LICENSE.LGPL liteide
copy LGPL_EXCEPTION.TXT liteide
copy README.TXT liteide
copy bash.bast liteide\bin
copy bin\*.exe liteide\bin
xcopy deploy liteide\share\liteide  /e /y /i
xcopy os_deploy\windows liteide\share\liteide /e /y  /i