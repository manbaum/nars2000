//***************************************************************************
//  NARS2000 -- Primitive Function -- Thorn
//***************************************************************************

#pragma pack (1)
#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "display.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

// ***FIXME*** -- Is this any better than filling as needed??
#define PREFILL         // With blanks


//***************************************************************************
//  PrimFnThorn_EM
//
//  Primitive function for monadic and dyadic Thorn ("default format" and "format by example")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnThorn_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnThorn_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UCS2_THORN);

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
        return PrimFnMonThorn_EM (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydThorn_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnThorn_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnMonThorn_EM
//
//  Primitive function for monadic Thorn ("default format")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonThorn_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnMonThorn_EM
    (LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    APLSTYPE    aplTypeRht;
    APLNELM     aplNELMRht,
                aplNELMRes;
    APLRANK     aplRankRht,
                aplRankRes;
    APLDIM      aplDimNCols,
                aplDimNRows,
                aplDimCol,
                aplLastDim;
    HGLOBAL     hGlbRht = NULL,
                hGlbRes = NULL;
    LPVOID      lpMemRht = NULL,
                lpMemRes = NULL;
    LPAPLDIM    lpMemRhtDim = NULL;
    APLINT      aplIntegerRht;
    APLFLOAT    aplFloatRht;
    APLCHAR     aplCharRht;
    LPFMTHEADER lpFmtHeader;
    LPFMTCOLSTR lpFmtColStr;
    LPAPLCHAR   lpaplChar,
                lpaplCharStart;
    APLUINT     ByteRes;
    BOOL        bRet = TRUE,
                bSimpleScalar;
    UINT        YYLclIndex;

    // Get new index into YYRes
    YYLclIndex = NewYYResIndex ();

    // Get the attributes (Type, NELM, and Rank) of the right args
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Get right arg's global ptrs
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Except for CHARs, the rank of the result is the
    //   larger of the rank of the right arg and one.
    aplRankRes = max (aplRankRht, 1);

    if (lpMemRht)
        lpMemRhtDim = VarArrayBaseToDim (lpMemRht);

    // Get the # rows & columns in the right arg
    if (aplRankRht EQ 0)
        aplDimNCols = aplDimNRows = 1;
    else
    {
        aplDimNCols = lpMemRhtDim[aplRankRht - 1];

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

    // Define a new FMTHEAD in the output
    ((LPFMTHEADER) lpwszFormat)->lpFmtHeadUp = NULL;
    lpFmtHeader = (LPFMTHEADER) lpwszFormat;
    lpFmtHeader->lpFmtRowUp  = NULL;                // No parent row struct
    lpFmtHeader->lpFmtColUp  = NULL;                // ...       col ...
    lpFmtHeader->uRows       = (UINT) aplDimNRows;
    lpFmtHeader->uCols       = (UINT) aplDimNCols;
    lpFmtHeader->uFmtRows    = 0;
    lpFmtHeader->uFmtInts    = 0;
    lpFmtHeader->uFmtFrcs    = 0;
    lpFmtHeader->uDepth      = 0;

    // Define <aplDimNCols> FMTCOLSTRs in the output
    lpFmtColStr = (LPFMTCOLSTR) (&lpFmtHeader[1]);
    lpFmtHeader->lpFmtCol1st = lpFmtColStr;
    ZeroMemory (lpFmtColStr, (UINT) aplDimNCols * sizeof (FMTCOLSTR));
    lpaplChar = lpaplCharStart = (LPAPLCHAR) &lpFmtColStr[aplDimNCols];

    // Loop through the array appending the formatted values (separated by L'\0')
    //   to the output vector, and accumulating the values in the appropriate
    //   FMTCOLSTR & FMTROWSTR entries.

    // If the right arg is a simple scalar,
    //   get its value
    bSimpleScalar = (aplRankRht EQ 0 && IsSimpleNH (aplTypeRht));
    if (bSimpleScalar)
        FirstValue (lptkRhtArg,
                   &aplIntegerRht,
                   &aplFloatRht,
                   &aplCharRht,
                    NULL,
                    NULL,
                    NULL,
                    NULL);
    // Split cases based upon the right arg's storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
////////////lpaplChar =
            CompileArrBool    (bSimpleScalar ? (LPAPLBOOL) &aplIntegerRht : (LPAPLBOOL)   lpMemRht,
                               lpFmtHeader,
                               NULL,
                               lpFmtColStr,
                               lpaplChar,
                               aplDimNRows,
                               aplDimNCols,
                               aplRankRht,
                               lpMemRhtDim);
            break;

        case ARRAY_INT:
////////////lpaplChar =
            CompileArrInteger (bSimpleScalar ? &aplIntegerRht : (LPAPLINT)    lpMemRht,
                               lpFmtHeader,
                               NULL,
                               lpFmtColStr,
                               lpaplChar,
                               aplDimNRows,
                               aplDimNCols,
                               aplRankRht,
                               lpMemRhtDim);
            break;

        case ARRAY_FLOAT:
////////////lpaplChar =
            CompileArrFloat   (bSimpleScalar ? &aplFloatRht : (LPAPLFLOAT)  lpMemRht,
                               lpFmtHeader,
                               NULL,
                               lpFmtColStr,
                               lpaplChar,
                               aplDimNRows,
                               aplDimNCols,
                               aplRankRht,
                               lpMemRhtDim);
            break;

        case ARRAY_CHAR:
            // Return the right arg

            // If the right arg is scalar,
            //   return it as an immediate
            if (aplRankRht EQ 0)
            {
                // Fill in the result token
                YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_VARIMMED;
                YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = IMMTYPE_CHAR;
////////////////YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;    // Already zero from ZeroMemory
                YYRes[YYLclIndex].tkToken.tkData.tkChar     = aplCharRht;
                YYRes[YYLclIndex].tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

                goto QUICK_EXIT;
            } else
            {
                // Make a copy of the right arg
                hGlbRes = CopySymGlbDirGlb (hGlbRht);

                goto NORMAL_EXIT;
            } // End IF/ELSE

        case ARRAY_APA:
////////////lpaplChar =
            CompileArrAPA     ((LPAPLAPA)    lpMemRht,
                               lpFmtHeader,
                               NULL,
                               lpFmtColStr,
                               lpaplChar,
                               aplDimNRows,
                               aplDimNCols,
                               aplRankRht,
                               lpMemRhtDim);
            break;

        case ARRAY_HETERO:
////////////lpaplChar =
            CompileArrHetero  ((LPAPLHETERO) lpMemRht,
                               lpFmtHeader,
                               NULL,
                               lpFmtColStr,
                               lpaplChar,
                               aplDimNRows,
                               aplDimNCols,
                               aplRankRht,
                               lpMemRhtDim);
            break;

        case ARRAY_NESTED:
////////////lpaplChar =
            CompileArrNested  ((LPAPLNESTED) lpMemRht,
                               lpFmtHeader,
                               NULL,
                               lpFmtColStr,
                               lpaplChar,
                               aplDimNRows,
                               aplDimNCols,
                               aplRankRht,
                               lpMemRhtDim);
            break;

        defstop
            break;
    } // End SWITCH

    // Propogate the row & col count up the line
    PropogateRowColCount (lpFmtHeader, lpFmtColStr, FALSE);

    // Add up the width of each column to get the
    //   # cols in the result
    for (aplLastDim = aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        aplLastDim += (lpFmtColStr[aplDimCol].uIntDigits + lpFmtColStr[aplDimCol].uFrcDigits);

    // Calculate the NELM of the result
    if (lpMemRhtDim && aplRankRht NE 0)
        for (aplNELMRes = aplLastDim, aplDimCol = 0; aplDimCol < (aplRankRht - 1); aplDimCol++)
            aplNELMRes *= lpMemRhtDim[aplDimCol];
    else
        aplNELMRes = 1;

    // ***FIXME*** -- Replace calculation for aplNELMRes with the product below
    //                and delete the code above
    Assert (aplNELMRes EQ (lpFmtHeader->uFmtRows * (lpFmtHeader->uFmtInts + lpFmtHeader->uFmtFrcs)));

    // Calculate the space needed for the result
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
    lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->Perm       = 0;           // Already zero from GHND
////lpHeader->SysVar     = 0;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;

#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);

    // Fill in all but the last dimension
    if (lpMemRhtDim && aplRankRht NE 0)
    for (aplDimCol = 0; aplDimCol < (aplRankRht - 1); aplDimCol++)
        *((LPAPLDIM) lpMemRes)++ = *lpMemRhtDim++;

    // Fill in the last dimension
    *((LPAPLDIM) lpMemRes)++ = aplLastDim;

    // lpMemRes now points to the data
#ifdef PREFILL
    // Fill it with all blanks
    for (lpaplChar = lpMemRes, aplDimCol = 0; aplDimCol < aplNELMRes; aplDimCol++)
        *lpaplChar++ = L' ';
#endif
    // Start over again
    lpaplChar = lpaplCharStart;

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
            FmtArrSimple (lpFmtColStr,
                          lpaplCharStart,
                          lpwszOut,
                          aplDimNRows,
                          aplDimNCols,
                          aplRankRht,
                          lpMemRhtDim,
                          FALSE);
            break;

        case ARRAY_NESTED:
////////////lpaplChar =
            FmtArrNested (lpMemRht,
                          lpFmtColStr,
                          lpaplCharStart,
                          lpwszOut,
                          aplDimNRows,
                          aplDimNCols,
                          FALSE);
            break;

        defstop
            break;
    } // End SWITCH

#undef  lpwszOut

NORMAL_EXIT:
    // Fill in the result token
    YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_VARARRAY;
////YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = 0;    // Already zero from ZeroMemory
////YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;    // Already zero from ZeroMemory
    YYRes[YYLclIndex].tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    YYRes[YYLclIndex].tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
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
        return &YYRes[YYLclIndex];
    else
        return NULL;
} // End PrimFnMonThorn_EM
#undef  APPEND_NAME


//***************************************************************************
//  FmtArrSimple
//
//  Format a simple array from its compiled form
//***************************************************************************

LPAPLCHAR FmtArrSimple
    (LPFMTCOLSTR lpFmtColStr,   // Ptr to ??
     LPAPLCHAR   lpaplChar,     // Ptr to compiled input
     LPAPLCHAR   lpwszOut,      // Ptr to output string
     APLDIM      aplDimNRows,   // # rows in this array
     APLDIM      aplDimNCols,   // # cols ...
     APLRANK     aplRank,       // Rank of this array
     LPAPLDIM    lpMemDim,      // Ptr to this array's dimensions (NULL for scalar)
     BOOL        bRawOutput)    // TRUE iff raw (not {thorn}) output

{
    APLDIM  aplDimRow,
            aplDimCol;
    UINT    uLen,
            uLen2,
            uCol,
            uWidth;
    LPWCHAR lpw;

    // Output the result at appropriate widths
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        Assert (((LPFMTROWSTR) lpaplChar)->uFmtRows EQ 1);

        // Skip over the FMTROWSTR as this is a simple array
        lpaplChar = (LPAPLCHAR) &((LPFMTROWSTR) lpaplChar)[1];

        for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        {
            // Split cases based upon the format type
            switch (lpFmtColStr[aplDimCol].uFmtType)
            {
                case FMTTYPE_INT:
                case FMTTYPE_FIX:
                case FMTTYPE_EXP:
                    // ***FIXME*** -- handle conversion from fixed point to exponential format
                    //                and line up the decimal points and the Es

                    // Get the # digits plus leading space, decimal point, and fractional part
                    uWidth = lpFmtColStr[aplDimCol].uIntDigits + lpFmtColStr[aplDimCol].uFrcDigits;
                    uLen = lstrlenW (lpaplChar);

                    Assert (uLen <= uWidth);

                    // Check for fractional part
                    lpw = strchrW (lpaplChar, L'.');
                    if (!lpw)
                        lpw = lpaplChar + uLen;

                    // Align the decimal points
                    uCol = (lpw - lpaplChar) + lpFmtColStr[aplDimCol].uFrcDigits;
                    uLen2 = uWidth - max (uLen, uCol);

                    // If this is raw output,
                    // break the line if it would exceed []PW
                    //   and the line is non-empty.
                    uCol = lpwszOut - lpwszTemp;
                    if (bRawOutput
                     && DEF_INDENT < uCol
                     && uQuadPW < (uWidth + uCol))
                    {
                        // Ensure properly terminated
                        *lpwszOut = L'\0';

                        // Output the line
                        AppendLine (lpwszTemp, TRUE);

                        // Reset the line start
                        lpwszOut  = lpwszTemp;


#ifdef PREFILL
                        // Skip over leading indent
                        lpwszOut += DEF_INDENT;
#else
                        // Fill with leading indent
                        for (uCol = 0; uCol < DEF_INDENT; uCol++)
                            *lpwszOut++ = L' ';
#endif
                    } else
#ifdef PREFILL
                        // Skip over leading blanks
                        lpwszOut += uLen2;
#else
                        // Fill with leading blanks
                        for (uCol = 0; uCol < uLen2; uCol++)
                            *lpwszOut++ = L' ';
#endif
                    break;

                defstop
                    break;
            } // End SWITCH

            // Copy the next value
            CopyMemory (lpwszOut, lpaplChar, uLen * sizeof (APLCHAR));
            lpwszOut += uLen;   // Skip over the formatted string
            uCol     += uLen;   // ...

#ifdef PREFILL
            // Skip over trailing blanks
            Assert (uCol <= uWidth);
            lpwszOut += (uWidth - uCol);
#else
            // Fill with trailing blanks
            for (; uCol < uWidth; uCol++)
                *lpwszOut++ = L' ';
#endif
            lpaplChar += (uLen + 1);

            // If this is raw output, output it
            if (bRawOutput)
            {
                // Ensure properly terminated
                *lpwszOut = L'\0';

                // Output the line
                AppendLine (lpwszTemp, FALSE);

                // Reset the line start
                lpwszOut  = lpwszTemp;

                // Handle blank lines between planes
                if (aplDimRow NE 0                      // Not first row, and
                 && aplDimRow NE (aplDimNRows - 1))     // not last row
                {
                    APLDIM aplDimAcc;

                    aplDimAcc = 1;
                    for (uCol = 0; uCol < (aplRank - 1); uCol++)
                    {
                        aplDimAcc *= lpMemDim[(aplRank - 2) - uCol];
                        if ((aplDimRow + 1) % aplDimAcc)
                            break;

                        AppendLine (L"", FALSE);
                    } // End FOR
                } // End IF
            } // End IF
        } // End FOR
    } // End FOR

    return lpaplChar;
} // End FmtArrSimple


