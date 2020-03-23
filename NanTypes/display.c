//***************************************************************************
//  NARS2000 -- Display Routines
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2020 Sudley Place Software

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
#define _USE_MATH_DEFINES
#include "headers.h"
#include "debug.h"              // For xxx_TEMP_OPEN macros


typedef enum tagDTOA_MODE
{
    DTOAMODE_SHORT_RND = 0,             // 0 = shortest string with rounding, e.g., 1e23
    DTOAMODE_SHORT_NORND,               // 1 = shortest string without rounding. e.g., 9.999999999999999e22
    DTOAMODE_SIGDIGS,                   // 2 = # significant digits
    DTOAMODE_FRACTDIGS,                 // 3 = # fractional digits (past decimal point)
} DTOAMODE;

WCHAR wcInf = UTF16_INFINITY,
      wcNaN = UTF16_NAN;

// DTOA mode for the corresponding FLTDSPFMT_xxx value
DTOAMODE gDTOA_Mode[FLTDISPFMT_LENGTH] = {DTOAMODE_SIGDIGS,     // E :  2 = nDigits significant digits
                                          DTOAMODE_FRACTDIGS,   // F :  3 = nDigits past decimal point
                                          DTOAMODE_SIGDIGS,     // RI:  2 = nDigits significant digits
                                          DTOAMODE_SHORT_RND};  // RF:  0 = shortest string

typedef struct tagROWPTRS
{
    LPAPLCHAR lpNxtChar,        // Ptr to next entry of formatted data
              lpEndChar;        // ...    byte after row end of ...
} ROWPTRS, *LPROWPTRS;


// # fractional digits to display for various Angles
#define DEF_ANGLEPREC_ad     1
#define DEF_ANGLEPREC_ah     4
#define DEF_ANGLEPREC_ar     3
#define DEF_ANGLEPREC_au     4
#define DEF_ANGLEPREC_UNK   -1


//***************************************************************************
//  $ArrayDisplay_EM
//
//  Display an array
//***************************************************************************

UBOOL ArrayDisplay_EM
    (LPTOKEN lptkRes,               // Ptr to value token
     UBOOL   bEndingCR,             // TRUE iff last line has CR
     LPUBOOL lpbCtrlBreak)          // Ptr to Ctrl-Break flag

{
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPAPLCHAR    lpaplChar;         // Ptr to output save area
    LPWCHAR      lpwszFormat;       // Ptr to formatting save area
    IMM_TYPES    immType;           // Type of immediate value
    APLCHAR      aplChar;           // Immediate value if character
    UBOOL        bRet = TRUE,       // TRUE iff the result is valid
                 oldgbBoxState,     // Save area for old BoxState
                 bFreeDisp = FALSE; // TRUE iff we're to free lptkDisp at the end
    TOKEN        tkDisp = {0};      // Temporary token
    LPTOKEN      lptkDisp;          // Ptr to token to display

    // Save the old value
    oldgbBoxState = gbBoxState;

    // If it's ON, ...
    if (gbBoxState)
    {
        LPPL_YYSTYPE lpYYBox;

        // Check for NoDisplay flag
        if (lptkRes->tkFlags.NoDisplay)
            goto NORMAL_EXIT;

        // Turn it OFF to avoid recursion
        gbBoxState = FALSE;

        // Display the array with boxing
        lpYYBox = SysFnMonFMT_EM_YY (NULL,      // Ptr to function token
                                     lptkRes,   // Ptr to right arg token (should be NULL)
                                     NULL);     // Ptr to axis token (may be NULL)
        // Copy the new token to local storage
        CopyAll (&tkDisp, &lpYYBox->tkToken);

        // Point to the new token to display and then free
        lptkDisp = &tkDisp;

        // Free the YYSTYPE
        YYFree (lpYYBox); lpYYBox = NULL;

        // Tell the trailing code to free lptkDisp at the end
        bFreeDisp = TRUE;

        // Restore the previous state
        gbBoxState = oldgbBoxState;
    } else
        // Point to the old token to display
        lptkDisp = lptkRes;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to formatting save area
    lpwszFormat = lpMemPTD->lpwszFormat;

    // Split cases based upon the token type
    switch (lptkDisp->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkDisp->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // Check for NoValue
            if (IsSymNoValue (lptkDisp->tkData.tkSym))
                goto NORMAL_EXIT;

            // If it's not immediate, it's an array
            if (!lptkDisp->tkData.tkSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (lptkDisp->tkData.tkSym->stData.stVoid));

                // Check for NoDisplay flag
                if (!lptkDisp->tkFlags.NoDisplay)
                {
                    bRet =
                      DisplayGlbArr_EM (lptkDisp->tkData.tkSym->stData.stGlbData,
                                        bEndingCR,          // TRUE iff last line has CR
                                        lpbCtrlBreak,       // Ptr to Ctrl-Break flag
                                        lptkDisp);          // Ptr to function token
                    goto NORMAL_EXIT;
                } // End IF
            } // End IF

            // Check for NoDisplay flag
            if (lptkDisp->tkFlags.NoDisplay)
                goto NORMAL_EXIT;

            // Handle the immediate case

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkDisp->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // stData is immediate
            Assert (lptkDisp->tkData.tkSym->stFlags.Imm);

            lpaplChar =
              FormatSymTabConst (lpwszFormat,
                                 lptkDisp->tkData.tkSym);
            immType = lptkDisp->tkData.tkSym->stFlags.ImmType;
            aplChar = lptkDisp->tkData.tkSym->stData.stChar;

            break;

        case TKT_VARIMMED:  // The tkData is an immediate constant
            // Check for NoDisplay flag
            if (lptkDisp->tkFlags.NoDisplay)
                goto NORMAL_EXIT;

            lpaplChar =
              FormatImmed (lpwszFormat,
                           lptkDisp->tkFlags.ImmType,
                           GetPtrTknLongest (lptkDisp));
            immType = lptkDisp->tkFlags.ImmType;
            aplChar = lptkDisp->tkData.tkChar;

            break;

        case TKT_LISTPAR:   // The tkData is an HGLOBAL of an array of LPSYMENTRYs/HGLOBALs
            goto SYNTAX_EXIT;

        case TKT_VARARRAY:  // The tkData is an HGLOBAL of an array of LPSYMENTRYs/HGLOBALs
            // Check for NoDisplay flag
            if (lptkDisp->tkFlags.NoDisplay)
                goto NORMAL_EXIT;

            switch (GetPtrTypeDir (lptkDisp->tkData.tkVoid))
            {
                case PTRTYPE_STCONST:
                    lpaplChar =
                      FormatSymTabConst (lpwszFormat,
                                         lptkDisp->tkData.tkSym);
                    immType = lptkDisp->tkData.tkSym->stFlags.ImmType;
                    aplChar = lptkDisp->tkData.tkSym->stData.stChar;

                    break;

                case PTRTYPE_HGLOBAL:
                    bRet =
                      DisplayGlbArr_EM (lptkDisp->tkData.tkGlbData,
                                        bEndingCR,          // TRUE iff last line has CR
                                        lpbCtrlBreak,       // Ptr to Ctrl-Break flag
                                        lptkDisp);          // Ptr to function token
                    goto NORMAL_EXIT;

                defstop
                    // Mark as in error
                    bRet = FALSE;

                    goto NORMAL_EXIT;
            } // End SWITCH

            break;

////////case TKT_STRAND:    // The tkData is an HGLOBAL of a single HGLOBAL
        defstop
            // Mark as in error
            bRet = FALSE;

            goto NORMAL_EXIT;
    } // End SWITCH

    // Delete the last blank in case it matters,
    //   and ensure properly terminated
    if (lpaplChar[-1] EQ L' ')
        *--lpaplChar = WC_EOS;
    else
        *lpaplChar = WC_EOS;

    // If the value is a character, ...
    if (IsImmChr (immType))
    // If we're NOT Output Debugging, ...
    if (!OptionFlags.bOutputDebug)
    // Split cases based upon the character value
    switch (aplChar)
    {
        case WC_BEL:
            // Sound the alarum!
            MessageBeep (NEG1U);

            // Empty the line
            lpwszFormat[0] = WC_EOS;

            break;

        case WC_HT:
            FillMemoryW (lpwszFormat, uTabStops, L' ');

            // Append a terminating zero
            lpwszFormat[uTabStops] = WC_EOS;

            break;

        case WC_LF:
        case WC_CR:
        default:
            break;
    } // End IF/IF/SWITCH

    // Display the line
    AppendLine (lpwszFormat, FALSE, bEndingCR);

    goto NORMAL_EXIT;

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkRes);
    // Mark as in error
    bRet = FALSE;

    goto NORMAL_EXIT;

NORMAL_EXIT:
    // If we're to free lptkDisp, ...
    if (bFreeDisp)
    {
        // Free the boxed display result
        FreeResultTkn (lptkDisp); lptkDisp = NULL;
    } // End IF

    return bRet;
} // End ArrayDisplay_EM


//***************************************************************************
//  $DisplayGlbArr_EM
//
//  Display a global array
//***************************************************************************

UBOOL DisplayGlbArr_EM
    (HGLOBAL hGlbArr,               // Global memory handle to display
     UBOOL   bEndingCR,             // TRUE iff last line has CR
     LPUBOOL lpbCtrlBreak,          // Ptr to Ctrl-Break flag
     LPTOKEN lptkFunc)              // Ptr to function token

{
    LPPERTABDATA      lpMemPTD;                 // Ptr to PerTabData global memory
    LPVARARRAY_HEADER lpMemHdrArr = NULL;       // Ptr to array header
    LPVOID            lpMemArr;                 // Ptr to incoming global memory
    LPAPLCHAR         lpaplCharIni = NULL,      // Ptr to temp area
                      lpaplChar,                // ...
                      lpaplCharStart,           // ...
                      lpwszNxt,                 // Ptr to next char of input
                      lpwszEnd;                 // ..     end  char of input
    APLSTYPE          aplType;                  // Arg storage type
    APLNELM           aplNELM;                  // Arg NELM
    APLRANK           aplRank;                  // Arg rank
    LPAPLDIM          lpMemDim;                 // Ptr to arg dimensions
    APLDIM            aplDimNCols,              // # cols
                      aplDimNRows,              // # rows
                      aplChrNCols,              // # cols for char arrays
                      aplDimCol,                // Col loop counter
                      aplLastDim;               // Length of the last dimension in the result
    APLNELM           aplNELMRes;               // Result NELM
    LPFMTHEADER       lpFmtHeader,              // Ptr to format header struc
                      lpFmtHeader2;             // ...
    LPFMTCOLSTR       lpFmtColStr;              // Ptr to format col struc
    APLUINT           uQuadPW;                  // []PW
    UBOOL             bLineCont = FALSE,        // TRUE iff this line is a continuation
                      bRawOut,                  // TRUE iff using raw output
                      bRet = FALSE;             // TRUE iff the result is valid
    APLUINT           uOutLen;                  // Output length for this line
    MEMVIRTSTR        lclMemVirtStr[1] = {0};   // Room for one GuardAlloc
    LPWCHAR           lpwszFormat;              // Ptr to formatting save area
    jmp_buf           oldHeapFull;              // Save area for previous jmp_buf

    // Save the previous heapFull
    *oldHeapFull = *heapFull;

    // Set the heapFull for a longjmp
    if (setjmp (heapFull) NE 0)
        goto WSFULL_EXIT;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to formatting save area
    lpwszFormat = lpMemPTD->lpwszFormat;

    // Get the current value of []PW
    uQuadPW = GetQuadPW ();

    // Allocate space for the display
    lclMemVirtStr[0].lpText   = "lpaplCharIni in <DisplayGlbArr_EM>";
    lclMemVirtStr[0].IncrSize = DEF_DISPLAY_INCRNELM * sizeof (APLCHAR);
    lclMemVirtStr[0].MaxSize  = DEF_DISPLAY_MAXNELM  * sizeof (APLCHAR);
    lclMemVirtStr[0].IniAddr  = (LPUCHAR)
    lpaplCharIni =
      GuardAlloc (NULL,             // Any address
                  lclMemVirtStr[0].MaxSize,
                  MEM_RESERVE,
                  PAGE_READWRITE);
    if (!lclMemVirtStr[0].IniAddr)
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsgW (L"DisplayGlbArr_EM:  GuardAlloc for <lpaplCharIni> failed");

        goto ERROR_EXIT;    // Mark as failed
    } // End IF

    // Link this struc into the chain
    LinkMVS (&lclMemVirtStr[0]);

////// Commit the intial size
////VirtualAlloc (lclMemVirtStr[0].IniAddr,
////              DEF_DISPLAY_INITNELM * sizeof (APLCHAR),
////              MEM_COMMIT,
////              PAGE_READWRITE);
    __try
    {
#ifdef DEBUG
        // Fill lpaplCharIni with FFs so we can tell what we actually wrote
        FillMemory (lpaplCharIni, 1024, 0xFF);
#endif
        // Lock the memory to get a ptr to it
        lpMemHdrArr = MyGlobalLockVar (hGlbArr);

#define lpHeader    lpMemHdrArr
        // Get the Array Type, NELM, and Rank
        aplType = lpHeader->ArrType;
        aplNELM = lpHeader->NELM;
        aplRank = lpHeader->Rank;
#undef  lpHeader

        // Skip over the header to the dimensions
        lpMemDim = VarArrayBaseToDim (lpMemHdrArr);

        // Skip over the header and dimension to the data
        lpMemArr = VarArrayDataFmBase (lpMemHdrArr);

        // Get the # columns
        if (IsScalar (aplRank))
            aplDimNCols = aplDimNRows = 1;
        else
        {
            aplDimNCols = lpMemDim[aplRank - 1];

            // If there are no columns and the rank is > 1, ignore this
            if (IsZeroDim (aplDimNCols)
             && IsRank2P (aplRank))
                goto NORMAL_EXIT;

            // Get the # rows (across all planes)
            if (IsVector (aplRank))
                aplDimNRows = 1;
            else
                aplDimNRows = aplNELM / aplDimNCols;
        } // End IF/ELSE

        // If the array is a non-empty non-PtrArray vector-like, display it specially
        if (!IsEmpty (aplNELM)
         && !IsPtrArray (aplType)
         && aplDimNCols EQ aplNELM)
        {
            // Display the vector-like array without running
            //   into length problems w/guard pages
            bRet =
              DisplayGlbVector (lpaplCharIni,       // Ptr to temp area
                                aplType,            // Array storage type
                                aplDimNCols,        // Array # cols
                                lpMemArr,           // Ptr to array global memory
                                bEndingCR,          // TRUE iff last line has CR
                                lpbCtrlBreak,       // Prt to Ctrl-Break flag
                                uQuadPW);           // []PW
            goto ERROR_EXIT;
        } // End IF

        // Format char arrays as one col
        aplChrNCols = (IsSimpleChar (aplType)) ? 1 : aplDimNCols;

        // Create a new FMTHEADER
        lpFmtHeader2 = (LPFMTHEADER) lpaplCharIni;
        ZeroMemory (lpFmtHeader2, sizeof (FMTHEADER));
////////lpFmtHeader2->lpFmtHeadUp = NULL;               // Filled in by ZeroMemory
        lpFmtHeader = lpFmtHeader2;
#ifdef DEBUG
        lpFmtHeader->Sig.nature  = FMTHEADER_SIGNATURE;
#endif
////////lpFmtHeader->lpFmtRowUp  = NULL;                // Already zero from ZeroMemory
////////lpFmtHeader->lpFmtColUp  = NULL;                // ...
////////lpFmtHeader->lpFmtRow1st =                      // Filled in below
////////lpFmtHeader->lpFmtCol1st =                      // ...
        lpFmtHeader->uRealRows   = (UINT) aplDimNRows;  // # real rows from dimensions
////////lpFmtHeader->uActRows    = 0;                   // Already zero from ZeroMemory (initial value)
        lpFmtHeader->uActCols    = (UINT) aplChrNCols;
////////lpFmtHeader->uFmtRows    = 0;                   // Already zero from ZeroMemory (initial value)
////////lpFmtHeader->uFmtInts    = 0;                   // ...
////////lpFmtHeader->uFmtChrs    = 0;                   // ...
////////lpFmtHeader->uFmtFrcs    = 0;                   // ...
////////lpFmtHeader->uFmtTrBl    = 0;                   // ...
////////lpFmtHeader->uMatRes     = 0;                   // ...

        // Create <aplChrNCols> FMTCOLSTRs in the output
        lpFmtColStr = (LPFMTCOLSTR) (&lpFmtHeader[1]);
        lpFmtHeader->lpFmtCol1st = lpFmtColStr;
        Assert (aplChrNCols EQ (APLU3264) aplChrNCols);
        ZeroMemory (lpFmtColStr, (APLU3264) aplChrNCols * sizeof (FMTCOLSTR));
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
        if (aplDimNRows NE 0)
            lpFmtHeader->lpFmtRow1st = (LPFMTROWSTR) lpaplCharStart;
        else
            lpFmtHeader->lpFmtRow1st = NULL;

        // Loop through the array appending the formatted values (separated by WC_EOS)
        //   to the output vector, and accumulating the values in the appropriate
        //   FMTCOLSTR & FMTROWSTR entries.

        // Split cases based upon the array's storage type
        switch (aplType)
        {
            case ARRAY_BOOL:
                lpaplChar =
                  CompileArrBool    ((LPAPLBOOL)   lpMemArr,// Ptr to right arg memory
                                     lpFmtHeader,           // Ptr to parent header
                                     lpFmtColStr,           // Ptr to vector of ColStrs
                                     lpaplCharStart,        // Ptr to compiled output
                                     aplDimNRows,           // # rows
                                     aplDimNCols,           // # cols
                                     aplRank,               // Right arg rank
                                     lpMemDim,              // Ptr to right arg dimensions
                                     TRUE);                 // TRUE iff top level array
                break;

            case ARRAY_CHAR:
                lpaplChar =
                  CompileArrChar    ((LPAPLCHAR)   lpMemArr,// Ptr to right arg memory
                                     lpFmtHeader,           // Ptr to parent header
                                     lpFmtColStr,           // Ptr to vector of ColStrs
                                     lpaplCharStart,        // Ptr to compiled output
                                     aplDimNRows,           // # rows
                                     aplDimNCols,           // # cols
                                     aplRank,               // Right arg rank
                                     lpMemDim,              // Ptr to right arg dimensions
                                     TRUE);                 // TRUE iff top level array
                break;

            case ARRAY_HETERO:
                lpaplChar =
                  CompileArrHetero  ((LPAPLHETERO) lpMemArr,// Ptr to right arg memory
                                     lpFmtHeader,           // Ptr to parent header
                                     lpFmtColStr,           // Ptr to vector of ColStrs
                                     lpaplCharStart,        // Ptr to compiled output
                                     aplDimNRows,           // # rows
                                     aplDimNCols,           // # cols
                                     aplRank,               // Right arg rank
                                     lpMemDim,              // Ptr to right arg dimensions
                                     TRUE,                  // TRUE iff top level array
                                     TRUE);                 // TRUE iff handle []TCLF specially
                break;

            case ARRAY_NESTED:
                lpaplChar =
                  CompileArrNested  ((LPAPLNESTED) lpMemArr,// Ptr to right arg memory
                                     lpFmtHeader,           // Ptr to parent header
                                     lpFmtColStr,           // Ptr to vector of ColStrs
                                     lpaplCharStart,        // Ptr to compiled output
                                     aplDimNRows,           // # rows
                                     aplDimNCols,           // # cols
                                     aplRank,               // Right arg rank
                                     lpMemDim,              // Ptr to right arg dimensions
                                     TRUE);                 // TRUE iff top level array
                break;

            case ARRAY_INT:
            case ARRAY_APA:
            case ARRAY_HC2I:
            case ARRAY_HC4I:
            case ARRAY_HC8I:
                lpaplChar =
                  CompileArrHCxI    (lpMemArr,              // Ptr to right arg memory
                                     lpFmtHeader,           // Ptr to parent header
                                     lpFmtColStr,           // Ptr to vector of ColStrs
                                     lpaplCharStart,        // Ptr to compiled output
                                     aplDimNRows,           // # rows
                                     aplDimNCols,           // # cols
                                     aplType,               // Right arg storage type
                                     aplRank,               // Right arg rank
                                     lpMemDim,              // Ptr to right arg dimensions
                                     TRUE);                 // TRUE iff top level array
                break;

            case ARRAY_FLOAT:
            case ARRAY_HC2F:
            case ARRAY_HC4F:
            case ARRAY_HC8F:
                lpaplChar =
                  CompileArrHCxF    (lpMemArr,              // Ptr to right arg memory
                                     lpFmtHeader,           // Ptr to parent header
                                     lpFmtColStr,           // Ptr to vector of ColStrs
                                     lpaplCharStart,        // Ptr to compiled output
                                     aplDimNRows,           // # rows
                                     aplDimNCols,           // # cols
                                     aplType,               // Right arg storage type
                                     aplRank,               // Right arg rank
                                     lpMemDim,              // Ptr to right arg dimensions
                                     TRUE);                 // TRUE iff top level array
                break;

            case ARRAY_RAT:
            case ARRAY_HC2R:
            case ARRAY_HC4R:
            case ARRAY_HC8R:
                lpaplChar =
                  CompileArrHCxR    (lpMemArr,              // Ptr to right arg memory
                                     lpFmtHeader,           // Ptr to parent header
                                     lpFmtColStr,           // Ptr to vector of ColStrs
                                     lpaplCharStart,        // Ptr to compiled output
                                     aplDimNRows,           // # rows
                                     aplDimNCols,           // # cols
                                     aplType,               // Right arg storage type
                                     aplRank,               // Right arg rank
                                     lpMemDim,              // Ptr to right arg dimensions
                                     TRUE);                 // TRUE iff top level array
                break;

            case ARRAY_VFP:
            case ARRAY_HC2V:
            case ARRAY_HC4V:
            case ARRAY_HC8V:
                lpaplChar =
                  CompileArrHCxV    (lpMemArr,              // Ptr to right arg memory
                                     lpFmtHeader,           // Ptr to parent header
                                     lpFmtColStr,           // Ptr to vector of ColStrs
                                     lpaplCharStart,        // Ptr to compiled output
                                     aplDimNRows,           // # rows
                                     aplDimNCols,           // # cols
                                     aplType,               // Right arg storage type
                                     aplRank,               // Right arg rank
                                     lpMemDim,              // Ptr to right arg dimensions
                                     TRUE);                 // TRUE iff top level array
                break;

            case ARRAY_ARB:
            case ARRAY_BA2F:
            case ARRAY_BA4F:
            case ARRAY_BA8F:
                lpaplChar =
                  CompileArrBAxF    (lpMemArr,              // Ptr to right arg memory
                                     lpFmtHeader,           // Ptr to parent header
                                     lpFmtColStr,           // Ptr to vector of ColStrs
                                     lpaplCharStart,        // Ptr to compiled output
                                     aplDimNRows,           // # rows
                                     aplDimNCols,           // # cols
                                     aplType,               // Right arg storage type
                                     aplRank,               // Right arg rank
                                     lpMemDim,              // Ptr to right arg dimensions
                                     TRUE);                 // TRUE iff top level array
                break;

            defstop
                break;
        } // End SWITCH

        if (lpaplChar EQ NULL)
            goto ERROR_EXIT;

        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;

        // Propagate the row & col count up the line
        PropagateRowColCount (lpFmtHeader);

        // lpaplCharStart now contains the compiled version of the output

        // Add up the width of each column to get the
        //   # cols in the result
        for (aplLastDim = aplDimCol = 0; aplDimCol < aplChrNCols; aplDimCol++)
            aplLastDim += ExteriorColWidth (&lpFmtColStr[aplDimCol]);
        Assert (aplLastDim EQ ExteriorHdrWidth (lpFmtHeader));

        // Calculate the NELM of the result
        aplNELMRes = lpFmtHeader->uFmtRows * aplLastDim;

        // Get ptr to formatting save area
        lpaplChar = lpwszFormat;

#ifdef PREFILL
        // Fill the output area with all zeros
        // Ensure at least one value filled in case the char array is empty
        if (IsSimpleChar (aplType))
            ZeroMemory (lpaplChar, (APLU3264) max (aplNELMRes, 1) * sizeof (APLCHAR));
        else
        {
            // Check for WS FULL
            if (aplNELMRes NE (APLU3264) aplNELMRes)
                goto WSFULL_EXIT;

            // Fill the output area with all blanks
            FillMemoryW (lpaplChar, (APLU3264) aplNELMRes, L' ');
        } // End IF/ELSE
#endif
        // Run through the array again processing the
        //   output stream into lpaplChar

        // Calc when to use raw output
        bRawOut = (!IsNested (aplType) && !IsSimpleHet (aplType) && !IsSimpleChar (aplType));

        // Split cases based upon the array's storage type
        switch (aplType)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_FLOAT:
            case ARRAY_CHAR:
            case ARRAY_APA:
            case ARRAY_HETERO:
            case ARRAY_RAT:
            case ARRAY_VFP:
            case ARRAY_HC2I:
            case ARRAY_HC2F:
            case ARRAY_HC2R:
            case ARRAY_HC2V:
            case ARRAY_HC4I:
            case ARRAY_HC4F:
            case ARRAY_HC4R:
            case ARRAY_HC4V:
            case ARRAY_HC8I:
            case ARRAY_HC8F:
            case ARRAY_HC8R:
            case ARRAY_HC8V:
            case ARRAY_BA1F:
            case ARRAY_BA2F:
            case ARRAY_BA4F:
            case ARRAY_BA8F:
                lpaplChar =
                  FormatArrSimple (lpFmtHeader,             // Ptr to FMTHEADER
                                   lpFmtColStr,             // Ptr to vector of <aplChrNCols> FMTCOLSTRs
                                   lpaplCharStart,          // Ptr to compiled input
                                  &lpaplChar,               // Ptr to output string
                                   lpFmtHeader->uActRows,   // # formatted rows in this array
                                   aplDimNCols,             // # formatted cols in this array
                                   aplLastDim,              // Length of last dim in result
                                   aplRank,                 // Rank of this array
                                   lpMemDim,                // Ptr to this array's dimensions
                                   aplType,                 // Storage type of this array
                                   TRUE,                    // TRUE iff skip to next row after this item
                                   bRawOut,                 // TRUE iff raw (not {thorn}) output
                                   bEndingCR,               // TRUE iff last line has CR
                                   lpbCtrlBreak);           // Ptr to Ctrl-Break flag
                break;

            case ARRAY_NESTED:
                lpaplChar =
                  FormatArrNested (lpFmtHeader,             // Ptr to FMTHEADER
                                   lpMemArr,                // Ptr to raw input
                                   lpFmtColStr,             // Ptr to vector of <aplDimNCols> FMTCOLSTRs
                                   lpaplCharStart,          // Ptr to compiled input
                                  &lpaplChar,               // Ptr to ptr to output string
                                   lpFmtHeader->uActRows,   // # formatted rows in this array
                                   aplDimNCols,             // # formatted cols ...
                                   aplRank,                 // Rank of this array
                                   lpMemDim,                // Ptr to this array's dimensions
                                   aplLastDim,              // Length of last dim in result
                                   bRawOut,                 // TRUE iff raw (not {thorn}) output
                                   bEndingCR,               // TRUE iff last line has CR
                                   lpbCtrlBreak);           // Ptr to Ctrl-Break flag
                break;

            defstop
                break;
        } // End SWITCH

        if (lpaplChar EQ NULL)
            goto ERROR_EXIT;

        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;

        // If we didn't use raw output in the
        //   FormatArrxxx routines, do it now
        // The following code handles wrapping at []PW
        //   of groups of rows
        if (!bRawOut)
        {
            APLUINT     uColGrp,
                        uColLim;
            WCHAR       wcCur = L' ';       // The replaced WCHAR (start with anything non-zero)
            UINT        uFmtRow;            // Loop counter
            LPAPLCHAR   lpwsz;              // Temporary ptr
            LPWCHAR     lpwszTemp,          // Ptr to formatting temp area
                        lpwszOrigTemp;      // ...    original value ...
            LPROWPTRS   lpRowPtrs;
            VARS_TEMP_OPEN

            // Save the original ptr to restore later
            lpwszOrigTemp = lpMemPTD->lpwszTemp;
            CHECK_TEMP_OPEN
            EXIT_TEMP_OPEN

            // Reserve space for Nxt and End Char ptrs
            lpRowPtrs = (LPVOID) lpMemPTD->lpwszTemp;
            lpMemPTD->lpwszTemp += (sizeof (ROWPTRS) * lpFmtHeader->uFmtRows);

            // Save the data ptr for each row
            for (uFmtRow = 0,
                   lpwsz = lpwszFormat;
                 uFmtRow < lpFmtHeader->uFmtRows;
                 uFmtRow++)
            {
                lpRowPtrs[uFmtRow].lpNxtChar = lpwsz;
                lpwsz += aplLastDim;
                lpRowPtrs[uFmtRow].lpEndChar = lpwsz;
            } // End FOR

            // Get ptr to formatting temp area
            //   and protect by skipping over it
            //   taking into account tab stops
            lpwszTemp = lpMemPTD->lpwszTemp;
            lpMemPTD->lpwszTemp += (uTabStops * aplLastDim) + 1;    // "+ 1" for the terminating zero

            // Insert a terminating zero
            *lpaplChar = WC_EOS;

            // Calculate the # of cols per row group
            //   taking into account the six-space indent for the
            //   second and subsequent lines.
            // The calculation is to find the smallest N such that
            //   aplLastDim <= []PW + (N - 1) x ([]PW - 6)
            // The lefthand []PW is for the first row, and (N - 1)
            //   counts the second and subsequent rows.
            //   Simplifying the above inequality...
            //   (aplLastDim - []PW) <= (N - 1) x ([]PW - 6)
            //   (aplLastDim - []PW) / ([]PW - 6) <= N - 1
            //   ceil ((aplLastDim - []PW) / ([]PW - 6)) == N - 1
            //   N == 1 + (aplLastDim - []PW + []PW - 7) / ([]PW - 6)
            //   N == 1 + (aplLastDim - 7) / ([]PW - 6)
            // We also must take into account lines shorter than 7 chars
            //   as the subtraction will go negative (or positive large
            //   as aplLastDim is unsigned).
            uColLim = 1 + (max (0, (int) aplLastDim - 7)) / (uQuadPW - 6);

            // Loop through the groups of cols
            for (uColGrp = 0; uColGrp < uColLim; uColGrp++)
            {
                // Loop through the formatted rows
                for (uFmtRow = 0;
                     uFmtRow < lpFmtHeader->uFmtRows;
                     uFmtRow++)
                {
                    APLUINT uColCur,            // ...
                            uTmp,               // Temporary
                            uMaxPos,            // Maximum col position
                            uCurPos;            // Current col position

                    // ***FIXME*** -- this routine may split a number in half
                    //                because it doesn't know the difference
                    //                between numbers and characters

                    // Set start and end of input
                    lpwszNxt = lpRowPtrs[uFmtRow].lpNxtChar;
                    lpwszEnd = lpRowPtrs[uFmtRow].lpEndChar;

                    if (lpwszNxt >= lpwszEnd)
                        break;

                    // Line Continue second and subsequent column groups
                    bLineCont = (uColGrp > 0);

                    // If we're Line Continuing, ...
                    if (bLineCont)
                    {
                        strcpyW (lpwszTemp, wszIndent);                 // Copy the indent
                        uOutLen = uQuadPW - DEF_INDENT;                 // Maximum output length
                        uCurPos = DEF_INDENT;
                    } else
                    {
                        uOutLen = uQuadPW;                              // Maximum output length
                        uCurPos = 0;
                    } // End IF/ELSE

                    // Set the current maximum position
                    uMaxPos = uCurPos;

                    // Loop through this part of the output line
                    for (uColCur = 0; (uColCur < uOutLen) && lpwszNxt < lpwszEnd; uColCur++)
                    {
                        // Get the next char
                        wcCur = *lpwszNxt++;

                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Split cases based upon the character
                        switch (wcCur)
                        {
                            // N.B.  The cases for CR & LF *MUST* come first as they
                            //       have different test conditions than the others
                            case WC_CR:         // []TCNL -- Restart at the beginning of a new line
                                // If we're NOT Output Debugging,
                                //   or we are, but we're excluding CR & LF from display, ...
                                if (!OptionFlags.bOutputDebug || OptionFlags.bOutputExCRLF)
                                {
                                    // Zap the temp buffer at the maximum position
                                    lpwszTemp[uMaxPos] = WC_EOS;

                                    // Reset the current position (but not the max width)
                                    uCurPos   =
                                    uColCur   = 0;
                                    uColCur--;              // Count it out
                                    uOutLen = uQuadPW;      // Maximum output length

                                    break;
                                } else
                                {
                                    // Fall through to common code
                                    // Note that bOutputDebug and bOutputExCRLF are FALSE,
                                    //   so the next IF statement (if present) falls through
                                } // End IF/ELSE

                            case WC_LF:         // []TCLF -- Start a new line
                                // If we're NOT Output Debugging,
                                //   or we are, but we're excluding CR & LF from display, ...
                                if (!OptionFlags.bOutputDebug || OptionFlags.bOutputExCRLF)
                                {
                                    // Ensure properly terminated at the max position
                                    lpwszTemp[uMaxPos] = WC_EOS;

                                    // Output the line up to this point w/o NL
                                    AppendLine (lpwszTemp, bLineCont, FALSE);

                                    // Mark the following lines as continued
                                    bLineCont = TRUE;

                                    // Output enough blanks to get back to the current position
                                    FillMemoryW (lpwszTemp, (APLU3264) uCurPos, L' ');

                                    // Reset the maximum position
                                    uMaxPos = uCurPos;

                                    break;
                                } else
                                {
                                    // Fall through to common code
                                    // Note that bOutputDebug and bOutputExCRLF are FALSE,
                                    //   so the next IF statement (if present) falls through
                                } // End IF/ELSE

                            case WC_HT:         // []TCHT -- Move ahead to the next tab stop
                                // If we're NOT Output Debugging, ...
                                if (!OptionFlags.bOutputDebug)
                                {
                                    // *Insert* enough blanks to get to the next tabstop
                                    uTmp = uTabStops - (uCurPos % uTabStops);
                                    MoveMemoryW (&lpwszTemp[uCurPos + uTmp], &lpwszTemp[uCurPos], (APLU3264) uTmp);
                                    FillMemoryW (&lpwszTemp[uCurPos], (APLU3264) uTmp, L' ');

                                    // Increase the maximum and current positions by the # inserted blanks
                                    uCurPos += uTmp;
                                    uMaxPos  = max (uMaxPos, uCurPos);

                                    break;
                                } else
                                {
                                    // Fall through to common code
                                    // Note that bOutputDebug is FALSE,
                                    //   so the next IF statement (if present) falls through
                                } // End IF/ELSE

                            case WC_BEL:        // []TCBEL -- Sound the alarum!
                                // If we're NOT Output Debugging, ...
                                if (!OptionFlags.bOutputDebug)
                                {
                                    MessageBeep (NEG1U);

                                    break;
                                } else
                                {
                                    // Fall through to common code
                                    // Note that bOutputDebug is FALSE,
                                    //   so the next IF statement (if present) falls through
                                } // End IF/ELSE

                            case WC_BS:         // []TCBS -- Backspace if there's room
                                // If we're NOT Output Debugging, ...
                                if (!OptionFlags.bOutputDebug)
                                {
                                    if (uCurPos NE 0)
                                        uCurPos--;
                                    break;
                                } else
                                {
                                    // Fall through to common code
                                    // Note that bOutputDebug is FALSE,
                                    //   so the next IF statement (if present) falls through
                                } // End IF/ELSE

                            default:
                                // Save the new char
                                lpwszTemp[uCurPos++] = wcCur;
                                uMaxPos = max (uMaxPos, uCurPos);

                                break;
                        } // End SWITCH
                    } // End FOR

                    // Save ptr to next input
                    lpRowPtrs[uFmtRow].lpNxtChar = lpwszNxt;

                    // Ensure properly terminated at the maximum position
                    lpwszTemp[uMaxPos] = WC_EOS;            // Zap it

                    // If we're in the first ColGrp and not the last row, ...
                    if (uColGrp EQ 0
                     && uFmtRow NE (lpFmtHeader->uFmtRows - 1))
                        AppendLine (lpwszTemp, bLineCont, bEndingCR);       // Display the line
                    else
                        AppendLine (lpwszTemp, bLineCont, FALSE);           // Display the line
                } // End FOR

                // If we're not at the last line, and
                //   the array is multirank, ...
                // ***FIXME*** -- Sometimes on matrices whose lines contain TCNLs at low []PW,
                //                we may display extra blank line(s) at the end because we
                //                use up uColGrps faster than does the FOR loop.
                if (uColGrp EQ (uColLim - 1)
                 || IsRank2P (aplRank))
                    // Display a blank separator line
                    AppendLine (L"", FALSE, TRUE);
            } // End FOR

            // Restore original ptr to formatting temp area
            lpMemPTD->lpwszTemp = lpwszOrigTemp;
        } // End IF
    } __except (CheckVirtAlloc (GetExceptionInformation (), WFCN))
    {
        // Split cases based upon the exception code
        switch (MyGetExceptionCode ())
        {
            case EXCEPTION_LIMIT_ERROR:
                goto LIMIT_EXIT;

            defstop
                break;
        } // End SWITCH
    } // End __try/__except

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LIMIT_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LIMIT_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

