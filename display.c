//***************************************************************************
//  NARS2000 -- Display Routines
//***************************************************************************

#define STRICT
#include <windows.h>
#include <float.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "termcode.h"
#include "externs.h"
#include "display.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


#ifdef DEBUG
// Resource debugging variables
extern int iCountGLOBAL[MAXOBJ];
extern HANDLE ahGLOBAL[MAXOBJ];
extern UINT auLinNumGLOBAL[MAXOBJ];
#endif

//***************************************************************************
//  $ArrayDisplay_EM
//
//  Display an array
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- AssignDisplay_EM"
#else
#define APPEND_NAME
#endif

BOOL ArrayDisplay_EM
    (LPTOKEN lptkRes,               // Ptr to value token
     BOOL    bEndingCR)             // TRUE iff last line has CR

{
    LPAPLCHAR lpaplChar;

    // Split cases based upon the token type
    switch (lptkRes->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRes->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, it's an array
            if (!lptkRes->tkData.tkSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (lptkRes->tkData.tkSym->stData.stVoid));

                // Check for NoDisplay flag
                if (!lptkRes->tkFlags.NoDisplay)
                    DisplayGlbArr (ClrPtrTypeDirGlb (lptkRes->tkData.tkSym->stData.stGlbData),
                                   bEndingCR);
                return TRUE;

                break;
            } // End IF

            // Check for NoDisplay flag
            if (lptkRes->tkFlags.NoDisplay)
                return TRUE;

            // Handle the immediate case

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRes->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // stData is immediate
            Assert (lptkRes->tkData.tkSym->stFlags.Imm);

            lpaplChar =
            FormatSymTabConst (lpwszFormat,
                               lptkRes->tkData.tkSym);
            break;

        case TKT_VARIMMED:  // The tkData is an immediate constant
            // Check for NoDisplay flag
            if (lptkRes->tkFlags.NoDisplay)
                return TRUE;

            lpaplChar =
            FormatImmed (lpwszFormat,
                         lptkRes->tkFlags.ImmType,
                         &lptkRes->tkData.tkLongest);
            break;

        case TKT_LISTPAR:   // The tkData is an HGLOBAL of an array of LPSYMENTRYs/HGLOBALs
            ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                       lptkRes);
            return FALSE;

        case TKT_VARARRAY:  // The tkData is an HGLOBAL of an array of LPSYMENTRYs/HGLOBALs
            // Check for NoDisplay flag
            if (lptkRes->tkFlags.NoDisplay)
                return TRUE;

            switch (GetPtrTypeDir (lptkRes->tkData.tkVoid))
            {
                case PTRTYPE_STCONST:
                    lpaplChar =
                    FormatSymTabConst (lpwszFormat,
                                       lptkRes->tkData.tkSym);
                    break;

                case PTRTYPE_HGLOBAL:
                    DisplayGlbArr (ClrPtrTypeDirGlb (lptkRes->tkData.tkGlbData),
                                   bEndingCR);
                    return TRUE;

                defstop
                    return FALSE;
            } // End SWITCH

            break;

////////case TKT_STRAND:    // The tkData is an HGLOBAL of a single HGLOBAL
        defstop
            return FALSE;
    } // End SWITCH

    // Delete the last blank in case it matters,
    //   and ensure properly terminated
    if (lpaplChar[-1] EQ L' ')
        *--lpaplChar = L'\0';
    else
        *lpaplChar = L'\0';

    // Display the line
    AppendLine (lpwszFormat, FALSE, bEndingCR);

    return TRUE;
} // End ArrayDisplay_EM
#undef  APPEND_NAME


//***************************************************************************
//  $DisplayGlbArr
//
//  Display a global array
//***************************************************************************

void DisplayGlbArr
    (HGLOBAL   hGlb,
     BOOL      bEndingCR)       // TRUE iff last line has CR

{
    LPVOID      lpMem;
    LPAPLCHAR   lpaplCharIni = NULL,
                lpaplChar,
                lpaplCharStart,
                lpwsz;
    APLSTYPE    aplType;
    APLNELM     aplNELM;
    APLRANK     aplRank;
    LPAPLDIM    lpMemDim;
    APLDIM      aplDimNCols,
                aplDimNRows,
                aplDimCol,
                aplLastDim;
    APLNELM     aplNELMRes;
    LPFMTHEADER lpFmtHeader;
    LPFMTCOLSTR lpFmtColStr;
    UINT        uCol;

#define DEF_DISPLAY_INITSIZE    65536
#define DEF_DISPLAY_MAXSIZE     65536

    // Allocate space for the display
    lpaplCharIni =
      VirtualAlloc (NULL,          // Any address
                    DEF_DISPLAY_MAXSIZE * sizeof (APLCHAR),
                    MEM_RESERVE,
                    PAGE_READWRITE);
    if (!lpaplCharIni)
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsg ("DisplayGlbArr:  VirtualAlloc for <lpaplCharIni> failed");

        return;             // Mark as failed
    } // End IF

    // Commit the intial size
    VirtualAlloc (lpaplCharIni,
                  DEF_DISPLAY_INITSIZE * sizeof (APLCHAR),
                  MEM_COMMIT,
                  PAGE_READWRITE);

#ifdef DEBUG
    // Fill lpaplCharIni with FFs so we can tell what we actually wrote
    FillMemory (lpaplCharIni, 1024, 0xFF);
#endif

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlb);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)

    // Get the header values
    aplType = lpHeader->ArrType;
    aplNELM = lpHeader->NELM;
    aplRank = lpHeader->Rank;

#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemDim = VarArrayBaseToDim (lpMem);

    // Skip over the header and dimension to the data
    lpMem = VarArrayBaseToData (lpMem, aplRank);

    // Get the # columns
    if (aplRank EQ 0)
        aplDimNCols = aplDimNRows = 1;
    else
    {
        aplDimNCols = lpMemDim[aplRank - 1];

        // If there are no columns and the rank is > 1, ignore this
        if (aplDimNCols EQ 0
         && aplRank > 1)
            goto NORMAL_EXIT;

        // Get the # rows (across all planes)
        if (aplRank EQ 1)
            aplDimNRows = 1;
        else
            aplDimNRows = aplNELM / aplDimNCols;
    } // End IF/ELSE

    // Define a new FMTHEAD in the output
    ZeroMemory ((LPFMTHEADER) lpaplCharIni, sizeof (FMTHEADER));
////((LPFMTHEADER) lpaplCharIni)->lpFmtHeadUp = NULL;  // Filled in by ZeroMemory
    lpFmtHeader = (LPFMTHEADER) lpaplCharIni;
#ifdef DEBUG
    lpFmtHeader->Signature   = FMTHEADER_SIGNATURE;
#endif
////lpFmtHeader->lpFmtRowUp  = NULL;                // Filled in by ZeroMemory
////lpFmtHeader->lpFmtColUp  = NULL;                // ...
////lpFmtHeader->lpFmtRow1st =                      // Filled in below
////lpFmtHeader->lpFmtCol1st =                      // ...
    lpFmtHeader->uRows       = (UINT) aplDimNRows;
    lpFmtHeader->uCols       = (UINT) aplDimNCols;
////lpFmtHeader->uFmtRows    = 0;                   // Filled in by ZeroMemory
////lpFmtHeader->uFmtInts    = 0;                   // ...
////lpFmtHeader->uFmtFrcs    = 0;                   // ...
////lpFmtHeader->uFmtTrBl    = 0;                   // ...
////lpFmtHeader->uDepth      = 0;                   // ...

    // Define <aplDimNCols> FMTCOLSTRs in the output
    lpFmtColStr = (LPFMTCOLSTR) (&lpFmtHeader[1]);
    lpFmtHeader->lpFmtCol1st = lpFmtColStr;
    ZeroMemory (lpFmtColStr, (UINT) aplDimNCols * sizeof (FMTCOLSTR));
#ifdef DEBUG
    {
        APLDIM uCol;

        for (uCol = 0; uCol < aplDimNCols; uCol++)
            lpFmtColStr[uCol].Signature = FMTCOLSTR_SIGNATURE;
    }
