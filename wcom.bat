REM Directory of the root of the NARS2000 project
Set NARSROOT=R:\NARS2000^\

REM Path and name of zip program (7-zip)
if "%ProgramFiles(x86)%" == "" (
        Set ZIPPRG="%ProgramFiles%\7-Zip\7z"
    ) else (
        Set ZIPPRG="%ProgramFiles(x86)%\7-Zip\7z"
    )

REM CygWin flag to suppress DOS file warning message
Set CYGWIN=nodosfilewarning
Set CYGWIN_DIR=C:\Cygwin^\

REM Path and name of parser generator (bison)
Set PARGEN=%CYGWIN_DIR%bin\bison

REM Path and name of makepro program
Set MAKEPRO=%NARSROOT%makepro.php

REM Directory of SDK ("%ProgramFiles%\Microsoft SDKs\Windows\v7.1")
Set SDK=U:^\

REM MSVC compiler ("%ProgramFiles%\Microsoft Visual Studio 10.0")
Set MSVC=V:\VC^\

Set VSINSTALLDIR=V:^\
Set VCINSTALLDIR=%MSVC%

if "%ORIGPATH%" == "" Set ORIGPATH=%PATH%

Set MS_DIR=%NARSROOT%msieve^\
Set MPIR_DIR=%NARSROOT%mpir^\
Set MPFR_DIR=%NARSROOT%mpfr^\
Set GSL_DIR=%NARSROOT%gsl^\
Set ECM_DIR=%NARSROOT%ecm^\

if "%1" == "32" goto W32
:W64
if exist M:\*.* subst M: /d
subst M: %MSVC%bin\amd64
if exist N:\*.* subst N: /d
subst N: %SDK%Bin\x64

REM Path to mspdbNNN.dll
Set MSPDB=%MSVC%bin\amd64\

goto START

:W32
if exist M:\*.* subst M: /d
subst M: %MSVC%bin
if exist N:\*.* subst N: /d
subst N: %SDK%Bin

REM Path to mspdbNNN.dll
Set MSPDB=%MSVC%bin^\

:START
Set INCLUDE=%MSVC%include;%SDK%include;%NARSROOT%gsl\include
Set PATH=M:\;N:\;%MSPDB%;%ORIGPATH%

if /i "%2" == "start" start V:\Common7\IDE\VCExpress.exe NARS2000.%1.sln /useenv
