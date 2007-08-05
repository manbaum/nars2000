//***************************************************************************
//  NARS2000 -- Primitive Function -- DownTackJot
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "termcode.h"
#include "externs.h"
#include "display.h"
#include "pertab.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $PrimFnDownTackJot_EM_YY
//
//  Primitive function for monadic and dyadic DownTackJot ("default format" and "format by example")
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

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
    {
        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                   lptkAxis);
        return NULL;
    } // End IF

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonDownTackJot_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydDownTackJot_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
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
    APLSTYPE     aplTypeRht;        // Right arg storage type
    APLNELM      aplNELMRht,        // Right arg NELM
                 aplNELMRes;        // Result    ...
    APLRANK      aplRankRht,        // Right arg rank
                 aplRankRes;        // Result    ...
    APLDIM       aplDimNCols,       // # columns
                 aplDimNRows,       // # rows
                 aplDimCol,         // Col loop counter
                 aplLastDim;        // Length of the last dimension in the result
    HGLOBAL      hGlbRht = NULL,    // Right arg global memory handle
                 hGlbRes = NULL;    // Result    ...
    LPVOID       lpMemRht = NULL,   // Ptr to right arg global memory
                 lpMemRes = NULL;   // Ptr to result    ...
    LPAPLDIM     lpMemDimRht = NULL;// Ptr to right arg dimensions
    APLINT       aplIntegerRht;     // Right arg temporary integer
    APLFLOAT     aplFloatRht;       // ...                 float
    APLCHAR      aplCharRht;        // ...                 char
    LPFMTHEADER  lpFmtHeader;       // Ptr to format header struc
    LPFMTCOLSTR  lpFmtColStr;       // Ptr to col struc
    LPAPLCHAR    lpaplChar,         // Ptr to output save area
                 lpaplCharStart;    // Ptr to start of output save area
    APLUINT      ByteRes;           // # bytes needed for the result
    BOOL         bRet = TRUE,       // TRUE iff result is valid
                 bSimpleScalar;     // TRUE if right arg is a simple scalar
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    // Get the attributes (Type, NELM, and Rank) of the right args
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Get right arg's global ptrs
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    if (lpMemRht)
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    // Get the # rows & columns in the right arg
    if (aplRankRht EQ 0)
        aplDimNCols = aplDimNRows = 1;
    else
    {
        aplDimNCols = lpMemDimRht[aplRankRht - 1];

        // Get the # rows (across all planes)
        if (aplRankRht EQ 1
         || aplDimNCols EQ 0)
            aplDimNRows = 1;
        else
            aplDimNRows = aplNELMRht / aplDimNCols;
    } // End IF/ELSE

    // Skip over the header and dimensions to the data
    if (lpMemRht)
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

#ifdef DEBUG
    // Fill lpwszFormat with FFs so we can tell what we actually wrote
    FillMemory (lpwszFormat, 1024, 0xFF);
#endif

    // Create a new FMTHEAD in the output
    ZeroMemory ((LPFMTHEADER) lpwszFormat, sizeof (FMTHEADER));
    ((LPFMTHEADER) lpwszFormat)->lpFmtHeadUp = NULL;
    lpFmtHeader = (LPFMTHEADER) lpwszFormat;
#ifdef DEBUG
    lpFmtHeader->Sig.nature  = FMTHEADER_SIGNATURE;
#endif
////lpFmtHeader->lpFmtRowUp  = NULL;                // No parent row struct
////lpFmtHeader->lpFmtColUp  = NULL;                // ...       col ...
////lpFmtHeader->lpFmtRow1st =                      // Filled in below
////lpFmtHeader->lpFmtCol1st =                      // ...
    lpFmtHeader->uRows       = (UINT) aplDimNRows;
    lpFmtHeader->uCols       = (UINT) aplDimNCols;
////lpFmtHeader->uFmtRows    = 0;                   // Filled in by ZeroMemory
////lpFmtHeader->uFmtInts    = 0;                   // ...
////lpFmtHeader->uFmtFrcs    = 0;                   // ...
////lpFmtHeader->uFmtTrBl    = FALSE;               // ...
////lpFmtHeader->uDepth      = 0;                   // ...

    // Create <aplDimNCols> FMTCOLSTRs in the output
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
    // Skip over the FMTCOLSTRs
    lpaplChar = lpaplCharStart = (LPAPLCHAR) &lpFmtColStr[aplDimNCols];

    // Save ptr to 1st child FMTROWSTR
    if (aplDimNRows)
        lpFmtHeader->lpFmtRow1st = (LPFMTROWSTR) lpaplChar;
    else
        lpFmtHeader->lpFmtRow1st = NULL;

    // Loop through the array appending the formatted values (separated by L'\0')
    //   to the output vector, and accumulating the values in the appropriate
    //   FMTCOLSTR & FMTROWSTR entries.

    // If the right arg is a simple scalar,
    //   get its value
    bSimpleScalar = (aplRankRht EQ 0 && IsSimpleNH (aplTypeRht));
    if (bSimpleScalar)
        FirstValue (lptkRhtArg,         // Ptr to right arg token
                   &aplIntegerRht,      // Ptr to integer result
                   &aplFloatRht,        // Ptr to float ...
                   &aplCharRht,         // Ptr to WCHAR ...
                    NULL,               // Ptr to longest ...
                    NULL,               // Ptr to lpSym/Glb ...
                    NULL,               // Ptr to ...immediate type ...
                    NULL);              // Ptr to array type ...
    // Split cases based upon the right arg's storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
////////////lpaplChar =
            CompileArrBool    (bSimpleScalar ? (LPAPLBOOL) &aplIntegerRht
                                             : (LPAPLBOOL)   lpMemRht,  // Ptr to right arg memory
                               lpFmtHeader,                             // Ptr to parent header
                               lpFmtColStr,                             // Ptr to vector of ColStrs
                               lpaplChar,                               // Ptr to compiled output
                               aplDimNRows,                             // # rows
                               aplDimNCols,                             // # cols
                               aplRankRht,                              // Right arg rank
                               lpMemDimRht,                             // Ptr to right arg dimensions
                               FALSE);                                  // TRUE iff nested
            break;

        case ARRAY_INT:
////////////lpaplChar =
            CompileArrInteger (bSimpleScalar ? &aplIntegerRht
                                             : (LPAPLINT)    lpMemRht,  // Ptr to right arg memory
                               lpFmtHeader,                             // Ptr to parent header
                               lpFmtColStr,                             // Ptr to vector of ColStrs
                               lpaplChar,                               // Ptr to compiled output
                               aplDimNRows,                             // # rows
                               aplDimNCols,                             // # cols
                               aplRankRht,                              // Right arg rank
                               lpMemDimRht,                             // Ptr to right arg dimensions
                               FALSE);                                  // TRUE iff nested
            break;

        case ARRAY_FLOAT:
////////////lpaplChar =
            CompileArrFloat   (bSimpleScalar ? &aplFloatRht
                                             : (LPAPLFLOAT)  lpMemRht,  // Ptr to right arg memory
                               lpFmtHeader,                             // Ptr to parent header
                               lpFmtColStr,                             // Ptr to vector of ColStrs
                               lpaplChar,                               // Ptr to compiled output
                               aplDimNRows,                             // # rows
                               aplDimNCols,                             // # cols
                               aplRankRht,                              // Right arg rank
                               lpMemDimRht,                             // Ptr to right arg dimensions
                               FALSE);                                  // TRUE iff nested
            break;

        case ARRAY_CHAR:
            // Return the right arg

            // If the right arg is scalar,
            //   return it as an immediate
            if (aplRankRht EQ 0)
            {
                // Allocate a new YYRes
                lpYYRes = YYAlloc ();

                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
                lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_CHAR;
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkChar     = aplCharRht;
                lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

                goto QUICK_EXIT;
            } else
            {
                // Make a copy of the right arg
                hGlbRes = CopySymGlbDirGlb (hGlbRht);

                goto NORMAL_EXIT;
            } // End IF/ELSE

        case ARRAY_APA:
////////////lpaplChar =
            CompileArrAPA     ((LPAPLAPA)    lpMemRht,      // Ptr to right arg memory
                               lpFmtHeader,                 // Ptr to parent header
                               lpFmtColStr,                 // Ptr to vector of ColStrs
                               lpaplChar,                   // Ptr to compiled output
                               aplDimNRows,                 // # rows
                               aplDimNCols,                 // # cols
                               aplRankRht,                  // Right arg rank
                               lpMemDimRht,                 // Ptr to right arg dimensions
                               FALSE);                                  // TRUE iff nested
            break;

        case ARRAY_HETERO:
////////////lpaplChar =
            CompileArrHetero  ((LPAPLHETERO) lpMemRht,      // Ptr to right arg memory
                               lpFmtHeader,                 // Ptr to parent header
                               lpFmtColStr,                 // Ptr to vector of ColStrs
                               lpaplChar,                   // Ptr to compiled output
                               aplDimNRows,                 // # rows
                               aplDimNCols,                 // # cols
                               aplRankRht,                  // Right arg rank
                               lpMemDimRht,                 // Ptr to right arg dimensions
                               FALSE);                                  // TRUE iff nested
            break;

        case ARRAY_NESTED:
////////////lpaplChar =
            CompileArrNested  ((LPAPLNESTED) lpMemRht,      // Ptr to right arg memory
                               lpFmtHeader,                 // Ptr to parent header
                               lpFmtColStr,                 // Ptr to vector of ColStrs
                               lpaplChar,                   // Ptr to compiled output
                               aplDimNRows,                 // # rows
                               aplDimNCols,                 // # cols
                               aplRankRht,                  // Right arg rank
                               lpMemDimRht,                 // Ptr to right arg dimensions
                               FALSE);                                  // TRUE iff nested
            break;

        defstop
            break;
    } // End SWITCH

    // Set matrix result value
    if (aplRankRht > 1)
        lpFmtHeader->uMatRes = TRUE;

    // Propogate the row & col count up the line
    PropogateRowColCount (lpFmtHeader,
                          FALSE);

    // CHARs are handled above

    // For nested arrays, the ran of the result is one
    //   if all items are scalars or vectors, otherwise
    //   the rank is two.
    // For non-CHAR simple arrays, the rank of the result
    //   is the larger of the rank of the right arg and one.
    if (aplTypeRht EQ ARRAY_NESTED)
    {
        aplRankRes = 1 + lpFmtHeader->uMatRes;
    } else
        aplRankRes = max (aplRankRht, 1);

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
    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRes, aplRankRes);

    //***************************************************************
    // Now we can allocate the storage for the result
    // N.B.:  Conversion from APLUINT to UINT.
    //***************************************************************
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)

    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->Perm       = 0;           // Already zero from GHND
////lpHeader->SysVar     = 0;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;

#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);

    // If this is a nested array and the rank
    //   of the result is two, fill in the
    //   next-to-the-last dimension
    if (aplTypeRht EQ ARRAY_NESTED
     && aplRankRes EQ 2)
        // Fill in the # rows
        *((LPAPLDIM) lpMemRes)++ = lpFmtHeader->uFmtRows;
    else
    {
        // Fill in all but the last dimension
        if (lpMemDimRht && aplRankRht NE 0)
        for (aplDimCol = 0; aplDimCol < (aplRankRht - 1); aplDimCol++)
            *((LPAPLDIM) lpMemRes)++ = *lpMemDimRht++;
    } // End IF

    // Fill in the last dimension
    *((LPAPLDIM) lpMemRes)++ = aplLastDim;

    // lpMemRes now points to the result's data
#ifdef PREFILL
    // Fill it with all blanks
    for (lpaplChar = lpMemRes, aplDimCol = 0; aplDimCol < aplNELMRes; aplDimCol++)
        *lpaplChar++ = L' ';
#endif

#define lpwszOut    ((LPAPLCHAR) lpMemRes)

    // Output the result at appropriate widths

    // Run through the right arg again processing the
    //   output stream into lpMemRes

    // Split cases based upon the right arg's storage type
    switch (aplTypeRht)
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
                            &lpwszOut,              // Ptr to output string
                             aplDimNRows,           // # rows in this array
                             aplDimNCols,           // # cols in this array
                             aplLastDim,            // Length of last dim in result
                             aplRankRht,            // Rank of this array
                             lpMemDimRht,           // Ptr to this array's dimensions
                             aplTypeRht,            // Storage type of this array
                             FALSE,                 // TRUE iff raw output
                             TRUE);                 // TRUE iff last line has CR
            break;

        case ARRAY_NESTED:
////////////lpaplChar =
            FormatArrNested (lpFmtHeader,           // Ptr to FMTHEADER
                             lpMemRht,              // Ptr to raw input
                             lpFmtColStr,           // Ptr to vector of <aplDimNCols> FMTCOLSTRs
                             lpaplCharStart,        // Ptr to compiled input
                            &lpwszOut,              // Ptr to ptr to output string
                             aplDimNRows,           // # rows in this array
                             aplDimNCols,           // # cols ...
                             aplRankRht,            // Rank of this array
                             lpMemDimRht,           // Ptr to this array's dimensions
                             aplLastDim,            // Length of last dim in result
                             FALSE,                 // TRUE iff raw (not {thorn}) output
                             TRUE);                 // TRUE iff last line has CR
            break;

        defstop
            break;
    } // End SWITCH

#undef  lpwszOut

NORMAL_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
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

    if (bRet)
        return lpYYRes;
    else
        return NULL;
} // End PrimFnMonDownTackJot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $FormatArrSimple
//
//  Format a simple array (this includes hetero) from its compiled form
//***************************************************************************

LPAPLCHAR FormatArrSimple
    (LPFMTHEADER lpFmtHeader,   // Ptr to FMTHEADER
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of aplChrNCols FMTCOLSTRs
     LPAPLCHAR   lpaplChar2,    // Ptr to compiled input
     LPAPLCHAR  *lplpwszOut,    // Ptr to ptr to output string
     APLDIM      aplDimNRows,   // # rows in this array
     APLDIM      aplDimNCols,   // # cols ...
     APLDIM      aplLastDim,    // Length of the last dimension in the result
     APLRANK     aplRank,       // Rank of this array
     LPAPLDIM    lpMemDim,      // Ptr to this array's dimensions (NULL for scalar)
     APLSTYPE    aplType,       // Storage type of this array
     BOOL        bRawOutput,    // TRUE iff raw (not {thorn}) output
     BOOL        bEndingCR)     // TRUE iff last line has CR