NORMAL_EXIT:
    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    // Restore previous heapFull
    *heapFull = *oldHeapFull;

    // If we allocated virtual storage, ...
    if (lclMemVirtStr[0].IniAddr)
    {
        // Free the virtual storage
        MyVirtualFree (lclMemVirtStr[0].IniAddr, 0, MEM_RELEASE); lclMemVirtStr[0].IniAddr = NULL;
        lpaplCharIni = NULL;

        // Unlink this struc from the chain
        UnlinkMVS (&lclMemVirtStr[0]);
    } // End IF

    if (hGlbArr NE NULL && lpMemHdrArr NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbArr); lpMemHdrArr = NULL;
    } // End IF

    return bRet;
} // End DisplayGlbArr_EM


//***************************************************************************
//  $FormatImmed
//
//  Format an immediate constant
//***************************************************************************

LPAPLCHAR FormatImmed
    (LPWCHAR      lpaplChar,
     IMM_TYPES    ImmType,
     LPAPLLONGEST lpaplLongest)

{
    LPVARARRAY_HEADER lpMemHdr;
    LPVOID            lpMemGlbNum;
    WCHAR             wcCur;

    // Split cases based upon the immediate type
    switch (ImmType)
    {
        case IMMTYPE_BOOL:
            lpaplChar =
              FormatAplIntFC (lpaplChar,            // Ptr to output save area
          BIT0 & *(LPAPLBOOL) lpaplLongest,         // The value to format
                              NegSign);             // Char to use as overbar
            break;

        case IMMTYPE_INT:
            lpaplChar =
              FormatAplInt (lpaplChar,              // Ptr to output save area
                 (LPAPLINT) lpaplLongest);          // Ptr to the value to format
            break;

        case IMMTYPE_CHAR:
            // Save the char
            wcCur = *(LPAPLCHAR) lpaplLongest;

            // If we're Output Debugging, ...
            if (OptionFlags.bOutputDebug
             && wcCur EQ WC_EOS)
                *lpaplChar++ = UTF16_REPLACEMENT0000;
            else
                *lpaplChar++ = *(LPAPLCHAR) lpaplLongest;
            *lpaplChar++ = L' ';                // Append a blank to be deleted

            break;

        case IMMTYPE_FLOAT:
            lpaplChar =
              FormatAplFlt (lpaplChar,      // Ptr to output save area
               (LPAPLFLOAT) lpaplLongest,   // Ptr to the value to format
                            0);             // Use default significant digits
            break;

        case IMMTYPE_RAT:
            lpaplChar =
              FormatAplRat (lpaplChar,      // Ptr to output save area
                 (LPAPLRAT)  VarArrayDataFmBase (ClrPtrTypeDir (lpaplLongest)));    // Ptr to the value to format
            break;

        case IMMTYPE_VFP:
            lpaplChar =
              FormatAplVfp (lpaplChar,      // Ptr to output save area
                 (LPAPLVFP)  VarArrayDataFmBase (ClrPtrTypeDir (lpaplLongest)),     // Ptr to the value to format
                            GetQuadPPV ());     // Use this many significant digits for VFP
            break;

        case IMMTYPE_HC2I:
            lpaplChar =
              FormatAplHC2I (lpaplChar,     // Ptr to output save area
                 (LPAPLHC2I) VarArrayDataFmBase (ClrPtrTypeDir (lpaplLongest)));    // Ptr to the value to format
            break;

        case IMMTYPE_HC4I:
            lpaplChar =
              FormatAplHC4I (lpaplChar,     // Ptr to output save area
                 (LPAPLHC4I) VarArrayDataFmBase (ClrPtrTypeDir (lpaplLongest)));    // Ptr to the value to format
            break;

        case IMMTYPE_HC8I:
            lpaplChar =
              FormatAplHC8I (lpaplChar,     // Ptr to output save area
                 (LPAPLHC8I) VarArrayDataFmBase (ClrPtrTypeDir (lpaplLongest)));    // Ptr to the value to format
            break;

        case IMMTYPE_HC2F:
            lpaplChar =
              FormatAplHC2F (lpaplChar,     // Ptr to output save area
                 (LPAPLHC2F) VarArrayDataFmBase (ClrPtrTypeDir (lpaplLongest)),     // Ptr to the value to format
                             0);            // Use default significant digits
            break;

        case IMMTYPE_HC4F:
            lpaplChar =
              FormatAplHC4F (lpaplChar,     // Ptr to output save area
                 (LPAPLHC4F) VarArrayDataFmBase (ClrPtrTypeDir (lpaplLongest)),     // Ptr to the value to format
                             0);            // Use default significant digits
            break;

        case IMMTYPE_HC8F:
            lpaplChar =
              FormatAplHC8F (lpaplChar,     // Ptr to output save area
                 (LPAPLHC8F) VarArrayDataFmBase (ClrPtrTypeDir (lpaplLongest)),     // Ptr to the value to format
                             0);            // Use default significant digits
            break;

        case IMMTYPE_HC2R:
            lpaplChar =
              FormatAplHC2R (lpaplChar,     // Ptr to output save area
                 (LPAPLHC2R) VarArrayDataFmBase (ClrPtrTypeDir (lpaplLongest)));    // Ptr to the value to format
            break;

        case IMMTYPE_HC4R:
            lpaplChar =
              FormatAplHC4R (lpaplChar,     // Ptr to output save area
                 (LPAPLHC4R) VarArrayDataFmBase (ClrPtrTypeDir (lpaplLongest)));    // Ptr to the value to format
            break;

        case IMMTYPE_HC8R:
            lpaplChar =
              FormatAplHC8R (lpaplChar,     // Ptr to output save area
                 (LPAPLHC8R) VarArrayDataFmBase (ClrPtrTypeDir (lpaplLongest)));    // Ptr to the value to format
            break;

        case IMMTYPE_HC2V:
            lpaplChar =
              FormatAplHC2V (lpaplChar,     // Ptr to output save area
                 (LPAPLHC2V) VarArrayDataFmBase (ClrPtrTypeDir (lpaplLongest)),     // Ptr to the value to format
                             GetQuadPPV ());// Use this many significant digits for VFP
            break;

        case IMMTYPE_HC4V:
            lpaplChar =
              FormatAplHC4V (lpaplChar,     // Ptr to output save area
                 (LPAPLHC4V) VarArrayDataFmBase (ClrPtrTypeDir (lpaplLongest)),     // Ptr to the value to format
                             GetQuadPPV ());// Use this many significant digits for VFP
            break;

        case IMMTYPE_HC8V:
            lpaplChar =
              FormatAplHC8V (lpaplChar,     // Ptr to output save area
                 (LPAPLHC8V) VarArrayDataFmBase (ClrPtrTypeDir (lpaplLongest)),     // Ptr to the value to format
                             GetQuadPPV ());// Use this many significant digits for VFP
            break;

        case IMMTYPE_ARB:
            lpaplChar =
              FormatAplArb (lpaplChar,      // Ptr to output save area
                 (LPAPLARB)  VarArrayDataFmBase (ClrPtrTypeDir (lpaplLongest)),     // Ptr to the value to format
                            GetQuadPPV ());     // Use this many significant digits for VFP
            break;

        case IMMTYPE_BA2F:
            lpaplChar =
              FormatAplBA2F (lpaplChar,     // Ptr to output save area
                 (LPAPLBA2F) VarArrayDataFmBase (ClrPtrTypeDir (lpaplLongest)),     // Ptr to the value to format
                             GetQuadPPV ());// Use this many significant digits for VFP
            break;

        case IMMTYPE_BA4F:
            lpaplChar =
              FormatAplBA4F (lpaplChar,     // Ptr to output save area
                 (LPAPLBA4F) VarArrayDataFmBase (ClrPtrTypeDir (lpaplLongest)),     // Ptr to the value to format
                             GetQuadPPV ());// Use this many significant digits for VFP
            break;

        case IMMTYPE_BA8F:
            lpaplChar =
              FormatAplBA8F (lpaplChar,     // Ptr to output save area
                 (LPAPLBA8F) VarArrayDataFmBase (ClrPtrTypeDir (lpaplLongest)),     // Ptr to the value to format
                             GetQuadPPV ());// Use this many significant digits for VFP
            break;

        case IMMTYPE_ERROR:
            Assert (GetPtrTypeDir (lpaplLongest) EQ PTRTYPE_HGLOBAL);

            // Lock the memory to get a ptr to it
            lpMemHdr = MyGlobalLockVar (lpaplLongest);

            // Skip over the header and dimensions to the data
            lpMemGlbNum = VarArrayDataFmBase (lpMemHdr);

            // Split cases based upon the storage type
            switch (lpMemHdr->ArrType)
            {
                case ARRAY_RAT:
                    lpaplChar =
                      FormatAplRat (lpaplChar,      // Ptr to output save area
                         (LPAPLRAT) lpMemGlbNum);   // Ptr to the value to format
                    break;

                case ARRAY_VFP:
                    lpaplChar =
                      FormatAplVfp (lpaplChar,      // Ptr to output save area
                         (LPAPLVFP) lpMemGlbNum,    // Ptr to the value to format
                                    GetQuadPPV ()); // Use this many significant digits for VFP
                    break;

                defstop
                    break;
            } // End SWITCH

            // We no longer need this ptr
            MyGlobalUnlock (lpaplLongest); lpMemHdr = NULL; lpMemGlbNum = NULL;

            break;

        defstop
            break;
    } // End SWITCH

    return lpaplChar;
} // End FormatImmed


//***************************************************************************
//  $FormatImmedFC
//
//  Format an immediate constant using []FC
//***************************************************************************

LPAPLCHAR FormatImmedFC
    (LPWCHAR      lpaplChar,        // Ptr to input string
     IMM_TYPES    ImmType,          // Immediate type
     LPAPLLONGEST lpaplLongest,     // Ptr to value to format
     APLUINT      nDigits,          // # significant digits
     APLCHAR      aplCharDecimal,   // Char to use as decimal separator
     APLCHAR      aplCharOverbar,   // Char to use as overbar
     FLTDISPFMT   fltDispFmt,       // Float display format
     UBOOL        bSubstInf)        // TRUE iff we're to substitute text for infinity

{
    WCHAR wc;

    // Split cases based upon the immediate type
    switch (ImmType)
    {
        case IMMTYPE_BOOL:
            lpaplChar =
              FormatAplIntFC (lpaplChar,                        // Ptr to output save area
                              BIT0 & *(LPAPLBOOL) lpaplLongest, // The value to format
                              aplCharOverbar);                  // Char to use as overbar
            break;

        case IMMTYPE_INT:
            lpaplChar =
              FormatAplIntFC (lpaplChar,                        // Ptr to output save area
                             *(LPAPLINT) lpaplLongest,          // The value to format
                              aplCharOverbar);                  // Char to use as overbar
            break;

        case IMMTYPE_CHAR:
            // Get the char
            wc = *(LPAPLCHAR) lpaplLongest;

            // Test for Terminal Control chars
            switch (wc)
            {
                // N.B.  The cases for CR & LF *MUST* come first as they
                //       have different test conditions than the others
                case WC_CR:     // CR
                    // If we're NOT Output Debugging,
                    //   or we are, but we're excluding CR & LF from display, ...
                    if (!OptionFlags.bOutputDebug || OptionFlags.bOutputExCRLF)
                    {
                        *lpaplChar++ = L' ';    // Append a blank to be deleted

                        break;                  // Can't go any farther left
                    } else
                    {
                        // Fall through to common code
                        // Note that Output Debugging is FALSE, so the next
                        //   IF statement (if present) falls through
                    } // End IF/ELSE

                case WC_LF:     // LF       // Handled during raw output
                    // If we're NOT Output Debugging,
                    //   or we are, but we're excluding CR & LF from display, ...
                    if (!OptionFlags.bOutputDebug || OptionFlags.bOutputExCRLF)
                    {
                        *lpaplChar++ = wc;      // Append it
                        *lpaplChar++ = L' ';    // Append a blank to be deleted

                        break;
                    } else
                    {
                        // Fall through to common code
                        // Note that Output Debugging is FALSE, so the next
                        //   IF statement (if present) falls through
                    } // End IF/ELSE

                case WC_BEL:    // Bel
                    // If we're NOT Output Debugging, ...
                    if (!OptionFlags.bOutputDebug)
                    {
                        MessageBeep (NEG1U);    // Sound the alarum!

                        break;
                    } else
                    {
                        // Fall through to common code
                        // Note that Output Debugging is FALSE, so the next
                        //   IF statement (if present) falls through
                    } // End IF/ELSE

                case WC_ESC:    // Esc
                case WC_FF:     // FF
                case WC_EOS:    // NUL
                case WC_BS:     // BS
                    if (!OptionFlags.bOutputDebug)
                    {
                        *lpaplChar++ = L' ';    // Append a blank to be deleted

                        break;                  // Can't go any farther left
                    } else
                    {
                        // Fall through to common code
                        // Note that Output Debugging is FALSE, so the next
                        //   IF statement (if present) falls through
                    } // End IF/ELSE

                case WC_HT:     // HT
                    // If we're NOT Output Debugging, ...
                    if (!OptionFlags.bOutputDebug)
                    {
                        // We're always at the (virtual) left margin,
                        //   so insert enough blanks for a TAB
                        lpaplChar = FillMemoryW (lpaplChar, uTabStops + 1, L' ');
///////////////////////*lpaplChar++ = L' ';     // Append a blank to be deleted

                        break;
                    } else
                    {
                        // Fall through to common code
                        // Note that Output Debugging is FALSE, so the next
                        //   IF statement (if present) falls through
                    } // End IF/ELSE

                default:
                    *lpaplChar++ = wc;          // Append it
                    *lpaplChar++ = L' ';        // Append a blank to be deleted

                    break;
            } // End SWITCH

            break;

        case IMMTYPE_FLOAT:
            lpaplChar =
              FormatAplFltFC (lpaplChar,        // Ptr to output save area
                *(LPAPLFLOAT) lpaplLongest,     // The value to format
                              nDigits,          // # significant digits
                              aplCharDecimal,   // Char to use as decimal separator
                              aplCharOverbar,   // Char to use as overbar
                              fltDispFmt,       // Float display format
                              bSubstInf);       // TRUE iff we're to substitute text for infinity
            break;

        defstop
            break;
    } // End SWITCH

    return lpaplChar;
} // End FormatImmedFC


//***************************************************************************
//  $FormatImmedPtr
//
//  Format an immediate constant from a ptr to it
//***************************************************************************

LPAPLCHAR FormatImmedPtr
    (LPWCHAR      lpaplChar,                // Ptr to output save area
     UINT         ImmType,                  // Immediate type
     LPAPLLONGEST lpaplLongest)             // Ptr to value

{
    // Split cases based upon the immediate type
    switch (ImmType)
    {
        case IMMTYPE_BOOL:
            lpaplChar =
              FormatAplIntFC (lpaplChar,        // Ptr to output save area
          BIT0 & *(LPAPLBOOL) lpaplLongest,     // The value to format
                              NegSign);         // Char to use as overbar
            break;

        case IMMTYPE_INT:
            lpaplChar =
              FormatAplInt (lpaplChar,          // Ptr to output save area
                 (LPAPLINT) lpaplLongest);      // The value to format
            break;

        case IMMTYPE_CHAR:
            *lpaplChar++ = *(LPAPLCHAR) lpaplLongest;
            *lpaplChar++ = L' ';                // Append a blank to be deleted

            break;

        case IMMTYPE_FLOAT:
            lpaplChar =
              FormatAplFlt (lpaplChar,          // Ptr to output save area
               (LPAPLFLOAT) lpaplLongest,       // The value to format
                            0);                 // Use default significant digits
            break;

        case IMMTYPE_RAT:
            lpaplChar =
              FormatAplRat (lpaplChar,          // Ptr to output save area
                 (LPAPLRAT) lpaplLongest);      // The value to format
            break;

        case IMMTYPE_VFP:
            lpaplChar =
              FormatAplVfp (lpaplChar,          // Ptr to output save area
                 (LPAPLVFP) lpaplLongest,       // The value to format
                            GetQuadPPV ());     // Use this many significant digits for VFP
            break;

        case IMMTYPE_HC2I:
            lpaplChar =
              FormatAplHC2I (lpaplChar,         // Ptr to output save area
                 (LPAPLHC2I) lpaplLongest);     // Ptr to the value to format
            break;

        case IMMTYPE_HC4I:
            lpaplChar =
              FormatAplHC4I (lpaplChar,         // Ptr to output save area
                 (LPAPLHC4I) lpaplLongest);     // Ptr to the value to format
            break;

        case IMMTYPE_HC8I:
            lpaplChar =
              FormatAplHC8I (lpaplChar,         // Ptr to output save area
                 (LPAPLHC8I) lpaplLongest);     // Ptr to the value to format
            break;

        case IMMTYPE_HC2F:
            lpaplChar =
              FormatAplHC2F (lpaplChar,         // Ptr to output save area
                 (LPAPLHC2F) lpaplLongest,      // Ptr to the value to format
                             0);                // Use default significant digits
            break;

        case IMMTYPE_HC4F:
            lpaplChar =
              FormatAplHC4F (lpaplChar,         // Ptr to output save area
                 (LPAPLHC4F) lpaplLongest,      // Ptr to the value to format
                             0);                // Use default significant digits
            break;

        case IMMTYPE_HC8F:
            lpaplChar =
              FormatAplHC8F (lpaplChar,         // Ptr to output save area
                 (LPAPLHC8F) lpaplLongest,      // Ptr to the value to format
                             0);                // Use default significant digits
            break;

        case IMMTYPE_HC2R:
            lpaplChar =
              FormatAplHC2R (lpaplChar,         // Ptr to output save area
                 (LPAPLHC2R) lpaplLongest);     // Ptr to the value to format
            break;

        case IMMTYPE_HC4R:
            lpaplChar =
              FormatAplHC4R (lpaplChar,         // Ptr to output save area
                 (LPAPLHC4R) lpaplLongest);     // Ptr to the value to format
            break;

        case IMMTYPE_HC8R:
            lpaplChar =
              FormatAplHC8R (lpaplChar,         // Ptr to output save area
                 (LPAPLHC8R) lpaplLongest);     // Ptr to the value to format
            break;

        case IMMTYPE_HC2V:
            lpaplChar =
              FormatAplHC2V (lpaplChar,         // Ptr to output save area
                 (LPAPLHC2V) lpaplLongest,      // Ptr to the value to format
                             GetQuadPPV ());    // Use this many significant digits for VFP
            break;

        case IMMTYPE_HC4V:
            lpaplChar =
              FormatAplHC4V (lpaplChar,         // Ptr to output save area
                 (LPAPLHC4V) lpaplLongest,      // Ptr to the value to format
                             GetQuadPPV ());    // Use this many significant digits for VFP
            break;

        case IMMTYPE_HC8V:
            lpaplChar =
              FormatAplHC8V (lpaplChar,         // Ptr to output save area
                 (LPAPLHC8V) lpaplLongest,      // Ptr to the value to format
                             GetQuadPPV ());    // Use this many significant digits for VFP
            break;

        case IMMTYPE_BA1F:
            lpaplChar =
              FormatAplArb  (lpaplChar,         // Ptr to output save area
                 (LPAPLARB)  lpaplLongest,      // Ptr to the value to format
                             GetQuadPPV ());    // Use this many significant digits for VFP
            break;

        case IMMTYPE_BA2F:
            lpaplChar =
              FormatAplBA2F (lpaplChar,         // Ptr to output save area
                 (LPAPLBA2F) lpaplLongest,      // Ptr to the value to format
                             GetQuadPPV ());    // Use this many significant digits for VFP
            break;

        case IMMTYPE_BA4F:
            lpaplChar =
              FormatAplBA4F (lpaplChar,         // Ptr to output save area
                 (LPAPLBA4F) lpaplLongest,      // Ptr to the value to format
                             GetQuadPPV ());    // Use this many significant digits for VFP
            break;

        case IMMTYPE_BA8F:
            lpaplChar =
              FormatAplBA8F (lpaplChar,         // Ptr to output save area
                 (LPAPLBA8F) lpaplLongest,      // Ptr to the value to format
                             GetQuadPPV ());    // Use this many significant digits for VFP
            break;

        defstop
            break;
    } // End SWITCH

    return lpaplChar;
} // End FormatImmedPtr


//***************************************************************************
//  $FormatAplInt
//
//  Format an APLINT
//***************************************************************************

LPAPLCHAR FormatAplInt
    (LPAPLCHAR lpaplChar,           // Ptr to output save area
     LPVOID    lpaplInt)            // Ptr to the value to format

{
    return
      FormatAplIntFC (lpaplChar,        // Ptr to output save area
          *(LPAPLINT) lpaplInt,         // The value to format
                      NegSign);         // Char to use as overbar
} // End FormatAplInt


//***************************************************************************
//  $FormatAplIntFC
//
//  Format an APLINT using []FC
//***************************************************************************

LPAPLCHAR FormatAplIntFC
    (LPAPLCHAR lpaplChar,           // Ptr to output save area
     APLINT    aplInt,              // Integer to format
     APLCHAR   aplCharOverbar)      // Char to use as overbar

{

#define MAXLEN  32

    WCHAR wszTemp[MAXLEN + 1];      // Temp save area
    int   i;                        // Temp
    UINT  u;                        // ...

    // Check the sign bit -- if set, save and make positive
    if (aplInt < 0)
    {
        // Save the overbar
        *lpaplChar++ = aplCharOverbar;

        // If it's the minimum integer, ...
        if (aplInt EQ MIN_APLINT)
        {
            // Copy the constant (skipping over the sign WCHAR) to the result
            strcpyW (lpaplChar, &MIN_APLINT_STR[1]);

            // Skip over it
            lpaplChar += lstrlenW (lpaplChar);

            goto NORMAL_EXIT;
        } // End IF

        // Now it's safe to negate the integer
        aplInt = -aplInt;
    } // End IF

    // Format the number one digit at a time
    //   in reverse order into lpaplChar
    i = MAXLEN - 1;

    // Execute at least once so zero prints
    do
    {
        u = (UINT) (aplInt % 10);
        wszTemp[i--] = u + L'0';
        aplInt = aplInt / 10;
    } while (aplInt NE 0);

    // Copy the digits to the result
    for (i++; i < MAXLEN; i++)
        *lpaplChar++ = wszTemp[i];
NORMAL_EXIT:
    // Include a separator
    *lpaplChar++ = L' ';

    return lpaplChar;
} // End FormatAplIntFC