#endif
    // Skip over the FMTCOLSTRs
    lpaplCharStart = (LPAPLCHAR) (&lpFmtColStr[aplDimNCols]);

    // Save ptr to 1st child FMTROWSTR
    if (aplDimNRows)
        lpFmtHeader->lpFmtRow1st = (LPFMTROWSTR) lpaplCharStart;
    else
        lpFmtHeader->lpFmtRow1st = NULL;

    // Loop through the array appending the formatted values (separated by L'\0')
    //   to the output vector, and accumulating the values in the appropriate
    //   FMTCOLSTR & FMTROWSTR entries.

    // Split cases based upon the array's storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
////////////lpaplChar =
            CompileArrBool    ((LPAPLBOOL)    lpMem,    // Ptr to right arg memory
                               lpFmtHeader,             // Ptr to parent header
                               lpFmtColStr,             // Ptr to vector of ColStrs
                               lpaplCharStart,          // Ptr to compiled output
                               aplDimNRows,             // # rows
                               aplDimNCols,             // # cols
                               aplRank,                 // Right arg rank
                               lpMemDim,                // Ptr to right arg dimensions
                               FALSE);                  // TRUE iff nested
            break;

        case ARRAY_INT:
////////////lpaplChar =
            CompileArrInteger ((LPAPLINT)    lpMem,     // Ptr to right arg memory
                               lpFmtHeader,             // Ptr to parent header
                               lpFmtColStr,             // Ptr to vector of ColStrs
                               lpaplCharStart,          // Ptr to compiled output
                               aplDimNRows,             // # rows
                               aplDimNCols,             // # cols
                               aplRank,                 // Right arg rank
                               lpMemDim,                // Ptr to right arg dimensions
                               FALSE);                  // TRUE iff nested
            break;

        case ARRAY_FLOAT:
////////////lpaplChar =
            CompileArrFloat   ((LPAPLFLOAT)  lpMem,     // Ptr to right arg memory
                               lpFmtHeader,             // Ptr to parent header
                               lpFmtColStr,             // Ptr to vector of ColStrs
                               lpaplCharStart,          // Ptr to compiled output
                               aplDimNRows,             // # rows
                               aplDimNCols,             // # cols
                               aplRank,                 // Right arg rank
                               lpMemDim,                // Ptr to right arg dimensions
                               FALSE);                  // TRUE iff nested
            break;

        case ARRAY_APA:
////////////lpaplChar =
            CompileArrAPA     ((LPAPLAPA)    lpMem,     // Ptr to right arg memory
                               lpFmtHeader,             // Ptr to parent header
                               lpFmtColStr,             // Ptr to vector of ColStrs
                               lpaplCharStart,          // Ptr to compiled output
                               aplDimNRows,             // # rows
                               aplDimNCols,             // # cols
                               aplRank,                 // Right arg rank
                               lpMemDim,                // Ptr to right arg dimensions
                               FALSE);                  // TRUE iff nested
            break;

        case ARRAY_CHAR:
////////////lpaplChar =
            CompileArrChar    ((LPAPLCHAR)   lpMem,     // Ptr to right arg memory
                               lpFmtHeader,             // Ptr to parent header
                               lpFmtColStr,             // Ptr to vector of ColStrs
                               lpaplCharStart,          // Ptr to compiled output
                               aplDimNRows,             // # rows
                               aplDimNCols,             // # cols
                               aplRank,                 // Right arg rank
                               lpMemDim,                // Ptr to right arg dimensions
                               FALSE);                  // TRUE iff nested
            break;

        case ARRAY_HETERO:
////////////lpaplChar =
            CompileArrHetero  ((LPAPLHETERO) lpMem,     // Ptr to right arg memory
                               lpFmtHeader,             // Ptr to parent header
                               lpFmtColStr,             // Ptr to vector of ColStrs
                               lpaplCharStart,          // Ptr to compiled output
                               aplDimNRows,             // # rows
                               aplDimNCols,             // # cols
                               aplRank,                 // Right arg rank
                               lpMemDim,                // Ptr to right arg dimensions
                               FALSE);                  // TRUE iff nested
            break;

        case ARRAY_NESTED:
////////////lpaplChar =
            CompileArrNested  ((LPAPLNESTED) lpMem,     // Ptr to right arg memory
                               lpFmtHeader,             // Ptr to parent header
                               lpFmtColStr,             // Ptr to vector of ColStrs
                               lpaplCharStart,          // Ptr to compiled output
                               aplDimNRows,             // # rows
                               aplDimNCols,             // # cols
                               aplRank,                 // Right arg rank
                               lpMemDim,                // Ptr to right arg dimensions
                               FALSE);                  // TRUE iff nested
            break;

        defstop
            break;
    } // End SWITCH

    // Propogate the row & col count up the line
    PropogateRowColCount (lpFmtHeader,
                          FALSE);

    // lpaplCharStart now contains the compiled version of the output

    // Add up the width of each column to get the
    //   # cols in the result
    for (aplLastDim = aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        aplLastDim += (lpFmtColStr[aplDimCol].uInts
                     + lpFmtColStr[aplDimCol].uFrcs);
    aplLastDim += lpFmtHeader->uFmtTrBl;
    Assert (aplLastDim EQ (lpFmtHeader->uFmtInts
                         + lpFmtHeader->uFmtFrcs
                         + lpFmtHeader->uFmtTrBl));
    // Calculate the NELM of the result
    aplNELMRes = (lpFmtHeader->uFmtRows * (lpFmtHeader->uFmtInts
                                         + lpFmtHeader->uFmtFrcs
                                         + lpFmtHeader->uFmtTrBl));
#ifdef PREFILL
    // Fill the output area with all blanks
    for (lpaplChar = lpwszFormat, aplDimCol = 0; aplDimCol < aplNELMRes; aplDimCol++)
        *lpaplChar++ = L' ';
#endif
    lpaplChar = lpwszFormat;

    // Run through the array again processing the
    //   output stream into lpwszFormat

    // Split cases based upon the array's storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_CHAR:
        case ARRAY_APA:
        case ARRAY_HETERO:
////////////lpaplChar =
            FormatArrSimple (lpFmtHeader,           // Ptr to FMTHEADER
                             lpFmtColStr,           // Ptr to vector of <aplDimNCols> FMTCOLSTRs
                             lpaplCharStart,        // Ptr to compiled input
                            &lpaplChar,             // Ptr to output string
                             aplDimNRows,           // # rows in this array
                             aplDimNCols,           // # cols in this array
                             aplLastDim,            // Length of last dim in result (NULL for !bRawOutput)
                             aplRank,               // Rank of this array
                             lpMemDim,              // Ptr to this array's dimensions
                             TRUE,                  // TRUE iff raw output
                             bEndingCR);            // TRUE iff last line has CR
            break;

        case ARRAY_NESTED:
////////////lpaplChar =
            FormatArrNested (lpFmtHeader,           // Ptr to FMTHEADER
                             lpMem,                 // Ptr to raw input
                             lpFmtColStr,           // Ptr to vector of <aplDimNCols> FMTCOLSTRs
                             lpaplCharStart,        // Ptr to compiled input
                            &lpaplChar,             // Ptr to ptr to output string
                             aplDimNRows,           // # rows in this array
                             aplDimNCols,           // # cols ...
                             aplRank,               // Rank of this array
                             lpMemDim,              // Ptr to this array's dimensions
                             aplLastDim,            // Length of last dim in result (NULL for !bRawOutput)
                             FALSE,                 // TRUE iff raw (not {thorn}) output
                             bEndingCR);            // TRUE iff last line has CR
            // Loop through the formatted rows
            for (lpwsz = lpwszFormat, uCol = 0;
                 uCol < lpFmtHeader->uFmtRows;
                 uCol++, lpwsz += aplLastDim)
            {
                WCHAR   wch;                // The replaced WCHAR
                APLDIM  aplDimTmp;          // Remaining line length to output
                APLUINT uOutLen;            // Output length for this line
                BOOL    bLineCont;          // TRUE iff this line is a continuation
                APLUINT uOffset;            // Offset in line to start of display

                // ***FIXME*** -- this routine may split a number in half
                //                because it doesn't know the difference
                //                numbers and characters

                aplDimTmp = aplLastDim;     // Save line length
                uOutLen = uQuadPW;          // Initial output length
                bLineCont = FALSE;          // First line isn't a continuation
                uOffset = 0;                // Initialize the line offset
                while (aplDimTmp > uQuadPW)
                {
                    // Because AppendLine works on single zero-terminated lines,
                    //   we need to create one
                    wch = lpwsz[uOffset + uOutLen];     // Save the ending char
                    lpwsz[uOffset + uOutLen] = L'\0';   // Terminate the line
                    AppendLine (lpwsz + uOffset, bLineCont, TRUE);  // Display the line
                    lpwsz[uOffset + uOutLen] = wch;     // Restore the ending char

                    bLineCont = TRUE;                   // Lines from here on are continuations
                    AppendLine (wszIndent, bLineCont, FALSE);   // Display the indent

                    aplDimTmp -= uOutLen;               // Less how much we output
                    uOffset += uOutLen;                 // Skip over what we just output
                    uOutLen = uQuadPW - DEF_INDENT;     // Take into account the indent
                } // End WHILE

                // Output whatever remains

                // Because AppendLine works on single zero-terminated lines,
                //   we need to create one
                wch = lpwsz[uOffset + aplDimTmp];       // Save the ending char
                lpwsz[uOffset + aplDimTmp] = L'\0';     // Terminate the line
                AppendLine (lpwsz + uOffset, bLineCont, TRUE);// Display the line
                lpwsz[uOffset + aplDimTmp] = wch;       // Restore the ending char
            } // End FOR

            // If this is an empty vector, make sure it skips a line
            if (lpFmtHeader->uFmtRows EQ 0
             && aplRank EQ 1)
                AppendLine (L"", FALSE, TRUE);// Display the empty line

            break;

        defstop
            break;
    } // End SWITCH
