//***************************************************************************
//  NARS2000 -- INI File Routines
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2010 Sudley Place Software

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
#include "headers.h"


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
#define KEYNAME_SYNTCLRPRNT             L"SyntClrPrnt"
#define KEYNAME_CHECKGROUP              L"CheckGroup"
#define KEYNAME_INSSTATE                L"InsState"
#define KEYNAME_REVDBLCLK               L"RevDblClk"
#define KEYNAME_VIEWSTATUSBAR           L"ViewStatusBar"
#define KEYNAME_DEFDISPFCNLINENUMS      L"DefDispFcnLineNums"

#define KEYNAME_SC_GLBNAME              L"GlbName"
#define KEYNAME_SC_LCLNAME              L"LclName"
#define KEYNAME_SC_LABEL                L"Label"
#define KEYNAME_SC_PRIMFCN              L"PrimFcn"
#define KEYNAME_SC_PRIMOP1              L"PrimOp1"
#define KEYNAME_SC_PRIMOP2              L"PrimOp2"
#define KEYNAME_SC_SYSFCN               L"SysFcn"
#define KEYNAME_SC_GLBSYSVAR            L"GlbSysVar"
#define KEYNAME_SC_LCLSYSVAR            L"LclSysVar"
#define KEYNAME_SC_CTRLSTRUC            L"CtrlStruc"
#define KEYNAME_SC_NUMCONST             L"NumConst"
#define KEYNAME_SC_CHRCONST             L"ChrConst"
#define KEYNAME_SC_COMMENT              L"Comment"
#define KEYNAME_SC_LINEDRAWING          L"LineDrawing"
#define KEYNAME_SC_FCNLINENUMS          L"FcnLineNums"
#define KEYNAME_SC_MATCHGRP1            L"MatchGrp1"
#define KEYNAME_SC_MATCHGRP2            L"MatchGrp2"
#define KEYNAME_SC_MATCHGRP3            L"MatchGrp3"
#define KEYNAME_SC_MATCHGRP4            L"MatchGrp4"
#define KEYNAME_SC_UNMATCHGRP           L"UnmatchGrp"
#define KEYNAME_SC_UNNESTED             L"UnnestedGrp"
#define KEYNAME_SC_UNK                  L"Unk"
#define KEYNAME_SC_WINTEXT              L"WinText"

#define KEYNAME_CUSTOMCOLORS            L"CustomColors"

// Format string for [Fonts] section LOGFONTW
#define FMTSTR_LOGFONT_INP      L"%d %d %d %d %d %d %d %d %d %d %d %d %d '%s'"
#define FMTSTR_LOGFONT_OUT      L"%d %d %d %d %d %d %d %d %d %d %d %d %d '%s'"

// Format string for [Colors] section
#define FMTSTR_SYNTAXCOLOR      L"%u, %u, %u"
#define FMTSTR_CUSTOMCOLORS     L"%u, %u, %u, %u, " \
                                L"%u, %u, %u, %u, " \
                                L"%u, %u, %u, %u, " \
                                L"%u, %u, %u, %u"