//***************************************************************************
//  $FormatAplRat
//
//  Format an APLRAT
//***************************************************************************

LPAPLCHAR FormatAplRat
    (LPAPLCHAR lpaplChar,           // Ptr to output save area
     LPVOID    lpaplRat)            // Ptr to the value to format

{
    return
      FormatAplRatFC (lpaplChar,        // Ptr to output save area
                      lpaplRat,         // Ptr to the value to format
                      NegSign,          // Char to use as overbar
                      DEF_RATSEP,       // Char to use as rational separator
                      FALSE,            // TRUE iff we're to substitute text for infinity
                      FALSE);           // TRUE iff this RAT is inside a larger syntax
} // End FormatAplRat


//***************************************************************************
//  $FormatAplRatFC
//
//  Format an APLRAT using []FC
//***************************************************************************

LPAPLCHAR FormatAplRatFC
    (LPAPLCHAR lpaplChar,           // Ptr to output save area
     LPAPLRAT  lpaplRat,            // Ptr to the value to format
     APLCHAR   aplCharOverbar,      // Char to use as overbar
     APLCHAR   aplCharRatSep,       // Char to use as rational separator
     UBOOL     bSubstInf,           // TRUE iff we're to substitute text for infinity
     UBOOL     bInsideSep)          // TRUE iff this RAT is inside a larger syntax

{
    APLINT    iLen,                 // String length
              iRes;                 // Loop counter
    LPAPLCHAR lpw;                  // Ptr to string
    UBOOL     bNeg;                 // TRUE iff the number is negative
    LPCHAR    lpRawFmt;             // Ptr to raw formatted #
    APLUINT   uTotSize;             // Total size of formatted #

    Assert (!IsMpqNULL (lpaplRat));

    lpRawFmt = (LPCHAR) lpaplChar;

    // If the number is NOT +/- infinity, and not a NaN ...
    if (!mpq_inf_p (lpaplRat)
     && !mpq_nan_p (lpaplRat))
    {
        // If the denominator is NE 1, ...
        if (mpz_cmp_ui (mpq_denref (lpaplRat), 1) NE 0)
            // Canonicalize the arg
            mpq_canonicalize (lpaplRat);

        // Calculate the needed size, where both "1 +"s include the sign,
        //   the first "+ 1" includes the 'r' separator and the second
        //   one the terminating zero
        uTotSize = 1 + mpz_sizeinbase (mpq_numref (lpaplRat), 10) + 1 +
                   1 + mpz_sizeinbase (mpq_denref (lpaplRat), 10) + 1;
        // Double as we convert in place
        uTotSize = 2 * uTotSize;

        if (uTotSize NE (APLU3264) uTotSize)
            uTotSize = UINT_MAX;

        // Test the output save area limits
        ZeroMemory (lpRawFmt, (APLU3264) uTotSize);
    } // End IF

    // Format the num/den
    mpq_get_str (lpRawFmt, 10, lpaplRat);

    // Convert a leading minus to aplCharOverbar
    bNeg = (lpRawFmt[0] EQ '-');

    // Get the char length
    iLen = lstrlen (lpRawFmt);

    // Convert the char to WCHAR
    for (iRes = iLen; iRes >= 0; iRes--)
        lpaplChar[iRes] = lpRawFmt[iRes];

    // Convert a leading minus to aplCharOverbar
    if (bNeg)
    {
        *lpaplChar++ = aplCharOverbar;
        iLen--;
    } // End IF

    // Check for NaN
    if (lpaplChar[0] EQ DEF_NAN_CHAR)
    {
        // If we're to substitute, ...
        if (bSubstInf)
            lpaplChar +=
              ConvertWideToNameLength (lpaplChar,   // Ptr to output save buffer
                                      &wcNaN,       // Ptr to incoming chars
                                       1);          // # chars to convert
        else
            *lpaplChar++ = wcNaN;
    } else
    // Check for Infinity
    if (lpaplChar[0] EQ DEF_POSINFINITY_CHAR)
    {
        // If we're to substitute, ...
        if (bSubstInf)
            lpaplChar +=
              ConvertWideToNameLength (lpaplChar,   // Ptr to output save buffer
                                      &wcInf,       // Ptr to incoming chars
                                       1);          // # chars to convert
        else
            *lpaplChar++ = wcInf;
    } else
    {
        // Convert '/' (if any) to aplCharRatSep
        lpw = strchrW (lpaplChar, L'/');
        if (lpw NE NULL)
            *lpw = aplCharRatSep;
        else
        if (OptionFlags.bDispMPSuf)
        {
            if (bInsideSep)
            {
                lpaplChar[iLen++] = aplCharRatSep;
                lpaplChar[iLen++] = L'1';
            } else
                lpaplChar[iLen++] = L'x';
        } // End IF/ELSE

        // Skip over the formatted number
        lpaplChar += iLen;
    } // End IF/ELSE

    // Include a separator
    *lpaplChar++ = L' ';

    return lpaplChar;
} // End FormatAplRatFC


//***************************************************************************
//  $FormatAplFlt
//
//  Format a floating point number
//***************************************************************************

LPAPLCHAR FormatAplFlt
    (LPWCHAR  lpaplChar,        // Ptr to output save area
     LPVOID   lpaplFloat,       // Ptr to the value to format
     APLUINT  nDigits)          // Raw or E-format:  # significant digits
                                // F-format:  # digits to right of decimal sep

{
    return
      FormatAplFltFC (lpaplChar,                // Ptr to output save area
        *(LPAPLFLOAT) lpaplFloat,               // The value to format
                      nDigits,                  // # significant digits (0 = default)
                      DecSep,                   // Char to use as decimal separator
                      NegSign,                  // Char to use as overbar
                      FLTDISPFMT_RAWFLT,        // Float display format
                      FALSE);                   // TRUE iff we're to substitute text for infinity
} // End FormatAplFlt


//***************************************************************************
//  $FormatAplFltFC
//
//  Format a APLFLOAT using []FC
//***************************************************************************

LPAPLCHAR FormatAplFltFC
    (LPWCHAR    lpaplChar,          // Ptr to output save area
     APLFLOAT   aplFloat,           // The value to format
     APLINT     nDigits,            // Raw or E-format:  # significant digits
                                    // F-format:  # digits to right of decimal sep
     APLCHAR    aplCharDecimal,     // Char to use as decimal separator
     APLCHAR    aplCharOverbar,     // Char to use as overbar
     FLTDISPFMT fltDispFmt,         // Float display format (see FLTDISPFMT)
     UBOOL      bSubstInf)          // TRUE iff we're to substitute text for infinity

{
    // Check for NaN
    if (_isnan (aplFloat))
    {
        // If we're to substitute, ...
        if (bSubstInf)
            lpaplChar +=
              ConvertWideToNameLength (lpaplChar,       // Ptr to output save buffer
                                      &wcNaN,           // Ptr to incoming chars
                                       1);              // # chars to convert
        else
            *lpaplChar++ = wcNaN;
    } else
    // Check for Infinity
    if (_isinf (aplFloat))
    {
        // Izit negative?
        if (aplFloat < 0)
            *lpaplChar++ = aplCharOverbar;

        // If we're to substitute, ...
        if (bSubstInf)
            lpaplChar +=
              ConvertWideToNameLength (lpaplChar,       // Ptr to output save buffer
                                      &wcInf,           // Ptr to incoming chars
                                       1);              // # chars to convert
        else
            *lpaplChar++ = wcInf;           // Char for infinity
    } else
    // Check for 0.0
    if (aplFloat EQ 0)
    {
        // If it's negative, ...
        if (SIGN_APLFLOAT (aplFloat))
            *lpaplChar++ = aplCharOverbar;

        // Split cases based upon the float display format
        switch (fltDispFmt)
        {
            case FLTDISPFMT_E:          // 0E0 or 0.0E0 or 0.0---0E0
                if (nDigits EQ 1)
                {
                    // Fill in the result of "0E0"
                    *lpaplChar++ = L'0';
                    *lpaplChar++ = DEF_EXPONENT_UC;
                    *lpaplChar++ = L'0';
                } else
                if (nDigits > 1)
                {
                    // Start with "0."
                    *lpaplChar++ = L'0';
                    nDigits--;
                    *lpaplChar++ = aplCharDecimal;

                    // While there are more digits, ...
                    while (nDigits > 0)
                    {
                        // Fill with leading zeros
                        *lpaplChar++ = L'0';
                        nDigits--;
                    } // End WHILE

                    // End with "E0"
                    *lpaplChar++ = DEF_EXPONENT_UC;
                    *lpaplChar++ = L'0';
                } else
                    DbgStop ();         // We should never get here
                break;

            case FLTDISPFMT_F:
                // Start with a "0"
                *lpaplChar++ = L'0';

                // If there are any significant digits, ...
                if (nDigits > 0)
                {
                    // Append a decimal point
                    *lpaplChar++ = aplCharDecimal;

                    // While there are more significant digits, ...
                    while (nDigits > 0)
                    {
                        // Append another zero
                        *lpaplChar++ = L'0';
                        nDigits--;
                    } // End WHILE
                } // End IF

                break;

            case FLTDISPFMT_RAWINT:
            case FLTDISPFMT_RAWFLT:
                // The result is "0"
                *lpaplChar++ = L'0';

                break;

            defstop
                break;
        } // End SWITCH
    } else
    // Non-zero
    {
        DTOAMODE dtoaMode;              // DTOA mode corresponding to fltDispFmt
        LPCHAR   s, s0;                 // Ptr to output from dtoa
        int      decpt,                 // Exponent from dtoa
                 sign;                  // TRUE iff the number is negative
        APLINT   iSigDig;               // # significant digits
        UBOOL    bPowerOfTwo;           // TRUE iff the # is a power of two
        LPWCHAR  lpaplCharIni;          // Initial ptr to output save area

        // Get the corresponding DTOA mode
        // DTOAMODE_SHORT_RND = 0,              // 0 = shortest string with rounding, e.g., 1e23
        // DTOAMODE_SHORT_NORND,                // 1 = shortest string without rounding. e.g., 9.999999999999999e22
        // DTOAMODE_SIGDIGS,                    // 2 = # significant digits
        // DTOAMODE_FRACTDIGS,                  // 3 = # fractional digits (past decimal point)

        // Save the initial ptr to the output save area
        lpaplCharIni = lpaplChar;

        // If this is raw float formatting, ...
        if (nDigits EQ 0 && fltDispFmt EQ FLTDISPFMT_RAWFLT)
            // Use the current value of []PP
            nDigits  = GetQuadPP ();
        // Get the corresponding DTOAMODE_xxx
        dtoaMode = gDTOA_Mode[fltDispFmt];

        // If this is RAWINT, ...
        if (fltDispFmt EQ FLTDISPFMT_RAWINT)
        {
            APLFLOATUNION aplFloatUnion;        // Temporary union

            // Save the FP value in the union so we can pick it apart
            aplFloatUnion.aplFloat = aplFloat;

            // Determine if the value is a power of two
            bPowerOfTwo = (aplFloatUnion.aplFloatStr.uMantissa EQ 0);

            nDigits = 0;                    // No digits (integral)
            dtoaMode = DTOAMODE_FRACTDIGS;  // 3 = nDigits past decimal point

            // Round aplFloat to the nearest integer
            //   taking into account the sign (which floor doesn't)
            if (aplFloat >= 0)
                aplFloat =  floor (0.5 + aplFloat);
            else
                aplFloat = -floor (0.5 - aplFloat);
            // Handle signed zero
            if (aplFloat EQ 0       // If it's zero, and
             && !gAllowNeg0)        //   we don't allow -0, ...
                // Make it an unsigned zero
                aplFloat = 0;
        } // End IF

        // Convert aplFloat to an ASCII string
        // Use David Gay's routines
        s = s0 = dtoa (aplFloat, dtoaMode, (UINT) nDigits, &decpt, &sign, NULL);

        // If we're displaying short round, ensure that we display no more digits
        //   than nDigits.
        if (fltDispFmt EQ FLTDISPFMT_RAWFLT
         && dtoaMode EQ DTOAMODE_SHORT_RND
         && nDigits NE 0
         && nDigits < lstrlen (s))
        {
            // Free the temporary storage
            freedtoa (s0);

            // Try again with SIGDIGS
            dtoaMode = DTOAMODE_SIGDIGS;        // Display up to []PP digits
            s = s0 = dtoa (aplFloat, dtoaMode, (UINT) nDigits, &decpt, &sign, NULL);
        } // End IF

        // Handle the sign
        if (sign NE 0)
            *lpaplChar++ = aplCharOverbar;

        // Split cases based upon the float display format
        switch (fltDispFmt)
        {
            case FLTDISPFMT_E:
                // Format the number with exactly nDigits significant digits
                lpaplChar =
                  FormatExpFmt (lpaplChar,          // Ptr to output save area
                         (int) -nDigits,            // # significant digits
                                s,                  // Ptr to raw formatted number
                                decpt,              // Exponent
                                DEF_MAX_QUADPP_IEEE,// Maximum # significant digits
                                aplCharDecimal,     // Char to use as decimal separator
                                aplCharOverbar);    // Char to use as overbar
                break;

            case FLTDISPFMT_F:
                // Format the number with no more than nDigits
                //   to the right of the decimal point

                // Get the maximum # significant digits
                iSigDig = DEF_MAX_QUADPP_IEEE;

                // Handle numbers between 0 and 1
                if (decpt <= 0)
                {
                    // Start with "0."
                    *lpaplChar++ = L'0';
                    *lpaplChar++ = aplCharDecimal;

                    // Then a bunch of 0s
                    while (nDigits > 0
                        && decpt < 0)
                    {
                        *lpaplChar++ = L'0';
                        nDigits--;
                        decpt++;
                    } // End WHILE

                    // If there are more significant digits, ...
                    if (nDigits NE 0)
                    {
                        // Copy the remaining digits (or underflow chars) to the result
                        //   converting from one-byte ASCII to two-byte UTF16
                        while (nDigits > 0
                            && *s)
                        {
                            *lpaplChar++ = (iSigDig > 0) ? *s++ : DEF_UNDERFLOW;
                            nDigits--;
                            iSigDig--;
                        } // End WHILE

                        // End with zeros or underflow chars for nDigits
                        while (nDigits > 0)
                        {
                            *lpaplChar++ = (iSigDig > 0) ? L'0' : DEF_UNDERFLOW;
                            nDigits--;
                            iSigDig--;
                        } // End WHILE
                    } // End IF
                } else
                {
                    // Copy no more than decpt digits to the result
                    //   converting from one-byte ASCII to two-byte UTF16
                    while (min (decpt, iSigDig) > 0
                        && *s)
                    {
                        *lpaplChar++ = *s++;
                        decpt--;
                        iSigDig--;
                    } // End WHILE

                    // If there are still more digits in the exponent, ...
                    if (decpt > 0)
                    {
                        // Fill with trailing zeros or underflow chars
                        while (decpt > 0)
                        {
                            *lpaplChar++ = (iSigDig > 0) ? L'0' : DEF_UNDERFLOW;
                            decpt--;
                            iSigDig--;
                        } // End WHILE

                        // Next, the decimal point
                        *lpaplChar++ = aplCharDecimal;

                        // End with zeros or underflow chars for nDigits
                        while (nDigits > 0)
                        {
                            *lpaplChar++ = (iSigDig > 0) ? L'0' : DEF_UNDERFLOW;
                            nDigits--;
                            iSigDig--;
                        } // End WHILE
                    } else
                    {
                        // Next, the decimal point
                        *lpaplChar++ = aplCharDecimal;

                        // Fill with trailing digits or underflow chars
                        while (nDigits > 0
                            && *s)
                        {
                            *lpaplChar++ = (iSigDig > 0) ? *s++ : DEF_UNDERFLOW;
                            nDigits--;
                            iSigDig--;
                        } // End WHILE

                        // End with zeros or underflow chars for nDigits
                        while (nDigits > 0)
                        {
                            *lpaplChar++ = (iSigDig > 0) ? L'0' : DEF_UNDERFLOW;
                            nDigits--;
                            iSigDig--;
                        } // End WHILE
                    } // End IF/ELSE
                } // End IF/ELSE

                break;

            case FLTDISPFMT_RAWINT:
                // If the number is a power of two, ...
                if (bPowerOfTwo)
                    // Copy the remaining digits to the result
                    //   converting from one-byte ASCII to two-byte UTF16
                    while (*s NE AC_EOS)
                        *lpaplChar++ = *s++;
                else
                {
                    APLINT nSigDig;             // # significant digits

                    // Copy no more than nDigits

                    // No more than DEF_MAX_QUADPP_IEEE digits
                    if (nDigits)
                        nSigDig = min (nDigits, DEF_MAX_QUADPP_IEEE);
                    else
                    {
                        nSigDig = DEF_MAX_QUADPP_IEEE;
                        nDigits = decpt;
                    } // End IF/ELSE

                    // Loop through the digits
                    while (nSigDig > 0
                        && decpt > 0
                        && *s)
                    {
                        // Copy the remaining digits to the result
                        //   converting from one-byte ASCII to two-byte UTF16
                        *lpaplChar++ = *s++;
                        nSigDig--;
                        nDigits--;
                        decpt--;
                    } // End WHILE

                    // If there are more digits in the exponent, ...
                    while (nSigDig > 0
                        && decpt > 0)
                    {
                        // Fill with trailing zeros
                        *lpaplChar++ = L'0';
                        nSigDig--;
                        nDigits--;
                        decpt--;
                    } // End WHILE

                    // If there are more digits in the result, ...
                    while (nDigits > 0)
                    {
                        // Fill with trailing underflow chars
                        *lpaplChar++ = DEF_UNDERFLOW;
                        nDigits--;
                    } // End WHILE
                } // End IF/ELSE

                break;

            case FLTDISPFMT_RAWFLT:
                // Format the number wth no more than nDigits significant digits

                // If we're formatting as default, ...
                if (nDigits EQ 0)
                    // Get the default precision
                    nDigits = GetQuadPP ();

                // If (decpt > nDigits) or (decpt < -5), switch to E-format
                if (decpt > nDigits
                 || decpt < -5)
                    // Format the number with no more than nDigits significant digits
                    lpaplChar =
                      FormatExpFmt (lpaplChar,          // Ptr to output save area
                              (int) nDigits,            // # significant digits
                                    s,                  // Ptr to raw formatted number
                                    decpt,              // Exponent
                                    DEF_MAX_QUADPP_IEEE,// Maximum # significant digits
                                    aplCharDecimal,     // Char to use as decimal separator
                                    aplCharOverbar);    // Char to use as overbar
                else
                // Handle numbers between 0 and 1
                if (decpt <= 0)
                {
                    // Start with "0."
                    *lpaplChar++ = L'0';
                    *lpaplChar++ = aplCharDecimal;

                    // Then a bunch of 0s
                    while (decpt < 0)
                    {
                        *lpaplChar++ = L'0';
                        decpt++;
                    } // End WHILE

                    // Copy the remaining digits to the result
                    //   converting from one-byte ASCII to two-byte UTF16
                    while (*s NE AC_EOS)
                        *lpaplChar++ = *s++;
                } else
                {
                    // Copy no more than nDigits digits to the result
                    //   converting from one-byte ASCII to two-byte UTF16
                    while (nDigits > 0
                        && decpt > 0
                        && *s)
                    {
                        *lpaplChar++ = *s++;
                        nDigits--;
                        decpt--;
                    } // End WHILE

                    // If there are still more digits in the exponent, ...
                    if (decpt > 0)
                        // Fill with trailing zeros
                        while (nDigits > 0
                            && decpt > 0)
                        {
                            *lpaplChar++ = L'0';
                            nDigits--;
                            decpt--;
                        } // End WHILE
                    else
                    {
                        // If there are more digits in the result, ...
                        if (nDigits > 0 && *s)
                        {
                            // Next, the decimal point
                            *lpaplChar++ = aplCharDecimal;

                            // Fill with trailing digits
                            while (nDigits > 0 && *s NE AC_EOS)
                            {
                                *lpaplChar++ = *s++;
                                nDigits--;
                            } // End WHILE
                        } // End IF
                    } // End IF/ELSE
                } // End IF/ELSE

                break;

            defstop
                break;
        } // End SWITCH

        // Free the temporary storage
        freedtoa (s0);
    } // End IF/ELSE/...

    // Append a separator
    *lpaplChar++ = L' ';

    return lpaplChar;
} // End FormatAplFltFC


//***************************************************************************
//  $FormatExpFmt
//
//  Format a number in exponential format
//***************************************************************************

LPAPLCHAR FormatExpFmt
    (LPAPLCHAR lpaplChar,                   // Ptr to output save area
     int       nDigits,                     // # significant digits
     LPCHAR    s,                           // Ptr to raw formatted number
     int       decpt,                       // Exponent
     APLINT    iSigDig,                     // # significant digits
     APLCHAR   aplCharDecimal,              // Char to use as decimal separator
     APLCHAR   aplCharOverbar)              // Char to use as overbar

{
    UBOOL     bExact;                       // TRUE iff to be formatted with exactly
                                            // nDigits significant digits

    // Check for exactly nDigits significant digits
    bExact = (nDigits < 0);

    // If so, make positive
    if (bExact)
        nDigits = -nDigits;

    // If the number is negative, ...
    if (s[0] EQ '-')
    {
        // Copy it to the result
        *lpaplChar++ = aplCharOverbar;

        // Skip over it
        s++;
    } // End IF

    // If there's only one significant digit, ...
    if (nDigits EQ 1)
    {
        WCHAR wch;

        // Copy the first digit
        wch = *s;

        // If there's a next digit and we should round up, ...
        if (s[1] && s[1] >= L'5')
        {
            // If the first digit is not at the top, ...
            if (wch NE L'9')
                // Round up
                wch++;
            else
                // Otherwise, wrap to '1'
                wch = L'1';
        } // End IF

        // Save the one and only digit
        *lpaplChar++ = wch;
    } else
    {
        // Copy the first digit
        *lpaplChar++ = *s++;
        nDigits--;
        iSigDig--;

        // If there are more digits or we're being exact
        if (*s NE AC_EOS || bExact)
            // Then the decimal separator
            *lpaplChar++ = aplCharDecimal;

        // If there are more digits, ...
        if (*s NE AC_EOS)
        // Then the next nDigits digits
        while (nDigits > 0
            && *s)
        {
            *lpaplChar++ = (iSigDig > 0) ? *s++ : DEF_UNDERFLOW;
            nDigits--;
            iSigDig--;
        } // End IF/WHILE

        // If we're being exact, ...
        if (bExact)
        // While there are more digits, ...
        while (nDigits > 0)
        {
            // Fill with trailing zeros or underflow chars
            *lpaplChar++ = (iSigDig > 0) ? L'0' : DEF_UNDERFLOW;
            nDigits--;
            iSigDig--;
        } // End WHILE
    } // End IF/ELSE

    // Then the exponent separator
    *lpaplChar++ = DEF_EXPONENT_UC;

    // Finally the exponent
    lpaplChar =
      FormatAplIntFC (lpaplChar,        // Ptr to output save area
                      decpt - 1,        // The value to format
                      aplCharOverbar);  // Char to use as overbar
    // Ensure properly terminated
    *--lpaplChar = WC_EOS;

    return lpaplChar;
} // End FormatExpFmt


//***************************************************************************
//  $FormatAplVfp
//
//  Format a variable-precision floating point number
//***************************************************************************

LPAPLCHAR FormatAplVfp
    (LPWCHAR  lpaplChar,        // Ptr to output save area
     LPVOID   lpaplVfp,         // Ptr to the value to format
     APLUINT  nDigits)          // # significant digits (0 = all)

{
    return
      FormatAplVfpFC (lpaplChar,                // Ptr to output save area
                      lpaplVfp,                 // Ptr to the value to format
                      nDigits,                  // # significant digits (0 = all)
                      0,                        // Maximum width including sign & decpt (0 = none)
                      DecSep,                   // Char to use as decimal separator
                      NegSign,                  // Char to use as overbar
                      FLTDISPFMT_RAWFLT,        // Float display format
                      FALSE,                    // TRUE iff nDigits is # fractional digits
                      FALSE,                    // TRUE iff we're to substitute text for infinity
                      FALSE);                   // TRUE iff we're to precede the display with (FPCnnn)
} // End FormatAplVfp


//***************************************************************************
//  $FormatAplVfpFC
//
//  Format a variable FP using []FC
//***************************************************************************

LPAPLCHAR FormatAplVfpFC
    (LPWCHAR    lpaplChar,          // Ptr to output save area
     LPAPLVFP   lpaplVfp,           // Ptr to the value to format
     APLINT     nDigits,            // # significant digits (0 = all), or
                                    // # fractional digits (if bFractDigs)
                                    // If negative, use E-format
     APLINT     nWid,               // Maximum width including sign & decpt (0 = none)
     APLCHAR    aplCharDecimal,     // Char to use as decimal separator
     APLCHAR    aplCharOverbar,     // Char to use as overbar
     FLTDISPFMT fltDispFmt,         // Float display format (see FLTDISPFMT)
     UBOOL      bFractDigs,         // TRUE iff nDigits is # fractional digits
     UBOOL      bSubstInf,          // TRUE iff we're to substitute text for infinity
     UBOOL      bPrecFPC)           // TRUE iff we're to precede the display with (FPCnnn)