{
    APLDIM       aplDimRow,         // Loop counter
                 aplDimCol,         // Loop counter
                 aplChrNCols;       // # cols for char arrays
    UINT         uActLen,
                 uLead,
                 uCol,
                 uCmpWid,
                 uInts,
                 uFrcs;
    LPWCHAR      lpw,
                 lpwszOut,
                 lpwszOutStart;
    LPFMTROWSTR  lpFmtRowStr;
    LPAPLCHAR    lpaplChar = lpaplChar2;
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    APLUINT      uQuadPW;       // []PW

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    uQuadPW = lpMemPTD->lpSymQuadPW->stData.stInteger;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Initialize local output string ptr
    lpwszOut = *lplpwszOut;

    // Format char arrays as one col
    aplChrNCols = (aplType EQ ARRAY_CHAR) ? 1 : aplDimNCols;

    // Loop through the formatted rows
    aplDimNRows = lpFmtHeader->uFmtRows;

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Point to the FMTROWSTR
        lpFmtRowStr = (LPFMTROWSTR) lpaplChar;

        // It's a simple array
        Assert (lpFmtRowStr->uFmtRows EQ 1);

        // Validate the FMTROWSTR
        Assert (lpFmtRowStr->Sig.nature EQ FMTROWSTR_SIGNATURE);

        // Skip over the FMTROWSTR as this is a simple array
        lpaplChar = (LPAPLCHAR) &lpFmtRowStr[1];

        // Save starting output string ptr
        lpwszOutStart = lpwszOut;

        // Handle non-blank rows
        if (!lpFmtRowStr->uBlank)
        {
            // Loop through the cols
            for (aplDimCol = 0; aplDimCol < aplChrNCols; aplDimCol++)
            {
////////////////// Split cases based upon the format type
////////////////switch (lpFmtColStr[aplDimCol].uFmtType)
////////////////{
////////////////    case FMTTYPE_INT:
////////////////    case FMTTYPE_FIX:
////////////////    case FMTTYPE_EXP:
                        // ***FIXME*** -- handle conversion from fixed point to exponential format
                        //                and line up the decimal points and the Es

                        // Get the # digits plus leading space, decimal point, and fractional part
                        uInts = lpFmtColStr[aplDimCol].uInts;
                        uFrcs = lpFmtColStr[aplDimCol].uFrcs;
                        uCmpWid  = uFrcs + uInts;               // Compiled width
                        uActLen  = lstrlenW (lpaplChar);        // Actual length

                        Assert (uActLen <= uCmpWid);

                        // Check for fractional part
                        lpw = strchrW (lpaplChar, L'.');
                        if (!lpw)
                            lpw = lpaplChar + uActLen;          // Place decimal point after integer part

                        // Align the decimal points
                        uCol = (lpw - lpaplChar) + uFrcs;
                        uLead = uCmpWid - max (uActLen, uCol);      // # leading blanks to align decimal points

                        // If this is raw output,
                        // break the line if it would exceed []PW
                        //   and the line is non-empty.
                        uCol = lpwszOut - lpwszOutStart;
                        if (bRawOutput
                         && DEF_INDENT < uCol
                         && uQuadPW < (uCmpWid + uCol))
                        {
                            // Ensure properly terminated
                            *lpwszOut = L'\0';

                            // Output the line
                            AppendLine (lpwszOutStart, TRUE, TRUE);

                            // Reset the line start
                            lpwszOut = lpw = *lplpwszOut;

                            // Fill the output area with all blanks
                            uCol = (UINT) aplLastDim - (*lplpwszOut - lpwszOutStart);
                            while (uCol--)
                                *lpw++ = L' ';

                            // Skip over leading indent
                            lpwszOut += DEF_INDENT;

                            // Shorten the width to act like this is the first col
                            //   (which doesn't have a leading blank)
                            uCmpWid = max (uCmpWid, uActLen + 1) - 1;
                            uCol = 0;
                        } else
                        {
#ifdef PREFILL
                            // Skip over leading blanks
                            lpwszOut += uLead;
                            uCol = uLead;
#else
                            // Fill with leading blanks
                            for (uCol = 0; uCol < uLead; uCol++)
                                *lpwszOut++ = L' ';
#endif
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
#ifdef PREFILL
                // Skip over trailing blanks
                Assert (uCol <= uCmpWid);
                lpwszOut += (uCmpWid - uCol);
#else
                // Fill with trailing blanks
                for (; uCol < uCmpWid; uCol++)
                    *lpwszOut++ = L' ';
#endif
                lpaplChar += (uActLen + 1);
            } // End FOR

            // If there is a trailing blank, skip over it
            if (lpFmtHeader->uFmtTrBl)
#ifdef PREFILL
                lpwszOut++;
#else
                *lpwszOut++ = L' ';
#endif
        } else
            // Run through this row again
            aplDimRow--;

        // Skip to the next row
        lpwszOut = lpwszOutStart + aplLastDim;

        // If this is raw output, output it
        if (bRawOutput)
        {
            // Ensure properly terminated
            *lpwszOut = L'\0';

            // Output the line
            AppendLine (lpwszOutStart, FALSE, bEndingCR || aplDimRow < (aplDimNRows - 1));

            // Reset the line start
            lpwszOut = lpw = *lplpwszOut;

            // Fill the output area with all blanks
            uCol = (UINT) aplLastDim - (*lplpwszOut - lpwszOutStart);
            while (uCol--)
                *lpw++ = L' ';

            // Handle blank lines between planes
            if (aplDimRow NE (aplDimNRows - 1))     // Not last row
            {
                APLDIM aplDimAcc;

                aplDimAcc = 1;
                for (uCol = 0; uCol < (aplRank - 1); uCol++)
                {
                    aplDimAcc *= lpMemDim[(aplRank - 2) - uCol];
                    if ((aplDimRow + 1) % aplDimAcc)
                        break;

                    AppendLine (L"", FALSE, TRUE);
                } // End FOR
            } // End IF
        } // End IF
    } // End FOR

    // Return the output string ptr
    *lplpwszOut = lpwszOut;

    return lpaplChar;
} // End FormatArrSimple


//***************************************************************************
//  $FormatArrNested
//
//  Format a nested array from its compiled form
//***************************************************************************

LPAPLCHAR FormatArrNested
    (LPFMTHEADER lpFmtHeader,   // Ptr to FMTHEADER
     LPVOID      lpMem,         // Ptr to raw input
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of <aplDimNCols> FMTCOLSTRs
     LPAPLCHAR   lpaplChar,     // Ptr to compiled input
     LPAPLCHAR  *lplpwszOut,    // Ptr to ptr to output string
     APLDIM      aplDimNRows,   // # rows in this array
     APLDIM      aplDimNCols,   // # cols ...
     APLRANK     aplRank,       // Rank of this array
     LPAPLDIM    lpMemDim,      // Ptr to this array's dimensions (NULL for scalar)
     APLDIM      aplLastDim,    // Length of the last dimension in the result
     BOOL        bRawOutput,    // TRUE iff raw (not {thorn}) output
     BOOL        bEndingCR)     // TRUE iff last line has CR

{
    APLDIM      aplDimRow,
                aplDimCol;
    UINT        uCol;
    LPWCHAR     lpwszOut,
                lpwszOutStart;
    LPFMTROWSTR lpFmtRowStr;
    APLSTYPE    aplType;

    // Initialize local output string ptr
    lpwszOut = *lplpwszOut;

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Point to the FMTROWSTR
        lpFmtRowStr = ((LPFMTROWSTR) lpaplChar);

        // Validate the FMTROWSTR
        Assert (lpFmtRowStr->Sig.nature EQ FMTROWSTR_SIGNATURE);

        // Skip over the FMTROWSTR
        lpaplChar = (LPAPLCHAR) &((LPFMTROWSTR) lpaplChar)[1];

        // Save starting output string ptr
        lpwszOutStart = lpwszOut;

        // Handle non-blank rows
        if (!lpFmtRowStr->uBlank)
        {
            // Loop through the cols
            for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++, ((LPAPLHETERO) lpMem)++)
            {
                // Save starting output string ptr
                lpwszOut = lpwszOutStart;

                // Offset lpwszOut from the start by the width of previous cols
                for (uCol = 0; uCol < aplDimCol; uCol++)
                    lpwszOut += (lpFmtColStr[uCol].uInts
                               + lpFmtColStr[uCol].uFrcs);

                // Split cases based upon the ptr type
                switch (GetPtrTypeInd (lpMem))
                {
                    case PTRTYPE_STCONST:
                        // Get the array storage type
                        aplType = TranslateImmTypeToArrayType ((*(LPSYMENTRY *) lpMem)->stFlags.ImmType);

                        // Split cases based upon the array storage type (STE immediate type)
                        switch (aplType)
                        {
                            case ARRAY_BOOL:
                            case ARRAY_INT:
                            case ARRAY_FLOAT:
                            case ARRAY_CHAR:
                                lpaplChar =
                                FormatArrSimple (lpFmtHeader,           // Ptr to FMTHEADER
                                                &lpFmtColStr[aplDimCol],// Ptr to vector of aplDimNCols FMTCOLSTRs
                                                 lpaplChar,             // Ptr to compiled input
                                                &lpwszOut,              // Ptr to output string
                                                 1,                     // # rows in this array
                                                 1,                     // # cols in this array
                                                 aplLastDim,            // Length of last dim in result
                                                 0,                     // Rank of this array
                                                 NULL,                  // Ptr to this array's dimensions
                                                 aplType,               // Storage type of this array
                                                 FALSE,                 // TRUE iff raw output
                                                 TRUE);                 // TRUE iff last line has CR
                                break;

                            defstop
                                break;
                        } // End SWITCH

                        break;

                    case PTRTYPE_HGLOBAL:
                        lpaplChar =
                        FormatArrNestedGlb (ClrPtrTypeIndGlb (lpMem),   // The global memory handle whose contents are to be formatted
                                            lpaplChar,                  // Ptr to compiled input
                                           &lpwszOut,                   // Ptr to ptr to output string
                                           &lpFmtColStr[aplDimCol],     // Ptr to the FMTCOLSTR for this column
                                            aplLastDim);                // Length of the last dimension in the result
                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR
        } else
            // Run through this row again
            aplDimRow--;

        // If there is a trailing blank, skip over it
        if (lpFmtHeader->uFmtTrBl)
#ifdef PREFILL
            lpwszOut++;
#else
            *lpwszOut++ = L' ';
#endif
        // Skip to the next row
        lpwszOut = lpwszOutStart + aplLastDim;
    } // End FOR

    // Return the output string ptr
    *lplpwszOut = lpwszOut;

    return lpaplChar;
} // End FormatArrNested


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
     APLDIM      aplLastDim)    // Length of the last dimension in the result

