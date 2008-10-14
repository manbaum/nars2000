//***************************************************************************
//  NARS2000 -- Primitive Function -- DownTackJot
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2008 Sudley Place Software

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

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "termcode.h"
#include "externs.h"
#include "display.h"
#include "sysvars.h"
#include "pertab.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


typedef struct tagWID_PRC
{
    APLUINT uWid:31,                // Actual width (if Auto, then this field is initially 0)
            Auto:1;                 // TRUE iff this width is automatic
    APLINT  iPrc;                   // Actual precision (0 = none, >0 = decimal notation, <0 = E-notation)
} WIDPRC, *LPWIDPRC;


//***************************************************************************
//  $PrimFnDownTackJot_EM_YY
//
//  Primitive function for monadic and dyadic DownTackJot
//    ("default format" and "format by example")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDownTackJot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDownTackJot_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_DOWNTACKJOT);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonDownTackJot_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydDownTackJot_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimFnDownTackJot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnDownTackJot_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic DownTackJot
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnDownTackJot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnDownTackJot_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnDownTackJot_EM_YY,// Ptr to primitive function routine
                                    lptkLftArg,             // Ptr to left arg token
                                    lptkFunc,               // Ptr to function token
                                    lptkRhtArg,             // Ptr to right arg token
                                    lptkAxis);              // Ptr to axis token (may be NULL)
} // End PrimProtoFnDownTackJot_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonDownTackJot_EM_YY
//
//  Primitive function for monadic DownTackJot ("default format")
//
//  Taken from the IBM APL2 documentation:
//
//  "The formal rules listed below for default formatting of nested arrays use
//   the function NOTCHAR.  NOTCHAR returns a 1 if its argument is not a simple
//   character array and a 0 otherwise.
//
//   For Z{is}{format}R, where R is a nested array:
//
//   * Z has a single left and right blank pad spaces
//   * Z has S intermediate blank spaces between horizontally adjacent items
//     A and B where:
//       S is the larger of ({rho}{rho}X) + NOTCHAR X
//       for both X =: A and B
//   * Z has LN intermediate blank lines between vertically adjacent items
//     C and D, where
//       LN is the 0{max}((({rho}{rho}C){max}{rho}{rho}D)-1
//   * If the rank of R is three or more, Z can contain blank lines for the
//     interdimensional spacing."
//   * Char scalar/vector items in columns containing numeric scalars are
//     right-justified; otherwise they are left-justified.
//
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonDownTackJot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonDownTackJot_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE      aplTypeRht;           // Right arg storage type
    APLNELM       aplNELMRht,           // Right arg NELM
                  aplNELMRes;           // Result    ...
    APLRANK       aplRankRht,           // Right arg rank
                  aplRankRes;           // Result    ...
    APLDIM        aplDimNCols,          // # columns
                  aplDimNRows,          // # rows
                  aplDimCol,            // Col loop counter
                  aplLastDim;           // Length of the last dimension in the result
    HGLOBAL       hGlbRht = NULL,       // Right arg global memory handle
                  hGlbRes = NULL;       // Result    ...
    LPVOID        lpMemRht = NULL,      // Ptr to right arg global memory
                  lpMemRes = NULL;      // Ptr to result    ...
    LPAPLDIM      lpMemDimRht = NULL;   // Ptr to right arg dimensions
    APLINT        aplIntegerRht;        // Right arg temporary integer
    APLFLOAT      aplFloatRht;          // ...                 float
    APLCHAR       aplCharRht;           // ...                 char
    LPFMTHEADER   lpFmtHeader,          // Ptr to format header struc
                  lpFmtHeader2;         // ...
    LPFMTCOLSTR   lpFmtColStr;          // Ptr to col struc
    LPAPLCHAR     lpaplChar,            // Ptr to output save area
                  lpaplCharStart;       // Ptr to start of output save area
    APLUINT       ByteRes;              // # bytes in the result
    UBOOL         bSimpleScalar;        // TRUE if right arg is a simple scalar
    LPPL_YYSTYPE  lpYYRes = NULL;       // Ptr to the result
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    HGLOBAL       hGlbPTD;              // PerTabData global memory handle
    LPPERTABDATA  lpMemPTD;             // Ptr to PerTabData global memory
    LPWCHAR       lpwszFormat;          // Ptr to formatting save area

    // Get the PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get ptr to formatting save area
    lpwszFormat = lpMemPTD->lpwszFormat;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the attributes (Type, NELM, and Rank) of the right args
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Get right arg's global ptrs
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    if (lpMemRht)
        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    // Get the # rows & columns in the right arg
    if (IsScalar (aplRankRht))
        aplDimNCols = aplDimNRows = 1;
    else
    {
        aplDimNCols = lpMemDimRht[aplRankRht - 1];

        // Get the # rows (across all planes)
        if (IsVector (aplRankRht)
         || IsZeroDim (aplDimNCols))
            aplDimNRows = 1;
        else
            aplDimNRows = aplNELMRht / aplDimNCols;
    } // End IF/ELSE

    // Skip over the header and dimensions to the data
    if (lpMemRht)
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

__try
{
#ifdef DEBUG
    // Fill lpwszFormat with FFs so we can tell what we actually wrote
    FillMemory (lpwszFormat, 4096, 0xFF);
#endif

    // Create a new FMTHEADER
    lpFmtHeader2 = (LPFMTHEADER) lpwszFormat;
    ZeroMemory (lpFmtHeader2, sizeof (FMTHEADER));
    lpFmtHeader = lpFmtHeader2;
    lpFmtHeader->lpFmtHeadUp = NULL;
#ifdef DEBUG
    lpFmtHeader->Sig.nature  = FMTHEADER_SIGNATURE;
#endif
////lpFmtHeader->lpFmtRowUp  = NULL;                // Already zero from ZeroMemory (No parent row struct)
////lpFmtHeader->lpFmtColUp  = NULL;                // ...                           ...       col ...
////lpFmtHeader->lpFmtRow1st =                      // Filled in below
////lpFmtHeader->lpFmtRowLst =                      // ...
////lpFmtHeader->lpFmtCol1st =                      // ...
    lpFmtHeader->uRealRows   = (UINT) aplDimNRows;  // # real rows from dimensions
////lpFmtHeader->uActRows    = 0;                   // Already zero from ZeroMemory (initial value)
    lpFmtHeader->uActCols    = (UINT) aplDimNCols;
////lpFmtHeader->uFmtRows    = 0;                   // Already zero from ZeroMemory (initial value)
////lpFmtHeader->uFmtInts    = 0;                   // ...                           ...
////lpFmtHeader->uFmtChrs    = 0;                   // ...                           ...
////lpFmtHeader->uFmtFrcs    = 0;                   // ...                           ...
////lpFmtHeader->uFmtTrBl    = 0;                   // ...                           ...
////lpFmtHeader->uDepth      = 0;                   // ...                           ...
////lpFmtHeader->uMatRes     =                      // Filled in below

    // Create <aplDimNCols> FMTCOLSTRs
    lpFmtColStr = (LPFMTCOLSTR) (&lpFmtHeader[1]);
    lpFmtHeader->lpFmtCol1st = lpFmtColStr;
    ZeroMemory (lpFmtColStr, (UINT) aplDimNCols * sizeof (FMTCOLSTR));
#ifdef DEBUG
    {
        APLDIM uCol;

        for (uCol = 0; uCol < aplDimNCols; uCol++)
            lpFmtColStr[uCol].Sig.nature = FMTCOLSTR_SIGNATURE;
    }
#endif
    // Skip over the FMTCOLSTRs to the next available position
    lpaplChar = lpaplCharStart = (LPAPLCHAR) &lpFmtColStr[aplDimNCols];

    // Save ptr to 1st and last child FMTROWSTR
    lpFmtHeader->lpFmtRow1st = lpFmtHeader->lpFmtRowLst =
      (IsZeroDim (aplDimNRows) ? NULL
                               : (LPFMTROWSTR) lpaplChar);
    // Loop through the array appending the formatted values (separated by L'\0')
    //   to the output vector, and accumulating the values in the appropriate
    //   FMTCOLSTR & FMTROWSTR entries.

    // If the right arg is a simple scalar,
    //   get its value
    bSimpleScalar = (IsScalar (aplRankRht) && IsSimpleNH (aplTypeRht));
    if (bSimpleScalar)
        GetFirstValueToken (lptkRhtArg,     // Ptr to right arg token
                           &aplIntegerRht,  // Ptr to integer result
                           &aplFloatRht,    // Ptr to float ...
                           &aplCharRht,     // Ptr to WCHAR ...
                            NULL,           // Ptr to longest ...
                            NULL,           // Ptr to lpSym/Glb ...
                            NULL,           // Ptr to ...immediate type ...
                            NULL);          // Ptr to array type ...
    // Split cases based upon the right arg's storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
////////////lpaplChar =
              CompileArrBool    (bSimpleScalar ? (LPAPLBOOL) &aplIntegerRht
                                               : (LPAPLBOOL)   lpMemRht,    // Ptr to right arg memory
                                 lpFmtHeader,                               // Ptr to parent header
                                 lpFmtColStr,                               // Ptr to vector of ColStrs
                                 lpaplChar,                                 // Ptr to compiled output
                                 aplDimNRows,                               // # rows
                                 aplDimNCols,                               // # cols
                                 aplRankRht,                                // Right arg rank
                                 lpMemDimRht,                               // Ptr to right arg dimensions
                                 TRUE);                                     // TRUE iff top level array
            break;

        case ARRAY_INT:
////////////lpaplChar =
              CompileArrInteger (bSimpleScalar ? &aplIntegerRht
                                               : (LPAPLINT)    lpMemRht,    // Ptr to right arg memory
                                 lpFmtHeader,                               // Ptr to parent header
                                 lpFmtColStr,                               // Ptr to vector of ColStrs
                                 lpaplChar,                                 // Ptr to compiled output
                                 aplDimNRows,                               // # rows
                                 aplDimNCols,                               // # cols
                                 aplRankRht,                                // Right arg rank
                                 lpMemDimRht,                               // Ptr to right arg dimensions
                                 TRUE);                                     // TRUE iff top level array
            break;

        case ARRAY_FLOAT:
////////////lpaplChar =
              CompileArrFloat   (bSimpleScalar ? &aplFloatRht
                                               : (LPAPLFLOAT)  lpMemRht,    // Ptr to right arg memory
                                 lpFmtHeader,                               // Ptr to parent header
                                 lpFmtColStr,                               // Ptr to vector of ColStrs
                                 lpaplChar,                                 // Ptr to compiled output
                                 aplDimNRows,                               // # rows
                                 aplDimNCols,                               // # cols
                                 aplRankRht,                                // Right arg rank
                                 lpMemDimRht,                               // Ptr to right arg dimensions
                                 TRUE);                                     // TRUE iff top level array
            break;

        case ARRAY_APA:
////////////lpaplChar =
              CompileArrAPA     ((LPAPLAPA)    lpMemRht,                    // Ptr to right arg memory
                                 lpFmtHeader,                               // Ptr to parent header
                                 lpFmtColStr,                               // Ptr to vector of ColStrs
                                 lpaplChar,                                 // Ptr to compiled output
                                 aplDimNRows,                               // # rows
                                 aplDimNCols,                               // # cols
                                 aplRankRht,                                // Right arg rank
                                 lpMemDimRht,                               // Ptr to right arg dimensions
                                 TRUE);                                     // TRUE iff top level array
            break;

        case ARRAY_CHAR:
            // Return the right arg

            // If the right arg is scalar,
            //   return it as an immediate
            if (IsScalar (aplRankRht))
            {
                // Allocate a new YYRes
                lpYYRes = YYAlloc ();

                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
                lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_CHAR;
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkChar     = aplCharRht;
                lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

                goto QUICK_EXIT;
            } else
            {
                // Make a copy of the right arg
                hGlbRes = CopySymGlbDirAsGlb (hGlbRht);

                goto NORMAL_EXIT;
            } // End IF/ELSE

        case ARRAY_HETERO:
////////////lpaplChar =
              CompileArrHetero  ((LPAPLHETERO) lpMemRht,        // Ptr to right arg memory
                                 lpFmtHeader,                   // Ptr to parent header
                                 lpFmtColStr,                   // Ptr to vector of ColStrs
                                 lpaplChar,                     // Ptr to compiled output
                                 aplDimNRows,                   // # rows
                                 aplDimNCols,                   // # cols
                                 aplRankRht,                    // Right arg rank
                                 lpMemDimRht,                   // Ptr to right arg dimensions
                                 TRUE,                          // TRUE iff top level array
                                 FALSE);                        // TRUE iff handle []TCLF specially
            break;

        case ARRAY_NESTED:
////////////lpaplChar =
              CompileArrNested  ((LPAPLNESTED) lpMemRht,        // Ptr to right arg memory
                                 lpFmtHeader,                   // Ptr to parent header
                                 lpFmtColStr,                   // Ptr to vector of ColStrs
                                 lpaplChar,                     // Ptr to compiled output
                                 aplDimNRows,                   // # rows
                                 aplDimNCols,                   // # cols
                                 aplRankRht,                    // Right arg rank
                                 lpMemDimRht,                   // Ptr to right arg dimensions
                                 TRUE,                          // TRUE iff top level array
                                 TRUE,                          // TRUE iff first (leftmost) col
                                 TRUE);                         // TRUE iff last (rightmost) col
            break;

        defstop
            break;
    } // End SWITCH
} __except (CheckVirtAlloc (GetExceptionInformation (), L"PrimFnMonDownTackJot_EM_YY"))
{
    // Split cases based upon the exception code
    switch (MyGetExceptionCode ())
    {
        case EXCEPTION_LIMIT_ERROR:
            goto LIMIT_EXIT;

        defstop
            break;
    } // End SWITCH
} // End __try/__Except

    // Propagate the row & col count up the line
    PropagateRowColCount (lpFmtHeader);

    // CHARs are handled above

    // For nested arrays, the rank of the result is one
    //   if all items are scalars or vectors, otherwise
    //   the rank is two.
    // For non-CHAR simple arrays, the rank of the result
    //   is the larger of the rank of the right arg and one.
    if (IsNested (aplTypeRht) || lpFmtHeader->uMatRes)
        aplRankRes = 1 + lpFmtHeader->uMatRes;
    else
        aplRankRes = max (aplRankRht, 1);

    // Add up the width of each column to get the
    //   # cols in the result
    for (aplLastDim = aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        aplLastDim += ExteriorColWidth (&lpFmtColStr[aplDimCol]);
    Assert (aplLastDim EQ ExteriorHdrWidth (lpFmtHeader));

    // Calculate the NELM of the result
    aplNELMRes = (lpFmtHeader->uFmtRows * ExteriorHdrWidth (lpFmtHeader));

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRes, aplRankRes);

    //***************************************************************
    // Now we can allocate the storage for the result
    // N.B.:  Conversion from APLUINT to UINT.
    //***************************************************************
    Assert (ByteRes EQ (__int3264) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (__int3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);

    // If this is a nested array and the rank
    //   of the result is two, fill in the
    //   next-to-the-last dimension
    if ((IsNested (aplTypeRht) || lpFmtHeader->uMatRes)
     && IsMatrix (aplRankRes))
        // Fill in the # rows
        *((LPAPLDIM) lpMemRes)++ = lpFmtHeader->uFmtRows;
    else
    {
        // Fill in all but the last dimension
        if (lpMemDimRht && !IsScalar (aplRankRht))
        for (aplDimCol = 0; aplDimCol < (aplRankRht - 1); aplDimCol++)
            *((LPAPLDIM) lpMemRes)++ = *lpMemDimRht++;
    } // End IF

    // Fill in the last dimension
    *((LPAPLDIM) lpMemRes)++ = aplLastDim;

    // lpMemRes now points to the result's data
#ifdef PREFILL
    // Fill it with all blanks
    Assert (aplNELMRes EQ (__int3264) aplNELMRes);
    FillMemoryW (lpMemRes, (__int3264) aplNELMRes, L' ');
#endif
    // Output the result at appropriate widths

    // Run through the right arg again processing the
    //   output stream into lpMemRes

    // Split cases based upon the right arg's storage type
#define lpwszOut    ((LPAPLCHAR) lpMemRes)
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_CHAR:
        case ARRAY_APA:
        case ARRAY_HETERO:
////////////lpaplChar =
              FormatArrSimple (lpFmtHeader,             // Ptr to FMTHEADER
                               lpFmtColStr,             // Ptr to vector of <aplDimNCols> FMTCOLSTRs
                               lpaplCharStart,          // Ptr to compiled input
                              &lpwszOut,                // Ptr to ptr to output string
                               lpFmtHeader->uActRows,   // # actual rows in this array
                               aplDimNCols,             // # cols in this array
                               aplLastDim,              // Length of last dim in result
                               aplRankRht,              // Rank of this array
                               lpMemDimRht,             // Ptr to this array's dimensions
                               aplTypeRht,              // Storage type of this array
                               TRUE,                    // TRUE iff skip to next row after this item
                               FALSE,                   // TRUE iff raw (not {thorn}) output
                               TRUE,                    // TRUE iff last line has CR
                               lpbCtrlBreak);           // Ptr to Ctrl-Break flag
            break;

        case ARRAY_NESTED:
////////////lpaplChar =
              FormatArrNested (lpFmtHeader,             // Ptr to FMTHEADER
                               lpMemRht,                // Ptr to raw input
                               lpFmtColStr,             // Ptr to vector of <aplDimNCols> FMTCOLSTRs
                               lpaplCharStart,          // Ptr to compiled input
                              &lpwszOut,                // Ptr to ptr to output string
                               lpFmtHeader->uActRows,   // # actual rows in this array
                               aplDimNCols,             // # cols ...
                               aplRankRht,              // Rank of this array
                               lpMemDimRht,             // Ptr to this array's dimensions
                               aplLastDim,              // Length of last dim in result
                               FALSE,                   // TRUE iff raw (not {thorn}) output
                               TRUE,                    // TRUE iff last line has CR
                               lpbCtrlBreak);           // Ptr to Ctrl-Break flag
            break;

        defstop
            break;
    } // End SWITCH