NORMAL_EXIT:
    // Free the local storage
    if (lpaplCharIni)
    {
        VirtualFree (lpaplCharIni, 0, MEM_RELEASE); lpaplCharIni = NULL;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlb); lpMem = NULL;
} // End DisplayGlbArr


//***************************************************************************
//  $FormatImmed
//
//  Format an immediate constant
//***************************************************************************

LPAPLCHAR FormatImmed
    (LPWCHAR      lpaplChar,
     UINT         ImmType,
     LPAPLLONGEST lpaplLongest)

{
    WCHAR wc;

    switch (ImmType)
    {
        case IMMTYPE_BOOL:
            lpaplChar =
            FormatAplint (lpaplChar,
                          *(LPAPLBOOL) lpaplLongest);
            break;

        case IMMTYPE_INT:
            lpaplChar =
            FormatAplint (lpaplChar,
                          *(LPAPLINT) lpaplLongest);
            break;

        case IMMTYPE_CHAR:
            // Get the char
            wc = *(LPAPLCHAR) lpaplLongest;

            // Test for Terminal Control chars
            switch (wc)
            {
                case TCBEL:     // Bel
                case TCBS:      // BS
                case TCDEL:     // Del
                case TCESC:     // Esc
                case TCFF:      // FF
                case TCHT:      // HT
                case TCLF:      // LF
                case TCNL:      // NL
                case TCNUL:     // NUL
                    DbgBrk ();  // ***FINISHME***








                    break;
            } // End SWITCH

            lpaplChar +=
            wsprintfW (lpaplChar,
///////////////////////L"'%c' ",
                       L"%c ",
                       wc);
            break;

        case IMMTYPE_FLOAT:
            lpaplChar =
            FormatFloat (lpaplChar,
                         *(LPAPLFLOAT) lpaplLongest);

            break;

        defstop
            break;
    } // End SWITCH

    return lpaplChar;
} // End FormatImmed


//***************************************************************************
//  $FormatAplint
//
//  Format an APLINT
//***************************************************************************

LPAPLCHAR FormatAplint
    (LPAPLCHAR lpaplChar,
     APLINT    aplInt)

{

#define MAXLEN  32

    WCHAR wszTemp[MAXLEN + 1];
    int   i;
    UINT  u;

    // Check the sign bit -- if set, save and make positive
    if (aplInt < 0)
    {
        *lpaplChar++ = UTF16_OVERBAR;
        aplInt = -aplInt;
    } // End IF

    // Format the number one digit at a time
    //   in reverse order into lpwszFormat
    i = MAXLEN - 1;

    // Execute at least once so zero prints
    do
    {
        u = (UINT) (aplInt % 10);
        wszTemp[i--] = u + L'0';
        aplInt = aplInt / 10;
    } while (aplInt);

    // Copy the digits to the result
    for (i++; i < MAXLEN; i++)
        *lpaplChar++ = wszTemp[i];

    // Include a separator
    *lpaplChar++ = L' ';

    return lpaplChar;
} // End FormatAplint


//***************************************************************************
//  $FormatFloat
//
//  Format a floating point number
//***************************************************************************

LPAPLCHAR FormatFloat
    (LPWCHAR  lpaplChar,
     APLFLOAT fFloat)

{
    if (!_finite (fFloat))
    {
        if (fFloat < 0)
            *lpaplChar++ = UTF16_OVERBAR;
        *lpaplChar++ = L'_';
    } else
    {
        LPAPLCHAR p, ep, dp;

        // Convert fFloat to an ASCII string
        // Use David Gay's routines
        g_fmt (lpszTemp,        // Output save area
               fFloat,          // # to convert to ASCII
               2,               // Mode 2: max (ndigits, 1)
         (int) uQuadPP);        // ndigits

        // Convert from one-byte ASCII to two-byte UTF16
        // The destin buffer length just needs be long enough
        //    to handle a FP number
        A2W (lpaplChar, lpszTemp, 4096);

        p  = lpaplChar;
        ep = strchrW (lpaplChar, L'e');
        dp = strchrW (lpaplChar, L'.');

        // Convert to normalized form
        // [s][int][.][frc][E][s][exp]

        // Check for minus sign in the mantissa
        if (lpaplChar[0] EQ L'-')
        {
            lpaplChar[0] = UTF16_OVERBAR;
            p++;        // Skip over it
        } // End IF

        // Check for no leading 0 with .123
        if (*p EQ L'.')
        {
            // Use MoveMemory as the source and destin blocks overlap
            MoveMemory (p + 1, p, (1 + lstrlenW (p)) * sizeof (APLCHAR));
            *p = L'0';
            dp++;
        } else
        // Check for trailing zeros in integer only because
        //   of small uQuadPP.  Replace them with E-notation.
        if (lstrlenW (p) > uQuadPP  // Too many digits vs. significant digits
         && dp EQ NULL              // No fractional part
         && ep EQ NULL)             // Not already E-notation
        {
            // Move data to the right to make room for a decimal point
            // Use MoveMemory as the source and destin blocks overlap
            MoveMemory (p + 2, p + 1, (lstrlenW (p)) * sizeof (APLCHAR));
            p[1] = L'.';        // Insert a decimal point
            dp = p + 1;         // Save location of decimal point

            p[uQuadPP + 1] = L'E';
            ep = &p[uQuadPP + 1];
            p = FormatAplint (&p[uQuadPP + 2], lstrlenW (p) - 2);
            p[-1] = L'\0';
        } // End IF/ELSE

        // p points to the trailing zero
        p = lpaplChar + lstrlenW (lpaplChar);
        if (ep)
        {
            // Check for trailing decimal point in the mantissa
            if (ep[-1] EQ L'.')
            {
                // Use MoveMemory as the source and destin blocks overlap
                MoveMemory (ep + 1, ep, (1 + p++ - ep) * sizeof (APLCHAR));
                *ep++ = L'0';   // Change to zero and skip over so it ends with ".0"
            } else  // No trailing decimal point:  insert one if not already present
            if (!dp)            // If not already present, ...
            {
                // Use MoveMemory as the source and destin blocks overlap
                MoveMemory (ep + 2, ep, (1 + p - ep) * sizeof (APLCHAR));
                p += 2;         // Add to length # chars we're inserting
                dp = ep;        // Save location of decimal point
                *ep++ = L'.';   // Insert a decimal point
                *ep++ = L'0';   // Insert a trailing zero
            } // End IF/ELSE

            *ep++ = L'E';       // Change to uppercase and skip over

            // Check for minus sign in the exponent
            if (ep[0] EQ L'-')
                *ep++ = UTF16_OVERBAR;  // Change to high minus and skip over
            else
            // Check for plus sign in the exponent
            if (ep[0] EQ L'+')
                // Delete by copying over
                CopyMemory (ep, ep + 1, (1 + p-- - (ep + 1)) * sizeof (APLCHAR));

            // Check for leading 0s in the exponent
            while (ep[0] EQ L'0')
                // Delete by copying over
                CopyMemory (ep, ep + 1, (1 + p-- - (ep + 1)) * sizeof (APLCHAR));

        } else
        // Check for trailing decimal point in the mantissa
        if (p[-1] EQ '.')
            *--p = L'\0';        // Zap it and back up to the terminating zero

        // Point to the terminating zero
        lpaplChar = p;
    } // End IF/ELSE/...

    // Append a separator
    *lpaplChar++ = L' ';

    return lpaplChar;
} // End FormatFloat


