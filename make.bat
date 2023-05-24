@REM Not checked

@echo off
setlocal enabledelayedexpansion

pushd %~dp0
mkdir build >nul 2>&1
set src=src\parseropt.c
set obj=
set opt=-Iinclude

for %%a in (%src%) do (
    gcc -c %%a -o build\%%a.o %opt%
    set obj=!obj! build\%%a.o
)

ar rs libparseropt.a %obj%
