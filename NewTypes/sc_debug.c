//***************************************************************************
//  NARS2000 -- System Command:  )DEBUG
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2018 Sudley Place Software

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
#include "headers.h"

extern UBOOL bDebugPLTrace,
             bDebugPLStart;


//***************************************************************************
//  $CmdDebug_EM
//
//  Execute the system command:  )DEBUG [PLTrace=[0|1]] [PLStart=[0|1]]
//***************************************************************************

UBOOL CmdDebug_EM
    (LPWCHAR lpwszTail)         // Ptr to command line tail

{
    LPWCHAR p = lpwszTail;
    WCHAR   wszTemp[1024];

    // Skip over leading white space
    p = SkipWhiteW (p);

    // Check for inquiry
    if (*p EQ WC_EOS)
    {
        MySprintfW (wszTemp,
                    sizeof (wszTemp),
                   L"Is PLTrace = %d  PLStart = %d",
                    bDebugPLTrace,
                    bDebugPLStart);
        // Tell the user about it
        AppendLine (wszTemp, FALSE, TRUE);
    } // End IF

    // Check for alloff
#define OPT     L"alloff"
    if (strncmpiW (p, OPT, strcountof (OPT)) EQ 0)
    {
        // Skip over the prefix
        p += strcountof (OPT);

        MySprintfW (wszTemp,
                    sizeof (wszTemp),
                   L"Was PLTrace = %d  PLStart = %d",
                    bDebugPLTrace,
                    bDebugPLStart);
        // Tell the user about it
        AppendLine (wszTemp, FALSE, TRUE);

        bDebugPLTrace =
        bDebugPLStart = FALSE;
    } // End IF
#undef  OPT

    // Check for allon
#define OPT     L"allon"
    if (strncmpiW (p, OPT, strcountof (OPT)) EQ 0)
    {
        // Skip over the prefix
        p += strcountof (OPT);

        MySprintfW (wszTemp,
                    sizeof (wszTemp),
                   L"Was PLTrace = %d  PLStart = %d",
                    bDebugPLTrace,
                    bDebugPLStart);
        // Tell the user about it
        AppendLine (wszTemp, FALSE, TRUE);

        bDebugPLTrace =
        bDebugPLStart = TRUE;
    } // End IF
#undef  OPT

    // Check for PLTrace=[0|1]
#define OPT     L"PLTrace="
    if (strncmpiW (p, OPT, strcountof (OPT)) EQ 0)
    {
        // Skip over the prefix
        p += strcountof (OPT);

        MySprintfW (wszTemp,
                    sizeof (wszTemp),
                   L"Was PLTrace = %d",
                    bDebugPLTrace);
        // Tell the user about it
        AppendLine (wszTemp, FALSE, TRUE);

        bDebugPLTrace = *p++ EQ L'1';
    } // End IF
#undef  OPT

    // Skip over leading white space
    p = SkipWhiteW (p);

    // Check for PLStart=[0|1]
#define OPT     L"PLStart="
    if (strncmpiW (p, OPT, strcountof (OPT)) EQ 0)
    {
        // Skip over the prefix
        p += strcountof (OPT);

        MySprintfW (wszTemp,
                    sizeof (wszTemp),
                   L"Was PLStart = %d",
                    bDebugPLStart);
        // Tell the user about it
        AppendLine (wszTemp, FALSE, TRUE);

        bDebugPLStart = *p++ EQ L'1';
    } // End IF
#undef  OPT

    // Skip over leading white space
    p = SkipWhiteW (p);




    return TRUE;
} // End CmdDebug_EM


//***************************************************************************
//  End of File: sc_debug.c
//***************************************************************************