{
    APLSTYPE aplType;
    APLRANK  aplRank;
    LPVOID   lpMem;
    LPAPLDIM lpMemDim;
    APLDIM   aplDimNRows,
             aplDimNCols;
    LPFMTHEADER lpFmtHeader;
    LPFMTCOLSTR lpFmtColLcl;
    UINT     uAcc,
             uCol,
             uWid;

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

    // Get the # rows and cols
    aplDimNCols = lpFmtHeader->uCols;
    aplDimNRows = lpFmtHeader->uRows;

    // Validate the FMTHEADER
    Assert (lpFmtHeader->Sig.nature EQ FMTHEADER_SIGNATURE);

    // Skip over the FMTHEADER
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
    // Skip over the FMTCOLSTRs
    lpaplChar = (LPAPLCHAR) &lpFmtColLcl[aplDimNCols];

    // Distribute this column's FMTCOLSTR (in lpFmtColStr)
    //   amongst the FMTCOLSTRs for this item (in lpaplChar)
    if (aplDimNCols NE 0)
    {
        // Calculate the global width less trailing blanks
        //   as the latter isn't used for right-justification
        uWid = lpFmtColStr->uInts
             + lpFmtColStr->uFrcs
             - lpFmtHeader->uFmtTrBl;

        // Calculate the local width
        for (uAcc = uCol = 0; uCol < lpFmtHeader->uCols; uCol++)
            uAcc += lpFmtColLcl[uCol].uInts
                  + lpFmtColLcl[uCol].uFrcs;

        Assert (uAcc <= uWid);

        // Add in the extra width as leading blanks,
        //   in effect right-justifying this item in the col
        lpFmtColLcl[0].uInts += (uWid - uAcc);
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
            FormatArrSimple (lpFmtHeader,       // Ptr to FMTHEADER
                             lpFmtColLcl,       // Ptr to vector of <aplDimNCols> FMTCOLSTRs
                             lpaplChar,         // Ptr to compiled input
                             lplpwszOut,        // Ptr to output string
                             aplDimNRows,       // # rows in this array
                             aplDimNCols,       // # cols in this array
                             aplLastDim,        // Length of last dim in result
                             aplRank,           // Rank of this array
                             lpMemDim,          // Ptr to this array's dimensions
                             aplType,           // Storage type of this array
                             FALSE,             // TRUE iff raw output
                             TRUE);             // TRUE iff last line has CR
            break;

        case ARRAY_NESTED:
            lpaplChar =
            FormatArrNested (lpFmtHeader,       // Ptr to FMTHEADER
                             lpMem,             // Ptr to raw input
                             lpFmtColLcl,       // Ptr to vector of <aplDimNCols> FMTCOLSTRs
                             lpaplChar,         // Ptr to compiled input
                             lplpwszOut,        // Ptr to ptr to output string
                             aplDimNRows,       // # rows in this array
                             aplDimNCols,       // # cols ...
                             aplRank,           // Rank of this array
                             lpMemDim,          // Ptr to this array's dimensions
                             aplLastDim,        // Length of last dim in result
                             FALSE,             // TRUE iff raw (not {thorn}) output
                             TRUE);             // TRUE iff last line has CR
            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlb); lpMem = NULL;

    return lpaplChar;
} // End FormatArrNestedGlb


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
     BOOL        bNested)       // TRUE iff nested

{
    UINT        uBitMask,
                uLen;
    APLDIM      aplDimCol,
                aplDimRow;
    LPFMTROWSTR lpFmtRowLcl = NULL;

    // Loop through the cols setting the common width
    for (aplDimCol = 1; aplDimCol < aplDimNCols; aplDimCol++)
    {
        // Max the current width with the width of this number
        lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, 2);
////////
////////// Max the current format type with FMT_INT
////////lpFmtColStr[aplDimCol].uFmtType = max (lpFmtColStr[aplDimCol].uFmtType, FMTTYPE_INT);
    } // End FOR

    // Add in a leading blank if nested or not scalar/vector
    // The "1 +" is the size of a Boolean
    uLen = 1 + (bNested || (aplRank > 1));
    lpFmtColStr[0].uInts = max (lpFmtColStr[0].uInts, uLen);

    // Add in a trailing blank if nested
    lpFmtColStr[0].uFrcs = max (lpFmtColStr[0].uFrcs, (UINT) bNested);

    // Initialize bit mask for all references to lpMem
    uBitMask = 0x01;

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
#ifdef DEBUG
        lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
#endif
        lpFmtRowLcl->uBlank      = FALSE;
        lpFmtRowLcl->uFmtRows    = 1;           // Initialize the count
////////lpFmtRowLcl->lpFmtRowNxt = NULL;        // Initialize the ptr (filled in below)
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Loop through the cols
        for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        {
            // Format the Boolean
            *lpaplChar++ = ((uBitMask & *lpMem) ? L'1' : L'0');

            // Append a string separator
            *lpaplChar++ = L'\0';

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

        // If this is not at the top level (Depth = 0),
        //   we need to count the interdimensional spacing
        //   as blank rows in the row count
        if (lpFmtHeader->uDepth                 // Not top level, and
         && aplDimRow NE 0                      // not first row, and
         && aplDimRow NE (aplDimNRows - 1))     // not last row
            lpaplChar = AppendBlankRows (lpaplChar,
                                         lpMemDim,
                                         aplRank,
                                         aplDimRow);
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
     BOOL        bNested)       // TRUE iff nested

{
    UINT        uLen;
    APLDIM      aplDimCol,
                aplDimRow;
    LPAPLCHAR   lpwszOut;
    LPFMTROWSTR lpFmtRowLcl = NULL;

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
#ifdef DEBUG
        lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
#endif
        lpFmtRowLcl->uBlank      = FALSE;
        lpFmtRowLcl->uFmtRows    = 1;           // Initialize the count
////////lpFmtRowLcl->lpFmtRowNxt = NULL;        // Initialize the ptr (filled in below)
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

            // Calculate the length of the integer part
            uLen = (lpaplChar - lpwszOut) - 1;

            // Add in a leading blank if nested or not scalar/vector or not 1st col
            uLen += bNested || (aplRank > 1) || aplDimCol NE 0;

            // Max the current integer width with this
            lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

            // Calculate the length of the fractional part
            uLen = 0;

            // Add in a trailing blank if nested and last col
            uLen += bNested && aplDimCol EQ (aplDimNCols - 1);

            // Max the current fractional width with this
            lpFmtColStr[aplDimCol].uFrcs = max (lpFmtColStr[aplDimCol].uFrcs, uLen);
////////////
////////////// Max the current format type with FMT_INT
////////////lpFmtColStr[aplDimCol].uFmtType = max (lpFmtColStr[aplDimCol].uFmtType, FMTTYPE_INT);
        } // End FOR

        // Save as ptr to next row struc
        lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;

        // If this is not at the top level (Depth = 0),
        //   we need to count the interdimensional spacing
        //   as blank rows in the row count
        if (lpFmtHeader->uDepth                 // Not top level, and
         && aplDimRow NE 0                      // not first row, and
         && aplDimRow NE (aplDimNRows - 1))     // not last row
            lpaplChar = AppendBlankRows (lpaplChar,
                                         lpMemDim,
                                         aplRank,
                                         aplDimRow);
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
     BOOL        bNested)       // TRUE iff nested

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
#endif
        lpFmtRowLcl->uBlank      = FALSE;
        lpFmtRowLcl->uFmtRows    = 1;           // Initialize the count