//***************************************************************************
//  $FormatSymTabConst
//
//  Format a symbol table constant
//***************************************************************************

LPAPLCHAR FormatSymTabConst
    (LPWCHAR    lpaplChar,
     LPSYMENTRY lpSymEntry)

{
    Assert (lpSymEntry->stFlags.Imm);

    return FormatImmed (lpaplChar,
                        lpSymEntry->stFlags.ImmType,
                       &lpSymEntry->stData.stLongest);
} // End FormatSymTabConst


#ifdef DEBUG
//***************************************************************************
//  $DisplayHshTab
//
//  Display the Hash Table entries
//***************************************************************************

void DisplayHshTab
    (void)

{
    LPHSHENTRY lpHshEntry;
    int        i, j;

    typedef struct tagHT_FLAGNAMES
    {
        UINT    uMask;
        LPWCHAR lpwszName;
    } HT_FLAGNAMES, *LPHT_FLAGNAMES;

    // Hash Table flag names
    static HT_FLAGNAMES ahtFlagNames[] =
    {
     {0x00001,  L" Inuse"    },
     {0x00002,  L" PrinHash" },
     {0x00004,  L" SymCopy"  },
    };

    DbgMsg ("********** Hash Table **********************************");

    wsprintf (lpszDebug,
              "lpHshTab = %08X, SplitNext = %08X, Last = %08X",
              lpHshTab,
              lpHshTabSplitNext,
              &lpHshTab[iHshTabTotalSize]);
    DbgMsg (lpszDebug);

    // Display the hash table
    for (lpHshEntry = lpHshTab, i = 0;
         i < iHshTabTotalSize;
         lpHshEntry++, i++)
    {
        WCHAR wszFlags[128] = {L'\0'};
        UINT  htFlags;

        // Format the flags
        htFlags = *(UINT *) &lpHshEntry->htFlags;
        for (j = 0;
             j < (sizeof (ahtFlagNames) / sizeof (ahtFlagNames[0]));
             j++)
        if (htFlags & ahtFlagNames[j].uMask)
            lstrcatW (wszFlags, ahtFlagNames[j].lpwszName);

        // In case we didn't find any matching flags,
        //   set the second byte to zero as well as
        //   when we do find flags, we skip over the
        //   leading blank.
        if (wszFlags[0] EQ L'\0')
            wszFlags[1] =  L'\0';

        if (lpHshEntry->htFlags.Inuse)
        {
            LPSYMENTRY lpSymEntry;

            lpSymEntry = lpHshEntry->lpSymEntry;
            if (lpSymEntry->stFlags.Imm)
                wsprintfW (lpwszDebug,
                           L"HT:%3d uH=%08X, uH&M=%d, <%s>, ull=%08X%08X, Sym=%08X",
                           i,
                           lpHshEntry->uHash,
                           lpHshEntry->uHashAndMask,
                           &wszFlags[1],
                           HIDWORD (lpSymEntry->stData.stInteger),
                           LODWORD (lpSymEntry->stData.stInteger),
                           lpSymEntry);
            else
            if (lpSymEntry->stFlags.UsrName
             || lpSymEntry->stFlags.SysName)
            {
                LPCHAR lpGlbName;

                lpGlbName = GlobalLock (lpHshEntry->hGlbName); Assert (lpGlbName NE NULL);

                wsprintfW (lpwszDebug,
                           L"HT:%3d uH=%08X, uH&M=%d, <%s>, <%s>, Sym=%08X, %08X-%08X",
                           i,
                           lpHshEntry->uHash,
                           lpHshEntry->uHashAndMask,
                           &wszFlags[1],
                           lpGlbName,
                           lpSymEntry,
                           lpHshEntry->NextSameHash,
                           lpHshEntry->PrevSameHash);
                // We no longer need this ptr
                GlobalUnlock (lpHshEntry->hGlbName); lpGlbName = NULL;
            } // End IF/ELSE/IF
        } else
            wsprintfW (lpwszDebug,
                       L"HT:%3d (EMPTY) <%s>, Sym=%08X, <%08X-%08X>",
                       i,
                       &wszFlags[1],
                       lpHshEntry->lpSymEntry,
                       lpHshEntry->NextSameHash,
                       lpHshEntry->PrevSameHash);
        DbgMsgW (lpwszDebug);
    } // End FOR

    DbgMsg ("********** End Hash Table ******************************");

    UpdateWindow (hWndDB);
} // End DisplayHshTab
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplaySymTab
//
//  Display the Symbol Table entries
//
//  If bDispAll is FALSE, display only those non-SysName entries
//    with a non-zero reference count.
//***************************************************************************

void DisplaySymTab
    (BOOL bDispAll)