#undef  lpwszOut

    // Check for Ctrl-Break
    if (CheckCtrlBreak (*lpbCtrlBreak))
        goto ERROR_EXIT;
NORMAL_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto QUICK_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LIMIT_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LIMIT_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
QUICK_EXIT:
    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return lpYYRes;
} // End PrimFnMonDownTackJot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $CompileArrBool
//
//  Compile an array of Booleans
//***************************************************************************

LPAPLCHAR CompileArrBool
    (LPAPLBOOL   lpMem,         // Ptr to data to format
     LPFMTHEADER lpFmtHeader,   // Ptr to parent FMTHEADER
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of aplDimNCols FMTCOLSTRs
     LPAPLCHAR   lpaplChar,     // Ptr to next available format position
     APLDIM      aplDimNRows,   // # rows to format (actually it's x/ all but last dim)
     APLDIM      aplDimNCols,   // # cols to format
     APLRANK     aplRank,       // Rank of data to format
     LPAPLDIM    lpMemDim,      // Ptr to dimension vector
     UBOOL       bTopLevel)     // TRUE iff top level array

{
    UINT        uBitMask,
                uLen;
    APLDIM      aplDimCol,
                aplDimRow;
    LPFMTROWSTR lpFmtRowLcl = NULL;

    // Loop through the cols, setting the common widths
    for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
    {
        // Include a leading blank if nested or not scalar/vector or not 1st col
        uLen = (IsMultiRank (aplRank) || aplDimCol NE 0);

        // Max the current leading blanks with this
        lpFmtColStr[aplDimCol].uLdBl = max (lpFmtColStr[aplDimCol].uLdBl, uLen);

        // Calculate the length of the integer part
        uLen = 1;

        // Max the current integer width with this
        lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

////////// Calculate the length of the fractional part
////////uLen = 0;
////////
////////// Max the current fractional width with this
////////lpFmtColStr[aplDimCol].uFrcs = max (lpFmtColStr[aplDimCol].uFrcs, uLen);

////////// Include a trailing blank if nested and last col
////////uLen = 0;
////////
////////// Max the current trailing blanks with this
////////lpFmtColStr[aplDimCol].uTrBl = max (lpFmtColStr[aplDimCol].uTrBl, uLen);

////////// Max the current format type with FMT_INT
////////lpFmtColStr[aplDimCol].uFmtType = max (lpFmtColStr[aplDimCol].uFmtType, FMTTYPE_INT);
    } // End FOR

    // Initialize bit mask for all references to lpMem
    uBitMask = 0x01;

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
#ifdef DEBUG
        lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
        lpFmtRowLcl->lpFmtColUp  = lpFmtColStr;
#endif
        lpFmtRowLcl->uFmtRows    = 1;               // Initialize the count
        lpFmtRowLcl->bRealRow    = TRUE;            // Initialize as a real row
        lpFmtRowLcl->bBlank      = FALSE;           // Initialize as not all blank
        lpFmtRowLcl->uColOff     = 0;               // Initialize the col offset
        lpFmtRowLcl->uItemCount  = 0;               // Initialize the # items to follow
        lpFmtRowLcl->lpFmtRowNxt = NULL;            // Initialize the ptr

        // Link into the row chain
        lpFmtHeader->lpFmtRowLst = lpFmtRowLcl;

        // Skip over the FMTROWSTR to the next available position
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Loop through the cols
        for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        {
            // Format the Boolean
            *lpaplChar++ = ((uBitMask & *lpMem) ? L'1' : L'0');

            // Append a string separator
            *lpaplChar++ = L'\0';

            // Count in another item
            lpFmtRowLcl->uItemCount++;

            // Shift over the bit mask
            uBitMask <<= 1;

            // Check for end-of-byte
            if (uBitMask EQ END_OF_BYTE)
            {
                uBitMask = 0x01;        // Start over
                lpMem++;                // Skip to next byte
            } // End IF
        } // End FOR

        // Save as ptr to next row struc
        lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;

        // If this is not at the top level and not the last row,
        //   we need to count the interplanar spacing
        //   as blank rows in the row count
        if ((!bTopLevel)                        // Not top level
         && aplDimRow NE (aplDimNRows - 1))     // Not last row
            lpaplChar = CompileBlankRows (lpaplChar,        // Ptr to output buffer
                                          lpMemDim,         // Ptr to item dimensions
                                          aplRank,          // Item rank
                                          aplDimRow,        // Item row #
                                          lpFmtColStr);     // Ptr to item FMTCOLSTR
    } // End FOR

    // Mark as last row struc
    if (lpFmtRowLcl)
        lpFmtRowLcl->lpFmtRowNxt = NULL;
    return lpaplChar;
} // End CompileArrBool


//***************************************************************************
//  $CompileArrInteger
//
//  Compile an array of integers
//***************************************************************************

LPAPLCHAR CompileArrInteger
    (LPAPLINT    lpMem,         // Ptr to data to format
     LPFMTHEADER lpFmtHeader,   // Ptr to parent FMTHEADER
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of aplDimNCols FMTCOLSTRs
     LPAPLCHAR   lpaplChar,     // Ptr to next available format position
     APLDIM      aplDimNRows,   // # rows to format (actually it's x/ all but last dim)
     APLDIM      aplDimNCols,   // # cols to format
     APLRANK     aplRank,       // Rank of data to format
     LPAPLDIM    lpMemDim,      // Ptr to dimension vector
     UBOOL       bTopLevel)     // TRUE iff top level array

{
    UINT        uLen;               // Length of integer part
    APLDIM      aplDimCol,          // Loop counter
                aplDimRow;          // ...
    LPAPLCHAR   lpwszOut;           // Ptr to output buffer
    LPFMTROWSTR lpFmtRowLcl = NULL; // Ptr to local FMTROWSTR

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
#ifdef DEBUG
        lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
        lpFmtRowLcl->lpFmtColUp  = lpFmtColStr;
#endif
        lpFmtRowLcl->uFmtRows    = 1;               // Initialize the count
        lpFmtRowLcl->bRealRow    = TRUE;            // Initialize as a real row
        lpFmtRowLcl->bBlank      = FALSE;           // Initialize as not all blank
        lpFmtRowLcl->uColOff     = 0;               // Initialize the col offset
        lpFmtRowLcl->uItemCount  = 0;               // Initialize the # items to follow
        lpFmtRowLcl->lpFmtRowNxt = NULL;            // Initialize the ptr

        // Link into the row chain
        lpFmtHeader->lpFmtRowLst = lpFmtRowLcl;

        // Skip over the FMTROWSTR to the next available position
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Loop through the cols
        for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        {
            // Format the integer
            lpaplChar =
              FormatAplint (lpwszOut = lpaplChar,
                            *lpMem++);
            // Zap the trailing blank
            lpaplChar[-1] = L'\0';

            // Count in another item
            lpFmtRowLcl->uItemCount++;

            // Include a leading blank if nested or not scalar/vector or not 1st col
            uLen = (           IsMultiRank (aplRank) || aplDimCol NE 0);

            // Max the current leading blanks with this
            lpFmtColStr[aplDimCol].uLdBl = max (lpFmtColStr[aplDimCol].uLdBl, uLen);

            // Calculate the length of the integer part
            uLen = (UINT) (lpaplChar - lpwszOut) - 1;

            // Max the current integer width with this
            lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

////////////// Calculate the length of the fractional part
////////////uLen = 0;
////////////
////////////// Max the current fractional width with this
////////////lpFmtColStr[aplDimCol].uFrcs = max (lpFmtColStr[aplDimCol].uFrcs, uLen);

////////////// Include a trailing blank if nested and last col
////////////uLen = 0;
////////////
////////////// Max the current trailing blanks with this
////////////lpFmtColStr[aplDimCol].uTrBl = max (lpFmtColStr[aplDimCol].uTrBl, uLen);

////////////// Max the current format type with FMT_INT
////////////lpFmtColStr[aplDimCol].uFmtType = max (lpFmtColStr[aplDimCol].uFmtType, FMTTYPE_INT);
        } // End FOR

        // Save as ptr to next row struc
        lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;

        // If this is not at the top level and not the last row,
        //   we need to count the interplanar spacing
        //   as blank rows in the row count
        if ((!bTopLevel)                        // Not top level
         && aplDimRow NE (aplDimNRows - 1))     // Not last row
            lpaplChar = CompileBlankRows (lpaplChar,        // Ptr to output buffer
                                          lpMemDim,         // Ptr to item dimensions
                                          aplRank,          // Item rank
                                          aplDimRow,        // Item row #
                                          lpFmtColStr);     // Ptr to item FMTCOLSTR
    } // End FOR

    // Mark as last row struc
    if (lpFmtRowLcl)
        lpFmtRowLcl->lpFmtRowNxt = NULL;
    return lpaplChar;
} // End CompileArrInteger


//***************************************************************************
//  $CompileArrFloat
//
//  Compile an array of floats
//***************************************************************************

LPAPLCHAR CompileArrFloat
    (LPAPLFLOAT  lpMem,         // Ptr to data to format
     LPFMTHEADER lpFmtHeader,   // Ptr to parent FMTHEADER
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of aplDimNCols FMTCOLSTRs
     LPAPLCHAR   lpaplChar,     // Ptr to next available format position
     APLDIM      aplDimNRows,   // # rows to format (actually it's x/ all but last dim)
     APLDIM      aplDimNCols,   // # cols to format
     APLRANK     aplRank,       // Rank of data to format
     LPAPLDIM    lpMemDim,      // Ptr to dimension vector
     UBOOL       bTopLevel)     // TRUE iff top level array

{
    UINT        uLen;
    APLDIM      aplDimCol,
                aplDimRow;
    LPAPLCHAR   lpwszOut;
    LPWCHAR     lpwsz;
    LPFMTROWSTR lpFmtRowLcl = NULL;

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
#ifdef DEBUG
        lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
        lpFmtRowLcl->lpFmtColUp  = lpFmtColStr;
#endif
        lpFmtRowLcl->uFmtRows    = 1;               // Initialize the count
        lpFmtRowLcl->bRealRow    = TRUE;            // Initialize as a real row
        lpFmtRowLcl->bBlank      = FALSE;           // Initialize as not all blank
        lpFmtRowLcl->uColOff     = 0;               // Initialize the col offset
        lpFmtRowLcl->uItemCount  = 0;               // Initialize the # items to follow
        lpFmtRowLcl->lpFmtRowNxt = NULL;            // Initialize the ptr

        // Link into the row chain
        lpFmtHeader->lpFmtRowLst = lpFmtRowLcl;

        // Skip over the FMTROWSTR to the next available position
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Loop through the cols
        for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        {
            // Format the float
            lpaplChar =
              FormatFloat (lpwszOut = lpaplChar,    // Ptr to output save area
                           *((LPAPLFLOAT) lpMem)++, // The floating point value
                           0);                      // Use default precision
            // Zap the trailing blank
            lpaplChar[-1] = L'\0';

            // Count in another item
            lpFmtRowLcl->uItemCount++;

////////////// Check for exponential format
////////////lpwsz = strchrW (lpwszOut, L'E');
////////////
////////////// If present, mark as exponential
////////////if (lpwsz)
////////////    lpFmtColStr[aplDimCol].uFmtType = max (lpFmtColStr[aplDimCol].uFmtType, FMTTYPE_EXP);
////////////
            // Include a leading blank if nested or not scalar/vector or not 1st col
            uLen = (           IsMultiRank (aplRank) || aplDimCol NE 0);

            // Max the current leading blanks with this
            lpFmtColStr[aplDimCol].uLdBl = max (lpFmtColStr[aplDimCol].uLdBl, uLen);

            // Check for decimal point
            lpwsz = strchrW (lpwszOut, L'.');
            if (lpwsz)
            {
                // Calculate the length of the integer part
                uLen = (UINT) (lpwsz - lpwszOut);

                // Max the current integer width with this
                lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

                // Calculate the length of the fractional part
                //   including the decimal point
                uLen = (UINT) (lpaplChar - lpwsz) - 1;

                // Max the current fractional width with this
                lpFmtColStr[aplDimCol].uFrcs = max (lpFmtColStr[aplDimCol].uFrcs, uLen);

////////////////// Max the current format type with FMTTYPE_FIX
////////////////lpFmtColStr[aplDimCol].uFmtType = max (lpFmtColStr[aplDimCol].uFmtType, FMTTYPE_FIX);
            } else  // No decimal point
            {
                // Calculate the length of the integer part
                uLen = (UINT) (lpaplChar - lpwszOut) - 1;

                // Max the current integer width with this
                lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

////////////////// Calculate the length of the fractional part
////////////////uLen = 0;
////////////////
////////////////// Max the current fractional width with this
////////////////lpFmtColStr[aplDimCol].uFrcs = max (lpFmtColStr[aplDimCol].uFrcs, uLen);
            } // End IF/ELSE

////////////// Include a trailing blank if nested and last col
////////////uLen = 0;
////////////
////////////// Max the current trailing blanks with this
////////////lpFmtColStr[aplDimCol].uTrBl = max (lpFmtColStr[aplDimCol].uTrBl, uLen);
        } // End FOR

        // Save as ptr to next row struc
        lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;

        // If this is not at the top level and not the last row,
        //   we need to count the interplanar spacing
        //   as blank rows in the row count
        if ((!bTopLevel)                        // Not top level
         && aplDimRow NE (aplDimNRows - 1))     // Not last row
            lpaplChar = CompileBlankRows (lpaplChar,        // Ptr to output buffer
                                          lpMemDim,         // Ptr to item dimensions
                                          aplRank,          // Item rank
                                          aplDimRow,        // Item row #
                                          lpFmtColStr);     // Ptr to item FMTCOLSTR
    } // End FOR

    // Mark as last row struc
    if (lpFmtRowLcl)
        lpFmtRowLcl->lpFmtRowNxt = NULL;
    return lpaplChar;
} // End CompileArrFloat