//***************************************************************************
//  FmtArrNested
//
//  Format a nested array from its compiled form
//***************************************************************************

LPAPLCHAR FmtArrNested
    (LPVOID      lpMem,         // Ptr to raw input
     LPFMTCOLSTR lpFmtColStr,   // Ptr to ???
     LPAPLCHAR   lpaplChar,     // Ptr to compiled input
     LPAPLCHAR   lpwszOut,      // Ptr to output string
     APLDIM      aplDimNRows,   // # rows in this array
     APLDIM      aplDimNCols,   // # cols ...
     BOOL        bRawOutput)    // TRUE iff raw (not {thorn}) output

{
    APLDIM    aplDimRow,
              aplDimCol;
    LPAPLCHAR lpwszOutRow;

    DbgBrk ();      // ***TESTME***

    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // ***FIXME*** -- For the moment, skip over the FMTROWSTR
        lpaplChar = (LPAPLCHAR) &((LPFMTROWSTR) lpaplChar)[1];

        // Save as start of this row
        lpwszOutRow = lpwszOut;

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
                        case IMMTYPE_INT:
                        case IMMTYPE_FLOAT:
                        case IMMTYPE_CHAR:
                            lpaplChar =
                            FmtArrSimple (&lpFmtColStr[aplDimCol],  // Ptr to ??
                                          lpaplChar,                // Ptr to compiled input
                                          lpwszOut,                 // Ptr to output string
                                          1,                        // # rows in this array
                                          1,                        // # cols in this array
                                          0,                        // Rank of this array
                                          NULL,                     // Ptr to this array's dimensions
                                          FALSE);                   // TRUE iff raw output
                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case PTRTYPE_HGLOBAL:
                    lpaplChar =
                    FmtArrNestedGlb (ClrPtrTypeIndGlb (lpMem),
                                     lpaplChar,
                                     lpwszOut,
                                     lpwszOutRow,
                                    &lpFmtColStr[aplDimCol]);
                    break;

                defstop
                    break;
            } // End SWITCH
        } // End FOR

        // If raw output, output it
        if (bRawOutput)
        {
            // Ensure properly terminated
            *lpwszOut = L'\0';

            // Output the line
            AppendLine (lpwszTemp, FALSE);

            // Reset the line start
            lpwszOut  = lpwszTemp;

            // Note that all nested arrays are formatted as
            //   matrices, so there is no interdimensional spacing.
        } // End IF
    } // End FOR

    return lpaplChar;
} // End FmtArrNested


