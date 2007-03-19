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

#ifdef DEBUG
    // Fill lpwszFormat with FFs so we can tell what we actually wrote
    FillMemory (lpwszFormat, 1024, 0xFF);
#endif

    // Create a new FMTHEAD in the output
    ZeroMemory ((LPFMTHEADER) lpwszFormat, sizeof (FMTHEADER));
    ((LPFMTHEADER) lpwszFormat)->lpFmtHeadUp = NULL;
    lpFmtHeader = (LPFMTHEADER) lpwszFormat;
#ifdef DEBUG
    lpFmtHeader->Signature   = FMTHEADER_SIGNATURE;
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
////lpFmtHeader->uFmtTrBl    = 0;                   // ...
////lpFmtHeader->uDepth      = 0;                   // ...

    // Create <aplDimNCols> FMTCOLSTRs in the output
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
    Assert (aplLastDim EQ (lpFmtHeader->uFmtInts
                         + lpFmtHeader->uFmtFrcs
                         + lpFmtHeader->uFmtTrBl));
    // Calculate the NELM of the result
    aplNELMRes = (lpFmtHeader->uFmtRows * (lpFmtHeader->uFmtInts
                                         + lpFmtHeader->uFmtFrcs
                                         + lpFmtHeader->uFmtTrBl));
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
        if (lpMemRhtDim && aplRankRht NE 0)
        for (aplDimCol = 0; aplDimCol < (aplRankRht - 1); aplDimCol++)
            *((LPAPLDIM) lpMemRes)++ = *lpMemRhtDim++;
    } // End IF

    // Fill in the last dimension
    *((LPAPLDIM) lpMemRes)++ = aplLastDim;

    // lpMemRes now points to the data
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
            FmtArrSimple (lpFmtHeader,              // Prt to FMTHEADER
                          lpFmtColStr,              // Ptr to vector of <aplDimNCols> FMTCOLSTRs
                          lpaplCharStart,           // Ptr to compiled input
                         &lpwszOut,                 // Ptr to output string
                          aplDimNRows,              // # rows in this array
                          aplDimNCols,              // # cols in this array
                          aplLastDim,               // Length of last dim in result
                          aplRankRht,               // Rank of this array
                          lpMemRhtDim,              // Ptr to this array's dimensions
                          FALSE);                   // TRUE iff raw output
            break;

        case ARRAY_NESTED:
////////////lpaplChar =
            FmtArrNested (lpFmtHeader,              // Ptr to FMTHEADER
                          lpMemRht,                 // Ptr to raw input
                          lpFmtColStr,              // Ptr to vector of <aplDimNCols> FMTCOLSTRs
                          lpaplCharStart,           // Ptr to compiled input
                         &lpwszOut,                 // Ptr to ptr to output string
                          aplDimNRows,              // # rows in this array
                          aplDimNCols,              // # cols ...
                          aplLastDim,               // Length of last dim in result
                          FALSE);                   // TRUE iff raw (not {thorn}) output
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
    (LPFMTHEADER lpFmtHeader,   // Ptr to FMTHEADER
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of aplDimNCols FMTCOLSTRs
     LPAPLCHAR   lpaplChar,     // Ptr to compiled input
     LPAPLCHAR  *lplpwszOut,    // Ptr to ptr to output string
     APLDIM      aplDimNRows,   // # rows in this array
     APLDIM      aplDimNCols,   // # cols ...
     APLDIM      aplLastDim,    // Length of the last dimension in the result
     APLRANK     aplRank,       // Rank of this array
     LPAPLDIM    lpMemDim,      // Ptr to this array's dimensions (NULL for scalar)
     BOOL        bRawOutput)    // TRUE iff raw (not {thorn}) output

