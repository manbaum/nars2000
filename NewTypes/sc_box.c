//***************************************************************************
//  NARS2000 -- System Command:  )BOX
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2019 Sudley Place Software

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


//***************************************************************************
//  $CmdBox_EM
//
//  Execute the system command:  )BOX [ON|OFF]
//***************************************************************************

UBOOL CmdBox_EM
    (LPWCHAR lpwszTail)         // Ptr to command line tail

{
    LPWCHAR p = lpwszTail;
    WCHAR   wszTemp[1024];
    UBOOL   bRet = FALSE;       // TRUE iff the result is valid

    // Skip over leading white space
    p = SkipWhiteW (p);

    // Check for inquiry
    if (*p EQ WC_EOS)
    {
        MySprintfW (wszTemp,
                    sizeof (wszTemp),
                   L"Is %s\r\n",
                    gbBoxState ? L"ON" : L"OFF");
        // Tell the user about it
        AppendLine (wszTemp, FALSE, TRUE);

        // Mark as successful
        return TRUE;
    } // End IF

    // Check for "ON"
#define OPT     L"ON"
    if (strncmpiW (p, OPT, strcountof (OPT)) EQ 0)
    {
        if (gbBoxState)
            AppendLine (L"Already ON", FALSE, TRUE);
        else
            AppendLine (L"Is ON", FALSE, TRUE);

        // Set to ON
        gbBoxState = TRUE;

        // Mark as successful
        return TRUE;
    } // End IF
#undef  OPT
    // Check for "OFF"
#define OPT     L"OFF"
    if (strncmpiW (p, OPT, strcountof (OPT)) EQ 0)
    {
        if (hWndDB EQ NULL)
            AppendLine (L"Already OFF", FALSE, TRUE);
        else
            AppendLine (L"Is OFF", FALSE, TRUE);

        // Set to OFF
        gbBoxState = FALSE;

        // Mark as successful
        return TRUE;
    } // End IF
#undef  OPT

#define BOX_MAC(OPT,TXT,NAM,TEST)                       \
    /* Skip over leading white space                  */\
    p = SkipWhiteW (p);                                 \
                                                        \
    /* Check for next option                          */\
    if (strncmpiW (p, OPT, strcountof (OPT)) EQ 0)      \
    {                                                   \
        /* Skip over the prefix                       */\
        q = SkipWhiteW (p + strcountof (OPT));          \
                                                        \
        if (q[0] EQ L'=')                               \
        {                                               \
            /* Skip over the separator                */\
            p = SkipWhiteW (q + 1);                     \
                                                        \
            /* If the input is in hex, ...            */\
            if (p[0] EQ L'0'                            \
             && p[1] EQ L'x')                           \
                /* Get the value                      */\
                sscanfW (&p[2], L"%x", &i);             \
            else                                        \
                /* Get the value                      */\
                sscanfW (p, L"%i", &i);                 \
                                                        \
            /* Skip over the number                   */\
            p = SkipBlackW (p);                         \
                                                        \
            /* Validate it                            */\
            if (TEST (i))                               \
            {                                           \
                /* Format the "Was" message           */\
                MySprintfW (wszTemp,                    \
                            sizeof (wszTemp),           \
                            TXT,                        \
                            NAM);                       \
                /* Tell the user about it             */\
                AppendLine (wszTemp, FALSE, TRUE);      \
                                                        \
                /* Save the new value                 */\
                NAM = i;                                \
                                                        \
                /* Mark as successful                 */\
                bRet = TRUE;                            \
            } /* End IF                               */\
        } /* End IF                                   */\
    } /* End IF                                       */

#define IsUCHAR(a)      (0 <= (a) && (a) < 256)
#define IsWORD(a)       (0 <= (a) && (a) < 65536)

////BOX_MAC (L"Test"       , L"Was Test = %d"      , guTest          , IsUCHAR       )

    // If there was no match, ...
    if (!bRet)
        // Tell the user about it
        IncorrectCommand ();

    return bRet;
} // End CmdBox_EM


//***************************************************************************
//  End of File: sc_box.c
//***************************************************************************
