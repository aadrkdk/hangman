@echo off

where gcc >nul 2>nul
if %ERRORLEVEL% neq 0 goto SkipBuild

if not exist "build" mkdir build
gcc -o build/hangman.exe hangman.c
goto Finished

:SkipBuild
echo "gcc not found"

:Finished
