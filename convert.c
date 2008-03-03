//***************************************************************************
//  NARS2000 -- Conversion Routines
//***************************************************************************

#define STRICT
#include <windows.h>
#include <stdio.h>
#include <math.h>

#include "main.h"
#include "resdebug.h"
#include "sysvars.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $FloatToAplint_SCT
//
//  Attempt to convert a Floating Point number to an APLINT
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

    // See how the number and its tolerant floor compare
    if (CompareCT (fFloat, (APLFLOAT) aplInteger, SYS_CT, NULL))
    {
        if (lpbRet)
            *lpbRet = TRUE;
        return aplInteger;
    } // End IF

    // Convert to an integer (rounding up)
    aplInteger = (APLINT) ceil (fFloat);

    // See how the number and its tolerant ceiling compare
    if (CompareCT (fFloat, (APLFLOAT) aplInteger, SYS_CT, NULL))
    {
        if (lpbRet)
            *lpbRet = TRUE;
        return aplInteger;
    } // End IF

    // It's not close enough, so we failed
    if (lpbRet)
        *lpbRet = FALSE;

    // Return the ceiling of the fractional value
    // The ceiling is important in CheckAxis for laminate
    return aplInteger;
} // End FloatToAplint_SCT


//***************************************************************************
//  $FloatToAplchar
//
//  Convert a floating point number to APLCHARs
//***************************************************************************

void FloatToAplchar
    (APLFLOAT  aplFloat,
     LPAPLCHAR lpMem)

{
    int i;

#define fta     ((LPCHAR) &aplFloat)

    for (i = 7; i >= 0; i--)
    {
        *lpMem++ = L"0123456789ABCDEF"[(fta[i] & 0xF0) >> 4];
        *lpMem++ = L"0123456789ABCDEF"[(fta[i] & 0x0F) >> 0];
    } // End FOR
#undef  fta
} // Enf FloatToAplchar


//// //***************************************************************************
//// //  $sprintfW
//// //
//// //  sprintf for wide chars
//// //***************************************************************************
////
//// int sprintfW
////     (LPAPLCHAR lpwszOut,
////      LPAPLCHAR lpwszFmt,
////      ...)
////
//// {
////     va_list vl;
////     int     i1, i2, i3, i4,
////             iLen;
////     char    szFmt[128],
////             szTemp[1024];
////
////     // We hope that no one calls us with more than
////     //   four arguments.
////     // Note we must grab them separately this way
////     //   as using va_arg in the argument list to
////     //   wsprintf pushes the arguments in reverse
////     //   order.
////     va_start (vl, lpwszFmt);
////
////     i1 = va_arg (vl, int);
////     i2 = va_arg (vl, int);
////     i3 = va_arg (vl, int);
////     i4 = va_arg (vl, int);
////
////     va_end (vl);
////
////     // Convert the format string to ASCII
////     W2A (szFmt, lpwszFmt, sizeof (szFmt) - 1);
////
////     // Format the data
////     iLen = sprintf (szTemp,
////                     szFmt,
////                     i1, i2, i3, i4);
////
////     // Convert the result back to wide chars
////     // The destin buffer length is a guess
////     A2W (lpwszOut, szTemp, 4096);
////
////     return iLen;
//// } // End sprintfW


//***************************************************************************
//  $ConvertWideToName
//
//  Convert wide chars to ASCII or {name} or {\xXXXX}
//***************************************************************************

UINT ConvertWideToName
    (LPWCHAR lpwszOut,
     LPWCHAR lpwszInp)

{
    WCHAR   wc;             // A wide char
    LPCHAR  lpSymbolName;   // Ptr to symbol name
    LPWCHAR lpwsz;          // Ptr to output save area

    // Initialize the ptr to the output save area
    lpwsz = lpwszOut;

    // Loop through the wide chars
    while (wc = *lpwszInp++)
    if (32 <= wc && wc <= 0x7E
     && wc NE L'\''
     && wc NE L'{'
     && wc NE L'}')
        *lpwsz++ = wc;
    else
    {
        // Check for name in hash table
        lpSymbolName = SymbolToName (wc);
        if (lpSymbolName)
            lpwsz += wsprintfW (lpwsz,
                                L"%S",
                                lpSymbolName);
        else
            lpwsz += wsprintfW (lpwsz,
                                L"{\\x%04X}",
                                wc);
    } // End IF/ELSE

    // Ensure properly terminated
    *lpwsz = L'\0';

    return (lpwsz - lpwszOut);
} // End ConvertWideToName


//***************************************************************************
//  $FormatCurDateTime
//
//  Format the current date & time as "dd/mm/yyyy hh:mm:ss"
//***************************************************************************

void FormatCurDateTime
    (LPAPLCHAR wszTemp)

{
    SYSTEMTIME SystemTime;

    // Get the current date & time
    if (bUseLocalTime)
        GetLocalTime  (&SystemTime);
    else
        GetSystemTime (&SystemTime);

    wsprintfW (wszTemp,
               L"%02d/%02d/%04d %2d:%02d:%02d",
               SystemTime.wMonth,
               SystemTime.wDay,
               SystemTime.wYear,
               SystemTime.wHour,
               SystemTime.wMinute,
               SystemTime.wSecond);
} // End FormatCurDateTime


//***************************************************************************
//  End of File: convert.c
//***************************************************************************