////////lpFmtRowLcl->lpFmtRowNxt = NULL;        // Initialize the ptr (filled in below)
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Loop through the cols
        for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        {
            // Format the float
            lpaplChar =
            FormatFloat (lpwszOut = lpaplChar,      // Ptr to output save area
                         *((LPAPLFLOAT) lpMem)++,   // The floating point value
                         0);                        // Use default precision
            // Zap the trailing blank
            lpaplChar[-1] = L'\0';

////////////// Check for exponential format
////////////lpwsz = strchrW (lpwszOut, L'E');
////////////
////////////// If present, mark as exponential
////////////if (lpwsz)
////////////    lpFmtColStr[aplDimCol].uFmtType = max (lpFmtColStr[aplDimCol].uFmtType, FMTTYPE_EXP);
////////////
            // Check for decimal point
            lpwsz = strchrW (lpwszOut, L'.');
            if (lpwsz)
            {
                // Calculate the length of the integer part
                uLen = (lpwsz - lpwszOut);

                // Add in a leading blank if nested or not scalar/vector or not 1st col
                uLen += bNested || (aplRank > 1) || aplDimCol NE 0;

                // Max the current integer width with this
                lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

                // Calculate the length of the fractional part
                //   including the decimal point
                uLen = (lpaplChar - lpwsz) - 1;

                // Add in a trailing blank if nested and last col
                uLen += bNested && aplDimCol EQ (aplDimNCols - 1);

                // Max the current fractional width with this
                lpFmtColStr[aplDimCol].uFrcs = max (lpFmtColStr[aplDimCol].uFrcs, uLen);
////////////////
////////////////// Max the current format type with FMTTYPE_FIX
////////////////lpFmtColStr[aplDimCol].uFmtType = max (lpFmtColStr[aplDimCol].uFmtType, FMTTYPE_FIX);
            } else  // No decimal point
            {
                // Calculate the length of the integer part
                uLen = (lpaplChar - lpwszOut) - 1;

                // Add in a leading blank unless scalar/vector and 1st col
                uLen += (aplRank > 1) || aplDimCol NE 0;

                // Max the current integer width with this
                lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

                // Calculate the length of the fractional part
                uLen = 0;

                // Add in a trailing blank if nested and last col
                uLen += bNested && aplDimCol EQ (aplDimNCols - 1);

                // Max the current fractional width with this
                lpFmtColStr[aplDimCol].uFrcs = max (lpFmtColStr[aplDimCol].uFrcs, uLen);
            } // End IF/ELSE
        } // End FOR

        // Save as ptr to next row struc
        lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;

        // If this is not at the top level (Depth = 0),
        //   we need to count the interdimensional spacing
        //   as blank rows in the row count
        if (lpFmtHeader->uDepth                 // Not top level, and
         && aplDimRow NE 0                      // not first row, and
         && aplDimRow NE (aplDimNRows - 1))     // not last row
            lpaplChar = AppendBlankRows (lpaplChar,
                                         lpMemDim,
                                         aplRank,
                                         aplDimRow);
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
     BOOL        bNested)       // TRUE iff nested

{
////APLDIM      aplDimCol,
    APLDIM      aplDimRow;
    LPFMTROWSTR lpFmtRowLcl = NULL;
    UINT        uRht,           // Loop counter
                uRes,           // Loop counter
                uTmp,           // Temporary
                uColWidth;      // Col width

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
#ifdef DEBUG
        lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
#endif
        lpFmtRowLcl->uBlank      = FALSE;
        lpFmtRowLcl->uFmtRows    = 1;           // Initialize the count
////////lpFmtRowLcl->lpFmtRowNxt = NULL;        // Initialize the ptr (filled in below)
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Copy the data including a terminating zero
        //  and launder special chars
        for (uRht = uRes = uColWidth = 0; uRht < aplDimNCols; uRht++)
        switch (lpMem[uRht])
        {
            case TCDEL:         // []TCDEL -- Ignore this
            case TCESC:         // []TCESC -- Ignore this
            case TCFF:          // []TCFF  -- Ignore this
            case TCNUL:         // []TCNUL -- Ignore this
                break;

            case TCBEL:         // []TCBEL -- Sound the alarum!
                MessageBeep (NEG1U);

                break;

            case TCLF:          // []TCLF -- Start a new line
                // Skip over the actual col width
                lpaplChar += uColWidth;

                // Ensure properly terminated
                *lpaplChar++ = L'\0';

                // Max the current width with the width of this col
                lpFmtColStr[0].uInts = max (lpFmtColStr[0].uInts, uColWidth);

                // Save as ptr to next row struc
                lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;

                // Create a new FMTROWSTR
                lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
#ifdef DEBUG
                lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
#endif
                lpFmtRowLcl->uBlank      = FALSE;
                lpFmtRowLcl->uFmtRows    = 1;           // Initialize the count
////////////////lpFmtRowLcl->lpFmtRowNxt = NULL;        // Initialize the ptr (filled in below)
                lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

                // Start the result part of this loop over again
                uRes = uColWidth = 0;

                break;

            case TCHT:          // []TCHT -- Move ahead to the next tab stop
                uTmp = (1 + (uRes % uTabs)) * uTabs;
                while (uRes < uTmp)
                {
                    lpaplChar[uRes++] = L' ';
                    uColWidth = max (uColWidth, uRes);
                } // End WHILE

                break;

            case TCNL:          // []TCNL -- Restart at the beginning of this line
                uRes = 0;

                break;

            case TCBS:          // []TCBS -- Backspace if there's room
                if (uRes)
                    uRes--;
                break;

            default:            // Insert a new char
                if (lpMem[uRht] < 32)   // If none of the above and not printable,
                    break;              //   ignore it

                lpaplChar[uRes++] = lpMem[uRht];
                uColWidth = max (uColWidth, uRes);

                break;
        } // End FOR/SWITCH

        // Skip over the actual col width
        lpaplChar += uColWidth;

        // Skip over the right arg cols
        lpMem     += aplDimNCols;

        // Ensure properly terminated
        *lpaplChar++ = L'\0';

        // Max the current width with the width of this col
        lpFmtColStr[0].uInts = max (lpFmtColStr[0].uInts, uColWidth);

        // Save as ptr to next row struc
        lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;

        // If this is not at the top level (Depth = 0),
        //   we need to count the interdimensional spacing
        //   as blank rows in the row count
        if (lpFmtHeader->uDepth                 // Not top level, and
         && aplDimRow NE 0                      // not first row, and
         && aplDimRow NE (aplDimNRows - 1))     // not last row
            lpaplChar = AppendBlankRows (lpaplChar,
                                         lpMemDim,
                                         aplRank,
                                         aplDimRow);
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
     BOOL        bNested)       // TRUE iff nested

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
#endif
        lpFmtRowLcl->uBlank      = FALSE;
        lpFmtRowLcl->uFmtRows    = 1;           // Initialize the count