{
    APLDIM      aplDimRow,
                aplDimCol;
    UINT        uLen,
                uLen2,
                uCol,
                uWid;
    LPWCHAR     lpw,
                lpwszOut,
                lpwszOutStart;
    LPFMTROWSTR lpFmtRowStr;

    // Initialize local output string ptr
    lpwszOut = *lplpwszOut;

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Point to the FMTROWSTR
        lpFmtRowStr = (LPFMTROWSTR) lpaplChar;

        // It's a simple array
        Assert (lpFmtRowStr->uFmtRows EQ 1);

#ifdef DEBUG
        // Validate the FMTROWSTR
        if (lpFmtRowStr->Signature NE FMTROWSTR_SIGNATURE)
            DbgStop ();     // We should never get here
#endif
        // Skip over the FMTROWSTR as this is a simple array
        lpaplChar = (LPAPLCHAR) &lpFmtRowStr[1];

        // Save starting output string ptr
        lpwszOutStart = lpwszOut;

        // Handle non-blank rows
        if (!lpFmtRowStr->uBlank)
        {
            // Loop through the cols
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
                        uWid = lpFmtColStr[aplDimCol].uInts + lpFmtColStr[aplDimCol].uFrcs;
                        uLen = lstrlenW (lpaplChar);

                        Assert (uLen <= uWid);

                        // Check for fractional part
                        lpw = strchrW (lpaplChar, L'.');
                        if (!lpw)
                            lpw = lpaplChar + uLen;

                        // Align the decimal points
                        uCol = (lpw - lpaplChar) + lpFmtColStr[aplDimCol].uFrcs;
                        uLen2 = uWid - max (uLen, uCol);

                        // If this is raw output,
                        // break the line if it would exceed []PW
                        //   and the line is non-empty.
                        uCol = lpwszOut - lpwszTemp;
                        if (bRawOutput
                         && DEF_INDENT < uCol
                         && uQuadPW < (uWid + uCol))
                        {
                            // Ensure properly terminated
                            *lpwszOut = L'\0';

                            // Output the line
                            AppendLine (lpwszTemp, TRUE);

                            // Reset the line start
                            lpwszOut = lpw = *lplpwszOut;

                            // Fill the output area with all blanks
                            uCol = (UINT) aplLastDim - (*lplpwszOut - lpwszTemp);
                            while (uCol--)
                                *lpw++ = L' ';

                            // Skip over leading indent
                            lpwszOut += DEF_INDENT;
                            uCol = 0;
                        } else
                        {
#ifdef PREFILL
                            // Skip over leading blanks
                            lpwszOut += uLen2;
                            uCol = uLen2;
#else
                            // Fill with leading blanks
                            for (uCol = 0; uCol < uLen2; uCol++)
                                *lpwszOut++ = L' ';
#endif
                        } // End IF/ELSE

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
                Assert (uCol <= uWid);
                lpwszOut += (uWid - uCol);
#else
                // Fill with trailing blanks
                for (; uCol < uWid; uCol++)
                    *lpwszOut++ = L' ';
#endif
                lpaplChar += (uLen + 1);
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
            AppendLine (lpwszTemp, FALSE);

            // Reset the line start
            lpwszOut = lpw = *lplpwszOut;

            // Fill the output area with all blanks
            uCol = (UINT) aplLastDim - (*lplpwszOut - lpwszTemp);
            while (uCol--)
                *lpw++ = L' ';

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

    // Return the output string ptr
    *lplpwszOut = lpwszOut;

    return lpaplChar;
} // End FmtArrSimple


//***************************************************************************
//  FmtArrNested
//
//  Format a nested array from its compiled form
//***************************************************************************

LPAPLCHAR FmtArrNested
    (LPFMTHEADER lpFmtHeader,   // Ptr to FMTHEADER
     LPVOID      lpMem,         // Ptr to raw input
     LPFMTCOLSTR lpFmtColStr,   // Ptr to vector of <aplDimNCols> FMTCOLSTRs
     LPAPLCHAR   lpaplChar,     // Ptr to compiled input
     LPAPLCHAR  *lplpwszOut,    // Ptr to ptr to output string
     APLDIM      aplDimNRows,   // # rows in this array
     APLDIM      aplDimNCols,   // # cols ...
     APLDIM      aplLastDim,    // Length of the last dimension in the result
     BOOL        bRawOutput)    // TRUE iff raw (not {thorn}) output

{
    APLDIM      aplDimRow,
                aplDimCol;
    UINT        uCol;
    LPWCHAR     lpw,
                lpwszOut,
                lpwszOutStart;

    // Initialize local output string ptr
    lpwszOut = *lplpwszOut;

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
#ifdef DEBUG
        // Validate the FMTROWSTR
        {
            LPFMTROWSTR lpFmtRowStr = ((LPFMTROWSTR) lpaplChar);

            if (lpFmtRowStr->Signature NE FMTROWSTR_SIGNATURE)
                DbgStop ();     // We should never get here
        }
#endif
        if (((LPFMTROWSTR) lpaplChar)->uBlank)
            DbgBrk ();

        // Skip over the FMTROWSTR
        lpaplChar = (LPAPLCHAR) &((LPFMTROWSTR) lpaplChar)[1];

        // Save starting output string ptr
        lpwszOutStart = lpwszOut;

        // Loop through the cols
        for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++, ((LPAPLHETERO) lpMem)++)
        {
            // Save starting output string ptr
            lpwszOut = lpwszOutStart;

            // Offset lpwszOut from the start by the widthof previous cols
            for (uCol = 0; uCol < aplDimCol; uCol++)
                lpwszOut += (lpFmtColStr[uCol].uInts
                           + lpFmtColStr[uCol].uFrcs);

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
                            FmtArrSimple (lpFmtHeader,              // Ptr to FMTHEADER
                                         &lpFmtColStr[aplDimCol],  // Ptr to vector of aplDimNCols FMTCOLSTRs
                                          lpaplChar,                // Ptr to compiled input
                                         &lpwszOut,               // Ptr to output string
                                          1,                        // # rows in this array
                                          1,                        // # cols in this array
                                          aplLastDim,               // Length of last dim in result
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
                                    &lpwszOut,
                                    &lpFmtColStr[aplDimCol],
                                     aplLastDim);
                    break;

                defstop
                    break;
            } // End SWITCH
        } // End FOR

        // If there is a trailing blank, skip over it
        if (lpFmtHeader->uFmtTrBl)
