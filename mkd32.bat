@echo off
Set NMAKE=NMAKE
if "%windir%" == "" Set NMAKE=NMAKER
call w32

Set DEL_SW=
if /i "%1"=="del" Set DEL_SW=1
if /i "%2"=="del" Set DEL_SW=1
if /i "%3"=="del" Set DEL_SW=1
if /i "%4"=="del" Set DEL_SW=1
if /i "%5"=="del" Set DEL_SW=1
if /i "%6"=="del" Set DEL_SW=1
if /i "%7"=="del" Set DEL_SW=1
if /i "%8"=="del" Set DEL_SW=1
if /i "%9"=="del" Set DEL_SW=1

%NMAKE% /nologo /f makefile.ind "WIN=32" "DEBUG=1" "NMAKE_SWS=%*" "DEL_SW=%DEL_SW%"
Set DEL_SW=
Set NMAKE=