{
    mp_exp_t  expptr;               // Ptr to exponent shift
    APLINT    iLen,                 // String length
              iDiff,                // ...           difference between maximum digits and requested # digits
              iRes;                 // Loop counter
    UBOOL     bNeg;                 // TRUE iff the number is negative
    APLINT    iDecPt = 0,           // Position of the decimal point (0 = none)
              iDelDigits,           // # trailing digits to delete if bFractDigs
              iUnderflow;           // # trailing underflow digits
    int       iSigDigs,             // # significant digits for <mpfr_get_str>
              iIntDigs,             // # integer digits (may be > iPrcDigs)
              iFrcDigs,             // # fractional digits (may be > iPrcDigs) (excluding 'Enn')
              iPrcDigs;             // # significant digits in the precision
    LPCHAR    lpRawFmt;             // Ptr to raw formatted #
    int       nDigFPC;              // # significant digits based on the precision

    // If we're to display all significant digits, ...
    if (nDigits EQ 0)
        // Use the default
        nDigits = GetQuadPPV ();

    // Convert this number's precision to the # digits it represents
    //   via the formula  1 + floor (log10 (2^P))
    //                  = 1 + floor (P x log10 (2))
    //   where log10 (2) = (ln (2)) / (ln (10))
    //                   = M_LN2 / M_LN10
    nDigFPC = 1 + (int) floor (mpfr_get_prec (lpaplVfp) * M_LN2 / M_LN10);

    // Display no more than this # digits
    if (nDigits < 0)
        nDigits = max (nDigits, -nDigFPC);
    else
        nDigits = min (nDigits,  nDigFPC);

    Assert (!IsMpfNULL (lpaplVfp));

    // If we're to precede the display with (FPCnnn), ...
    if (bPrecFPC)
    {
        // Append leading marker
        *lpaplChar++ = L'(';

        lpaplChar =
          FormatAplIntFC (lpaplChar,                // Ptr to output save area
                          mpfr_get_prec (lpaplVfp), // The value to format
                          NegSign);                 // Char to use as overbar
        // Append trailing marker overwriting the trailing space
        lpaplChar[-1] = L')';
    } // End IF

    // Izit a NaN?
    if (IsMpfNaN (lpaplVfp))
    {
        // If we're to substitute, ...
        if (bSubstInf)
            lpaplChar +=
              ConvertWideToNameLength (lpaplChar,       // Ptr to output save buffer
                                      &wcNaN,           // Ptr to incoming chars
                                       1);              // # chars to convert
        else
            *lpaplChar++ = wcNaN;
    } else
    // Izit an infinity?
    if (IsMpfInfinity (lpaplVfp))
    {
        // Izit negative?
        if (mpfr_sgn (lpaplVfp) < 0)
            *lpaplChar++ = aplCharOverbar;

        // If we're to substitute, ...
        if (bSubstInf)
            lpaplChar +=
              ConvertWideToNameLength (lpaplChar,       // Ptr to output save buffer
                                      &wcInf,           // Ptr to incoming chars
                                       1);              // # chars to convert
        else
            *lpaplChar++ = wcInf;           // Char for infinity
    } else
    // Check for 0.0
    if (IsMpf0 (lpaplVfp))
    {
        // If it's negative, ...
        if (SIGN_APLVFP (lpaplVfp))
            *lpaplChar++ = aplCharOverbar;

        // Split cases based upon the float display format
        switch (fltDispFmt)
        {
            case FLTDISPFMT_E:          // 0E0 or 0.0E0 or 0.0---0E0
                if (nDigits EQ 1)
                {
                    // Fill in the result of "0E0"
                    *lpaplChar++ = L'0';
                    *lpaplChar++ = DEF_EXPONENT_UC;
                    *lpaplChar++ = L'0';
                } else
                if (nDigits > 1)
                {
                    // Start with "0."
                    *lpaplChar++ = L'0';
                    nDigits--;
                    *lpaplChar++ = aplCharDecimal;

                    // While there are more digits, ...
                    while (nDigits > 0)
                    {
                        // Fill with leading zeros
                        *lpaplChar++ = L'0';
                        nDigits--;
                    } // End WHILE

                    // End with "E0"
                    *lpaplChar++ = DEF_EXPONENT_UC;
                    *lpaplChar++ = L'0';
                } else
                    DbgStop ();         // We should never get here
                break;

            case FLTDISPFMT_F:
                // Start with a "0"
                *lpaplChar++ = L'0';

                // If there are any significant digits, ...
                if (nDigits > 0)
                {
                    // Append a decimal point
                    *lpaplChar++ = aplCharDecimal;

                    // While there are more significant digits, ...
                    while (nDigits > 0)
                    {
                        // Append another zero
                        *lpaplChar++ = L'0';
                        nDigits--;
                    } // End WHILE
                } // End IF

                break;

            case FLTDISPFMT_RAWINT:
            case FLTDISPFMT_RAWFLT:
                // The result is "0"
                *lpaplChar++ = L'0';

                break;

            defstop
                break;
        } // End SWITCH
    } else
    // Non-zero
    {
        lpRawFmt = (LPCHAR) lpaplChar;

        // Get # significant digits
        if (bFractDigs)
            iSigDigs = (int) GetQuadPPV ();
        else
            iSigDigs = abs ((UINT) nDigits);

        // Format the VFP at maximum # digits
        mpfr_get_str (lpRawFmt,             // Ptr to output save area
                     &expptr,               // Ptr to exponent save area
                      10,                   // Base of number system
                      0,                    // # significant digits (0 = all)
                      lpaplVfp,             // Ptr to VFP number
                      MPFR_RNDN);           // Rounding mode
        // Check for negative
        bNeg = (lpRawFmt[0] EQ '-');

        // If we're not already displaying in E-format, and
        //   there's no maximum width, ...
        if (nDigits > 0
         && nWid EQ 0
         && !bFractDigs)
        {
            // If the number is too large or too small, ...
            if (expptr > nDigits
             || expptr < -5)
                // Switch to E-format
                nDigits = -nDigits;
        } // End IF

        // Get the char length
        iDiff = lstrlen (lpRawFmt);

        // Get the # digits in the precision
        iPrcDigs = (int) iDiff - bNeg;

        // nDigits < 0 ==> !bFractDigs
        // nDigits >= 0 &&  bFractDigs ==> 'Fxx.yy' []FMT ... or nWid nDigits{format}...
        // nDigits >= 0 && !bFractDigs ==> 'Ixx'    []FMT ... or FormatAplVfp () or dyadic DownTackJot

        // If abs (number) >= 1
        if (expptr > 0)
        {
            iIntDigs = expptr;

            // If we're not displaying in E-format, ...
            if (nDigits >= 0)
            {
                // If nDigits is # fractional digits, ...
                if (bFractDigs)
                {
                    iFrcDigs = (int) nDigits;
                    iSigDigs = iIntDigs + iFrcDigs;
                } else
                {
                    iFrcDigs = max (iPrcDigs - iIntDigs, 0);
                    iSigDigs = (int) nDigits;
                } // End IF/ELSE
            } // End IF/ELSE
        } else
        // abs (number) < 1
        // 0.0000nnnnnnnnnnnnnn
        {
            // The integer digit is 0
            iIntDigs = 1;

            // If we're not displaying in E-format, ...
            if (nDigits >= 0)
            {
                // If nDigits is # fractional digits, ...
                if (bFractDigs)
                {
                    iFrcDigs = (int) nDigits;
                    iSigDigs = iIntDigs + iFrcDigs;
                } else
                {
                    iFrcDigs = (int) nDigits - expptr;
                    iSigDigs = (int) nDigits;
                } // End IF/ELSE
            } // End IF/ELSE
        } // End IF/ELSE

        // If we're not displaying in E-format, ...
        if (nDigits >= 0)
        {
////        // If abs (number) >= 1,
////        //   and nDigits is # fractional digits, ...
////        if (expptr > 0
////         && bFractDigs)
////            // Add in the # integer digits
////            iSigDigs += expptr;
////////////else
////////////    // iSigDigs set above

            // Ask for no more digits than are in the precision
            iSigDigs = min (iSigDigs, iPrcDigs);
////////} else
////////{
////////    Assert (!bFractDigs);
        } // End IF/ELSE

        // If # significant digits NE # precision digits, ...
        if (iSigDigs NE iPrcDigs)
            // Format the VFP
            mpfr_get_str (lpRawFmt,                     // Ptr to output save area
                         &expptr,                       // Ptr to exponent save area
                          10,                           // Base of number system
                          iSigDigs,                     // # significant digits (0 = all)
                          lpaplVfp,                     // Ptr to VFP number
                          MPFR_RNDN);                   // Rounding mode
        // Get the char length
        iLen = lstrlen (lpRawFmt);

        if (fltDispFmt NE FLTDISPFMT_F)
        // Delete trailing zeros as <mpf_get_str> does
        for (iRes = iLen - 1; iRes >= (bNeg + max (expptr, 0)); iRes--)
        if (lpRawFmt[iRes] EQ '0')
        {
            lpRawFmt[iRes] = AC_EOS;
            iLen--;
        } else
            break;

        // If we're not displaying in E-format, ...
        if (nDigits >= 0)
            // Get the difference in char lengths
            iDiff -= iLen;

        // If displaying fractional digits, ...
        if (bFractDigs)
        {
            // Calculate the # trailing underflow digits
            iUnderflow = abs64 (nDigits) + max (expptr, 0) - max (nDigFPC, (APLUINT) iLen);
            iUnderflow = max (iUnderflow, 0);
        } else
            iUnderflow = 0;

        // If we're formatting in E-format, ...
        if (nDigits < 0)
        {
            // Copy the raw # to a point to the right of itself so we can format it in place
            CopyMemory (&lpRawFmt[6 + 2 * (iLen + 1)], lpRawFmt, (APLU3264) (iLen + 1));
            lpRawFmt += 6 + 2 * (iLen + 1);

            // Format the number with exactly nDigits significant digits
            lpaplChar =
              FormatExpFmt (lpaplChar,              // Ptr to output save area
                     (int) -nDigits,                // # significant digits
                            lpRawFmt,               // Ptr to raw formatted number
                            expptr,                 // Exponent
                            DEF_MAX_QUADPP_VFP,     // Maximum # significant digits
                            aplCharDecimal,         // Char to use as decimal separator
                            aplCharOverbar);        // Char to use as overbar
        } else
        {
            // Convert the char to WCHAR
            for (iRes = iLen - 1; iRes >= 0; iRes--)
                lpaplChar[iRes] = lpRawFmt[iRes];

            // Check for Infinity
            if (lpaplChar[bNeg] EQ DEF_POSINFINITY_CHAR)
            {
                if (bSubstInf)
////////////////////lpaplChar += bNeg +     // No need to skip over
                      ConvertWideToNameLength (&lpaplChar[bNeg],    // Ptr to output save buffer
                                               &wcInf,              // Ptr to incoming chars
                                                1);                 // # chars to convert
                else
                    lpaplChar[bNeg] = wcInf;
            } else
            // If the number is in (-1, 1), ...
            if (expptr <= 0)
            {
                // Get the absolute value of the exponent
                expptr = -expptr;

                // If displaying fractional digits, ...
                if (bFractDigs)
                    iDelDigits = expptr;
                else
                    iDelDigits = 0;

                // Make room for "0." and leading zeros
                // If bFractDigs, nDigits is the # fractional digits to display
                //   so we must delete trailing expptr chars as
                //   the format string above produces nDigits
                //   significant digits
                MoveMemoryW (&lpaplChar[bNeg + 2 + expptr], &lpaplChar[bNeg], (APLU3264) (iLen - bNeg));

                // Fill in the leading "0."
                lpaplChar[bNeg + 0] = L'0';
                lpaplChar[iDecPt = bNeg + 1] = aplCharDecimal;

                // Fill in the leading zeros
                FillMemoryW (&lpaplChar[bNeg + 2], expptr, L'0');

                // If displaying fractional digits, ...
                if (bFractDigs)
                {
                    if (nDigits > ((iLen - bNeg) - expptr))
                        // Fill in the trailing zeros
                        FillMemoryW (&lpaplChar[iLen + 2], (APLU3264) (nDigits - ((iLen - bNeg) - expptr)), L'0');

                    //       Neg  "0."  Digits
                    iLen  = bNeg + 2 + nDigits;
                } else
                    //            "0." Leading
                    iLen +=        2 + expptr;
            } else
            // If the # significant digits is smaller than the exponent, ...
            if (iLen < (bNeg + expptr))
            {
                // Fill in trailing underbars up to but not including the decimal point
                FillMemoryW (&lpaplChar[iLen], (APLU3264) (bNeg + expptr - iLen), DEF_UNDERFLOW);
                iLen = bNeg + expptr;
            } else
            // If the # significant digits is larger than the exponent, ...
            if (iLen > (bNeg + expptr))
            {
                // Make room for the decimal point
                MoveMemoryW (&lpaplChar[bNeg + 1 + expptr], &lpaplChar[bNeg + expptr], (APLU3264) (iLen - (bNeg + expptr)));

                // Insert the decimal point
                lpaplChar[iDecPt = bNeg + expptr] = aplCharDecimal;
                iLen++;
            } // End IF/ELSE

            // If displaying fractional digits, ...
            if (bFractDigs)
            {
                if (iDecPt EQ 0)
                    lpaplChar[iDecPt = iLen++] = aplCharDecimal;

                // If the # digits after the decimal point is less than nDigits, ...
                if (nDigits > (iLen - iDecPt))
                {
                    // Fill with trailing zeros
                    FillMemoryW (&lpaplChar[iLen], (APLU3264) (nDigits - (iLen - (iDecPt + 1))), L'0');

                    //           Decpt    Digits
                    iLen =  1 + iDecPt + nDigits;
                } // End IF

                // Fill in the trailing underflow digits, possibly overwriting the decimal point
                if (iUnderflow NE 0)
                {
                    FillMemoryW (&lpaplChar[iLen - iUnderflow], (APLU3264) iUnderflow, DEF_UNDERFLOW);

                    // Restore the decimal point
                    lpaplChar[iDecPt] = aplCharDecimal;
                } // End IF
            } // End IF

            // If the number is negative, ...
            if (bNeg)
                lpaplChar[0] = aplCharOverbar;

            // Skip over the formatted number
            lpaplChar += iLen;
        } // End IF/ELSE/...
    } // End IF/ELSE

    if (OptionFlags.bDispMPSuf)
        // Include the suffix
        *lpaplChar++ = L'v';

    // Include a separator
    *lpaplChar++ = L' ';

    return lpaplChar;
} // End FormatAplVfpFC


//***************************************************************************
//  $FormatAplHC2I
//
//  Format a HC2I number
//***************************************************************************

LPAPLCHAR FormatAplHC2I
    (LPWCHAR  lpaplChar,        // Ptr to output save area
     LPVOID   lpaplHC2I)        // Ptr to the value to format

{
    return
      FormatAplHC2IFC (lpaplChar,       // Ptr to output save area
                       lpaplHC2I,       // Ptr to the value to format
                       NegSign,         // Char to use as overbar
                       GetHC2Sep ());   // Ptr to char(s) to use as separator
} // End FormatAplHC2I


//***************************************************************************
//  $FormatAplHC2IFC_Polar
//
//  Format a HC2I using []FC in Polar format
//***************************************************************************

LPAPLCHAR FormatAplHC2IFC_Polar
    (LPWCHAR    lpaplChar,          // Ptr to output save area
     LPAPLHC2I  lpaplHC2I,          // Ptr to the value to format
     APLCHAR    aplCharOverbar,     // Char to use as overbar
     LPAPLCHAR  lpaplCharSep)       // Ptr to char(s) to use as separator

{
    ALLTYPES atRes = {0};
    UBOOL    bRet;

    // Convert the HC2I to HC2F
    (*aTypeActConvert[ARRAY_HC2I][ARRAY_HC2F]) (lpaplHC2I, 0, &atRes, &bRet);

    // Check for error
    if (!bRet)
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    return
      FormatAplHC2FFC_Polar (lpaplChar,         // Ptr to output save area
                            &atRes.aplHC2F,     // Ptr to the value to format
                             GetQuadPP (),      // Raw or E-format:  # significant digits
                                                // F-format:  # digits to right of decimal sep
                             DecSep,            // Char to use as decimal separator
                             aplCharOverbar,    // Char to use as overbar
                             GetHC2Sep (),      // Ptr to char(s) to use as separator
                             FLTDISPFMT_RAWFLT, // Float display format for Magnitude
                             FALSE);            // TRUE iff we're to substitute text for infinity
} // End FormatAplHC2IFC_Polar


//***************************************************************************
//  $FormatAplHC2IFC
//
//  Format a HC2I using []FC
//***************************************************************************

LPAPLCHAR FormatAplHC2IFC
    (LPWCHAR    lpaplChar,          // Ptr to output save area
     LPAPLHC2I  lpaplHC2I,          // Ptr to the value to format
     APLCHAR    aplCharOverbar,     // Char to use as overbar
     LPAPLCHAR  lpaplCharSep)       // Ptr to char(s) to use as separator

{
    // Split cases based upon the Complex Number Display Separator
    switch (CndSep)
    {
        case L'i':
        case L'J':
            return
              FormatAplHC2IFC_Cart  (lpaplChar,         // Ptr to output save area
                                     lpaplHC2I,         // Ptr to the value to format
                                     aplCharOverbar,    // Char to use as overbar
                                     lpaplCharSep);     // Ptr to char(s) to use as separator
        case L'd':
        case L'h':
        case L'r':
        case L'u':
            return
              FormatAplHC2IFC_Polar (lpaplChar,         // Ptr to output save area
                                     lpaplHC2I,         // Ptr to the value to format
                                     aplCharOverbar,    // Char to use as overbar
                                     lpaplCharSep);     // Ptr to char(s) to use as separator
        defstop
            return NULL;
    } // End SWITCH
} // End FormatAplHC2IFC


//***************************************************************************
//  $FormatAplHC2IFC_Cart
//
//  Format a HC2I using []FC in Cartesian coordinates
//***************************************************************************

LPAPLCHAR FormatAplHC2IFC_Cart
    (LPWCHAR    lpaplChar,          // Ptr to output save area
     LPAPLHC2I  lpaplHC2I,          // Ptr to the value to format
     APLCHAR    aplCharOverbar,     // Char to use as overbar
     LPAPLCHAR  lpaplCharSep)       // Ptr to char(s) to use as separator

{
    int i,                          // Loop counter
        iHCDimVar = 2;              // HC Dimension (1, 2, 4, 8)

    // If we are displaying HC in infix form, ...
    if (OptionFlags.bDispInfix)
        // Format the real part
        lpaplChar =
          FormatAplIntFC (lpaplChar,            // Ptr to output save area
                          lpaplHC2I->parts[0],  // The value to format
                          aplCharOverbar);      // Char to use as overbar
    else
    // Otherwise, we're displaying in postfix form
    {
        // If the real part is non-zero, or the entire number is zero, ...
        if (lpaplHC2I->parts[0] NE 0
         || IsZeroHCxI (lpaplHC2I, iHCDimVar))
        {
            // Format the real part
            lpaplChar =
              FormatAplIntFC (lpaplChar,            // Ptr to output save area
                              lpaplHC2I->parts[0],  // The value to format
                              aplCharOverbar);      // Char to use as overbar
            // If the imaginary part of the number is NOT all zero, ...
            if (OptionFlags.bDisp0Imag
             || IzitImaginary (uHCxI[aHCDimToIndex[iHCDimVar]], lpaplHC2I))
                // Append the real part separator
                //  overwriting the trailing blank
                lpaplChar[-1] = L's';
            else
                // Back up to the trailing blank so the next
                //   coefficient overwrites it
                lpaplChar--;
        } // End IF
    } // End IF/ELSE

    // Loop through the imaginary parts
    for (i = 1; i < iHCDimVar; i++)
    // If the imaginary part is non-zero, ...
    if (OptionFlags.bDisp0Imag
     || lpaplHC2I->parts[i] NE 0)
    {
        // If we are displaying HC in infix form, ...
        if (OptionFlags.bDispInfix)
        {
            // Append the Complex separator
            //  overwriting the trailing blank
            lstrcpyW (&lpaplChar[-1], lpaplCharSep);

            // Skip over the separator
            lpaplChar += lstrlenW (lpaplChar);
        } // End IF

        // Format the imag part
        lpaplChar =
          FormatAplIntFC (lpaplChar,            // Ptr to output save area
                          lpaplHC2I->parts[i],  // The value to format
                          aplCharOverbar);      // Char to use as overbar
        // If we are NOT displaying HC in infix form, ...
        if (!OptionFlags.bDispInfix)
        {
            // Append the Complex separator
            //  overwriting the trailing blank
            lstrcpyW (&lpaplChar[-1], lpaplCharSep);

            // Skip over the separator
            lpaplChar += lstrlenW (lpaplChar);
        } // End IF
    } // End FOR/IF

    // If we are NOT displaying HC in infix form, ...
    if (!OptionFlags.bDispInfix)
        // Append a trailing blank
        *lpaplChar++ = L' ';

    return lpaplChar;
} // End FormatAplHC2I_Cart


//***************************************************************************
//  $FormatAplHC4I
//
//  Format a HC4I number
//***************************************************************************

LPAPLCHAR FormatAplHC4I
    (LPWCHAR  lpaplChar,        // Ptr to output save area
     LPVOID   lpaplHC4I)        // Ptr to the value to format

{
    return
      FormatAplHCxIFC (lpaplChar,       // Ptr to output save area
                       lpaplHC4I,       // Ptr to the value to format
                       NegSign,         // Char to use as overbar
                       4);              // HC Dimension (1, 2, 4, 8)
} // End FormatAplHC4I


//***************************************************************************
//  $FormatAplHC8I
//
//  Format a HC8I number
//***************************************************************************

LPAPLCHAR FormatAplHC8I
    (LPWCHAR  lpaplChar,        // Ptr to output save area
     LPVOID   lpaplHC8I)        // Ptr to the value to format

{
    return
      FormatAplHCxIFC (lpaplChar,       // Ptr to output save area
                       lpaplHC8I,       // Ptr to the value to format
                       NegSign,         // Char to use as overbar
                       8);              // HC Dimension (1, 2, 4, 8)
} // End FormatAplHC8I


//***************************************************************************
//  $FormatAplHCxIFC
//
//  Format a HCxI using []FC
//***************************************************************************

LPAPLCHAR FormatAplHCxIFC
    (LPWCHAR    lpaplChar,          // Ptr to output save area
     LPAPLHC8I  lpaplHC8I,          // Ptr to the value to format
     APLCHAR    aplCharOverbar,     // Char to use as overbar
     int        iHCDimVar)          // HC Dimension (1, 2, 4, 8)

{
    int i;                          // Loop counter

    // If we are displaying HC in infix form, ...
    if (OptionFlags.bDispInfix)
        // Format the real part
        lpaplChar =
          FormatAplIntFC (lpaplChar,            // Ptr to output save area
                          lpaplHC8I->parts[0],  // The value to format
                          aplCharOverbar);      // Char to use as overbar
    else
    // Otherwise, we're displaying in postfix form
    {
        // If the real part is non-zero, or the entire number is zero, ...
        if (lpaplHC8I->parts[0] NE 0
         || IsZeroHCxI (lpaplHC8I, iHCDimVar))
        {
            // Format the real part
            lpaplChar =
              FormatAplIntFC (lpaplChar,            // Ptr to output save area
                              lpaplHC8I->parts[0],  // The value to format
                              aplCharOverbar);      // Char to use as overbar
            // If the imaginary part of the number is NOT all zero, ...
            if (OptionFlags.bDisp0Imag
             || IzitImaginary (uHCxI[aHCDimToIndex[iHCDimVar]], lpaplHC8I))
                // Append the real part separator
                //  overwriting the trailing blank
                lpaplChar[-1] = L's';
            else
                // Back up to the trailing blank so the next
                //   coefficient overwrites it
                lpaplChar--;
        } // End IF
    } // End IF/ELSE

    // Loop through the imaginary parts
    for (i = 1; i < iHCDimVar; i++)
    // If the next imaginary part is non-zero, ...
    if (OptionFlags.bDisp0Imag
     || lpaplHC8I->parts[i] NE 0)
    {
        // If we are displaying HC in infix form, ...
        if (OptionFlags.bDispInfix)
        {
            // Append the next HCx separator
            //  overwriting the trailing blank
            strcpyW (&lpaplChar[-1], HC8SEP[i - 1]);

            // Skip over the separator
            lpaplChar += lstrlenW (HC8SEP[i - 1]) - 1;
        } // End IF

        // Format the next imag part
        lpaplChar =
          FormatAplIntFC (lpaplChar,            // Ptr to output save area
                          lpaplHC8I->parts[i],  // The value to format
                          aplCharOverbar);      // Char to use as overbar
        // If we are NOT displaying HC in infix form, ...
        if (!OptionFlags.bDispInfix)
        {
            // Append the next HCx separator
            //  overwriting the trailing blank
            strcpyW (&lpaplChar[-1], HC8SEP[i - 1]);

            // Skip over the separator
            lpaplChar += lstrlenW (HC8SEP[i - 1]) - 1;
        } // End IF
    } // End FOR/IF

    // If we are NOT displaying HC in infix form, ...
    if (!OptionFlags.bDispInfix)
        // Append a trailing blank
        *lpaplChar++ = L' ';

    return lpaplChar;
} // End FormatAplHCxIFC


//***************************************************************************
//  $FormatAplHC2F
//
//  Format a HC2F number
//***************************************************************************

LPAPLCHAR FormatAplHC2F
    (LPWCHAR  lpaplChar,        // Ptr to output save area
     LPVOID   lpaplHC2F,        // Ptr to the value to format
     APLUINT  nDigits)          // Raw or E-format:  # significant digits
                                // F-format:  # digits to right of decimal sep

{
    return
      FormatAplHC2FFC (lpaplChar,           // Ptr to output save area
                       lpaplHC2F,           // Ptr to the value to format
                       nDigits,             // # significant digits (0 = default)
                       DecSep,              // Char to use as decimal separator
                       NegSign,             // Char to use as overbar
                       GetHC2Sep (),        // Ptr to char(s) to use as separator
                       FLTDISPFMT_RAWFLT,   // Float display format
                       FALSE);              // TRUE iff we're to substitute text for infinity
} // End FormatAplHC2F


//***************************************************************************
//  $FormatAplHC2FFC_Polar
//
//  Format a HC2F using []FC in Polar format
//***************************************************************************

LPAPLCHAR FormatAplHC2FFC_Polar
    (LPWCHAR    lpaplChar,          // Ptr to output save area
     LPAPLHC2F  lpaplHC2F,          // Ptr to the value to format
     APLINT     nDigits,            // Raw or E-format:  # significant digits
                                    // F-format:  # digits to right of decimal sep
     APLCHAR    aplCharDecimal,     // Char to use as decimal separator
     APLCHAR    aplCharOverbar,     // Char to use as overbar
     LPAPLCHAR  lpaplCharSep,       // Ptr to char(s) to use a separator
     FLTDISPFMT fltDispFmtMag,      // Float display format for Magnitude (see FLTDISPFMT)
     UBOOL      bSubstInf)          // TRUE iff we're to substitute text for infinity

{
    APLHC2F    aplRes;                          // The result
    APLUINT    uAnglePrec = DEF_ANGLEPREC_UNK;  // Angle fractional precision
    UBOOL      bAnglePrec = FALSE;              // TRUE iff we're displaying Polar form
    FLTDISPFMT fltDispFmtAng = fltDispFmtMag;   // Float display format for Angle
    APLCHAR      aplCNDSEP;                         // Value of CndSep

    // Get the current value of CNDSEP from []FC
    aplCNDSEP = CndSep;

    // Convert the HC2F to Angle in Signed-Radians [-1p1, 1p1]
    aplRes.partsHi = atan2 (lpaplHC2F->partsHi, lpaplHC2F->partsLo);

    // Ensure the Angle is in Radians [0, 2p1] unless we're using 'ah'
    //   in which case leave it in Signed-Radians [-1p1, 1p1]
    if (aplRes.partsHi < 0
     && aplCNDSEP NE L'h')
        // Add in 2p1
        aplRes.partsHi += 2 * FloatPi;

    // Convert the HC2F to Radius
    aplRes.partsLo = MagHC2F (*lpaplHC2F);

    // Split cases based upon the Complex Number Display Separator
    switch (aplCNDSEP)
    {
        case L'd':
            // Check for reduced angle fractional precision
            if (uAnglePrec EQ DEF_ANGLEPREC_UNK)
            {
                uAnglePrec    = DEF_ANGLEPREC_ad;   // # digits to right of decimal sep
                fltDispFmtAng = FLTDISPFMT_F;       // Float display format for Angle
                bAnglePrec    = TRUE;
            } // End IF

            // If entered at L'd',
            //      convert the Angle in Radians [0, 2p1] to Degree-Radians [0, 720p1]
            aplRes.partsHi *= 360;

            // Fall through to common code

        case L'h':
            // Check for reduced angle fractional precision
            if (uAnglePrec EQ DEF_ANGLEPREC_UNK)
            {
                uAnglePrec    = DEF_ANGLEPREC_ah;   // # digits to right of decimal sep
                fltDispFmtAng = FLTDISPFMT_F;       // Float display format for Angle
                bAnglePrec    = TRUE;
            } // End IF

            // Fall through to common code

        case L'u':
            // Check for reduced angle fractional precision
            if (uAnglePrec EQ DEF_ANGLEPREC_UNK)
            {
                uAnglePrec    = DEF_ANGLEPREC_au;   // # digits to right of decimal sep
                fltDispFmtAng = FLTDISPFMT_F;       // Float display format for Angle
                bAnglePrec    = TRUE;
            } // End IF

            // If entered at L'h',
            //      convert the Angle in Signed-Radians [-1p1, 1p1] to Signed Unit Normalized [-0.5, 0.5]
            // If entered at L'd',
            //      convert the Angle in Degree-Radians [0, 720p1] to Degrees [0, 360]
            // If entered at L'u',
            //      convert the Angle in Radians [0, 2p1] to the Unit Normalized [0, 1]
            aplRes.partsHi /= 2*FloatPi;

            // Fall through to common code

        case L'i':
        case L'J':
            // Check for reduced angle fractional precision
            if (uAnglePrec EQ DEF_ANGLEPREC_UNK)
            {
                uAnglePrec    = GetQuadPPV ();      // # significant digits
////////////////fltDispFmtMag = fltDispFmtMag;      // Float display format for Cartesian coordinates
////////////////bAnglePrec    = FALSE;              // Already FALSE
            } // End IF

            // Fall through to common code

        case L'r':
            // Check for reduced angle fractional precision
            if (uAnglePrec EQ DEF_ANGLEPREC_UNK)
            {
                uAnglePrec    = DEF_ANGLEPREC_ar;   // # digits to right of decimal sep
                fltDispFmtAng = FLTDISPFMT_F;       // Float display format for Angle
                bAnglePrec    = TRUE;
            } // End IF

            return
              FormatAplHC2FFC_Cart  (lpaplChar,         // Ptr to output save area
                                    &aplRes,            // Ptr to the value to format
                                     nDigits,           // Raw or E-format:  # significant digits
                                                        // F-format:  # digits to right of decimal sep
                                     uAnglePrec,        // Angle fractional precision
                                     aplCharDecimal,    // Char to use as decimal separator
                                     aplCharOverbar,    // Char to use as overbar
                                     lpaplCharSep,      // Ptr to char(s) to use a separator
                                     fltDispFmtMag,     // Float display format for Magnitude (see FLTDISPFMT)
                                     fltDispFmtAng,     // ...                      Angle     ...
                                     bAnglePrec,        // TRUE iff we're displaying Polar form
                                     bSubstInf);        // TRUE iff we're to substitute text for infinity
        defstop
            return NULL;
    } // End SWITCH
} // End FormatAplHC2FFC_Polar


//***************************************************************************
//  $FormatAplHC2FFC
//
//  Format a HC2F using []FC
//***************************************************************************

LPAPLCHAR FormatAplHC2FFC
    (LPWCHAR    lpaplChar,          // Ptr to output save area
     LPAPLHC2F  lpaplHC2F,          // Ptr to the value to format
     APLINT     nDigits,            // Raw or E-format:  # significant digits
                                    // F-format:  # digits to right of decimal sep
     APLCHAR    aplCharDecimal,     // Char to use as decimal separator
     APLCHAR    aplCharOverbar,     // Char to use as overbar
     LPAPLCHAR  lpaplCharSep,       // Ptr to char(s) to use a separator
     FLTDISPFMT fltDispFmtMag,      // Float display format (see FLTDISPFMT)
     UBOOL      bSubstInf)          // TRUE iff we're to substitute text for infinity

{
    // Split cases based upon the Complex Number Display Separator
    switch (CndSep)
    {
        case L'i':
        case L'J':
            return
              FormatAplHC2FFC_Cart  (lpaplChar,         // Ptr to output save area
                                     lpaplHC2F,         // Ptr to the value to format
                                     nDigits,           // Raw or E-format:  # significant digits
                                                        // F-format:  # digits to right of decimal sep
                                     nDigits,           // Angle precision
                                     aplCharDecimal,    // Char to use as decimal separator
                                     aplCharOverbar,    // Char to use as overbar
                                     lpaplCharSep,      // Ptr to char(s) to use a separator
                                     fltDispFmtMag,     // Float display format for Magnitude (see FLTDISPFMT)
                                     fltDispFmtMag,     // ...                      Angle     ...
                                     FALSE,             // TRUE iff we're displaying Polar form
                                     bSubstInf);        // TRUE iff we're to substitute text for infinity
        case L'd':
        case L'h':
        case L'r':
        case L'u':
            return
              FormatAplHC2FFC_Polar (lpaplChar,         // Ptr to output save area
                                     lpaplHC2F,         // Ptr to the value to format
                                     nDigits,           // Raw or E-format:  # significant digits
                                                        // F-format:  # digits to right of decimal sep
                                     aplCharDecimal,    // Char to use as decimal separator
                                     aplCharOverbar,    // Char to use as overbar
                                     lpaplCharSep,      // Ptr to char(s) to use a separator
                                     fltDispFmtMag,     // Float display format for Magnitude (see FLTDISPFMT)
                                     bSubstInf);        // TRUE iff we're to substitute text for infinity
        defstop
            return NULL;
    } // End SWITCH
} // End FormatAplHC2FFC