////////lpFmtRowLcl->lpFmtRowNxt = NULL;        // Initialize the ptr (filled in below)
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

            // Calculate the length of the integer part
            uLen = (lpaplChar - lpwszOut) - 1;

            // Add in a leading blank if nested or not scalar/vector or not 1st col
            uLen += bNested || (aplRank > 1) || aplDimCol NE 0;

            // Max the current integer width with this
            lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

            // Calculate the length of the fractional part
            uLen = 0;

            // Add in a trailing blank if nested and last col
            uLen += bNested && aplDimCol EQ (aplDimNCols - 1);

            // Max the current fractional width with this
            lpFmtColStr[aplDimCol].uFrcs = max (lpFmtColStr[aplDimCol].uFrcs, uLen);
        } // End FOR

        // Save as ptr to next row struc
        lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;

        // If this is not at the top level (Depth = 0),
        //   we need to count the interdimensional spacing
        //   as blank rows in the row count
        if (lpFmtHeader->uDepth                 // Not top level, and
         && aplDimRow NE 0                      // not first row, and
         && aplDimRow NE (aplDimNRows - 1))     // not last row
            lpaplChar = AppendBlankRows (lpaplChar,
                                         lpMemDim,
                                         aplRank,
                                         aplDimRow);
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
     BOOL        bNested)       // TRUE iff nested

{
    UINT        uLen;
    APLDIM      aplDimCol,
                aplDimRow;
    LPAPLCHAR   lpwszOut;
    LPWCHAR     lpwsz;
    LPFMTROWSTR lpFmtRowLcl = NULL;
    UINT        immTypeCur,
                immTypeLst;

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
#ifdef DEBUG
        lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
#endif
        lpFmtRowLcl->uBlank      = FALSE;
        lpFmtRowLcl->uFmtRows    = 1;           // Initialize the count
////////lpFmtRowLcl->lpFmtRowNxt = NULL;        // Initialize the ptr (filled in below)
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Note that it doesn't matter what we start off the row
        //   with as the last immediate type (immTypeLst) because
        //   the phrase "(aplDimCol NE 0) &&" causes any value to
        //   which we set it to be ignored.
////////immTypeLst = IMMTYPE_BOOL;

        // Loop through the cols
        for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        {
#define lpSymEntry      ((LPSYMENTRY *) lpMem)

            // Save the immediate type
            immTypeCur = (*lpSymEntry)->stFlags.ImmType;

            // Handle []TCLF specially
            if (immTypeCur EQ IMMTYPE_CHAR
             && *lpaplChar EQ TCLF)
            {
                DbgBrk ();      // ***FINISHME*** -- []TCLF






            } else
                lpaplChar =
                FormatImmed (lpwszOut = lpaplChar,
                             immTypeCur,
                            &(*lpSymEntry)->stData.stLongest);
            // Skip to next entry
            lpSymEntry++;

#undef  lpSymEntry

            // Zap the trailing blank
            lpaplChar[-1] = L'\0';

            // Check for decimal point
            lpwsz = strchrW (lpwszOut, L'.');
            if (lpwsz)
            {
                // Calculate the length of the integer part
                uLen = (lpwsz - lpwszOut);

////////////////// Add in a leading blank if nested or not scalar/vector or not 1st col
////////////////uLen += bNested || (aplRank > 1) || aplDimCol NE 0;
                // Add in a leading blank if nested or not scalar/vector or (not 1st col and either last or current is not a char)
                uLen += bNested
/////////////////////|| (aplRank > 1)
                     || (aplDimCol NE 0
                      && (immTypeLst NE IMMTYPE_CHAR || immTypeCur NE IMMTYPE_CHAR));

                // Max the current integer width with this
                lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

                // Calculate the length of the fractional part
                //   including the decimal point
                uLen = (lpaplChar - lpwsz) - 1;

                // Add in a trailing blank if nested and last col
                uLen += bNested && aplDimCol EQ (aplDimNCols - 1);

                // Max the current fractional width with this
                lpFmtColStr[aplDimCol].uFrcs = max (lpFmtColStr[aplDimCol].uFrcs, uLen);
            } else  // No decimal point
            {
                // Calculate the length of the integer part
                uLen = (lpaplChar - lpwszOut) - 1;

////////////////// Add in a leading blank unless scalar/vector and 1st col
////////////////uLen += (aplRank > 1) || aplDimCol NE 0;
                // Add in a leading blank if nested or not scalar/vector or (not 1st col and either last or current is not a char)
                uLen += bNested
/////////////////////|| (aplRank > 1)
                     || (aplDimCol NE 0
                      && (immTypeLst NE IMMTYPE_CHAR || immTypeCur NE IMMTYPE_CHAR));

                // Max the current integer width with this
                lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

                // Calculate the length of the fractional part
                uLen = 0;

                // Add in a trailing blank if nested and last col
                uLen += bNested && aplDimCol EQ (aplDimNCols - 1);

                // Max the current fractional width with this
                lpFmtColStr[aplDimCol].uFrcs = max (lpFmtColStr[aplDimCol].uFrcs, uLen);
            } // End IF/ELSE

            // Save as last immediate type
            immTypeLst = immTypeCur;
        } // End FOR

        // Save as ptr to next row struc
        lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;

        // If this is not at the top level (Depth = 0),
        //   we need to count the interdimensional spacing
        //   as blank rows in the row count
        if (lpFmtHeader->uDepth                 // Not top level, and
         && aplDimRow NE 0                      // not first row, and
         && aplDimRow NE (aplDimNRows - 1))     // not last row
            lpaplChar = AppendBlankRows (lpaplChar,
                                         lpMemDim,
                                         aplRank,
                                         aplDimRow);
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
     BOOL        bNested)       // TRUE iff nested

{
    APLDIM      aplDimCol,
                aplDimRow;
    LPFMTROWSTR lpFmtRowLcl = NULL;

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
#ifdef DEBUG
        lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
#endif
        lpFmtRowLcl->uBlank      = FALSE;
        lpFmtRowLcl->uFmtRows    = 0;           // Initialize the count
        lpFmtRowLcl->lpFmtRowNxt = NULL;        // Initialize the ptr
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Loop through the cols
        for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++, ((LPAPLHETERO) lpMem)++)
        {
            // Split cases based upon the ptr type
            switch (GetPtrTypeInd (lpMem))
            {
                case PTRTYPE_STCONST:
                    // Split cases based upon the STE immediate type
                    switch ((*(LPSYMENTRY *) lpMem)->stFlags.ImmType)
                    {
                        case IMMTYPE_BOOL:
                            lpaplChar =
                            CompileArrBool    (&(*(LPSYMENTRY *) lpMem)->stData.stBoolean,  // Ptr to right arg memory
                                                lpFmtHeader,                                // Ptr to parent header
                                               &lpFmtColStr[aplDimCol],                     // Ptr to vector of ColStrs
                                                lpaplChar,                                  // Ptr to compiled output
                                                1,                                          // # rows
                                                1,                                          // # cols
                                                0,                                          // Right arg rank
                                                NULL,                                       // Ptr to right arg dimensions
                                                TRUE);                                      // TRUE iff nested
                            break;

                        case IMMTYPE_INT:
                            lpaplChar =
                            CompileArrInteger (&(*(LPSYMENTRY *) lpMem)->stData.stInteger,  // Ptr to right arg memory
                                                lpFmtHeader,                                // Ptr to parent header
                                               &lpFmtColStr[aplDimCol],                     // Ptr to vector of ColStrs
                                                lpaplChar,                                  // Ptr to compiled output
                                                1,                                          // # rows
                                                1,                                          // # cols
                                                0,                                          // Right arg rank
                                                NULL,                                       // Ptr to right arg dimensions
                                                TRUE);                                      // TRUE iff nested
                            break;

                        case IMMTYPE_FLOAT:
                            lpaplChar =
                            CompileArrFloat   (&(*(LPSYMENTRY *) lpMem)->stData.stFloat,    // Ptr to right arg memory
                                                lpFmtHeader,                                // Ptr to parent header
                                               &lpFmtColStr[aplDimCol],                     // Ptr to vector of ColStrs
                                                lpaplChar,                                  // Ptr to compiled output
                                                1,                                          // # rows
                                                1,                                          // # cols
                                                0,                                          // Right arg rank
                                                NULL,                                       // Ptr to right arg dimensions
                                                TRUE);                                      // TRUE iff nested
                            break;

                        case IMMTYPE_CHAR:
                            lpaplChar =
                            CompileArrChar    (&(*(LPSYMENTRY *) lpMem)->stData.stChar,     // Ptr to right arg memory
                                                lpFmtHeader,                                // Ptr to parent header
                                               &lpFmtColStr[aplDimCol],                     // Ptr to vector of ColStrs
                                                lpaplChar,                                  // Ptr to compiled output
                                                1,                                          // # rows
                                                1,                                          // # cols
                                                0,                                          // Right arg rank
                                                NULL,                                       // Ptr to right arg dimensions
                                                TRUE);                                      // TRUE iff nested
                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case PTRTYPE_HGLOBAL:
                    lpaplChar =
                    CompileArrNestedGlb (ClrPtrTypeIndGlb (lpMem),
                                         lpFmtHeader,
                                         lpFmtRowLcl,
                                        &lpFmtColStr[aplDimCol],
                                         lpaplChar);
                    break;

                defstop
                    break;
            } // End SWITCH
        } // End FOR

        // Save as ptr to next row struc
        lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;

        // If this is not the first or last row,
        //   we need to count the interdimensional spacing
        //   as blank rows in the row count
        // Note that we don't check for Depth == 0 as this
        //   is a nested array, so as the result is formatted
        //   as a matrix there's no other place to insert
        //   interdimensional spacing.
        if (aplDimRow NE 0                      // Not first row, and
         && aplDimRow NE (aplDimNRows - 1))     // not last row
            lpaplChar = AppendBlankRows (lpaplChar,
                                         lpMemDim,
                                         aplRank,
                                         aplDimRow);
    } // End FOR

    // Mark as last row struc
    if (lpFmtRowLcl)
        lpFmtRowLcl->lpFmtRowNxt = NULL;
    return lpaplChar;
} // End CompileArrNested


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
     LPAPLCHAR   lpaplChar)     // Ptr to next available position

