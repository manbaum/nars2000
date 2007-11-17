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

            // Check for NoValue
            if (IsSymNoValue (lptkRes->tkData.tkSym))
                return TRUE;

            // If it's not immediate, it's an array
            if (!lptkRes->tkData.tkSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (lptkRes->tkData.tkSym->stData.stVoid));

                // Check for NoDisplay flag
                if (!lptkRes->tkFlags.NoDisplay)
                    DisplayGlbArr (ClrPtrTypeDirAsGlb (lptkRes->tkData.tkSym->stData.stGlbData),
                                   bEndingCR);
                return TRUE;
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
                    DisplayGlbArr (ClrPtrTypeDirAsGlb (lptkRes->tkData.tkGlbData),
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
    LPVOID       lpMem;
    LPAPLCHAR    lpaplCharIni = NULL,
                 lpaplChar,
                 lpaplCharStart,
                 lpwsz;
    APLSTYPE     aplType;           // Arg storage type
    APLNELM      aplNELM;           // Arg NELM
    APLRANK      aplRank;           // Arg rank
    LPAPLDIM     lpMemDim;          // Ptr to arg dimensions
    APLDIM       aplDimNCols,       // # cols
                 aplDimNRows,       // # rows
                 aplChrNCols,       // # cols for char arrays
                 aplDimCol,         // Col loop counter
                 aplLastDim;        // Length of the last dimension in the result
    APLNELM      aplNELMRes;        // Result NELM
    LPFMTHEADER  lpFmtHeader;       // Ptr to format header struc
    LPFMTCOLSTR  lpFmtColStr;       // Ptr to format col struc
    APLUINT      uQuadPP,           // []PP
                 uQuadPW;           // []PW
    BOOL         bLineCont = FALSE, // TRUE iff this line is a continuation
                 bRawOut;           // TRUE iff using raw output
    APLUINT      uOutLen;           // Output length for this line

#define DEF_DISPLAY_INITSIZE    (64*1024)
#define DEF_DISPLAY_MAXSIZE     (64*1024)

    // Get the current value of []PP & PW
    uQuadPP = GetQuadPP ();
    uQuadPW = GetQuadPW ();

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
    // Get the Array Type, NELM, and Rank
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

    // Format char arrays as one col
    aplChrNCols = (aplType EQ ARRAY_CHAR) ? 1 : aplDimNCols;

    // Create a new FMTHEADER
    ZeroMemory ((LPFMTHEADER) lpaplCharIni, sizeof (FMTHEADER));
////((LPFMTHEADER) lpaplCharIni)->lpFmtHeadUp = NULL;  // Filled in by ZeroMemory
    lpFmtHeader = (LPFMTHEADER) lpaplCharIni;
#ifdef DEBUG
    lpFmtHeader->Sig.nature  = FMTHEADER_SIGNATURE;
#endif
////lpFmtHeader->lpFmtRowUp  = NULL;                // Already zero from ZeroMemory
////lpFmtHeader->lpFmtColUp  = NULL;                // ...
////lpFmtHeader->lpFmtRow1st =                      // Filled in below
////lpFmtHeader->lpFmtCol1st =                      // ...
    lpFmtHeader->uRealRows   = (UINT) aplDimNRows;  // # real rows from dimensions
////lpFmtHeader->uActRows    = 0;                   // Already zero from ZeroMemory (initial value)
    lpFmtHeader->uActCols    = (UINT) aplChrNCols;
////lpFmtHeader->uFmtRows    = 0;                   // Already zero from ZeroMemory (initial value)
////lpFmtHeader->uFmtInts    = 0;                   // ...
////lpFmtHeader->uFmtChrs    = 0;                   // ...
////lpFmtHeader->uFmtFrcs    = 0;                   // ...
////lpFmtHeader->uFmtTrBl    = 0;                   // ...
////lpFmtHeader->uDepth      = 0;                   // ...
////lpFmtHeader->uMatRes     = 0;                   // ...

    // Create <aplChrNCols> FMTCOLSTRs in the output
    lpFmtColStr = (LPFMTCOLSTR) (&lpFmtHeader[1]);
    lpFmtHeader->lpFmtCol1st = lpFmtColStr;
    ZeroMemory (lpFmtColStr, (UINT) aplChrNCols * sizeof (FMTCOLSTR));
#ifdef DEBUG
    {
        APLDIM uCol;

        for (uCol = 0; uCol < aplChrNCols; uCol++)
            lpFmtColStr[uCol].Sig.nature = FMTCOLSTR_SIGNATURE;
    }
#endif
    // Skip over the FMTCOLSTRs
    lpaplCharStart = (LPAPLCHAR) (&lpFmtColStr[aplChrNCols]);

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
                               TRUE);                   // TRUE iff top level array
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
                               TRUE);                   // TRUE iff top level array
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
                               TRUE);                   // TRUE iff top level array
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
                               TRUE);                   // TRUE iff top level array
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
                               TRUE);                   // TRUE iff top level array
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
                               TRUE,                    // TRUE iff top level array
                               TRUE);                   // TRUE iff handle []TCLF specially
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
                               TRUE,                    // TRUE iff top level array
                               TRUE,                    // TRUE iff first (leftmost) col
                               TRUE);                   // TRUE iff last (rightmost) col
            break;

        defstop
            break;
    } // End SWITCH

    // Propagate the row & col count up the line
    PropagateRowColCount (lpFmtHeader);

    // lpaplCharStart now contains the compiled version of the output

    // Add up the width of each column to get the
    //   # cols in the result
    for (aplLastDim = aplDimCol = 0; aplDimCol < aplChrNCols; aplDimCol++)
        aplLastDim += ExteriorColWidth (&lpFmtColStr[aplDimCol]);
    Assert (aplLastDim EQ ExteriorHdrWidth (lpFmtHeader));

    // Calculate the NELM of the result
    aplNELMRes = lpFmtHeader->uFmtRows * ExteriorHdrWidth (lpFmtHeader);