{
    LPSYMENTRY lpSymEntry;
    int        i, j;

    typedef struct tagST_FLAGNAMES
    {
        UINT    uMask;
        LPWCHAR lpwszName;
    } ST_FLAGNAMES, *LPST_FLAGNAMES;

    // Symbol Table flag names
    static ST_FLAGNAMES astFlagNames[] =
    {
//// {0x000001,  L" Imm"        },
//// {0x00001E,  L" ImmType"    },
     {0x000020,  L" SysName"    },
     {0x000040,  L" SysVar"     },
     {0x000080,  L" SysFn0"     },
     {0x000100,  L" SysFn12"    },
     {0x000200,  L" NotCase"    },
     {0x000400,  L" Perm"       },
     {0x000800,  L" Inuse"      },
     {0x001000,  L" Value"      },
     {0x002000,  L" UsrName"    },
     {0x004000,  L" UsrVar"     },
     {0x008000,  L" UsrFn0"     },
     {0x010000,  L" UsrFn12"    },
     {0x020000,  L" UsrOp1"     },
     {0x040000,  L" UsrOp2"     },
//// {0x780000,  L" SysVarValid"},
    };

    if (bDispAll)
        DbgMsg ("********** Symbol Table ********************************");
    else
        DbgMsg ("********** Symbol Table Referenced Non-SysNames ******** ");

    wsprintf (lpszDebug,
              "lpSymTab = %08X, Last = %08X",
              lpSymTab,
              &lpSymTab[iSymTabTotalSize]);
    DbgMsg (lpszDebug);

    // Display the symbol table
    for (lpSymEntry = lpSymTab, i = 0;
         lpSymEntry NE lpSymTabNext;
         lpSymEntry++, i++)
    if (bDispAll ||
        !lpSymEntry->stFlags.SysName)
    {
        WCHAR   wszFlags[128] = {L'\0'};
        STFLAGS stFlags;
        LPWCHAR lpGlbName;

        // Format the flags
        stFlags = lpSymEntry->stFlags;
        if (stFlags.Imm)
            wsprintfW (&wszFlags[lstrlenW (wszFlags)],
                       L" Imm/Type=%d",
                       stFlags.ImmType);
        for (j = 0;
             j < (sizeof (astFlagNames) / sizeof (astFlagNames[0]));
             j++)
        if ((*(UINT *) &stFlags) & astFlagNames[j].uMask)
            lstrcatW (wszFlags, astFlagNames[j].lpwszName);

        if (stFlags.SysVar)
            wsprintfW (&wszFlags[lstrlenW (wszFlags)],
                       L" SysVarValid=%d",
                       stFlags.SysVarValid);

        // In case we didn't find any matching flags,
        //   set the second WCHAR to zero as well --
        //   when we do find flags, we skip over the
        //   leading blank.
        if (wszFlags[0] EQ L'\0')
            wszFlags[1] =  L'\0';

        if (lpSymEntry->stFlags.Inuse)
        {
            WCHAR wszName[128] = {'\0'};

            if (lpSymEntry->stFlags.UsrName
             || lpSymEntry->stFlags.SysName)
            {
                LPHSHENTRY lpHshEntry;

                lpHshEntry = lpSymEntry->lpHshEntry;

                if (lpHshEntry)
                {
                    lpGlbName = GlobalLock (lpHshEntry->hGlbName); Assert (lpGlbName NE NULL);

                    lstrcpynW (wszName, lpGlbName, sizeof (wszName) / sizeof (wszName[0]));

                    // We no longer need this ptr
                    GlobalUnlock (lpHshEntry->hGlbName); lpGlbName = NULL;
                } // End IF
            } // End IF

            if (lpSymEntry->stFlags.Imm)
            {
                wsprintfW (lpwszDebug,
                           L"ST:%08X <%s> <%s>, ull=%08X%08X, Hsh=%08X",
                           lpSymEntry,
                           wszName,
                           &wszFlags[1],
                           HIDWORD (lpSymEntry->stData.stInteger),
                           LODWORD (lpSymEntry->stData.stInteger),
                           lpSymEntry->lpHshEntry);
            } else
            if (lpSymEntry->stFlags.UsrName
             || lpSymEntry->stFlags.SysName)
            {
                LPHSHENTRY lpHshEntry;

                lpHshEntry = lpSymEntry->lpHshEntry;

                if (lpHshEntry)
                {
                    wsprintfW (lpwszDebug,
                               L"ST:%08X <%s>, <%s>, Data=%08X, Hsh=%08X",
                               lpSymEntry,
                               wszName,
                               &wszFlags[1],
                               lpSymEntry->stData.stVoid,
                               lpHshEntry);
                } else
                    wsprintfW (lpwszDebug,
                               L"ST:%08X <******>, <%s>, Hsh=0",
                               lpSymEntry,
                               &wszFlags[1]);
            } // End IF/ELSE/IF
        } else
            wsprintfW (lpwszDebug,
                       L"ST:%08X (EMPTY) <%s>, Hsh=%08X",
                       lpSymEntry,
                       &wszFlags[1],
                       lpSymEntry->lpHshEntry);
        DbgMsgW (lpwszDebug);
    } // End FOR

    DbgMsg ("********** End Symbol Table ****************************");

    UpdateWindow (hWndDB);
} // End DisplaySymTab
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayGlobals
//
//  Display outstanding global memory objects
//***************************************************************************
void DisplayGlobals
    (UINT uDispGlb)     // 0 = Display non-permanent non-sysvars
                        // 1 = ...     non-sysvars
                        // 2 = ...     all globals

{
    int       i;
    HGLOBAL   hGlb;
    LPVOID    lpMem;
    APLDIM    aplDim;
    LPVOID    lpData;
    APLCHAR   aplArrChar[19];
    LPAPLCHAR lpwsz;

    DbgMsg ("********** Globals *************************************");

    for (i = 0; i < MAXOBJ; i++)
    if (hGlb = ahGLOBAL[i])
    {
        lpMem = GlobalLock (hGlb); Assert (lpMem NE NULL);

        if (!lpMem)
        {
            wsprintf (lpszDebug,
                      "hGlb=%08X *** INVALID ***",
                      hGlb);
            DbgMsg (lpszDebug);

            continue;
        } // End IF

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)
        if (lpHeader->Sign.ature EQ VARARRAY_HEADER_SIGNATURE)
        {
            if (uDispGlb EQ 2
             || !lpHeader->SysVar)
            {
                // It's a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (MakeGlbTypeGlb (hGlb)));

                if (lpHeader->Rank EQ 0)
                    aplDim = (APLDIM) -1;
                else
                    aplDim = *VarArrayBaseToDim (lpHeader);
                // Skip over the header and dimension to the data
                lpData = VarArrayBaseToData (lpHeader, lpHeader->Rank);

                // Split cases based upon the array storage type
                switch (lpHeader->ArrType)
                {
                    case ARRAY_BOOL:
                    case ARRAY_INT:
                    case ARRAY_FLOAT:
                    case ARRAY_APA:
                        if (lpHeader->NELM EQ 0)
                        {
                            aplArrChar[0] = UTF16_ZILDE;
                            aplArrChar[1] = L'\0';
                        } else
                        {
                            lpwsz =
                            FormatImmed (aplArrChar,
                                         TranslateArrayTypeToImmType (lpHeader->ArrType),
                                         (LPAPLLONGEST) lpData);
                            // Delete the trailing blank
                            lpwsz[-1] = L'\0';
                        } // End IF/ELSE

                        break;

                    case ARRAY_HETERO:
                    case ARRAY_NESTED:
                        aplArrChar[0] = L'\0';

                        break;

                    case ARRAY_CHAR:
                        lstrcpynW (aplArrChar, lpData, 1 + (UINT) min (6, lpHeader->NELM));
                        aplArrChar[min (6, lpHeader->NELM)] = L'\0';

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Check for non-permanents
                if (uDispGlb EQ 1
                 || uDispGlb EQ 2
                 || !lpHeader->Perm)
                {
                    wsprintfW (lpwszDebug,
                               L"hGlb=%08X, ArrType=%c%c, NELM=%3d, RC=%1d, Rank=%2d, Dim1=%3d, Line#=%4d, (%s)",
                               hGlb,
                               L"BIFCHNLA"[lpHeader->ArrType],
                               L" *"[lpHeader->Perm],
                               LODWORD (lpHeader->NELM),
                               lpHeader->RefCnt,
                               LODWORD (lpHeader->Rank),
                               LODWORD (aplDim),
                               auLinNumGLOBAL[i],
                               aplArrChar);
                    DbgMsgW (lpwszDebug);
                } // End IF
            } // End IF
        } else
#undef  lpHeader
#define lpHeader    ((LPFCNARRAY_HEADER) lpMem)
        if (lpHeader->Sign.ature EQ FCNARRAY_HEADER_SIGNATURE)
        {
            // It's a valid HGLOBAL function array
            Assert (IsGlbTypeFcnDir (MakeGlbTypeGlb (hGlb)));

            wsprintf (lpszDebug,
                      "hGlb=%08X, FcnType=%1d,  NELM=%3d, RC=%1d,                    Line#=%4d",
                      hGlb,
                      lpHeader->FcnType,
                      LODWORD (lpHeader->NELM),
                      lpHeader->RefCnt,
                      auLinNumGLOBAL[i]);
            DbgMsg (lpszDebug);
        } else
#undef  lpHeader
        if (uDispGlb EQ 2)
        {
            wsprintf (lpszDebug,
                      "hGlb=%08X -- No NARS/FCNS Signature",
                      hGlb);
            DbgMsg (lpszDebug);
        } // End IF/ELSE

        // We no longer need this ptr
        GlobalUnlock (hGlb); lpMem = NULL;
    } // End FOR/IF

    DbgMsg ("********** End Globals *********************************");

    UpdateWindow (hWndDB);
} // End DisplayGlobals
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayTokens
//
//  Display the contents of the current token stream
//***************************************************************************

void DisplayTokens
    (HGLOBAL hGlbToken)

