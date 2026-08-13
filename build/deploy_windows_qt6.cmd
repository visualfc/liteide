@echo off

setlocal

if defined QTDIR set "PATH=%QTDIR%\bin;%PATH%"
where windeployqt >nul 2>&1 || goto deploy_missing

for %%F in (
  liteide\lib\liteide\plugins\liteeditor.dll
  liteide\lib\liteide\plugins\welcome.dll
  liteide\lib\liteide\plugins\litebuild.dll
  liteide\lib\liteide\plugins\dlvdebugger.dll
  liteide\bin\liteapp.dll
) do (
  windeployqt --dir liteide\bin %%F
  if ERRORLEVEL 1 goto deploy_fail
)
exit /b 0

:deploy_missing
echo error, windeployqt not found in PATH
exit /b 1

:deploy_fail
echo error, windeployqt failed
exit /b 1
