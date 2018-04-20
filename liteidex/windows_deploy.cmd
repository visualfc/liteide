@echo on

copy LICENSE.LGPL liteide
copy LGPL_EXCEPTION.TXT liteide
copy ..\README.md liteide
copy bin\gocode.exe liteide\bin
copy bin\gotools.exe liteide\bin
copy bin\gomodifytags.exe liteide\bin
xcopy deploy liteide\share\liteide  /e /y /i
xcopy os_deploy\windows liteide\share\liteide /e /y  /i
