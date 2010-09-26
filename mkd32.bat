@echo off
Set NMAKE=NMAKE
if "%windir%" == "" Set NMAKE=NMAKER
call w32
%NMAKE% /nologo /f makefile.w32 DEBUG=1 %1 %2 %3 %4 %5 %6 %7 %8 %9
Set NMAKE=
