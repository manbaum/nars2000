REM Directory of the root of the NARS2000 project
Set NARSROOT=R:\NARS2000^\

REM Path to Doctor Dump SDK root
Set CS_DIR=R:\DoctorDumpSDK^\

REM Path and name of zip program (7-zip)
Set ZIPPRG="%ProgramFiles%\7-Zip\7z"

REM CygWin flag to suppress DOS file warning message
Set CYGWIN=nodosfilewarning
Set CYGWIN_DIR=C:\Cygwin64^\

REM Path and name of parser generator (bison)
Set PARGEN=%CYGWIN_DIR%bin\bison

REM Path and name of makedep program
Set MAKEDEP=%NARSROOT%makedep.php

REM Path and name of makepro program
Set MAKEPRO=%NARSROOT%makepro.php

REM Directory of SDK ("%ProgramFiles%\Microsoft SDKs\Windows\v7.1")
Set SDK=U:^\

REM MSVC compiler ("%ProgramFiles%\Microsoft Visual Studio 9.0")
Set MSVC=V:\VC^\

Set VSINSTALLDIR=V:^\
Set VCINSTALLDIR=%MSVC%

if not defined ORIGPATH Set ORIGPATH=%PATH%

Set MS_DIR=%NARSROOT%msieve^\
Set MPIR_DIR=%NARSROOT%mpir^\
Set MPFR_DIR=%NARSROOT%mpfr^\
Set GSL_DIR=%NARSROOT%gsl^\
Set ECM_DIR=%NARSROOT%ecm^\

if not exist .\SImPL-Medium.ttf copy /Y ..\SImPL-Medium.ttf . >NUL

if "%1" == "32" goto W32
:W64
subst M: /d >NUL
subst M: %MSVC%bin\amd64
subst N: /d >NUL
subst N: %SDK%Bin\x64

REM Path to mspdbNNN.dll
Set MSPDB=%MSVC%bin\amd64\

REM Path to library files (x64 uses the 64-bit libraries)
Set LIB=%SDK%Lib\x64;%MSVC%lib\amd64

goto START

:W32
subst M: /d >NUL
subst M: %MSVC%bin
subst N: /d >NUL
subst N: %SDK%Bin

REM Path to mspdbNNN.dll
Set MSPDB=%MSVC%bin^\

REM Path to library files (x86 uses the 32-bit libraries)
Set LIB=%SDK%Lib;%MSVC%lib

:START
Set _NT_SYMBOL_PATH=srv*C:\Symbols*http://msdl.microsoft.com/download/symbols
Set INCLUDE=%MSVC%include;%SDK%include
Set PATH=M:\;N:\;%MSPDB%;%NARSROOT%qdebug;%ORIGPATH%

if /i "%2" == "start" start V:\Common7\IDE\VCExpress.exe NARS2000.%1.sln /useenv