#ifdef PREFILL
    // Fill the output area with all zeros
    if (aplType EQ ARRAY_CHAR)
        ZeroMemory (lpwszFormat, (UINT) aplNELMRes * sizeof (APLCHAR));
    else
    {
        // Fill the output area with all blanks
        Assert (aplNELMRes EQ (UINT) aplNELMRes);
        FillMemoryW (lpwszFormat, (UINT) aplNELMRes, L' ');
    } // End IF/ELSE
#endif
    lpaplChar = lpwszFormat;

    // Run through the array again processing the
    //   output stream into lpwszFormat

    // Calc when to use raw output
    bRawOut = (aplType NE ARRAY_CHAR && aplType NE ARRAY_NESTED);

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
                             lpFmtColStr,           // Ptr to vector of <aplChrNCols> FMTCOLSTRs
                             lpaplCharStart,        // Ptr to compiled input
                            &lpaplChar,             // Ptr to output string
                             lpFmtHeader->uActRows, // # formatted rows in this array
                             aplDimNCols,           // # formatted cols in this array
                             aplLastDim,            // Length of last dim in result (NULL for !bRawOutput)
                             aplRank,               // Rank of this array
                             lpMemDim,              // Ptr to this array's dimensions
                             aplType,               // Storage type of this array
                             TRUE,                  // TRUE iff skip to next row after this item
                             bRawOut,               // TRUE iff raw (not {thorn}) output
                             bEndingCR);            // TRUE iff last line has CR
            break;

        case ARRAY_NESTED:
////////////lpaplChar =
            FormatArrNested (lpFmtHeader,           // Ptr to FMTHEADER
                             lpMem,                 // Ptr to raw input
                             lpFmtColStr,           // Ptr to vector of <aplDimNCols> FMTCOLSTRs
                             lpaplCharStart,        // Ptr to compiled input
                            &lpaplChar,             // Ptr to ptr to output string
                             lpFmtHeader->uActRows, // # formatted rows in this array
                             aplDimNCols,           // # formatted cols ...
                             aplRank,               // Rank of this array
                             lpMemDim,              // Ptr to this array's dimensions
                             aplLastDim,            // Length of last dim in result (NULL for !bRawOutput)
                             bRawOut,               // TRUE iff raw (not {thorn}) output
                             bEndingCR);            // TRUE iff last line has CR
            break;

        defstop
            break;
    } // End SWITCH

    // If we didn't use raw output in the
    //   FormatArrxxx routines, do it now
    // The following code handles wrapping at []PW
    if (!bRawOut)
    {
        UINT uFmtRow;               // Loop counter

        uOutLen = uQuadPW;          // Initial output length

        // Loop through the formatted rows
        for (lpwsz = lpwszFormat,
               uFmtRow = 0;
             uFmtRow < lpFmtHeader->uFmtRows;
             uFmtRow++,
               lpwsz += aplLastDim)
        {
            WCHAR   wch;                // The replaced WCHAR
            APLDIM  aplDimTmp;          // Remaining line length to output
            APLUINT uOffset;            // Offset in line to start of display

            // ***FIXME*** -- this routine may split a number in half
            //                because it doesn't know the difference
            //                numbers and characters

            if (bLineCont)
                AppendLine (wszIndent, bLineCont, FALSE);   // Display the indent

            // For char lines with embedded []TCLFs, the actual line
            //   length is smaller than the total length, so we need
            //   to compare against that here
            aplDimTmp = lstrlenW (lpwsz);   // Get line length
            aplDimTmp = min (aplDimTmp, aplLastDim);// Use the smaller
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
    } // End IF

    // If this is an empty vector, make sure it skips a line
    if (lpFmtHeader->uFmtRows EQ 0
     && aplRank EQ 1)
        AppendLine (L"", FALSE, TRUE);// Display the empty line
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
////UINT  uRht;

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
                    MessageBeep (NEG1U);    // Sound the alarum!

                    // Fall through to common code

                case TCDEL:     // Del
                case TCESC:     // Esc
                case TCFF:      // FF
                case TCNUL:     // NUL
                case TCBS:      // BS
                case TCNL:      // NL
                    *lpaplChar++ = L' ';    // Append a blank to be deleted

                    break;          // Can't go any farther left

                case TCHT:      // HT
                    // We're always at the (virtual) left margin,
                    //   so insert enough blanks for a TAB
                    lpaplChar = FillMemoryW (lpaplChar, uTabs + 1, L' ');
////////////////////*lpaplChar++ = L' ';    // Append a blank to be deleted

                    break;

                case TCLF:      // LF       // Handled during raw output
                    *lpaplChar++ = wc;      // Append it
                    *lpaplChar++ = L' ';    // Append a blank to be deleted

                    break;

                default:
                    if (wc >= 32)           // If none of the above but printable,
                        *lpaplChar++ = wc;  //   append it

                    *lpaplChar++ = L' ';    // Append a blank to be deleted

                    break;
            } // End SWITCH

            break;

        case IMMTYPE_FLOAT:
            lpaplChar =
            FormatFloat (lpaplChar,                 // Prt to output save area
                         *(LPAPLFLOAT) lpaplLongest,// The value to format
                         0);                        // Use default precision
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
    (LPWCHAR  lpaplChar,        // Ptr to output save area
     APLFLOAT fFloat,           // The value to format
     APLUINT  uPrecision)       // Precision to use (0 = default)

{
    APLUINT uQuadPP;            // []PP

    if (uPrecision EQ 0)
        // Get the current value of []PP
        uQuadPP = GetQuadPP ();
    else
        uQuadPP = uPrecision;

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


//***************************************************************************
//  End of File: display.c
//***************************************************************************