// Array of keynames for use in [Colors] section
LPWCHAR aColorKeyNames[] =
{KEYNAME_SC_GLBNAME     ,       // 00:  Global Name
 KEYNAME_SC_LCLNAME     ,       // 01:  Local  ...
 KEYNAME_SC_LABEL       ,       // 02:  Label
 KEYNAME_SC_PRIMFCN     ,       // 03:  Primitive Function
 KEYNAME_SC_PRIMOP1     ,       // 04:  Primitive Monadic Operator
 KEYNAME_SC_PRIMOP2     ,       // 05:  Primitive Dyadic Operator
 KEYNAME_SC_SYSFCN      ,       // 06:  System Function
 KEYNAME_SC_GLBSYSVAR   ,       // 07:  Global System Variable
 KEYNAME_SC_LCLSYSVAR   ,       // 08:  Local  ...
 KEYNAME_SC_CTRLSTRUC   ,       // 09:  Control Structure
 KEYNAME_SC_NUMCONST    ,       // 0A:  Numeric constant
 KEYNAME_SC_CHRCONST    ,       // 0B:  Character constant
 KEYNAME_SC_COMMENT     ,       // 0C:  Comment
 KEYNAME_SC_LINEDRAWING ,       // 0D:  Line drawing chars
 KEYNAME_SC_FCNLINENUMS ,       // 0E:  Function line numbers
 KEYNAME_SC_MATCHGRP1   ,       // 0F:  Matched Grouping Symbols [] () {}
 KEYNAME_SC_MATCHGRP2   ,       // 10:  Matched Grouping Symbols [] () {}
 KEYNAME_SC_MATCHGRP3   ,       // 11:  Matched Grouping Symbols [] () {}
 KEYNAME_SC_MATCHGRP4   ,       // 12:  Matched Grouping Symbols [] () {}
 KEYNAME_SC_UNMATCHGRP  ,       // 13:  Unmatched Grouping Symbols [] () {} ' "
 KEYNAME_SC_UNNESTED    ,       // 14:  Improperly Nested Grouping Symbols [] () {}
 KEYNAME_SC_UNK         ,       // 15:  Unknown symbol
 KEYNAME_SC_WINTEXT     ,       // 16:  Window text
};


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

#define TEMPBUFLEN      countof (wszTemp)
#define WS_APPDATA          L"APPDATA"

    // If we didn't find it, ...
    if (0 EQ GetEnvironmentVariableW (WS_APPDATA, wszAppData, TEMPBUFLEN))
    {
        MBW (L"Unable to find " WS_APPDATA L" in the environment.");

        return FALSE;
    } // End IF

#undef  WS_APPDATA

    // Append the "\\NARS2000" part
    lstrcatW (wszAppData, WS_SLOPE WS_APPNAME);

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
    lstrcpyW (&wszAppData[cchSize], WS_SLOPE WS_APPNAME L".ini");
    lpwszIniFile = wszAppData;

    // Get ptr to where the workspaces dir will be stored
    // The "+ 1" is to skip over the terminating zero
    lpwszWorkDir = &wszAppData[lstrlenW (lpwszIniFile) + 1];

    // Append the AppData directory part so we can form the workspaces dir
    // The "+ 1" is because lstrcpynW includes a terminating zero in the count
    lstrcpynW (lpwszWorkDir, lpwszIniFile, cchSize + 1);

    // Append the workspaces name
    lstrcatW (lpwszWorkDir, WS_SLOPE WS_WKSNAME);

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
    lstrcatW (lpwszWorkDir, WS_SLOPE);

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

