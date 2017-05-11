@echo off

SetLocal

Set NMAKE=NMAKE
call w64

Set DEL_SW=
FOR %%G IN (%*) DO (IF /I "%1" == "del" Set DEL_SW=1)

%NMAKE% /nologo /f makefile.ind "WIN=64" "DEBUG=0" "NMAKE_SWS=%*" "DEL_SW=%DEL_SW%"

EndLocal
