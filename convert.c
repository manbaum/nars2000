//***************************************************************************
//  NARS2000 -- Conversion Routines
//***************************************************************************

#pragma pack (1)
#define STRICT
#include <windows.h>
#include <stdio.h>
#include <math.h>

#include "main.h"
#include "datatype.h"
#include "errors.h"
#include "resdebug.h"
#include "sysvars.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  FloatToAplint_SCT
//
//  Convert a Floating Point number to an APLINT
//    using System Comparison Tolerance
//***************************************************************************

APLINT FloatToAplint_SCT
    (APLFLOAT fFloat,       // The number to convert
     LPBOOL   lpbRet)       // TRUE iff successful conversion
                            // (may be NULL if the caller isn't interested)
{
    APLINT aplInteger;

    // Convert to an integer (rounding down)
    aplInteger = (APLINT) floor (fFloat);

    if (SYS_CT > fabs (fFloat - (APLFLOAT) aplInteger))
    {
        if (lpbRet)
            *lpbRet = TRUE;
        return aplInteger;
    } // End IF

    // Convert to an integer (rounding up)
    aplInteger = (APLINT) ceil (fFloat);

    if (SYS_CT > fabs (fFloat - (APLFLOAT) aplInteger))
    {
        if (lpbRet)
            *lpbRet = TRUE;
        return aplInteger;
    } // End IF

    // It's not close enough, so we failed
    if (lpbRet)
        *lpbRet = FALSE;

    return 0;
} // End FloatToAplint_SCT


//***************************************************************************
//  sprintfW
//
//  sprintf for wide chars
//***************************************************************************

int sprintfW
    (LPAPLCHAR lpwszOut,
     LPAPLCHAR lpwszFmt,
     ...)

{
    va_list vl;
    int     i1, i2, i3, i4,
            iLen;
    char    szFmt[128];

    // We hope that no one calls us with more than
    //   four arguments.
    // Note we must grab them separately this way
    //   as using va_arg in the argument list to
    //   wsprintf pushes the arguments in reverse
    //   order.
    va_start (vl, lpwszFmt);

    i1 = va_arg (vl, int);
    i2 = va_arg (vl, int);
    i3 = va_arg (vl, int);
    i4 = va_arg (vl, int);

    // Convert the format string to ASCII
    W2A (szFmt, lpwszFmt, sizeof (szFmt) - 1);

    // Format the data
    iLen = sprintf (lpszTemp,
                    szFmt,
                    i1, i2, i3, i4);

    // Convert the result back to wide chars
    A2W (lpwszOut, lpszTemp);

    va_end (vl);

    return iLen;
} // End sprintfW


//***************************************************************************
//  End of File: convert.c
//***************************************************************************