//***************************************************************************
//  $CompileArrChar
//
//  Compile an array of chars
//***************************************************************************

LPAPLCHAR CompileArrChar
    (LPAPLCHAR   lpMem,         // Ptr to data to format
     LPFMTHEADER lpFmtHeader,   // Ptr to parent FMTHEADER
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of aplDimNCols FMTCOLSTRs
     LPAPLCHAR   lpaplChar,     // Ptr to next available format position
     APLDIM      aplDimNRows,   // # rows to format (actually it's x/ all but last dim)
     APLDIM      aplDimNCols,   // # cols to format
     APLRANK     aplRank,       // Rank of data to format
     LPAPLDIM    lpMemDim,      // Ptr to dimension vector
     UBOOL       bTopLevel)     // TRUE iff top level array

{
    APLDIM      aplDimRow,      // Loop counter
                aplDimCol;      // ...
    LPFMTROWSTR lpFmtRowLcl = NULL; // Ptr to local FMTROWSTR
    UINT        uCurWidth,      // Current col width
                uCurPos,        // Current col position
                uMaxWidth,      // Maximum col width
                uItmWidth,      // Maximum item width
                uTmp;           // Temporary

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
#ifdef DEBUG
        lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
        lpFmtRowLcl->lpFmtColUp  = lpFmtColStr;
#endif
        lpFmtRowLcl->uFmtRows    = 1;               // Initialize the count
        lpFmtRowLcl->bRealRow    = TRUE;            // Initialize as a real row
        lpFmtRowLcl->bBlank      = FALSE;           // Initialize as not all blank
        lpFmtRowLcl->uColOff     = 0;               // Initialize the col offset
        lpFmtRowLcl->uItemCount  = 0;               // Initialize the # items to follow
        lpFmtRowLcl->lpFmtRowNxt = NULL;            // Initialize the ptr

        // Link into the row chain
        lpFmtHeader->lpFmtRowLst = lpFmtRowLcl;

        // Skip over the FMTROWSTR to the next available position
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Copy the data including a terminating zero
        //  and launder special chars
        for (aplDimCol = uCurWidth = uCurPos = uMaxWidth = uItmWidth = 0;
             aplDimCol < aplDimNCols;
             aplDimCol++)
        switch (lpMem[aplDimCol])
        {
            case TCDEL:         // []TCDEL -- Ignore this
            case TCESC:         // []TCESC -- Ignore this
            case TCFF:          // []TCFF  -- Ignore this
            case TCNUL:         // []TCNUL -- Ignore this
                break;

            case TCBEL:         // []TCBEL -- Sound the alarum!
                MessageBeep (NEG1U);

                break;

            case TCHT:          // []TCHT -- Move ahead to the next tab stop
                uTmp = (1 + (uCurPos % uTabStops)) * uTabStops;
                while (uCurPos < uTmp)
                {
                    lpaplChar[uCurPos] = L' ';
                    uCurPos++;
                    uCurWidth++;
                    uItmWidth++;
                    uMaxWidth = max (uMaxWidth, uCurWidth);
                } // End WHILE

                break;

            case TCBS:          // []TCBS -- Backspace if there's room
                if (uCurPos)
                {
                    uCurPos--;
                    uCurWidth--;
                } // End IF

                break;

            case TCNL:          // []TCNL -- Restart at the beginning of this line
                // Start at the leftmost position
                uCurPos = 0;

                break;

            case TCLF:          // []TCLF -- Start a new line
                // Skip over the item width
                lpaplChar += uItmWidth;

                // Ensure properly terminated
                *lpaplChar++ = L'\0';

                // Max the current width with the maximum width of this col
                lpFmtColStr[0].uChrs = max (lpFmtColStr[0].uChrs, uMaxWidth);

                // Save as ptr to next row struc
                lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;

                // Create a new FMTROWSTR
                lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
#ifdef DEBUG
                lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
                lpFmtRowLcl->lpFmtColUp  = lpFmtColStr;
#endif
                lpFmtRowLcl->uFmtRows    = 1;               // Initialize the count
                lpFmtRowLcl->bRealRow    = FALSE;           // Initialize as not a real row
                lpFmtRowLcl->bBlank      = FALSE;           // Initialize as not all blank
                lpFmtRowLcl->uColOff     = uCurPos;         // Initialize the col offset
                lpFmtRowLcl->uItemCount  = 0;               // Initialize the # items to follow
////////////////lpFmtRowLcl->lpFmtRowNxt = NULL;            // Initialize the ptr (filled in below)

                // Link into the row chain
                lpFmtHeader->lpFmtRowLst = lpFmtRowLcl;

                // Skip over the FMTROWSTR to the next available position
                lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

                // Mark as matrix result
                lpFmtHeader->uMatRes = TRUE;

                // Start at the leftmost position
                uCurPos = uItmWidth = 0;

                break;

            default:            // Insert a new char
                if (lpMem[aplDimCol] < 32)  // If none of the above and not printable,
                    break;                  //   ignore it

                lpaplChar[uCurPos] = lpMem[aplDimCol];
                uCurPos++;
                uCurWidth++;
                uItmWidth++;
                uMaxWidth = max (uMaxWidth, uCurWidth);

                break;
        } // End FOR/SWITCH

        // Skip over the current item width
        lpaplChar += uItmWidth;

        // Skip over the right arg cols
        lpMem     += aplDimNCols;

        // Ensure properly terminated
        *lpaplChar++ = L'\0';

        // Count in another item
        lpFmtRowLcl->uItemCount++;

        // Max the current width with the width of this col
        lpFmtColStr[0].uChrs = max (lpFmtColStr[0].uChrs, uMaxWidth);

        // Save as ptr to next row struc
        lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;

////    // If this is not at the top level and not the last row,
        // If this is not the last row,
        //   we need to count the interplanar spacing
        //   as blank rows in the row count
////    if ((!bTopLevel)                        // Not top level
////     && aplDimRow NE (aplDimNRows - 1))     // Not last row
        if (aplDimRow NE (aplDimNRows - 1))     // Not last row
            lpaplChar = CompileBlankRows (lpaplChar,        // Ptr to output buffer
                                          lpMemDim,         // Ptr to item dimensions
                                          aplRank,          // Item rank
                                          aplDimRow,        // Item row #
                                          lpFmtColStr);     // Ptr to item FMTCOLSTR
    } // End FOR

    // Mark as last row struc
    if (lpFmtRowLcl)
        lpFmtRowLcl->lpFmtRowNxt = NULL;
    return lpaplChar;
} // End CompileArrChar


//***************************************************************************
//  $CompileArrAPA
//
//  Compile an array of APA
//***************************************************************************

LPAPLCHAR CompileArrAPA
    (LPAPLAPA    lpAPA,         // Ptr to data to format
     LPFMTHEADER lpFmtHeader,   // Ptr to parent FMTHEADER
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of aplDimNCols FMTCOLSTRs
     LPAPLCHAR   lpaplChar,     // Ptr to next available format position
     APLDIM      aplDimNRows,   // # rows to format (actually it's x/ all but last dim)
     APLDIM      aplDimNCols,   // # cols to format
     APLRANK     aplRank,       // Rank of data to format
     LPAPLDIM    lpMemDim,      // Ptr to dimension vector
     UBOOL       bTopLevel)     // TRUE iff top level array

{
    APLINT      apaOff,
                apaMul,
                apaAcc;
    UINT        uLen;
    APLDIM      aplDimCol,
                aplDimRow;
    LPAPLCHAR   lpwszOut;
    LPFMTROWSTR lpFmtRowLcl = NULL;

    // Get the APA parameters
    apaOff = lpAPA->Off;
    apaMul = lpAPA->Mul;

    // Loop through the rows
    for (aplDimRow = apaAcc = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
#ifdef DEBUG
        lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
        lpFmtRowLcl->lpFmtColUp  = lpFmtColStr;
#endif
        lpFmtRowLcl->uFmtRows    = 1;               // Initialize the count
        lpFmtRowLcl->bRealRow    = TRUE;            // Initialize as a real row
        lpFmtRowLcl->bBlank      = FALSE;           // Initialize as not all blank
        lpFmtRowLcl->uColOff     = 0;               // Initialize the col offset
        lpFmtRowLcl->uItemCount  = 0;               // Initialize the # items to follow
        lpFmtRowLcl->lpFmtRowNxt = NULL;            // Initialize the ptr

        // Link into the row chain
        lpFmtHeader->lpFmtRowLst = lpFmtRowLcl;

        // Skip over the FMTROWSTR to the next available position
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Loop through the cols
        for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++, apaAcc++)
        {
            // Format the integer
            lpaplChar =
              FormatAplint (lpwszOut = lpaplChar,
                            apaOff + apaMul * apaAcc);
            // Zap the trailing blank
            lpaplChar[-1] = L'\0';

            // Count in another item
            lpFmtRowLcl->uItemCount++;

            // Include a leading blank if nested or not scalar/vector or not 1st col
            uLen = (           IsMultiRank (aplRank) || aplDimCol NE 0);

            // Max the current leading blanks with this
            lpFmtColStr[aplDimCol].uLdBl = max (lpFmtColStr[aplDimCol].uLdBl, uLen);

            // Calculate the length of the integer part
            uLen = (UINT) (lpaplChar - lpwszOut) - 1;

            // Max the current integer width with this
            lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

////////////// Calculate the length of the fractional part
////////////uLen = 0;
////////////
////////////// Max the current fractional width with this
////////////lpFmtColStr[aplDimCol].uFrcs = max (lpFmtColStr[aplDimCol].uFrcs, uLen);

////////////// Include a trailing blank if nested and last col
////////////uLen = 0;
////////////
////////////// Max the current trailing blanks with this
////////////lpFmtColStr[aplDimCol].uTrBl = max (lpFmtColStr[aplDimCol].uTrBl, uLen);
        } // End FOR

        // Save as ptr to next row struc
        lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;

        // If this is not at the top level and not the last row,
        //   we need to count the interplanar spacing
        //   as blank rows in the row count
        if ((!bTopLevel)                        // Not top level
         && aplDimRow NE (aplDimNRows - 1))     // Not last row
            lpaplChar = CompileBlankRows (lpaplChar,        // Ptr to output buffer
                                          lpMemDim,         // Ptr to item dimensions
                                          aplRank,          // Item rank
                                          aplDimRow,        // Item row #
                                          lpFmtColStr);     // Ptr to item FMTCOLSTR
    } // End FOR

    // Mark as last row struc
    if (lpFmtRowLcl)
        lpFmtRowLcl->lpFmtRowNxt = NULL;
    return lpaplChar;
} // End CompileArrAPA


//***************************************************************************
//  $CompileArrHetero
//
//  Compile an array of LPSYMENTRYs
//***************************************************************************

LPAPLCHAR CompileArrHetero
    (LPAPLHETERO lpMem,         // Ptr to data to format
     LPFMTHEADER lpFmtHeader,   // Ptr to parent FMTHEADER
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of aplDimNCols FMTCOLSTRs
     LPAPLCHAR   lpaplChar,     // Ptr to next available format position
     APLDIM      aplDimNRows,   // # rows to format (actually it's x/ all but last dim)
     APLDIM      aplDimNCols,   // # cols to format
     APLRANK     aplRank,       // Rank of data to format
     LPAPLDIM    lpMemDim,      // Ptr to dimension vector
     UBOOL       bTopLevel,     // TRUE iff top level array
     UBOOL       bTCLF)         // TRUE iff handle []TCLF specially

{
    UINT        uLen;           // Length of integer or fractional part
    APLDIM      aplDimCol,      // Loop counter
                aplDimRow;      // ...
    LPAPLCHAR   lpwszOut;       // Ptr to output buffer
    LPWCHAR     lpwsz;          // ...
    LPFMTROWSTR lpFmtRowLcl = NULL; // Ptr to current FMTROWSTR
    UINT        immTypeCur,     // Current immediate type
                immTypeLst;     // Last    ...

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
#ifdef DEBUG
        lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
        lpFmtRowLcl->lpFmtColUp  = lpFmtColStr;
#endif
        lpFmtRowLcl->uFmtRows    = 1;               // Initialize the count
        lpFmtRowLcl->bRealRow    = TRUE;            // Initialize as a real row
        lpFmtRowLcl->bBlank      = FALSE;           // Initialize as not all blank
        lpFmtRowLcl->uColOff     = 0;               // Initialize the col offset
        lpFmtRowLcl->uItemCount  = 0;               // Initialize the # items to follow
        lpFmtRowLcl->lpFmtRowNxt = NULL;            // Initialize the ptr

        // Link into the row chain
        lpFmtHeader->lpFmtRowLst = lpFmtRowLcl;

        // Skip over the FMTROWSTR to the next available position
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Note that it doesn't matter what we start off the row
        //   with as the last immediate type (immTypeLst) because
        //   the phrase "(aplDimCol NE 0) &&" causes any value to
        //   which we set it to be ignored.
////////immTypeLst = IMMTYPE_BOOL;

        // Loop through the cols
        for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        {
            UBOOL bZapBlank = TRUE;         // TRUE iff we should zap the trailing
                                            //   blank from FormatImmed

#define lpSymEntry      ((LPAPLHETERO) lpMem)

            // Save the immediate type
            immTypeCur = (*lpSymEntry)->stFlags.ImmType;

            // If we're to handle []TCLF specially, ...
            if (bTCLF                                   // Handle specially, and
             && IsImmChr (immTypeCur)                   //   it's char, and
             && (*lpSymEntry)->stData.stChar EQ TCLF)   //   it's []TCLF
            {
                APLDIM uCol;            // Loop counter

                // Append trailing empty cells for each remaining col
                for (uCol = aplDimCol; uCol < aplDimNCols; uCol++)
                {
                    *lpaplChar++ = L'\0';

                    // Count in another item
                    lpFmtRowLcl->uItemCount++;
                } // End IF

                // Save as ptr to next row struc
                lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;

                // Create a new FMTROWSTR
                lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
#ifdef DEBUG
                lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
                lpFmtRowLcl->lpFmtColUp  = lpFmtColStr;
#endif
                lpFmtRowLcl->uFmtRows    = 1;               // Initialize the count
                lpFmtRowLcl->bRealRow    = FALSE;           // Initialize as not a real row
                lpFmtRowLcl->bBlank      = FALSE;           // Initialize as not all blank
                lpFmtRowLcl->uColOff     = 0;               // Initialize the col offset
                lpFmtRowLcl->uItemCount  = 0;               // Initialize the # items to follow
////////////////lpFmtRowLcl->lpFmtRowNxt = NULL;            // Initialize the ptr (filled in below)

                // Link into the row chain
                lpFmtHeader->lpFmtRowLst = lpFmtRowLcl;

                // Skip over the FMTROWSTR to the next available position
                lpwszOut =
                  lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

                // Mark as matrix result
                lpFmtHeader->uMatRes = TRUE;

                // Append leading empty cells for each leading col
                for (uCol = 0; uCol <= aplDimCol; uCol++)
                {
                    *lpaplChar++ = L'\0';

                    // Count in another item
                    lpFmtRowLcl->uItemCount++;
                } // End IF

                // Avoid zapping last char
                bZapBlank = FALSE;
            } else
                lpaplChar =
                  FormatImmed (lpwszOut = lpaplChar,
                               immTypeCur,
                              &(*lpSymEntry)->stData.stLongest);

            // Skip to next entry
            lpSymEntry++;

#undef  lpSymEntry

            // Zap the trailing blank
            if (bZapBlank)
                lpaplChar[-1] = L'\0';

            // Count in another item
            lpFmtRowLcl->uItemCount++;

            // Include a leading blank if (first col and (nested or rank > 1))
            //                          or (not first col) and either last or current is not a char
            uLen = ((IsZeroDim (aplDimCol) && IsMultiRank (aplRank))
                 || ((aplDimCol NE 0) && (immTypeLst NE IMMTYPE_CHAR || immTypeCur NE IMMTYPE_CHAR)));
            // Max the current leading blanks with this
            lpFmtColStr[aplDimCol].uLdBl = max (lpFmtColStr[aplDimCol].uLdBl, uLen);

            // Check for decimal point, unless char
            if (immTypeCur NE IMMTYPE_CHAR
             && (lpwsz = strchrW (lpwszOut, L'.')))
            {
                // Calculate the length of the integer part
                uLen = (UINT) (lpwsz - lpwszOut);

                // Max the current integer width with this
                lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

                // Calculate the length of the fractional part
                uLen = (UINT) (lpaplChar - lpwsz) - 1;

                // Max the current fractional width with this
                lpFmtColStr[aplDimCol].uFrcs = max (lpFmtColStr[aplDimCol].uFrcs, uLen);
            } else  // No decimal point
            {
                // Calculate the length of the integer part
                // We use the following odd construction because
                //   if the previous SymEntry was a []TCLF,
                //   lpaplChar EQ lpwszOut
                uLen = (UINT) (lpaplChar - lpwszOut);
                uLen = max (uLen, 1) - 1;

                if (IsImmChr (immTypeCur))
                    // Max the current integer width with this
                    lpFmtColStr[aplDimCol].uChrs = max (lpFmtColStr[aplDimCol].uChrs, uLen);
                else
                    // Max the current integer width with this
                    lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

////////////////// Calculate the length of the fractional part
////////////////uLen = 0;
////////////////
////////////////// Max the current fractional width with this
////////////////lpFmtColStr[aplDimCol].uFrcs = max (lpFmtColStr[aplDimCol].uFrcs, uLen);
            } // End IF/ELSE

////////////// Include a trailing blank if (last col and nested)
////////////uLen = 0;
////////////
////////////// Max the current trailing blanks with this
////////////lpFmtColStr[aplDimCol].uTrBl = max (lpFmtColStr[aplDimCol].uTrBl, uLen);

            // Save as last immediate type
            immTypeLst = immTypeCur;
        } // End FOR

        // Save as ptr to next row struc
        lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;

        // If this is not at the top level and not the last row,
        //   we need to count the interplanar spacing
        //   as blank rows in the row count
        if ((!bTopLevel)                        // Not top level
         && aplDimRow NE (aplDimNRows - 1))     // Not last row
            lpaplChar = CompileBlankRows (lpaplChar,        // Ptr to output buffer
                                          lpMemDim,         // Ptr to item dimensions
                                          aplRank,          // Item rank
                                          aplDimRow,        // Item row #
                                          lpFmtColStr);     // Ptr to item FMTCOLSTR
    } // End FOR

    // Mark as last row struc
    if (lpFmtRowLcl)
        lpFmtRowLcl->lpFmtRowNxt = NULL;
    return lpaplChar;
} // End CompileArrHetero


