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
#include "dfnhdr.h"
#include "pertab.h"

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
                    DisplayGlbArr (ClrPtrTypeDirGlb (lptkRes->tkData.tkSym->stData.stGlbData),
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
    UINT         uCol;              // Loop counter
    HGLOBAL      hGlbPTD;           // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    APLUINT      uQuadPP,           // []PP
                 uQuadPW;           // []PW

#define DEF_DISPLAY_INITSIZE    65536
#define DEF_DISPLAY_MAXSIZE     65536

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    uQuadPP = lpMemPTD->lpSymQuadPP->stData.stInteger;
    uQuadPW = lpMemPTD->lpSymQuadPW->stData.stInteger;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

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

    // Format char arrays as one col
    aplChrNCols = (aplType EQ ARRAY_CHAR) ? 1 : aplDimNCols;

    // Create a new FMTHEAD in the output
    ZeroMemory ((LPFMTHEADER) lpaplCharIni, sizeof (FMTHEADER));
////((LPFMTHEADER) lpaplCharIni)->lpFmtHeadUp = NULL;  // Filled in by ZeroMemory
    lpFmtHeader = (LPFMTHEADER) lpaplCharIni;
#ifdef DEBUG
    lpFmtHeader->Sig.nature  = FMTHEADER_SIGNATURE;
#endif
////lpFmtHeader->lpFmtRowUp  = NULL;                // Filled in by ZeroMemory
////lpFmtHeader->lpFmtColUp  = NULL;                // ...
////lpFmtHeader->lpFmtRow1st =                      // Filled in below
////lpFmtHeader->lpFmtCol1st =                      // ...
    lpFmtHeader->uRows       = (UINT) aplDimNRows;
    lpFmtHeader->uCols       = (UINT) aplChrNCols;
////lpFmtHeader->uFmtRows    = 0;                   // Filled in by ZeroMemory
////lpFmtHeader->uFmtInts    = 0;                   // ...
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
    for (aplLastDim = aplDimCol = 0; aplDimCol < aplChrNCols; aplDimCol++)
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
                             lpFmtColStr,           // Ptr to vector of <aplChrNCols> FMTCOLSTRs
                             lpaplCharStart,        // Ptr to compiled input
                            &lpaplChar,             // Ptr to output string
                             aplDimNRows,           // # rows in this array
                             aplDimNCols,           // # cols in this array
                             aplLastDim,            // Length of last dim in result (NULL for !bRawOutput)
                             aplRank,               // Rank of this array
                             lpMemDim,              // Ptr to this array's dimensions
                             aplType,               // Storage type of thsi array
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
    UINT  uRht;

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
                    for (uRht = 0; uRht < uTabs; uRht)
                        *lpaplChar++ = L' ';
                    *lpaplChar++ = L' ';    // Append a blank to be deleted

                    break;

                case TCLF:      // LF
                    DbgBrk ();  // ***FINISHME*** -- []TCLF -- ???








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
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    APLUINT      uQuadPP;       // []PP

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    if (uPrecision EQ 0)
        uQuadPP = lpMemPTD->lpSymQuadPP->stData.stInteger;
    else
        uQuadPP = uPrecision;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

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
    LPHSHENTRY   lpHshEntry;
    int          i, j;
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    typedef struct tagHT_FLAGNAMES
    {
        UINT    uMask;
        LPWCHAR lpwszName;
    } HT_FLAGNAMES, *LPHT_FLAGNAMES;

    // Hash Table flag names
    static HT_FLAGNAMES ahtFlagNames[] =
    {
     {0x00001,  L" Inuse"      },
     {0x00002,  L" PrinHash"   },
     {0x00004,  L" CharIsValid"},
     {0x00008,  L" Temp"       },
    };

// The # rows in the above table
#define HT_FLAGNAMES_NROWS  (sizeof (ahtFlagNames) / sizeof (ahtFlagNames[0]))

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    DbgMsg ("********** Hash Table **********************************");

    wsprintf (lpszDebug,
              "lpHshTab = %08X, SplitNext = %08X, Last = %08X",
              lpMemPTD->lpHshTab,
              lpMemPTD->lpHshTabSplitNext,
             &lpMemPTD->lpHshTab[lpMemPTD->iHshTabTotalSize]);
    DbgMsg (lpszDebug);

    // Display the hash table
    for (lpHshEntry = lpMemPTD->lpHshTab, i = 0;
         i < lpMemPTD->iHshTabTotalSize;
         lpHshEntry++, i++)
    {
        WCHAR wszFlags[128] = {L'\0'};
        UINT  htFlags;

        // Format the flags
        htFlags = *(UINT *) &lpHshEntry->htFlags;
        for (j = 0;
             j < HT_FLAGNAMES_NROWS;
             j++)
        if (htFlags & ahtFlagNames[j].uMask)
            lstrcatW (wszFlags, ahtFlagNames[j].lpwszName);

        // In case we didn't find any matching flags,
        //   set the second byte to zero as well as
        //   when we do find flags, we skip over the
        //   leading blank.
        if (wszFlags[0] EQ L'\0')
            wszFlags[1] =  L'\0';

        if (lpHshEntry->htFlags.Inuse
         && lpHshEntry->htSymEntry)
        {
            LPSYMENTRY lpSymEntry;

            lpSymEntry = lpHshEntry->htSymEntry;
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
            if (lpSymEntry->stFlags.ObjName NE OBJNAME_NONE)
            {
                LPCHAR lpGlbName;

                // Lock the memory to get a ptr to it
                lpGlbName = GlobalLock (lpHshEntry->htGlbName); Assert (lpGlbName NE NULL);

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
                GlobalUnlock (lpHshEntry->htGlbName); lpGlbName = NULL;
            } // End IF/ELSE/IF
        } else
            wsprintfW (lpwszDebug,
                       L"HT:%3d (EMPTY) <%s>, Sym=%08X, <%08X-%08X>",
                       i,
                      &wszFlags[1],
                       lpHshEntry->htSymEntry,
                       lpHshEntry->NextSameHash,
                       lpHshEntry->PrevSameHash);
        DbgMsgW (lpwszDebug);
    } // End FOR

    DbgMsg ("********** End Hash Table ******************************");

    UpdateDBWindow ();

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End DisplayHshTab
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplaySymTab
//
//  Display the Symbol Table entries
//
//  If bDispAll is FALSE, display only those non-OBJNAME_SYS entries
//    with a non-zero reference count.
//***************************************************************************

void DisplaySymTab
    (BOOL bDispAll)

{
    LPSYMENTRY   lpSymEntry;    // Ptr to current SYMENTRY
    int          i, j;          // Loop counters
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    typedef struct tagST_FLAGNAMES
    {
        UINT    uMask;
        LPWCHAR lpwszName;
    } ST_FLAGNAMES, *LPST_FLAGNAMES;

    // Symbol Table flag names
    static ST_FLAGNAMES astFlagNames[] =
    {
//// {0x00000001,  L" Imm"        },
//// {0x0000001E,  L" ImmType"    },
     {0x00000020,  L" NotCase"    },
     {0x00000040,  L" Perm"       },
     {0x00000080,  L" Inuse"      },
     {0x00000100,  L" Value"      },
/////{0x00000600,  L" ObjName"    },
//// {0x00003800,  L" UsrType"    },
//// {0x0003C000,  L" SysVarValid"},
     {0x00040000,  L" UsrDfn"     },
     {0x00080000,  L" DfnLabel"   },
     {0x00100000,  L" DfnSysLabel"},
     {0x00200000,  L" DfnAxis"    },
     {0x00400000,  L" FcnDir"     },
    };

// The # rows in the above table
#define ST_FLAGNAMES_NROWS  (sizeof (astFlagNames) / sizeof (astFlagNames[0]))

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    if (bDispAll)
        DbgMsg ("********** Symbol Table ********************************");
    else
        DbgMsg ("********** Symbol Table Referenced Non-SysNames ********");

    wsprintf (lpszDebug,
              "lpSymTab = %08X, Last = %08X",
              lpMemPTD->lpSymTab,
              &lpMemPTD->lpSymTab[lpMemPTD->iSymTabTotalSize]);
    DbgMsg (lpszDebug);

    // Display the symbol table
    for (lpSymEntry = lpMemPTD->lpSymTab, i = 0;
         lpSymEntry NE lpMemPTD->lpSymTabNext;
         lpSymEntry++, i++)
    if (bDispAll ||
        lpSymEntry->stFlags.ObjName NE OBJNAME_SYS)
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
        if (stFlags.ObjName NE OBJNAME_NONE)
            wsprintfW (&wszFlags[lstrlenW (wszFlags)],
                       L" ObjName=%s",
                       lpwObjNameStr[stFlags.ObjName]);
        if (stFlags.ObjType NE NAMETYPE_UNK)
            wsprintfW (&wszFlags[lstrlenW (wszFlags)],
                       L" ObjType=%s",
                       lpwNameTypeStr[stFlags.ObjType]);

        for (j = 0;
             j < ST_FLAGNAMES_NROWS;
             j++)
        if ((*(UINT *) &stFlags) & astFlagNames[j].uMask)
            lstrcatW (wszFlags, astFlagNames[j].lpwszName);

        if (IsNameTypeVar (stFlags.ObjType)
         && !stFlags.DfnSysLabel)
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
#define WSZNAME_LEN     128
            WCHAR wszName[WSZNAME_LEN] = {'\0'};

            if (lpSymEntry->stFlags.ObjName NE OBJNAME_NONE)
            {
                LPHSHENTRY lpHshEntry;

                lpHshEntry = lpSymEntry->stHshEntry;

                if (lpHshEntry)
                {
                    lpGlbName = GlobalLock (lpHshEntry->htGlbName); Assert (lpGlbName NE NULL);

                    lstrcpynW (wszName, lpGlbName, WSZNAME_LEN);

                    // We no longer need this ptr
                    GlobalUnlock (lpHshEntry->htGlbName); lpGlbName = NULL;
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
                           lpSymEntry->stHshEntry);
            } else
            if (lpSymEntry->stFlags.ObjName NE OBJNAME_NONE)
            {
                LPHSHENTRY lpHshEntry;

                lpHshEntry = lpSymEntry->stHshEntry;

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
                       lpSymEntry->stHshEntry);
        DbgMsgW (lpwszDebug);
    } // End FOR

    DbgMsg ("********** End Symbol Table ****************************");

    UpdateDBWindow ();

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End DisplaySymTab
#endif


#ifdef DEBUG
//***************************************************************************
//  UpdateDBWindow
//
//  Call UpdateWindow on the DB window
//***************************************************************************

void UpdateDBWindow
    (void)

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    UpdateWindow (lpMemPTD->hWndDB);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End UpdateDBWindow
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
    int          i;
    HGLOBAL      hGlb;
    LPVOID       lpMem;
    APLDIM       aplDim;
    LPVOID       lpData;
    APLCHAR      aplArrChar[19];
    LPAPLCHAR    lpwsz;
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

    // Lock the memory to get a ptr to it
    // Because we're inside DisplayGlobals, don't use
    //   MyGlobalLock/Unlock so as not to pollute the result
    lpMemPTD = GlobalLock (hGlbPTD);

    DbgMsg ("********** Globals *************************************");

    for (i = 0; i < MAXOBJ; i++)
    if (hGlb = ahGLOBAL[i])
    {
        // Note we don't use MyGlobalLock here as that function might well
        //   fail and trigger a hard error which we prefer to catch softly.

        // Lock the memory to get a ptr to it
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
        if (lpHeader->Sig.nature EQ VARARRAY_HEADER_SIGNATURE)
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

                    case ARRAY_LIST:
                        lstrcpyW (aplArrChar, L"[...]");

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
                               L"hGlb=%08X, ArrType=%c%c, NELM=%3d, RC=%1d, Rank=%2d, Dim1=%3d, Lock=%d, Line#=%4d, (%s)",
                               hGlb,
                               L"BIFCHNLA"[lpHeader->ArrType],
                               L" *"[lpHeader->Perm],
                               LODWORD (lpHeader->NELM),
                               lpHeader->RefCnt,
                               LODWORD (lpHeader->Rank),
                               LODWORD (aplDim),
                               (MyGlobalFlags (hGlb) & GMEM_LOCKCOUNT) - 1,
                               auLinNumGLOBAL[i],
                               aplArrChar);
                    DbgMsgW (lpwszDebug);
                } // End IF
            } // End IF
        } else
