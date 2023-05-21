@REM Not checked

@echo off
setlocal enabledelayedexpansion

pushd %~dp0
mkdir build >nul 2>&1
set src=src\parseropt.c
set obj=

for %%a in (%src%) do (
    echo gcc -c %%a -o build\%%a.o
    set obj=!obj! build\%%a.o
)

echo ar rs libparseropt.a %obj%