//***************************************************************************
//  $CompileArrNested
//
//  Compile an array of LPSYMENTRYs/NESTEDs
//***************************************************************************

LPAPLCHAR CompileArrNested
    (LPAPLNESTED lpMem,         // Ptr to data to format
     LPFMTHEADER lpFmtHeader,   // Ptr to parent FMTHEADER
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of aplDimNCols FMTCOLSTRs
     LPAPLCHAR   lpaplChar,     // Ptr to next available format position
     APLDIM      aplDimNRows,   // # rows to format (actually it's x/ all but last dim)
     APLDIM      aplDimNCols,   // # cols to format
     APLRANK     aplRank,       // Rank of data to format
     LPAPLDIM    lpMemDim,      // Ptr to dimension vector
     UBOOL       bTopLevel,     // TRUE iff top level array
     UBOOL       b1stCol,       // TRUE iff first (leftmost) col
     UBOOL       bLstCol)       // TRUE iff last (rightmost) col

{
    APLDIM      aplDimCol,          // Loop counter
                aplDimRow;          // ...
    LPFMTROWSTR lpFmtRowLcl = NULL; // Ptr to local FMTROWSTR
    UINT        NotCharPlus;        // ({rho}{rho}L) + NOTCHAR L

    // Set the Matrix result bit if the arg is
    //   a matrix or higher rank array
    lpFmtHeader->uMatRes |= IsMultiRank (aplRank);

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
#ifdef DEBUG
        lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
        lpFmtRowLcl->lpFmtColUp  = lpFmtColStr;
#endif
        lpFmtRowLcl->uFmtRows    = 1;               // Initialize the count
        lpFmtRowLcl->bRealRow    = TRUE;            // Initialize as a real row
        lpFmtRowLcl->bBlank      = FALSE;           // Initialize as not all blank
        lpFmtRowLcl->uColOff     = 0;               // Initialize the col offset
        lpFmtRowLcl->uItemCount  = 0;               // Initialize the # items to follow
        lpFmtRowLcl->lpFmtRowNxt = NULL;            // Initialize the ptr

        // Link into the row chain
        lpFmtHeader->lpFmtRowLst = lpFmtRowLcl;

        // Skip over the FMTROWSTR to the next available position
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Initialize ({rho}{rho}L) + NOTCHAR L
        NotCharPlus = NEG1U;

        // Loop through the cols
        for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++, ((LPAPLHETERO) lpMem)++)
        {
            // Split cases based upon the ptr type
            switch (GetPtrTypeInd (lpMem))
            {
                case PTRTYPE_STCONST:
                    lpaplChar =
                      CompileArrNestedCon ((LPAPLHETERO) lpMem,             // Ptr to data to format
                                           lpFmtHeader,                     // The parent FMTHEADER
                                           lpFmtRowLcl,                     // This row's FMTROWSTR
                                          &lpFmtColStr[aplDimCol],          // This col's FMTCOLSTR
                                          &NotCharPlus,                     // Ptr to NOTCHAR+ var
                                           lpaplChar);                      // Ptr to next available position
                    break;

                case PTRTYPE_HGLOBAL:
                    lpaplChar =
                      CompileArrNestedGlb (ClrPtrTypeIndAsGlb (lpMem),      // The HGLOBAL to format
                                           lpFmtHeader,                     // The parent FMTHEADER
                                           lpFmtRowLcl,                     // This row's FMTROWSTR
                                          &lpFmtColStr[aplDimCol],          // This col's FMTCOLSTR
                                          &NotCharPlus,                     // Ptr to NOTCHAR+ var
                                           lpaplChar,                       // Ptr to next available position
                                           aplDimCol EQ 0,                  // TRUE iff first (leftmost) col
                                           aplDimCol EQ (aplDimNCols - 1)); // TRUE iff last (rightmost) col
                    break;

                defstop
                    break;
            } // End SWITCH
        } // End FOR

        // Save as ptr to next row struc
        lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;

        // If this is not the last row,
        //   we need to count the interplanar spacing
        //   as blank rows in the row count
        if (aplDimRow NE (aplDimNRows - 1))     // Not last row
            lpaplChar = CompileBlankRows (lpaplChar,        // Ptr to output buffer
                                          lpMemDim,         // Ptr to item dimensions
                                          aplRank,          // Item rank
                                          aplDimRow,        // Item row #
                                          lpFmtColStr);     // Ptr to item FMTCOLSTR
    } // End FOR

    // Include a leading and trailing blank column, as necessary
    if (aplDimNCols)
    {
        if (b1stCol)
            lpFmtColStr[0].uLdBl++;
        if (bLstCol)
            lpFmtColStr[aplDimNCols - 1].uTrBl++;
    } // End IF

    // Mark as last row struc
    if (lpFmtRowLcl)
        lpFmtRowLcl->lpFmtRowNxt = NULL;
    return lpaplChar;
} // End CompileArrNested


//***************************************************************************
//  $CompileArrNestedCon
//
//  Compile a symbol table constant with a nested array
//***************************************************************************

LPAPLCHAR CompileArrNestedCon
    (LPAPLHETERO lpMem,         // Ptr to data to format
     LPFMTHEADER lpFmtHeader,   // The parent FMTHEADER
     LPFMTROWSTR lpFmtRowLcl,   // This row's FMTROWSTR
     LPFMTCOLSTR lpFmtColStr,   // This col's FMTCOLSTR
     LPUINT      lpNotCharPlus, // Ptr to NOTCHAR+ var
     LPAPLCHAR   lpaplChar)     // Ptr to next available position

{
    LPFMTHEADER lpFmtHeader2;   // Ptr to format header struc
    IMM_TYPES   immTypeCon;     // Constant immediate type
    UINT        NotCharPlus;    // Current NOTCHAR+ var

#define aplDimNRows     1
#define aplDimNCols     1
#define aplRank         0

    // Create a new FMTHEADER
    lpFmtHeader2 = (LPFMTHEADER) lpaplChar;
    ZeroMemory (lpFmtHeader2, sizeof (FMTHEADER));
    lpFmtHeader2->lpFmtHeadUp = lpFmtHeader;
    lpFmtHeader = lpFmtHeader2;
#ifdef DEBUG
    lpFmtHeader->Sig.nature  = FMTHEADER_SIGNATURE;
#endif
    lpFmtHeader->lpFmtRowUp  = lpFmtRowLcl;
    lpFmtHeader->lpFmtColUp  = lpFmtColStr;
////lpFmtHeader->lpFmtRow1st =                      // Filled in below
////lpFmtHeader->lpFmtRowLst =                      // ...
////lpFmtHeader->lpFmtCol1st =                      // ...
////lpFmtHeader->uRealRows   = 0;                   // Already zero from ZeroMemory (initial value)
////lpFmtHeader->uActRows    = 0;                   // ...
    lpFmtHeader->uActCols    = (UINT) aplDimNCols;
////lpFmtHeader->uFmtRows    = 0;                   // Already zero from ZeroMemory (initial value)
////lpFmtHeader->uFmtInts    = 0;                   // ...                           ...
////lpFmtHeader->uFmtChrs    = 0;                   // ...                           ...
////lpFmtHeader->uFmtFrcs    = 0;                   // ...                           ...
////lpFmtHeader->uFmtTrBl    = 0;                   // ...                           ...
    lpFmtHeader->uDepth      = 1 + lpFmtHeader->lpFmtHeadUp->uDepth;
    lpFmtHeader->uMatRes     = IsMultiRank (aplRank);

    // Skip over the FMTHEADER to the next available position
    lpFmtColStr = (LPFMTCOLSTR) (&lpFmtHeader[1]);

    // Create <aplDimNCols> FMTCOLSTRs in the output
    lpFmtHeader->lpFmtCol1st = lpFmtColStr;
    ZeroMemory (lpFmtColStr, (__int3264) aplDimNCols * sizeof (FMTCOLSTR));
#ifdef DEBUG
    {
        APLDIM uCol;

        for (uCol = 0; uCol < aplDimNCols; uCol++)
            lpFmtColStr[uCol].Sig.nature = FMTCOLSTR_SIGNATURE;
    }
#endif
    // Skip over the FMTCOLSTRs to the next available position
    lpaplChar = (LPAPLCHAR) (&lpFmtColStr[aplDimNCols]);

    // Save ptr to 1st and last child FMTROWSTR
    lpFmtHeader->lpFmtRow1st = lpFmtHeader->lpFmtRowLst =
      (IsZeroDim (aplDimNRows) ? NULL
                               : (LPFMTROWSTR) lpaplChar);
#undef  aplRank
#undef  aplDimNCols
#undef  aplDimNRows

    // Set the immediate type
    immTypeCon = (*lpMem)->stFlags.ImmType;

    // Split cases based upon the STE immediate type
    switch (immTypeCon)
    {
        case IMMTYPE_BOOL:
            lpaplChar =
              CompileArrBool    (&(*lpMem)->stData.stBoolean,   // Ptr to right arg memory
                                  lpFmtHeader,                  // Ptr to parent header
                                  lpFmtColStr,                  // Ptr to vector of ColStrs
                                  lpaplChar,                    // Ptr to compiled output
                                  1,                            // # rows
                                  1,                            // # cols
                                  0,                            // Right arg rank
                                  NULL,                         // Ptr to right arg dimensions
                                  FALSE);                       // TRUE iff top level array
            break;

        case IMMTYPE_INT:
            lpaplChar =
              CompileArrInteger (&(*lpMem)->stData.stInteger,   // Ptr to right arg memory
                                  lpFmtHeader,                  // Ptr to parent header
                                  lpFmtColStr,                  // Ptr to vector of ColStrs
                                  lpaplChar,                    // Ptr to compiled output
                                  1,                            // # rows
                                  1,                            // # cols
                                  0,                            // Right arg rank
                                  NULL,                         // Ptr to right arg dimensions
                                  FALSE);                       // TRUE iff top level array
            break;

        case IMMTYPE_FLOAT:
            lpaplChar =
              CompileArrFloat   (&(*lpMem)->stData.stFloat,     // Ptr to right arg memory
                                  lpFmtHeader,                  // Ptr to parent header
                                  lpFmtColStr,                  // Ptr to vector of ColStrs
                                  lpaplChar,                    // Ptr to compiled output
                                  1,                            // # rows
                                  1,                            // # cols
                                  0,                            // Right arg rank
                                  NULL,                         // Ptr to right arg dimensions
                                  FALSE);                       // TRUE iff top level array
            break;

        case IMMTYPE_CHAR:
            lpaplChar =
              CompileArrChar    (&(*lpMem)->stData.stChar,      // Ptr to right arg memory
                                  lpFmtHeader,                  // Ptr to parent header
                                  lpFmtColStr,                  // Ptr to vector of ColStrs
                                  lpaplChar,                    // Ptr to compiled output
                                  1,                            // # rows
                                  1,                            // # cols
                                  0,                            // Right arg rank
                                  NULL,                         // Ptr to right arg dimensions
                                  FALSE);                       // TRUE iff top level array
            break;

        defstop
            break;
    } // End SWITCH

    // Set current NOTCHAR+ var
    NotCharPlus = 0 + (immTypeCon NE IMMTYPE_CHAR);

    // Increment leading blanks as per NotCharPlus
    if (*lpNotCharPlus NE NEG1U)
        lpFmtColStr->uLdBl += max (*lpNotCharPlus, NotCharPlus);

    // Set parent's NOTCHAR+ var
    *lpNotCharPlus = NotCharPlus;

    // Propagate the row & col count up the line
    PropagateRowColCount (lpFmtHeader);

    return lpaplChar;
} // End CompileArrNestedCon


//***************************************************************************
//  $CompileArrNestedGlb
//
//  Compile a global memory object within a nested array
//***************************************************************************

LPAPLCHAR CompileArrNestedGlb
    (HGLOBAL     hGlb,          // The HGLOBAL to format
     LPFMTHEADER lpFmtHeader,   // The parent FMTHEADER
     LPFMTROWSTR lpFmtRowStr,   // This row's FMTROWSTR
     LPFMTCOLSTR lpFmtColStr,   // This col's FMTCOLSTR
     LPUINT      lpNotCharPlus, // Ptr to NOTCHAR+ var
     LPAPLCHAR   lpaplChar,     // Ptr to next available position
     UBOOL       b1stCol,       // TRUE iff first (leftmost) col
     UBOOL       bLstCol)       // TRUE iff last (rightmost) col