{
    LPTOKEN lpToken;
    int i, iLen;

    DbgMsg ("********** Tokens **************************************");

    // Ensure it's valid
    if (!hGlbToken)
    {
        DbgMsg ("DisplayTokens:  ***INAVLID HANDLE***:  hGlbToken == 0");
        return;
    } // End IF

    // Note we don't use MyGlobalLock & MyGlobalUnlock
    //   as they expect the lock count to be zero
    //   which need not be the way our caller left it.
    lpToken = GlobalLock (hGlbToken); Assert (lpToken NE NULL);

    // Ensure it's valid
    if (!lpToken)
    {
        DbgMsg ("DisplayTokens:  ***INAVLID HANDLE***:  GlobalLock failed");
        return;
    } // End IF

#define lpHeader    ((LPTOKEN_HEADER) lpToken)

    wsprintf (lpszDebug,
              "lpToken = %08X, Version # = %d, TokenCnt = %d, PrevGroup = %d",
              lpToken,
              lpHeader->Version,
              lpHeader->TokenCnt,
              lpHeader->PrevGroup);
    DbgMsg (lpszDebug);

    iLen = lpHeader->TokenCnt;

#undef  lpHeader

    lpToken = TokenBaseToStart (lpToken);   // Skip over TOKEN_HEADER

    for (i = 0; i < iLen; i++, lpToken++)
    {
        wsprintf (lpszDebug,
                  "(%2d) Data=%08X%08X, CharIndex=%2d, Type=%s",
                  i,
                  HIDWORD (*(LPAPLINT) &lpToken->tkData.tkFloat),
                  LODWORD (*(LPAPLINT) &lpToken->tkData.tkFloat),
                  lpToken->tkCharIndex,
                  GetTokenTypeName (lpToken->tkFlags.TknType));
        DbgMsg (lpszDebug);
    } // End FOR

    DbgMsg ("********** End Tokens **********************************");

    // We no longer need this ptr
    GlobalUnlock (hGlbToken); lpToken = NULL;
} // End DisplayTokens
#endif


#ifdef DEBUG
//***************************************************************************
//  $GetTokenTypeName
//
//  Convert a token type value to a name
//***************************************************************************

LPCHAR GetTokenTypeName
    (UINT uType)

{
typedef struct tagTOKENNAMES
{
    LPCHAR lpsz;
    UINT   uTokenNum;
} TOKENNAMES, *LPTOKENNAMES;

static TOKENNAMES tokenNames[] =
{{"VARNAMED"  , TKT_VARNAMED }, //  1: Symbol table entry for a named var (data is LPSYMENTRY)
 {"STRING"    , TKT_STRING   }, //  2: String  (data is HGLOBAL)
 {"VARIMMED"  , TKT_VARIMMED }, //  3: Immediate data (data is immediate)
 {"COMMENT"   , TKT_COMMENT  }, //  4: Comment (data is HGLOBAL)
 {"ASSIGN"    , TKT_ASSIGN   }, //  5: Assignment symbol (data is UTF16_LEFTARROW)
 {"LISTSEP"   , TKT_LISTSEP  }, //  6: List separator    (data is ';')
 {"FCNIMMED"  , TKT_FCNIMMED }, //  7: Primitive function (any valence) (data is UTF16_***)
 {"OP1IMMED"  , TKT_OP1IMMED }, //  8: Monadic primitive operator (data is UTF16_***)
 {"OP2IMMED"  , TKT_OP2IMMED }, //  9: Dyadic  ...
 {"OPJOTDOT"  , TKT_OPJOTDOT }, // 10: Outer product monadic operator (with right scope) (data is NULL)
 {"LPAREN"    , TKT_LPAREN   }, // 11: Left paren (data is TKT_LPAREN)
 {"RPAREN"    , TKT_RPAREN   }, // 12: Right ...   ...         RPAREN
 {"LBRACKET"  , TKT_LBRACKET }, // 13: Left bracket ...        LBRACKET
 {"RBRACKET"  , TKT_RBRACKET }, // 14: Right ...   ...         RBRACKET
 {"EOS"       , TKT_EOS      }, // 15: End-of-Stmt (data is length of stmt including this token)
 {"EOL"       , TKT_EOL      }, // 16: End-of-Line  ...
 {"LINECONT"  , TKT_LINECONT }, // 17: Line continuation (data is NULL)
 {"INPOUT"    , TKT_INPOUT   }, // 18: Input/Output (data is UTF16_QUAD or UTF16_QUOTEQUAD)
 {"STRAND"    , TKT_STRAND   }, // 19: Strand accumulating (data is LPTOKEN)
 {"LISTINT"   , TKT_LISTINT  }, // 20: List in parens    (data is HGLOBAL)
 {"LISTPAR"   , TKT_LISTPAR  }, // 21: List in parens    (data is HGLOBAL)
 {"LISTBR"    , TKT_LISTBR   }, // 22: List in brackets  (data is HGLOBAL)
 {"VARARRAY"  , TKT_VARARRAY }, // 23: Array of data (data is HGLOBAL)
 {"FCNARRAY"  , TKT_FCNARRAY }, // 24: Array of functions (data is HGLOBAL)
 {"FCNNAMED"  , TKT_FCNNAMED }, // 25: Symbol table entry for a named function (data is LPSYMENTRY)
 {"AXISIMMED" , TKT_AXISIMMED}, // 26: An immediate axis specification (data is immediate)
 {"AXISARRAY" , TKT_AXISARRAY}, // 27: An array of  ...   (data is HGLOBAL)
 {"OP1NAMED"  , TKT_OP1NAMED }, // 28: A named monadic primitive operator (data is LPSYMENTRY)
 {"OP2NAMED"  , TKT_OP2NAMED }, // 29: ...     dyadic  ...
 {"STRNAMED"  , TKT_STRNAMED }, // 30: ...     strand  ...
};

    if ((sizeof (tokenNames) / sizeof (tokenNames[0])) > (uType - TKT_FIRST))
        return tokenNames[uType - TKT_FIRST].lpsz;
    else
    {
        static char szTemp[64];

        wsprintf (szTemp,
                  "GetTokenTypeName:  *** Unknown Token Type:  %d",
                  uType);
        return szTemp;
    } // End IF/ELSE
} // End GetTokenTypeName
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayFcnStrand
//
//  Display a function strand
//***************************************************************************

void DisplayFcnStrand
    (LPTOKEN lpToken)

{
    HGLOBAL hGlbData;
    LPWCHAR lpaplChar = lpwszDebug;

    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:          // Because we don't distinguish between
                                    //   functions and variables in AssignName_EM
            return;

        case TKT_FCNIMMED:
        case TKT_OP1IMMED:
        case TKT_OP2IMMED:
            lpaplChar += wsprintfW (lpaplChar,
                                    L"FcnType=%1d, NELM=%3d, RC=%2d, Fn:  ",
                                    FCNTYPE_FCN,// lpHeader->FcnType,
                                    1,          // LODWORD (lpHeader->NELM),
                                    0);         // lpHeader->RefCnt);
            *lpaplChar++ = lpToken->tkData.tkChar;

            break;

        case TKT_FCNNAMED:
        case TKT_OP1NAMED:
        case TKT_OP2NAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, ...
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                hGlbData = lpToken->tkData.tkSym->stData.stGlbData;

                // stData is an HGLOBAL function array
                Assert (IsGlbTypeFcnDir (hGlbData));

                lpaplChar = DisplayFcnGlb (lpaplChar, ClrPtrTypeDirGlb (hGlbData), TRUE);
            } else
            {
                lpaplChar += wsprintfW (lpaplChar,
                                        L"FcnType=%1d, NELM=%3d, RC=%2d, Fn:  ",
                                        FCNTYPE_FCN,// lpHeader->FcnType,
                                        1,          // LODWORD (lpHeader->NELM),
                                        0);         // lpHeader->RefCnt);
                *lpaplChar++ = lpToken->tkData.tkSym->stData.stChar;
            } // End IF/ELSE

            break;

        case TKT_FCNARRAY:
            hGlbData = lpToken->tkData.tkGlbData;

            // tkData is an HGLOBAL function array
            Assert (IsGlbTypeFcnDir (hGlbData));

            lpaplChar = DisplayFcnGlb (lpaplChar, ClrPtrTypeDirGlb (hGlbData), TRUE);

            break;

        defstop
            break;
    } // End SWITCH

    // Ensure properly terminated
    *lpaplChar = L'\0';

    // Display the line in the debugging window
    DbgMsgW (lpwszDebug);
} // End DisplayFcnStrand
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayFcnGlb
//
//  Display a function from an HGLOBAL
//***************************************************************************

