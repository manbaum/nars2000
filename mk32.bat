@echo off
@php wincver.php version.rc

Set NMAKE=NMAKE
if "%windir%" == "" Set NMAKE=NMAKER
call w32

Set NMAKE_SWS=
if /i "%1"=="/n" Set NMAKE_SWS=/n %NMAKE_SWS%
if /i "%2"=="/n" Set NMAKE_SWS=/n %NMAKE_SWS%
if /i "%3"=="/n" Set NMAKE_SWS=/n %NMAKE_SWS%
if /i "%4"=="/n" Set NMAKE_SWS=/n %NMAKE_SWS%
if /i "%5"=="/n" Set NMAKE_SWS=/n %NMAKE_SWS%
if /i "%6"=="/n" Set NMAKE_SWS=/n %NMAKE_SWS%
if /i "%7"=="/n" Set NMAKE_SWS=/n %NMAKE_SWS%
if /i "%8"=="/n" Set NMAKE_SWS=/n %NMAKE_SWS%
if /i "%9"=="/n" Set NMAKE_SWS=/n %NMAKE_SWS%

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

%NMAKE% /nologo /f makefile.ind "WIN=32" "DEBUG=0" "NMAKE_SWS=%NMAKE_SWS%" "DEL_SW=%DEL_SW%" %*
Set DEL_SW=
Set NMAKE_SWS=
Set NMAKE=