{
    LPFMTHEADER lpFmtHeader2;   // Ptr to format header struc
    APLSTYPE    aplType;        // Arg storage type
    APLNELM     aplNELM;        // Arg NELM
    APLRANK     aplRank;        // Arg rank
    LPVOID      lpMem;
    LPAPLDIM    lpMemDim;       // Ptr to arg dimensions
    APLDIM      aplDimNRows,    // # rows
                aplDimNCols,    // # cols
                aplChrNCols;    // # cols for char arrays
    UBOOL       bSimpleScalar;
    APLINT      aplInteger;
    APLFLOAT    aplFloat;
    APLCHAR     aplChar;
    UINT        NotCharPlus;    // Current NOTCHAR+ var

    // Get the attributes (Type, NELM, Rank) of the global
    AttrsOfGlb (hGlb, &aplType, &aplNELM, &aplRank, NULL);

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlb);

    // Skip over the header to the dimensions
    lpMemDim = VarArrayBaseToDim (lpMem);

    // Get the # rows & columns in the right arg
    if (IsScalar (aplRank))
        aplDimNCols = aplDimNRows = 1;
    else
    {
        aplDimNCols = lpMemDim[aplRank - 1];

        // Get the # rows (across all planes)
        if (IsVector (aplRank)
         || IsZeroDim (aplDimNCols))
            aplDimNRows = 1;
        else
            aplDimNRows = aplNELM / aplDimNCols;
    } // End IF/ELSE

    // Skip over the header and dimensions to the data
    lpMem = VarArrayBaseToData (lpMem, aplRank);

    // Format char arrays as one col
    aplChrNCols = (IsSimpleChar (aplType)) ? 1 : aplDimNCols;

    // Create a new FMTHEADER
    lpFmtHeader2 = (LPFMTHEADER) lpaplChar;
    ZeroMemory (lpFmtHeader2, sizeof (FMTHEADER));
    lpFmtHeader2->lpFmtHeadUp = lpFmtHeader;
    lpFmtHeader = lpFmtHeader2;
#ifdef DEBUG
    lpFmtHeader->Sig.nature  = FMTHEADER_SIGNATURE;
#endif
    lpFmtHeader->lpFmtRowUp  = lpFmtRowStr;
    lpFmtHeader->lpFmtColUp  = lpFmtColStr;
////lpFmtHeader->lpFmtRow1st =                      // Filled in below
////lpFmtHeader->lpFmtRowLst =                      // ...
////lpFmtHeader->lpFmtCol1st =                      // ...
////lpFmtHeader->uRealRows   = 0;                   // Already zero from ZeroMemory (initial value)
////lpFmtHeader->uActRows    = 0;                   // ...
    lpFmtHeader->uActCols    = (UINT) aplChrNCols;
////lpFmtHeader->uFmtRows    = 0;                   // Already zero from ZeroMemory (initial value)
////lpFmtHeader->uFmtInts    = 0;                   // ...                           ...
////lpFmtHeader->uFmtChrs    = 0;                   // ...                           ...
////lpFmtHeader->uFmtFrcs    = 0;                   // ...                           ...
////lpFmtHeader->uFmtTrBl    = 0;                   // ...                           ...
    lpFmtHeader->uDepth      = 1 + lpFmtHeader->lpFmtHeadUp->uDepth;
    lpFmtHeader->uMatRes     = IsMultiRank (aplRank);

    // Create <aplChrNCols> FMTCOLSTRs in the output
    lpFmtColStr = (LPFMTCOLSTR) (&lpFmtHeader[1]);
    lpFmtHeader->lpFmtCol1st = lpFmtColStr;
    ZeroMemory (lpFmtColStr, (__int3264) aplChrNCols * sizeof (FMTCOLSTR));
#ifdef DEBUG
    {
        APLDIM uCol;

        for (uCol = 0; uCol < aplChrNCols; uCol++)
            lpFmtColStr[uCol].Sig.nature = FMTCOLSTR_SIGNATURE;
    }
#endif
    // Skip over the FMTCOLSTRs to the next available position
    lpaplChar = (LPAPLCHAR) (&lpFmtColStr[aplChrNCols]);

    // Save ptr to 1st and last child FMTROWSTR
    lpFmtHeader->lpFmtRow1st = lpFmtHeader->lpFmtRowLst =
      (IsZeroDim (aplDimNRows) ? NULL
                               : (LPFMTROWSTR) lpaplChar);
    // Loop through the array appending the formatted values (separated by L'\0')
    //   to the output vector, and accumulating the values in the appropriate
    //   FMTCOLSTR & FMTROWSTR entries.

    // If the right arg is a simple scalar,
    //   get its value
    bSimpleScalar = (IsScalar (aplRank) && IsSimpleNH (aplType));
    if (bSimpleScalar)
        GetFirstValueGlb (hGlb,                 // The global memory handle
                         &aplInteger,           // Ptr to integer result
                         &aplFloat,             // Ptr to float ...
                         &aplChar,              // Ptr to WCHAR ...
                          NULL,                 // Ptr to longest ...
                          NULL,                 // Ptr to lpSym/Glb ...
                          NULL,                 // Ptr to ...immediate type ...
                          NULL,                 // Ptr to array type ...
                          TRUE);                // TRUE iff we should recurse through LPSYMENTRYs
    // Split cases based upon the right arg's storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
            lpaplChar =
              CompileArrBool    (bSimpleScalar ? (LPAPLBOOL) &aplInteger
                                               : (LPAPLBOOL)   lpMem,   // Ptr to right arg memory
                                 lpFmtHeader,                           // Ptr to parent header
                                 lpFmtColStr,                           // Ptr to vector of ColStrs
                                 lpaplChar,                             // Ptr to compiled output
                                 aplDimNRows,                           // # rows
                                 aplDimNCols,                           // # cols
                                 aplRank,                               // Right arg rank
                                 lpMemDim,                              // Ptr to right arg dimensions
                                 FALSE);                                // TRUE iff top level array
            break;

        case ARRAY_INT:
            lpaplChar =
              CompileArrInteger (bSimpleScalar ? &aplInteger
                                               : (LPAPLINT)    lpMem,   // Ptr to right arg memory
                                 lpFmtHeader,                           // Ptr to parent header
                                 lpFmtColStr,                           // Ptr to vector of ColStrs
                                 lpaplChar,                             // Ptr to compiled output
                                 aplDimNRows,                           // # rows
                                 aplDimNCols,                           // # cols
                                 aplRank,                               // Right arg rank
                                 lpMemDim,                              // Ptr to right arg dimensions
                                 FALSE);                                // TRUE iff top level array
            break;

        case ARRAY_FLOAT:
            lpaplChar =
              CompileArrFloat   (bSimpleScalar ? &aplFloat
                                               : (LPAPLFLOAT)  lpMem,   // Ptr to right arg memory
                                 lpFmtHeader,                           // Ptr to parent header
                                 lpFmtColStr,                           // Ptr to vector of ColStrs
                                 lpaplChar,                             // Ptr to compiled output
                                 aplDimNRows,                           // # rows
                                 aplDimNCols,                           // # cols
                                 aplRank,                               // Right arg rank
                                 lpMemDim,                              // Ptr to right arg dimensions
                                 FALSE);                                // TRUE iff top level array
            break;

        case ARRAY_CHAR:
            lpaplChar =
              CompileArrChar    (bSimpleScalar ? &aplChar
                                               : (LPAPLCHAR)  lpMem,    // Ptr to right arg memory
                                 lpFmtHeader,                           // Ptr to parent header
                                 lpFmtColStr,                           // Ptr to vector of ColStrs
                                 lpaplChar,                             // Ptr to compiled output
                                 aplDimNRows,                           // # rows
                                 aplDimNCols,                           // # cols
                                 aplRank,                               // Right arg rank
                                 lpMemDim,                              // Ptr to right arg dimensions
                                 FALSE);                                // TRUE iff top level array
            break;

        case ARRAY_APA:
            lpaplChar =
              CompileArrAPA     ((LPAPLAPA)    lpMem,                   // Ptr to right arg memory
                                 lpFmtHeader,                           // Ptr to parent header
                                 lpFmtColStr,                           // Ptr to vector of ColStrs
                                 lpaplChar,                             // Ptr to compiled output
                                 aplDimNRows,                           // # rows
                                 aplDimNCols,                           // # cols
                                 aplRank,                               // Right arg rank
                                 lpMemDim,                              // Ptr to right arg dimensions
                                 FALSE);                                // TRUE iff top level array
            break;

        case ARRAY_HETERO:
            lpaplChar =
              CompileArrHetero  ((LPAPLHETERO) lpMem,                   // Ptr to right arg memory
                                 lpFmtHeader,                           // Ptr to parent header
                                 lpFmtColStr,                           // Ptr to vector of ColStrs
                                 lpaplChar,                             // Ptr to compiled output
                                 aplDimNRows,                           // # rows
                                 aplDimNCols,                           // # cols
                                 aplRank,                               // Right arg rank
                                 lpMemDim,                              // Ptr to right arg dimensions
                                 FALSE,                                 // TRUE iff top level array
                                 TRUE);                                 // TRUE iff handle []TCLF specially
            break;

        case ARRAY_NESTED:
            lpaplChar =
              CompileArrNested  ((LPAPLNESTED) lpMem,                   // Ptr to right arg memory
                                 lpFmtHeader,                           // Ptr to parent header
                                 lpFmtColStr,                           // Ptr to vector of ColStrs
                                 lpaplChar,                             // Ptr to compiled output
                                 aplDimNRows,                           // # rows
                                 aplDimNCols,                           // # cols
                                 aplRank,                               // Right arg rank
                                 lpMemDim,                              // Ptr to right arg dimensions
                                 FALSE,                                 // TRUE iff top level array
                                 b1stCol,                               // TRUE iff first (leftmost) col
                                 bLstCol);                              // TRUE iff last (rightmost) col
            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlb); lpMem = NULL;

    // Set current NOTCHAR+ var
    NotCharPlus = ((UINT) aplRank) + !IsSimpleChar (aplType);

    // Increment leading blanks as per NotCharPlus
    if (*lpNotCharPlus NE NEG1U)
        lpFmtColStr->uLdBl += max (*lpNotCharPlus, NotCharPlus);

    // Set parent's NOTCHAR+ var
    *lpNotCharPlus = NotCharPlus;

    // Propagate the row & col count up the line
    PropagateRowColCount (lpFmtHeader);

    return lpaplChar;
} // End CompileArrNestedGlb


//***************************************************************************
//  $InteriorColWidth
//
//  Calculate the interior width of a col
//***************************************************************************

UINT InteriorColWidth
    (LPFMTCOLSTR lpFmtColStr)

{
    UINT uWid;

    // Calc numeric width
    uWid = lpFmtColStr->uInts + lpFmtColStr->uFrcs;

    // Return larger of numeric & char widths
    return max (uWid, lpFmtColStr->uChrs);
} // End InteriorColWidth


//***************************************************************************
//  $ExteriorColWidth
//
//  Calculate the exterior width of a col
//***************************************************************************

UINT ExteriorColWidth
    (LPFMTCOLSTR lpFmtColStr)

{
    // Return leading blanks + interior width + trailing blanks
    return lpFmtColStr->uLdBl + InteriorColWidth (lpFmtColStr) + lpFmtColStr->uTrBl;
} // End ExteriorColWidth


//***************************************************************************
//  $InteriorHdrWidth
//
//  Calculate the interior width of a header
//***************************************************************************

UINT InteriorHdrWidth
    (LPFMTHEADER lpFmtHeader)

{
    UINT uWid;

    // Calc numeric width
    uWid = lpFmtHeader->uFmtInts + lpFmtHeader->uFmtFrcs;

    // Return larger of numeric & char widths
    return max (uWid, lpFmtHeader->uFmtChrs);
} // End InteriorHdrWidth


//***************************************************************************
//  $ExteriorHdrWidth
//
//  Calculate the exterior width of a header
//***************************************************************************

UINT ExteriorHdrWidth
    (LPFMTHEADER lpFmtHeader)

{
    // Return leading blanks + interior width + trailing blanks
    return lpFmtHeader->uFmtLdBl + InteriorHdrWidth (lpFmtHeader) + lpFmtHeader->uFmtTrBl;
} // End ExteriorHdrWidth


//***************************************************************************
//  $PropagateRowColCount
//
//  Propagate the row & col count up the line
//***************************************************************************

void PropagateRowColCount
    (LPFMTHEADER lpFmtHeader)       // Ptr to FMTHEADER struc

{
    UINT        uActRows,           // Accumulator for actual    rows
                uFmtRows,           // ...             formatted rows
                uInts,              // ...             integer widths
                uFrcs,              // ...             fraction widths
                uLdBl,              // ...             leading blanks
                uTrBl,              // ...             trailing blanks
                uCol;               // Loop counter
    LPFMTROWSTR lpFmtRowLcl;        // Ptr to local FMTROWSTR
    LPFMTCOLSTR lpFmtColLcl;        // Ptr to local FMTCOLSTR

    // Count the # actual & formatted rows in this block
    for (lpFmtRowLcl = lpFmtHeader->lpFmtRow1st,
           uActRows = uFmtRows = 0;
         lpFmtRowLcl NE NULL;
         lpFmtRowLcl = lpFmtRowLcl->lpFmtRowNxt, uActRows++)
        uFmtRows += lpFmtRowLcl->uFmtRows;

    // Save in head struc
    if (lpFmtHeader->lpFmtRowUp)
        lpFmtHeader->lpFmtRowUp->uFmtRows = max (lpFmtHeader->lpFmtRowUp->uFmtRows, uFmtRows);
    lpFmtHeader->uActRows = uActRows;
    lpFmtHeader->uFmtRows = max (lpFmtHeader->uFmtRows, uFmtRows);

    // Count the col width in this block
    for (uInts = uFrcs = uCol = 0,
           lpFmtColLcl = lpFmtHeader->lpFmtCol1st;
         uCol < lpFmtHeader->uActCols;
         uCol++, lpFmtColLcl++)
        uInts += lpFmtColLcl->uLdBl
               + max (lpFmtColLcl->uInts, lpFmtColLcl->uChrs)
               + lpFmtColLcl->uFrcs
               + lpFmtColLcl->uTrBl;
    // Handle leading and trailing blanks
    if (lpFmtHeader->uActCols)
    {
        uLdBl = lpFmtHeader->lpFmtCol1st->uLdBl;
        if (uInts >= uLdBl)     // Avoid going negative
            uInts -= uLdBl;
        else
            uLdBl = 0;
        uTrBl = lpFmtHeader->lpFmtCol1st[lpFmtHeader->uActCols - 1].uTrBl;
        if (uFrcs >= uTrBl)     // Avoid goinig negative
            uFrcs -= uTrBl;
        else
            uTrBl = 0;
    } else
        uLdBl = uTrBl = 0;

    // Save in col struc
    if (lpFmtHeader->lpFmtColUp)
    {
        lpFmtHeader->lpFmtColUp->uLdBl = max (lpFmtHeader->lpFmtColUp->uLdBl, uLdBl);
        lpFmtHeader->lpFmtColUp->uInts = max (lpFmtHeader->lpFmtColUp->uInts, uInts);
        lpFmtHeader->lpFmtColUp->uFrcs = max (lpFmtHeader->lpFmtColUp->uFrcs, uFrcs);
        lpFmtHeader->lpFmtColUp->uTrBl = max (lpFmtHeader->lpFmtColUp->uTrBl, uTrBl);
    } // End IF

    // Save in head struc
    lpFmtHeader->uFmtLdBl = max (lpFmtHeader->uFmtLdBl, uLdBl);
    lpFmtHeader->uFmtInts = max (lpFmtHeader->uFmtInts, uInts);
    lpFmtHeader->uFmtFrcs = max (lpFmtHeader->uFmtFrcs, uFrcs);
    lpFmtHeader->uFmtTrBl = max (lpFmtHeader->uFmtTrBl, uTrBl);

    // Propagate the uMatRes bit
    if (lpFmtHeader->lpFmtHeadUp)
        lpFmtHeader->lpFmtHeadUp->uMatRes |= lpFmtHeader->uMatRes;

    // Recurse
    if (lpFmtHeader->lpFmtHeadUp)
        PropagateRowColCount (lpFmtHeader->lpFmtHeadUp);
} // End PropagateRowColCount