LPWCHAR DisplayFcnGlb
    (LPWCHAR lpaplChar,
     HGLOBAL hGlb,
     BOOL    bDispHeader)

{
    LPVOID    lpMem;
    APLNELM   aplNELM;
    LPYYSTYPE lpYYMem;

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlb);

#define lpHeader    ((LPFCNARRAY_HEADER) lpMem)
    aplNELM = lpHeader->NELM;
    if (bDispHeader)
        lpaplChar += wsprintfW (lpaplChar,
////////////////////////////////L"FcnType=%1d, NELM=%08X%08X, RC=%2d, Fn:  ",
                                L"FcnType=%1d, NELM=%3d, RC=%2d, Fn:  ",
                                lpHeader->FcnType,
////////////////////////////////HIDWORD (lpHeader->NELM),
                                LODWORD (lpHeader->NELM),
                                lpHeader->RefCnt);
#undef  lpHeader

    // Skip over the header to the data
    lpYYMem = FcnArrayBaseToData (lpMem);

    lpaplChar = DisplayFcnSub (lpaplChar, lpYYMem, aplNELM);

////switch (aplNELM)
////{
////    case 2:                 // e.g., {fn}{op1} or {fn}{jotdot}
////        // Handle {jotdot} in reverse
////        if (((LPYYSTYPE) lpMem)[1].tkToken.tkFlags.TknType EQ TKT_OPJOTDOT)
////        {
////            lpaplChar =
////              DisplayFcnSub (lpaplChar, &((LPYYSTYPE) lpMem)[1].tkToken);
////            lpaplChar =
////              DisplayFcnSub (lpaplChar, &((LPYYSTYPE) lpMem)[0].tkToken;
////        } else
////        {
////            lpaplChar =
////              DisplayFcnSub (lpaplChar, &((LPYYSTYPE) lpMem)[0].tkToken);
////            lpaplChar =
////              DisplayFcnSub (lpaplChar, &((LPYYSTYPE) lpMem)[1].tkToken);
////        } // End IF/ELSE
////
////        break;
////
////    case 3:                 // e.g., {fn}{op2}{fn}
////        lpaplChar =
////          DisplayFcnSub (lpaplChar, &((LPYYSTYPE) lpMem)[0].tkToken);
////        lpaplChar =
////          DisplayFcnSub (lpaplChar, &((LPYYSTYPE) lpMem)[1].tkToken);
////        lpaplChar =
////          DisplayFcnSub (lpaplChar, &((LPYYSTYPE) lpMem)[2].tkToken);
////
////        break;
////
////    defstop
////        break;
////} // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlb); lpMem = lpYYMem = NULL;

    return lpaplChar;
} // End DisplayFcnGlb
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayFcnSub
//
//  Display function subroutine
//***************************************************************************

LPWCHAR DisplayFcnSub
    (LPWCHAR   lpaplChar,
     LPYYSTYPE lpYYMem,
     APLNELM   aplNELM)

