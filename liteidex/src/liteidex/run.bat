@echo off
windres -o liteide-res.syso liteide.rc
go build
del liteide-res.syso
copy liteidex.exe ..\..\liteide\bin
cd ..\..\liteide\bin
liteidex