//***************************************************************************
//  $CompileBlankRows
//
//  Compile blank rows to account for interdimensional spacing
//***************************************************************************

LPAPLCHAR CompileBlankRows
    (LPAPLCHAR   lpaplChar,             // Ptr to output buffer
     LPAPLDIM    lpMemDim,              // Ptr to item dimensions
     APLRANK     aplRank,               // Item rank
     APLDIM      aplDimRow,             // Item row #
     LPFMTCOLSTR lpFmtColStr)           // Ptr to item FMTCOLSTR

{
    APLDIM      aplDimAcc,
                aplDimCol;
    LPFMTROWSTR lpFmtRowLcl = NULL;

    // Initialize the product accumulator
    aplDimAcc = 1;

    // Loop through all but the last dimension
    for (aplDimCol = 0; aplDimCol < (aplRank - 1); aplDimCol++)
    {
        aplDimAcc *= lpMemDim[(aplRank - 2) - aplDimCol];
        if ((aplDimRow + 1) % aplDimAcc)
            break;

        // Insert a row of blanks
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
#ifdef DEBUG
        lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
        lpFmtRowLcl->lpFmtColUp  = lpFmtColStr;
#endif
        lpFmtRowLcl->uFmtRows    = 1;               // Initialize the count
        lpFmtRowLcl->bRealRow    = FALSE;           // Initialize as not a real row
        lpFmtRowLcl->bBlank      = TRUE;            // Initialize as a blank row
////////lpFmtRowLcl->lpFmtRowNxt = NULL;            // Initialize the ptr (filled in below)

        // Skip over the FMTROWSTR to the next available position
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Save as ptr to next row struc
        lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;
    } // End FOR

    return lpaplChar;
} // End CompileBlankRows


//***************************************************************************
//  $AppendBlankRows
//
//  Append blank rows via AppendLine
//***************************************************************************

void AppendBlankRows
    (APLRANK  aplRank,          // Arg rank
     APLDIM   aplDimRow,        // Arg row count
     LPAPLDIM lpMemDim,         // Ptr to arg dimensions
     LPUBOOL  lpbCtrlBreak)     // Ptr to Ctrl-Break flag

{
    APLDIM  aplDimAcc;
    APLRANK uRank;

    aplDimAcc = 1;
    for (uRank = 0; uRank < (aplRank - 1); uRank++)
    {
        aplDimAcc *= lpMemDim[(aplRank - 2) - uRank];
        if ((aplDimRow - 1) % aplDimAcc)
            break;

        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            return;

        AppendLine (L"", FALSE, TRUE);
    } // End FOR
} // End AppendBlankRows


//***************************************************************************
//  $FormatArrSimple
//
//  Format a simple array (this includes hetero) from its compiled form
//***************************************************************************

LPAPLCHAR FormatArrSimple
    (LPFMTHEADER lpFmtHeader,       // Ptr to FMTHEADER
     LPFMTCOLSTR lpFmtColStr,       // Ptr to vector of aplChrNCols FMTCOLSTRs
     LPAPLCHAR   lpaplChar2,        // Ptr to compiled input
     LPAPLCHAR  *lplpwszOut,        // Ptr to ptr to output string
     APLDIM      aplDimNRows,       // # formatted rows in this array
     APLDIM      aplDimNCols,       // # formatted cols ...
     APLDIM      aplLastDim,        // Length of the last dimension in the result
     APLRANK     aplRank,           // Rank of this array
     LPAPLDIM    lpMemDim,          // Ptr to this array's dimensions (NULL for scalar)
     APLSTYPE    aplType,           // Storage type of this array
     UBOOL       bNextRow,          // TRUE iff skip to next row after this item
     UBOOL       bRawOutput,        // TRUE iff raw (not {thorn}) output
     UBOOL       bEndingCR,         // TRUE iff last line has CR
     LPUBOOL     lpbCtrlBreak)      // Ptr to Ctrl-Break flag

{
    APLDIM      aplDimRow,          // Loop counter
                aplDimCol,          // Loop counter
                aplChrNCols,        // # cols for char arrays
                aplRealNRows,       // # real rows
                aplRealRow;         // Loop counter
    UINT        uActLen,            // Actual length
                uLead,              // # leading blanks
                uLeadBefore,        // # leading blanks before test for []PW
                uCol,               // Column #
                uCmpWid,            // Compiled width
                uColOff;            // Current row's col offset
    LPWCHAR     lpwszOut,           // Ptr to local output string
                lpwszOutStart;      // Ptr to starting output string
    LPFMTROWSTR lpFmtRowStr;        // Ptr to this item's FMTROWSTR
    LPAPLCHAR   lpaplChar = lpaplChar2; // Ptr to moving output string
    APLUINT     uQuadPW;            // []PW

    // Get the current value of []PW
    uQuadPW = GetQuadPW ();

    // Initialize local output string ptr
    lpwszOut = *lplpwszOut;

    // Initialize the # real rows
    aplRealNRows = lpFmtHeader->uRealRows;

    // Format char arrays as one col
    aplChrNCols = (IsSimpleChar (aplType)) ? 1 : aplDimNCols;

    // Loop through the formatted rows
    for (aplDimRow = aplRealRow = 0;
         aplDimRow < aplDimNRows;
         aplDimRow++)
    {
        UBOOL bRealRow;         // TRUE iff this is a real row

        // Point to the FMTROWSTR
        lpFmtRowStr = (LPFMTROWSTR) lpaplChar;

        // Validate the FMTROWSTR
        Assert (lpFmtRowStr->Sig.nature EQ FMTROWSTR_SIGNATURE);

        // It's a simple array
        Assert (lpFmtRowStr->uFmtRows EQ 1);

        // Accumulate the # real rows so far
        bRealRow = lpFmtRowStr->bRealRow;
        aplRealRow += bRealRow;

        // Save this row's col offset
        uColOff = lpFmtRowStr->uColOff;

        // Skip over the FMTROWSTR to the next available position
        lpaplChar = (LPAPLCHAR) &lpFmtRowStr[1];

        // Save starting output string ptr
        lpwszOutStart = lpwszOut;

        // Handle non-blank rows
        if (!lpFmtRowStr->bBlank)
        {
            // If this row's col offset is non-zero, fill with leading blanks
            if (uColOff)
                FillMemoryW (lpwszOut, uColOff, L' ');

            // Loop through the cols
            for (aplDimCol = 0; aplDimCol < aplChrNCols; aplDimCol++)
            {
                    LPWCHAR lpw;

////////////////// Split cases based upon the format type
////////////////switch (lpFmtColStr[aplDimCol].uFmtType)
////////////////{
////////////////    case FMTTYPE_INT:
////////////////    case FMTTYPE_FIX:
////////////////    case FMTTYPE_EXP:
                        // ***FIXME*** -- handle conversion from fixed point to exponential format
                        //                and line up the decimal points and the Es
                        uCmpWid  = InteriorColWidth (&lpFmtColStr[aplDimCol]);  // Compiled width
                        uActLen  = lstrlenW (lpaplChar);                        // Actual length

                        Assert (uActLen <= uCmpWid);

                        // Check for fractional part unless char ***FIXME*** -- Fails on '.' in hetero array??
                        if (IsSimpleChar (aplType))
                            lpw = NULL;
                        else
                            lpw = strchrW (lpaplChar, L'.');

                        if (!lpw)
                            lpw = lpaplChar + uActLen;              // Place decimal point after integer part

                        // Align the decimal points unless char ***FIXME*** -- Fails on '.' in hetero array??
                        if (IsSimpleChar (aplType))
                            uLead = 0;
                        else
                        {
                            uCol = (UINT) (lpw - lpaplChar) + lpFmtColStr[aplDimCol].uFrcs;
                            uLead = uCmpWid - max (uActLen, uCol);  // # leading blanks to align decimal points
                        } // End IF/ELSE

                        // Plus leading blanks
                        uLeadBefore = lpFmtColStr[aplDimCol].uLdBl;
                        uLead += uLeadBefore;

                        // If this is raw output,
                        // break the line if it would exceed []PW
                        //   and the line is non-empty.
                        uCol = (UINT) (lpwszOut - lpwszOutStart);
                        if (bRawOutput
                         && DEF_INDENT < uCol
                         && uQuadPW < (uLeadBefore + uCmpWid + uCol))
                        {
                            // Ensure properly terminated
                            *lpwszOut = L'\0';

                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                return NULL;

                            // Output the line
                            AppendLine (lpwszOutStart, TRUE, TRUE);

                            // Reset the line start
                            lpwszOut = lpw = *lplpwszOut;

                            // Fill the output area with all blanks
                            uCol = (UINT) aplLastDim - (*lplpwszOut - lpwszOutStart);
                            FillMemoryW (lpw, uCol, L' ');

                            // Skip over leading indent
                            lpwszOut += DEF_INDENT;

                            // Shorten the width to act like this is the first col
                            //   (which doesn't have a leading blank)
                            uCmpWid = max (uCmpWid, uActLen + 1) - 1;
                            uCol = uLeadBefore;
                        } else
                        {
                            // Include this row's col offset
                            uLead += uColOff;
#ifdef PREFILL
                            // Skip over leading blanks
                            lpwszOut += uLead;
                            uCol = uLead;
#else
                            // Fill with leading blanks
                            lpwszOut = FillMemoryW (lpwszOut, uLead, L' ');
#endif
                            // Subtract out as the col offset isn't in the compiled width
                            uCol -= uColOff;
                        } // End IF/ELSE
////////////////
////////////////        break;
////////////////
////////////////    defstop
////////////////        break;
////////////////} // End SWITCH

                // Copy the next value
                CopyMemory (lpwszOut, lpaplChar, uActLen * sizeof (APLCHAR));
                lpwszOut += uActLen;    // Skip over the formatted string
                uCol     += uActLen;    // ...

                // Include leading and trailing blanks
                uCmpWid += lpFmtColStr[aplDimCol].uLdBl + lpFmtColStr[aplDimCol].uTrBl;
#ifdef PREFILL
                // Skip over trailing blanks
                Assert (uCol <= uCmpWid);
                lpwszOut += (uCmpWid - uCol);
#else
                // Fill with trailing blanks
                lpwszOut = FillMemoryW (lpwszOut, uCmpWid - uCol, L' ');
#endif
                lpaplChar += (uActLen + 1);
            } // End FOR

#ifdef PREFILL
            // Skip over trailing blanks
            lpwszOut += lpFmtHeader->uFmtTrBl;
#else
            // Skip over trailing blanks
            lpwszOut = FillMemoryW (lpwszOut, lpFmtHeader->uFmtTrBl, L' ');
#endif
        } // End IF

        // If blank row or not last row or last row and requested to do so, ...
        if (lpFmtRowStr->bBlank
         || (aplDimRow NE (aplDimNRows - 1))
         || (bNextRow && aplDimRow EQ (aplDimNRows - 1)))
            // Skip to the start of the next row
            lpwszOut = lpwszOutStart + aplLastDim;

        // If this is raw output, output it
        if (bRawOutput)
        {
            // Handle blank lines between planes
            if (bRealRow && aplRealRow NE 1)        // Real row and not first row
                AppendBlankRows (aplRank, aplRealRow, lpMemDim, lpbCtrlBreak);
            // Ensure properly terminated
            *lpwszOut = L'\0';

            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpbCtrlBreak))
                return NULL;

            // Output the line
            AppendLine (lpwszOutStart, FALSE, bEndingCR || aplDimRow NE (aplDimNRows - 1));

            // Reset the line start
            lpwszOut = *lplpwszOut;

            // Fill the output area with all blanks
            uCol = (UINT) aplLastDim - (*lplpwszOut - lpwszOutStart);
            FillMemoryW (lpwszOut, uCol, L' ');
        } // End IF
    } // End FOR

    // Return the output string ptr
    *lplpwszOut = max (lpwszOut, *lplpwszOut);

    return lpaplChar;
} // End FormatArrSimple


//***************************************************************************
//  $FormatArrNested
//
//  Format a nested array from its compiled form
//***************************************************************************

LPAPLCHAR FormatArrNested
    (LPFMTHEADER lpFmtHeader,       // Ptr to FMTHEADER
     LPVOID      lpMem,             // Ptr to raw input
     LPFMTCOLSTR lpFmtColStr,       // Ptr to vector of <aplDimNCols> FMTCOLSTRs
     LPAPLCHAR   lpaplChar,         // Ptr to compiled input
     LPAPLCHAR  *lplpwszOut,        // Ptr to ptr to output string
     APLDIM      aplDimNRows,       // # formatted rows in this array
     APLDIM      aplDimNCols,       // # formatted cols ...
     APLRANK     aplRank,           // Rank of this array
     LPAPLDIM    lpMemDim,          // Ptr to this array's dimensions (NULL for scalar)
     APLDIM      aplLastDim,        // Length of the last dimension in the result
     UBOOL       bRawOutput,        // TRUE iff raw (not {thorn}) output
     UBOOL       bEndingCR,         // TRUE iff last line has CR
     LPUBOOL     lpbCtrlBreak)      // Ptr to Ctrl-Break flag