{
    HGLOBAL hGlbData;

    // Split cases based upon the token type
    switch (lpYYMem[0].tkToken.tkFlags.TknType)
    {
        case TKT_OP1IMMED:
            // Check for axis operator
            if (aplNELM > 1
             && (lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
              || lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
            {
                if (aplNELM > 2)
                    lpaplChar =
                      DisplayFcnSub (lpaplChar, &lpYYMem[2], aplNELM - 2);  // fcn
                *lpaplChar++ = lpYYMem[0].tkToken.tkData.tkChar;            // Op1
                lpaplChar =
                  DisplayFcnSub (lpaplChar, &lpYYMem[1], 1);                // [X]
            } else
            {
                lpaplChar =
                  DisplayFcnSub (lpaplChar, &lpYYMem[1], aplNELM - 1);      // fcn
                *lpaplChar++ = lpYYMem[0].tkToken.tkData.tkChar;            // Op1
            } // End IF/ELSE/...

            break;

        case TKT_OP2IMMED:
            lpaplChar =
              DisplayFcnSub (lpaplChar, &lpYYMem[1], lpYYMem[1].FcnCount);  // Lfcn
            *lpaplChar++ = lpYYMem[0].tkToken.tkData.tkChar;                // Op2
            if (lpYYMem[2].FcnCount > 1)
                *lpaplChar++ = L'(';
            lpaplChar =
              DisplayFcnSub (lpaplChar, &lpYYMem[2], lpYYMem[2].FcnCount);  // Rfcn
            if (lpYYMem[2].FcnCount > 1)
                *lpaplChar++ = L')';
            break;

        case TKT_FCNIMMED:
            *lpaplChar++ = lpYYMem[0].tkToken.tkData.tkChar;                // fcn

            // Check for axis operator
            if (aplNELM > 1
             && (lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
              || lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
            {
                lpaplChar =
                  DisplayFcnSub (lpaplChar, &lpYYMem[1], 1);                // [X]
            } // End IF

            break;

        case TKT_OP1NAMED:
            DbgBrk ();          // ***FINISHME***






            break;

        case TKT_OP2NAMED:
            DbgBrk ();          // ***FINISHME***





            break;

        case TKT_FCNNAMED:
            DbgBrk ();          // ***TESTME***

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpYYMem->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not an immediate function, ...
            if (!lpYYMem->tkToken.tkData.tkSym->stFlags.Imm)
            {
                hGlbData = lpYYMem->tkToken.tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL function array
                Assert (IsGlbTypeFcnDir (hGlbData));

                // Display the function strand in global memory
                lpaplChar = DisplayFcnGlb (lpaplChar, ClrPtrTypeDirGlb (hGlbData), FALSE);
            } else
                // Handle the immediate case
                *lpaplChar++ = lpYYMem->tkToken.tkData.tkSym->stData.stChar;

            // Check for axis operator
            if (aplNELM > 1
             && (lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
              || lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
            {
                lpaplChar =
                  DisplayFcnSub (lpaplChar, &lpYYMem[1], 1);            // [X]
            } // End IF

            break;

        case TKT_OPJOTDOT:
            *lpaplChar++ = UTF16_JOT;
            *lpaplChar++ = L'.';

            // Surround with parens if more than one token
            if (aplNELM > 2)
                *lpaplChar++ = L'(';
            lpaplChar =
              DisplayFcnSub (lpaplChar, &lpYYMem[1], aplNELM - 1);
            if (aplNELM > 2)
                *lpaplChar++ = L')';

            break;

        case TKT_AXISIMMED:
            *lpaplChar++ = L'[';
            lpaplChar =
              FormatAplint (lpaplChar,
                            lpYYMem[0].tkToken.tkData.tkInteger);
            lpaplChar[-1] = L']';   // Overwrite the trailing blank

            break;

        case TKT_AXISARRAY:
            *lpaplChar++ = L'[';

            hGlbData = lpYYMem->tkToken.tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (hGlbData));
#if TRUE
            lstrcpyW (lpaplChar, L"TXT_AXISARRAY ");    // N.B.:  trailing blank is significant
            lpaplChar += lstrlenW (lpaplChar);
#else
            lpaplChar = FormatGlobal (lpaplChar, ClrPtrTypeDirGlb (hGlbData));
#endif
            lpaplChar[-1] = L']';   // Overwrite the trailing blank

            break;

        case TKT_VARIMMED:
            lpaplChar =
              FormatAplint (lpaplChar,
                            lpYYMem[0].tkToken.tkData.tkInteger);
            lpaplChar[-1] = L'\0';  // Overwrite the trailing blank

            break;

        case TKT_VARARRAY:
            hGlbData = lpYYMem->tkToken.tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (hGlbData));
#if TRUE
            lstrcpyW (lpaplChar, L"TXT_VARARRAY ");     // N.B.:  trailing blank is significant
            lpaplChar += lstrlenW (lpaplChar);
#else
            lpaplChar =
              FormatGlobal (lpaplChar, ClrPtrTypeDirGlb (hGlbData));
#endif
            lpaplChar[-1] = L'\0';  // Overwrite the trailing blank

            break;

        case TKT_VARNAMED:
            DbgBrk ();      // ***FINISHME***











            break;

        defstop
            break;
    } // End SWITCH

    return lpaplChar;
} // End DisplayFcnSub
#endif


//// #ifdef DEBUG
//// //***************************************************************************
//// //  DisplayFcnSub
//// //
//// //  Display function subroutine
//// //***************************************************************************
////
//// LPWCHAR DisplayFcnSub
////     (LPWCHAR lpaplChar,
////      LPTOKEN lpToken,
////      BOOL    bParensIfGlb)
////
//// {
////     // Split cases based upon the token type
////     switch (lpToken->tkFlags.TknType)
////     {
////         case TKT_OPJOTDOT:
////             *lpaplChar++ = UTF16_JOT;
////             *lpaplChar++ = L'.';
////
////             break;
////
////         case TKT_FCNIMMED:
////         case TKT_OP1IMMED:
////         case TKT_OP2IMMED:
////         case TKT_LPAREN:
////         case TKT_RPAREN:
////             *lpaplChar++ = lpToken->tkData.tkChar;
////
////             break;
////
////         case TKT_FCNNAMED:
////             // tkData is an LPSYMENTRY
////             Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);
////
////             // If it's an immediate
////             if (lpToken->tkData.tkSym->stFlags.Imm)
////                 *lpaplChar++ = lpToken->tkData.tkSym->stData.stChar;
////             else
////             {
////                 // stData is a valid HGLOBAL function array
////                 Assert (IsGlbTypeFcnDir (lpToken->tkData.tkSym->stData.stGlbData));
////
////                 if (bParensIfGlb)
////                     *lpaplChar++ = L'(';
////
////                 lpaplChar =
////                 DisplayFcnGlb (lpaplChar,
////                                ClrPtrTypeDirGlb (lpToken->tkData.tkSym->stData.stGlbData),
////                                FALSE);
////                 if (bParensIfGlb)
////                     *lpaplChar++ = L')';
////             } // End IF/ELSE
////
////             break;
////
////         case TKT_FCNARRAY:
////             // tkData is a valid HGLOBAL function array
////             Assert (IsGlbTypeFcnDir (lpToken->tkData.tkGlbData));
////
////             *lpaplChar++ = L'(';
////
////             lpaplChar =
////             DisplayFcnGlb (lpaplChar,
////                            ClrPtrTypeDirGlb (lpToken->tkData.tkGlbData),
////                            FALSE);
////             *lpaplChar++ = L')';
////
////             break;
////
////         defstop
////             break;
////     } // End SWITCH
////
////     return lpaplChar;
//// } // End DisplayFcnSub
//// #endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayStrand
//
//  Display the strand stack
//***************************************************************************

void DisplayStrand
    (int strType)               // Strand type (VARSTRAND or FCNSTRAND)

{
    LPYYSTYPE lp,
              lpLast;
    LPPLLOCALVARS lpplLocalVars;

    // Check debug level
    if (gDbgLvl <= 2)
        return;

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsPlLocalVars);

    switch (strType)
    {
        case VARSTRAND:
            DbgMsg ("********** Variable Strands ****************************");

            break;

        case FCNSTRAND:
            DbgMsg ("********** Function Strands ****************************");

            break;

        defstop
            break;
    } // End SWITCH

    wsprintf (lpszDebug,
              "Start=%08X Base=%08X Next=%08X",
              lpplLocalVars->lpYYStrandStart[strType],
              lpplLocalVars->lpYYStrandBase[strType],
              lpplLocalVars->lpYYStrandNext[strType]);
    DbgMsg (lpszDebug);

    for (lp = lpplLocalVars->lpYYStrandStart[strType], lpLast = NULL;
         lp NE lpplLocalVars->lpYYStrandNext[strType];
         lp ++)
    {
        if (lpLast NE lp->unYYSTYPE.lpYYStrandBase)
        {
            DbgMsg ("--------------------------------------------------------");
            lpLast = lp->unYYSTYPE.lpYYStrandBase;
        } // End IF

        wsprintf (lpszDebug,
                  "Strand (%08X): %-9.9s D=%08X CI=%2d TC=%1d FC=%1d IN=%1d F=%08X B=%08X",
                  lp,
                  GetTokenTypeName (lp->tkToken.tkFlags.TknType),
                  LODWORD (lp->tkToken.tkData.tkInteger),
                  lp->tkToken.tkCharIndex,
                  lp->TknCount,
                  lp->FcnCount,
                  lp->Indirect,
                  lp->lpYYFcn,
                  lpLast);
        DbgMsg (lpszDebug);
    } // End FOR

    DbgMsg ("********** End Strands *********************************");
} // End DisplayStrand
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayUndo
//
//  Display the Undo Buffer
//***************************************************************************

void DisplayUndo
    (HWND hWnd)         // Window handle of the Edit Control

{
    UINT      uCharPos,
              uLineCount;
    HGLOBAL   hGlbEC;
    LPWCHAR   lpwsz, p;
    HWND      hWndParent;
    LPUNDOBUF lpUndoBeg,    // Ptr to start of Undo Buffer
              lpUndoNxt;    // ...    next available slot in the Undo Buffer
    BOOL      bShift;
    static LPWCHAR Actions[]={L"None",
                              L"Ins",
                              L"Rep",
                              L"Del",
                              L"Sel",
                              L"Back",
                              L"InsToggle"};

    DbgMsg ("********** Undo Buffer *********************************");

    // Get the shift key state
    bShift = (GetKeyState (VK_SHIFT) & 0x8000);

    // Get the char position of the caret
    uCharPos = GetCurCharPos (hWnd);

    // Get the lines in the text
    uLineCount = SendMessageW (hWnd, EM_GETLINECOUNT, 0, 0);

    // Get the Edit Control's memory handle
    (long) hGlbEC = SendMessageW (hWnd, EM_GETHANDLE, 0, 0);

    // Display it
    dprintf ("Caret position = %d, # lines = %d, hGlbEC = %08X",
             uCharPos,
             uLineCount,
             hGlbEC);

    // Lock the memory to get a ptr to it
    lpwsz = MyGlobalLock (hGlbEC);

#define VIS_CR  L'\xAE'
#define VIS_NL  L'\xA9'
#define VIS_HT  L'\xBB'

    // Replace L'\r' with a visible char
    while (p = strchrW (lpwsz, L'\r'))
        *p = VIS_CR;

    // Replace L'\n' with a visible char
    while (p = strchrW (lpwsz, L'\n'))
        *p = VIS_NL;

    // Replace L'\t' with a visible char
    while (p = strchrW (lpwsz, L'\t'))
        *p = VIS_HT;

    // Display it
    dprintfW (L"Text = <%S>",
              lpwsz);

    // Restore L'\t'
    while (p = strchrW (lpwsz, VIS_HT))
        *p = L'\t';

    // Restore L'\n'
    while (p = strchrW (lpwsz, VIS_NL))
        *p = L'\n';

    // Restore L'\r'
    while (p = strchrW (lpwsz, VIS_CR))
        *p = L'\r';

    if (bShift)
        DbgBrk ();

    // We no longer need this ptr
    MyGlobalUnlock (hGlbEC); lpwsz = NULL;

    // Get the parent window handle
    hWndParent = GetParent (hWnd);

    // Get the ptrs to the next available slot in our Undo Buffer
    (long) lpUndoNxt = GetWindowLong (hWndParent, GWLSF_UNDO_NXT);
    (long) lpUndoBeg = GetWindowLong (hWndParent, GWLSF_UNDO_BEG);

    // Loop through the undo buffer entries
    for (; lpUndoBeg < lpUndoNxt; lpUndoBeg++)
    {
        wsprintfW (lpwszDebug,
                   L"Act = %9s, %2d-%2d, Group = %3d, Char = 0x%04X",
                   Actions[lpUndoBeg->Action],
                   lpUndoBeg->CharPosBeg,
                   lpUndoBeg->CharPosEnd,
                   lpUndoBeg->Group,
                   lpUndoBeg->Char);
        DbgMsgW (lpwszDebug);
    } // End FOR

    DbgMsg ("********** End Undo Buffer *****************************");
} // End DisplayUndo
#endif


//***************************************************************************
//  End of File: display.c
//***************************************************************************