#ifdef PREFILL
            lpwszOut++;
#else
            *lpwszOut++ = L' ';
#endif
////////// Skip to the next row if this is a multi-row array
////////if (aplDimNRows > 1)
////////    lpwszOut = lpwszOutStart + aplLastDim;
        // Skip to the next row
        lpwszOut = lpwszOutStart + aplLastDim;

        // If raw output, output it
        if (bRawOutput)
        {
            // Ensure properly terminated
            *lpwszOut = L'\0';

            // Output the line
            AppendLine (lpwszTemp, FALSE);

            // Reset the line start
            lpwszOut = lpw = lpwszTemp;

            // Fill the output area with all blanks
            for (uCol = 0; uCol < aplLastDim; uCol++)
                *lpw++ = L' ';

            // Note that all nested arrays are formatted as
            //   matrices, so there is no interdimensional spacing.
        } // End IF
    } // End FOR

    // Return the output string ptr
    *lplpwszOut = lpwszOut;

    return lpaplChar;
} // End FmtArrNested


//***************************************************************************
//  FmtArrNestedGlb
//
//  Format a nested array global memory object from its compiled form
//***************************************************************************

LPAPLCHAR FmtArrNestedGlb
    (HGLOBAL     hGlb,          // The global memory handle whose contentsd are to be formatted
     LPAPLCHAR   lpaplChar,     // Ptr to compiled input
     LPAPLCHAR  *lplpwszOut,    // Ptr to ptr to output string
     LPFMTCOLSTR lpFmtColStr,   // Ptr to the FMTCOLSTR for this column
     APLDIM      aplLastDim)    // Length of the last dimension in the result