//***************************************************************************
//  FmtArrNestedGlb
//
//  Format a nested array global memory object from its compiled form
//***************************************************************************

LPAPLCHAR FmtArrNestedGlb
    (HGLOBAL     hGlb,
     LPAPLCHAR   lpaplChar,
     LPAPLCHAR   lpwszOut,
     LPAPLCHAR   lpwszOutRow,
     LPFMTCOLSTR lpFmtColStr)

{
    APLSTYPE aplType;
    APLNELM  aplNELM;
    APLRANK  aplRank;
    LPVOID   lpMem;
    LPAPLDIM lpMemDim;
    APLDIM   aplDimNRows,
             aplDimNCols;

    // Get attrs (Type, NELM, Rank) of the global
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

    DbgBrk ();      // ***TESTME***

    // ***FIXME*** -- For the moment, skip over the FMTHEADER
    lpaplChar = (LPAPLCHAR) &((LPFMTHEADER) lpaplChar)[1];

    // ***FIXME*** -- For the moment, skip over the FMTCOLSTRs
    lpaplChar = (LPAPLCHAR) &((LPFMTCOLSTR) lpaplChar)[aplDimNCols];

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
            FmtArrSimple (lpFmtColStr,
                          lpaplChar,
                          lpwszOut,
                          aplDimNRows,
                          aplDimNCols,
                          aplRank,
                          lpMemDim,
                          FALSE);
            break;

        case ARRAY_NESTED:
            lpaplChar =
            FmtArrNested (lpMem,
                          lpFmtColStr,
                          lpaplChar,
                          lpwszOut,
                          aplDimNRows,
                          aplDimNCols,
                          FALSE);
            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlb); lpMem = NULL;

    return lpaplChar;
} // End FmtArrNestedGlb


