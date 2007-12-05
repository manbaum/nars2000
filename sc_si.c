//***************************************************************************
//  NARS2000 -- System Command:  )SI
//***************************************************************************

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
    return CmdSiSinlCom_EM (lpwszTail, FALSE);
} // End CmdSi_EM


//***************************************************************************
//  $CmdSinl_EM
//
//  Execute the system command:  )SINL [n]
//***************************************************************************

BOOL CmdSinl_EM
    (LPWCHAR lpwszTail)         // Ptr to command line tail

{
    return CmdSiSinlCom_EM (lpwszTail, TRUE);
} // End CmdSinl_EM


//***************************************************************************
//  $CmdSiSinlCom_EM
//
//  Execute the system command:  )SI or )SINL
//***************************************************************************

BOOL CmdSiSinlCom_EM
    (LPWCHAR lpwszTail,         // Ptr to command line tail
     BOOL    bSINL)             // TRUE iff )SINL

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    LPSIS_HEADER lpSISCur;      // Ptr to current SIS_HEADER srtuc

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // ***FIXME*** -- Make sensitive to [n] in lpwszTail

#ifdef DEBUG
    if (lpMemPTD->lpSISCur EQ NULL)
       AppendLine (WS_UTF16_ZILDE, FALSE, TRUE);
    else
#endif
    // Loop backwards through the SI levels
    for (lpSISCur = lpMemPTD->lpSISCur;
         lpSISCur;
         lpSISCur = lpSISCur->lpSISPrv)
    {
        LPAPLCHAR lpMemName;            // Ptr to function name global memory

        // Split cases based upon the caller's function type
        switch (lpSISCur->DfnType)
        {
            case DFNTYPE_IMM:
#ifdef DEBUG
                AppendLine (WS_UTF16_EPSILON, FALSE, TRUE);
#endif
                break;

            case DFNTYPE_OP1:
            case DFNTYPE_OP2:
            case DFNTYPE_FCN:
                // Lock the memory to get a ptr to it
                lpMemName = MyGlobalLock (lpSISCur->hGlbFcnName);

                // Format the Name, Line #, and Suspension marker
                wsprintfW (lpwszTemp,
                           L"%s[%d] %c",
                           lpMemName,
                           lpSISCur->CurLineNum,
                           " *"[lpSISCur->Suspended]);
                // We no longer need this ptr
                MyGlobalUnlock (lpSISCur->hGlbFcnName); lpMemName = NULL;

                // Display the function name & line #
                AppendLine (lpwszTemp, FALSE, !bSINL);

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
                        lpw = CopySteName (lpwszTemp, &lpSymEntryNxt[numSym]);

                        // Ensure properly terminated
                        *lpw = L'\0';

                        // Get the name length plus two leading blanks
                        uNameLen = 2 + lstrlenW (lpwszTemp);

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
                AppendLine (WS_UTF16_UPTACKJOT, FALSE, TRUE);

                break;

            case DFNTYPE_QUAD:
                AppendLine (WS_UTF16_QUAD, FALSE, TRUE);

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
