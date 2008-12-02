//***************************************************************************
//  NARS2000 -- INI File Routines
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2008 Sudley Place Software

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

#define STRICT
#include <windows.h>
#include <stdio.h>

#include "main.h"
#include "resdebug.h"
#include "resource.h"
#include "sysvars.h"
#include "externs.h"
#include "unitranshdr.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


// Section names
#define SECTNAME_GENERAL                L"General"
#define SECTNAME_SYSVARS                L"SysVars"
#define SECTNAME_FONTS                  L"Fonts"
#define SECTNAME_OPTIONS                L"Options"
#define SECTNAME_RANGELIMITS            L"RangeLimits"
#define SECTNAME_RESETVARS              L"ResetVars"
#define SECTNAME_SAMEFONTAS             L"SameFontAs"
#define SECTNAME_COLORS                 L"Colors"

// Key names
#define KEYNAME_VERSION                 L"Version"
#define KEYNAME_SIZESTATE               L"SizeState"
#define KEYNAME_XPOS                    L"xPos"
#define KEYNAME_XSIZE                   L"xSize"
#define KEYNAME_YPOS                    L"yPos"
#define KEYNAME_YSIZE                   L"ySize"
#define KEYNAME_INITIALCAT              L"InitialCategory"

#define KEYNAME_QUADALX                 L"QuadALX"
#define KEYNAME_QUADCT                  L"QuadCT"
#define KEYNAME_QUADELX                 L"QuadELX"
#define KEYNAME_QUADFC                  L"QuadFC"
#define KEYNAME_QUADIC                  L"QuadIC"
#define KEYNAME_QUADIO                  L"QuadIO"
#define KEYNAME_QUADLX                  L"QuadLX"
#define KEYNAME_QUADMF                  L"QuadMF"
#define KEYNAME_QUADPP                  L"QuadPP"
#define KEYNAME_QUADPR                  L"QuadPR"
#define KEYNAME_QUADPW                  L"QuadPW"
#define KEYNAME_QUADRL                  L"QuadRL"
#define KEYNAME_QUADSA                  L"QuadSA"

#define KEYNAME_LOGFONTFE               L"LogFontFE"
#define KEYNAME_LOGFONTME               L"LogFontME"
#define KEYNAME_LOGFONTPR               L"LogFontPR"
#define KEYNAME_LOGFONTSM               L"LogFontSM"
#define KEYNAME_LOGFONTTC               L"LogFontTC"
#define KEYNAME_LOGFONTVE               L"LogFontVE"

#define KEYNAME_ADJUSTPW                L"AdjustPW"
#define KEYNAME_UNDERBARTOLOWERCASE     L"UnderbarToLowercase"
#define KEYNAME_NEWTABONCLEAR           L"NewTabOnClear"
#define KEYNAME_NEWTABONLOAD            L"NewTabOnload"
#define KEYNAME_USELOCALTIME            L"UseLocalTime"
#define KEYNAME_BACKUPONLOAD            L"BackupOnLoad"
#define KEYNAME_BACKUPONSAVE            L"BackupOnSave"
#define KEYNAME_NOCOPYRIGHTMSG          L"NoCopyrightMsg"
#define KEYNAME_DEFAULTPASTE            L"DefaultPaste"
#define KEYNAME_DEFAULTCOPY             L"DefaultCopy"
#define KEYNAME_SYNTCLRFCNS             L"SyntClrFcns"
#define KEYNAME_SYNTCLRSESS             L"SyntClrSess"

#define KEYNAME_SC_GLBNAME              L"GlbName"
#define KEYNAME_SC_LCLNAME              L"LclName"
#define KEYNAME_SC_LABEL                L"Label"
#define KEYNAME_SC_PRIM                 L"Prim"
#define KEYNAME_SC_SYSFCN               L"SysFcn"
#define KEYNAME_SC_GLBSYSVAR            L"GlbSysVar"
#define KEYNAME_SC_LCLSYSVAR            L"LclSysVar"
#define KEYNAME_SC_CTRLSTRUC            L"CtrlStruc"
#define KEYNAME_SC_NUMCONST             L"NumConst"
#define KEYNAME_SC_CHRCONST             L"ChrConst"
#define KEYNAME_SC_COMMENT              L"Comment"
#define KEYNAME_SC_MATCHGRP1            L"MatchGrp1"
#define KEYNAME_SC_MATCHGRP2            L"MatchGrp2"
#define KEYNAME_SC_MATCHGRP3            L"MatchGrp3"
#define KEYNAME_SC_MATCHGRP4            L"MatchGrp4"
#define KEYNAME_SC_UNMATCHGRP           L"UnmatchGrp"
#define KEYNAME_SC_UNK                  L"Unk"

// Format string for [Fonts] section LOGFONTW
#define FMTSTR_LOGFONT_INP      L"%d %d %d %d %d %d %d %d %d %d %d %d %d '%s'"
#define FMTSTR_LOGFONT_OUT      L"%d %d %d %d %d %d %d %d %d %d %d %d %d '%s'"


//***************************************************************************
//  $CreateAppDataDirs
//
//  Ensure the Application Data and workspaces
//    directories are present
//***************************************************************************

UBOOL CreateAppDataDirs
    (void)

{
    static WCHAR wszAppData[2 * MAX_PATH];
    DWORD        cchSize;
    WCHAR        wszTemp[1024];
    UINT         uNxt;

#define TEMPBUFLEN      itemsizeof (wszTemp)
#define WS_APPDATA          L"APPDATA"

    // If we didn't find it, ...
    if (0 EQ GetEnvironmentVariableW (WS_APPDATA, wszAppData, TEMPBUFLEN))
    {
        MBW (L"Unable to find " WS_APPDATA L" in the environment.");

        return FALSE;
    } // End IF

#undef  WS_APPDATA

    // Append the "\\NARS2000" part
    lstrcatW (wszAppData, L"\\" WS_APPNAME);

    // Ensure the "...\\Application Data\\NARS2000" directory is present
    if (!CreateDirectoryW (wszAppData, NULL)
     && GetLastError () NE ERROR_ALREADY_EXISTS)
    {
        // Start with leading text
        lstrcpyW (wszTemp, L"CreateAppDataDirs (wszAppData):  ");
        uNxt = lstrlenW (wszTemp);

        // Format the error message
        FormatMessageW (FORMAT_MESSAGE_FROM_SYSTEM, // Source and processing options
                        NULL,                       // Pointer to  message source
                        GetLastError (),            // Requested message identifier
                        0,                          // Language identifier for requested message
                       &wszTemp[uNxt],              // Pointer to message buffer
                        TEMPBUFLEN - uNxt,          // Maximum size of message buffer
                        NULL);                      // Address of array of message inserts
#ifdef DEBUG
        DbgBrk ();
#endif
        MBW (wszTemp);

        return FALSE;
    } // End IF

    // Get the size so we can append to it
    cchSize = lstrlenW (wszAppData);

    // Now append the .ini file name to get lpwszIniFile
    lstrcpyW (&wszAppData[cchSize], L"\\" WS_APPNAME L".ini");
    lpwszIniFile = wszAppData;

    // Get ptr to where the workspaces dir will be stored
    // The "+ 1" is to skip over the terminating zero
    lpwszWorkDir = &wszAppData[lstrlenW (lpwszIniFile) + 1];

    // Append the AppData directory part so we can form the workspaces dir
    // The "+ 1" is because lstrcpynW includes a terminating zero in the count
    lstrcpynW (lpwszWorkDir, lpwszIniFile, cchSize + 1);

    // Append the workspaces name
    lstrcatW (lpwszWorkDir, L"\\" WS_WKSNAME);

    // Ensure the workspaces dir is present
    if (!CreateDirectoryW (lpwszWorkDir, NULL)
     && GetLastError () NE ERROR_ALREADY_EXISTS)
    {
        // Start with leading text
        lstrcpyW (wszTemp, L"CreateAppDataDirs (lpwszWorkDir):  ");
        uNxt = lstrlenW (wszTemp);

        // Format the error message
        FormatMessageW (FORMAT_MESSAGE_FROM_SYSTEM, // Source and processing options
                        NULL,                       // Pointer to  message source
                        GetLastError (),            // Requested message identifier
                        0,                          // Language identifier for requested message
                       &wszTemp[uNxt],              // Pointer to message buffer
                        TEMPBUFLEN - uNxt,          // Maximum size of message buffer
                        NULL);                      // Address of array of message inserts
#undef  TEMPBUFLEN
#ifdef DEBUG
        DbgBrk ();
#endif
        MBW (wszTemp);

        return FALSE;
    } // End IF

    // Ensure there's a trailing backslash
    lstrcatW (lpwszWorkDir, L"\\");

    return TRUE;
} // End CreateAppDataDirs


//***************************************************************************
//  $ReadIniFileGlb
//
//  Read in global-specific .ini file settings
//***************************************************************************

void ReadIniFileGlb
    (void)