//***************************************************************************
//  CompileArrBool
//
//  Compile an array of Booleans
//***************************************************************************

LPAPLCHAR CompileArrBool
    (LPAPLBOOL   lpMem,         // Ptr to data to format
     LPFMTHEADER lpFmtHeader,   // Ptr to parent FMTHEADER
     LPFMTROWSTR lpFmtRowStr,   // Ptr to this row's FMTROWSTR
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of aplDimNCols FMTCOLSTRs
     LPAPLCHAR   lpaplChar,     // Ptr to next available format position
     APLDIM      aplDimNRows,   // # rows to format (actually it's x/ all but last dim)
     APLDIM      aplDimNCols,   // # cols to format
     APLRANK     aplRank,       // Rank of data to format
     LPAPLDIM    lpMemDim)      // Ptr to dimension vector

{
    UINT        uBitMask,
                uLen;
    APLDIM      aplDimCol,
                aplDimRow;
    LPFMTROWSTR lpFmtRowLcl = NULL;

    // Save ptr to 1st child FMTROWSTR
    if (aplDimNRows)
        lpFmtHeader->lpFmtRow1st = (LPFMTROWSTR) lpaplChar;
    else
        lpFmtHeader->lpFmtRow1st = NULL;

    for (aplDimCol = 1; aplDimCol < aplDimNCols; aplDimCol++)
    {
        // Max the current width with the width of this number
        lpFmtColStr[aplDimCol].uIntDigits = max (lpFmtColStr[aplDimCol].uIntDigits, 2);

////////// Max the current format type with FMT_INT
////////lpFmtColStr[aplDimCol].uFmtType = max (lpFmtColStr[aplDimCol].uFmtType, FMTTYPE_INT);
    } // End FOR

    // Add in a leading blank unless scalar/vector
    uLen = 1 + (aplRank > 1);
    lpFmtColStr[0].uIntDigits = max (lpFmtColStr[0].uIntDigits, uLen);

    uBitMask = 0x01;
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
        lpFmtRowLcl->uBlank      = FALSE;
        lpFmtRowLcl->uFmtRows    = 1;
////////lpFmtRowLcl->lpFmtRowNxt = NULL;
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

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
    lpFmtRowLcl->lpFmtRowNxt = NULL;

    return lpaplChar;
} // End CompileArrBool


//***************************************************************************
//  CompileArrInteger
//
//  Compile an array of integers
//***************************************************************************

LPAPLCHAR CompileArrInteger
    (LPAPLINT    lpMem,         // Ptr to data to format
     LPFMTHEADER lpFmtHeader,   // Ptr to parent FMTHEADER
     LPFMTROWSTR lpFmtRowStr,   // Ptr to this row's FMTROWSTR
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of aplDimNCols FMTCOLSTRs
     LPAPLCHAR   lpaplChar,     // Ptr to next available format position
     APLDIM      aplDimNRows,   // # rows to format (actually it's x/ all but last dim)
     APLDIM      aplDimNCols,   // # cols to format
     APLRANK     aplRank,       // Rank of data to format
     LPAPLDIM    lpMemDim)      // Ptr to dimension vector