{
    APLSTYPE aplType;           // Arg storage type
    APLNELM  aplNELM;           // Arg NELM
    APLRANK  aplRank;           // Arg rank
    LPVOID   lpMem;
    LPAPLDIM lpMemDim;          // Ptr to arg dimensions
    APLDIM   aplDimNRows,       // # rows
             aplDimNCols,       // # cols
             aplChrNCols;       // # cols for char arrays
    BOOL     bSimpleScalar;
    APLINT   aplInteger;
    APLFLOAT aplFloat;
    APLCHAR  aplChar;

    // Get the attributes (Type, NELM, Rank) of the global
    AttrsOfGlb (hGlb, &aplType, &aplNELM, &aplRank, NULL);

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlb);

    // Skip over the header to the dimensions
    lpMemDim = VarArrayBaseToDim (lpMem);

    // Get the # rows & columns in the right arg
    if (aplRank EQ 0)
        aplDimNCols = aplDimNRows = 1;
    else
    {
        aplDimNCols = lpMemDim[aplRank - 1];

        // Get the # rows (across all planes)
        if (aplRank EQ 1
         || aplDimNCols EQ 0)
            aplDimNRows = 1;
        else
            aplDimNRows = aplNELM / aplDimNCols;
    } // End IF/ELSE

    // Skip over the header and dimensions to the data
    lpMem = VarArrayBaseToData (lpMem, aplRank);

    // Format char arrays as one col
    aplChrNCols = (aplType EQ ARRAY_CHAR) ? 1 : aplDimNCols;

    // Create a new FMTHEAD in the output
    ZeroMemory ((LPFMTHEADER) lpaplChar, sizeof (FMTHEADER));
    ((LPFMTHEADER) lpaplChar)->lpFmtHeadUp = lpFmtHeader;
    lpFmtHeader = (LPFMTHEADER) lpaplChar;
#ifdef DEBUG
    lpFmtHeader->Sig.nature  = FMTHEADER_SIGNATURE;
#endif
    lpFmtHeader->lpFmtRowUp  = lpFmtRowStr;
    lpFmtHeader->lpFmtColUp  = lpFmtColStr;
////lpFmtHeader->lpFmtRow1st =                      // Filled in below
////lpFmtHeader->lpFmtCol1st =                      // ...
    lpFmtHeader->uRows       = (UINT) aplDimNRows;
    lpFmtHeader->uCols       = (UINT) aplChrNCols;
////lpFmtHeader->uFmtRows    = 0;                   // Filled in by ZeroMemory
////lpFmtHeader->uFmtInts    = 0;                   // ...
////lpFmtHeader->uFmtFrcs    = 0;                   // ...
////lpFmtHeader->uFmtTrBl    = FALSE;               // ...
    lpFmtHeader->uDepth      = 1 + lpFmtHeader->lpFmtHeadUp->uDepth;
    lpFmtHeader->uMatRes     = (aplRank > 1);

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
    lpaplChar = (LPAPLCHAR) (&lpFmtColStr[aplChrNCols]);

    // Save ptr to 1st child FMTROWSTR
    if (aplDimNRows)
        lpFmtHeader->lpFmtRow1st = (LPFMTROWSTR) lpaplChar;
    else
        lpFmtHeader->lpFmtRow1st = NULL;

    // Loop through the array appending the formatted values (separated by L'\0')
    //   to the output vector, and accumulating the values in the appropriate
    //   FMTCOLSTR & FMTROWSTR entries.

    // If the right arg is a simple scalar,
    //   get its value
    bSimpleScalar = (aplRank EQ 0 && IsSimpleNH (aplType));
    if (bSimpleScalar)
        FirstValueSymGlb (MakeGlbTypeGlb (hGlb),    // Ptr to right arg token
                         &aplInteger,               // Ptr to integer result
                         &aplFloat,                 // Ptr to float ...
                         &aplChar,                  // Ptr to WCHAR ...
                          NULL,                     // Ptr to longest ...
                          NULL,                     // Ptr to lpSym/Glb ...
                          NULL,                     // Ptr to ...immediate type ...
                          NULL);                    // Ptr to array type ...
    // Split cases based upon the right arg's storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
            lpaplChar =
            CompileArrBool    (bSimpleScalar ? (LPAPLBOOL) &aplInteger
                                             : (LPAPLBOOL)   lpMem,     // Ptr to right arg memory
                               lpFmtHeader,                             // Ptr to parent header
                               lpFmtColStr,                             // Ptr to vector of ColStrs
                               lpaplChar,                               // Ptr to compiled output
                               aplDimNRows,                             // # rows
                               aplDimNCols,                             // # cols
                               aplRank,                                 // Right arg rank
                               lpMemDim,                                // Ptr to right arg dimensions
                               FALSE);                                  // TRUE iff nested
            break;

        case ARRAY_INT:
            lpaplChar =
            CompileArrInteger (bSimpleScalar ? &aplInteger
                                             : (LPAPLINT)    lpMem,     // Ptr to right arg memory
                               lpFmtHeader,                             // Ptr to parent header
                               lpFmtColStr,                             // Ptr to vector of ColStrs
                               lpaplChar,                               // Ptr to compiled output
                               aplDimNRows,                             // # rows
                               aplDimNCols,                             // # cols
                               aplRank,                                 // Right arg rank
                               lpMemDim,                                // Ptr to right arg dimensions
                               FALSE);                                  // TRUE iff nested
            break;

        case ARRAY_FLOAT:
            lpaplChar =
            CompileArrFloat   (bSimpleScalar ? &aplFloat
                                             : (LPAPLFLOAT)  lpMem,     // Ptr to right arg memory
                               lpFmtHeader,                             // Ptr to parent header
                               lpFmtColStr,                             // Ptr to vector of ColStrs
                               lpaplChar,                               // Ptr to compiled output
                               aplDimNRows,                             // # rows
                               aplDimNCols,                             // # cols
                               aplRank,                                 // Right arg rank
                               lpMemDim,                                // Ptr to right arg dimensions
                               FALSE);                                  // TRUE iff nested
            break;

        case ARRAY_CHAR:
            lpaplChar =
            CompileArrChar    (bSimpleScalar ? &aplChar
                                             : (LPAPLCHAR)  lpMem,      // Ptr to right arg memory
                               lpFmtHeader,                             // Ptr to parent header
                               lpFmtColStr,                             // Ptr to vector of ColStrs
                               lpaplChar,                               // Ptr to compiled output
                               aplDimNRows,                             // # rows
                               aplDimNCols,                             // # cols
                               aplRank,                                 // Right arg rank
                               lpMemDim,                                // Ptr to right arg dimensions
                               FALSE);                                  // TRUE iff nested
            break;

        case ARRAY_APA:
            lpaplChar =
            CompileArrAPA     ((LPAPLAPA)    lpMem,                     // Ptr to right arg memory
                               lpFmtHeader,                             // Ptr to parent header
                               lpFmtColStr,                             // Ptr to vector of ColStrs
                               lpaplChar,                               // Ptr to compiled output
                               aplDimNRows,                             // # rows
                               aplDimNCols,                             // # cols
                               aplRank,                                 // Right arg rank
                               lpMemDim,                                // Ptr to right arg dimensions
                               FALSE);                                  // TRUE iff nested
            break;

        case ARRAY_HETERO:
            lpaplChar =
            CompileArrHetero  ((LPAPLHETERO) lpMem,                     // Ptr to right arg memory
                               lpFmtHeader,                             // Ptr to parent header
                               lpFmtColStr,                             // Ptr to vector of ColStrs
                               lpaplChar,                               // Ptr to compiled output
                               aplDimNRows,                             // # rows
                               aplDimNCols,                             // # cols
                               aplRank,                                 // Right arg rank
                               lpMemDim,                                // Ptr to right arg dimensions
                               FALSE);                                  // TRUE iff nested
            break;

        case ARRAY_NESTED:
            lpaplChar =
            CompileArrNested  ((LPAPLNESTED) lpMem,                     // Ptr to right arg memory
                               lpFmtHeader,                             // Ptr to parent header
                               lpFmtColStr,                             // Ptr to vector of ColStrs
                               lpaplChar,                               // Ptr to compiled output
                               aplDimNRows,                             // # rows
                               aplDimNCols,                             // # cols
                               aplRank,                                 // Right arg rank
                               lpMemDim,                                // Ptr to right arg dimensions
                               FALSE);                                  // TRUE iff nested
            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlb); lpMem = NULL;

    // Propogate the row & col count up the line
    PropogateRowColCount (lpFmtHeader,
                          TRUE);
    return lpaplChar;
} // End CompileArrNestedGlb


