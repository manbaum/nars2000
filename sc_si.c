//***************************************************************************
//  NARS2000 -- System Command:  )SI
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
#include <time.h>

#include "main.h"
#include "resdebug.h"
#include "externs.h"
#include "aplerrors.h"
#include "pertab.h"
#include "dfnhdr.h"
#include "sis.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $CmdSi_EM
//
//  Execute the system command:  )SI [n]
//***************************************************************************

BOOL CmdSi_EM
    (LPWCHAR lpwszTail)         // Ptr to command line tail

{
    return CmdSiSinlCom_EM (lpwszTail, FALSE, NULL);
} // End CmdSi_EM


//***************************************************************************
//  $CmdSinl_EM
//
//  Execute the system command:  )SINL [n]
//***************************************************************************

BOOL CmdSinl_EM
    (LPWCHAR lpwszTail)         // Ptr to command line tail

{
    return CmdSiSinlCom_EM (lpwszTail, TRUE, NULL);
} // End CmdSinl_EM


//***************************************************************************
//  $CmdSiSinlCom_EM
//
//  Execute the system command:  )SI or )SINL
//***************************************************************************

BOOL CmdSiSinlCom_EM
    (LPWCHAR   lpwszTail,       // Ptr to command line tail
     BOOL      bSINL,           // TRUE iff )SINL
     LPAPLCHAR lpMemSaveWSID)   // Ptr to the file name (NULL = not called from CmdSave_EM)

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    LPSIS_HEADER lpSISCur;      // Ptr to current SIS_HEADER srtuc
    UINT         SILevel;       // SI level (for CmdSave_EM)
    APLCHAR      szSILevel[10]; // Formatted SI level

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // ***FIXME*** -- Make sensitive to [n] in lpwszTail

#ifdef DEBUG
    if (lpMemPTD->lpSISCur EQ NULL && lpMemSaveWSID EQ NULL)
       AppendLine (WS_UTF16_ZILDE, FALSE, TRUE);
    else