//***************************************************************************
//  $FormatAplHC2FFC_Cart
//
//  Format a HC2F using []FC using Cartesian coordinates
//***************************************************************************

LPAPLCHAR FormatAplHC2FFC_Cart
    (LPWCHAR    lpaplChar,          // Ptr to output save area
     LPAPLHC2F  lpaplHC2F,          // Ptr to the value to format
     APLINT     nDigits,            // Raw or E-format:  # significant digits
                                    // F-format:  # digits to right of decimal sep
     APLUINT    uAnglePrec,         // Angle fractional precision
     APLCHAR    aplCharDecimal,     // Char to use as decimal separator
     APLCHAR    aplCharOverbar,     // Char to use as overbar
     LPAPLCHAR  lpaplCharSep,       // Ptr to char(s) to use a separator
     FLTDISPFMT fltDispFmtMag,      // Float display format for Magnitude (see FLTDISPFMT)
     FLTDISPFMT fltDispFmtAng,      // ...                      Angle     ...
     UBOOL      bAnglePrec,         // TRUE iff we're displaying Polar form
     UBOOL      bSubstInf)          // TRUE iff we're to substitute text for infinity

{
    int i,                          // Loop counter
        iHCDimVar = 2;              // HC Dimension (1, 2, 4, 8)

    // If we are displaying HC in infix form, ...
    if (OptionFlags.bDispInfix)
        // Format the real part
        lpaplChar =
          FormatAplFltFC (lpaplChar,            // Ptr to output save area
                          lpaplHC2F->parts[0],  // The value to format
                          nDigits,              // Raw or E-format:  # significant digits
                                                // F-format:  # digits to right of decimal sep
                          aplCharDecimal,       // Char to use as decimal separator
                          aplCharOverbar,       // Char to use as overbar
                          fltDispFmtMag,        // Float display format for Magnitude (see FLTDISPFMT)
                          bSubstInf);           // TRUE iff we're to substitute text for infinity
    else
    {
        // If the real part is non-zero, or the entire number is zero, ...
        if (lpaplHC2F->parts[0] NE 0
         || IsZeroHCxF (lpaplHC2F, iHCDimVar))
        {
            // Format the real part
            lpaplChar =
              FormatAplFltFC (lpaplChar,            // Ptr to output save area
                              lpaplHC2F->parts[0],  // The value to format
                              nDigits,              // Raw or E-format:  # significant digits
                                                    // F-format:  # digits to right of decimal sep
                              aplCharDecimal,       // Char to use as decimal separator
                              aplCharOverbar,       // Char to use as overbar
                              fltDispFmtMag,        // Float display format for Magnitude (see FLTDISPFMT)
                              bSubstInf);           // TRUE iff we're to substitute text for infinity
            // If the imaginary part of the number is NOT all zero, ...
            if (OptionFlags.bDisp0Imag
             || IzitImaginary (uHCxF[aHCDimToIndex[iHCDimVar]], lpaplHC2F))
                // Append the real part separator
                //  overwriting the trailing blank
                lpaplChar[-1] = L's';
            else
                // Back up to the trailing blank so the next
                //   coefficient overwrites it
                lpaplChar--;
        } // End IF
    } // End IF/ELSE

    // Loop through the imaginary parts
    for (i = 1; i < iHCDimVar; i++)
    // If the imaginary part is non-zero, ...
    if (OptionFlags.bDisp0Imag
     || lpaplHC2F->parts[i] NE 0.0)
    {
        APLINT nDigs;               // nDigits for Angle fractional precision

        // If we are displaying HC in infix form, ...
        if (OptionFlags.bDispInfix)
        {
            // Append the Complex separator
            //  overwriting the trailing blank
            lstrcpyW (&lpaplChar[-1], lpaplCharSep);

            // Skip over the separator
            lpaplChar += lstrlenW (lpaplChar);
        } // End IF

        // If we're displaying Polar form, ...
        if (bAnglePrec)
            nDigs = uAnglePrec;
        else
            nDigs = nDigits;

        // Format the imaginary part
        lpaplChar =
          FormatAplFltFC (lpaplChar,            // Ptr to output save area
                          lpaplHC2F->parts[i],  // The value to format
                          nDigs,                // Raw or E-format:  # significant digits
                                                // F-format:  # digits to right of decimal sep
                          aplCharDecimal,       // Char to use as decimal separator
                          aplCharOverbar,       // Char to use as overbar
                          fltDispFmtAng,        // Float display format for Angle (see FLTDISPFMT)
                          bSubstInf);           // TRUE iff we're to substitute text for infinity
        // If we are NOT displaying HC in infix form, ...
        if (!OptionFlags.bDispInfix)
        {
            // Append the Complex separator
            //  overwriting the trailing blank
            lstrcpyW (&lpaplChar[-1], lpaplCharSep);

            // Skip over the separator
            lpaplChar += lstrlenW (lpaplChar);
        } // End IF
    } // End FOR/IF

    // If we are NOT displaying HC in infix form, ...
    if (!OptionFlags.bDispInfix)
        // Append a trailing blank
        *lpaplChar++ = L' ';

    return lpaplChar;
} // End FormatAplHC2FFC_Cart


//***************************************************************************
//  $FormatAplHC4F
//
//  Format a HC4F number
//***************************************************************************

LPAPLCHAR FormatAplHC4F
    (LPWCHAR  lpaplChar,        // Ptr to output save area
     LPVOID   lpaplHC4F,        // Ptr to the value to format
     APLUINT  nDigits)          // Raw or E-format:  # significant digits
                                // F-format:  # digits to right of decimal sep

{
    return
      FormatAplHCxFFC (lpaplChar,           // Ptr to output save area
                       lpaplHC4F,           // Ptr to the value to format
                       nDigits,             // # significant digits (0 = default)
                       DecSep,              // Char to use as decimal separator
                       NegSign,             // Char to use as overbar
                       FLTDISPFMT_RAWFLT,   // Float display format
                       FALSE,               // TRUE iff we're to substitute text for infinity
                       4);                  // HC Dimension (1, 2, 4, 8)
} // End FormatAplHC4F


//***************************************************************************
//  $FormatAplHC8F
//
//  Format a HC8F number
//***************************************************************************

LPAPLCHAR FormatAplHC8F
    (LPWCHAR  lpaplChar,        // Ptr to output save area
     LPVOID   lpaplHC8F,        // Ptr to the value to format
     APLUINT  nDigits)          // Raw or E-format:  # significant digits
                                // F-format:  # digits to right of decimal sep

{
    return
      FormatAplHCxFFC (lpaplChar,           // Ptr to output save area
                       lpaplHC8F,           // Ptr to the value to format
                       nDigits,             // # significant digits (0 = default)
                       DecSep,              // Char to use as decimal separator
                       NegSign,             // Char to use as overbar
                       FLTDISPFMT_RAWFLT,   // Float display format
                       FALSE,               // TRUE iff we're to substitute text for infinity
                       8);                  // HC Dimension (1, 2, 4, 8)
} // End FormatAplHC8F


//***************************************************************************
//  $FormatAplHCxFFC
//
//  Format a HCxF using []FC
//***************************************************************************

LPAPLCHAR FormatAplHCxFFC
    (LPWCHAR    lpaplChar,          // Ptr to output save area
     LPAPLHC8F  lpaplHC8F,          // Ptr to the value to format
     APLINT     nDigits,            // Raw or E-format:  # significant digits
                                    // F-format:  # digits to right of decimal sep
     APLCHAR    aplCharDecimal,     // Char to use as decimal separator
     APLCHAR    aplCharOverbar,     // Char to use as overbar
     FLTDISPFMT fltDispFmt,         // Float display format (see FLTDISPFMT)
     UBOOL      bSubstInf,          // TRUE iff we're to substitute text for infinity
     int        iHCDimVar)          // HC Dimension (1, 2, 4, 8)

{
    int i;                          // Loop counter

    // If we are displaying HC in infix form, ...
    if (OptionFlags.bDispInfix)
        // Format the real part
        lpaplChar =
          FormatAplFltFC (lpaplChar,            // Ptr to output save area
                          lpaplHC8F->parts[0],  // The value to format
                          nDigits,              // Raw or E-format:  # significant digits
                                                // F-format:  # digits to right of decimal sep
                          aplCharDecimal,       // Char to use as decimal separator
                          aplCharOverbar,       // Char to use as overbar
                          fltDispFmt,           // Float display format (see FLTDISPFMT)
                          bSubstInf);           // TRUE iff we're to substitute text for infinity
    else
    // Otherwise, we're displaying in postfix form
    {
        // If the real part is non-zero, or the entire number is zero, ...
        if (lpaplHC8F->parts[0] NE 0
         || IsZeroHCxF (lpaplHC8F, iHCDimVar))
        {
            // Format the real part
            lpaplChar =
              FormatAplFltFC (lpaplChar,            // Ptr to output save area
                              lpaplHC8F->parts[0],  // The value to format
                              nDigits,              // Raw or E-format:  # significant digits
                                                    // F-format:  # digits to right of decimal sep
                              aplCharDecimal,       // Char to use as decimal separator
                              aplCharOverbar,       // Char to use as overbar
                              fltDispFmt,           // Float display format (see FLTDISPFMT)
                              bSubstInf);           // TRUE iff we're to substitute text for infinity
            // If the imaginary part of the number is NOT all zero, ...
            if (OptionFlags.bDisp0Imag
             || IzitImaginary (uHCxF[aHCDimToIndex[iHCDimVar]], lpaplHC8F))
                // Append the real part separator
                //  overwriting the trailing blank
                lpaplChar[-1] = L's';
            else
                // Back up to the trailing blank so the next
                //   coefficient overwrites it
                lpaplChar--;
        } // End IF
    } // End IF/ELSE

    // Loop through the imaginary parts
    for (i = 1; i < iHCDimVar; i++)
    // If the next imaginary part is non-zero, ...
    if (OptionFlags.bDisp0Imag
     || lpaplHC8F->parts[i] NE 0.0)
    {
        // If we are displaying HC in infix form, ...
        if (OptionFlags.bDispInfix)
        {
            // Append the next HCx separator
            //  overwriting the trailing blank
            strcpyW (&lpaplChar[-1], HC8SEP[i - 1]);

            // Skip over the separator
            lpaplChar += lstrlenW (HC8SEP[i - 1]) - 1;
        } // End IF

        // Format the next imag part
        lpaplChar =
          FormatAplFltFC (lpaplChar,            // Ptr to output save area
                          lpaplHC8F->parts[i],  // The value to format
                          nDigits,              // Raw or E-format:  # significant digits
                                                // F-format:  # digits to right of decimal sep
                          aplCharDecimal,       // Char to use as decimal separator
                          aplCharOverbar,       // Char to use as overbar
                          fltDispFmt,           // Float display format (see FLTDISPFMT)
                          bSubstInf);           // TRUE iff we're to substitute text for infinity
        // If we are NOT displaying HC in infix form, ...
        if (!OptionFlags.bDispInfix)
        {
            // Append the next HCx separator
            //  overwriting the trailing blank
            strcpyW (&lpaplChar[-1], HC8SEP[i - 1]);

            // Skip over the separator
            lpaplChar += lstrlenW (HC8SEP[i - 1]) - 1;
        } // End IF
    } // End FOR/IF

    // If we are NOT displaying HC in infix form, ...
    if (!OptionFlags.bDispInfix)
        // Append a trailing blank
        *lpaplChar++ = L' ';

    return lpaplChar;
} // End FormatAplHCxFFC


//***************************************************************************
//  $FormatAplHC2R
//
//  Format a HC2R number
//***************************************************************************

LPAPLCHAR FormatAplHC2R
    (LPWCHAR  lpaplChar,        // Ptr to output save area
     LPVOID   lpaplHC2R)        // Ptr to the value to format

{
    return
      FormatAplHC2RFC (lpaplChar,       // Ptr to output save area
                       lpaplHC2R,       // Ptr to the value to format
                       NegSign,         // Char to use as overbar
                       DEF_RATSEP,      // Char to use as RAT separator
                       GetHC2Sep (),    // Ptr to char(s) to use as separator
                       FALSE);          // TRUE iff we're to substitute text for infinity
} // End FormatAplHC2R


//***************************************************************************
//  $FormatAplHC2RFC_Polar
//
//  Format a HC2R using []FC in Polar format
//***************************************************************************

LPAPLCHAR FormatAplHC2RFC_Polar
    (LPWCHAR    lpaplChar,          // Ptr to output save area
     LPAPLHC2R  lpaplHC2R,          // Ptr to the value to format
     APLCHAR    aplCharOverbar,     // Char to use as overbar
     APLCHAR    aplCharRatSep,      // Char to use as separator
     LPAPLCHAR  lpaplCharSep,       // Ptr to char(s) to use as separator
     UBOOL      bSubstInf)          // TRUE iff we're to substitute text for infinity

{
    ALLTYPES atRes = {0};
    UBOOL    bRet;

    // Convert the HC2R to HC2V
    (*aTypeActConvert[ARRAY_HC2R][ARRAY_HC2V]) (lpaplHC2R, 0, &atRes, &bRet);

    // Check for error
    if (!bRet)
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    lpaplChar =
      FormatAplHC2VFC_Polar (lpaplChar,         // Ptr to output save area
                            &atRes.aplHC2V,     // Ptr to the value to format
                             GetQuadPPV (),     // Use this many significant digits for VFP
                             DecSep,            // Char to use as decimal separator
                             aplCharOverbar,    // Char to use as overbar
                             lpaplCharSep,      // Ptr to char(s) to use as separator
                             FLTDISPFMT_RAWFLT, // Float display format
                             FALSE,             // TRUE iff nDigits is # fractional digits
                             FALSE,             // TRUE iff we're to substitute text for infinity
                             FALSE);            // TRUE iff we're to precede the display with (FPCnnn)
    // We no longer need this storage
    Myhc2v_clear (&atRes.aplHC2V);

    return lpaplChar;
} // End FormatAplHC2RFC_Polar


//***************************************************************************
//  $FormatAplHC2RFC
//
//  Format a HC2R using []FC
//***************************************************************************

LPAPLCHAR FormatAplHC2RFC
    (LPWCHAR    lpaplChar,          // Ptr to output save area
     LPAPLHC2R  lpaplHC2R,          // Ptr to the value to format
     APLCHAR    aplCharOverbar,     // Char to use as overbar
     APLCHAR    aplCharRatSep,      // Char to use as separator
     LPAPLCHAR  lpaplCharSep,       // Ptr to char(s) to use as separator
     UBOOL      bSubstInf)          // TRUE iff we're to substitute text for infinity

{
    // Split cases based upon the Complex Number Display Separator
    switch (CndSep)
    {
        case L'i':
        case L'J':
            return
              FormatAplHC2RFC_Cart  (lpaplChar,          // Ptr to output save area
                                     lpaplHC2R,          // Ptr to the value to format
                                     aplCharOverbar,     // Char to use as overbar
                                     aplCharRatSep,      // Char to use as separator
                                     lpaplCharSep,       // Ptr to char(s) to use as separator
                                     bSubstInf);         // TRUE iff we're to substitute text for infinity

        case L'd':
        case L'h':
        case L'r':
        case L'u':
            return
              FormatAplHC2RFC_Polar (lpaplChar,         // Ptr to output save area
                                     lpaplHC2R,         // Ptr to the value to format
                                     aplCharOverbar,    // Char to use as overbar
                                     aplCharRatSep,     // Char to use as separator
                                     lpaplCharSep,      // Ptr to char(s) to use as separator
                                     bSubstInf);        // TRUE iff we're to substitute text for infinity
        defstop
            return NULL;
    } // End SWITCH
} // End FormatAplHC2R


//***************************************************************************
//  $FormatAplHC2RFC_Cart
//
//  Format a HC2R using []FC using Cartesian coordinates
//***************************************************************************

LPAPLCHAR FormatAplHC2RFC_Cart
    (LPWCHAR    lpaplChar,          // Ptr to output save area
     LPAPLHC2R  lpaplHC2R,          // Ptr to the value to format
     APLCHAR    aplCharOverbar,     // Char to use as overbar
     APLCHAR    aplCharRatSep,      // Char to use as separator
     LPAPLCHAR  lpaplCharSep,       // Ptr to char(s) to use as separator
     UBOOL      bSubstInf)          // TRUE iff we're to substitute text for infinity

{
    int   iHCDimVar = 2,
           i;
    UBOOL bImag0[2],
          bScan0[2];

    bScan0[iHCDimVar - 1] = TRUE;

    // Loop through the imaginary parts
    for (i = iHCDimVar - 1; i >= 1; i--)
    {
        bImag0[i] = IsMpq0 (&lpaplHC2R->parts[i]);

        bScan0[i - 1] &= bScan0[i];
    } // End IF

    // If we are displaying HC in infix form, ...
    if (OptionFlags.bDispInfix)
        // Format the real part
        lpaplChar =
          FormatAplRatFC (lpaplChar,            // Ptr to output save area
                         &lpaplHC2R->parts[0],  // Ptr to the value to format
                          aplCharOverbar,       // Char to use as overbar
                          aplCharRatSep,        // Char to use as separator
                          bSubstInf,            // TRUE iff we're to substitute text for infinity
                         !bScan0[0]);           // TRUE iff this RAT is inside a larger syntax
    else
    // Otherwise, we're displaying in postfix form
    {
        // If the real part is non-zero, or the entire number is zero, ...
        if (!IsZeroHCxR (&lpaplHC2R->parts[0], 1)
         || IsZeroHCxR (lpaplHC2R, iHCDimVar))
        {
            lpaplChar =
              FormatAplRatFC (lpaplChar,            // Ptr to output save area
                             &lpaplHC2R->parts[0],  // Ptr to the value to format
                              aplCharOverbar,       // Char to use as overbar
                              aplCharRatSep,        // Char to use as separator
                              bSubstInf,            // TRUE iff we're to substitute text for infinity
                             !bScan0[0]);           // TRUE iff this RAT is inside a larger syntax
            // If the imaginary part of the number is NOT all zero, ...
            if (OptionFlags.bDisp0Imag
             || IzitImaginary (uHCxR[aHCDimToIndex[iHCDimVar]], lpaplHC2R))
                // Append the real part separator
                //  overwriting the trailing blank
                lpaplChar[-1] = L's';
            else
                // Back up to the trailing blank so the next
                //   coefficient overwrites it
                lpaplChar--;
        } // End IF
    } // End IF/ELSE

    // Loop through the imaginary parts
    for (i = 1; i < iHCDimVar; i++)
    // If the imaginary part is non-zero, ...
    if (OptionFlags.bDisp0Imag
     || !bImag0[i])
    {
        // If we are displaying HC in infix form, ...
        if (OptionFlags.bDispInfix)
        {
            // Append the Complex separator
            //  overwriting the trailing blank
            lstrcpyW (&lpaplChar[-1], lpaplCharSep);

            // Skip over the separator
            lpaplChar += lstrlenW (lpaplChar);
        } // End IF

        // Format the next imaginary part
        lpaplChar =
          FormatAplRatFC (lpaplChar,            // Ptr to output save area
                         &lpaplHC2R->parts[i],  // Ptr to the value to format
                          aplCharOverbar,       // Char to use as overbar
                          aplCharRatSep,        // Char to use as separator
                          bSubstInf,            // TRUE iff we're to substitute text for infinity
                         !bScan0[i]);           // TRUE iff this RAT is inside a larger syntax
        // If we are NOT displaying HC in infix form, ...
        if (!OptionFlags.bDispInfix)
        {
            // Append the Complex separator
            //  overwriting the trailing blank
            lstrcpyW (&lpaplChar[-1], lpaplCharSep);

            // Skip over the separator
            lpaplChar += lstrlenW (lpaplChar);
        } // End IF
    } // End FOR/IF

    // If we are NOT displaying HC in infix form, ...
    if (!OptionFlags.bDispInfix)
        // Append a trailing blank
        *lpaplChar++ = L' ';

    return lpaplChar;
} // End FormatAplHC2R_Cart


//***************************************************************************
//  $FormatAplHC4R
//
//  Format a HC4R number
//***************************************************************************

LPAPLCHAR FormatAplHC4R
    (LPWCHAR  lpaplChar,        // Ptr to output save area
     LPVOID   lpaplHC4R)        // Ptr to the value to format

{
    return
      FormatAplHCxRFC (lpaplChar,       // Ptr to output save area
                       lpaplHC4R,       // Ptr to the value to format
                       NegSign,         // Char to use as overbar
                       DEF_RATSEP,      // Char to use as RAT separator
                       FALSE,           // TRUE iff we're to substitute text for infinity
                       4);              // HC Dimension (1, 2, 4, 8)
} // End FormatAplHC4R


//***************************************************************************
//  $FormatAplHC8R
//
//  Format a HC8R number
//***************************************************************************

LPAPLCHAR FormatAplHC8R
    (LPWCHAR  lpaplChar,        // Ptr to output save area
     LPVOID   lpaplHC8R)        // Ptr to the value to format

{
    return
      FormatAplHCxRFC (lpaplChar,       // Ptr to output save area
                       lpaplHC8R,       // Ptr to the value to format
                       NegSign,         // Char to use as overbar
                       DEF_RATSEP,      // Char to use as RAT separator
                       FALSE,           // TRUE iff we're to substitute text for infinity
                       8);              // HC Dimension (1, 2, 4, 8)
} // End FormatAplHC8R


//***************************************************************************
//  $FormatAplHCxRFC
//
//  Format a HCxR using []FC
//***************************************************************************

LPAPLCHAR FormatAplHCxRFC
    (LPWCHAR    lpaplChar,          // Ptr to output save area
     LPAPLHC8R  lpaplHC8R,          // Ptr to the value to format
     APLCHAR    aplCharOverbar,     // Char to use as overbar
     APLCHAR    aplCharRatSep,      // Char to use as separator
     UBOOL      bSubstInf,          // TRUE iff we're to substitute text for infinity
     int        iHCDimVar)          // HC Dimension (1, 2, 4, 8)

{
    int   i;                        // Loop counter
    UBOOL bImag0[8],
          bScan0[8];

    bScan0[iHCDimVar - 1] = TRUE;

    // Loop through the imaginary parts
    for (i = iHCDimVar - 1; i >= 1; i--)
    {
        bImag0[i] = IsMpq0 (&lpaplHC8R->parts[i]);

        bScan0[i - 1] &= bScan0[i];
    } // End IF

    // If we are displaying HC in infix form, ...
    if (OptionFlags.bDispInfix)
        // Format the real part
        lpaplChar =
          FormatAplRatFC (lpaplChar,            // Ptr to output save area
                         &lpaplHC8R->parts[0],  // Ptr to the value to format
                          aplCharOverbar,       // Char to use as overbar
                          aplCharRatSep,        // Char to use as separator
                          bSubstInf,            // TRUE iff we're to substitute text for infinity
                         !bScan0[0]);           // TRUE iff this RAT is inside a larger syntax
    else
    // Otherwise, we're displaying in postfix form
    {
        // If the real part is non-zero, or the entire number is zero, ...
        if (!IsZeroHCxR (&lpaplHC8R->parts[0], 1)
         || IsZeroHCxR (lpaplHC8R, iHCDimVar))
        {
            // Format the real part
            lpaplChar =
              FormatAplRatFC (lpaplChar,            // Ptr to output save area
                             &lpaplHC8R->parts[0],  // Ptr to the value to format
                              aplCharOverbar,       // Char to use as overbar
                              aplCharRatSep,        // Char to use as separator
                              bSubstInf,            // TRUE iff we're to substitute text for infinity
                             !bScan0[0]);           // TRUE iff this RAT is inside a larger syntax
            // If the imaginary part of the number is NOT all zero, ...
            if (OptionFlags.bDisp0Imag
             || IzitImaginary (uHCxR[aHCDimToIndex[iHCDimVar]], lpaplHC8R))
                // Append the real part separator
                //  overwriting the trailing blank
                lpaplChar[-1] = L's';
            else
                // Back up to the trailing blank so the next
                //   coefficient overwrites it
                lpaplChar--;
        } // End IF
    } // End IF/ELSE

    // Loop through the imaginary parts
    for (i = 1; i < iHCDimVar; i++)
    // If the next imaginary part is non-zero, ...
    if (OptionFlags.bDisp0Imag
     || !bImag0[i])
    {
        // If we are displaying HC in infix form, ...
        if (OptionFlags.bDispInfix)
        {
            // Append the next HCx separator
            //  overwriting the trailing blank
            strcpyW (&lpaplChar[-1], HC8SEP[i - 1]);

            // Skip over the separator
            lpaplChar += lstrlenW (HC8SEP[i - 1]) - 1;
        } // End IF

        // Format the next imag part
        lpaplChar =
          FormatAplRatFC (lpaplChar,            // Ptr to output save area
                         &lpaplHC8R->parts[i],  // Ptr to the value to format
                          aplCharOverbar,       // Char to use as overbar
                          aplCharRatSep,        // Char to use as separator
                          bSubstInf,            // TRUE iff we're to substitute text for infinity
                         !bScan0[i]);           // TRUE iff this RAT is inside a larger syntax
        // If we are NOT displaying HC in infix form, ...
        if (!OptionFlags.bDispInfix)
        {
            // Append the next HCx separator
            //  overwriting the trailing blank
            strcpyW (&lpaplChar[-1], HC8SEP[i - 1]);

            // Skip over the separator
            lpaplChar += lstrlenW (HC8SEP[i - 1]) - 1;
        } // End IF
    } // End FOR/IF

    // If we are NOT displaying HC in infix form, ...
    if (!OptionFlags.bDispInfix)
        // Append a trailing blank
        *lpaplChar++ = L' ';

    return lpaplChar;
} // End FormatAplHCxRFC


//***************************************************************************
//  $FormatAplHC2V
//
//  Format a HC2V number
//***************************************************************************

LPAPLCHAR FormatAplHC2V
    (LPWCHAR  lpaplChar,        // Ptr to output save area
     LPVOID   lpaplHC2V,        // Ptr to the value to format
     APLUINT  nDigits)          // # significant digits (0 = all)

{
    return
      FormatAplHC2VFC (lpaplChar,           // Ptr to output save area
                       lpaplHC2V,           // Ptr to the value to format
                       nDigits,             // # significant digits (0 = all)
                       DecSep,              // Char to use as decimal separator
                       NegSign,             // Char to use as overbar
                       GetHC2Sep (),        // Ptr to char(s) to use as separator
                       FLTDISPFMT_RAWFLT,   // Float display format
                       FALSE,               // TRUE iff nDigits is # fractional digits
                       FALSE,               // TRUE iff we're to substitute text for infinity
                       FALSE);              // TRUE iff we're to precede the display with (FPCnnn)
} // End FormatAplHC2V


//***************************************************************************
//  $FormatAplHC2VFC_Polar
//
//  Format a HC2V using []FC in Polar format
//***************************************************************************

LPAPLCHAR FormatAplHC2VFC_Polar
    (LPWCHAR    lpaplChar,          // Ptr to output save area
     LPAPLHC2V  lpaplHC2V,          // Ptr to the value to format
     APLINT     nDigits,            // # significant digits (0 = all), or
                                    // # fractional digits (if bFractDigs)
                                    // If negative, use E-format
     APLCHAR    aplCharDecimal,     // Char to use as decimal separator
     APLCHAR    aplCharOverbar,     // Char to use as overbar
     LPAPLCHAR  lpaplCharSep,       // Ptr to char(s) to use as separator
     FLTDISPFMT fltDispFmtMag,      // Float display format for Magnitude (see FLTDISPFMT)
     UBOOL      bFractDigs,         // TRUE iff nDigits is # fractional digits
     UBOOL      bSubstInf,          // TRUE iff we're to substitute text for infinity
     UBOOL      bPrecFPC)           // TRUE iff we're to precede the display with (FPCnnn)

