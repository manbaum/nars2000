@echo off
REM echo Args = "%1" "%2" "%3"
Set INCLUDE=%1.;%1Include;%1Include\Wine;%1Include\msvcrt;.;K:\include
REM echo INCLUDE=%INCLUDE%
cl /Od /D "_MBCS" /GF /FD /EHsc /MTd /GS- /GR- /Fo""%3"\\" /Fd"Debug\vc90.pdb" /W3 /nologo /c /Wp64 /ZI /TC /errorReport:prompt  /D DEBUG /D RESDEBUG=1 /D _CRT_SECURE_NO_WARNINGS /I "%1"Include\Wine /I "%1"Include\msvcrt "%2"