#endif
    // Loop backwards through the SI levels
    for (lpSISCur = lpMemPTD->lpSISCur, SILevel = 0;
         lpSISCur;
         lpSISCur = lpSISCur->lpSISPrv, SILevel++)
    {
        LPAPLCHAR lpMemName;            // Ptr to function name global memory

        // Format the SI level
        wsprintfW (szSILevel,
                   L"%d",
                   SILevel);
        // Split cases based upon the caller's function type
        switch (lpSISCur->DfnType)
        {
            case DFNTYPE_IMM:
                // If it's not CmsSave_EM, ...
                if (lpMemSaveWSID EQ NULL)
                {
#ifdef DEBUG
                    AppendLine (WS_UTF16_IOTA, FALSE, TRUE);
#endif
                } else
                    WritePrivateProfileStringW (L"SI",          // Ptr to the section name
                                                szSILevel,      // Ptr to the key name
                                                L"{iota}",      // Ptr to the key value
                                                lpMemSaveWSID); // Ptr to the file name
                break;

            case DFNTYPE_OP1:
            case DFNTYPE_OP2:
            case DFNTYPE_FCN:
                // Lock the memory to get a ptr to it
                lpMemName = MyGlobalLock (lpSISCur->hGlbFcnName);

                // If it's CmsSave_EM, ...
                if (lpMemSaveWSID)
                    // Format the text as an ASCII string with non-ASCII chars
                    //   represented as either {symbol} or {\xXXXX} where XXXX is
                    //   a four-digit hex number.
                    ConvertWideToName (lpwszFormat, lpMemName);

                // Format the Name, Line #, and Suspension marker
                wsprintfW (lpwszTemp,
                           L"%s[%d] %c",
                           (lpMemSaveWSID EQ NULL) ? lpMemName : lpwszFormat,
                           lpSISCur->CurLineNum,
                           " *"[lpSISCur->Suspended]);
                // We no longer need this ptr
                MyGlobalUnlock (lpSISCur->hGlbFcnName); lpMemName = NULL;

                // If it's not CmsSave_EM, ...
                if (lpMemSaveWSID EQ NULL)
                    // Display the function name & line #
                    AppendLine (lpwszTemp, FALSE, !bSINL);
                else
                    WritePrivateProfileStringW (L"SI",          // Ptr to the section name
                                                szSILevel,      // Ptr to the key name
                                                lpwszTemp,      // Ptr to the key value
                                                lpMemSaveWSID); // Ptr to the file name
                // If it's )SINL, display the namelist
                if (bSINL)
                {
                    UINT       numSymEntries,   // # LPSYMENTRYs localized on the stack
                               numSym,          // Loop counter
                               uLen;            // Loop counter
                    LPSYMENTRY lpSymEntryNxt;   // Ptr to next localized LPSYMENTRY on the SIS
                    APLUINT    uQuadPW,         // []PW
                               uLineInd,        // Line indent
                               uLineLen,        // Current line length
                               uNameLen;        // Name length
                    LPAPLCHAR  lpw;             // Ptr to end of name

                    // Get the current value of []PW
                    uQuadPW = GetQuadPW ();

                    // Initialize the line indent and current line length
                    uLineLen = uLineInd = lstrlenW (lpwszTemp);

                    // Get # LPSYMENTRYs on the stack
                    numSymEntries = lpSISCur->numSymEntries;

                    // Point to the localized LPSYMENTRYs
                    lpSymEntryNxt = (LPSYMENTRY) ByteAddr (lpSISCur, sizeof (SIS_HEADER));

                    // Loop through the # LPSYMENTRYs
                    for (numSym = 0; numSym < numSymEntries; numSym++)
                    {
                        // Copy the STE name to local storage
                        lpw = CopySteName (lpwszTemp, &lpSymEntryNxt[numSym], &uNameLen);

                        // Ensure properly terminated
                        *lpw = L'\0';

                        // Get the name length plus two leading blanks
                        uNameLen += 2;

                        // If the name's length would push us over the
                        //   page width, go to a new line
                        if (uQuadPW < (uLineLen + uNameLen))
                        {
                            AppendLine (L"", TRUE, TRUE);
                            for (uLen = 0; uLen < uLineInd; uLen++)
                                AppendLine (L" ", TRUE, FALSE);
                            uLineLen = uLineInd;
                        } // End IF

                        // Display the name preceded by two blanks
                        AppendLine (L"  ", FALSE, FALSE);
                        AppendLine (lpwszTemp, FALSE, FALSE);
                        uLineLen += uNameLen;
                    } // End FOR

                    // End the last line
                    AppendLine (L"", TRUE, TRUE);
                } // End IF

                break;

            case DFNTYPE_EXEC:
                // If it's not CmsSave_EM, ...
                if (lpMemSaveWSID EQ NULL)
                    AppendLine (WS_UTF16_UPTACKJOT, FALSE, TRUE);
                else
                    WritePrivateProfileStringW (L"SI",              // Ptr to the section name
                                                szSILevel,          // Ptr to the key name
                                                L"{uptackjot}",     // Ptr to the key value
                                                lpMemSaveWSID);     // Ptr to the file name
                break;

            case DFNTYPE_QUAD:
                // If it's not CmsSave_EM, ...
                if (lpMemSaveWSID EQ NULL)
                    AppendLine (WS_UTF16_QUAD, FALSE, TRUE);
                else
                    WritePrivateProfileStringW (L"SI",              // Ptr to the section name
                                                szSILevel,          // Ptr to the key name
                                                L"{quad}",          // Ptr to the key value
                                                lpMemSaveWSID);     // Ptr to the file name
                break;

            case DFNTYPE_UNK:
            defstop
                break;
        } // End SWITCH
    } // End IF/FOR

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return TRUE;
} // End CmdSiSinlCom_EM


//***************************************************************************
//  End of File: sc_si.c
//***************************************************************************