{
    LPPERTABDATA lpMemPTD = GetMemPTD ();           // Ptr to PerTabData global memory handle
    APLHC2V      aplRes = {0};                      // The result
    APLUINT      uAnglePrec = DEF_ANGLEPREC_UNK;    // Angle fractional precision
    UBOOL        bAnglePrec = FALSE;                // TRUE iff we're displaying Polar form
    FLTDISPFMT   fltDispFmtAng = fltDispFmtMag;     // Float display format for Angle
    APLCHAR      aplCNDSEP;                         // Value of CndSep

    // Get the current value of CNDSEP from []FC
    aplCNDSEP = CndSep;

    // Initialize the high part
    mpfr_init0 (&aplRes.partsHi);

    // Convert the HC2V to Angle in Signed-Radians [-1p1, 1p1]
    mpfr_atan2 (&aplRes.partsHi, &lpaplHC2V->partsHi, &lpaplHC2V->partsLo, MPFR_RNDN);

    // Ensure the Angle is in Radians [0, 2p1] unless we're using 'ah'
    //   in which case leave it in Signed-Radians [-1p1, 1p1]
    if (mpfr_sgn (&aplRes.partsHi) < 0
     && aplCNDSEP NE L'h')
    {
        // Add in 2p1
        mpfr_add (&aplRes.partsHi, &aplRes.partsHi, &lpMemPTD->mpfrHC8V_Pi.parts[0], MPFR_RNDN);
        mpfr_add (&aplRes.partsHi, &aplRes.partsHi, &lpMemPTD->mpfrHC8V_Pi.parts[0], MPFR_RNDN);
    } // End IF

    // Convert the HC2V to Radius
    aplRes.partsLo = MagHC2V (*lpaplHC2V);

    // Split cases based upon the Complex Number Display Separator
    switch (aplCNDSEP)
    {
        case L'd':
            // Check for reduced angle fractional precision
            if (uAnglePrec EQ DEF_ANGLEPREC_UNK)
            {
                uAnglePrec    = DEF_ANGLEPREC_ad;   // # digits to right of decimal sep
                fltDispFmtAng = FLTDISPFMT_F;       // Float display format for Angle
                bAnglePrec    = TRUE;
            };

            // If entered at L'd',
            //      convert the Angle in Radians [0, 2p1] to Degree-Radians [0, 720p1]
            mpfr_mul_si (&aplRes.partsHi, &aplRes.partsHi, 360, MPFR_RNDN);

            // Fall through to common code

        case L'h':
            // Check for reduced angle fractional precision
            if (uAnglePrec EQ DEF_ANGLEPREC_UNK)
            {
                uAnglePrec    = DEF_ANGLEPREC_ah;   // # digits to right of decimal sep
                fltDispFmtAng = FLTDISPFMT_F;       // Float display format for Angle
                bAnglePrec    = TRUE;
            };

            // Fall through to common code

        case L'u':
            // Check for reduced angle fractional precision
            if (uAnglePrec EQ DEF_ANGLEPREC_UNK)
            {
                uAnglePrec    = DEF_ANGLEPREC_au;   // # digits to right of decimal sep
                fltDispFmtAng = FLTDISPFMT_F;       // Float display format for Angle
                bAnglePrec    = TRUE;
            };

            // If entered at L'h',
            //      convert the Angle in Signed-Radians [-1p1, 1p1] to Signed Unit Normalized [-0.5, 0.5]
            // If entered at L'd',
            //      convert the Angle in Degree-Radians [0, 720p1] to Degrees [0, 360]
            // If entered at L'u',
            //      convert the Angle in Radians [0, 2p1] to the Unit Normalized [0, 1]
            mpfr_div    (&aplRes.partsHi, &aplRes.partsHi, &lpMemPTD->mpfrHC8V_Pi.parts[0], MPFR_RNDN);
            mpfr_div_si (&aplRes.partsHi, &aplRes.partsHi, 2                              , MPFR_RNDN);

            // Fall through to common code

        case L'i':
        case L'J':
            // Check for reduced angle fractional precision
            if (uAnglePrec EQ DEF_ANGLEPREC_UNK)
            {
                uAnglePrec    = nDigits;            // # significant digits
////////////////fltDispFmtAng = FltDispFmtMag;      // Float display format for Magnitude
////////////////bAnglePrec    = FALSE;              // Already FALSE
            };

            // Fall through to common code

        case L'r':
            // Check for reduced angle fractional precision
            if (uAnglePrec EQ DEF_ANGLEPREC_UNK)
            {
                uAnglePrec    = DEF_ANGLEPREC_ar;   // # digits to right of decimal sep
                fltDispFmtAng = FLTDISPFMT_F;       // Float display format for Angle
                bAnglePrec    = TRUE;
            };

            lpaplChar =
              FormatAplHC2VFC_Cart  (lpaplChar,         // Ptr to output save area                                         return
                                    &aplRes,            // Ptr to the value to format
                                     nDigits,           // # significant digits (0 = all), or
                                                        // # fractional digits (if bFractDigs)
                                                        // If negative, use E-format
                                     uAnglePrec,        // Angle fractional precision
                                     aplCharDecimal,    // Char to use as decimal separator
                                     aplCharOverbar,    // Char to use as overbar
                                     lpaplCharSep,      // Ptr to char(s) to use as separator
                                     fltDispFmtMag,     // Float display format for Magnitude (see FLTDISPFMT)
                                     fltDispFmtAng,     // ...                      Angle     ...
                                     bFractDigs,        // TRUE iff nDigits is # fractional digits
                                     bAnglePrec,        // TRUE iff we're displaying Polar form
                                     bSubstInf,         // TRUE iff we're to substitute text for infinity
                                     bPrecFPC);         // TRUE iff we're to precede the display with (FPCnnn)
            // We no longer need this storage
            Myhc2v_clear (&aplRes);

            return lpaplChar;

        defstop
            return NULL;
    } // End SWITCH
} // End FormatAplHC2VFC_Polar


//***************************************************************************
//  $FormatAplHC2VFC
//
//  Format a HC2V using []FC
//***************************************************************************

LPAPLCHAR FormatAplHC2VFC
    (LPWCHAR    lpaplChar,          // Ptr to output save area
     LPAPLHC2V  lpaplHC2V,          // Ptr to the value to format
     APLINT     nDigits,            // # significant digits (0 = all), or
                                    // # fractional digits (if bFractDigs)
                                    // If negative, use E-format
     APLCHAR    aplCharDecimal,     // Char to use as decimal separator
     APLCHAR    aplCharOverbar,     // Char to use as overbar
     LPAPLCHAR  lpaplCharSep,       // Ptr to char(s) to use as separator
     FLTDISPFMT fltDispFmtMag,      // Float display format for Magnitude (see FLTDISPFMT)
     UBOOL      bFractDigs,         // TRUE iff nDigits is # fractional digits
     UBOOL      bSubstInf,          // TRUE iff we're to substitute text for infinity
     UBOOL      bPrecFPC)           // TRUE iff we're to precede the display with (FPCnnn)

{
    // Split cases based upon the Complex Number Display Separator
    switch (CndSep)
    {
        case L'i':
        case L'J':
            return
              FormatAplHC2VFC_Cart  (lpaplChar,         // Ptr to output save area
                                     lpaplHC2V,         // Ptr to the value to format
                                     nDigits,           // # significant digits (0 = all), or
                                                        // # fractional digits (if bFractDigs)
                                                        // If negative, use E-format
                                     nDigits,           // Angle precision
                                     aplCharDecimal,    // Char to use as decimal separator
                                     aplCharOverbar,    // Char to use as overbar
                                     lpaplCharSep,      // Ptr to char(s) to use as separator
                                     fltDispFmtMag,     // Float display format for Magnitude (see FLTDISPFMT)
                                     fltDispFmtMag,     // ...                      Angle     ...
                                     bFractDigs,        // TRUE iff nDigits is # fractional digits
                                     FALSE,             // TRUE iff we're displaying Polar form
                                     bSubstInf,         // TRUE iff we're to substitute text for infinity
                                     bPrecFPC);         // TRUE iff we're to precede the display with (FPCnnn)
        case L'd':
        case L'h':
        case L'r':
        case L'u':
            return
              FormatAplHC2VFC_Polar (lpaplChar,         // Ptr to output save area
                                     lpaplHC2V,         // Ptr to the value to format
                                     nDigits,           // # significant digits (0 = all), or
                                                        // # fractional digits (if bFractDigs)
                                                        // If negative, use E-format
                                     aplCharDecimal,    // Char to use as decimal separator
                                     aplCharOverbar,    // Char to use as overbar
                                     lpaplCharSep,      // Ptr to char(s) to use as separator
                                     fltDispFmtMag,     // Float display format for Magnitude (see FLTDISPFMT)
                                     bFractDigs,        // TRUE iff nDigits is # fractional digits
                                     bSubstInf,         // TRUE iff we're to substitute text for infinity
                                     bPrecFPC);         // TRUE iff we're to precede the display with (FPCnnn)
        defstop
            return NULL;
    } // End SWITCH
} // End FormatAplHC2VFC


//***************************************************************************
//  $FormatAplHC2VFC_Cart
//
//  Format a HC2V using []FC using Cartesian coordinates
//***************************************************************************

LPAPLCHAR FormatAplHC2VFC_Cart
    (LPWCHAR    lpaplChar,          // Ptr to output save area
     LPAPLHC2V  lpaplHC2V,          // Ptr to the value to format
     APLINT     nDigits,            // # significant digits (0 = all), or
                                    // # fractional digits (if bFractDigs)
                                    // If negative, use E-format
     APLUINT    uAnglePrec,         // Angle fractional precision
     APLCHAR    aplCharDecimal,     // Char to use as decimal separator
     APLCHAR    aplCharOverbar,     // Char to use as overbar
     LPAPLCHAR  lpaplCharSep,       // Ptr to char(s) to use as separator
     FLTDISPFMT fltDispFmtMag,      // Float display format for Magnitude (see FLTDISPFMT)
     FLTDISPFMT fltDispFmtAng,      // ...                      Angle     ...
     UBOOL      bFractDigs,         // TRUE iff nDigits is # fractional digits
     UBOOL      bAnglePrec,         // TRUE iff we're displaying Polar form
     UBOOL      bSubstInf,          // TRUE iff we're to substitute text for infinity
     UBOOL      bPrecFPC)           // TRUE iff we're to precede the display with (FPCnnn)

{
    int i,                          // Loop counter
        iHCDimVar = 2;              // HC Dimension (1, 2, 4, 8)

    // If we are displaying HC in infix form, ...
    if (OptionFlags.bDispInfix)
        // Format the real part
        lpaplChar =
          FormatAplVfpFC (lpaplChar,            // Ptr to output save area
                         &lpaplHC2V->parts[0],  // Ptr to the value to format
                          nDigits,              // # significant digits (0 = all), or
                                                // # fractional digits (if bFractDigs)
                                                // If negative, use E-format
                          0,                    // Maximum width including sign & decpt (0 = none)
                          aplCharDecimal,       // Char to use as decimal separator
                          aplCharOverbar,       // Char to use as overbar
                          fltDispFmtMag,        // Float display format for Magnitude (see FLTDISPFMT)
                          bFractDigs,           // TRUE iff nDigits is # fractional digits
                          bSubstInf,            // TRUE iff we're to substitute text for infinity
                          bPrecFPC);            // TRUE iff we're to precede the display with (FPCnnn)
    else
    // Otherwise, we're displaying in postfix form
    {
        // If the real part is non-zero, or the entire number is zero, ...
        if (!IsZeroHCxV (&lpaplHC2V->parts[0], 1)
         || IsZeroHCxV (lpaplHC2V, iHCDimVar))
        {
            // Format the real part
            lpaplChar =
              FormatAplVfpFC (lpaplChar,            // Ptr to output save area
                             &lpaplHC2V->parts[0],  // Ptr to the value to format
                              nDigits,              // # significant digits (0 = all), or
                                                    // # fractional digits (if bFractDigs)
                                                    // If negative, use E-format
                              0,                    // Maximum width including sign & decpt (0 = none)
                              aplCharDecimal,       // Char to use as decimal separator
                              aplCharOverbar,       // Char to use as overbar
                              fltDispFmtMag,        // Float display format for Magnitude (see FLTDISPFMT)
                              bFractDigs,           // TRUE iff nDigits is # fractional digits
                              bSubstInf,            // TRUE iff we're to substitute text for infinity
                              bPrecFPC);            // TRUE iff we're to precede the display with (FPCnnn)
            // If the imaginary part of the number is NOT all zero, ...
            if (OptionFlags.bDisp0Imag
             || IzitImaginary (uHCxV[aHCDimToIndex[iHCDimVar]], lpaplHC2V))
                // Append the real part separator
                //  overwriting the trailing blank
                lpaplChar[-1] = L's';
            else
                // Back up to the trailing blank so the next
                //   coefficient overwrites it
                lpaplChar--;
        } // End IF
    } // End IF/ELSE

    // Loop through the imaginary parts
    for (i = 1; i < iHCDimVar; i++)
    // If the imaginary part is non-zero, ...
    if (OptionFlags.bDisp0Imag
     || !IsMpf0 (&lpaplHC2V->parts[i]))
    {
        APLINT     nDigs;           // nDigits for Angle fractional precision
        UBOOL      bFDigs;          // bFractDigs for ...

        // If we are displaying HC in infix form, ...
        if (OptionFlags.bDispInfix)
        {
            // Append the Complex separator
            //  overwriting the trailing blank
            lstrcpyW (&lpaplChar[-1], lpaplCharSep);

            // Skip over the separator
            lpaplChar += lstrlenW (lpaplChar);
        } // End IF

        // If we're displaying Polar form, ...
        if (bAnglePrec)
        {
            nDigs   = uAnglePrec;
            bFDigs  = TRUE;
        } else
        {
            nDigs   = nDigits;
            bFDigs  = bFractDigs;
        } // End IF/ELSE

        // Format the imaginary part
        lpaplChar =
          FormatAplVfpFC (lpaplChar,            // Ptr to output save area
                         &lpaplHC2V->parts[i],  // Ptr to the value to format
                          nDigs,                // # significant digits (0 = all), or
                                                // # fractional digits (if bFractDigs)
                                                // If negative, use E-format
                          0,                    // Maximum width including sign & decpt (0 = none)
                          aplCharDecimal,       // Char to use as decimal separator
                          aplCharOverbar,       // Char to use as overbar
                          fltDispFmtAng,        // Float display format for Angle (see FLTDISPFMT)
                          bFDigs,               // TRUE iff nDigits is # fractional digits
                          bSubstInf,            // TRUE iff we're to substitute text for infinity
                          bPrecFPC);            // TRUE iff we're to precede the display with (FPCnnn)
        // If we are NOT displaying HC in infix form, ...
        if (!OptionFlags.bDispInfix)
        {
            // Append the Complex separator
            //  overwriting the trailing blank
            lstrcpyW (&lpaplChar[-1], lpaplCharSep);

            // Skip over the separator
            lpaplChar += lstrlenW (lpaplChar);
        } // End IF
    } // End FOR/IF

    // If we are NOT displaying HC in infix form, ...
    if (!OptionFlags.bDispInfix)
        // Append a trailing blank
        *lpaplChar++ = L' ';

    return lpaplChar;
} // End FormatAplHC2VFC_Cart


//***************************************************************************
//  $FormatAplBA2FFC
//
//  Format a BA2F using []FC
//***************************************************************************

LPAPLCHAR FormatAplBA2FFC
    (LPWCHAR    lpaplChar,          // Ptr to output save area
     LPAPLBA2F  lpaplBA2F,          // Ptr to the value to format
     APLINT     nDigits,            // # significant digits (0 = all), or
                                    // # fractional digits (if bFractDigs)
                                    // If negative, use E-format
     APLCHAR    aplCharDecimal,     // Char to use as decimal separator
     APLCHAR    aplCharOverbar,     // Char to use as overbar
     LPAPLCHAR  lpaplCharSep,       // Ptr to char(s) to use as separator
     FLTDISPFMT fltDispFmt,         // Float display format (see FLTDISPFMT)
     UBOOL      bFractDigs,         // TRUE iff nDigits is # fractional digits
     UBOOL      bSubstInf,          // TRUE iff we're to substitute text for infinity
     UBOOL      bPrecFPC)           // TRUE iff we're to precede the display with (FPCnnn)

{
    int i,                          // Loop counter
        iHCDimVar = 2;              // HC Dimension (1, 2, 4, 8)

    // If we are displaying HC in infix form, ...
    if (OptionFlags.bDispInfix)
        // Format the real part
        lpaplChar =
          FormatAplArbFC (lpaplChar,            // Ptr to output save area
                         &lpaplBA2F->parts[0],  // Ptr to the value to format
                          nDigits,              // # significant digits (0 = all), or
                                                // # fractional digits (if bFractDigs)
                                                // If negative, use E-format
                          0,                    // Maximum width including sign & decpt (0 = none)
                          aplCharDecimal,       // Char to use as decimal separator
                          aplCharOverbar,       // Char to use as overbar
                          fltDispFmt,           // Float display format (see FLTDISPFMT)
                          bFractDigs,           // TRUE iff nDigits is # fractional digits
                          bSubstInf,            // TRUE iff we're to substitute text for infinity
                          bPrecFPC);            // TRUE iff we're to precede the display with (FPCnnn)
    else
    // Otherwise, we're displaying in postfix form
    {
        // If the real part is non-zero, or the entire number is zero, ...
        if (!IsZeroHCxV (&lpaplBA2F->parts[0], 1)
         || IsZeroHCxV (lpaplBA2F, iHCDimVar))
        {
            // Format the real part
            lpaplChar =
              FormatAplArbFC (lpaplChar,            // Ptr to output save area
                             &lpaplBA2F->parts[0],  // Ptr to the value to format
                              nDigits,              // # significant digits (0 = all), or
                                                    // # fractional digits (if bFractDigs)
                                                    // If negative, use E-format
                              0,                    // Maximum width including sign & decpt (0 = none)
                              aplCharDecimal,       // Char to use as decimal separator
                              aplCharOverbar,       // Char to use as overbar
                              fltDispFmt,           // Float display format (see FLTDISPFMT)
                              bFractDigs,           // TRUE iff nDigits is # fractional digits
                              bSubstInf,            // TRUE iff we're to substitute text for infinity
                              bPrecFPC);            // TRUE iff we're to precede the display with (FPCnnn)
            // If the imaginary part of the number is NOT all zero, ...
            if (OptionFlags.bDisp0Imag
             || IzitImaginary (uHCxV[aHCDimToIndex[iHCDimVar]], lpaplBA2F))
                // Append the real part separator
                //  overwriting the trailing blank
                lpaplChar[-1] = L's';
            else
                // Back up to the trailing blank so the next
                //   coefficient overwrites it
                lpaplChar--;
        } // End IF
    } // End IF/ELSE

    // Loop through the imaginary parts
    for (i = 1; i < iHCDimVar; i++)
    // If the imaginary part is non-zero, ...
    if (OptionFlags.bDisp0Imag
     || !IsArb0 (&lpaplBA2F->parts[i]))
    {
        // If we are displaying HC in infix form, ...
        if (OptionFlags.bDispInfix)
        {
            // Append the Complex separator
            //  overwriting the trailing blank
            lstrcpyW (&lpaplChar[-1], lpaplCharSep);

            // Skip over the separator
            lpaplChar += lstrlenW (lpaplChar);
        } // End IF

        // Format the next imaginary part
        lpaplChar =
          FormatAplArbFC (lpaplChar,            // Ptr to output save area
                         &lpaplBA2F->parts[i],  // Ptr to the value to format
                          nDigits,              // # significant digits (0 = all), or
                                                // # fractional digits (if bFractDigs)
                                                // If negative, use E-format
                          0,                    // Maximum width including sign & decpt (0 = none)
                          aplCharDecimal,       // Char to use as decimal separator
                          aplCharOverbar,       // Char to use as overbar
                          fltDispFmt,           // Float display format (see FLTDISPFMT)
                          bFractDigs,           // TRUE iff nDigits is # fractional digits
                          bSubstInf,            // TRUE iff we're to substitute text for infinity
                          bPrecFPC);            // TRUE iff we're to precede the display with (FPCnnn)
        // If we are NOT displaying HC in infix form, ...
        if (!OptionFlags.bDispInfix)
        {
            // Append the Complex separator
            //  overwriting the trailing blank
            lstrcpyW (&lpaplChar[-1], lpaplCharSep);

            // Skip over the separator
            lpaplChar += lstrlenW (lpaplChar);
        } // End IF
    } // End FOR/IF

    // If we are NOT displaying HC in infix form, ...
    if (!OptionFlags.bDispInfix)
        // Append a trailing blank
        *lpaplChar++ = L' ';

    return lpaplChar;
} // End FormatAplBA2FFC


//***************************************************************************
//  $FormatAplHC4V
//
//  Format a HC4V number
//***************************************************************************

LPAPLCHAR FormatAplHC4V
    (LPWCHAR  lpaplChar,        // Ptr to output save area
     LPVOID   lpaplHC4V,        // Ptr to the value to format
     APLUINT  nDigits)          // # significant digits (0 = all)

{
    return
      FormatAplHCxVFC (lpaplChar,           // Ptr to output save area
                       lpaplHC4V,           // Ptr to the value to format
                       nDigits,             // # significant digits (0 = all)
                       DecSep,              // Char to use as decimal separator
                       NegSign,             // Char to use as overbar
                       FLTDISPFMT_RAWFLT,   // Float display format
                       FALSE,               // TRUE iff nDigits is # fractional digits
                       FALSE,               // TRUE iff we're to substitute text for infinity
                       FALSE,               // TRUE iff we're to precede the display with (FPCnnn)
                       4);                  // HC Dimension (1, 2, 4, 8)
} // End FormatAplHC4V


//***************************************************************************
//  $FormatAplHC8V
//
//  Format a HC8V number
//***************************************************************************

LPAPLCHAR FormatAplHC8V
    (LPWCHAR  lpaplChar,        // Ptr to output save area
     LPVOID   lpaplHC8V,        // Ptr to the value to format
     APLUINT  nDigits)          // # significant digits (0 = all)

{
    return
      FormatAplHCxVFC (lpaplChar,           // Ptr to output save area
                       lpaplHC8V,           // Ptr to the value to format
                       nDigits,             // # significant digits (0 = all)
                       DecSep,              // Char to use as decimal separator
                       NegSign,             // Char to use as overbar
                       FLTDISPFMT_RAWFLT,   // Float display format
                       FALSE,               // TRUE iff nDigits is # fractional digits
                       FALSE,               // TRUE iff we're to substitute text for infinity
                       FALSE,               // TRUE iff we're to precede the display with (FPCnnn)
                       8);                  // HC Dimension (1, 2, 4, 8)
} // End FormatAplHC8V


//***************************************************************************
//  $FormatAplHCxVFC
//
//  Format a HCxV using []FC
//***************************************************************************

LPAPLCHAR FormatAplHCxVFC
    (LPWCHAR    lpaplChar,          // Ptr to output save area
     LPAPLHC8V  lpaplHC8V,          // Ptr to the value to format
     APLINT     nDigits,            // # significant digits (0 = all), or
                                    // # fractional digits (if bFractDigs)
                                    // If negative, use E-format
     APLCHAR    aplCharDecimal,     // Char to use as decimal separator
     APLCHAR    aplCharOverbar,     // Char to use as overbar
     FLTDISPFMT fltDispFmt,         // Float display format (see FLTDISPFMT)
     UBOOL      bFractDigs,         // TRUE iff nDigits is # fractional digits
     UBOOL      bSubstInf,          // TRUE iff we're to substitute text for infinity
     UBOOL      bPrecFPC,           // TRUE iff we're to precede the display with (FPCnnn)
     int        iHCDimVar)          // HC Dimension (1, 2, 4, 8)

{
    int i;

    // If we are displaying HC in infix form, ...
    if (OptionFlags.bDispInfix)
        // Format the real part
        lpaplChar =
          FormatAplVfpFC (lpaplChar,            // Ptr to output save area
                         &lpaplHC8V->parts[0],  // Ptr to the value to format
                          nDigits,              // # significant digits (0 = all), or
                                                // # fractional digits (if bFractDigs)
                                                // If negative, use E-format
                          0,                    // Maximum width including sign & decpt (0 = none)
                          aplCharDecimal,       // Char to use as decimal separator
                          aplCharOverbar,       // Char to use as overbar
                          fltDispFmt,           // Float display format (see FLTDISPFMT)
                          bFractDigs,           // TRUE iff nDigits is # fractional digits
                          bSubstInf,            // TRUE iff we're to substitute text for infinity
                          bPrecFPC);            // TRUE iff we're to precede the display with (FPCnnn)
    else
    // Otherwise, we're displaying in postfix form
    {
        // If the real part is non-zero, or the entire number is zero, ...
        if (!IsZeroHCxV (&lpaplHC8V->parts[0], 1)
         || IsZeroHCxV (lpaplHC8V, iHCDimVar))
        {
            // Format the real part
            lpaplChar =
              FormatAplVfpFC (lpaplChar,            // Ptr to output save area
                             &lpaplHC8V->parts[0],  // Ptr to the value to format
                              nDigits,              // # significant digits (0 = all), or
                                                    // # fractional digits (if bFractDigs)
                                                    // If negative, use E-format
                              0,                    // Maximum width including sign & decpt (0 = none)
                              aplCharDecimal,       // Char to use as decimal separator
                              aplCharOverbar,       // Char to use as overbar
                              fltDispFmt,           // Float display format (see FLTDISPFMT)
                              bFractDigs,           // TRUE iff nDigits is # fractional digits
                              bSubstInf,            // TRUE iff we're to substitute text for infinity
                              bPrecFPC);            // TRUE iff we're to precede the display with (FPCnnn)
            // If the imaginary part of the number is NOT all zero, ...
            if (OptionFlags.bDisp0Imag
             || IzitImaginary (uHCxV[aHCDimToIndex[iHCDimVar]], lpaplHC8V))
                // Append the real part separator
                //  overwriting the trailing blank
                lpaplChar[-1] = L's';
            else
                // Back up to the trailing blank so the next
                //   coefficient overwrites it
                lpaplChar--;
        } // End IF
    } // End IF/ELSE

    // Loop through the imaginary parts
    for (i = 1; i < iHCDimVar; i++)
    // If the next imaginary part is non-zero, ...
    if (OptionFlags.bDisp0Imag
     || !IsMpf0 (&lpaplHC8V->parts[i]))
    {
        // If we are displaying HC in infix form, ...
        if (OptionFlags.bDispInfix)
        {
            // Append the next imaginary separator
            //  overwriting the trailing blank
            strcpyW (&lpaplChar[-1], HC8SEP[i - 1]);

            // Skip over the separator
            lpaplChar += lstrlenW (HC8SEP[i - 1]) - 1;
        } // End IF

        // Format the next imag part
        lpaplChar =
          FormatAplVfpFC (lpaplChar,            // Ptr to output save area
                         &lpaplHC8V->parts[i],  // Ptr to the value to format
                          nDigits,              // # significant digits (0 = all), or
                                                // # fractional digits (if bFractDigs)
                                                // If negative, use E-format
                          0,                    // Maximum width including sign & decpt (0 = none)
                          aplCharDecimal,       // Char to use as decimal separator
                          aplCharOverbar,       // Char to use as overbar
                          fltDispFmt,           // Float display format (see FLTDISPFMT)
                          bFractDigs,           // TRUE iff nDigits is # fractional digits
                          bSubstInf,            // TRUE iff we're to substitute text for infinity
                          bPrecFPC);            // TRUE iff we're to precede the display with (FPCnnn)
        // If we are NOT displaying HC in infix form, ...
        if (!OptionFlags.bDispInfix)
        {
            // Append the next HCx separator
            //  overwriting the trailing blank
            strcpyW (&lpaplChar[-1], HC8SEP[i - 1]);

            // Skip over the separator
            lpaplChar += lstrlenW (HC8SEP[i - 1]) - 1;
        } // End IF
    } // End FOR/IF

    // If we are NOT displaying HC in infix form, ...
    if (!OptionFlags.bDispInfix)
        // Append a trailing blank
        *lpaplChar++ = L' ';

    return lpaplChar;
} // End FormatAplHCxVFC


//***************************************************************************
//  $FormatAplArbFC
//
//  Format ARB using []FC
//***************************************************************************

LPAPLCHAR FormatAplArbFC
    (LPWCHAR    lpaplChar,          // Ptr to output save area
     LPAPLARB   lpaplArb,           // Ptr to the value to format
     APLINT     nDigits,            // # significant digits (0 = all), or
                                    // # fractional digits (if bFractDigs)
                                    // If negative, use E-format
     APLINT     nWid,               // Maximum width including sign & decpt (0 = none)
     APLCHAR    aplCharDecimal,     // Char to use as decimal separator
     APLCHAR    aplCharOverbar,     // Char to use as overbar
     FLTDISPFMT fltDispFmt,         // Float display format (see FLTDISPFMT)
     UBOOL      bFractDigs,         // TRUE iff nDigits is # fractional digits
     UBOOL      bSubstInf,          // TRUE iff we're to substitute text for infinity
     UBOOL      bPrecFPC)           // TRUE iff we're to precede the display with (FPCnnn)