{
    APLSTYPE aplType;
    APLNELM  aplNELM;
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

    // Get attrs (Type, NELM, Rank) of the global
    AttrsOfGlb (hGlb, &aplType, &aplNELM, &aplRank, NULL);

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

#ifdef DEBUG
    // Validate the FMTHEADER
    if (lpFmtHeader->Signature NE FMTHEADER_SIGNATURE)
        DbgStop ();     // We should never get here
#endif
    // Skip over the FMTHEADER
    lpaplChar = (LPAPLCHAR) &lpFmtHeader[1];

    // Point to the FMTCOLSTRs
    lpFmtColLcl = (LPFMTCOLSTR) lpaplChar;

#ifdef DEBUG
    // Validate the FMTCOLSTRs
    {
        APLDIM uCol;

        for (uCol = 0; uCol < aplDimNCols; uCol++)
        if (lpFmtColLcl[uCol].Signature NE FMTCOLSTR_SIGNATURE)
            DbgStop ();     // We should never get here
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
            FmtArrSimple (lpFmtHeader,          // Ptr to FMTHEADER
                          lpFmtColLcl,          // Ptr to vector of <aplDimNCols> FMTCOLSTRs
                          lpaplChar,            // Ptr to compiled input
                          lplpwszOut,           // Ptr to output string
                          aplDimNRows,          // # rows in this array
                          aplDimNCols,          // # cols in this array
                          aplLastDim,           // Length of last dim in result
                          aplRank,              // Rank of this array
                          lpMemDim,             // Ptr to this array's dimensions
                          FALSE);               // TRUE iff raw output
            break;

        case ARRAY_NESTED:
            lpaplChar =
            FmtArrNested (lpFmtHeader,          // Ptr to FMTHEADER
                          lpMem,                // Ptr to raw input
                          lpFmtColLcl,          // Ptr to vector of <aplDimNCols> FMTCOLSTRs
                          lpaplChar,            // Ptr to compiled input
                          lplpwszOut,           // Ptr to ptr to output string
                          aplDimNRows,          // # rows in this array
                          aplDimNCols,          // # cols ...
                          aplLastDim,           // Length of last dim in result
                          FALSE);               // TRUE iff raw (not {thorn}) output
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

    // Loop through the cols setting the common width
    for (aplDimCol = 1; aplDimCol < aplDimNCols; aplDimCol++)
    {
        // Max the current width with the width of this number
        lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, 2);

////////// Max the current format type with FMT_INT
////////lpFmtColStr[aplDimCol].uFmtType = max (lpFmtColStr[aplDimCol].uFmtType, FMTTYPE_INT);
    } // End FOR

    // Add in a leading blank unless scalar/vector
    uLen = 1 + (aplRank > 1);
    lpFmtColStr[0].uInts = max (lpFmtColStr[0].uInts, uLen);

    // Initialize bit mask for all references to lpMem
    uBitMask = 0x01;

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
#ifdef DEBUG
        lpFmtRowLcl->Signature   = FMTROWSTR_SIGNATURE;
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

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
#ifdef DEBUG
        lpFmtRowLcl->Signature   = FMTROWSTR_SIGNATURE;
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

            // Calculate the string length
            uLen = (lpaplChar - lpwszOut) - 1;

            // Add in a leading blank unless scalar/vector and 1st col
            uLen += (aplRank > 1) || aplDimCol NE 0;

            // Max the current width with this
            lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

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

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
#ifdef DEBUG
        lpFmtRowLcl->Signature   = FMTROWSTR_SIGNATURE;
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
                lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

                // Calculate the length of the fractional part
                //   including the decimal point
                uLen = (lpaplChar - lpwsz) - 1;

                // Max the current fractional part with this
                lpFmtColStr[aplDimCol].uFrcs = max (lpFmtColStr[aplDimCol].uFrcs, uLen);

                // Max the current format type with FMTTYPE_FIX
                lpFmtColStr[aplDimCol].uFmtType = max (lpFmtColStr[aplDimCol].uFmtType, FMTTYPE_FIX);
            } else
            {
                // Calculate the string length
                uLen = (lpaplChar - lpwszOut) - 1;

                // Add in a leading blank unless scalar/vector and 1st col
                uLen += (aplRank > 1) || aplDimCol NE 0;

                // Max the current width with this
                lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);
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
//// LPFMTROWSTR lpFmtRowStr,   // Ptr to this row's FMTROWSTR
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

    // Loop through the cols setting the common width
    for (aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
    {
        // Max the current width with the width of this char
        lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, 1);
    } // End FOR

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
#ifdef DEBUG
        lpFmtRowLcl->Signature   = FMTROWSTR_SIGNATURE;
#endif
        lpFmtRowLcl->uBlank      = FALSE;
        lpFmtRowLcl->uFmtRows    = 1;           // Initialize the count
