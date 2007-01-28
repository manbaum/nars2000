@echo off
Set NMAKE=NMAKE
if "%windir%" == "" Set NMAKE=NMAKER
%NMAKE% /nologo DEBUG=0 /f tools.mak %1 %2 %3 %4 %5 %6 %7 %8 %9
Set NMAKE=