//***************************************************************************
//  $PropogateRowColCount
//
//  Propogate the row & col count up the line
//***************************************************************************

void PropogateRowColCount
    (LPFMTHEADER lpFmtHeader,       // Ptr to FMTHEADER struc
     BOOL        bNested)           // TRUE iff nested (to be surrounded by blanks)

{
    UINT        uAcc,
                uInts,
                uFrcs,
                uTrBl,
                uCol;
    LPFMTROWSTR lpFmtRowLcl;

    // Count the # rows in this block
    for (lpFmtRowLcl = lpFmtHeader->lpFmtRow1st,
           uAcc = 0;
         lpFmtRowLcl NE NULL;
         lpFmtRowLcl = lpFmtRowLcl->lpFmtRowNxt)
        uAcc += lpFmtRowLcl->uFmtRows;

    // Save in row, col, and head strucs
    if (lpFmtHeader->lpFmtRowUp)
        lpFmtHeader->lpFmtRowUp->uFmtRows = max (lpFmtHeader->lpFmtRowUp->uFmtRows, uAcc);
    lpFmtHeader->uFmtRows = max (lpFmtHeader->uFmtRows, uAcc);

    // Count the col width in this block
    for (uInts = uFrcs = uCol = 0; uCol < lpFmtHeader->uCols; uCol++)
    {
        uInts += lpFmtHeader->lpFmtCol1st[uCol].uInts;
        uFrcs += lpFmtHeader->lpFmtCol1st[uCol].uFrcs;
    } // End FOR

    // If this item is nested, surround it with a
    //   column of blanks on either side.
    uInts += bNested;       // One on the left
    uTrBl  = bNested;       // ...        right

    // Save in col & head strucs
    if (lpFmtHeader->lpFmtColUp)
    {
        lpFmtHeader->lpFmtColUp->uInts = max (lpFmtHeader->lpFmtColUp->uInts, uInts);
        lpFmtHeader->lpFmtColUp->uFrcs = max (lpFmtHeader->lpFmtColUp->uFrcs, uFrcs + uTrBl);
    } // End IF

    lpFmtHeader->uFmtInts = max (lpFmtHeader->uFmtInts, uInts);
    lpFmtHeader->uFmtFrcs = max (lpFmtHeader->uFmtFrcs, uFrcs);
    lpFmtHeader->uFmtTrBl |= uTrBl;

    // Propogate the uMatRes bit
    if (lpFmtHeader->lpFmtHeadUp)
        lpFmtHeader->lpFmtHeadUp->uMatRes |= lpFmtHeader->uMatRes;

    // Recurse
    if (lpFmtHeader->lpFmtHeadUp)
        PropogateRowColCount (lpFmtHeader->lpFmtHeadUp,
                              FALSE);
} // End PropogateRowColCount


//***************************************************************************
//  $AppendBlankRows
//
//  Append blank rows to account for interdimensional spacing
//***************************************************************************

LPAPLCHAR AppendBlankRows
    (LPAPLCHAR lpaplChar,
     LPAPLDIM  lpMemDim,
     APLRANK   aplRank,
     APLDIM    aplDimRow)

{
    APLDIM      aplDimAcc,
                aplDimCol;
    LPFMTROWSTR lpFmtRowLcl;

    // Initialize the product accumulator
    aplDimAcc = 1;

    // Loop through all but the last dimension
    for (aplDimCol = 0; aplDimCol < (aplRank - 1); aplDimCol++)
    {
        aplDimAcc *= lpMemDim[(aplRank - 2) - aplDimCol];
        if ((aplDimRow + 1) % aplDimAcc)
            break;

        // Insert a row of blanks
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
#ifdef DEBUG
        lpFmtRowLcl->Sig.nature  = FMTROWSTR_SIGNATURE;
#endif
        lpFmtRowLcl->uBlank      = TRUE;
        lpFmtRowLcl->uFmtRows    = 1;           // Initialize the count
////////lpFmtRowLcl->lpFmtRowNxt = NULL;        // Initialize the ptr (filled in below)

        // Skip over the FMTROWSTR
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Save as ptr to next row struc
        lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;
    } // End FOR

    return lpaplChar;
} // End AppendBlankRows


//***************************************************************************
//  $PrimFnDydDownTackJot_EM_YY
//
//  Primitive function for dyadic DownTackJot ("format by example")
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
    APLSTYPE     aplTypeLft,
                 aplTypeRht;
    APLNELM      aplNELMLft,
                 aplNELMRht;
    APLRANK      aplRankLft,
                 aplRankRht;
    HGLOBAL      hGlbLft,
                 hGlbRht;
    LPVOID       lpMemLft,
                 lpMemRht;
    BOOL         bRet = TRUE;
    LPPL_YYSTYPE lpYYRes = NULL;

    return PrimFnNonceError_EM (lptkFunc);

    DbgBrk ();          // ***FINISHME*** -- PrimFnDydDownTackJot_EM_YY

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Get left and right arg's global ptrs
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Check for RANK ERROR
    if (aplRankLft > 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkLftArg);
        bRet = FALSE;

        goto ERROR_EXIT;
    } // End IF

    // Check for LENGTH ERROR
////if (aplNELMLft NE 2
//// && aplNELMLft NE (2 * 1))  // ***FIXME***
    {
        ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                   lptkLftArg);
        bRet = FALSE;

        goto ERROR_EXIT;
    } // End IF









    // Allocate a new YYRes
    lpYYRes = YYAlloc ();




ERROR_EXIT:
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

    if (bRet)
        return lpYYRes;
    else
    {
        YYFree (lpYYRes); lpYYRes = NULL;
        return NULL;
    } // End IF/ELSE
} // End PrimFnDydDownTackJot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_dtackjot.c
//***************************************************************************