#undef  lpHeader
#define lpHeader    ((LPFCNARRAY_HEADER) lpMem)
        if (lpHeader->Sig.nature EQ FCNARRAY_HEADER_SIGNATURE)
        {
            // It's a valid HGLOBAL function array
            Assert (IsGlbTypeFcnDir (MakeGlbTypeGlb (hGlb)));

            wsprintf (lpszDebug,
                      "hGlb=%08X, NamTyp=%s, NELM=%3d, RC=%1d,                    Lock=%d, Line#=%4d",
                      hGlb,
                      lpNameTypeStr[lpHeader->NameType],
                      lpHeader->fcnNELM,
                      lpHeader->RefCnt,
                      (MyGlobalFlags (hGlb) & GMEM_LOCKCOUNT) - 1,
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

    UpdateDBWindow ();

    // We no longer need this ptr
    GlobalUnlock (hGlbPTD); lpMemPTD = NULL;
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
    LPTOKEN      lpToken;
    int          i, iLen;
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    if (gDbgLvl <= 2)
        return;

    DbgMsg ("********** Tokens **************************************");

    // Ensure it's valid
    if (!hGlbToken)
    {
        DbgMsg ("DisplayTokens:  ***INAVLID HANDLE***:  hGlbToken == 0");
        return;
    } // End IF

    // Lock the memory to get a ptr to it
    lpToken = MyGlobalLock (hGlbToken);

////// Ensure it's valid
////if (!lpToken)
////{
////    DbgMsg ("DisplayTokens:  ***INAVLID HANDLE***:  GlobalLock failed");
////    return;
////} // End IF

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

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
    MyGlobalUnlock (hGlbToken); lpToken = NULL;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
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
{{"VARNAMED"  , TKT_VARNAMED }, // 01: Symbol table entry for a named var (data is LPSYMENTRY)
 {"STRING"    , TKT_STRING   }, // 02: String  (data is HGLOBAL)
 {"VARIMMED"  , TKT_VARIMMED }, // 03: Immediate data (data is immediate)
 {"COMMENT"   , TKT_COMMENT  }, // 04: Comment (data is HGLOBAL)
 {"ASSIGN"    , TKT_ASSIGN   }, // 05: Assignment symbol (data is UTF16_LEFTARROW)
 {"LISTSEP"   , TKT_LISTSEP  }, // 06: List separator    (data is ';')
 {"COLON"     , TKT_COLON    }, // 07: Label ...         (data is ':')
 {"FCNIMMED"  , TKT_FCNIMMED }, // 08: Primitive function (any valence) (data is UTF16_***)
 {"OP1IMMED"  , TKT_OP1IMMED }, // 09: Monadic primitive operator (data is UTF16_***)
 {"OP2IMMED"  , TKT_OP2IMMED }, // 0A: Dyadic  ...
 {"OPJOTDOT"  , TKT_OPJOTDOT }, // 0B: Outer product monadic operator (with right scope) (data is NULL)
 {"LPAREN"    , TKT_LPAREN   }, // 0C: Left paren (data is TKT_LPAREN)
 {"RPAREN"    , TKT_RPAREN   }, // 0D: Right ...   ...         RPAREN
 {"LBRACKET"  , TKT_LBRACKET }, // 0E: Left bracket ...        LBRACKET
 {"RBRACKET"  , TKT_RBRACKET }, // 0F: Right ...   ...         RBRACKET
 {"EOS"       , TKT_EOS      }, // 10: End-of-Stmt (data is length of stmt including this token)
 {"EOL"       , TKT_EOL      }, // 11: End-of-Line  ...
 {"LINECONT"  , TKT_LINECONT }, // 12: Line continuation (data is NULL)
 {"INPOUT"    , TKT_INPOUT   }, // 13: Input/Output (data is UTF16_QUAD or UTF16_QUOTEQUAD)
 {"STRAND"    , TKT_STRAND   }, // 14: Strand accumulating (data is LPTOKEN)
 {"LISTINT"   , TKT_LISTINT  }, // 15: List in parens    (data is HGLOBAL)
 {"LISTPAR"   , TKT_LISTPAR  }, // 16: List in parens    (data is HGLOBAL)
 {"LISTBR"    , TKT_LISTBR   }, // 17: List in brackets  (data is HGLOBAL)
 {"VARARRAY"  , TKT_VARARRAY }, // 18: Array of data (data is HGLOBAL)
 {"FCNARRAY"  , TKT_FCNARRAY }, // 19: Array of functions (data is HGLOBAL)
 {"FCNNAMED"  , TKT_FCNNAMED }, // 1A: Symbol table entry for a named function (data is LPSYMENTRY)
 {"AXISIMMED" , TKT_AXISIMMED}, // 1B: An immediate axis specification (data is immediate)
 {"AXISARRAY" , TKT_AXISARRAY}, // 1C: An array of  ...   (data is HGLOBAL)
 {"OP1NAMED"  , TKT_OP1NAMED }, // 1D: A named monadic primitive operator (data is LPSYMENTRY)
 {"OP2NAMED"  , TKT_OP2NAMED }, // 1E: ...     dyadic  ...
 {"STRNAMED"  , TKT_STRNAMED }, // 1F: ...     strand  ...
};

// The # rows in the above table
#define TOKENNAMES_NROWS    (sizeof (tokenNames) / sizeof (tokenNames[0]))

    if (TOKENNAMES_NROWS > (uType - TKT_FIRST))
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
    (LPTOKEN lptkFunc)          // Ptr to function token

{
    HGLOBAL      hGlbData;      // Function array global memory handle
    LPWCHAR      lpaplChar;     // Ptr to output save area
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    lpaplChar = lpwszDebug;

    // Split cases based upon the token type
    switch (lptkFunc->tkFlags.TknType)
    {
        case TKT_VARNAMED:          // Because we don't distinguish between
                                    //   functions and variables in AssignName_EM
            DbgBrk ();          // ***FINISHME*** -- TKT_VARNAMED in DisplayFcnStrand






            goto NORMAL_EXIT;

        case TKT_FCNIMMED:
        case TKT_OP1IMMED:
        case TKT_OP2IMMED:
            lpaplChar += wsprintfW (lpaplChar,
                                    L"NameType=%1d, NELM=%3d, RC=%2d, Fn:  ",
                                    NAMETYPE_FN12,  // lpHeader->NameType,
                                    1,              // LODWORD (lpHeader->NELM),
                                    0);             // lpHeader->RefCnt);
            *lpaplChar++ = lptkFunc->tkData.tkChar;

            break;

        case TKT_FCNNAMED:
        case TKT_OP1NAMED:
        case TKT_OP2NAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkFunc->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, ...
            if (!lptkFunc->tkData.tkSym->stFlags.Imm)
            {
                hGlbData = lptkFunc->tkData.tkSym->stData.stGlbData;

                // stData is an internal function or a HGLOBAL function array or defined function
                Assert (lptkFunc->tkData.tkSym->stFlags.FcnDir
                     || IsGlbTypeFcnDir (hGlbData)
                     || IsGlbTypeDfnDir (hGlbData));

                // Check for internal functions
                if (lptkFunc->tkData.tkSym->stFlags.FcnDir)
                {
                    // Start off with the header
                    lpaplChar += wsprintfW (lpaplChar,
                                            L"NameType=%1d, NELM=%3d, RC=%2d, Fn:  ",
                                            NAMETYPE_FN12,  // lpHeader->NameType,
                                            1,              // LODWORD (lpHeader->NELM),
                                            0);             // lpHeader->RefCnt);
                    // Display the function name from the symbol table
                    lpaplChar = CopySteName (lpaplChar, lptkFunc->tkData.tkSym);
                } else
                if (!lptkFunc->tkData.tkSym->stFlags.UsrDfn)
                    lpaplChar = DisplayFcnGlb (lpaplChar, ClrPtrTypeDirGlb (hGlbData), TRUE);
            } else
            {
                // Start off with the header
                lpaplChar += wsprintfW (lpaplChar,
                                        L"NameType=%1d, NELM=%3d, RC=%2d, Fn:  ",
                                        NAMETYPE_FN12,  // lpHeader->NameType,
                                        1,              // LODWORD (lpHeader->NELM),
                                        0);             // lpHeader->RefCnt);
                *lpaplChar++ = lptkFunc->tkData.tkSym->stData.stChar;
            } // End IF/ELSE

            break;

        case TKT_FCNARRAY:
            hGlbData = lptkFunc->tkData.tkGlbData;

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
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End DisplayFcnStrand
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayFcnGlb
//
//  Display a function from an HGLOBAL
//***************************************************************************

LPWCHAR DisplayFcnGlb
    (LPWCHAR lpaplChar,         // Ptr to output save area
     HGLOBAL hGlbFcnArr,        // Function array global memory handle
     BOOL    bDispHeader)       // TRUE iff we're to display the header (NameType=...)

{
    LPVOID  lpMemFcnArr;    // Ptr to function array global memory
    UINT    fcnNELM;        // Function array NELM

    // Lock the memory to get a ptr to it
    lpMemFcnArr = MyGlobalLock (hGlbFcnArr);

#define lpHeader    ((LPFCNARRAY_HEADER) lpMemFcnArr)
    fcnNELM = lpHeader->fcnNELM;

    if (bDispHeader)
        lpaplChar += wsprintfW (lpaplChar,
                                L"NameType=%s, NELM=%3d, RC=%2d, Fn:  ",
                                lpwNameTypeStr[lpHeader->NameType],
                                fcnNELM,
                                lpHeader->RefCnt);
#undef  lpHeader

    lpaplChar =
      DisplayFcnSub (lpaplChar,
                     FcnArrayBaseToData (lpMemFcnArr),  // Skip over the header to the data
                     fcnNELM);
    // We no longer need this ptr
    MyGlobalUnlock (hGlbFcnArr); lpMemFcnArr = NULL;

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
    (LPWCHAR      lpaplChar,    // Ptr to output save area
     LPPL_YYSTYPE lpYYMem,      // Ptr to function array
     UINT         fcnNELM)      // Function NELM

{
    HGLOBAL hGlbData;           // Function array global memory handle
    LPVOID  lpMemData;          // Ptr to function array global memory
    UINT    FcnCount;           // Function count

    // Split cases based upon the token type
    switch (lpYYMem[0].tkToken.tkFlags.TknType)
    {
        case TKT_OP1IMMED:
            // Check for axis operator
            if (fcnNELM > 1
             && (lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
              || lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
            {
                // If there's a function, ...
                if (fcnNELM > 2)
                    lpaplChar =
                      DisplayFcnSub (lpaplChar, &lpYYMem[2], fcnNELM - 2);  // fcn
                *lpaplChar++ = lpYYMem[0].tkToken.tkData.tkChar;            // Op1
                lpaplChar =
                  DisplayFcnSub (lpaplChar, &lpYYMem[1], 1);                // [X]
            } else
            {
                lpaplChar =
                  DisplayFcnSub (lpaplChar, &lpYYMem[1], fcnNELM - 1);      // fcn
                *lpaplChar++ = lpYYMem[0].tkToken.tkData.tkChar;            // Op1
            } // End IF/ELSE/...

            break;

        case TKT_OP2IMMED:
            FcnCount = 1 + lpYYMem[1].FcnCount;
            lpaplChar =
              DisplayFcnSub (lpaplChar, &lpYYMem[1], lpYYMem[1].FcnCount);  // Lfcn
            *lpaplChar++ = lpYYMem[0].tkToken.tkData.tkChar;                // Op2
            if (lpYYMem[FcnCount].FcnCount > 1)
                *lpaplChar++ = L'(';
            lpaplChar =
              DisplayFcnSub (lpaplChar, &lpYYMem[FcnCount], lpYYMem[FcnCount].FcnCount);  // Rfcn
            if (lpYYMem[FcnCount].FcnCount > 1)
                *lpaplChar++ = L')';
            break;

        case TKT_FCNIMMED:
            *lpaplChar++ = lpYYMem[0].tkToken.tkData.tkChar;                // fcn

            // Check for axis operator
            if (fcnNELM > 1
             && (lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
              || lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
                lpaplChar =
                  DisplayFcnSub (lpaplChar, &lpYYMem[1], 1);                // [X]

            break;

        case TKT_OP1NAMED:
            DbgBrk ();          // ***FINISHME*** -- TKT_OP1NAMED in DisplayFcnSub






            break;

        case TKT_OP2NAMED:
            DbgBrk ();          // ***FINISHME*** -- TKT_OP2NAMED in DisplayFcnSub





            break;

        case TKT_FCNNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpYYMem->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not an immediate function, ...
            if (!lpYYMem->tkToken.tkData.tkSym->stFlags.Imm)
            {
                // Check for internal functions
                if (lpYYMem->tkToken.tkData.tkSym->stFlags.FcnDir)
                    // Copy the internal function name
                    lpaplChar =
                      CopySteName (lpaplChar,
                                   lpYYMem->tkToken.tkData.tkSym);
                else
                {
                    // Get the function array global memory handle
                    hGlbData = lpYYMem->tkToken.tkData.tkSym->stData.stGlbData;

                    // stData is a valid HGLOBAL function array
                    Assert (IsGlbTypeFcnDir (hGlbData));

                    // Display the function strand in global memory
                    lpaplChar =
                      DisplayFcnGlb (lpaplChar, ClrPtrTypeDirGlb (hGlbData), FALSE);
                } // End IF/ELSE
            } else
                // Handle the immediate case
                *lpaplChar++ = lpYYMem->tkToken.tkData.tkSym->stData.stChar;

            // Check for axis operator
            if (fcnNELM > 1
             && (lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
              || lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
                lpaplChar =
                  DisplayFcnSub (lpaplChar, &lpYYMem[1], 1);            // [X]

            break;

        case TKT_OPJOTDOT:
            *lpaplChar++ = UTF16_JOT;
            *lpaplChar++ = L'.';

            // Surround with parens if more than one token
            if (fcnNELM > 2)
                *lpaplChar++ = L'(';
            lpaplChar =
              DisplayFcnSub (lpaplChar, &lpYYMem[1], fcnNELM - 1);
            if (fcnNELM > 2)
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

            // Get the axis array global memory handle
            hGlbData = lpYYMem->tkToken.tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (hGlbData));
#if TRUE
            lstrcpyW (lpaplChar, L"TXT_AXISARRAY ");    // N.B.:  trailing blank is significant
            lpaplChar += lstrlenW (lpaplChar);
#else
            lpaplChar =
              FormatGlobal (lpaplChar, ClrPtrTypeDirGlb (hGlbData));
#endif
            if (lpaplChar[-1] EQ L' ')
                lpaplChar--;            // Back over the trailing blank
            *lpaplChar++ = L']';

            break;

        case TKT_VARIMMED:
            lpaplChar =
              FormatAplint (lpaplChar,
                            lpYYMem[0].tkToken.tkData.tkInteger);
            if (lpaplChar[-1] EQ L' ')
                lpaplChar--;            // Back over the trailing blank

            break;

        case TKT_VARARRAY:
            // Get the variable array global memory handle
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

        case TKT_FCNARRAY:
            // Get the function array global memory handle
            hGlbData = lpYYMem->tkToken.tkData.tkGlbData;

            // tkData is a valid HGLOBAL function array or defined function
            Assert (IsGlbTypeFcnDir (hGlbData)
                 || IsGlbTypeDfnDir (hGlbData));

            // Clear the ptr type bits
            hGlbData = ClrPtrTypeDirGlb (hGlbData);

            // Lock the memory to get a ptr to it
            lpMemData = MyGlobalLock (hGlbData);

            // Split cases based upon the array signature
            switch (((LPHEADER_SIGNATURE) lpMemData)->nature)
            {
                case FCNARRAY_HEADER_SIGNATURE:
#define lpHeader    ((LPFCNARRAY_HEADER) lpMemData)
                    fcnNELM = lpHeader->fcnNELM;
#undef  lpHeader
                    // Surround with parens if more than one token
                    if (fcnNELM > 1)
                        *lpaplChar++ = L'(';
                    lpaplChar =
                      DisplayFcnSub (lpaplChar,
                                     FcnArrayBaseToData (lpMemData),    // Skip over the header to the data
                                     fcnNELM);
                    if (fcnNELM > 1)
                        *lpaplChar++ = L')';
                    break;

                case DFN_HEADER_SIGNATURE:
                    // Copy the defined function name
                    lpaplChar =
                      CopySteName (lpaplChar,
                                   ((LPDFN_HEADER) lpMemData)->steFcnName);
                    break;

                defstop
                    break;
            } // End SWITCH

            // We no longer need this ptr
            MyGlobalUnlock (hGlbData); lpMemData = NULL;

            break;

        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpYYMem->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

            // Copy the STE name
            lpaplChar =
              CopySteName (lpaplChar,
                           lpYYMem->tkToken.tkData.tkSym);
            break;

        defstop
            break;
    } // End SWITCH

    // Ensure properly terminated
    *lpaplChar = L'\0';

    return lpaplChar;
} // End DisplayFcnSub
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayFcnArr
//
//  Display a Function Array
//***************************************************************************

void DisplayFcnArr
    (HGLOBAL hGlbStr)           // Function array global memory handle

{
    Assert (IsGlbTypeFcnDir (MakeGlbTypeGlb (hGlbStr)));

    DbgMsgW (L"********** Function Array ******************************");

    DisplayFcnGlb (lpwszDebug, hGlbStr, TRUE);

    DbgMsgW (lpwszDebug);

    DbgMsgW (L"********** End Function Array **************************");
} // End DisplayFcnArr
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayStrand
//
//  Display the strand stack
//***************************************************************************

void DisplayStrand
    (int strType)               // Strand type (STRAND_VAR or STRAND_FCN)

{
    LPPL_YYSTYPE  lp,
                  lpLast;
    LPPLLOCALVARS lpplLocalVars;
    HGLOBAL       hGlbPTD;      // PerTabData global memory handle
    LPPERTABDATA  lpMemPTD;     // Ptr to PerTabData global memory

    // Check debug level
    if (gDbgLvl <= 2)
        return;

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsPlLocalVars);

    switch (strType)
    {
        case STRAND_VAR:
            DbgMsg ("********** Variable Strands ****************************");

            break;

        case STRAND_FCN:
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
        if (lpLast NE lp->lpYYStrandBase)
        {
            DbgMsg ("--------------------------------------------------------");
            lpLast  = lp->lpYYStrandBase;
        } // End IF

        wsprintf (lpszDebug,
                  "Strand (%08X): %-9.9s D=%08X CI=%2d TC=%1d FC=%1d IN=%1d F=%08X B=%08X",
                  lp,
                  GetTokenTypeName (lp->tkToken.tkFlags.TknType),
                  LODWORD (lp->tkToken.tkData.tkInteger),
                  lp->tkToken.tkCharIndex,
                  lp->TknCount,
                  lp->FcnCount,
                  lp->YYIndirect,
                  lp->lpYYFcn,
                  lpLast);
        DbgMsg (lpszDebug);
    } // End FOR

    DbgMsg ("********** End Strands *********************************");

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
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
    UINT         uCharPos,
                 uLineCount;
    HGLOBAL      hGlbEC;
    LPWCHAR      lpwsz, p;
    HWND         hWndParent;
    LPUNDOBUF    lpUndoBeg,     // Ptr to start of Undo Buffer
                 lpUndoNxt;     // ...    next available slot in the Undo Buffer
    BOOL         bShift;
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    static LPWCHAR Actions[]={L"None",
                              L"Ins",
                              L"Rep",
                              L"Del",
                              L"Sel",
                              L"Back",
                              L"InsToggle"};

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

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
    dprintfW (L"Text = <%s>",
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

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End DisplayUndo
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayFnHdr
//
//  Display the function header
//***************************************************************************

void DisplayFnHdr
    (LPFHLOCALVARS lpfhLocalVars)

{
    WCHAR     wszTemp[1024];
    HGLOBAL   hGlbName;
    LPAPLCHAR lpMemName;
    UINT      uLen, uItm;

    // Initialize the output string
    lstrcpyW (wszTemp, L"Saving function header:  " WS_UTF16_DEL L" ");

    // Check for result
    if (lpfhLocalVars->lpYYResult)
    {
        // Get the strand length
        uLen = lpfhLocalVars->lpYYResult->uStrandLen;

        // Append a separator
        if (uLen > 1)
            lstrcatW (wszTemp, L"(");

        for (uItm = 0; uItm < uLen; uItm++)
        {
            // Get the Name's global memory handle
            hGlbName = lpfhLocalVars->lpYYResult[uItm].tkToken.tkData.tkSym->stHshEntry->htGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLock (hGlbName);

            // Copy the name
            lstrcatW (wszTemp, lpMemName);

            // If we're not at the last item, separate with a space
            if (uItm < (uLen - 1))
                lstrcatW (wszTemp, L" ");

            // We no longer need this ptr
            MyGlobalUnlock (hGlbName); lpMemName = NULL;
        } // End FOR

        // Append a separator
        if (uLen > 1)
            lstrcatW (wszTemp, L")");

        // Append a left arrow
        lstrcatW (wszTemp, WS_UTF16_LEFTARROW);
    } // End IF

    // Check for left argument
    if (lpfhLocalVars->lpYYLftArg)
    {
        // Get the strand length
        uLen = lpfhLocalVars->lpYYLftArg->uStrandLen;

        // Append a separator
        if (lpfhLocalVars->FcnValence EQ FCNVALENCE_AMB)
            lstrcatW (wszTemp, L"[");
        else
        if (uLen > 1)
            lstrcatW (wszTemp, L"(");

        for (uItm = 0; uItm < uLen; uItm++)
        {
            // Get the Name's global memory handle
            hGlbName = lpfhLocalVars->lpYYLftArg[uItm].tkToken.tkData.tkSym->stHshEntry->htGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLock (hGlbName);

            // Copy the name
            lstrcatW (wszTemp, lpMemName);

            // If we're not at the last item, separate with a space
            if (uItm < (uLen - 1))
                lstrcatW (wszTemp, L" ");

            // We no longer need this ptr
            MyGlobalUnlock (hGlbName); lpMemName = NULL;
        } // End FOR

        // Append a separator
        if (lpfhLocalVars->FcnValence EQ FCNVALENCE_AMB)
            lstrcatW (wszTemp, L"]");
        else
        if (uLen > 1)
            lstrcatW (wszTemp, L")");

        // Append a separator
        lstrcatW (wszTemp, L" ");
    } // End IF

    // Split cases based upon the defined function type
    switch (lpfhLocalVars->DfnType)
    {
        case DFNTYPE_OP1:
        case DFNTYPE_OP2:
            // Append a separator
            lstrcatW (wszTemp, L"(");

            // Check for left operand
            if (lpfhLocalVars->lpYYLftOpr)
            {
                // Get the Name's global memory handle
                hGlbName = lpfhLocalVars->lpYYLftOpr->tkToken.tkData.tkSym->stHshEntry->htGlbName;

                // Lock the memory to get a ptr to it
                lpMemName = MyGlobalLock (hGlbName);

                // Copy the name
                lstrcatW (wszTemp, lpMemName);

                // We no longer need this ptr
                MyGlobalUnlock (hGlbName); lpMemName = NULL;

                // Append a separator
                lstrcatW (wszTemp, L" ");
            } // End IF

            // Get the Name's global memory handle
            hGlbName = lpfhLocalVars->lpYYFcnName->tkToken.tkData.tkSym->stHshEntry->htGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLock (hGlbName);

            // Copy the name
            lstrcatW (wszTemp, lpMemName);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbName); lpMemName = NULL;

            // Get the Name's global memory handle
            hGlbName = lpfhLocalVars->lpYYRhtOpr->tkToken.tkData.tkSym->stHshEntry->htGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLock (hGlbName);

            // Copy the name
            lstrcatW (wszTemp, lpMemName);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbName); lpMemName = NULL;

            // Append a separator
            lstrcatW (wszTemp, L")");

            break;

        case DFNTYPE_FCN:
            // Get the Name's global memory handle
            hGlbName = lpfhLocalVars->lpYYFcnName->tkToken.tkData.tkSym->stHshEntry->htGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLock (hGlbName);

            // Copy the name
            lstrcatW (wszTemp, lpMemName);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbName); lpMemName = NULL;

            break;

        case DFNTYPE_UNK:
            lstrcatW (wszTemp, L"<empty>");

            break;

        defstop
            break;
    } // End SWITCH

    // Check for right argument
    if (lpfhLocalVars->lpYYRhtArg)
    {
        // Append a separator
        lstrcatW (wszTemp, L" ");

        // Get the strand length
        uLen = lpfhLocalVars->lpYYRhtArg->uStrandLen;

        // Append a separator
        if (uLen > 1)
            lstrcatW (wszTemp, L"(");

        for (uItm = 0; uItm < uLen; uItm++)
        {
            // Get the Name's global memory handle
            hGlbName = lpfhLocalVars->lpYYRhtArg[uItm].tkToken.tkData.tkSym->stHshEntry->htGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLock (hGlbName);

            // Copy the name
            lstrcatW (wszTemp, lpMemName);

            // If we're not at the last item, separate with a space
            if (uItm < (uLen - 1))
                lstrcatW (wszTemp, L" ");

            // We no longer need this ptr
            MyGlobalUnlock (hGlbName); lpMemName = NULL;
        } // End FOR

        // Append a separator
        if (uLen > 1)
            lstrcatW (wszTemp, L")");
    } // End IF

    // Display it in the debug window
    DbgMsgW (wszTemp);
} // End DisplayFnHdr
#endif


//***************************************************************************
//  End of File: display.c
//***************************************************************************
