@echo on

copy LICENSE.LGPL liteide
copy LGPL_EXCEPTION.TXT liteide
copy ..\README.md liteide
copy bin\*.exe liteide\bin
xcopy deploy liteide\share\liteide  /e /y /i
xcopy os_deploy\windows liteide\share\liteide /e /y  /i