////////lpFmtRowLcl->lpFmtRowNxt = NULL;        // Initialize the ptr (filled in below)
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Loop through the cols
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

    // Get the APA parameters
    apaOff = lpMem->Off;
    apaMul = lpMem->Mul;

    // Loop through the rows
    for (aplDimRow = apaAcc = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
#ifdef DEBUG
        lpFmtRowLcl->Signature   = FMTROWSTR_SIGNATURE;
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

            // Calculate the string length
            uLen = (lpaplChar - lpwszOut) - 1;

            // Add in a leading blank unless scalar/vector and 1st col
            uLen += (aplRank > 1) || aplDimCol NE 0;

            // Max the current width with this
            lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);
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

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
#ifdef DEBUG
        lpFmtRowLcl->Signature   = FMTROWSTR_SIGNATURE;
#endif
        lpFmtRowLcl->uBlank      = FALSE;
        lpFmtRowLcl->uFmtRows    = 1;           // Initialize the count
////////lpFmtRowLcl->lpFmtRowNxt = NULL;        // Initialize the ptr (filled in below)
        lpaplChar = (LPAPLCHAR) &lpFmtRowLcl[1];

        // Loop through the cols
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
            lpFmtColStr[aplDimCol].uInts = max (lpFmtColStr[aplDimCol].uInts, uLen);

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

    // Loop through the rows
    for (aplDimRow = 0; aplDimRow < aplDimNRows; aplDimRow++)
    {
        // Create a new FMTROWSTR
        lpFmtRowLcl = (LPFMTROWSTR) lpaplChar;
#ifdef DEBUG
        lpFmtRowLcl->Signature   = FMTROWSTR_SIGNATURE;
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
                            CompileArrBool   (&(*(LPSYMENTRY *) lpMem)->stData.stBoolean,
                                           lpFmtHeader,
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

    // Create a new FMTHEAD in the output
    ZeroMemory ((LPFMTHEADER) lpaplChar, sizeof (FMTHEADER));
    ((LPFMTHEADER) lpaplChar)->lpFmtHeadUp = lpFmtHeader;
    lpFmtHeader = (LPFMTHEADER) lpaplChar;
#ifdef DEBUG
    lpFmtHeader->Signature   = FMTHEADER_SIGNATURE;
#endif
    lpFmtHeader->lpFmtRowUp  = lpFmtRowStr;
    lpFmtHeader->lpFmtColUp  = lpFmtColStr;
////lpFmtHeader->lpFmtRow1st =                      // Filled in below
////lpFmtHeader->lpFmtCol1st =                      // ...
    lpFmtHeader->uRows       = (UINT) aplDimNRows;
    lpFmtHeader->uCols       = (UINT) aplDimNCols;
////lpFmtHeader->uFmtRows    = 0;                   // Filled in by ZeroMemory
////lpFmtHeader->uFmtInts    = 0;                   // ...
////lpFmtHeader->uFmtFrcs    = 0;                   // ...
////lpFmtHeader->uFmtTrBl    = 0;                   // ...
    lpFmtHeader->uDepth      = 1 + lpFmtHeader->lpFmtHeadUp->uDepth;
    lpFmtHeader->uMatRes     = (aplRank > 1);

    // Create <aplDimNCols> FMTCOLSTRs in the output
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
    lpaplChar = (LPAPLCHAR) (&lpFmtColStr[aplDimNCols]);

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
    PropogateRowColCount (lpFmtHeader,
                          TRUE);
    return lpaplChar;
} // End CompileArrNestedGlb


//***************************************************************************
//  PropogateRowColCount
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
    if (lpFmtHeader->lpFmtColUp)
        lpFmtHeader->lpFmtColUp->uFmtRows = max (lpFmtHeader->lpFmtColUp->uFmtRows, uAcc);
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
    lpFmtHeader->uFmtTrBl = max (lpFmtHeader->uFmtTrBl, uTrBl);

    // Propogate the uMatRes bit
    if (lpFmtHeader->lpFmtHeadUp)
        lpFmtHeader->lpFmtHeadUp->uMatRes |= lpFmtHeader->uMatRes;

    // Recurse
    if (lpFmtHeader->lpFmtHeadUp)
        PropogateRowColCount (lpFmtHeader->lpFmtHeadUp,
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
#ifdef DEBUG
        lpFmtRowLcl->Signature   = FMTROWSTR_SIGNATURE;
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