{
    WCHAR wszTemp[1024],            // Temporary storage for string results
         *lpwszTemp;                // Temporary ptr into wszTemp
    UINT  uCnt;                     // Loop counter

#define TEMPBUFLEN      itemsizeof (wszTemp)

    //***************************************************************
    // Read in the [Fonts] section
    //***************************************************************

    // Read in the LOGFONTW strucs
    GetPrivateProfileLogFontW (SECTNAME_FONTS, KEYNAME_LOGFONTFE, &lfFE);
    GetPrivateProfileLogFontW (SECTNAME_FONTS, KEYNAME_LOGFONTME, &lfME);
    GetPrivateProfileLogFontW (SECTNAME_FONTS, KEYNAME_LOGFONTPR, &lfPR);
    GetPrivateProfileLogFontW (SECTNAME_FONTS, KEYNAME_LOGFONTSM, &lfSM);
    GetPrivateProfileLogFontW (SECTNAME_FONTS, KEYNAME_LOGFONTTC, &lfTC);
    GetPrivateProfileLogFontW (SECTNAME_FONTS, KEYNAME_LOGFONTVE, &lfVE);

    //***************************************************************
    // Read in the [SameFontAs] section
    //***************************************************************

    for (uCnt = 0; uCnt < FONTENUM_LENGTH; uCnt++)
    {
        wsprintfW (wszTemp,
                   L"%u",
                   uCnt);
        glbSameFontAs[uCnt] =
          GetPrivateProfileIntW (SECTNAME_SAMEFONTAS,   // Ptr to the section name
                                 wszTemp,               // Ptr to the key name
                                 uCnt,                  // Default value if not found
                                 lpwszIniFile);         // Ptr to the file name
    } // End FOR

    //***************************************************************
    // Read in the [Options] section
    //***************************************************************

    // Read in bAdjustPW
    OptionFlags.bAdjustPW =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_ADJUSTPW,      // Ptr to the key name
                             DEF_ADJUSTPW,          // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bUnderbarToLowercase
    OptionFlags.bUnderbarToLowercase =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_UNDERBARTOLOWERCASE,// Ptr to the key name
                             DEF_UNDERBARTOLOWERCASE,// Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bNewTabOnClear
    OptionFlags.bNewTabOnClear =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_NEWTABONCLEAR, // Ptr to the key name
                             DEF_NEWTABONCLEAR,     // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bNewTabOnLoad
    OptionFlags.bNewTabOnLoad =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_NEWTABONLOAD,  // Ptr to the key name
                             DEF_NEWTABONLOAD,      // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bUseLocalTime
    OptionFlags.bUseLocalTime =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_USELOCALTIME,  // Ptr to the key name
                             DEF_USELOCALTIME,      // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bBackupOnLoad
    OptionFlags.bBackupOnLoad =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_BACKUPONLOAD,  // Ptr to the key name
                             DEF_BACKUPONLOAD,      // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bBackupOnSave
    OptionFlags.bBackupOnSave =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_BACKUPONSAVE,  // Ptr to the key name
                             DEF_BACKUPONSAVE,      // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bNoCopyrightMsg
    OptionFlags.bNoCopyrightMsg =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_NOCOPYRIGHTMSG,// Ptr to the key name
                             DEF_NOCOPYRIGHTMSG,    // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in uDefaultPaste
    OptionFlags.uDefaultPaste =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_DEFAULTPASTE,  // Ptr to the key name
                             DEF_DEFAULTPASTE,      // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in uDefaultCopy
    OptionFlags.uDefaultCopy =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_DEFAULTCOPY,   // Ptr to the key name
                             DEF_DEFAULTCOPY,       // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bSyntClrFcns
    OptionFlags.bSyntClrFcns =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_SYNTCLRFCNS,   // Ptr to the key name
                             DEF_SYNTCLRFCNS,       // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bSyntClrSess
    OptionFlags.bSyntClrSess =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_SYNTCLRSESS,   // Ptr to the key name
                             DEF_SYNTCLRSESS,       // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    //***************************************************************
    // Read in the [SysVars] section -- default values for system
    //                                  variables in a CLEAR WS
    //***************************************************************

    // Read in []ALX
    hGlbQuadALX_CWS =
      GetPrivateProfileGlbCharW (SECTNAME_SYSVARS,  // Ptr to the section name
                                 KEYNAME_QUADALX,   // Ptr to the key name
                                 DEF_QUADALX_CWS_BR,// Ptr to default value
                                 DEF_QUADALX_GLB,   // HGLOBAL of the result
                                 lpwszIniFile);     // Ptr to the file name
    // Read in []CT
    fQuadCT_CWS =
      GetPrivateProfileFloatW (SECTNAME_SYSVARS,    // Ptr to the section name
                               KEYNAME_QUADCT,      // Ptr to the key name
                               DEF_QUADCT_CWS,      // Ptr to the default value
                               lpwszIniFile);       // Ptr to the file name
    // Read in []ELX
    hGlbQuadELX_CWS =
      GetPrivateProfileGlbCharW (SECTNAME_SYSVARS,  // Ptr to the section name
                                 KEYNAME_QUADELX,   // Ptr to the key name
                                 DEF_QUADELX_CWS_BR,// Ptr to default value
                                 DEF_QUADELX_GLB,   // HGLOBAL of the result
                                 lpwszIniFile);     // Ptr to the file name
    // Read in []FC
    hGlbQuadFC_CWS =
      GetPrivateProfileGlbCharW (SECTNAME_SYSVARS,  // Ptr to the section name
                                 KEYNAME_QUADFC,    // Ptr to the key name
                                 DEF_QUADFC_CWS_BR, // Ptr to default value
                                 DEF_QUADFC_GLB,    // HGLOBAL of the result
                                 lpwszIniFile);     // Ptr to the file name
    // Make the default wide-string form of aplDefaultIC
    for (uCnt = 0, lpwszTemp = wszTemp; uCnt < ICNDX_LENGTH; uCnt++)
        lpwszTemp +=
          wsprintfW (lpwszTemp,
                     L"%d ",
                     aplDefaultIC[uCnt]);
    // Read in []IC
    hGlbQuadIC_CWS =
      GetPrivateProfileGlbIntW (SECTNAME_SYSVARS,   // Ptr to the section name
                                KEYNAME_QUADIC,     // Ptr to the key name
                                wszTemp,            // Ptr to default value
                                DEF_QUADIC_GLB,     // HGLOBAL of the result
                                ICNDX_LENGTH,       // Length of the default integer vector
                                lpwszIniFile);      // Ptr to the file name
    // Read in []IO
    bQuadIO_CWS = (APLBOOL)
      GetPrivateProfileIntW (SECTNAME_SYSVARS,      // Ptr to the section name
                             KEYNAME_QUADIO,        // Ptr to the key name
                             DEF_QUADIO_CWS,        // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in []LX
    hGlbQuadLX_CWS =
      GetPrivateProfileGlbCharW (SECTNAME_SYSVARS,  // Ptr to the section name
                                 KEYNAME_QUADLX,    // Ptr to the key name
                                 DEF_QUADLX_CWS,    // Ptr to default value
                                 DEF_QUADLX_GLB,    // HGLOBAL of the result
                                 lpwszIniFile);     // Ptr to the file name
    // Read in []MF
    uQuadMF_CWS =
      GetPrivateProfileIntW (SECTNAME_SYSVARS,      // Ptr to the section name
                             KEYNAME_QUADMF,        // Ptr to the key name
                             DEF_QUADMF_CWS,        // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in []PP
    uQuadPP_CWS =
      GetPrivateProfileIntW (SECTNAME_SYSVARS,      // Ptr to the section name
                             KEYNAME_QUADPP,        // Ptr to the key name
                             DEF_QUADPP_CWS,        // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in []PW
    uQuadPW_CWS =
      GetPrivateProfileIntW (SECTNAME_SYSVARS,      // Ptr to the section name
                             KEYNAME_QUADPW,        // Ptr to the key name
                             DEF_QUADPW_CWS,        // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in []PR
    GetPrivateProfileStringW (SECTNAME_SYSVARS,     // Ptr to the section name
                              KEYNAME_QUADPR,       // Ptr to the key name
                              DEF_QUADPR_CWS,       // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              TEMPBUFLEN,           // Byte size of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // Convert any {name}s to symbols
    ConvertNameInPlace (wszTemp);

    // If the value is empty (''), ...
    if (wszTemp[0] EQ L'\0')
    {
        cQuadPR_CWS = CQUADPR_MT;
        hGlbQuadPR_CWS = hGlbV0Char;
    } else
        cQuadPR_CWS = wszTemp[0];

    // Read in []RL
    uQuadRL_CWS =
      GetPrivateProfileIntW (SECTNAME_SYSVARS,      // Ptr to the section name
                             KEYNAME_QUADRL,        // Ptr to the key name
                             DEF_QUADRL_CWS,        // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in []SA
    cQuadxSA_CWS = (APLCHAR)
      GetPrivateProfileIntW (SECTNAME_SYSVARS,      // Ptr to the section name
                             KEYNAME_QUADSA,        // Ptr to the key name
                             DEF_QUADxSA_CWS,       // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Set the correct value for []SA
    switch (cQuadxSA_CWS)
    {
        case 0:         // ""
            hGlbQuadSA_CWS = hGlbV0Char;
            break;

        case 1:         // "CLEAR"
            hGlbQuadSA_CWS = hGlbSAClear;
            break;

        case 2:         // "ERROR"
            hGlbQuadSA_CWS = hGlbSAError;
            break;

        case 3:         // "EXIT"
            hGlbQuadSA_CWS = hGlbSAExit;
            break;

        case 4:         // "OFF"
            hGlbQuadSA_CWS = hGlbSAOff;
            break;

        defstop
            break;
    } // End SWITCH

    //***************************************************************
    // Read in the [RangeLimits] section
    //***************************************************************

    // Read in bRangeLimit.CT
    bRangeLimit.CT =
      GetPrivateProfileIntW (SECTNAME_RANGELIMITS,  // Ptr to the section name
                             KEYNAME_QUADCT,        // Ptr to the key name
                             DEF_RANGELIMIT_CT,     // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bRangeLimit.IC
    bRangeLimit.IC =
      GetPrivateProfileIntW (SECTNAME_RANGELIMITS,  // Ptr to the section name
                             KEYNAME_QUADIC,        // Ptr to the key name
                             DEF_RANGELIMIT_IC,     // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bRangeLimit.IO
    bRangeLimit.IO =
      GetPrivateProfileIntW (SECTNAME_RANGELIMITS,  // Ptr to the section name
                             KEYNAME_QUADIO,        // Ptr to the key name
                             DEF_RANGELIMIT_IO,     // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bRangeLimit.PP
    bRangeLimit.PP =
      GetPrivateProfileIntW (SECTNAME_RANGELIMITS,  // Ptr to the section name
                             KEYNAME_QUADPP,        // Ptr to the key name
                             DEF_RANGELIMIT_PP,     // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bRangeLimit.PW
    bRangeLimit.PW =
      GetPrivateProfileIntW (SECTNAME_RANGELIMITS,  // Ptr to the section name
                             KEYNAME_QUADPW,        // Ptr to the key name
                             DEF_RANGELIMIT_PW,     // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bRangeLimit.RL
    bRangeLimit.RL =
      GetPrivateProfileIntW (SECTNAME_RANGELIMITS,  // Ptr to the section name
                             KEYNAME_QUADRL,        // Ptr to the key name
                             DEF_RANGELIMIT_RL,     // Default value if not found
                             lpwszIniFile);         // Ptr to the file name

    //***************************************************************
    // Read in the [ResetVars] section
    //***************************************************************

    // Read in bResetVars.CT
    bResetVars.CT =
      GetPrivateProfileIntW (SECTNAME_RESETVARS,    // Ptr to the section name
                             KEYNAME_QUADCT,        // Ptr to the key name
                             DEF_RESETVARS_CT,      // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bResetVars.FC
    bResetVars.FC =
      GetPrivateProfileIntW (SECTNAME_RESETVARS,    // Ptr to the section name
                             KEYNAME_QUADFC,        // Ptr to the key name
                             DEF_RESETVARS_FC,      // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bResetVars.IC
    bResetVars.IC =
      GetPrivateProfileIntW (SECTNAME_RESETVARS,    // Ptr to the section name
                             KEYNAME_QUADIC,        // Ptr to the key name
                             DEF_RESETVARS_IC,      // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bResetVars.IO
    bResetVars.IO =
      GetPrivateProfileIntW (SECTNAME_RESETVARS,    // Ptr to the section name
                             KEYNAME_QUADIO,        // Ptr to the key name
                             DEF_RESETVARS_IO,      // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bResetVars.PP
    bResetVars.PP =
      GetPrivateProfileIntW (SECTNAME_RESETVARS,    // Ptr to the section name
                             KEYNAME_QUADPP,        // Ptr to the key name
                             DEF_RESETVARS_PP,      // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bResetVars.PW
    bResetVars.PW =
      GetPrivateProfileIntW (SECTNAME_RESETVARS,    // Ptr to the section name
                             KEYNAME_QUADPW,        // Ptr to the key name
                             DEF_RESETVARS_PW,      // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bResetVars.RL
    bResetVars.RL =
      GetPrivateProfileIntW (SECTNAME_RESETVARS,    // Ptr to the section name
                             KEYNAME_QUADRL,        // Ptr to the key name
                             DEF_RESETVARS_RL,      // Default value if not found
                             lpwszIniFile);         // Ptr to the file name

    //***************************************************************
    // Read in the [Colors] section
    //***************************************************************

    // Read in GlbName
    GetPrivateProfileStringW (SECTNAME_COLORS,      // Ptr to the section name
                              KEYNAME_SC_GLBNAME,   // Ptr to the key name
                              L"",                  // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              TEMPBUFLEN,           // Byte size of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // Check for value
    if (wszTemp[0] NE L'\0')
        // Convert the numbers
        swscanf (wszTemp,
                 L"%u, %u, %u",
                &gSyntaxColors  [SC_GLBNAME].crFore,
                &gSyntaxColors  [SC_GLBNAME].crBack,
                &gSyntClrBGTrans[SC_GLBNAME]);
    // Read in LclName
    GetPrivateProfileStringW (SECTNAME_COLORS,      // Ptr to the section name
                              KEYNAME_SC_LCLNAME,   // Ptr to the key name
                              L"",                  // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              TEMPBUFLEN,           // Byte size of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // Check for value
    if (wszTemp[0] NE L'\0')
        // Convert the numbers
        swscanf (wszTemp,
                 L"%u, %u, %u",
                &gSyntaxColors  [SC_LCLNAME].crFore,
                &gSyntaxColors  [SC_LCLNAME].crBack,
                &gSyntClrBGTrans[SC_LCLNAME]);
    // Read in Label
    GetPrivateProfileStringW (SECTNAME_COLORS,      // Ptr to the section name
                              KEYNAME_SC_LABEL,     // Ptr to the key name
                              L"",                  // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              TEMPBUFLEN,           // Byte size of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // Check for value
    if (wszTemp[0] NE L'\0')
        // Convert the numbers
        swscanf (wszTemp,
                 L"%u, %u, %u",
                &gSyntaxColors  [SC_LABEL].crFore,
                &gSyntaxColors  [SC_LABEL].crBack,
                &gSyntClrBGTrans[SC_LABEL]);
    // Read in Prim
    GetPrivateProfileStringW (SECTNAME_COLORS,      // Ptr to the section name
                              KEYNAME_SC_PRIM,      // Ptr to the key name
                              L"",                  // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              TEMPBUFLEN,           // Byte size of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // Check for value
    if (wszTemp[0] NE L'\0')
        // Convert the numbers
        swscanf (wszTemp,
                 L"%u, %u, %u",
                &gSyntaxColors  [SC_PRIMITIVE].crFore,
                &gSyntaxColors  [SC_PRIMITIVE].crBack,
                &gSyntClrBGTrans[SC_PRIMITIVE]);
    // Read in SysFcn
    GetPrivateProfileStringW (SECTNAME_COLORS,      // Ptr to the section name
                              KEYNAME_SC_SYSFCN,    // Ptr to the key name
                              L"",                  // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              TEMPBUFLEN,           // Byte size of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // Check for value
    if (wszTemp[0] NE L'\0')
        // Convert the numbers
        swscanf (wszTemp,
                 L"%u, %u, %u",
                &gSyntaxColors  [SC_SYSFCN].crFore,
                &gSyntaxColors  [SC_SYSFCN].crBack,
                &gSyntClrBGTrans[SC_SYSFCN]);
    // Read in GlbSysVar
    GetPrivateProfileStringW (SECTNAME_COLORS,      // Ptr to the section name
                              KEYNAME_SC_GLBSYSVAR, // Ptr to the key name
                              L"",                  // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              TEMPBUFLEN,           // Byte size of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // Check for value
    if (wszTemp[0] NE L'\0')
        // Convert the numbers
        swscanf (wszTemp,
                 L"%u, %u, %u",
                &gSyntaxColors  [SC_GLBSYSVAR].crFore,
                &gSyntaxColors  [SC_GLBSYSVAR].crBack,
                &gSyntClrBGTrans[SC_GLBSYSVAR]);
    // Read in LclSysVar
    GetPrivateProfileStringW (SECTNAME_COLORS,      // Ptr to the section name
                              KEYNAME_SC_LCLSYSVAR, // Ptr to the key name
                              L"",                  // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              TEMPBUFLEN,           // Byte size of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // Check for value
    if (wszTemp[0] NE L'\0')
        // Convert the numbers
        swscanf (wszTemp,
                 L"%u, %u, %u",
                &gSyntaxColors  [SC_LCLSYSVAR].crFore,
                &gSyntaxColors  [SC_LCLSYSVAR].crBack,
                &gSyntClrBGTrans[SC_LCLSYSVAR]);
    // Read in CtrlStruc
    GetPrivateProfileStringW (SECTNAME_COLORS,      // Ptr to the section name
                              KEYNAME_SC_CTRLSTRUC, // Ptr to the key name
                              L"",                  // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              TEMPBUFLEN,           // Byte size of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // Check for value
    if (wszTemp[0] NE L'\0')
        // Convert the numbers
        swscanf (wszTemp,
                 L"%u, %u, %u",
                &gSyntaxColors  [SC_CTRLSTRUC].crFore,
                &gSyntaxColors  [SC_CTRLSTRUC].crBack,
                &gSyntClrBGTrans[SC_CTRLSTRUC]);
    // Read in NumConst
    GetPrivateProfileStringW (SECTNAME_COLORS,      // Ptr to the section name
                              KEYNAME_SC_NUMCONST,  // Ptr to the key name
                              L"",                  // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              TEMPBUFLEN,           // Byte size of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // Check for value
    if (wszTemp[0] NE L'\0')
        // Convert the numbers
        swscanf (wszTemp,
                 L"%u, %u, %u",
                &gSyntaxColors  [SC_NUMCONST].crFore,
                &gSyntaxColors  [SC_NUMCONST].crBack,
                &gSyntClrBGTrans[SC_NUMCONST]);
    // Read in ChrConst
    GetPrivateProfileStringW (SECTNAME_COLORS,      // Ptr to the section name
                              KEYNAME_SC_CHRCONST,  // Ptr to the key name
                              L"",                  // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              TEMPBUFLEN,           // Byte size of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // Check for value
    if (wszTemp[0] NE L'\0')
        // Convert the numbers
        swscanf (wszTemp,
                 L"%u, %u, %u",
                &gSyntaxColors  [SC_CHRCONST].crFore,
                &gSyntaxColors  [SC_CHRCONST].crBack,
                &gSyntClrBGTrans[SC_CHRCONST]);
    // Read in Comment
    GetPrivateProfileStringW (SECTNAME_COLORS,      // Ptr to the section name
                              KEYNAME_SC_COMMENT,   // Ptr to the key name
                              L"",                  // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              TEMPBUFLEN,           // Byte size of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // Check for value
    if (wszTemp[0] NE L'\0')
        // Convert the numbers
        swscanf (wszTemp,
                 L"%u, %u, %u",
                &gSyntaxColors  [SC_COMMENT].crFore,
                &gSyntaxColors  [SC_COMMENT].crBack,
                &gSyntClrBGTrans[SC_COMMENT]);
    // Read in MatchGrp1
    GetPrivateProfileStringW (SECTNAME_COLORS,      // Ptr to the section name
                              KEYNAME_SC_MATCHGRP1, // Ptr to the key name
                              L"",                  // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              TEMPBUFLEN,           // Byte size of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // Check for value
    if (wszTemp[0] NE L'\0')
        // Convert the numbers
        swscanf (wszTemp,
                 L"%u, %u, %u",
                &gSyntaxColors  [SC_MATCHGRP1].crFore,
                &gSyntaxColors  [SC_MATCHGRP1].crBack,
                &gSyntClrBGTrans[SC_MATCHGRP1]);
    // Read in MatchGrp2
    GetPrivateProfileStringW (SECTNAME_COLORS,      // Ptr to the section name
                              KEYNAME_SC_MATCHGRP2, // Ptr to the key name
                              L"",                  // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              TEMPBUFLEN,           // Byte size of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // Check for value
    if (wszTemp[0] NE L'\0')
        // Convert the numbers
        swscanf (wszTemp,
                 L"%u, %u, %u",
                &gSyntaxColors  [SC_MATCHGRP2].crFore,
                &gSyntaxColors  [SC_MATCHGRP2].crBack,
                &gSyntClrBGTrans[SC_MATCHGRP2]);
    // Read in MatchGrp3
    GetPrivateProfileStringW (SECTNAME_COLORS,      // Ptr to the section name
                              KEYNAME_SC_MATCHGRP3, // Ptr to the key name
                              L"",                  // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              TEMPBUFLEN,           // Byte size of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // Check for value
    if (wszTemp[0] NE L'\0')
        // Convert the numbers
        swscanf (wszTemp,
                 L"%u, %u, %u",
                &gSyntaxColors  [SC_MATCHGRP3].crFore,
                &gSyntaxColors  [SC_MATCHGRP3].crBack,
                &gSyntClrBGTrans[SC_MATCHGRP3]);
    // Read in MatchGrp4
    GetPrivateProfileStringW (SECTNAME_COLORS,      // Ptr to the section name
                              KEYNAME_SC_MATCHGRP4, // Ptr to the key name
                              L"",                  // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              TEMPBUFLEN,           // Byte size of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // Check for value
    if (wszTemp[0] NE L'\0')
        // Convert the numbers
        swscanf (wszTemp,
                 L"%u, %u, %u",
                &gSyntaxColors  [SC_MATCHGRP4].crFore,
                &gSyntaxColors  [SC_MATCHGRP4].crBack,
                &gSyntClrBGTrans[SC_MATCHGRP4]);
    // Read in UnmatchGrp
    GetPrivateProfileStringW (SECTNAME_COLORS,      // Ptr to the section name
                              KEYNAME_SC_UNMATCHGRP,// Ptr to the key name
                              L"",                  // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              TEMPBUFLEN,           // Byte size of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // Check for value
    if (wszTemp[0] NE L'\0')
        // Convert the numbers
        swscanf (wszTemp,
                 L"%u, %u, %u",
                &gSyntaxColors  [SC_UNMATCHGRP].crFore,
                &gSyntaxColors  [SC_UNMATCHGRP].crBack,
                &gSyntClrBGTrans[SC_UNMATCHGRP]);
    // Read in Unk
    GetPrivateProfileStringW (SECTNAME_COLORS,      // Ptr to the section name
                              KEYNAME_SC_UNK,       // Ptr to the key name
                              L"",                  // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              TEMPBUFLEN,           // Byte size of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // Check for value
    if (wszTemp[0] NE L'\0')
        // Convert the numbers
        swscanf (wszTemp,
                 L"%u, %u, %u",
                &gSyntaxColors  [SC_UNK].crFore,
                &gSyntaxColors  [SC_UNK].crBack,
                &gSyntClrBGTrans[SC_UNK]);
#undef  TEMPBUFLEN
} // End ReadIniFileGlb


//***************************************************************************
//  $GetPrivateProfileLogFontW
//
//  Get a LOGFONTW struc from a .ini file
//***************************************************************************

void GetPrivateProfileLogFontW
    (LPWCHAR    lpwSectName,                        // Ptr to the section name
     LPWCHAR    wszKeyName,                         // Ptr to the key name
     LPLOGFONTW lplfFont)                           // Ptr to the output buffer

{
    WCHAR wszTemp[1024];            // Temporary storage for string results

    GetPrivateProfileStringW (lpwSectName,          // Ptr to the section name
                              wszKeyName,           // Ptr to the key name
                              L"",                  // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              sizeof (wszTemp) - 1, // Byte size of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // If the new value is present, ...
    if (wszTemp[0])
    {
        // Note that this works for little-endian formats only as
        //   the .lfItalic and following numeric fields are BYTE
        //   but there is no way to tell swscanf about that, so it
        //   treats them as four-byte ints overwiting the next three
        //   bytes each time it scans a number which is actually
        //   a one-byte int.  As this happens sequentially and the
        //   last field is the FaceName, there is no harm.
        swscanf (wszTemp,
                 FMTSTR_LOGFONT_INP,
                &lplfFont->lfHeight,
                &lplfFont->lfWidth,
                &lplfFont->lfEscapement,
                &lplfFont->lfOrientation,
                &lplfFont->lfWeight,
                &lplfFont->lfItalic,
                &lplfFont->lfUnderline,
                &lplfFont->lfStrikeOut,
                &lplfFont->lfCharSet,
                &lplfFont->lfOutPrecision,
                &lplfFont->lfClipPrecision,
                &lplfFont->lfQuality,
                &lplfFont->lfPitchAndFamily,
                &lplfFont->lfFaceName);
        // If the facename contains an embedded blank, swscanf misses
        //   the tail of the name so we do it over again here
        lstrcpyW (lplfFont->lfFaceName, strchrW (wszTemp, L'\'') + 1);
        *(strchrW (lplfFont->lfFaceName, L'\'')) = L'\0';
    } // End IF
} // End GetPrivateProfileLogFontW


//***************************************************************************
//  $GetPrivateProfileFloatW
//
//  Read in a float value from a .ini file
//***************************************************************************

APLFLOAT GetPrivateProfileFloatW
    (LPWCHAR  lpwSectName,                          // Ptr to the section name
     LPWCHAR  lpwKeyName,                           // Ptr to the key name
     APLFLOAT fDefVal,                              // Ptr to the default value
     LPWCHAR  lpwszIniFile)                         // Ptr to the file name

{
    WCHAR wszTemp[1024];                            // Temporary storage for string results
    char  szTemp[1024];                             // ...

    GetPrivateProfileStringW (lpwSectName,          // Ptr to the section name
                              lpwKeyName,           // Ptr to the key name
                              L"",                  // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              sizeof (wszTemp),     // Byte size of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // If the new value is present, ...
    if (wszTemp[0])
    {
        // Convert wide to single-byte so we can use strtod
        //   which has no wide counterpart
        W2A (szTemp, wszTemp, sizeof (wszTemp));

        // Use David Gay's routines
        return strtod (szTemp, NULL);
    } else
        return fDefVal;
} // End GetPrivateProfileFloatW


//***************************************************************************
//  $GetPrivateProfileGlbCharW
//
//  Read in a global char vector from a .ini file
//***************************************************************************

HGLOBAL GetPrivateProfileGlbCharW
    (LPWCHAR  lpwSectName,                          // Ptr to the section name
     LPWCHAR  lpwKeyName,                           // Ptr to the key name
     LPWCHAR  lpwDefVal,                            // Ptr to the default value
     HGLOBAL  hDefVal,                              // Default value global memory handle
     LPWCHAR  lpwszIniFile)                         // Ptr to the file name

{
    return
    GetPrivateProfileGlbComW (lpwSectName,          // Ptr to the section name
                              lpwKeyName,           // Ptr to the key name
                              lpwDefVal,            // Ptr to the default value
                              hDefVal,              // Default value global memory handle
                              ARRAY_CHAR,           // Result storage type
                              sizeof (APLCHAR),     // Size of each item in the result
                              lstrlenW (lpwDefVal) - 2,// Length of the default vector
                                                    //   less the surrounding single quotes
                              lpwszIniFile);        // Ptr to the file name
} // End GetPrivateProfileGlbCharW


//***************************************************************************
//  $GetPrivateProfileGlbIntW
//
//  Read in a global integer vector from a .ini file
//***************************************************************************

HGLOBAL GetPrivateProfileGlbIntW
    (LPWCHAR  lpwSectName,                          // Ptr to the section name
     LPWCHAR  lpwKeyName,                           // Ptr to the key name
     LPWCHAR  lpwDefVal,                            // Ptr to the default value
     HGLOBAL  hDefVal,                              // Default value global memory handle
     UINT     uVecLen,                              // Length of the default integer vector
     LPWCHAR  lpwszIniFile)                         // Ptr to the file name

{
    return
    GetPrivateProfileGlbComW (lpwSectName,          // Ptr to the section name
                              lpwKeyName,           // Ptr to the key name
                              lpwDefVal,            // Ptr to the default value
                              hDefVal,              // Default value global memory handle
                              ARRAY_INT,            // Result storage type
                              sizeof (APLINT),      // Size of each item in the result
                              uVecLen,              // Length of the default vector
                              lpwszIniFile);        // Ptr to the file name
} // End GetPrivateProfileGlbIntW


//***************************************************************************
//  $GetPrivateProfileGlbComW
//
//  Read in a global integer or char vector from a .ini file
//***************************************************************************

HGLOBAL GetPrivateProfileGlbComW
    (LPWCHAR  lpwSectName,                          // Ptr to the section name
     LPWCHAR  lpwKeyName,                           // Ptr to the key name
     LPWCHAR  lpDefVal,                             // Ptr to the default values
     HGLOBAL  hDefVal,                              // Default value global memory handle
     APLSTYPE aplTypeRes,                           // Result storage type
     UINT     aplSizeRes,                           // Size of each item in the result
     UINT     uVecLen,                              // Length of the default vector
     LPWCHAR  lpwszIniFile)                         // Ptr to the file name

{
    WCHAR   wszTemp[1024];                          // Temporary storage for string results
    APLNELM aplNELMRes;                             // Result NELM
    APLUINT ByteRes;                                // # bytes in the result
    HGLOBAL hGlbRes;                                // Result global memory handle
    LPVOID  lpMemRes,                               // Ptr to result global memory
            lpMemInp;                               // Ptr to input global memory

    GetPrivateProfileStringW (lpwSectName,          // Ptr to the section name
                              lpwKeyName,           // Ptr to the key name
                              L"\x0001",            // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              sizeof (wszTemp),     // Byte size of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // If the new value is present, ...
    if (wszTemp[0] NE L'\x0001')
    {
        // Catch short default values of []IC where we inserted a
        //   new value into the middle of the integer vector
        if (lstrcmpW (lpwKeyName, KEYNAME_QUADIC) EQ 0
         && lstrcmpW (wszTemp, L"3 4 2 2 2 1 2 2 1 2 2 2 1") EQ 0)
            // Use the new default value
            lstrcpyW (wszTemp, L"3 4 2 2 2 1 2 2 1 2 2 2 2 1");
        // Use the given (or substituted value)
        lpMemInp = wszTemp;
    } else
        // Use the default value
        lpMemInp = lpDefVal;

    // Find out how many elements are in the .ini file value
    aplNELMRes = ScanNELM (lpMemInp, aplTypeRes);

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, 1);

    // Allocate space for the data
    // Note, we can't use DbgGlobalAlloc here as the
    //   PTD has not been allocated as yet
    hGlbRes = MyGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        return hGlbRes;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Save the dimension
    *VarArrayBaseToDim (lpMemRes) = aplNELMRes;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, 1);

    // Copy and convert the values to the result
    CopyConvertDataOfType (lpMemRes, aplTypeRes, aplNELMRes, lpMemInp);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    return hGlbRes;
} // End GetPrivateProfileGlbComW


//***************************************************************************
//  $CopyConvertDataOfType
//
//  Copy and convert data of a given type to another location
//***************************************************************************

void CopyConvertDataOfType
    (LPVOID   lpMemRes,                             // Ptr to result global memory
     APLSTYPE aplTypeRes,                           // Result storage type
     APLNELM  aplNELMRes,                           // Result NELM
     LPWCHAR  wszTemp)                              // Ptr to the output buffer

{
    APLUINT uCnt;                                   // Loop counter
    WCHAR   wcTmp;                                  // Temporary char

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        case ARRAY_INT:
            // Loop through the result elements
            for (uCnt = 0; uCnt < aplNELMRes; uCnt++)
            {
                // Convert the next #
                swscanf (wszTemp,
                         L"%I64d",
                         lpMemRes);
                // Skip to the next field
                wszTemp = SkipPastCharW (wszTemp, L' ');

                // Skip to next result location
                ((LPAPLINT) lpMemRes)++;
            } // End FOR

            break;

        case ARRAY_CHAR:
            // Loop through the result elements
            for (uCnt = 0; uCnt < aplNELMRes; uCnt++)
            {
                // Convert the single {name} or other char to UTF16_xxx
                if (L'{' EQ  *wszTemp)
                {
                    // Get the next char
                    wcTmp = SymbolNameToChar (wszTemp);

                    // If there's a matching UTF16_xxx equivalent, ...
                    if (wcTmp)
                    {
                        // Save in the result and skip over it
                        *((LPAPLCHAR) lpMemRes)++ = wcTmp;

                        // Skip to the next field
                        wszTemp = SkipPastCharW (wszTemp, L'}');
                    } else
                    {
                        // Copy source to destin up to and including the matching '}'
                        while (wszTemp[0] NE L'}')
                            // Save in the result and skip over it
                            *((LPAPLCHAR) lpMemRes)++ = *wszTemp++;

                        // Copy the '}'
                        *((LPAPLCHAR) lpMemRes)++ = *wszTemp++;
                    } // End IF/ELSE
                } else
                    // Save in the result and skip over it
                    *((LPAPLCHAR) lpMemRes)++ = *wszTemp++;
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH
} // End CopyConvertDataOfType


//***************************************************************************
//  $ScanNELM
//
//  Scan a string and return the # elements in it
//***************************************************************************

APLNELM ScanNELM
    (LPWCHAR  wszTemp,
     APLSTYPE aplTypeRes)

{
    APLNELM aplNELMRes;             // Result NELM
    APLINT  aplIntTemp;             // Temporary integer for swscanf

    // Initialize the result NELM
    aplNELMRes = 0;

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        case ARRAY_INT:
            // Loop through the arg counting one integer at a time
            while (*wszTemp)
            {
                if (EOF EQ swscanf (wszTemp,
                                    L"%I64d",
                                   &aplIntTemp))
                    break;
                aplNELMRes++;
                wszTemp = SkipPastCharW (wszTemp, L' ');
            } // End WHILE

            return aplNELMRes;

        case ARRAY_CHAR:
            // Loop through the arg counting {name} as one char
            while (*wszTemp)
            {
                if (*wszTemp EQ L'{')
                    // Skip over the name
                    wszTemp = SkipPastCharW (wszTemp, L'}');
                else
                    // Skip to next char
                    wszTemp++;

                // Count in another char
                aplNELMRes++;
            } // End WHILE

            return aplNELMRes;

        defstop
            return (APLNELM) -1;
    } // End SWITCH
} // End ScanNELM


//***************************************************************************
//  $ReadIniFileWnd
//
//  Read in window-specific .ini file settings
//***************************************************************************

void ReadIniFileWnd
    (HWND hWnd)

{
    RECT  rcDtop;           // Rectangle for desktop
    POINT PosCtr;           // x- and y- positions

    // Read in the values from the [General] section

    // Setup the default values in case the .INI file is not present
    GetWindowRect (hWnd, &rcDtop);
    MFSize.cx = rcDtop.right  - rcDtop.left;
    MFSize.cy = rcDtop.bottom - rcDtop.top;

    // Save the window size for use in WM_MOVE messages
    // Note that this is the window size, not the client area size.
    MFSize.cx =
      GetPrivateProfileIntW (SECTNAME_GENERAL,      // Ptr to the section name
                             KEYNAME_XSIZE,         // Ptr to the key name
                             MFSize.cx,             // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    MFSize.cy =
      GetPrivateProfileIntW (SECTNAME_GENERAL,      // Ptr to the section name
                             KEYNAME_YSIZE,         // Ptr to the key name
                             MFSize.cy,             // Default value if not found
                             lpwszIniFile);         // Ptr to the file name

    // Setup the default values in case the .INI file is not present
    GetWindowRect (GetDesktopWindow(), &rcDtop);
    PosCtr.x = rcDtop.left + MFSize.cx / 2; // Center horizontally
    PosCtr.y = rcDtop.top  + MFSize.cy / 2; // Center vertically

    MFPosCtr.x =
      GetPrivateProfileIntW (SECTNAME_GENERAL,      // Ptr to the section name
                             KEYNAME_XPOS,          // Ptr to the key name
                             PosCtr.x,              // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    MFPosCtr.y =
      GetPrivateProfileIntW (SECTNAME_GENERAL,      // Ptr to the section name
                             KEYNAME_YPOS,          // Ptr to the key name
                             PosCtr.y,              // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    MFSizeState =
      GetPrivateProfileIntW (SECTNAME_GENERAL,      // Ptr to the section name
                             KEYNAME_SIZESTATE,     // Ptr to the key name
                             MFSizeState,           // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    gInitCustomizeCategory =
      GetPrivateProfileIntW (SECTNAME_GENERAL,      // Ptr to the section name
                             KEYNAME_INITIALCAT,    // Ptr to the key name
                             DEF_INIT_CATEGORY,     // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
} // End ReadIniFileWnd


//***************************************************************************
//  $SaveIniFile
//
//  Save window-specific values into an .ini file
//***************************************************************************

void SaveIniFile
    (void)

{
    WCHAR     wszTemp[1024],                                // Temporary storage
              wszKey[3];                                    // ...
    UINT      uCnt;                                         // Loop counter
    LPVOID    lpMemObj;                                     // Ptr to object global memory
    LPAPLCHAR lpaplChar;                                    // Ptr to output save area
    APLNELM   aplNELMObj;                                   // Object NELM

    //*********************************************************
    // Write out [General] section entries
    //*********************************************************

    // Write out the version #
    WritePrivateProfileStringW (SECTNAME_GENERAL,           // Ptr to the section name
                                KEYNAME_VERSION,            // Ptr to the key name
                                L"0.01",                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    // Format the x-position
    wsprintfW (wszTemp,
               L"%d",
               MFPosCtr.x);
    // Write out the x-position
    WritePrivateProfileStringW (SECTNAME_GENERAL,           // Ptr to the section name
                                KEYNAME_XPOS,               // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    // Format the x-size
    wsprintfW (wszTemp,
               L"%d",
               MFSize.cx);
    // Write out the x-size
    WritePrivateProfileStringW (SECTNAME_GENERAL,           // Ptr to the section name
                                KEYNAME_XSIZE,              // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    // Format the y-position
    wsprintfW (wszTemp,
               L"%d",
               MFPosCtr.y);
    // Write out the y-position
    WritePrivateProfileStringW (SECTNAME_GENERAL,           // Ptr to the section name
                                KEYNAME_YPOS,               // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    // Format the y-size
    wsprintfW (wszTemp,
               L"%d",
               MFSize.cy);
    // Write out the y-size
    WritePrivateProfileStringW (SECTNAME_GENERAL,           // Ptr to the section name
                                KEYNAME_YSIZE,              // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    // Format the size state
    wsprintfW (wszTemp,
               L"%d",
               MFSizeState);
    // Write out the size state
    WritePrivateProfileStringW (SECTNAME_GENERAL,           // Ptr to the section name
                                KEYNAME_SIZESTATE,          // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    // Format the initial category
    wsprintfW (wszTemp,
               L"%d",
               gInitCustomizeCategory);
    // Write out the initial category
    WritePrivateProfileStringW (SECTNAME_GENERAL,           // Ptr to the section name
                                KEYNAME_INITIALCAT,         // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name

    //*********************************************************
    // Write out [Fonts] section entries
    //*********************************************************

    // Write out the LOGFONTW struc for FE
    WritePrivateProfileLogfontW (SECTNAME_FONTS,            // Ptr to the section name
                                 KEYNAME_LOGFONTFE,         // Ptr to the key name
                                &lfFE,                      // Ptr to LOGFONTW
                                 lpwszIniFile);             // Ptr to the file name
    // Write out the LOGFONTW struc for ME
    WritePrivateProfileLogfontW (SECTNAME_FONTS,            // Ptr to the section name
                                 KEYNAME_LOGFONTME,         // Ptr to the key name
                                &lfME,                      // Ptr to LOGFONTW
                                 lpwszIniFile);             // Ptr to the file name
    // Write out the LOGFONTW struc for PR
    WritePrivateProfileLogfontW (SECTNAME_FONTS,            // Ptr to the section name
                                 KEYNAME_LOGFONTPR,         // Ptr to the key name
                                &lfPR,                      // Ptr to LOGFONTW
                                 lpwszIniFile);             // Ptr to the file name
    // Write out the LOGFONTW struc for SM
    WritePrivateProfileLogfontW (SECTNAME_FONTS,            // Ptr to the section name
                                 KEYNAME_LOGFONTSM,         // Ptr to the key name
                                &lfSM,                      // Ptr to LOGFONTW
                                 lpwszIniFile);             // Ptr to the file name
    // Write out the LOGFONTW struc for TC
    WritePrivateProfileLogfontW (SECTNAME_FONTS,            // Ptr to the section name
                                 KEYNAME_LOGFONTTC,         // Ptr to the key name
                                &lfTC,                      // Ptr to LOGFONTW
                                 lpwszIniFile);             // Ptr to the file name
    // Write out the LOGFONTW struc for VE
    WritePrivateProfileLogfontW (SECTNAME_FONTS,            // Ptr to the section name
                                 KEYNAME_LOGFONTVE,         // Ptr to the key name
                                &lfVE,                      // Ptr to LOGFONTW
                                 lpwszIniFile);             // Ptr to the file name
    //*********************************************************
    // Write out [SameFontAs] section entries
    //*********************************************************

    for (uCnt = 0; uCnt < FONTENUM_LENGTH; uCnt++)
    {
        wsprintfW (wszKey,
                   L"%u",
                   uCnt);
        wsprintfW (wszTemp,
                   L"%u",
                   glbSameFontAs[uCnt]);
        WritePrivateProfileStringW (SECTNAME_SAMEFONTAS,    // Ptr to the section name
                                    wszKey,                 // Ptr to the key name
                                    wszTemp,                // Ptr to the SameFontAs value
                                    lpwszIniFile);          // Ptr to the file name
    } // End FOR

    //*********************************************************
    // Write out [Options] section entries
    //*********************************************************

    //******************* bAdjustPW ***************************
    wszTemp[0] = L'0' + OptionFlags.bAdjustPW;
    wszTemp[1] = L'\0';

    // Write out bAdjustPw
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_ADJUSTPW,           // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bUnderbarToLowercase ****************
    wszTemp[0] = L'0' + OptionFlags.bUnderbarToLowercase;
    wszTemp[1] = L'\0';

    // Write out bUnderbarToLowercase
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_UNDERBARTOLOWERCASE,// Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bNewTabOnClear **********************
    wszTemp[0] = L'0' + OptionFlags.bNewTabOnClear;
    wszTemp[1] = L'\0';

    // Write out bNewTabOnClear
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_NEWTABONCLEAR,      // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bNewTabOnLoad ***********************
    wszTemp[0] = L'0' + OptionFlags.bNewTabOnLoad;
    wszTemp[1] = L'\0';

    // Write out bNewTabOnLoad
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_NEWTABONLOAD,       // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bUseLocalTime ***********************
    wszTemp[0] = L'0' + OptionFlags.bUseLocalTime;
    wszTemp[1] = L'\0';

    // Write out bUseLocalTime
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_USELOCALTIME,       // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bBackupOnLoad ***********************
    wszTemp[0] = L'0' + OptionFlags.bBackupOnLoad;
    wszTemp[1] = L'\0';

    // Write out bBackupOnLoad
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_BACKUPONLOAD,       // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bBackupOnSave ***********************
    wszTemp[0] = L'0' + OptionFlags.bBackupOnSave;
    wszTemp[1] = L'\0';

    // Write out bBackupOnSave
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_BACKUPONSAVE,       // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bNoCopyrightMsg *********************
    wszTemp[0] = L'0' + OptionFlags.bNoCopyrightMsg;
    wszTemp[1] = L'\0';

    // Write out bNoCopyrightMsg
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_NOCOPYRIGHTMSG,     // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* uDefaultPaste ***********************
    wsprintfW (wszTemp,
               L"%u",
               OptionFlags.uDefaultPaste);
    // Write out uDefaultPaste
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_DEFAULTPASTE,       // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* uDefaultCopy ************************
    wsprintfW (wszTemp,
               L"%u",
               OptionFlags.uDefaultCopy);
    // Write out uDefaultCopy
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_DEFAULTCOPY,        // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bSyntClrFcns ************************
    wszTemp[0] = L'0' + OptionFlags.bSyntClrFcns;
    wszTemp[1] = L'\0';

    // Write out bNoCopyrightMsg
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_SYNTCLRFCNS,        // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bSyntClrSess ************************
    wszTemp[0] = L'0' + OptionFlags.bSyntClrSess;
    wszTemp[1] = L'\0';

    // Write out bNoCopyrightMsg
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_SYNTCLRSESS,        // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name

    //*********************************************************
    // Write out [SysVars] section entries
    //*********************************************************

    //************************ []ALX **************************
    // Write out []ALX
    WritePrivateProfileGlbCharW (SECTNAME_SYSVARS,
                                 KEYNAME_QUADALX,
                                 hGlbQuadALX_CWS,
                                 lpwszIniFile);
    //************************ []CT ***************************
    // Format []CT
    lpaplChar =
     FormatFloatFC (wszTemp,                                // Ptr to output save area
                    fQuadCT_CWS,                            // The value to format
                    DEF_MAX_QUADPP,                         // Precision to use
                    L'.',                                   // Char to use as decimal separator
                    L'-',                                   // Char to use as overbar
                    DEF_DTOA_MODE);                         // DTOA mode (Mode 2: max (ndigits, 1))
    // Zap the trailing blank
    lpaplChar[-1] = L'\0';

    // Write out []CT
    WritePrivateProfileStringW (SECTNAME_SYSVARS,           // Ptr to the section name
                                KEYNAME_QUADCT,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //************************ []ELX **************************
    // Write out []ELX
    WritePrivateProfileGlbCharW (SECTNAME_SYSVARS,
                                 KEYNAME_QUADELX,
                                 hGlbQuadELX_CWS,
                                 lpwszIniFile);
    //************************ []FC ***************************
    // Write out []FC
    WritePrivateProfileGlbCharW (SECTNAME_SYSVARS,
                                 KEYNAME_QUADFC,
                                 hGlbQuadFC_CWS,
                                 lpwszIniFile);
    //************************ []IC ***************************
    // Lock the memory to get a ptr to it
    lpMemObj = MyGlobalLock (hGlbQuadIC_CWS);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemObj)
    // Get the # bytes
    aplNELMObj = lpHeader->NELM;
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMemObj = VarArrayBaseToData (lpMemObj, 1);

    // Format []IC
    lpaplChar = wszTemp;
    while (aplNELMObj--)
        lpaplChar +=
          wsprintfW (lpaplChar,
                     L"%d ",
                    *((LPAPLINT) lpMemObj)++);
    // Zap the trailing blank
    lpaplChar[-1] = L'\0';

    // We no longer need this ptr
    MyGlobalUnlock (hGlbQuadIC_CWS); lpMemObj = NULL;

    // Write out []IC
    WritePrivateProfileStringW (SECTNAME_SYSVARS,           // Ptr to the section name
                                KEYNAME_QUADIC,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //************************ []IO ***************************
    // Format []IO
    lpaplChar =
      FormatAplintFC (wszTemp,                              // Ptr to output save area
                      bQuadIO_CWS,                          // The value to format
                      L'-');                                // Char to use as overbar
    // Zap the trailing blank
    lpaplChar[-1] = L'\0';

    // Write out []IO
    WritePrivateProfileStringW (SECTNAME_SYSVARS,           // Ptr to the section name
                                KEYNAME_QUADIO,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //************************ []LX ***************************
    // Write out []LX
    WritePrivateProfileGlbCharW (SECTNAME_SYSVARS,
                                 KEYNAME_QUADLX,
                                 hGlbQuadLX_CWS,
                                 lpwszIniFile);
    //************************ []MF ***************************
    // Format []MF
    lpaplChar =
      FormatAplintFC (wszTemp,                              // Ptr to output save area
                      uQuadMF_CWS,                          // The value to format
                      L'-');                                // Char to use as overbar
    // Zap the trailing blank
    lpaplChar[-1] = L'\0';

    // Write out []MF
    WritePrivateProfileStringW (SECTNAME_SYSVARS,           // Ptr to the section name
                                KEYNAME_QUADMF,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //************************ []PP ***************************
    // Format []PP
    lpaplChar =
      FormatAplintFC (wszTemp,                              // Ptr to output save area
                      uQuadPP_CWS,                          // The value to format
                      L'-');                                // Char to use as overbar
    // Zap the trailing blank
    lpaplChar[-1] = L'\0';

    // Write out []PP
    WritePrivateProfileStringW (SECTNAME_SYSVARS,           // Ptr to the section name
                                KEYNAME_QUADPP,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //************************ []PR ***************************
    // Format []PR
    if (cQuadPR_CWS EQ CQUADPR_MT)
    {
        wszTemp[0] =
        wszTemp[1] = L'\'';
        wszTemp[2] = L'\0';
    } else
    {
        UINT uLen;

        wszTemp[0] = L'\'';
        uLen =
          ConvertWideToNameLength (&wszTemp[1], &cQuadPR_CWS, 1);
        wszTemp[uLen + 1] = L'\'';
        wszTemp[uLen + 2] = L'\0';
    } // End IF/ELSE

    // Write out []PR
    WritePrivateProfileStringW (SECTNAME_SYSVARS,           // Ptr to the section name
                                KEYNAME_QUADPR,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //************************ []PW ***************************
    // Format []PW
    lpaplChar =
      FormatAplintFC (wszTemp,                              // Ptr to output save area
                      uQuadPW_CWS,                          // The value to format
                      L'-');                                // Char to use as overbar
    // Zap the trailing blank
    lpaplChar[-1] = L'\0';

    // Write out []PW
    WritePrivateProfileStringW (SECTNAME_SYSVARS,           // Ptr to the section name
                                KEYNAME_QUADPW,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //************************ []RL ***************************
    // Format []RL
    lpaplChar =
      FormatAplintFC (wszTemp,                              // Ptr to output save area
                      uQuadRL_CWS,                          // The value to format
                      L'-');                                // Char to use as overbar
    // Zap the trailing blank
    lpaplChar[-1] = L'\0';

    // Write out []RL
    WritePrivateProfileStringW (SECTNAME_SYSVARS,           // Ptr to the section name
                                KEYNAME_QUADRL,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //************************ []SA ***************************
    // Write out []SA
    WritePrivateProfileStringW (SECTNAME_SYSVARS,           // Ptr to the section name
                                KEYNAME_QUADSA,             // Ptr to the key name
                                L"0" + cQuadxSA_CWS,        // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //*********************************************************
    // Write out [RangeLimits] section entries
    //*********************************************************

    //******************* bRangeLimit.CT **********************
    wszTemp[0] = L'0' + bRangeLimit.CT;
    wszTemp[1] = L'\0';

    // Write out bRangeLimit.CT
    WritePrivateProfileStringW (SECTNAME_RANGELIMITS,       // Ptr to the section name
                                KEYNAME_QUADCT,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bRangeLimit.IC **********************
    wszTemp[0] = L'0' + bRangeLimit.IC;
    wszTemp[1] = L'\0';

    // Write out bRangeLimit.IC
    WritePrivateProfileStringW (SECTNAME_RANGELIMITS,       // Ptr to the section name
                                KEYNAME_QUADIC,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bRangeLimit.IO **********************
    wszTemp[0] = L'0' + bRangeLimit.IO;
    wszTemp[1] = L'\0';

    // Write out bRangeLimit.IO
    WritePrivateProfileStringW (SECTNAME_RANGELIMITS,       // Ptr to the section name
                                KEYNAME_QUADIO,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bRangeLimit.PP **********************
    wszTemp[0] = L'0' + bRangeLimit.PP;
    wszTemp[1] = L'\0';

    // Write out bRangeLimit.PP
    WritePrivateProfileStringW (SECTNAME_RANGELIMITS,       // Ptr to the section name
                                KEYNAME_QUADPP,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bRangeLimit.PW **********************
    wszTemp[0] = L'0' + bRangeLimit.PW;
    wszTemp[1] = L'\0';

    // Write out bRangeLimit.PW
    WritePrivateProfileStringW (SECTNAME_RANGELIMITS,       // Ptr to the section name
                                KEYNAME_QUADPW,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bRangeLimit.RL **********************
    wszTemp[0] = L'0' + bRangeLimit.RL;
    wszTemp[1] = L'\0';

    // Write out bRangeLimit.RL
    WritePrivateProfileStringW (SECTNAME_RANGELIMITS,       // Ptr to the section name
                                KEYNAME_QUADRL,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //*********************************************************
    // Write out [ResetVars] section entries
    //*********************************************************

    //****************** bResetVars.CT ************************
    wszTemp[0] = L'0' + bResetVars.CT;
    wszTemp[1] = L'\0';

    // Write out bResetVars.CT
    WritePrivateProfileStringW (SECTNAME_RESETVARS,         // Ptr to the section name
                                KEYNAME_QUADCT,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** bResetVars.FC ************************
    wszTemp[0] = L'0' + bResetVars.FC;
    wszTemp[1] = L'\0';

    // Write out bResetVars.FC
    WritePrivateProfileStringW (SECTNAME_RESETVARS,         // Ptr to the section name
                                KEYNAME_QUADFC,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** bResetVars.IC ************************
    wszTemp[0] = L'0' + bResetVars.IC;
    wszTemp[1] = L'\0';

    // Write out bResetVars.IC
    WritePrivateProfileStringW (SECTNAME_RESETVARS,         // Ptr to the section name
                                KEYNAME_QUADIC,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** bResetVars.IO ************************
    wszTemp[0] = L'0' + bResetVars.IO;
    wszTemp[1] = L'\0';

    // Write out bResetVars.IO
    WritePrivateProfileStringW (SECTNAME_RESETVARS,         // Ptr to the section name
                                KEYNAME_QUADIO,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** bResetVars.PP ************************
    wszTemp[0] = L'0' + bResetVars.PP;
    wszTemp[1] = L'\0';

    // Write out bResetVars.PP
    WritePrivateProfileStringW (SECTNAME_RESETVARS,         // Ptr to the section name
                                KEYNAME_QUADPP,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** bResetVars.PW ************************
    wszTemp[0] = L'0' + bResetVars.PW;
    wszTemp[1] = L'\0';

    // Write out bResetVars.PW
    WritePrivateProfileStringW (SECTNAME_RESETVARS,         // Ptr to the section name
                                KEYNAME_QUADPW,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** bResetVars.RL ************************
    wszTemp[0] = L'0' + bResetVars.RL;
    wszTemp[1] = L'\0';

    // Write out bResetVars.RL
    WritePrivateProfileStringW (SECTNAME_RESETVARS,         // Ptr to the section name
                                KEYNAME_QUADRL,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //*********************************************************
    // Write out [Colors] section entries
    //*********************************************************

    //****************** GlbName ******************************
     wsprintfW (wszTemp,
                L"%u, %u, %u",
                gSyntaxColors  [SC_GLBNAME].crFore,
                gSyntaxColors  [SC_GLBNAME].crBack,
                gSyntClrBGTrans[SC_GLBNAME]);
    // Write out GlbName
    WritePrivateProfileStringW (SECTNAME_COLORS,            // Ptr to the section name
                                KEYNAME_SC_GLBNAME,         // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** LclName ******************************
     wsprintfW (wszTemp,
                L"%u, %u, %u",
                gSyntaxColors  [SC_LCLNAME].crFore,
                gSyntaxColors  [SC_LCLNAME].crBack,
                gSyntClrBGTrans[SC_LCLNAME]);
    // Write out LclName
    WritePrivateProfileStringW (SECTNAME_COLORS,            // Ptr to the section name
                                KEYNAME_SC_LCLNAME,         // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** Label ********************************
     wsprintfW (wszTemp,
                L"%u, %u, %u",
                gSyntaxColors  [SC_LABEL].crFore,
                gSyntaxColors  [SC_LABEL].crBack,
                gSyntClrBGTrans[SC_LABEL]);
    // Write out Label
    WritePrivateProfileStringW (SECTNAME_COLORS,            // Ptr to the section name
                                KEYNAME_SC_LABEL,           // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** Prim *********************************
     wsprintfW (wszTemp,
                L"%u, %u, %u",
                gSyntaxColors  [SC_PRIMITIVE].crFore,
                gSyntaxColors  [SC_PRIMITIVE].crBack,
                gSyntClrBGTrans[SC_PRIMITIVE]);
    // Write out Prim
    WritePrivateProfileStringW (SECTNAME_COLORS,            // Ptr to the section name
                                KEYNAME_SC_PRIM,            // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** SysFcn *******************************
     wsprintfW (wszTemp,
                L"%u, %u, %u",
                gSyntaxColors  [SC_SYSFCN].crFore,
                gSyntaxColors  [SC_SYSFCN].crBack,
                gSyntClrBGTrans[SC_SYSFCN]);
    // Write out SysFcn
    WritePrivateProfileStringW (SECTNAME_COLORS,            // Ptr to the section name
                                KEYNAME_SC_SYSFCN,          // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** GlbSysVar ****************************
     wsprintfW (wszTemp,
                L"%u, %u, %u",
                gSyntaxColors  [SC_GLBSYSVAR].crFore,
                gSyntaxColors  [SC_GLBSYSVAR].crBack,
                gSyntClrBGTrans[SC_GLBSYSVAR]);
    // Write out GlbSysVar
    WritePrivateProfileStringW (SECTNAME_COLORS,            // Ptr to the section name
                                KEYNAME_SC_GLBSYSVAR,       // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** LclSysVar ****************************
     wsprintfW (wszTemp,
                L"%u, %u, %u",
                gSyntaxColors  [SC_LCLSYSVAR].crFore,
                gSyntaxColors  [SC_LCLSYSVAR].crBack,
                gSyntClrBGTrans[SC_LCLSYSVAR]);
    // Write out LclSysVar
    WritePrivateProfileStringW (SECTNAME_COLORS,            // Ptr to the section name
                                KEYNAME_SC_LCLSYSVAR,       // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** CtrlStruc ****************************
     wsprintfW (wszTemp,
                L"%u, %u, %u",
                gSyntaxColors  [SC_CTRLSTRUC].crFore,
                gSyntaxColors  [SC_CTRLSTRUC].crBack,
                gSyntClrBGTrans[SC_CTRLSTRUC]);
    // Write out CtrlStruc
    WritePrivateProfileStringW (SECTNAME_COLORS,            // Ptr to the section name
                                KEYNAME_SC_CTRLSTRUC,       // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** NumConst *****************************
     wsprintfW (wszTemp,
                L"%u, %u, %u",
                gSyntaxColors  [SC_NUMCONST].crFore,
                gSyntaxColors  [SC_NUMCONST].crBack,
                gSyntClrBGTrans[SC_NUMCONST]);
    // Write out NumConst
    WritePrivateProfileStringW (SECTNAME_COLORS,            // Ptr to the section name
                                KEYNAME_SC_NUMCONST,        // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** ChrConst *****************************
     wsprintfW (wszTemp,
                L"%u, %u, %u",
                gSyntaxColors  [SC_CHRCONST].crFore,
                gSyntaxColors  [SC_CHRCONST].crBack,
                gSyntClrBGTrans[SC_CHRCONST]);
    // Write out ChrConst
    WritePrivateProfileStringW (SECTNAME_COLORS,            // Ptr to the section name
                                KEYNAME_SC_CHRCONST,        // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** Comment ******************************
     wsprintfW (wszTemp,
                L"%u, %u, %u",
                gSyntaxColors  [SC_COMMENT].crFore,
                gSyntaxColors  [SC_COMMENT].crBack,
                gSyntClrBGTrans[SC_COMMENT]);
    // Write out Comment
    WritePrivateProfileStringW (SECTNAME_COLORS,            // Ptr to the section name
                                KEYNAME_SC_COMMENT,         // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** MatchGrp1 ****************************
     wsprintfW (wszTemp,
                L"%u, %u, %u",
                gSyntaxColors  [SC_MATCHGRP1].crFore,
                gSyntaxColors  [SC_MATCHGRP1].crBack,
                gSyntClrBGTrans[SC_MATCHGRP1]);
    // Write out MatchGrp1
    WritePrivateProfileStringW (SECTNAME_COLORS,            // Ptr to the section name
                                KEYNAME_SC_MATCHGRP1,       // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** MatchGrp2 ****************************
     wsprintfW (wszTemp,
                L"%u, %u, %u",
                gSyntaxColors  [SC_MATCHGRP2].crFore,
                gSyntaxColors  [SC_MATCHGRP2].crBack,
                gSyntClrBGTrans[SC_MATCHGRP2]);
    // Write out MatchGrp2
    WritePrivateProfileStringW (SECTNAME_COLORS,            // Ptr to the section name
                                KEYNAME_SC_MATCHGRP2,       // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** MatchGrp3 ****************************
     wsprintfW (wszTemp,
                L"%u, %u, %u",
                gSyntaxColors  [SC_MATCHGRP3].crFore,
                gSyntaxColors  [SC_MATCHGRP3].crBack,
                gSyntClrBGTrans[SC_MATCHGRP3]);
    // Write out MatchGrp3
    WritePrivateProfileStringW (SECTNAME_COLORS,            // Ptr to the section name
                                KEYNAME_SC_MATCHGRP3,       // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** MatchGrp4 ****************************
     wsprintfW (wszTemp,
                L"%u, %u, %u",
                gSyntaxColors  [SC_MATCHGRP4].crFore,
                gSyntaxColors  [SC_MATCHGRP4].crBack,
                gSyntClrBGTrans[SC_MATCHGRP4]);
    // Write out MatchGrp4
    WritePrivateProfileStringW (SECTNAME_COLORS,            // Ptr to the section name
                                KEYNAME_SC_MATCHGRP4,       // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** UnmatchGrp ***************************
     wsprintfW (wszTemp,
                L"%u, %u, %u",
                gSyntaxColors  [SC_UNMATCHGRP].crFore,
                gSyntaxColors  [SC_UNMATCHGRP].crBack,
                gSyntClrBGTrans[SC_UNMATCHGRP]);
    // Write out UnmatchGrp
    WritePrivateProfileStringW (SECTNAME_COLORS,            // Ptr to the section name
                                KEYNAME_SC_UNMATCHGRP,      // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** Unk **********************************
     wsprintfW (wszTemp,
                L"%u, %u, %u",
                gSyntaxColors  [SC_UNK].crFore,
                gSyntaxColors  [SC_UNK].crBack,
                gSyntClrBGTrans[SC_UNK]);
    // Write out Unk
    WritePrivateProfileStringW (SECTNAME_COLORS,            // Ptr to the section name
                                KEYNAME_SC_UNK,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
} // End SaveIniFile


//***************************************************************************
//  $WritePrivateProfileLogfontW
//
//  Write out a LOGFONTW struc to a .ini file
//***************************************************************************

void WritePrivateProfileLogfontW
    (LPWCHAR    lpwSectName,                // Ptr to the section name
     LPWCHAR    lpwKeyName,                 // Ptr to the key name
     LPLOGFONTW lplfFont,                   // Ptr to LOGFONTW
     LPWCHAR    lpwszIniFile)               // Ptr to file name

{
    WCHAR wszTemp[1024];                            // Temporary storage

    // Format the LOGFONTW struc entries
    wsprintfW (wszTemp,
               FMTSTR_LOGFONT_OUT,
               lplfFont->lfHeight,
               lplfFont->lfWidth,
               lplfFont->lfEscapement,
               lplfFont->lfOrientation,
               lplfFont->lfWeight,
               lplfFont->lfItalic,
               lplfFont->lfUnderline,
               lplfFont->lfStrikeOut,
               lplfFont->lfCharSet,
               lplfFont->lfOutPrecision,
               lplfFont->lfClipPrecision,
               lplfFont->lfQuality,
               lplfFont->lfPitchAndFamily,
              &lplfFont->lfFaceName);
    // Write out the LOGFONTW struc
    WritePrivateProfileStringW (lpwSectName,        // Ptr to the section name
                                lpwKeyName,         // Ptr to the key name
                                wszTemp,            // Ptr to the key value
                                lpwszIniFile);      // Ptr to the file name
} // End WritePrivateProfileLogfontW


//***************************************************************************
//  $WritePrivateProfileGlbCharW
//
//  Write out a global char string to a .ini file
//***************************************************************************

void WritePrivateProfileGlbCharW
    (LPWCHAR lpwSectName,                   // Ptr to the section name
     LPWCHAR lpwKeyName,                    // Ptr to the key name
     HGLOBAL hGlbObj,                       // Object global memory handle
     LPWCHAR lpwszIniFile)                  // Ptr to file name

{
    LPAPLCHAR lpMemObj,                     // Ptr to object global memory
              lpaplChar;                    // Ptr to running output
    APLNELM   aplNELMObj;                   // Object NELM

    // Lock the memory to get a ptr to it
    lpMemObj = MyGlobalLock (hGlbObj);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemObj)
    // Get the NELM
    aplNELMObj = lpHeader->NELM;
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMemObj = VarArrayBaseToData (lpMemObj, 1);

    // Get starting ptr
    lpaplChar = lpwszGlbTemp;

    // Leading single quote
    *lpaplChar++ = L'\'';

    // Format the text as an ASCII string with non-ASCII chars
    //   represented as either {symbol} or {\xXXXX} where XXXX is
    //   a four-digit hex number.
    lpaplChar +=
      ConvertWideToNameLength (lpaplChar,       // Ptr to output save buffer
                               lpMemObj,        // Ptr to incoming chars
                    (APLU3264) aplNELMObj);     // # chars to convert
    // Trailing single quote
    *lpaplChar++ = L'\'';

    // Ensure properly terminated
    *lpaplChar   = L'\0';

    // Write out the global char vector
    WritePrivateProfileStringW (lpwSectName,        // Ptr to the section name
                                lpwKeyName,         // Ptr to the key name
                                lpwszGlbTemp,       // Ptr to the key value
                                lpwszIniFile);      // Ptr to the file name
    // We no longer need this ptr
    MyGlobalUnlock (hGlbObj); lpMemObj = NULL;
} // End WritePrivateProfileGlbCharW


//***************************************************************************
//  End of File: inifile.c
//***************************************************************************