{
    UINT        uLen;
    APLDIM      aplDimCol,
                aplDimRow;
    LPAPLCHAR   lpwszOut;
    LPFMTROWSTR lpFmtRowLcl;

    // Save ptr to 1st child FMTROWSTR
    if (aplDimNRows)
        lpFmtHeader->lpFmtRow1st = (LPFMTROWSTR) lpaplChar;
    else
        lpFmtHeader->lpFmtRow1st = NULL;

    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
        lpFmtRowLcl->uBlank      = FALSE;
        lpFmtRowLcl->uFmtRows    = 1;
////////lpFmtRowLcl->lpFmtRowNxt = NULL;
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        {
            // Format the integer
            lpaplChar =
            FormatAplint (lpwszOut = lpaplChar,
                          *lpMem++);
            // Zap the trailing blank
            lpaplChar[-1] = L'\0';

            // Calculate the string length
            uLen = (lpaplChar - lpwszOut) - 1;

            // Add in a leading blank unless scalar/vector and 1st col
            uLen += (aplRank > 1) || aplDimCol NE 0;

            // Max the current width with this
            lpFmtColStr[aplDimCol].uIntDigits = max (lpFmtColStr[aplDimCol].uIntDigits, uLen);

////////////// Max the current format type with FMT_INT
////////////lpFmtColStr[aplDimCol].uFmtType = max (lpFmtColStr[aplDimCol].uFmtType, FMT_TYPEINT);
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
    lpFmtRowLcl->lpFmtRowNxt = NULL;

    return lpaplChar;
} // End CompileArrInteger


//***************************************************************************
//  CompileArrFloat
//
//  Compile an array of floats
//***************************************************************************

LPAPLCHAR CompileArrFloat
    (LPAPLFLOAT  lpMem,         // Ptr to data to format
     LPFMTHEADER lpFmtHeader,   // Ptr to parent FMTHEADER
     LPFMTROWSTR lpFmtRowStr,   // Ptr to this row's FMTROWSTR
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of aplDimNCols FMTCOLSTRs
     LPAPLCHAR   lpaplChar,     // Ptr to next available format position
     APLDIM      aplDimNRows,   // # rows to format (actually it's x/ all but last dim)
     APLDIM      aplDimNCols,   // # cols to format
     APLRANK     aplRank,       // Rank of data to format
     LPAPLDIM    lpMemDim)      // Ptr to dimension vector

{
    UINT        uLen;
    APLDIM      aplDimCol,
                aplDimRow;
    LPAPLCHAR   lpwszOut;
    LPWCHAR     lpwsz;
    LPFMTROWSTR lpFmtRowLcl;

    // Save ptr to 1st child FMTROWSTR
    if (aplDimNRows)
        lpFmtHeader->lpFmtRow1st = (LPFMTROWSTR) lpaplChar;
    else
        lpFmtHeader->lpFmtRow1st = NULL;

    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
        lpFmtRowLcl->uBlank      = FALSE;
        lpFmtRowLcl->uFmtRows    = 1;
////////lpFmtRowLcl->lpFmtRowNxt = NULL;
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        {
            // Format the float
            lpaplChar =
            FormatFloat (lpwszOut = lpaplChar,
                         *((LPAPLFLOAT) lpMem)++);
            // Zap the trailing blank
            lpaplChar[-1] = L'\0';

            // Check for exponential format
            lpwsz = strchrW (lpwszOut, L'E');

            // If present, mark as exponential
            if (lpwsz)
                lpFmtColStr[aplDimCol].uFmtType = max (lpFmtColStr[aplDimCol].uFmtType, FMTTYPE_EXP);

            // Check for decimal point
            lpwsz = strchrW (lpwszOut, L'.');
            if (lpwsz)
            {
                // Calculate the length of integer part
                uLen = (lpwsz - lpwszOut);

                // Add in a leading blank unless scalar/vector and 1st col
                uLen += (aplRank > 1) || aplDimCol NE 0;

                // Max the current width with this
                lpFmtColStr[aplDimCol].uIntDigits = max (lpFmtColStr[aplDimCol].uIntDigits, uLen);

                // Calculate the length of the fractional part
                //   including the decimal point
                uLen = (lpaplChar - lpwsz) - 1;

                // Max the current fractional part with this
                lpFmtColStr[aplDimCol].uFrcDigits = max (lpFmtColStr[aplDimCol].uFrcDigits, uLen);

                // Max the current format type with FMTTYPE_FIX
                lpFmtColStr[aplDimCol].uFmtType = max (lpFmtColStr[aplDimCol].uFmtType, FMTTYPE_FIX);
            } else
            {
                // Calculate the string length
                uLen = (lpaplChar - lpwszOut) - 1;

                // Add in a leading blank unless scalar/vector and 1st col
                uLen += (aplRank > 1) || aplDimCol NE 0;

                // Max the current width with this
                lpFmtColStr[aplDimCol].uIntDigits = max (lpFmtColStr[aplDimCol].uIntDigits, uLen);
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
    lpFmtRowLcl->lpFmtRowNxt = NULL;

    return lpaplChar;
} // End CompileArrFloat


//***************************************************************************
//  CompileArrChar
//
//  Compile an array of chars
//***************************************************************************

LPAPLCHAR CompileArrChar
    (LPAPLCHAR   lpMem,         // Ptr to data to format
     LPFMTHEADER lpFmtHeader,   // Ptr to parent FMTHEADER
     LPFMTROWSTR lpFmtRowStr,   // Ptr to this row's FMTROWSTR
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of aplDimNCols FMTCOLSTRs
     LPAPLCHAR   lpaplChar,     // Ptr to next available format position
     APLDIM      aplDimNRows,   // # rows to format (actually it's x/ all but last dim)
     APLDIM      aplDimNCols,   // # cols to format
     APLRANK     aplRank,       // Rank of data to format
     LPAPLDIM    lpMemDim)      // Ptr to dimension vector

{
    APLDIM      aplDimCol,
                aplDimRow;
    LPFMTROWSTR lpFmtRowLcl;

    // Save ptr to 1st child FMTROWSTR
    if (aplDimNRows)
        lpFmtHeader->lpFmtRow1st = (LPFMTROWSTR) lpaplChar;
    else
        lpFmtHeader->lpFmtRow1st = NULL;

    for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
    {
        // Max the current width with the width of this char
        lpFmtColStr[aplDimCol].uIntDigits = max (lpFmtColStr[aplDimCol].uIntDigits, 1);
    } // End FOR

    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
        lpFmtRowLcl->uBlank      = FALSE;
        lpFmtRowLcl->uFmtRows    = 1;
////////lpFmtRowLcl->lpFmtRowNxt = NULL;
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        {
            // Append the char
            *lpaplChar++ = *lpMem++;
            *lpaplChar++ = L'\0';
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
    lpFmtRowLcl->lpFmtRowNxt = NULL;

    return lpaplChar;
} // End CompileArrChar


//***************************************************************************
//  CompileArrAPA
//
//  Compile an array of APA
//***************************************************************************

LPAPLCHAR CompileArrAPA
    (LPAPLAPA    lpMem,         // Ptr to data to format
     LPFMTHEADER lpFmtHeader,   // Ptr to parent FMTHEADER
     LPFMTROWSTR lpFmtRowStr,   // Ptr to this row's FMTROWSTR
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of aplDimNCols FMTCOLSTRs
     LPAPLCHAR   lpaplChar,     // Ptr to next available format position
     APLDIM      aplDimNRows,   // # rows to format (actually it's x/ all but last dim)
     APLDIM      aplDimNCols,   // # cols to format
     APLRANK     aplRank,       // Rank of data to format
     LPAPLDIM    lpMemDim)      // Ptr to dimension vector

{
    APLINT      apaOff,
                apaMul,
                apaAcc;
    UINT        uLen;
    APLDIM      aplDimCol,
                aplDimRow;
    LPAPLCHAR   lpwszOut;
    LPFMTROWSTR lpFmtRowLcl;

    // Save ptr to 1st child FMTROWSTR
    if (aplDimNRows)
        lpFmtHeader->lpFmtRow1st = (LPFMTROWSTR) lpaplChar;
    else
        lpFmtHeader->lpFmtRow1st = NULL;

    // Get the APA parameters
    apaOff = lpMem->Off;
    apaMul = lpMem->Mul;

    for (aplDimRow = apaAcc = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
        lpFmtRowLcl->uBlank      = FALSE;
        lpFmtRowLcl->uFmtRows    = 1;
////////lpFmtRowLcl->lpFmtRowNxt = NULL;
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++, apaAcc++)
        {
            // Format the integer
            lpaplChar =
            FormatAplint (lpwszOut = lpaplChar,
                          apaOff + apaMul * apaAcc);
            // Zap the trailing blank
            lpaplChar[-1] = L'\0';

            // Calculate the string length
            uLen = (lpaplChar - lpwszOut) - 1;

            // Add in a leading blank unless scalar/vector and 1st col
            uLen += (aplRank > 1) || aplDimCol NE 0;

            // Max the current width with this
            lpFmtColStr[aplDimCol].uIntDigits = max (lpFmtColStr[aplDimCol].uIntDigits, uLen);
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
    lpFmtRowLcl->lpFmtRowNxt = NULL;

    return lpaplChar;
} // End CompileArrAPA


//***************************************************************************
//  CompileArrHetero
//
//  Compile an array of LPSYMENTRYs
//***************************************************************************

LPAPLCHAR CompileArrHetero
    (LPAPLHETERO lpMem,         // Ptr to data to format
     LPFMTHEADER lpFmtHeader,   // Ptr to parent FMTHEADER
     LPFMTROWSTR lpFmtRowStr,   // Ptr to this row's FMTROWSTR
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of aplDimNCols FMTCOLSTRs
     LPAPLCHAR   lpaplChar,     // Ptr to next available format position
     APLDIM      aplDimNRows,   // # rows to format (actually it's x/ all but last dim)
     APLDIM      aplDimNCols,   // # cols to format
     APLRANK     aplRank,       // Rank of data to format
     LPAPLDIM    lpMemDim)      // Ptr to dimension vector

{
    UINT        uLen;
    APLDIM      aplDimCol,
                aplDimRow;
    LPAPLCHAR   lpwszOut;
    LPFMTROWSTR lpFmtRowLcl;

    // Save ptr to 1st child FMTROWSTR
    if (aplDimNRows)
        lpFmtHeader->lpFmtRow1st = (LPFMTROWSTR) lpaplChar;
    else
        lpFmtHeader->lpFmtRow1st = NULL;

    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
        lpFmtRowLcl->uBlank      = FALSE;
        lpFmtRowLcl->uFmtRows    = 1;
////////lpFmtRowLcl->lpFmtRowNxt = NULL;
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
        {

#define lpSymEntry      ((LPSYMENTRY *) lpMem)

            lpaplChar =
            FormatImmed (lpwszOut = lpaplChar,
                         (*lpSymEntry)->stFlags.ImmType,
                        &(*lpSymEntry)->stData.lpVoid);

            // Skip to next entry
            lpSymEntry++;

#undef  lpSymEntry

            // Zap the trailing blank
            lpaplChar[-1] = L'\0';

            // Calculate the string length
            uLen = (lpaplChar - lpwszOut) - 1;

            // Add in a leading blank unless scalar/vector and 1st col
            uLen += (aplRank > 1) || aplDimCol NE 0;

            // Max the current width with this
            lpFmtColStr[aplDimCol].uIntDigits = max (lpFmtColStr[aplDimCol].uIntDigits, uLen);

////////////// Max the current format type with FMT_INT
////////////lpFmtColStr[aplDimCol].uFmtType = max (lpFmtColStr[aplDimCol].uFmtType, FMT_TYPEINT);
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
    lpFmtRowLcl->lpFmtRowNxt = NULL;

    return lpaplChar;
} // End CompileArrHetero


//***************************************************************************
//  CompileArrNested
//
//  Compile an array of LPSYMENTRYs/NESTEDs
//***************************************************************************

LPAPLCHAR CompileArrNested
    (LPAPLNESTED lpMem,         // Ptr to data to format
     LPFMTHEADER lpFmtHeader,   // Ptr to parent FMTHEADER
     LPFMTROWSTR lpFmtRowStr,   // Ptr to this row's FMTROWSTR
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of aplDimNCols FMTCOLSTRs
     LPAPLCHAR   lpaplChar,     // Ptr to next available format position
     APLDIM      aplDimNRows,   // # rows to format (actually it's x/ all but last dim)
     APLDIM      aplDimNCols,   // # cols to format
     APLRANK     aplRank,       // Rank of data to format
     LPAPLDIM    lpMemDim)      // Ptr to dimension vector

{
    APLDIM      aplDimCol,
                aplDimRow;
    LPFMTROWSTR lpFmtRowLcl;

    // Save ptr to 1st child FMTROWSTR
    if (aplDimNRows)
        lpFmtHeader->lpFmtRow1st = (LPFMTROWSTR) lpaplChar;
    else
        lpFmtHeader->lpFmtRow1st = NULL;

    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
        lpFmtRowLcl->uBlank      = FALSE;
        lpFmtRowLcl->uFmtRows    = 0;       // Initialize the count
        lpFmtRowLcl->lpFmtRowNxt = NULL;
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

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
                            CompileArrBool   (&(*(LPSYMENTRY *) lpMem)->stData.stBoolean,
                                           lpFmtHeader,
                                           lpFmtRowLcl,
                                          &lpFmtColStr[aplDimCol],
                                           lpaplChar,
                                           1,
                                           1,
                                           0,
                                           NULL);
                            break;

                        case IMMTYPE_INT:
                            lpaplChar =
                            CompileArrInteger (&(*(LPSYMENTRY *) lpMem)->stData.stInteger,
                                           lpFmtHeader,
                                           lpFmtRowLcl,
                                          &lpFmtColStr[aplDimCol],
                                           lpaplChar,
                                           1,
                                           1,
                                           0,
                                           NULL);
                            break;

                        case IMMTYPE_FLOAT:
                            lpaplChar =
                            CompileArrFloat   (&(*(LPSYMENTRY *) lpMem)->stData.stFloat,
                                           lpFmtHeader,
                                           lpFmtRowLcl,
                                          &lpFmtColStr[aplDimCol],
                                           lpaplChar,
                                           1,
                                           1,
                                           0,
                                           NULL);
                            break;

                        case IMMTYPE_CHAR:
                            lpaplChar =
                            CompileArrChar    (&(*(LPSYMENTRY *) lpMem)->stData.stChar,
                                           lpFmtHeader,
                                           lpFmtRowLcl,
                                          &lpFmtColStr[aplDimCol],
                                           lpaplChar,
                                           1,
                                           1,
                                           0,
                                           NULL);
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
    lpFmtRowLcl->lpFmtRowNxt = NULL;

////// Propogate the row & col count up the line
////PropogateRowColCount (lpFmtHeader, lpFmtColStr, FALSE);

    return lpaplChar;
} // End CompileArrNested


//***************************************************************************
//  CompileArrNestedGlb
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
    APLSTYPE aplType;
    APLNELM  aplNELM;
    APLRANK  aplRank;
    LPVOID   lpMem;
    LPAPLDIM lpMemDim;
    APLDIM   aplDimNRows,
             aplDimNCols;
    BOOL     bSimpleScalar;
    APLINT   aplInteger;
    APLFLOAT aplFloat;
    APLCHAR  aplChar;

    // Get attrs (Type, NELM, Rank) of the global
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

    // Define a new FMTHEAD in the output
    ((LPFMTHEADER) lpaplChar)->lpFmtHeadUp = lpFmtHeader;
    lpFmtHeader = (LPFMTHEADER) lpaplChar;
    lpFmtHeader->lpFmtRowUp  = lpFmtRowStr;
    lpFmtHeader->lpFmtColUp  = lpFmtColStr;
    lpFmtHeader->uRows       = (UINT) aplDimNRows;
    lpFmtHeader->uCols       = (UINT) aplDimNCols;
    lpFmtHeader->uFmtRows    = 0;
    lpFmtHeader->uFmtInts    = 0;
    lpFmtHeader->uFmtFrcs    = 0;
    lpFmtHeader->uDepth      = 1 + lpFmtHeader->lpFmtHeadUp->uDepth;

    // Define <aplDimNCols> FMTCOLSTRs in the output
    lpFmtColStr  = (LPFMTCOLSTR) (&lpFmtHeader[1]);
    lpFmtHeader->lpFmtCol1st = lpFmtColStr;
    ZeroMemory (lpFmtColStr, (UINT) aplDimNCols * sizeof (FMTCOLSTR));
    lpaplChar = (LPAPLCHAR) (&lpFmtColStr[aplDimNCols]);

    // Loop through the array appending the formatted values (separated by L'\0')
    //   to the output vector, and accumulating the values in the appropriate
    //   FMTCOLSTR & FMTROWSTR entries.

    // If the right arg is a simple scalar,
    //   get its value
    bSimpleScalar = (aplRank EQ 0 && IsSimpleNH (aplType));
    if (bSimpleScalar)
        FirstValueGlb (hGlb,
                      &aplInteger,
                      &aplFloat,
                      &aplChar,
                       NULL,
                       NULL,
                       NULL,
                       NULL);
    // Split cases based upon the right arg's storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
            lpaplChar =
            CompileArrBool    (bSimpleScalar ? (LPAPLBOOL) &aplInteger : (LPAPLBOOL)   lpMem,
                               lpFmtHeader,
                               lpFmtRowStr,
                               lpFmtColStr,
                               lpaplChar,
                               aplDimNRows,
                               aplDimNCols,
                               aplRank,
                               lpMemDim);
            break;

        case ARRAY_INT:
            lpaplChar =
            CompileArrInteger (bSimpleScalar ? &aplInteger : (LPAPLINT)    lpMem,
                               lpFmtHeader,
                               lpFmtRowStr,
                               lpFmtColStr,
                               lpaplChar,
                               aplDimNRows,
                               aplDimNCols,
                               aplRank,
                               lpMemDim);
            break;

        case ARRAY_FLOAT:
            lpaplChar =
            CompileArrFloat   (bSimpleScalar ? &aplFloat : (LPAPLFLOAT)  lpMem,
                               lpFmtHeader,
                               lpFmtRowStr,
                               lpFmtColStr,
                               lpaplChar,
                               aplDimNRows,
                               aplDimNCols,
                               aplRank,
                               lpMemDim);
            break;

        case ARRAY_CHAR:
            lpaplChar =
            CompileArrChar    (bSimpleScalar ? &aplChar : (LPAPLCHAR)  lpMem,
                               lpFmtHeader,
                               lpFmtRowStr,
                               lpFmtColStr,
                               lpaplChar,
                               aplDimNRows,
                               aplDimNCols,
                               aplRank,
                               lpMemDim);
            break;

        case ARRAY_APA:
            lpaplChar =
            CompileArrAPA     ((LPAPLAPA)    lpMem,
                               lpFmtHeader,
                               lpFmtRowStr,
                               lpFmtColStr,
                               lpaplChar,
                               aplDimNRows,
                               aplDimNCols,
                               aplRank,
                               lpMemDim);
            break;

        case ARRAY_HETERO:
            lpaplChar =
            CompileArrHetero  ((LPAPLHETERO) lpMem,
                               lpFmtHeader,
                               lpFmtRowStr,
                               lpFmtColStr,
                               lpaplChar,
                               aplDimNRows,
                               aplDimNCols,
                               aplRank,
                               lpMemDim);
            break;

        case ARRAY_NESTED:
            lpaplChar =
            CompileArrNested  ((LPAPLNESTED) lpMem,
                               lpFmtHeader,
                               lpFmtRowStr,
                               lpFmtColStr,
                               lpaplChar,
                               aplDimNRows,
                               aplDimNCols,
                               aplRank,
                               lpMemDim);
            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlb); lpMem = NULL;

    // Propogate the row & col count up the line
    PropogateRowColCount (lpFmtHeader, lpFmtColStr, TRUE);

    return lpaplChar;
} // End CompileArrNestedGlb


//***************************************************************************
//  PropogateRowColCount
//
//  Propogate the row & col count up the line
//***************************************************************************

void PropogateRowColCount
    (LPFMTHEADER lpFmtHeader,       // Ptr to FMTHEADER struc
     LPFMTCOLSTR lpFmtColStr,       // ...    FMTCOLSTR struc
     BOOL        bNested)           // TRUE iff nested (to be surrounded by blanks)

{
    UINT        uAcc,
                uInt,
                uFrc,
                uCol;
    LPFMTROWSTR lpFmtRowLcl;

    // Count the # rows in this block
    for (lpFmtRowLcl = lpFmtHeader->lpFmtRow1st,
           uAcc = 0;
         lpFmtRowLcl NE NULL;
         lpFmtRowLcl = lpFmtRowLcl->lpFmtRowNxt)
        uAcc += lpFmtRowLcl->uFmtRows;

    // Save in row & head strucs
    if (lpFmtHeader->lpFmtRowUp)
        lpFmtHeader->lpFmtRowUp->uFmtRows = max (lpFmtHeader->lpFmtRowUp->uFmtRows, uAcc);
    lpFmtHeader->uFmtRows = max (lpFmtHeader->uFmtRows, uAcc);

    // Count the col width in this block
    for (uInt = uFrc = uCol = 0; uCol < lpFmtHeader->uCols; uCol++)
    {
        uInt += lpFmtHeader->lpFmtCol1st[uCol].uIntDigits;
        uFrc += lpFmtHeader->lpFmtCol1st[uCol].uFrcDigits;
    } // End FOR

    if (bNested)
    {
        // Surround this nested item with one column of blanks
        //   on either side
        uInt++;
        uFrc++;
    } // End IF

    // Save in col & head strucs
    if (lpFmtHeader->lpFmtColUp)
    {
        lpFmtHeader->lpFmtColUp->uIntDigits = max (lpFmtHeader->lpFmtColUp->uIntDigits, uInt);
        lpFmtHeader->lpFmtColUp->uFrcDigits = max (lpFmtHeader->lpFmtColUp->uFrcDigits, uFrc);
    } // End IF

    lpFmtHeader->uFmtInts = max (lpFmtHeader->uFmtInts, uInt);
    lpFmtHeader->uFmtFrcs = max (lpFmtHeader->uFmtFrcs, uFrc);

    // Recurse
    if (lpFmtHeader->lpFmtHeadUp)
        PropogateRowColCount (lpFmtHeader->lpFmtHeadUp,
                              lpFmtHeader->lpFmtColUp,
                              FALSE);
} // End PropogateRowColCount


//***************************************************************************
//  AppendBlankRows
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
        lpFmtRowLcl->uBlank   = TRUE;
        lpFmtRowLcl->uFmtRows = 1;
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Save as ptr to next row struc
        lpFmtRowLcl->lpFmtRowNxt = (LPFMTROWSTR) lpaplChar;
    } // End FOR

    return lpaplChar;
} // End AppendBlankRows


//***************************************************************************
//  PrimFnDydThorn_EM
//
//  Primitive function for dyadic Thorn ("format by example")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydThorn_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnDydThorn_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    APLSTYPE aplTypeLft,
             aplTypeRht;
    APLNELM  aplNELMLft,
             aplNELMRht;
    APLRANK  aplRankLft,
             aplRankRht;
    HGLOBAL  hGlbLft,
             hGlbRht;
    LPVOID   lpMemLft,
             lpMemRht;
    BOOL     bRet = TRUE;
    UINT     YYLclIndex;

    // Get new index into YYRes
    YYLclIndex = NewYYResIndex ();

    DbgBrk ();          // ***FINISHME***

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Get left and right arg's global ptrs
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Check for RANK ERROR
    if (aplRankLft NE 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkLftArg);
        bRet = FALSE;

        goto ERROR_EXIT;
    } // End IF















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
        return &YYRes[YYLclIndex];
    else
        return NULL;
} // End PrimFnDydThorn_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_thorn.c
//***************************************************************************