{
    APLVFP aplRad = {0};            // Radius

    Assert (!IsArbNULL (lpaplArb));

////// If we're to precede the display with (FPCnnn), ...
////if (bPrecFPC)
////{
////    // Append leading marker
////    *lpaplChar++ = L'(';
////
////    lpaplChar =
////      FormatAplIntFC (lpaplChar,            // Ptr to output save area
////                      arb_get_prec (lpaplArb),  // The value to format
////                      UTF16_OVERBAR);       // Char to use as overbar
////    // Append trailing marker overwriting the trailing space
////    lpaplChar[-1] = L')';
////} // End IF

    // Izit a NaN?
    if (IsArfNaN (arb_midref (lpaplArb)))
    {
        // If we're to substitute, ...
        if (bSubstInf)
            lpaplChar +=
              ConvertWideToNameLength (lpaplChar,       // Ptr to output save buffer
                                      &wcNaN,           // Ptr to incoming chars
                                       1);              // # chars to convert
        else
            *lpaplChar++ = wcNaN;
    } else
    // Izit an infinity?
    if (IsArfInf (arb_midref (lpaplArb)))
    {
        // Izit negative?
        if (arf_sgn (arb_midref (lpaplArb)) < 0)
            *lpaplChar++ = aplCharOverbar;

        // If we're to substitute, ...
        if (bSubstInf)
            lpaplChar +=
              ConvertWideToNameLength (lpaplChar,       // Ptr to output save buffer
                                      &wcInf,           // Ptr to incoming chars
                                       1);              // # chars to convert
        else
            *lpaplChar++ = wcInf;           // Char for infinity
    } else
    // Check for 0.0
    if (IsArb0 (lpaplArb))
    {
        // If it's negative, ...
        if (SIGN_APLARB (lpaplArb))
            *lpaplChar++ = aplCharOverbar;

        // Split cases based upon the float display format
        switch (fltDispFmt)
        {
            case FLTDISPFMT_E:          // 0E0 or 0.0E0 or 0.0---0E0
                if (nDigits EQ 1)
                {
                    // Fill in the result of "0E0"
                    *lpaplChar++ = L'0';
                    *lpaplChar++ = DEF_EXPONENT_UC;
                    *lpaplChar++ = L'0';
                } else
                if (nDigits > 1)
                {
                    // Start with "0."
                    *lpaplChar++ = L'0';
                    nDigits--;
                    *lpaplChar++ = aplCharDecimal;

                    // While there are more digits, ...
                    while (nDigits > 0)
                    {
                        // Fill with leading zeros
                        *lpaplChar++ = L'0';
                        nDigits--;
                    } // End WHILE

                    // End with "E0"
                    *lpaplChar++ = DEF_EXPONENT_UC;
                    *lpaplChar++ = L'0';
                } else
                    DbgStop ();         // We should never get here
                break;

            case FLTDISPFMT_F:
                // Start with a "0"
                *lpaplChar++ = L'0';

                // If there are any significant digits, ...
                if (nDigits > 0)
                {
                    // Append a decimal point
                    *lpaplChar++ = aplCharDecimal;

                    // While there are more significant digits, ...
                    while (nDigits > 0)
                    {
                        // Append another zero
                        *lpaplChar++ = L'0';
                        nDigits--;
                    } // End WHILE
                } // End IF

                break;

            case FLTDISPFMT_RAWINT:
            case FLTDISPFMT_RAWFLT:
                // The result is "0"
                *lpaplChar++ = L'0';

                break;

            defstop
                break;
        } // End SWITCH
    } else
    // Non-zero
    {
        APLVFP aplMid = {0};            // Midpoint
        mp_limb_signed_t prec = ARB_PREC_FPC;

        // Initialize to 0
        mpfr_init0 (&aplMid);
        mpfr_init0 (&aplRad);

        // Extract the Midpoint as a VFP number
        arf_get_mpfr2 (&aplMid, arb_midref (lpaplArb), prec);

        // Extract the Radius as a VFP number
        mag_get_mpfr  (&aplRad, arb_radref (lpaplArb), MPFR_RNDN);

        // If we're displaying # signifcant digits, ...
        if ((fltDispFmt EQ FLTDISPFMT_RAWINT
          || fltDispFmt EQ FLTDISPFMT_RAWFLT)
         && nDigits > 0
         && OptionFlags.bRadCtlMidPrec)
        {
            APLVFP aplTmp = {0};            // Temp
            double tmp;
            int    nDigs, nDigsF, nDigsI;

            // Initialize to NaN
            mpfr_init  (&aplTmp);

            // Calculate the base 10 log so as to count the maximum #
            //   significant digits in the Midpoint
            mpfr_log10    (&aplTmp, &aplRad, MPFR_RNDN);

            // Extract the nearest double precision value
            tmp = mpfr_get_d (&aplTmp, MPFR_RNDN);

            // # fractional digits
            nDigsF = -(int) (signumflt (tmp) * ceil (fabs (tmp)));

            // Get the absolute value
            mpfr_abs      (&aplTmp, &aplMid, MPFR_RNDN);

            // Calculate the base 10 log so as to count the maximum #
            //   significant digits in the Midpoint
            mpfr_log10    (&aplTmp, &aplTmp, MPFR_RNDN);

            // Extract the nearest double precision value
            tmp = mpfr_get_d (&aplTmp, MPFR_RNDN);

            // # integer digits
            nDigsI = (int) (signumflt (tmp) * ceil (fabs (tmp)));

            // Add in the # integer digits
            nDigs = nDigsI + nDigsF;

            // If it's not positive, ...
            if (nDigs <= 0)
                // No significant digits to speak of
                nDigs = 1;

            // Format the Midpoint to no more than this # significant digits
            nDigits = min (nDigs, nDigits);

            // We no longer need this storage
            mpfr_clear (&aplTmp);
        } // End IF

        // Format the Midpoint
        lpaplChar =
          FormatAplVfpFC (lpaplChar,        // Ptr to output save area
                         &aplMid,           // Ptr to the value to format
                          nDigits,          // # significant digits (0 = all), or
                                            // # fractional digits (if bFractDigs)
                                            // If negative, use E-format
                          nWid,             // Maximum width including sign & decpt (0 = none)
                          aplCharDecimal,   // Char to use as decimal separator
                          aplCharOverbar,   // Char to use as overbar
                          fltDispFmt,       // Float display format (see FLTDISPFMT)
                          bFractDigs,       // TRUE iff nDigits is # fractional digits
                          bSubstInf,        // TRUE iff we're to substitute text for infinity
                          FALSE);           // TRUE iff we're to precede the display with (FPCnnn)
        // We no longer need this storage
        mpfr_clear (&aplMid);

        // If the Radius is non-zero or we're displaying all zeroes, ...
        if (!IsMpf0 (&aplRad)
         || OptionFlags.bDisp0Imag)
        {
            // Include the ARB separator (overwriting the trailing blank)
            lpaplChar[-1] = DEF_ARBSEP_W;

            // Format the Radius
            lpaplChar =
              FormatAplVfpFC (lpaplChar,            // Ptr to output save area
                             &aplRad,               // Ptr to the value to format
                              arbRadiusDigs,        // # significant digits >= 0
                              0,                    // Maximum width including sign & decpt (0 = none)
                              aplCharDecimal,       // Char to use as decimal separator
                              aplCharOverbar,       // Char to use as overbar
                              FLTDISPFMT_RAWFLT,    // Float display format
                              FALSE,                // TRUE iff nDigits is # fractional digits
                              bSubstInf,            // TRUE iff we're to substitute text for infinity
                              FALSE);               // TRUE iff we're to precede the display with (FPCnnn)
            // If the formatted Radius ends with "E0 ", ...
            if (tolowerW (lpaplChar[-3]) EQ L'e'
             &&           lpaplChar[-2]  EQ L'0')
            {
                // Back up over the "E0 "
                lpaplChar -= 3;

                // Include a separator
                *lpaplChar++ = L' ';
            } // End IF

            goto NORMAL_EXIT;
        } else
        if (OptionFlags.bDispMPSuf)
            // Include the ARB suffix (overwriting the trailing blank)
            lpaplChar[-1] = DEF_ARBSEP_W;
        else
            goto NORMAL_EXIT;
    } // End IF/ELSE

    // Include a separator
    *lpaplChar++ = L' ';
NORMAL_EXIT:
    // We no longer need this storage
    // If we didn't use this var, <Myf_clear> will ignore it
    Myf_clear (&aplRad);

    return lpaplChar;
} // End FormatAplArbFC


//***************************************************************************
//  $FormatAplArb
//
//  Format an ARB number
//***************************************************************************

LPAPLCHAR FormatAplArb
    (LPWCHAR  lpaplChar,        // Ptr to output save area
     LPVOID   lpaplBA1F,        // Ptr to the value to format
     APLUINT  nDigits)          // # significant digits (0 = all)

{
    return
      FormatAplBAxFFC (lpaplChar,           // Ptr to output save area
                       lpaplBA1F,           // Ptr to the value to format
                       nDigits,             // # significant digits (0 = all)
                       UTF16_DOT,           // Char to use as decimal separator
                       UTF16_OVERBAR,       // Char to use as overbar
                       FLTDISPFMT_RAWFLT,   // Float display format
                       FALSE,               // TRUE iff nDigits is # fractional digits
                       FALSE,               // TRUE iff we're to substitute text for infinity
                       FALSE,               // TRUE iff we're to precede the display with (FPCnnn)
                       1);                  // HC Dimension (1, 2, 4, 8)
} // End FormatAplArb


//***************************************************************************
//  $FormatAplBA2F
//
//  Format a BA2F number
//***************************************************************************

LPAPLCHAR FormatAplBA2F
    (LPWCHAR  lpaplChar,        // Ptr to output save area
     LPVOID   lpaplBA2F,        // Ptr to the value to format
     APLUINT  nDigits)          // # significant digits (0 = all)

{
    return
      FormatAplBAxFFC (lpaplChar,           // Ptr to output save area
                       lpaplBA2F,           // Ptr to the value to format
                       nDigits,             // # significant digits (0 = all)
                       UTF16_DOT,           // Char to use as decimal separator
                       UTF16_OVERBAR,       // Char to use as overbar
                       FLTDISPFMT_RAWFLT,   // Float display format
                       FALSE,               // TRUE iff nDigits is # fractional digits
                       FALSE,               // TRUE iff we're to substitute text for infinity
                       FALSE,               // TRUE iff we're to precede the display with (FPCnnn)
                       2);                  // HC Dimension (1, 2, 4, 8)
} // End FormatAplBA2F


//***************************************************************************
//  $FormatAplBA4F
//
//  Format a BA4F number
//***************************************************************************

LPAPLCHAR FormatAplBA4F
    (LPWCHAR  lpaplChar,        // Ptr to output save area
     LPVOID   lpaplBA4F,        // Ptr to the value to format
     APLUINT  nDigits)          // # significant digits (0 = all)

{
    return
      FormatAplBAxFFC (lpaplChar,           // Ptr to output save area
                       lpaplBA4F,           // Ptr to the value to format
                       nDigits,             // # significant digits (0 = all)
                       UTF16_DOT,           // Char to use as decimal separator
                       UTF16_OVERBAR,       // Char to use as overbar
                       FLTDISPFMT_RAWFLT,   // Float display format
                       FALSE,               // TRUE iff nDigits is # fractional digits
                       FALSE,               // TRUE iff we're to substitute text for infinity
                       FALSE,               // TRUE iff we're to precede the display with (FPCnnn)
                       4);                  // HC Dimension (1, 2, 4, 8)
} // End FormatAplBA4F


//***************************************************************************
//  $FormatAplBA8F
//
//  Format a BA8F number
//***************************************************************************

LPAPLCHAR FormatAplBA8F
    (LPWCHAR  lpaplChar,        // Ptr to output save area
     LPVOID   lpaplBA8F,        // Ptr to the value to format
     APLUINT  nDigits)          // # significant digits (0 = all)

{
    return
      FormatAplBAxFFC (lpaplChar,           // Ptr to output save area
                       lpaplBA8F,           // Ptr to the value to format
                       nDigits,             // # significant digits (0 = all)
                       UTF16_DOT,           // Char to use as decimal separator
                       UTF16_OVERBAR,       // Char to use as overbar
                       FLTDISPFMT_RAWFLT,   // Float display format
                       FALSE,               // TRUE iff nDigits is # fractional digits
                       FALSE,               // TRUE iff we're to substitute text for infinity
                       FALSE,               // TRUE iff we're to precede the display with (FPCnnn)
                       8);                  // HC Dimension (1, 2, 4, 8)
} // End FormatAplBA8F


//***************************************************************************
//  $FormatAplBAxFFC
//
//  Format a BAxF using []FC
//***************************************************************************

LPAPLCHAR FormatAplBAxFFC
    (LPWCHAR    lpaplChar,          // Ptr to output save area
     LPAPLBA8F  lpaplBA8F,          // Ptr to the value to format
     APLINT     nDigits,            // # significant digits (0 = all), or
                                    // # fractional digits (if bFractDigs)
                                    // If negative, use E-format
     APLCHAR    aplCharDecimal,     // Char to use as decimal separator
     APLCHAR    aplCharOverbar,     // Char to use as overbar
     FLTDISPFMT fltDispFmt,         // Float display format (see FLTDISPFMT)
     UBOOL      bFractDigs,         // TRUE iff nDigits is # fractional digits
     UBOOL      bSubstInf,          // TRUE iff we're to substitute text for infinity
     UBOOL      bPrecFPC,           // TRUE iff we're to precede the display with (FPCnnn)
     int        iHCDimVar)          // HC Dimension (1, 2, 4, 8)

{
    int i;

    // If we are displaying HC in infix form, ...
    if (OptionFlags.bDispInfix)
        // Format the real part
        lpaplChar =
          FormatAplArbFC (lpaplChar,            // Ptr to output save area
                         &lpaplBA8F->parts[0],  // Ptr to the value to format
                          nDigits,              // # significant digits (0 = all), or
                                                // # fractional digits (if bFractDigs)
                                                // If negative, use E-format
                          0,                    // Maximum width including sign & decpt (0 = none)
                          aplCharDecimal,       // Char to use as decimal separator
                          aplCharOverbar,       // Char to use as overbar
                          fltDispFmt,           // Float display format (see FLTDISPFMT)
                          bFractDigs,           // TRUE iff nDigits is # fractional digits
                          bSubstInf,            // TRUE iff we're to substitute text for infinity
                          bPrecFPC);            // TRUE iff we're to precede the display with (FPCnnn)
    else
    // Otherwise, we're displaying in postfix form
    {
        // If the real part is non-zero, or the entire number is zero, ...
        if (!IsZeroBAxF (&lpaplBA8F->parts[0], 1)
         || IsZeroBAxF (lpaplBA8F, iHCDimVar))
        {
            // Format the real part
            lpaplChar =
              FormatAplArbFC (lpaplChar,            // Ptr to output save area
                             &lpaplBA8F->parts[0],  // Ptr to the value to format
                              nDigits,              // # significant digits (0 = all), or
                                                    // # fractional digits (if bFractDigs)
                                                    // If negative, use E-format
                              0,                    // Maximum width including sign & decpt (0 = none)
                              aplCharDecimal,       // Char to use as decimal separator
                              aplCharOverbar,       // Char to use as overbar
                              fltDispFmt,           // Float display format (see FLTDISPFMT)
                              bFractDigs,           // TRUE iff nDigits is # fractional digits
                              bSubstInf,            // TRUE iff we're to substitute text for infinity
                              bPrecFPC);            // TRUE iff we're to precede the display with (FPCnnn)
            // If the imaginary part of the number is NOT all zero, ...
            if (OptionFlags.bDisp0Imag
             || IzitImaginary (uHCxV[aHCDimToIndex[iHCDimVar]], lpaplBA8F))
                // Append the real part separator
                //  overwriting the trailing blank
                lpaplChar[-1] = L's';
            else
                // Back up to the trailing blank so the next
                //   coefficient overwrites it
                lpaplChar--;
        } // End IF
    } // End IF/ELSE

    // Loop through the imaginary parts
    for (i = 1; i < iHCDimVar; i++)
    // If the next imaginary part is non-zero, ...
    if (OptionFlags.bDisp0Imag
     || !IsArb0 (&lpaplBA8F->parts[i]))
    {
        // If we are displaying HC in infix form, ...
        if (OptionFlags.bDispInfix)
        {
            // Append the next imaginary separator
            //  overwriting the trailing blank
            strcpyW (&lpaplChar[-1], HC8SEP[i - 1]);

            // Skip over the separator
            lpaplChar += lstrlenW (HC8SEP[i - 1]) - 1;
        } // End IF

        // Format the next imag part
        lpaplChar =
          FormatAplArbFC (lpaplChar,            // Ptr to output save area
                         &lpaplBA8F->parts[i],  // Ptr to the value to format
                          nDigits,              // # significant digits (0 = all), or
                                                // # fractional digits (if bFractDigs)
                                                // If negative, use E-format
                          0,                    // Maximum width including sign & decpt (0 = none)
                          aplCharDecimal,       // Char to use as decimal separator
                          aplCharOverbar,       // Char to use as overbar
                          fltDispFmt,           // Float display format (see FLTDISPFMT)
                          bFractDigs,           // TRUE iff nDigits is # fractional digits
                          bSubstInf,            // TRUE iff we're to substitute text for infinity
                          bPrecFPC);            // TRUE iff we're to precede the display with (FPCnnn)
        // If we are NOT displaying HC in infix form, ...
        if (!OptionFlags.bDispInfix)
        {
            // Append the next HCx separator
            //  overwriting the trailing blank
            strcpyW (&lpaplChar[-1], HC8SEP[i - 1]);

            // Skip over the separator
            lpaplChar += lstrlenW (HC8SEP[i - 1]) - 1;
        } // End IF
    } // End FOR/IF

    // If we are NOT displaying HC in infix form, ...
    if (!OptionFlags.bDispInfix)
        // Append a trailing blank
        *lpaplChar++ = L' ';

    return lpaplChar;
} // End FormatAplBAxFFC


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

    return
      FormatImmed (lpaplChar,
                   lpSymEntry->stFlags.ImmType,
                  &lpSymEntry->stData.stLongest);
} // End FormatSymTabConst


//***************************************************************************
//  $DisplayTransferVar2
//
//  Create the Type 2 transfer form of a variable
//***************************************************************************

LPWCHAR DisplayTransferVar2
    (LPWCHAR    lpwszTemp,                  // Ptr to output save area
     LPSYMENTRY lpSymEntry)                 // Ptr to SYMENTRY of the var to display

{
    LPWCHAR lpwszOrig = lpwszTemp;          // Save the original ptr

    // Copy the var's name
    lpwszTemp =
      CopySteName (lpwszTemp,               // Ptr to result global memory
                   lpSymEntry,              // Ptr to function symbol table entry
                   NULL);                   // Ptr to name length (may be NULL)
    // If it's a System Var, ...
    if (lpSymEntry->stFlags.ObjName EQ OBJNAME_SYS)
        // Convert to upper case as some .atf interpreters don't handle lowercase well
        CharUpperBuffW (lpwszOrig, (DWORD) (lpwszTemp - lpwszOrig));

    // Append a left arrow
    *lpwszTemp++ = UTF16_LEFTARROW;

    // If the var is an immediate, ...
    if (lpSymEntry->stFlags.Imm)
        // Display the immediate value
        lpwszTemp =
          DisplayTransferImm2 (lpwszTemp,   // Ptr to output save area
                               lpSymEntry); // Ptr to SYMENTRY of the immediate to display
    else
        // Display the global memory value
        lpwszTemp =
          DisplayTransferGlb2 (lpwszTemp,                       // Ptr to output save area
                               lpSymEntry->stData.stGlbData,    // Arg global memory handle
                               0,                               // Outer NELM (if not at top level)
                               0,                               // Outer rank (if not at top level)
                               TRUE);                           // TRUE iff this call is at the top level
    // Delete the last blank in case it matters,
    //   and ensure properly terminated
    if (lpwszTemp[-1] EQ L' ')
        *--lpwszTemp = WC_EOS;
    else
        *lpwszTemp = WC_EOS;

    return lpwszTemp;
} // End DisplayTransferVar2


//***************************************************************************
//  $DisplayTransferChr2
//
//  Create the Type 2 transfer form of a character scalar or vector
//***************************************************************************

LPWCHAR DisplayTransferChr2
    (LPWCHAR    lpwszTemp,                  // Ptr to output save area
     LPAPLCHAR  lpMemArg,                   // Ptr to character scalar/vector
     APLNELM    aplNELMArg,                 // Arg NELM
     APLRANK    aplRankArg)                 // Arg rank

{
    APLUINT uCnt;                           // Loop counter

    // Start with a leading quote
    *lpwszTemp++ = WC_SQ;

    // Loop through the elements
    for (uCnt = 0; uCnt < aplNELMArg; uCnt++, ((LPAPLCHAR) lpMemArg)++)
    // If the data is a single quote, ...
    if ((*(LPAPLCHAR) lpMemArg) EQ WC_SQ)
    {
        // The data is ''
        *lpwszTemp++ = WC_SQ;
        *lpwszTemp++ = WC_SQ;
    } else
        // ***FIXME*** -- Use []UCS to handle chars outside APL2's charset
        *lpwszTemp++ = *(LPAPLCHAR) lpMemArg;

    // End with a trailing quote
    *lpwszTemp++ = WC_SQ;

    // and a trailing blank
    *lpwszTemp++ = L' ';

    return lpwszTemp;
} // End DisplayTransferChr2


//***************************************************************************
//  $DisplayTransferImm2
//
//  Create the Type 2 transfer form of an immediate
//***************************************************************************

LPWCHAR DisplayTransferImm2
    (LPWCHAR    lpwszTemp,                  // Ptr to output save area
     LPSYMENTRY lpSymEntry)                 // Ptr to SYMENTRY of the immediate to display

{
    // If the immediate is a char, ...
    if (IsImmChr (lpSymEntry->stFlags.ImmType))
        // Display the char immediate
        lpwszTemp =
          DisplayTransferChr2 (lpwszTemp,   // Ptr to output save area
                              &lpSymEntry->stData.stChar,   // Ptr to character scalar/vector
                               1,           // Arg NELM
                               0);          // Arg rank
    else
        lpwszTemp =
          FormatImmedFC (lpwszTemp,                         // Ptr to input string
                         lpSymEntry->stFlags.ImmType,       // Immediate type
                        &lpSymEntry->stData.stLongest,      // Ptr to value to format
                         DEF_MAX_QUADPP_IEEE,               // # significant digits
                         UTF16_DOT,                         // Char to use as decimal separator
                         NegSign,                           // Char to use as overbar
                         FLTDISPFMT_RAWFLT,                 // Float display format
                         FALSE);                            // TRUE iff we're to substitute text for infinity
    return lpwszTemp;
} // End DisplayTransferImm2


//***************************************************************************
//  $DisplayTransferGlb2
//
//  Create the Type 2 transfer form of a global
//***************************************************************************

LPWCHAR DisplayTransferGlb2
    (LPWCHAR    lpwszTemp,                  // Ptr to output save area
     HGLOBAL    hGlbArg,                    // Arg global memory handle
     APLNELM    aplNELMOut,                 // Outer NELM (if not at top level)
     APLRANK    aplRankOut,                 // Outer rank (if not at top level)
     UBOOL      bTopLevel)                  // TRUE iff this call is at the top level