{
    LPFMTROWSTR lpFmtRowStr;        // Ptr to current FMTROWSTR
    APLDIM      aplDimRow,          // Loop counter
                aplDimCol;          // ...
    LPWCHAR     lpwszOut,           // Ptr to local output string
                lpwszOutStart;      // Ptr to starting position for each row
    APLSTYPE    aplType;            // STE storage type
    UINT        uPrvWid;            // Sum of the widths of previous columns

    // Initialize local output string ptr
    lpwszOut = *lplpwszOut;

    // Loop through the formatted rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Point to the FMTROWSTR
        lpFmtRowStr = (LPFMTROWSTR) lpaplChar;

        // Validate the FMTROWSTR
        Assert (lpFmtRowStr->Sig.nature EQ FMTROWSTR_SIGNATURE);

        // Skip over the FMTROWSTR to the next available position
        lpaplChar = (LPAPLCHAR) &lpFmtRowStr[1];

        // Save starting output string ptr
        lpwszOutStart = lpwszOut;

        // Handle non-blank rows
        if (!lpFmtRowStr->bBlank)
        {
            // Loop through the cols
            for (aplDimCol = uPrvWid = 0; aplDimCol < aplDimNCols; aplDimCol++, ((LPAPLHETERO) lpMem)++)
            {
                // Save starting output string ptr
                lpwszOut = lpwszOutStart;

                // Accumulate widths of previous cols
                if (aplDimCol > 0)
                    uPrvWid += ExteriorColWidth (&lpFmtColStr[aplDimCol - 1]);

                // Offset lpwszOut from the start by the width of previous cols
                lpwszOut += uPrvWid;

                // Split cases based upon the ptr type
                switch (GetPtrTypeInd (lpMem))
                {
                    case PTRTYPE_STCONST:
                        // Point to the FMTHEADER
                        lpFmtHeader = (LPFMTHEADER) lpaplChar;

                        // Validate the FMTHEADER
                        Assert (lpFmtHeader->Sig.nature EQ FMTHEADER_SIGNATURE);

                        // Get the array storage type
                        aplType = TranslateImmTypeToArrayType ((*(LPAPLHETERO) lpMem)->stFlags.ImmType);

                        lpaplChar =
                          FormatArrNestedCon (aplType,                      // Storage type of this array
                                              lpaplChar,                    // Ptr to compiled input
                                             &lpwszOut,                     // Ptr to ptr to output string
                                             &lpFmtColStr[aplDimCol],       // Ptr to the FMTCOLSTR for this column
                                              aplLastDim,                   // Length of the last dimension in the result
                                              lpbCtrlBreak);                // Ptr to Ctrl-Break flag
                        break;

                    case PTRTYPE_HGLOBAL:
                        lpaplChar =
                          FormatArrNestedGlb (ClrPtrTypeIndAsGlb (lpMem),   // The global memory handle whose contents are to be formatted
                                              lpaplChar,                    // Ptr to compiled input
                                             &lpwszOut,                     // Ptr to ptr to output string
                                             &lpFmtColStr[aplDimCol],       // Ptr to the FMTCOLSTR for this column
                                              aplLastDim,                   // Length of the last dimension in the result
                                              lpbCtrlBreak);                // Ptr to Ctrl-Break flag
                        break;

                    defstop
                        break;
                } // End SWITCH

                // Check for Ctrl-Break
                if (CheckCtrlBreak (*lpbCtrlBreak))
                    return NULL;

////////////////// Back off to start of row
////////////////lpwszOut -= uPrvWid;

                // Return the output string ptr
                *lplpwszOut = max (lpwszOut, *lplpwszOut);
            } // End FOR
#ifdef PREFILL
            // Skip to the start of the next row
            //   if we're not at the last row
            if (aplLastDim && aplDimRow NE (aplDimNRows - 1))
                lpwszOut = lpwszOutStart
                         + aplLastDim
                         * (((aplLastDim - 1)
                           + lpwszOut
                           - lpwszOutStart)
                          / aplLastDim);
            // Use the larger
            *lplpwszOut = max (lpwszOut, *lplpwszOut);
#else
            // Skip over trailing blanks
            lpwszOut = FillMemoryW (lpwszOut, lpFmtHeader->uFmtTrBl, L' ');
#endif
        } else
            // Skip over the blanks to the next row
            *lplpwszOut += aplLastDim;

        // Reset local ptr
        lpwszOut = *lplpwszOut;
    } // End FOR

    // Return the output string ptr
    *lplpwszOut = max (lpwszOut, *lplpwszOut);

    return lpaplChar;
} // End FormatArrNested


//***************************************************************************
//  $FormatArrNestedCon
//
//  Format a nested array constant from its compiled form
//***************************************************************************

LPAPLCHAR FormatArrNestedCon
    (APLSTYPE    aplType,       // Storage type of this array
     LPAPLCHAR   lpaplChar,     // Ptr to compiled input
     LPAPLCHAR  *lplpwszOut,    // Ptr to ptr to output string
     LPFMTCOLSTR lpFmtColStr,   // Ptr to the FMTCOLSTR for this column
     APLDIM      aplLastDim,    // Length of the last dimension in the result
     LPUBOOL     lpbCtrlBreak)  // Ptr to Ctrl-Break flag

{
    APLDIM   aplDimNRows,       // # formatted rows
             aplDimNCols;       // # formatted cols
    LPFMTHEADER lpFmtHeader;    // Ptr to this item's FMTHEADER
    LPFMTCOLSTR lpFmtColLcl;    // Ptr to this item's FMTCOLSTR

    // Point to the FMTHEADER
    lpFmtHeader = (LPFMTHEADER) lpaplChar;

    // Validate the FMTHEADER
    Assert (lpFmtHeader->Sig.nature EQ FMTHEADER_SIGNATURE);

    // Get the # actual rows and cols
    aplDimNCols = lpFmtHeader->uActCols;
    aplDimNRows = lpFmtHeader->uActRows;

    // Skip over the FMTHEADER  to the next available position
    lpaplChar = (LPAPLCHAR) &lpFmtHeader[1];

    // Point to the FMTCOLSTRs
    lpFmtColLcl = (LPFMTCOLSTR) lpaplChar;

#ifdef DEBUG
    // Validate the FMTCOLSTRs
    {
        APLDIM uCol;

        for (uCol = 0; uCol < aplDimNCols; uCol++)
            Assert (lpFmtColLcl[uCol].Sig.nature EQ FMTCOLSTR_SIGNATURE);
    }
#endif
    // Include the parent's leading and trailing blanks
    lpFmtColLcl->uLdBl = lpFmtColStr->uLdBl;
    lpFmtColLcl->uTrBl = lpFmtColStr->uTrBl;

    // Skip over the FMTCOLSTRs to the next available position
    lpaplChar = (LPAPLCHAR) &lpFmtColLcl[aplDimNCols];

    return
      FormatArrSimple (lpFmtHeader,         // Ptr to FMTHEADER
                       lpFmtColLcl,         // Ptr to vector of aplDimNCols FMTCOLSTRs
                       lpaplChar,           // Ptr to compiled input
                       lplpwszOut,          // Ptr to ptr to output string
                       aplDimNRows,         // # formatted rows in this array
                       aplDimNCols,         // # formatted cols in this array
                       aplLastDim,          // Length of last dim in result
                       0,                   // Rank of this array
                       NULL,                // Ptr to this array's dimensions
                       aplType,             // Storage type of this array
                       FALSE,               // TRUE iff skip to next row after this item
                       FALSE,               // TRUE iff raw output
                       TRUE,                // TRUE iff last line has CR
                       lpbCtrlBreak);       // Ptr to Ctrl-Break flag
} // End FormatArrNestedCon


//***************************************************************************
//  $FormatArrNestedGlb
//
//  Format a nested array global memory object from its compiled form
//***************************************************************************

LPAPLCHAR FormatArrNestedGlb
    (HGLOBAL     hGlb,          // The global memory handle whose contents are to be formatted
     LPAPLCHAR   lpaplChar,     // Ptr to compiled input
     LPAPLCHAR  *lplpwszOut,    // Ptr to ptr to output string
     LPFMTCOLSTR lpFmtColStr,   // Ptr to the FMTCOLSTR for this column
     APLDIM      aplLastDim,    // Length of the last dimension in the result
     LPUBOOL     lpbCtrlBreak)  // Ptr to Ctrl-Break flag

{
    APLSTYPE    aplType;        // This item's storage type
    APLRANK     aplRank;        // This item's rank
    LPVOID      lpMem;          // Ptr to this item's global memory
    LPAPLDIM    lpMemDim;       // Ptr to this item's dimensions
    APLDIM      aplDimNRows,    // # formatted rows
                aplDimNCols,    // # formatted cols
                uCol;           // Loop counter
    LPFMTHEADER lpFmtHeader;    // Ptr to this item's FMTHEADER
    LPFMTCOLSTR lpFmtColLcl;    // Ptr to this item's FMTCOLSTRs

    // Get the attributes (Type, NELM, Rank) of the global
    AttrsOfGlb (hGlb, &aplType, NULL, &aplRank, NULL);

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlb);

    // Skip over the header to the dimensions
    lpMemDim = VarArrayBaseToDim (lpMem);

    // Skip over the header and dimensions to the data
    lpMem = VarArrayBaseToData (lpMem, aplRank);

    // Point to the FMTHEADER
    lpFmtHeader = (LPFMTHEADER) lpaplChar;

    // Validate the FMTHEADER
    Assert (lpFmtHeader->Sig.nature EQ FMTHEADER_SIGNATURE);

    // Get the # actual rows and cols
    aplDimNCols = lpFmtHeader->uActCols;
    aplDimNRows = lpFmtHeader->uActRows;

    // Skip over the FMTHEADER to the next available position
    lpaplChar = (LPAPLCHAR) &lpFmtHeader[1];

    // Point to the FMTCOLSTRs
    lpFmtColLcl = (LPFMTCOLSTR) lpaplChar;

#ifdef DEBUG
    // Validate the FMTCOLSTRs
    for (uCol = 0; uCol < aplDimNCols; uCol++)
        Assert (lpFmtColLcl[uCol].Sig.nature EQ FMTCOLSTR_SIGNATURE);
#endif
    // Skip over the FMTCOLSTRs to the next available position
    lpaplChar = (LPAPLCHAR) &lpFmtColLcl[aplDimNCols];

    // Distribute this column's FMTCOLSTR (in lpFmtColStr)
    //   amongst the FMTCOLSTRs for this item (in lpaplChar)
    if (!IsZeroDim (aplDimNCols))
    {
        UINT uLclWid,           // Local column width
             uGlbWid;           // Global ...

        // Calculate the global interior width
        uGlbWid = InteriorColWidth (lpFmtColStr);

        // Calculate the local exterior width
        for (uLclWid = 0, uCol = 0; uCol < aplDimNCols; uCol++)
            uLclWid += ExteriorColWidth (&lpFmtColLcl[uCol]);

        // Less the leading and trailing blanks to get interior width
        if (aplDimNCols)
            uLclWid -= lpFmtColLcl[0              ].uLdBl
                     + lpFmtColLcl[aplDimNCols - 1].uTrBl;

        Assert (uLclWid <= uGlbWid);

        // Save the extra width as leading blanks,
        //   in effect right-justifying this item in the col
        lpFmtColLcl[0].uLdBl = lpFmtColStr->uLdBl + (uGlbWid - uLclWid);
    } // End IF

    // Split cases based upon the right arg's storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_CHAR:
        case ARRAY_APA:
        case ARRAY_HETERO:
            lpaplChar =
              FormatArrSimple (lpFmtHeader,     // Ptr to FMTHEADER
                               lpFmtColLcl,     // Ptr to vector of <aplDimNCols> FMTCOLSTRs
                               lpaplChar,       // Ptr to compiled input
                               lplpwszOut,      // Ptr to output string
                               aplDimNRows,     // # rows in this array
                               aplDimNCols,     // # cols in this array
                               aplLastDim,      // Length of last dim in result
                               aplRank,         // Rank of this array
                               lpMemDim,        // Ptr to this array's dimensions
                               aplType,         // Storage type of this array
                               FALSE,           // TRUE iff skip to next row after this item
                               FALSE,           // TRUE iff raw output
                               TRUE,            // TRUE iff last line has CR
                               lpbCtrlBreak);   // Ptr to Ctrl-Break flag
            break;

        case ARRAY_NESTED:
            lpaplChar =
              FormatArrNested (lpFmtHeader,     // Ptr to FMTHEADER
                               lpMem,           // Ptr to raw input
                               lpFmtColLcl,     // Ptr to vector of <aplDimNCols> FMTCOLSTRs
                               lpaplChar,       // Ptr to compiled input
                               lplpwszOut,      // Ptr to ptr to output string
                               aplDimNRows,     // # rows in this array
                               aplDimNCols,     // # cols ...
                               aplRank,         // Rank of this array
                               lpMemDim,        // Ptr to this array's dimensions
                               aplLastDim,      // Length of last dim in result
                               FALSE,           // TRUE iff raw (not {thorn}) output
                               TRUE,            // TRUE iff last line has CR
                               lpbCtrlBreak);   // Ptr to Ctrl-Break flag
            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlb); lpMem = NULL;

    return lpaplChar;
} // End FormatArrNestedGlb


//***************************************************************************
//  $PrimFnDydDownTackJot_EM_YY
//
//  Primitive function for dyadic DownTackJot
//     ("format by specification"/"format by example")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydDownTackJot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydDownTackJot_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeLft,        // Left arg storage type
                 aplTypeRht,        // Right ...
                 aplTypeSubRht;     // Right arg item ...
    APLNELM      aplNELMLft,        // Left arg NELM
                 aplNELMRht,        // Right ...
                 aplNELMRes;        // Result   ...
    APLRANK      aplRankLft,        // Left arg rank
                 aplRankRht,        // Right ...
                 aplRankSubRht,     // Right arg item ...
                 aplRankRes;        // Result   ...
    APLDIM       aplDimNCols,       // # columns
                 aplDimNRows,       // # rows
                 aplDimCol,         // Loop counter
                 aplDimRow,         // ...
                 aplColsRht;        // Right arg # cols
    HGLOBAL      hGlbLft = NULL,    // Left arg global memory handle
                 hGlbRht = NULL,    // Right ...
                 hGlbRes = NULL,    // Result   ...
                 hGlbWidPrc = NULL, // Left arg WIDPRC struct ...
                 hGlbItmRht = NULL; // Right arg item ...
    LPVOID       lpMemLft = NULL,   // Ptr to left arg global memory
                 lpMemRht = NULL;   // Ptr to right ...
    LPAPLCHAR    lpMemRes = NULL;   // Ptr to result   ...
    LPAPLDIM     lpMemDimRht = NULL;// Ptr to right arg dimensions
    APLLONGEST   aplLongestLft,     // Left arg immediate value
                 aplLongestRht;     // Right ...
    LPWIDPRC     lpMemWidPrc = NULL;// Ptr to left arg WIDPRC struc
    LPAPLCHAR    lpaplChar,         // Ptr to next available format position
                 lpaplCharIni;      // Ptr to initial format position
    APLINT       apaOffLft,         // Left arg APA offset
                 apaMulLft,         // ...          multiplier
                 aplIntegerLft,     // Left arg temporary integer
                 iPrc;              // Precision ...
    APLUINT      ByteRes,           // # bytes to allocate
                 uDim,              // Loop counter
                 uRht,              // Loop counter
                 uPar,              // Parity indicator
                 uTotWid,           // Total width for this col
                 uAccWid,           // Accumulated width for this col
                 uWid;              // Width for this col
    IMM_TYPES    immTypeRht;        // Right arg element immediate type
    APLCHAR      aplCharDecimal,    // []FC[FCNDX_DECIMAL_SEP]
                 aplCharOverflow,   // []FC[FCNDX_OVERFLOW_FILL]
                 aplCharOverbar;    // []FC[FCNDX_OVERBAR]
    UINT         uBitMask,          // Bit mask for looping through Booleans
                 uLen;              // Length of formatted number
    UBOOL        bRet = TRUE,       // TRUE iff result is valid
                 Auto;              // TRUE iff the col is automatic width
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to result
    HGLOBAL      hGlbPTD;           // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPWCHAR      lpwszFormat;       // Ptr to formatting save area

    // Get the PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get ptr to formatting save area
    lpwszFormat = lpMemPTD->lpwszFormat;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, &aplColsRht);

    // Check for LEFT RANK ERROR
    if (IsMultiRank (aplRankLft))
        goto RANK_EXIT;

    // Check for "format by example"
    if (IsSimpleChar (aplTypeLft))
        return PrimFnDydDownTackJotFBE_EM_YY (lptkLftArg,   // Ptr to left arg token
                                              lptkFunc,     // Ptr to function token
                                              lptkRhtArg,   // Ptr to right arg token
                                              lptkAxis);    // Ptr to axis token (may be NULL)
    // Continue with "format by specification"

    // Check for LEFT LENGTH ERROR
    if (!IsSingleton (aplNELMLft)           // Single number
     && aplNELMLft NE 2                     // Pair of numbers
     && (0 NE (aplNELMLft % 2)              // Pairs of numbers
      || ((aplNELMLft / 2) NE aplColsRht))) // One pair per right arg col
        goto LENGTH_EXIT;

    // Check for LEFT DOMAIN ERROR
    if (!IsSimpleNum (aplTypeLft))
        goto DOMAIN_EXIT;

    // Get left and right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Allocate temp storage for the normalized left arg
    // N.B.: Conversion from APLUINT to UINT
    ByteRes = aplColsRht * sizeof (WIDPRC);
    Assert (ByteRes EQ (__int3264) ByteRes);
    hGlbWidPrc = DbgGlobalAlloc (GHND, (__int3264) ByteRes);
    if (!hGlbWidPrc)
        goto WSFULL_EXIT;

    // Copy left arg to temp storage (as WIDPRCs),
    //   and check the left arg for valid values

    // Lock the memory to get a ptr to it
    lpMemWidPrc = MyGlobalLock (hGlbWidPrc);

    if (hGlbLft)
        // Skip over the header to the data
        lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);

    // Handle singleton left arg
    if (IsSingleton (aplNELMLft))
    {
        // Get the first value from the token
        GetFirstValueToken (lptkLftArg,         // Ptr to the token
                            NULL,               // Ptr to the integer (or Boolean) (may be NULL)
                            NULL,               // ...        float (may be NULL)
                            NULL,               // ...        char (may be NULL)
                           &aplLongestLft,      // ...        longest (may be NULL)
                            NULL,               // ...        LPSYMENTRY or HGLOBAL (may be NULL)
                            NULL,               // ...        immediate type (see IMM_TYPES) (may be NULL)
                            NULL);              // ...        array type:  ARRAY_TYPES (may be NULL)
        // If it's float, ...
        if (IsSimpleFlt (aplTypeLft))
            // Attempt to convert the float to an integer using System CT
            aplLongestLft = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestLft, &bRet);
        if (!bRet)
            goto DOMAIN_EXIT;

        // Save as actual precision
        lpMemWidPrc[0].iPrc = aplLongestLft;
        lpMemWidPrc[0].Auto = TRUE;
    } else
    // Ensure that the first value in each pair is non-negative
    // Split cases based upon the left arg's storage type
    switch (aplTypeLft)
    {
        case ARRAY_BOOL:
            uBitMask = 0x01;

            for (uDim = uPar = 0; uDim < aplNELMLft; uDim++, uPar = 1 - uPar)
            {
                aplIntegerLft = (uBitMask & *(LPAPLBOOL) lpMemLft) ? TRUE : FALSE;
////////////////if ((!uPar) && aplIntegerLft < 0)   // Not needed on Booleans
////////////////    goto DOMAIN_EXIT;
                *((LPAPLINT) lpMemWidPrc)++ = aplIntegerLft;

                // Shift over the left bit mask
                uBitMask <<= 1;

                // Check for end-of-byte
                if (uBitMask EQ END_OF_BYTE)
                {
                    uBitMask = 0x01;            // Start over
                    ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                } // End IF
            } // End FOR

            break;

        case ARRAY_INT:
            for (uDim = uPar = 0; uDim < aplNELMLft; uDim++, uPar = 1 - uPar)
            {
                aplIntegerLft = *((LPAPLINT) lpMemLft)++;
                if ((!uPar) && aplIntegerLft < 0)
                    goto DOMAIN_EXIT;
                *((LPAPLINT) lpMemWidPrc)++ = aplIntegerLft;
            } // End FOR

            break;

        case ARRAY_FLOAT:
            for (uDim = uPar = 0; uDim < aplNELMLft; uDim++, uPar = 1 - uPar)
            {
                // Attempt to convert the float to an integer using System CT
                aplIntegerLft = FloatToAplint_SCT (*((LPAPLFLOAT) lpMemLft)++, &bRet);
                if ((!bRet) || ((!uPar) && aplIntegerLft < 0))
                    goto DOMAIN_EXIT;
                *((LPAPLINT) lpMemWidPrc)++ = aplIntegerLft;
            } // End FOR

            break;

        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemLft)
            // Get the APA parameters
            apaOffLft = lpAPA->Off;
            apaMulLft = lpAPA->Mul;
