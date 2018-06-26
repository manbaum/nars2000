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
             bDebugPLStart,
             bDebugExecTrace,
             bDebugCS,
             bDebugFH,
             bDebugPN,
             gYYAlloc;


//***************************************************************************
//  $CmdDebug_EM
//
//  Execute the system command:  )DEBUG [PLTrace=[0|1]] [PLStart=[0|1]]
//***************************************************************************

UBOOL CmdDebug_EM
    (LPWCHAR lpwszTail)         // Ptr to command line tail

{
    LPWCHAR p = lpwszTail,
            q;
    WCHAR   wszTemp[1024];
    int     i;

    // Skip over leading white space
    p = SkipWhiteW (p);

    // Check for inquiry
    if (*p EQ WC_EOS)
    {
        MySprintfW (wszTemp,
                    sizeof (wszTemp),
                   L"Is PLTrace = %d  PLStart = %d  ExecTrace = %d  CS = %d  FH = %d  PN = %d  YYAlloc = 0x%X\r\n"
                   L"   DbgLvl = %d  FcnLvl = %d  VfpLvl = %d  ResizeLvl = %d",
                    bDebugPLTrace,
                    bDebugPLStart,
                    bDebugExecTrace,
                    bDebugCS,
                    bDebugFH,
                    bDebugPN,
                    gYYAlloc,
                    gDbgLvl,
                    gFcnLvl,
                    gVfpLvl,
                    gResizeLvl);
        // Tell the user about it
        AppendLine (wszTemp, FALSE, TRUE);

        return TRUE;
    } // End IF

    // Check for allPL=[0|1]
#define OPT     L"allPL"
    if (strncmpiW (p, OPT, strcountof (OPT)) EQ 0)
    {
        // Skip over the prefix
        q = SkipWhiteW (p + strcountof (OPT));

        if (q[0] EQ L'=')
        {
            // Skip over the separator
            p = SkipWhiteW (q + 1);

            // Get the value
            sscanfW (p, L"%i", &i);

////////////// Get the value
////////////i = _wtoi (p);

            // Skip over the number
            p = SkipBlackW (p);

            // Validate it
            if (IsBooleanValue (i))
            {
                // Format the "Was" message
                MySprintfW (wszTemp,
                            sizeof (wszTemp),
                           L"Was PLTrace = %d  PLStart = %d",
                            bDebugPLTrace,
                            bDebugPLStart);
                // Tell the user about it
                AppendLine (wszTemp, FALSE, TRUE);

                // Save the new value
                bDebugPLTrace =
                bDebugPLStart = i;
            } else
            {
                // Tell the user about it
                IncorrectCommand ();

                return FALSE;
            } // End IF/ELSE
        } else
        {
            // Tell the user about it
            IncorrectCommand ();

            return FALSE;
        } // End IF/ELSE
    } // End IF
#undef  OPT

#define DEBUG_MAC(OPT,TXT,NAM,TEST)                     \
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
            /* Get the value                          */\
            sscanfW (p, L"%i", &i);                     \
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
            } else                                      \
            {                                           \
                /* Tell the user about it             */\
                IncorrectCommand ();                    \
                                                        \
                return FALSE;                           \
            } /* End IF/ELSE                          */\
        } else                                          \
        {                                               \
            /* Tell the user about it                 */\
            IncorrectCommand ();                        \
                                                        \
            return FALSE;                               \
        } /* End IF/ELSE                              */\
    } /* End IF                                       */

#define IsUCHAR(a)      (0 <= (a) && (a) < 256)
#define IsWORD(a)       (0 <= (a) && (a) < 65536)

    DEBUG_MAC (L"PLTrace"    , L"Was PLTrace = %d"   , bDebugPLTrace   , IsBooleanValue)
    DEBUG_MAC (L"PLStart"    , L"Was PLStart = %d"   , bDebugPLStart   , IsBooleanValue)
    DEBUG_MAC (L"ExecTrace"  , L"Was ExecTrace = %d" , bDebugExecTrace , IsBooleanValue)
    DEBUG_MAC (L"CS"         , L"Was CS = %d"        , bDebugCS        , IsBooleanValue)
    DEBUG_MAC (L"FH"         , L"Was FH = %d"        , bDebugFH        , IsBooleanValue)
    DEBUG_MAC (L"PN"         , L"Was PN = %d"        , bDebugPN        , IsBooleanValue)
    DEBUG_MAC (L"YYAlloc"    , L"Was YYAlloc = 0x%X" , gYYAlloc        , IsWORD        )
    DEBUG_MAC (L"DbgLvl"     , L"Was DbgLvl = %d"    , gDbgLvl         , IsUCHAR       )
    DEBUG_MAC (L"FcnLvl"     , L"Was FcnLvl = %d"    , gFcnLvl         , IsUCHAR       )
    DEBUG_MAC (L"VfpLvl"     , L"Was VfpLvl = %d"    , gVfpLvl         , IsUCHAR       )
    DEBUG_MAC (L"ResizeLvl"  , L"Was ResizeLvl = %d" , gResizeLvl      , IsUCHAR       )

    return TRUE;
} // End CmdDebug_EM


//***************************************************************************
//  End of File: sc_debug.c
//***************************************************************************