{
    LPVARARRAY_HEADER lpMemHdrArg = NULL;       // Ptr to arg item global memory header
    LPVOID            lpMemArg;                 // Ptr to arg item global memory
    APLSTYPE          aplTypeArg;               // Arg item storage type
    APLNELM           aplNELMArg,               // Arg item NELM
                      aplNELMNst;               // Arg item NELM if nested
    APLRANK           aplRankArg;               // Arg item rank
    APLUINT           uCnt;                     // Loop counter
    UBOOL             bNeedParens = FALSE,      // TRUE iff this level needs surrounding parens
                      bDispMPSuf,               // Save area for OptionFlags value
                      bDisp0Imag,               // ...
                      bDispInfix,               // ...
                      bDispOctoDig;             // ...
    APLCHAR           lpwCopyFC[FCNDX_LENGTH];  // ...

    // Save OptionFlags for display to fixed
    //   values so we convert values on )LOAD,
    //   )SAVE, )COPY, )OUT, and []TF consistently.
    SetOptionFlagsDisplay (lpwCopyFC, &bDisp0Imag, &bDispInfix, &bDispOctoDig, &bDispMPSuf);

    // Lock the memory to get a ptr to it
    lpMemHdrArg = MyGlobalLockVar (hGlbArg);

    // Skip over the header to the dimensions
    lpMemArg = VarArrayBaseToDim (lpMemHdrArg);

#define lpHeader        lpMemHdrArg
    // Get the Array Type, NELM, and Rank
    aplTypeArg = lpHeader->ArrType;
    aplNELMArg = lpHeader->NELM;
    aplRankArg = lpHeader->Rank;
#undef  lpHeader

    // If not at the top level, ...
    if (!bTopLevel)
    {
        // ***FIXME*** -- If the char vector has a non-APL2 char, use []UCS
        // If the inner item is not a non-singleton char vector,
        //   and the outer item is not a scalar,
        //   and the outer item is not empty, we need parens
        bNeedParens = (!(IsSimpleChar (aplTypeArg) && IsVector (aplRankArg) && !IsSingleton (aplNELMArg))
                    && !IsScalar (aplRankOut)
                    && !IsEmpty (aplNELMOut));
        if (bNeedParens)
            *lpwszTemp++ = L'(';

        // If the outer shell is a scalar or empty, append an {enclose} symbol
        if (IsScalar (aplRankOut)
         || IsEmpty (aplNELMOut))
            *lpwszTemp++ = UTF16_LEFTSHOE;
    } // End IF

    // If the array is a matrix
    //   or a non-scalar singleton,
    //   or an empty nested array, ...
    if (IsMatrix (aplRankArg)
     || (IsSingleton (aplNELMArg) && !IsScalar (aplRankArg))
     || (IsEmpty (aplNELMArg) && IsNested (aplTypeArg)))
    {
        // Format & save the shape
        for (uCnt = 0; uCnt < aplRankArg; uCnt++)
            lpwszTemp =
              FormatAplIntFC (lpwszTemp,            // Ptr to output save area
                 *((LPAPLDIM) lpMemArg)++,          // The value to format
                              NegSign);             // Char to use as overbar
        // lpMemArg now points to the data

        // If the array is not a scalar, ...
        if (!IsScalar (aplRankArg))
            // Append a {rho}, zapping the trailing blank
            lpwszTemp[-1] = UTF16_RHO;
    } else
        // Skip over the dimension(s)
        ((LPAPLDIM) lpMemArg) += aplRankArg;

    //***************************************************************************
    // lpMemArg now points to the data
    //***************************************************************************

    // Split cases based upon the arg storage type
    switch (aplTypeArg)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
            // Loop through the elements
            for (uCnt = 0; uCnt < aplNELMArg; uCnt++)
                lpwszTemp =
                  FormatAplIntFC (lpwszTemp,            // Ptr to output save area
                                  GetNextInteger (lpMemArg, aplTypeArg, uCnt),  // The value to format
                                  NegSign);             // Char to use as overbar
            break;

        case ARRAY_APA:
            // APAs are displayed as Off-Mulx[]IO-{iota}Len
            //   unless it's {zilde} in which case we use 0{rho}0

#define lpAPA           ((LPAPLAPA) lpMemArg)
            // If it's {zilde}, ...
            if (IsEmpty (aplNELMArg))
            {
                *lpwszTemp++ = L'0';
                *lpwszTemp++ = UTF16_RHO;
                *lpwszTemp++ = L'0';
            } else
            {
                // Append the offset
                lpwszTemp =
                  FormatAplIntFC (lpwszTemp,            // Ptr to output save area
                                  lpAPA->Off,           // The value to format
                                  NegSign);             // Char to use as overbar
                // Append a minus sign, zapping the trailing blank
                lpwszTemp[-1] = L'-';

                // Append the multiplier
                lpwszTemp =
                  FormatAplIntFC (lpwszTemp,            // Ptr to output save area
                                  lpAPA->Mul,           // The value to format
                                  NegSign);             // Char to use as overbar
#undef  lpAPA
                // Append {times}[]IO-{iota}, zapping the trailing blank
                strcpyW (&lpwszTemp[-1], WS_UTF16_TIMES $QUAD_IO L"-" WS_UTF16_IOTA);
                lpwszTemp = &lpwszTemp[lstrlenW (lpwszTemp)];

                // Format the length
                lpwszTemp =
                  FormatAplIntFC (lpwszTemp,            // Ptr to output save area
                                  aplNELMArg,           // The value to format
                                  NegSign);             // Char to use as overbar
            } // End IF/ELSE

            break;

        case ARRAY_FLOAT:
            // Loop through the elements
            for (uCnt = 0; uCnt < aplNELMArg; uCnt++)
                lpwszTemp =
                  FormatAplFltFC (lpwszTemp,            // Ptr to output save area
                   *((LPAPLFLOAT) lpMemArg)++,          // Ptr to float value
                                  DEF_MAX_QUADPP_IEEE,  // # significant digits
                                  DecSep,               // Char to use as decimal separator
                                  NegSign,              // Char to use as overbar
                                  FLTDISPFMT_RAWFLT,    // Float display format
                                  FALSE);               // TRUE iff we're to substitute text for infinity
            break;

        case ARRAY_CHAR:
            // Display the char vector
            lpwszTemp =
              DisplayTransferChr2 (lpwszTemp,           // Ptr to output save area
                                   lpMemArg,            // Ptr to character scalar/vector
                                   aplNELMArg,          // Arg NELM
                                   1);                  // Arg rank
            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // If it's an empty nested array, ...
            if (IsEmpty (aplNELMArg) && IsNested (aplTypeArg))
                aplNELMNst = 1;
            else
                aplNELMNst = aplNELMArg;

            // Loop through the elements
            for (uCnt = 0; uCnt < aplNELMNst; uCnt++, ((LPAPLHETERO) lpMemArg)++)
            {
                // Split cases based upon the ptr type
                switch (GetPtrTypeInd (lpMemArg))
                {
                    case PTRTYPE_STCONST:
                        // Display the immediate value
                        lpwszTemp =
                          DisplayTransferImm2 (lpwszTemp,                   // Ptr to output save area
                                               *(LPAPLHETERO) lpMemArg);    // Ptr to SYMENTRY of the immediate to display
                        break;

                    case PTRTYPE_HGLOBAL:
                        // Display the global memory value
                        lpwszTemp =
                          DisplayTransferGlb2 (lpwszTemp,               // Ptr to output save area
                                               *(LPAPLHETERO) lpMemArg, // Arg global memory handle
                                               aplNELMArg,              // Outer NELM (if not at top level)
                                               aplRankArg,              // Outer rank (if not at top level)
                                               FALSE);                  // TRUE iff this call is at the top level
                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            break;

        case ARRAY_HC2I:
            // Loop through the elements
            for (uCnt = 0; uCnt < aplNELMArg; uCnt++)
                lpwszTemp =
                  FormatAplHC2IFC (lpwszTemp,           // Ptr to output save area
                      ((LPAPLHC2I) lpMemArg)++,         // Ptr to float value
                                   NegSign,             // Char to use as overbar
                                   GetHC2Sep ());       // Ptr to char(s) to use as separator
            break;

        case ARRAY_HC4I:
            // Loop through the elements
            for (uCnt = 0; uCnt < aplNELMArg; uCnt++)
                lpwszTemp =
                  FormatAplHCxIFC (lpwszTemp,           // Ptr to output save area
          (LPAPLHC8I) ((LPAPLHC4I) lpMemArg)++,         // Ptr to float value
                                   NegSign,             // Char to use as overbar
                                   4);                  // HC Dimension (1, 2, 4, 8)
            break;

        case ARRAY_HC8I:
            // Loop through the elements
            for (uCnt = 0; uCnt < aplNELMArg; uCnt++)
                lpwszTemp =
                  FormatAplHCxIFC (lpwszTemp,           // Ptr to output save area
                      ((LPAPLHC8I) lpMemArg)++,         // Ptr to float value
                                   NegSign,             // Char to use as overbar
                                   8);                  // HC Dimension (1, 2, 4, 8)
            break;

        case ARRAY_HC2F:
            // Loop through the elements
            for (uCnt = 0; uCnt < aplNELMArg; uCnt++)
                lpwszTemp =
                  FormatAplHC2F (lpwszTemp,             // Ptr to output save area
                    ((LPAPLHC2F) lpMemArg)++,           // Ptr to float value
                                   DEF_MAX_QUADPP_IEEE);// Precision to use
            break;

        case ARRAY_HC4F:
            // Loop through the elements
            for (uCnt = 0; uCnt < aplNELMArg; uCnt++)
                lpwszTemp =
                  FormatAplHC4F (lpwszTemp,             // Ptr to output save area
                    ((LPAPLHC4F) lpMemArg)++,           // Ptr to float value
                                 DEF_MAX_QUADPP_IEEE);  // Precision to use
            break;

        case ARRAY_HC8F:
            // Loop through the elements
            for (uCnt = 0; uCnt < aplNELMArg; uCnt++)
                lpwszTemp =
                  FormatAplHC8F (lpwszTemp,             // Ptr to output save area
                    ((LPAPLHC8F) lpMemArg)++,           // Ptr to float value
                                 DEF_MAX_QUADPP_IEEE);  // Precision to use
            break;

        case ARRAY_RAT :
            // Loop through the elements
            for (uCnt = 0; uCnt < aplNELMArg; uCnt++)
                lpwszTemp =
                  FormatAplRat (lpwszTemp,              // Ptr to output save area
                    ((LPAPLRAT) lpMemArg)++);           // Ptr to float value
            break;

        case ARRAY_HC2R:
            // Loop through the elements
            for (uCnt = 0; uCnt < aplNELMArg; uCnt++)
                lpwszTemp =
                  FormatAplHC2R (lpwszTemp,             // Ptr to output save area
                    ((LPAPLHC2R) lpMemArg)++);          // Ptr to float value
            break;

        case ARRAY_HC4R:
            // Loop through the elements
            for (uCnt = 0; uCnt < aplNELMArg; uCnt++)
                lpwszTemp =
                  FormatAplHC4R (lpwszTemp,             // Ptr to output save area
                    ((LPAPLHC4R) lpMemArg)++);          // Ptr to float value
            break;

        case ARRAY_HC8R:
            // Loop through the elements
            for (uCnt = 0; uCnt < aplNELMArg; uCnt++)
                lpwszTemp =
                  FormatAplHC8R (lpwszTemp,             // Ptr to output save area
                    ((LPAPLHC8R) lpMemArg)++);          // Ptr to float value
            break;

        case ARRAY_VFP :
            // Loop through the elements
            for (uCnt = 0; uCnt < aplNELMArg; uCnt++)
                lpwszTemp =
                  FormatAplVfp (lpwszTemp,              // Ptr to output save area
                    ((LPAPLVFP) lpMemArg)++,            // Ptr to float value
                                0);                     // # significant digits (0 = all)
            break;

        case ARRAY_HC2V:
            // Loop through the elements
            for (uCnt = 0; uCnt < aplNELMArg; uCnt++)
                lpwszTemp =
                  FormatAplHC2V (lpwszTemp,             // Ptr to output save area
                    ((LPAPLHC2V) lpMemArg)++,           // Ptr to float value
                                 0);                    // # significant digits (0 = all)
            break;

        case ARRAY_HC4V:
            // Loop through the elements
            for (uCnt = 0; uCnt < aplNELMArg; uCnt++)
                lpwszTemp =
                  FormatAplHC4V (lpwszTemp,             // Ptr to output save area
                    ((LPAPLHC4V) lpMemArg)++,           // Ptr to float value
                                 0);                    // # significant digits (0 = all)
            break;

        case ARRAY_HC8V:
            // Loop through the elements
            for (uCnt = 0; uCnt < aplNELMArg; uCnt++)
                lpwszTemp =
                  FormatAplHC8V (lpwszTemp,             // Ptr to output save area
                    ((LPAPLHC8V) lpMemArg)++,           // Ptr to float value
                                 0);                    // # significant digits (0 = all)
            break;

        case ARRAY_ARB :
            // Loop through the elements
            for (uCnt = 0; uCnt < aplNELMArg; uCnt++)
                lpwszTemp =
                  FormatAplArb (lpwszTemp,              // Ptr to output save area
                    ((LPAPLARB) lpMemArg)++,            // Ptr to float value
                                0);                     // # significant digits (0 = all)
            break;

        case ARRAY_BA2F:
            // Loop through the elements
            for (uCnt = 0; uCnt < aplNELMArg; uCnt++)
                lpwszTemp =
                  FormatAplBA2F (lpwszTemp,             // Ptr to output save area
                    ((LPAPLBA2F) lpMemArg)++,           // Ptr to float value
                                 0);                    // # significant digits (0 = all)
            break;

        case ARRAY_BA4F:
            // Loop through the elements
            for (uCnt = 0; uCnt < aplNELMArg; uCnt++)
                lpwszTemp =
                  FormatAplBA4F (lpwszTemp,             // Ptr to output save area
                    ((LPAPLBA4F) lpMemArg)++,           // Ptr to float value
                                 0);                    // # significant digits (0 = all)
            break;

        case ARRAY_BA8F:
            // Loop through the elements
            for (uCnt = 0; uCnt < aplNELMArg; uCnt++)
                lpwszTemp =
                  FormatAplBA8F (lpwszTemp,             // Ptr to output save area
                    ((LPAPLBA8F) lpMemArg)++,           // Ptr to float value
                                 0);                    // # significant digits (0 = all)
            break;

        defstop
            break;
    } // End SWITCH

    // Delete the last blank in case it matters,
    //   and ensure properly terminated
    if (lpwszTemp[-1] EQ L' '
     && (bNeedParens || lpwszTemp[-2] NE WC_SQ))
        *--lpwszTemp = WC_EOS;
    else
        *lpwszTemp = WC_EOS;

    // If we need parens, ...
    if (bNeedParens)
        *lpwszTemp++ = L')';

    // We no longer need this ptr
    MyGlobalUnlock (hGlbArg); lpMemHdrArg = NULL;

    // Restore the OptionFlags values
    RestoreOptionFlagsDisplay (lpwCopyFC, bDisp0Imag, bDispInfix, bDispOctoDig, bDispMPSuf);

    return lpwszTemp;
} // End DisplayTansferGlb2


//***************************************************************************
//  $DisplayTransferFcn2
//
//  Create the Type 2 transfer form of a function
//***************************************************************************

LPWCHAR DisplayTransferFcn2
    (LPWCHAR    lpwszTemp,                  // Ptr to output save area
     LPSYMENTRY lpSymEntry,                 // Ptr to SYMENTRY of the var to display
     FILETIME  *lpftCreation)               // Ptr to output save area for creation time stamp (may be NULL)

{
    HGLOBAL           hGlbDfnHdr = NULL;        // User-defined function operator global memory handle
    LPDFN_HEADER      lpMemDfnHdr = NULL;       // Ptr to user-defined function/operator global memory
    LPMEMTXT_UNION    lpMemTxtLine;             // Ptr to header/line text global memory
    HGLOBAL           hGlbTxtLine;              // Line text global memory handle
    LPFCNLINE         lpFcnLines;               // Ptr to array of function line structs (FCNLINE[numFcnLines])
    UINT              uNumLines,                // # function lines
                      uLine;                    // Loop counter

    // If the function is user-defined, ...
    if (lpSymEntry->stFlags.UsrDfn)
    {
        // Append "[]FX "
        strcpyW (lpwszTemp, $QUAD_FX L" ");
        lpwszTemp = &lpwszTemp[lstrlenW (lpwszTemp)];

        // Get the user-defined function/operator global memory handle
        hGlbDfnHdr = lpSymEntry->stData.stGlbData;

        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLockDfn (hGlbDfnHdr);

        // Save creation time if requested
        if (lpftCreation NE NULL)
            *lpftCreation = lpMemDfnHdr->ftCreation;

        // Lock the memory to get a ptr to it
        lpMemTxtLine = MyGlobalLockTxt (lpMemDfnHdr->hGlbTxtHdr);

        // Display the function header
        lpwszTemp =
          DisplayTransferChr2 (lpwszTemp,
                              &lpMemTxtLine->C,
                               lpMemTxtLine->U,
                               1);
        // We no longer need this ptr
        MyGlobalUnlock (lpMemDfnHdr->hGlbTxtHdr); lpMemTxtLine = NULL;

        // Get ptr to array of function line structs (FCNLINE[numFcnLines])
        lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

        // Get # function lines
        uNumLines = lpMemDfnHdr->numFcnLines;

        // Run through the function lines looking for the longest
        for (uLine = 0; uLine < uNumLines; uLine++)
        {
            // Get the line text global memory handle
            hGlbTxtLine = lpFcnLines->hGlbTxtLine;

            if (hGlbTxtLine NE NULL)
            {
                // Lock the memory to get a ptr to it
                lpMemTxtLine = MyGlobalLockTxt (hGlbTxtLine);

                // Display the function line
                lpwszTemp =
                  DisplayTransferChr2 (lpwszTemp,
                                      &lpMemTxtLine->C,
                                       lpMemTxtLine->U,
                                       1);
                // We no longer need this ptr
                MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;
            } // End IF

            // Skip to the next struct
            lpFcnLines++;
        } // End FOR

        // We no longer need this ptr
        MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
    } else
    {
        // Copy the var's name
        lpwszTemp =
          CopySteName (lpwszTemp,               // Ptr to result global memory
                       lpSymEntry,              // Ptr to function symbol table entry
                       NULL);                   // Ptr to name length (may be NULL)
        // Append a left arrow
        *lpwszTemp++ = UTF16_LEFTARROW;

        // If the function is immediate, ...
        if (lpSymEntry->stFlags.Imm)
            // Append the function/operator
            *lpwszTemp++ = lpSymEntry->stData.stChar;
        else
            // Append the function strand
            lpwszTemp =
              DisplayFcnGlb (lpwszTemp,
                             lpSymEntry->stData.stGlbData,
                             FALSE,
                             NULL,
                             NULL,
                             NULL,
                             NULL);
    } // End IF/ELSE

    // Delete the last blank in case it matters,
    //   and ensure properly terminated
    if (lpwszTemp[-1] EQ L' ')
        *--lpwszTemp = WC_EOS;
    else
        *lpwszTemp = WC_EOS;

    return lpwszTemp;
} // End DisplayTransferFcn2


//***************************************************************************
//  $GetQuadFCValue
//
//  Return a specified value from []FC
//***************************************************************************

APLCHAR GetQuadFCValue
    (FC_INDICES uIndex)                         // The index into []FC

{
    LPPERTABDATA      lpMemPTD;                 // Ptr to PerTabData global memory
    HGLOBAL           hGlbQuadFC;               // []FC global memory handle
    LPVARARRAY_HEADER lpMemHdrQuadFC = NULL;    // Ptr to []FC global memory header
    LPAPLCHAR         lpMemQuadFC;              // Ptr to []FC global memory
    APLNELM           aplNELMQuadFC;            // []FC NELM
    APLRANK           aplRankQuadFC;            // []FC rank
    APLCHAR           aplCharQuadFC;            // []FC[uIndex]

    Assert (uIndex < FCNDX_LENGTH);

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the []FC global memory handle
    hGlbQuadFC = lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_FC]->stData.stGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrQuadFC = MyGlobalLockVar (hGlbQuadFC);

#define lpHeader        lpMemHdrQuadFC
    // Get the array NELM and rank
    aplNELMQuadFC = lpHeader->NELM;
    aplRankQuadFC = lpHeader->Rank;
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMemQuadFC = VarArrayDataFmBase (lpMemHdrQuadFC);

    // Check for short []FC
    if (uIndex >= aplNELMQuadFC)
        aplCharQuadFC = aplDefaultFC[uIndex];
    else
        aplCharQuadFC = lpMemQuadFC[uIndex];

    // We no longer need this ptr
    MyGlobalUnlock (hGlbQuadFC); lpMemHdrQuadFC = NULL;

    return aplCharQuadFC;
} // End GetQuadFCValue


//***************************************************************************
//  $SetQuadFCValue
//
//  Set a specified value into []FC
//***************************************************************************

UBOOL SetQuadFCValue
    (FC_INDICES uIndex,                         // The index into []FC
     APLCHAR    aplCharQuadFC)                  // The new value for []FC[uIndex]

{
    LPPERTABDATA      lpMemPTD;                 // Ptr to PerTabData global memory
    HGLOBAL           hGlbQuadFC;               // []FC global memory handle
    LPVARARRAY_HEADER lpMemHdrQuadFC = NULL;    // Ptr to []FC global memory header
    LPAPLCHAR         lpMemQuadFC;              // Ptr to []FC global memory
    APLNELM           aplNELMQuadFC;            // []FC NELM
    APLRANK           aplRankQuadFC;            // []FC rank
    UBOOL             bRet = TRUE;              // TRUE iff the result is valid

    Assert (uIndex < FCNDX_LENGTH);

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the []FC global memory handle
    hGlbQuadFC = lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_FC]->stData.stGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrQuadFC = MyGlobalLockVar (hGlbQuadFC);

#define lpHeader        lpMemHdrQuadFC
    // Get the array NELM and rank
    aplNELMQuadFC = lpHeader->NELM;
    aplRankQuadFC = lpHeader->Rank;
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMemQuadFC = VarArrayDataFmBase (lpMemHdrQuadFC);

    // Check for short []FC
    if (uIndex >= aplNELMQuadFC)
        bRet = FALSE;
    else
        lpMemQuadFC[uIndex] = aplCharQuadFC;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbQuadFC); lpMemHdrQuadFC = NULL;

    return bRet;
} // End SetQuadFCValue


//***************************************************************************
//  $GetQuadICValue
//
//  Return a specified value from []IC
//***************************************************************************

APLINT GetQuadICValue
    (IC_INDICES icIndex)

{
    LPPERTABDATA      lpMemPTD;                 // Ptr to PerTabData global memory
    HGLOBAL           hGlbQuadIC;               // []IC global memory handle
    LPVARARRAY_HEADER lpMemHdrQuadIC = NULL;    // Ptr to []IC global memory header
    LPAPLINT          lpMemQuadIC;              // Ptr to []IC global memory
    APLSTYPE          aplTypeQuadIC;            // []IC storage type
    APLNELM           aplNELMQuadIC;            // []IC NELM
    APLINT            aplIntQuadIC;             // []IC[uIndex]

    Assert (icIndex < ICNDX_LENGTH);

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the []IC global memory handle
    hGlbQuadIC = lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_IC]->stData.stGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrQuadIC = MyGlobalLockVar (hGlbQuadIC);

#define lpHeader        lpMemHdrQuadIC
    // Get the array Type and NELM
    aplTypeQuadIC = lpHeader->ArrType;
    aplNELMQuadIC = lpHeader->NELM;
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMemQuadIC = VarArrayDataFmBase (lpMemHdrQuadIC);

    // Check for short []IC
    if (icIndex >= aplNELMQuadIC)
        aplIntQuadIC = aplDefaultIC[icIndex];
    else
        // Get next item from global memory ([]IC can be BOOL, INT, or APA only)
        GetNextItemMem (lpMemQuadIC,        // Ptr to item global memory data
                        aplTypeQuadIC,      // Item storage type
                        aplNELMQuadIC,      // Item NELM
                        icIndex,            // Index into item
                        NULL,               // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                       &aplIntQuadIC);      // Ptr to result immediate value (may be NULL)
    // We no longer need this ptr
    MyGlobalUnlock (hGlbQuadIC); lpMemHdrQuadIC = NULL;

    return aplIntQuadIC;
} // End GetQuadICValue


//***************************************************************************
//  $DisplayGlbVector
//
//  Display a non-empty non-PtrArray vector-like array
//    without running into a LIMIT ERROR on a guard page
//    in case the vector is too long.
//***************************************************************************

UBOOL DisplayGlbVector
    (LPAPLCHAR lpaplCharIni,        // Ptr to temp area
     APLSTYPE  aplType,             // Array storage type
     APLDIM    aplDimNCols,         // Array # cols (!= 0)
     LPVOID    lpMemArr,            // Ptr to array global memory
     UBOOL     bEndingCR,           // TRUE iff last line has CR
     LPUBOOL   lpbCtrlBreak,        // Ptr to Ctrl-Break flag
     APLUINT   uQuadPW)             // []PW

{
    APLINT    uCopyLen;             // Length
    APLUINT   uCnt,                 // Loop counter
              uIniPos,              // Initial position
              uCurPos,              // Current ...
              uOffPos,              // Offset  ...
              uMaxPos;              // Maximum ...
    IMM_TYPES immType;              // Immediate type
    WCHAR     wc;                   // Temp var
    LPAPLCHAR lpaplChar,            // Prt to current output save char
              lpaplCharNxt;         // Ptr to next char
    LPAPLCHAR lpMemArrIni;          // Ptr to initial array global memory
    APLINT    iSizeof;              // sizeof () datatype
    UBOOL     bRet = FALSE,         // TRUE iff we succeeded
              bLineCont = FALSE,    // TRUE iff a continued line
              bBoolAPA = FALSE;     // TRUE iff the type is BOOL or APA
    HWND      hWndSM,               // Window handle for the Session Manager
              hWndEC;               // ...                                  Edit Ctrl

    // Get the current SM window handle
    hWndSM = GetMemPTD ()->hWndSM;
    hWndEC = (HWND) (HANDLE_PTR) GetWindowLongPtrW (hWndSM, GWLSF_HWNDEC);

    // Set initial values
    lpaplChar = lpaplCharIni;
    uCurPos   =
    uIniPos   =
    uOffPos   =
    uMaxPos   = 0;

    // Split cases based upon the storage type
    switch (aplType)
    {
        case ARRAY_CHAR:
            // Save the initial ptr
            lpMemArrIni = lpMemArr;

            // Continue until we're out of cols
            while (aplDimNCols NE 0)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Copy the chars to temp storage
                for (uCnt = 0;
                     (uCnt < aplDimNCols) && (uCurPos < uQuadPW);
                     uCnt++, ((LPAPLCHAR) lpMemArr)++)
                {
                    APLUINT uSkip;

                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Check for Terminal Control chars
                    uSkip =
                      CheckTermCodes ((LPAPLCHAR) lpMemArr, // Ptr to current char to test
                                      lpMemArrIni,          // ...    initial ...
                                      hWndEC,               // EditCtrl window handle
                                      lpaplCharIni,         // Ptr to initial output save area
                                     &lpaplChar,            // Ptr to ptr to output save area
                                     &uCurPos,              // Ptr to current position
                                     &uMaxPos,              // Ptr to maximum position
                                     &uIniPos,              // Ptr to initial position
                                     &bLineCont);           // Ptr to TRUE iff a continued line
                    // Skip over chars (as necessary)
                    uCnt                   += uSkip;
                    ((LPAPLCHAR) lpMemArr) += uSkip;
                } // End FOR

                // Ensure properly terminated at the maximum width
                lpaplCharIni[uMaxPos - uOffPos] = WC_EOS;

                // Account for it
                aplDimNCols -= uCnt;

                // Display the continued line w/o ending CRLF
                AppendLine (lpaplCharIni, bLineCont, FALSE);

                // Reset ptrs
                lpaplChar = lpaplCharIni;

                // Set 2nd and subsequent copy length & initial positions
////////////////uCopyLen = (APLU3264) uQuadPW - DEF_INDENT;
                uIniPos = uCurPos = uMaxPos = uOffPos = DEF_INDENT;

                // If there's more to display, ...
                if (aplDimNCols NE 0)
                    // Display the continued indent w/o ending CRLF
                    AppendLine (wszIndent, TRUE, FALSE);
                else
                    // Display an empty line with ending CRLF
                    AppendLine (L"", FALSE, TRUE);
            } // End WHILE

            break;

        case ARRAY_BOOL:
        case ARRAY_APA:
            // Mark as special
            bBoolAPA = TRUE;

            // Fall through to common code

        case ARRAY_INT:
        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:
        case ARRAY_FLOAT:
        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:
        case ARRAY_RAT:
        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:
        case ARRAY_VFP:
        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:
        case ARRAY_BA1F:
        case ARRAY_BA2F:
        case ARRAY_BA4F:
        case ARRAY_BA8F:
            // Translate the array type to immediate type & sizeof
            immType = TranslateArrayTypeToImmType (aplType);
            iSizeof = TranslateArrayTypeToSizeof (aplType);

            // Set initial values
            uCopyLen  = (APLU3264) uQuadPW;

            // Loop through the cols
            for (uCnt = 0; uCnt < aplDimNCols; uCnt++)
            {
                // If it's BOOL/APA, ...
                if (bBoolAPA)
                {
                    APLINT aplInteger = GetNextInteger (lpMemArr, aplType, uCnt);

                    // Format the next value
                    lpaplCharNxt =
                      FormatImmedPtr (lpaplChar,        // Ptr to output save area
                                      immType,          // Immediate type
                                     &aplInteger);      // Ptr to the value
                } else
                {
                    // Format the next value
                    lpaplCharNxt =
                      FormatImmedPtr (lpaplChar,        // Ptr to output save area
                                      immType,          // Immediate type
                                      lpMemArr);        // Ptr to the value
                    // Skip over the item
                    ((LPBYTE) lpMemArr) += iSizeof;
                } // End IF/ELSE

                // Recall that <FormatImmedPtr> and similar routines all append a
                //   trailing blank and return a ptr to the char after the blank,
                //   hence <lpaplCharNxt - 1> points to the trailing blank and is
                //   thus a proper ptr to the char after the last char to be displayed.

                // While the line exceeds the width, ...
                while (uCopyLen < ((lpaplCharNxt - 1) - lpaplCharIni))
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // If this is the first item on the line, ...
                    if (lpaplChar EQ lpaplCharIni)
                        // lpaplCharIni -> '11111111111234567890123456789r12345'
                        // lpaplChar    ->  ^
                        // lpaplCharNxt ->                                     ^
                        // uCopyLen     ->                                 ^

                        // Point to end of the page width
                        lpaplChar = &lpaplCharIni[uCopyLen];
                    else
                    {
                        // lpaplCharIni -> '1111111111 1234567890123456789r12345'
                        // lpaplChar    ->             ^
                        // lpaplCharNxt ->                                      ^
                        // uCopyLen     ->                                  ^
                    } // End IF/ELSE

                    // Terminate the string
                    wc = lpaplChar[0];
                    lpaplChar[0] = WC_EOS;

                    // Display the line w/o ending CRLF
                    AppendLine (lpaplCharIni, FALSE, FALSE);

                    // Restore the zapped digit
                    lpaplChar[0] = wc;

                    // Move down the remaining values
                    CopyMemoryW (lpaplCharIni,
                                 lpaplChar,
                                 lpaplCharNxt - lpaplChar);

                    // Reset the current & next ptrs
                    lpaplCharNxt = &lpaplCharIni[lpaplCharNxt - lpaplChar];
                    lpaplChar    = lpaplCharIni;

                    // Set 2nd and subsequent copy length & initial positions
                    uCopyLen = (APLU3264) uQuadPW - DEF_INDENT;
                    uIniPos = uCurPos = DEF_INDENT;

                    // Display the continued indent w/o ending CRLF
                    AppendLine (wszIndent, TRUE, FALSE);
                } // End WHILE

                // Reset the current ptr
                lpaplChar = lpaplCharNxt;

                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;
            } // End FOR

            // If there's still more values to display, ...
            if (lpaplChar > lpaplCharIni)
            {
                // Terminate the string
                lpaplChar[0] = WC_EOS;

                // Display the line with ending CRLF
                AppendLine (lpaplCharIni, FALSE, TRUE);
            } // End IF

            break;

        defstop
            break;
    } // End SWITCH

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    return bRet;
} // End DisplayGlbVector


//***************************************************************************
//  $CheckTermCodes
//
//  Check a character for terminal Codes
//***************************************************************************

APLUINT CheckTermCodes
    (LPWCHAR    lpwc,           // Ptr to Temp var
     LPWCHAR    lpwcIni,        // Ptr to initial ...
     HWND       hWndEC,         // Window handle of the EditCtrl
     LPAPLCHAR  lpaplCharIni,   // Ptr to initial output save area
     LPAPLCHAR *lplpaplChar,    // Ptr to ptr to output save area
     APLUINT   *lpuCurPos,      // Ptr to current position
     APLUINT   *lpuMaxPos,      // Ptr to maximum position
     APLUINT   *lpuIniPos,      // Ptr to initial position
     LPUBOOL    lpbLineCont)    // Ptr to TRUE iff a continued line

{
    APLUINT uSpaces,            // # spaces to fill in for HT
            uSkip = 0;          // # chars to skip over
    UBOOL   bPrevCR;            // TRUE iff the preceding char is WC_CR

    // Split cases based upon the char value
    switch (*lpwc)
    {
        // N.B.  The cases for CR & LF *MUST* come first as they
        //       have different test conditions than the others
        case WC_CR:
            // If we're NOT Output Debugging,
            //   or we are, but we're excluding CR & LF from display, ...
            if (!OptionFlags.bOutputDebug || OptionFlags.bOutputExCRLF)
            {
                // Ensure properly terminated at the max width
                lpaplCharIni[*lpuMaxPos - *lpuIniPos] = WC_EOS;

                // Reset the ptrs and counters (but not max width) to the start of the line
                (*lplpaplChar) = lpaplCharIni;
                (*lpuCurPos)   =
                (*lpuIniPos)   = 0;

                // If this char is followed by WC_CR and WC_LF, ...
                if (lpwc[1] EQ WC_CR
                 && lpwc[2] EQ WC_LF)
                {
                    UINT uCharPos;              // Current char position in the buffer

                    // Get the indices of the selected text, and as
                    //   there is none, we need the leading position only
                    SendMessageW (hWndEC, EM_GETSEL, (WPARAM) &uCharPos, 0);

                    // Insert the original text at <lpaplCharIni>
                    SendMessageW (hWndEC, EM_REPLACESEL, FALSE, (LPARAM) lpaplCharIni);

                    // Zap the original line
                    (*lpaplCharIni) = WC_EOS;

                    // Get the current position
                    SendMessageW (hWndEC, EM_GETSEL, (WPARAM) &uCharPos, 0);

                    // End the line with CR/CR/LF
                    SendMessageW (hWndEC, EM_REPLACESEL, FALSE, (LPARAM) WS_CRCRLF);

                    uSkip = 2;
                } // End IF

                break;
            } else
            {
                // Fall through to common code
                // Note that bOutputDebug and bOutputExCRLF are FALSE,
                //   so the next IF statement (if present) falls through
            } // End IF/ELSE

        case WC_LF:
            // If we're NOT Output Debugging,
            //   or we are, but we're excluding CR & LF from display, ...
            if (!OptionFlags.bOutputDebug || OptionFlags.bOutputExCRLF)
            {
                // Ensure properly terminated at the max width
                lpaplCharIni[*lpuMaxPos - *lpuIniPos] = WC_EOS;

                // Is the preceding char WC_CR?
                bPrevCR = (lpwcIni < lpwc
                        && lpwc[-1] EQ WC_CR);

                // Display the (possibly continued) line with or w/o ending CRLF
                AppendLine (lpaplCharIni, *lpbLineCont, bPrevCR);

                // If the preceding char isn't WC_CR, ...
                if (!bPrevCR)
                    // Mark the following lines as continued
                    *lpbLineCont = TRUE;

                // Fill the line up to the current position with blanks
                FillMemoryW (lpaplCharIni, (APLU3264) *lpuCurPos, L' ');

                // Reset the maximum position to the current position
                *lpuMaxPos = *lpuCurPos;

                break;
            } else
            {
                // Fall through to common code
                // Note that bOutputDebug and bOutputExCRLF are FALSE,
                //   so the next IF statement (if present) falls through
            } // End IF/ELSE

        case WC_HT:
            // If we're NOT Output Debugging, ...
            if (!OptionFlags.bOutputDebug)
            {
                // Insert a tab -- convert into insert N spaces
                uSpaces = (((*lpuCurPos) / DEF_TABS) * DEF_TABS + DEF_TABS) - *lpuCurPos;

                Assert (uSpaces <= DEF_TABS);

                // Fill with spaces
                FillMemoryW (*lplpaplChar, (APLU3264) uSpaces, L' ');

                // Skip over the spaces
                (*lplpaplChar) += uSpaces;
                (*lpuCurPos)   += uSpaces;
                (*lpuMaxPos)   = max (*lpuMaxPos, *lpuCurPos);

                break;
            } else
            {
                // Fall through to common code
                // Note that bOutputDebug is FALSE,
                //   so the next IF statement (if present) falls through
            } // End IF/ELSE

        case WC_BEL:    // Bel
            // If we're NOT Output Debugging, ...
            if (!OptionFlags.bOutputDebug)
            {
                // Sound the alarum!
                MessageBeep (NEG1U);

                break;
            } else
            {
                // Fall through to common code
                // Note that bOutputDebug is FALSE,
                //   so the next IF statement (if present) falls through
            } // End IF/ELSE

        case WC_BS:
            // If we're NOT Output Debugging, ...
            if (!OptionFlags.bOutputDebug)
            {
                // If there's room to backspace, ...
                if (lpuCurPos[0] > *lpuIniPos)
                {
                    // Back up one position
                    (*lplpaplChar)--;
                    (*lpuCurPos)--;
                } // End IF

                break;
            } else
            {
                // Fall through to common code
                // Note that bOutputDebug is FALSE,
                //   so the next IF statement (if present) falls through
            } // End IF/ELSE

        default:
            // Save the new char
            *(*lplpaplChar)++ = *lpwc;
            (*lpuCurPos)++;
            (*lpuMaxPos)      = max (*lpuMaxPos, *lpuCurPos);

            break;

        case WC_EOS:
            *(*lplpaplChar)++ = UTF16_REPLACEMENT0000;
            (*lpuCurPos)++;
            (*lpuMaxPos)      = max (*lpuMaxPos, *lpuCurPos);

            break;
    } // End SWITCH

    return uSkip;
} // End CheckTermCodes


//***************************************************************************
//  End of File: display.c
//***************************************************************************