#undef  lpAPA
            for (uDim = uPar = 0; uDim < aplNELMLft; uDim++, uPar = 1 - uPar)
            {
                aplIntegerLft = apaOffLft + apaMulLft * uDim;
                if ((!uPar) && aplIntegerLft < 0)
                    goto DOMAIN_EXIT;
                *((LPAPLINT) lpMemWidPrc)++ = aplIntegerLft;
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH

    if (hGlbLft)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    // Restore lpMemWidPrc to the start of the block
    MyGlobalUnlock (hGlbWidPrc); lpMemWidPrc = NULL;
    lpMemWidPrc = MyGlobalLock (hGlbWidPrc);

    // If there's only one pair and more than one right arg col,
    //    spread the pair through lpMemWidPrc
    if (aplNELMLft <= 2
     && aplColsRht > 1)
    {
        // Get the corresponding attributes for this col
        uWid = lpMemWidPrc[0].uWid;
        iPrc = lpMemWidPrc[0].iPrc;

        for (uRht = 1; uRht < aplColsRht; uRht++)
        {
            // Save in temporary left arg
            lpMemWidPrc[uRht].uWid = uWid;
            lpMemWidPrc[uRht].iPrc = iPrc;
        } // End FOR
    } // End IF

    // Set the Auto bit if the width is zero
    for (uRht = 0; uRht < aplColsRht; uRht++)
        lpMemWidPrc[uRht].Auto = (lpMemWidPrc[uRht].uWid EQ 0);

    if (lpMemRht)
        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    // Get the # rows & columns in the right arg
    if (IsScalar (aplRankRht))
        aplDimNCols = aplDimNRows = 1;
    else
    {
        aplDimNCols = lpMemDimRht[aplRankRht - 1];

        // Get the # rows (across all planes)
        if (IsVector (aplRankRht)
         || IsZeroDim (aplDimNCols))
            aplDimNRows = 1;
        else
            aplDimNRows = aplNELMRht / aplDimNCols;
    } // End IF/ELSE

    if (lpMemRht)
        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    // Validate the right arg as either simple (including heterogeneous) or
    //   nested consisting of numeric scalars and/or
    //   simple character scalars or vectors
    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_CHAR:
        case ARRAY_APA:
            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Loop through the right arg
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Split cases based upon the right arg item ptr type
                switch (GetPtrTypeDir (((LPAPLNESTED) lpMemRht)[uRht]))
                {
                    case PTRTYPE_STCONST:
                        break;

                    case PTRTYPE_HGLOBAL:
                        // Get the attributes of the global memory handle
                        AttrsOfGlb (((LPAPLNESTED) lpMemRht)[uRht],
                                   &aplTypeSubRht,
                                    NULL,
                                   &aplRankSubRht,
                                    NULL);
                        // Check for RANK ERROR
                        switch (aplTypeSubRht)
                        {
                            case ARRAY_BOOL:
                            case ARRAY_INT:
                            case ARRAY_FLOAT:
                            case ARRAY_APA:
                                // Numeric scalars only
                                if (!IsScalar (aplRankSubRht))
                                    goto DOMAIN_EXIT;
                                break;

                            case ARRAY_CHAR:
                                // Character scalars or vectors only
                                if (IsMultiRank (aplRankSubRht))
                                    goto DOMAIN_EXIT;
                                break;

                            case ARRAY_HETERO:
                            case ARRAY_NESTED:
                                goto DOMAIN_EXIT;

                                break;

                            defstop
                                break;
                        } // End SWITCH

                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH

    // The result rank is at least a vector
    aplRankRes = max (aplRankRht, 1);

    // Initialize the output save area ptr
    lpaplChar = lpwszFormat;

    // Get the []FC values we need
    aplCharDecimal  = GetQuadFCValue (FCNDX_DECIMAL_SEP);
    aplCharOverflow = GetQuadFCValue (FCNDX_OVERFLOW_FILL);
    aplCharOverbar =  GetQuadFCValue (FCNDX_OVERBAR);

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
        case ARRAY_FLOAT:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Loop through the right arg col by col
            //   formatting the values into lpwszFormat
            //   and accumulating the widths
            for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
            {
                // Get the corresponding attributes for this col
                Auto = (BOOL) lpMemWidPrc[aplDimCol].Auto;
                uWid = lpMemWidPrc[aplDimCol].uWid;
                iPrc = lpMemWidPrc[aplDimCol].iPrc;

                // Loop through all rows (and across planes)
                for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
                {
                    // Save the initial position
                    lpaplCharIni = lpaplChar;

                    // Get the next value from the right arg
                    if (lpMemRht)
                        GetNextValueMem (lpMemRht,
                                         aplTypeRht,
                                         aplDimCol + aplDimRow * aplDimNCols,
                                        &hGlbItmRht,
                                        &aplLongestRht,
                                        &immTypeRht);
                    else
                        immTypeRht = TranslateArrayTypeToImmType (aplTypeRht);

                    // If the item is an HGLOBAL, ...
                    if (hGlbItmRht)
                    {
                        LPAPLCHAR lpMemItmRht;
                        APLNELM   aplNELMItmRht;
                        APLRANK   aplRankItmRht;

                        // The item must be a char vector

                        // Lock the memory to get a ptr to it
                        lpMemItmRht = MyGlobalLock (hGlbItmRht);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemItmRht)
                        // Get the array parameters
                        aplNELMItmRht = lpHeader->NELM;
                        aplRankItmRht = lpHeader->Rank;
#undef  lpHeader
                        // Skip over the header to the data
                        lpMemItmRht = VarArrayBaseToData (lpMemItmRht, aplRankItmRht);

                        // Copy the data to the format area
                        CopyMemory (lpaplChar, lpMemItmRht, (__int3264) aplNELMItmRht * sizeof (APLCHAR));

                        // Skip over the copied data
                        lpaplChar += aplNELMItmRht;

                        // Ensure it's properly terminated
                        *lpaplChar++ = L'\0';

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbItmRht); lpMemItmRht = NULL;
                    } else
                    // Split cases based upon the immediate type
                    switch (immTypeRht)
                    {
                        case IMMTYPE_BOOL:
                        case IMMTYPE_INT:
                            // Split cases based upon the sgn of iPrc
                            if (iPrc >= 0)
                            {
                                // Format the number
                                lpaplChar =
                                  FormatAplintFC (lpaplChar,        // Ptr to output save area
                                                  aplLongestRht,    // The value to format
                                                  aplCharOverbar);  // Char to use as overbar
                                // Zap the trailing blank
                                lpaplChar[-1] = L'\0';

                                if (iPrc)
                                {
                                    // Append decimal separator followed by iPrc 0s
                                    lpaplChar[-1] = aplCharDecimal;
                                    FillMemoryW (lpaplChar, (__int3264) iPrc, L'0');
                                    lpaplChar += iPrc;
                                    *lpaplChar++ = L'\0';
                                } // End IF
                            } else
                            if (iPrc < 0)
                            {
                                // ***FIXME*** -- We need a way to force the format to E-notation if iPrc < 0



                                // Format the number
                                lpaplChar =
                                  FormatFloatFC (lpaplChar,                         // Ptr to output save area
                                                 (APLFLOAT) (APLINT) aplLongestRht, // The value to format
                                                 -iPrc,                             // Precision to use
                                                 aplCharDecimal,                    // Char to use as decimal separator
                                                 aplCharOverbar,                    // Char to use as overbar
                                                 DEF_DTOA_MODE);                    // DTOA mode (Mode 2: max (ndigits, 1))
                            } // End IF/ELSE

                            break;

                        case IMMTYPE_FLOAT:
                            // ***FIXME*** -- We need a way to force the format to E-notation if iPrc < 0



                            // Format the number
                            lpaplChar =
                              FormatFloatFC (lpaplChar,                             // Ptr to output save area
                                            *(LPAPLFLOAT) &aplLongestRht,           // The value to format
                                             abs64 (iPrc),                          // Precision to use
                                             aplCharDecimal,                        // Char to use as decimal separator
                                             aplCharOverbar,                        // Char to use as overbar
                                             DEF_DTOA_MODE);                        // DTOA mode (Mode 2: max (ndigits, 1))
                            // If iPrc is positive, ensure there are at least that many
                            //   digits to the right of the decimal point -- pad with zeros
                            //   if not.





                            // Zap the trailing blank
                            lpaplChar[-1] = L'\0';

                            break;

                        case IMMTYPE_CHAR:
                            *lpaplChar++ = (APLCHAR) aplLongestRht;
                            *lpaplChar++ = L'\0';

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    // Get the formatted length
                    uLen = (UINT) (lpaplChar - lpaplCharIni) - 1;

                    // Check for automatic width
                    if (Auto)
                        lpMemWidPrc[aplDimCol].uWid = uWid = max (uWid, uLen);
                    else
                    // Check for width overflow
                    if (uWid < uLen)
                    {
                        if (aplCharOverflow EQ L'0')
                            goto DOMAIN_EXIT;

                        FillMemoryW (lpaplCharIni, (__int3264) uWid, aplCharOverflow);
                        lpaplChar = lpaplCharIni + uWid;
                        *lpaplChar++ = L'\0';
                    } // End IF/ELSE/...
                } // End FOR
            } // End FOR

            break;

        case ARRAY_CHAR:
            // Pass the right arg on as the result
            hGlbRes = CopySymGlbDirAsGlb (hGlbRht);

            goto YYALLOC_EXIT;

        defstop
            break;
    } // End SWITCH

    // Calculate the total width, including extra column for auto width
    for (uRht = uTotWid = 0; uRht < aplColsRht; uRht++)
        uTotWid += lpMemWidPrc[uRht].Auto + lpMemWidPrc[uRht].uWid;

    // Calculate the result NELM
    aplNELMRes = aplDimNRows * uTotWid;

    // Calculate the space needed for the result
    ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRes, aplRankRes);

    // Allocate space for the result
    Assert (ByteRes EQ (__int3264) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (__int3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Skip over the header to the dimensions
    (LPVOID) lpMemRes = VarArrayBaseToDim (lpMemRes);

    // Fill in the dimensions
    if (IsMatrix (aplRankRes))
        *((LPAPLDIM) lpMemRes)++ = aplDimNRows;
    *((LPAPLDIM) lpMemRes)++ = uTotWid;

    // Initialize the output save area ptr
    lpaplChar = lpwszFormat;

    // Fill in the data

    // Fill the result with blanks
    FillMemoryW (lpMemRes, (__int3264) aplNELMRes, L' ');

    // Loop through the formatted data and copy it to the result
    for (aplDimCol = uAccWid = 0; aplDimCol < aplDimNCols; aplDimCol++, uAccWid += Auto + uWid)
    {
        // Get the corresponding attributes for this col
        Auto = (BOOL) lpMemWidPrc[aplDimCol].Auto;
        uWid = lpMemWidPrc[aplDimCol].uWid;
////////iPrc = lpMemWidPrc[aplDimCol].iPrc;

        // Loop through all rows (and across planes)
        for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
        {
            // Get the string length
            uLen = lstrlenW (lpaplChar);

            // ***FIXME*** -- Align the decimal points
            // ***FIXME*** -- If the col is entirely chars, left-justify the items

            // Copy the next formatted value to the result,
            //   right-justifying it in the process
            CopyMemory (&lpMemRes[aplDimRow * uTotWid + uAccWid + Auto + (uWid - uLen)],
                         lpaplChar,
                         uLen * sizeof (APLCHAR));
            // Skip over the formatted value and the trailing zero
            lpaplChar += uLen + 1;
        } // End FOR
    } // End FOR
YYALLOC_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (hGlbWidPrc)
    {
        if (lpMemWidPrc)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbWidPrc); lpMemWidPrc = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbWidPrc); hGlbWidPrc = NULL;
    } // End IF

    if (hGlbLft && lpMemLft)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    return lpYYRes;
} // End PrimFnDydDownTackJot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydDownTackJotFBE_EM_YY
//
//  Primitive function for dyadic DownTackJot ("format by example")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydDownTackJotFBE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydDownTackJotFBE_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnNonceError_EM (lptkFunc);

    DbgBrk ();          // ***FINISHME*** -- PrimFnDydDownTackJot_EM_YY ("format by example")






} // End PrimFnDydDownTackJotFBE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_dtackjot.c
//***************************************************************************
