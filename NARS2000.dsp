# Microsoft Developer Studio Project File - Name="NARS2000" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=NARS2000 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NARS2000.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NARS2000.mak" CFG="NARS2000 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NARS2000 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "NARS2000 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NARS2000 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "NARS2000 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /Zp1 /W3 /WX /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D RESDEBUG=1 /D "DEBUG" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib version.lib ret\nd\DbgBrk.obj /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "NARS2000 - Win32 Release"
# Name "NARS2000 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\assign.c
# End Source File
# Begin Source File

SOURCE=.\bjhash.c
# End Source File
# Begin Source File

SOURCE=.\chkesp.c
# End Source File
# Begin Source File

SOURCE=.\common.c
# End Source File
# Begin Source File

SOURCE=.\convert.c
# End Source File
# Begin Source File

SOURCE=.\debug.c
# End Source File
# Begin Source File

SOURCE=.\display.c
# End Source File
# Begin Source File

SOURCE=.\exec.c
# End Source File
# Begin Source File

SOURCE=.\free.c
# End Source File
# Begin Source File

SOURCE=.\main.c
# End Source File
# Begin Source File

SOURCE=.\main.rc

!IF  "$(CFG)" == "NARS2000 - Win32 Release"

!ELSEIF  "$(CFG)" == "NARS2000 - Win32 Debug"

# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /d "BUILD_BETA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Debug\parse.c
# End Source File
# Begin Source File

SOURCE=.\parse.y

!IF  "$(CFG)" == "NARS2000 - Win32 Release"

!ELSEIF  "$(CFG)" == "NARS2000 - Win32 Debug"

# Begin Custom Build
OutDir=.\Debug
InputPath=.\parse.y

"$(OutDir)\parse.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	C:\Cygwin\bin\bison -o$(OutDir)\parse.c -v -r all -l $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pf_circle.c
# End Source File
# Begin Source File

SOURCE=.\pf_cstar.c
# End Source File
# Begin Source File

SOURCE=.\pf_dcaret.c
# End Source File
# Begin Source File

SOURCE=.\pf_divide.c
# End Source File
# Begin Source File

SOURCE=.\pf_dstile.c
# End Source File
# Begin Source File

SOURCE=.\pf_equal.c
# End Source File
# Begin Source File

SOURCE=.\pf_greateq.c
# End Source File
# Begin Source File

SOURCE=.\pf_iota.c
# End Source File
# Begin Source File

SOURCE=.\pf_lcaret.c
# End Source File
# Begin Source File

SOURCE=.\pf_lesseq.c
# End Source File
# Begin Source File

SOURCE=.\pf_lshoe.c
# End Source File
# Begin Source File

SOURCE=.\pf_minus.c
# End Source File
# Begin Source File

SOURCE=.\pf_ndcaret.c
# End Source File
# Begin Source File

SOURCE=.\pf_nequal.c
# End Source File
# Begin Source File

SOURCE=.\pf_nucaret.c
# End Source File
# Begin Source File

SOURCE=.\pf_plus.c
# End Source File
# Begin Source File

SOURCE=.\pf_rcaret.c
# End Source File
# Begin Source File

SOURCE=.\pf_rho.c
# End Source File
# Begin Source File

SOURCE=.\pf_star.c
# End Source File
# Begin Source File

SOURCE=.\pf_stile.c
# End Source File
# Begin Source File

SOURCE=.\pf_times.c
# End Source File
# Begin Source File

SOURCE=.\pf_ucaret.c
# End Source File
# Begin Source File

SOURCE=.\pf_ustile.c
# End Source File
# Begin Source File

SOURCE=.\primfns.c
# End Source File
# Begin Source File

SOURCE=.\primspec.c
# End Source File
# Begin Source File

SOURCE=.\refcnt.c
# End Source File
# Begin Source File

SOURCE=.\reg.c
# End Source File
# Begin Source File

SOURCE=.\resdebug.c
# End Source File
# Begin Source File

SOURCE=.\sessman.c
# End Source File
# Begin Source File

SOURCE=.\strand.c
# End Source File
# Begin Source File

SOURCE=.\symtab.c
# End Source File
# Begin Source File

SOURCE=.\sysfns.c
# End Source File
# Begin Source File

SOURCE=.\sysvars.c
# End Source File
# Begin Source File

SOURCE=.\ver.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\aplerrors.h
# End Source File
# Begin Source File

SOURCE=..\Include\COLORS.H
# End Source File
# Begin Source File

SOURCE=.\compro.h
# End Source File
# Begin Source File

SOURCE=.\datatype.h
# End Source File
# Begin Source File

SOURCE=.\errors.h
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# Begin Source File

SOURCE=.\parse.h
# End Source File
# Begin Source File

SOURCE=.\primspec.h
# End Source File
# Begin Source File

SOURCE=.\resdebug.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\symtab.h
# End Source File
# Begin Source File

SOURCE=.\tokens.h
# End Source File
# Begin Source File

SOURCE=.\types.h
# End Source File
# Begin Source File

SOURCE=.\Unicode.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Images\About.ico
# End Source File
# Begin Source File

SOURCE=".\Images\DB-L.ico"
# End Source File
# Begin Source File

SOURCE=".\Images\DB-S.ico"
# End Source File
# Begin Source File

SOURCE=.\Images\LineCont.bmp
# End Source File
# Begin Source File

SOURCE=".\Images\MF-L.ico"
# End Source File
# Begin Source File

SOURCE=".\MF-L.ico"
# End Source File
# Begin Source File

SOURCE=".\Images\MF-S.ico"
# End Source File
# Begin Source File

SOURCE=".\MF-S.ico"
# End Source File
# Begin Source File

SOURCE=".\Images\SM-L.ico"
# End Source File
# Begin Source File

SOURCE=".\SM-L.ico"
# End Source File
# Begin Source File

SOURCE=".\Images\SM-S.ico"
# End Source File
# Begin Source File

SOURCE=".\SM-S.ico"
# End Source File
# End Group
# End Target
# End Project