#define TEMPBUFLEN      countof (wszTemp)

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
        // Overcome a bug in which the saved glbSameFontAs value may be
        //   larger than the corresponding index
        glbSameFontAs[uCnt] = min (glbSameFontAs[uCnt], (int) uCnt);
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
    // Read in bSyntClrPrnt
    OptionFlags.bSyntClrPrnt =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_SYNTCLRPRNT,   // Ptr to the key name
                             DEF_SYNTCLRPRNT,       // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bCheckGroup
    OptionFlags.bCheckGroup =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_CHECKGROUP,    // Ptr to the key name
                             DEF_CHECKGROUP,        // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bInsState
    OptionFlags.bInsState =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_INSSTATE,      // Ptr to the key name
                             DEF_INSSTATE,          // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bRevDblClk
    OptionFlags.bRevDblClk =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_REVDBLCLK,     // Ptr to the key name
                             DEF_REVDBLCLK,         // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bViewStatusBar
    OptionFlags.bViewStatusBar =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_VIEWSTATUSBAR, // Ptr to the key name
                             DEF_VIEWSTATUSBAR,     // Default value if not found
                             lpwszIniFile);         // Ptr to the file name
    // Read in bDefDispFcnLineNums
    OptionFlags.bDefDispFcnLineNums =
      GetPrivateProfileIntW (SECTNAME_OPTIONS,      // Ptr to the section name
                             KEYNAME_DEFDISPFCNLINENUMS, // Ptr to the key name
                             DEF_DISPFCNLINENUMS,   // Default value if not found
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
                              TEMPBUFLEN,           // Count of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // Convert any {name}s to symbols
    ConvertNameInPlace (wszTemp);

    // If the value is empty (''), ...
    if (wszTemp[0] EQ WC_EOS)
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

    Assert (SC_LENGTH EQ countof (aColorKeyNames));

    // Loop through the color names
    for (uCnt = 0; uCnt < SC_LENGTH; uCnt++)
    {
        // Read in the string
        GetPrivateProfileStringW (SECTNAME_COLORS,      // Ptr to the section name
                                  aColorKeyNames[uCnt], // Ptr to the key name
                                  L"",                  // Ptr to the default value
                                  wszTemp,              // Ptr to the output buffer
                                  TEMPBUFLEN,           // Count of the output buffer
                                  lpwszIniFile);        // Ptr to the file name
        // Check for value
        if (wszTemp[0] NE WC_EOS)
            // Convert the numbers
            swscanf (wszTemp,
                     FMTSTR_SYNTAXCOLOR,
                    &gSyntaxColorName[uCnt].syntClr.crFore,
                    &gSyntaxColorName[uCnt].syntClr.crBack,
                    &gSyntClrBGTrans [uCnt]);
    } // End FOR

    // Respecify the Window Background brush
    //   because the Window Background color has changed
    RedoWinBG ();

    // Read in the CustomColors
    GetPrivateProfileStringW (SECTNAME_COLORS,      // Ptr to the section name
                              KEYNAME_CUSTOMCOLORS, // Ptr to the key name
                              L"",                  // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              TEMPBUFLEN,           // Count of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // Check for value
    if (wszTemp[0] NE WC_EOS)
        // Convert the numbers
        swscanf (wszTemp,
                 FMTSTR_CUSTOMCOLORS,
                &aCustomColors[ 0], &aCustomColors[ 1], &aCustomColors[ 2], &aCustomColors[ 3],
                &aCustomColors[ 4], &aCustomColors[ 5], &aCustomColors[ 6], &aCustomColors[ 7],
                &aCustomColors[ 8], &aCustomColors[ 9], &aCustomColors[10], &aCustomColors[11],
                &aCustomColors[12], &aCustomColors[13], &aCustomColors[14], &aCustomColors[15]);
#undef  TEMPBUFLEN
} // End ReadIniFileGlb


//***************************************************************************
//  $RedoWinBG
//
//  Respecify the Window Background brush
//    because the Window Background color has changed
//***************************************************************************

void RedoWinBG
    (void)

{
    UINT uCnt;                      // Loop counter

    // Delete the window background brush
    if (ghBrushBG)
    {
        DeleteObject (ghBrushBG); ghBrushBG = NULL;
    } // End IF

    // Initialize the window background brush
    ghBrushBG = CreateSolidBrush (gSyntaxColorText.crBack);

    // Loop through the Syntax Colors
    for (uCnt = 0; uCnt < SC_LENGTH; uCnt++)
    // If the background is transparent, change it
    if (gSyntClrBGTrans[uCnt])
        gSyntaxColorName[uCnt].syntClr.crBack = gSyntaxColorText.crBack;
} // End RedoWinBG


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
                              countof (wszTemp) - 1,// Count of the output buffer
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
        lstrcpyW (lplfFont->lfFaceName, strchrW (wszTemp, WC_SQ) + 1);
        *(strchrW (lplfFont->lfFaceName, WC_SQ)) = WC_EOS;
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
                              countof (wszTemp),    // Count of the output buffer
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
    HGLOBAL hGlbRes,                                // Result global memory handle
            hGlbChk;                                // Result from CheckGlobals
    LPVOID  lpMemRes,                               // Ptr to result global memory
            lpMemInp;                               // Ptr to input global memory

    GetPrivateProfileStringW (lpwSectName,          // Ptr to the section name
                              lpwKeyName,           // Ptr to the key name
                              L"\x0001",            // Ptr to the default value
                              wszTemp,              // Ptr to the output buffer
                              countof (wszTemp),    // Count of the output buffer
                              lpwszIniFile);        // Ptr to the file name
    // If the new value is present, ...
    if (wszTemp[0] NE L'\x0001')
    {
        // Catch short default values of []IC where we inserted a
        //   new value into the middle of the integer vector
        if (lstrcmpW (lpwKeyName, KEYNAME_QUADIC) EQ 0
         && (lstrcmpW (wszTemp, L"3 4 2 2 2 1 2 2 1 2 2 2 1")   EQ 0
          || lstrcmpW (wszTemp, L"3 4 2 2 2 1 2 2 1 2 2 2 2 1") EQ 0))
            // Use the new default value
            lstrcpyW (wszTemp, L"3 4 2 2 2 1 2 2 2 2 1 2 2 2 2 1");
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

    // Check to see if this value duplicates one of the already
    //   allocated permanent globals
    hGlbChk = CheckGlobals (hGlbRes);
    if (hGlbChk)
    {
        // We no longer need this storage
        FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;

        // Copy the global memory handle
        hGlbRes = CopySymGlbDirAsGlb (hGlbChk);

        // Clear the ptr type bits
        hGlbRes = ClrPtrTypeDir (hGlbRes);
    } // End IF

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

#define ZEROSTR     L"{\\x0000}"

                    // If there's a matching UTF16_xxx equivalent,
                    //   or the hex value is zero
                    if (wcTmp || strncmpW (wszTemp, ZEROSTR, strcountof (ZEROSTR)) EQ 0)
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
    wszTemp[1] = WC_EOS;

    // Write out bAdjustPw
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_ADJUSTPW,           // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bUnderbarToLowercase ****************
    wszTemp[0] = L'0' + OptionFlags.bUnderbarToLowercase;
    wszTemp[1] = WC_EOS;

    // Write out bUnderbarToLowercase
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_UNDERBARTOLOWERCASE,// Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bNewTabOnClear **********************
    wszTemp[0] = L'0' + OptionFlags.bNewTabOnClear;
    wszTemp[1] = WC_EOS;

    // Write out bNewTabOnClear
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_NEWTABONCLEAR,      // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bNewTabOnLoad ***********************
    wszTemp[0] = L'0' + OptionFlags.bNewTabOnLoad;
    wszTemp[1] = WC_EOS;

    // Write out bNewTabOnLoad
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_NEWTABONLOAD,       // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bUseLocalTime ***********************
    wszTemp[0] = L'0' + OptionFlags.bUseLocalTime;
    wszTemp[1] = WC_EOS;

    // Write out bUseLocalTime
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_USELOCALTIME,       // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bBackupOnLoad ***********************
    wszTemp[0] = L'0' + OptionFlags.bBackupOnLoad;
    wszTemp[1] = WC_EOS;

    // Write out bBackupOnLoad
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_BACKUPONLOAD,       // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bBackupOnSave ***********************
    wszTemp[0] = L'0' + OptionFlags.bBackupOnSave;
    wszTemp[1] = WC_EOS;

    // Write out bBackupOnSave
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_BACKUPONSAVE,       // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bNoCopyrightMsg *********************
    wszTemp[0] = L'0' + OptionFlags.bNoCopyrightMsg;
    wszTemp[1] = WC_EOS;

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
    wszTemp[1] = WC_EOS;

    // Write out bSyntClrFcns
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_SYNTCLRFCNS,        // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bSyntClrSess ************************
    wszTemp[0] = L'0' + OptionFlags.bSyntClrSess;
    wszTemp[1] = WC_EOS;

    // Write out bSyntClrSess
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_SYNTCLRSESS,        // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bSyntClrPrnt ************************
    wszTemp[0] = L'0' + OptionFlags.bSyntClrPrnt;
    wszTemp[1] = WC_EOS;

    // Write out bSyntClrPrnt
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_SYNTCLRPRNT,        // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bCheckGroup *************************
    wszTemp[0] = L'0' + OptionFlags.bCheckGroup;
    wszTemp[1] = WC_EOS;

    // Write out bCheckGroup
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_CHECKGROUP,         // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bInsState ***************************
    wszTemp[0] = L'0' + OptionFlags.bInsState;
    wszTemp[1] = WC_EOS;

    // Write out bInsState
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_INSSTATE,           // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name

    //******************* bRevDblClk **************************
    wszTemp[0] = L'0' + OptionFlags.bRevDblClk;
    wszTemp[1] = WC_EOS;

    // Write out bRevDblClk
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_REVDBLCLK,          // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name

    //******************* bViewStatusBar **********************
    wszTemp[0] = L'0' + OptionFlags.bViewStatusBar;
    wszTemp[1] = WC_EOS;

    // Write out bViewStatusBar
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_VIEWSTATUSBAR,      // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name

    //******************* bDefDispFcnLineNums *****************
    wszTemp[0] = L'0' + OptionFlags.bDefDispFcnLineNums;
    wszTemp[1] = WC_EOS;

    // Write out bViewStatusBar
    WritePrivateProfileStringW (SECTNAME_OPTIONS,           // Ptr to the section name
                                KEYNAME_DEFDISPFCNLINENUMS, // Ptr to the key name
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
                    FLTDISPFMT_RAWFLT);                     // Float display format
    // Zap the trailing blank
    lpaplChar[-1] = WC_EOS;

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
    lpaplChar[-1] = WC_EOS;

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
    lpaplChar[-1] = WC_EOS;

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
    lpaplChar[-1] = WC_EOS;

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
    lpaplChar[-1] = WC_EOS;

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
        wszTemp[1] = WC_SQ;
        wszTemp[2] = WC_EOS;
    } else
    {
        UINT uLen;

        wszTemp[0] = WC_SQ;
        uLen = (UINT)
          ConvertWideToNameLength (&wszTemp[1], &cQuadPR_CWS, 1);
        wszTemp[uLen + 1] = WC_SQ;
        wszTemp[uLen + 2] = WC_EOS;
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
    lpaplChar[-1] = WC_EOS;

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
    lpaplChar[-1] = WC_EOS;

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
    wszTemp[1] = WC_EOS;

    // Write out bRangeLimit.CT
    WritePrivateProfileStringW (SECTNAME_RANGELIMITS,       // Ptr to the section name
                                KEYNAME_QUADCT,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bRangeLimit.IC **********************
    wszTemp[0] = L'0' + bRangeLimit.IC;
    wszTemp[1] = WC_EOS;

    // Write out bRangeLimit.IC
    WritePrivateProfileStringW (SECTNAME_RANGELIMITS,       // Ptr to the section name
                                KEYNAME_QUADIC,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bRangeLimit.IO **********************
    wszTemp[0] = L'0' + bRangeLimit.IO;
    wszTemp[1] = WC_EOS;

    // Write out bRangeLimit.IO
    WritePrivateProfileStringW (SECTNAME_RANGELIMITS,       // Ptr to the section name
                                KEYNAME_QUADIO,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bRangeLimit.PP **********************
    wszTemp[0] = L'0' + bRangeLimit.PP;
    wszTemp[1] = WC_EOS;

    // Write out bRangeLimit.PP
    WritePrivateProfileStringW (SECTNAME_RANGELIMITS,       // Ptr to the section name
                                KEYNAME_QUADPP,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bRangeLimit.PW **********************
    wszTemp[0] = L'0' + bRangeLimit.PW;
    wszTemp[1] = WC_EOS;

    // Write out bRangeLimit.PW
    WritePrivateProfileStringW (SECTNAME_RANGELIMITS,       // Ptr to the section name
                                KEYNAME_QUADPW,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //******************* bRangeLimit.RL **********************
    wszTemp[0] = L'0' + bRangeLimit.RL;
    wszTemp[1] = WC_EOS;

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
    wszTemp[1] = WC_EOS;

    // Write out bResetVars.CT
    WritePrivateProfileStringW (SECTNAME_RESETVARS,         // Ptr to the section name
                                KEYNAME_QUADCT,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** bResetVars.FC ************************
    wszTemp[0] = L'0' + bResetVars.FC;
    wszTemp[1] = WC_EOS;

    // Write out bResetVars.FC
    WritePrivateProfileStringW (SECTNAME_RESETVARS,         // Ptr to the section name
                                KEYNAME_QUADFC,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** bResetVars.IC ************************
    wszTemp[0] = L'0' + bResetVars.IC;
    wszTemp[1] = WC_EOS;

    // Write out bResetVars.IC
    WritePrivateProfileStringW (SECTNAME_RESETVARS,         // Ptr to the section name
                                KEYNAME_QUADIC,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** bResetVars.IO ************************
    wszTemp[0] = L'0' + bResetVars.IO;
    wszTemp[1] = WC_EOS;

    // Write out bResetVars.IO
    WritePrivateProfileStringW (SECTNAME_RESETVARS,         // Ptr to the section name
                                KEYNAME_QUADIO,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** bResetVars.PP ************************
    wszTemp[0] = L'0' + bResetVars.PP;
    wszTemp[1] = WC_EOS;

    // Write out bResetVars.PP
    WritePrivateProfileStringW (SECTNAME_RESETVARS,         // Ptr to the section name
                                KEYNAME_QUADPP,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** bResetVars.PW ************************
    wszTemp[0] = L'0' + bResetVars.PW;
    wszTemp[1] = WC_EOS;

    // Write out bResetVars.PW
    WritePrivateProfileStringW (SECTNAME_RESETVARS,         // Ptr to the section name
                                KEYNAME_QUADPW,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //****************** bResetVars.RL ************************
    wszTemp[0] = L'0' + bResetVars.RL;
    wszTemp[1] = WC_EOS;

    // Write out bResetVars.RL
    WritePrivateProfileStringW (SECTNAME_RESETVARS,         // Ptr to the section name
                                KEYNAME_QUADRL,             // Ptr to the key name
                                wszTemp,                    // Ptr to the key value
                                lpwszIniFile);              // Ptr to the file name
    //*********************************************************
    // Write out [Colors] section entries
    //*********************************************************

    // Loop through the color names
    for (uCnt = 0; uCnt < SC_LENGTH; uCnt++)
    {
         wsprintfW (wszTemp,
                    FMTSTR_SYNTAXCOLOR,
                    gSyntaxColorName[uCnt].syntClr.crFore,
                    gSyntaxColorName[uCnt].syntClr.crBack,
                    gSyntClrBGTrans [uCnt]);
        // Write out the entry
        WritePrivateProfileStringW (SECTNAME_COLORS,            // Ptr to the section name
                                    aColorKeyNames[uCnt],       // Ptr to the key name
                                    wszTemp,                    // Ptr to the key value
                                    lpwszIniFile);              // Ptr to the file name
    } // End FOR

    // Write out the CustomColors
     wsprintfW (wszTemp,
                FMTSTR_CUSTOMCOLORS,
                aCustomColors[ 0], aCustomColors[ 1], aCustomColors[ 2], aCustomColors[ 3],
                aCustomColors[ 4], aCustomColors[ 5], aCustomColors[ 6], aCustomColors[ 7],
                aCustomColors[ 8], aCustomColors[ 9], aCustomColors[10], aCustomColors[11],
                aCustomColors[12], aCustomColors[13], aCustomColors[14], aCustomColors[15]);
    // Write out the entry
    WritePrivateProfileStringW (SECTNAME_COLORS,            // Ptr to the section name
                                KEYNAME_CUSTOMCOLORS,       // Ptr to the key name
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
    *lpaplChar++ = WC_SQ;

    // Format the text as an ASCII string with non-ASCII chars
    //   represented as either {symbol} or {\xXXXX} where XXXX is
    //   a four-digit hex number.
    lpaplChar +=
      ConvertWideToNameLength (lpaplChar,       // Ptr to output save buffer
                               lpMemObj,        // Ptr to incoming chars
                    (APLU3264) aplNELMObj);     // # chars to convert
    // Trailing single quote
    *lpaplChar++ = WC_SQ;

    // Ensure properly terminated
    *lpaplChar   = WC_EOS;

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
