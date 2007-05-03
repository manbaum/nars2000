//***************************************************************************
//  NARS2000 -- Primitive Operator -- Each
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  PrimOpEach_EM
//
//  Primitive operator for monadic and dyadic Each ("each" and "each")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpEach_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpEach_EM
    (LPTOKEN       lptkLftArg,      // Ptr to left arg token (may be NULL if monadic)
     LPYYSTYPE     lpYYFcnStr,      // Ptr to function strand
     LPTOKEN       lptkRhtArg,      // Ptr to right arg token
     LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return PrimOpMonEach_EM             (lpYYFcnStr, lptkRhtArg, lpplLocalVars);
    else
        return PrimOpDydEach_EM (lptkLftArg, lpYYFcnStr, lptkRhtArg, lpplLocalVars);
} // End PrimOpEach_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimOpMonEach_EM
//
//  Primitive operator for monadic derived function from Each ("each")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpMonEach_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpMonEach_EM
    (LPYYSTYPE     lpYYFcnStr,      // Ptr to function strand
     LPTOKEN       lptkRhtArg,      // Ptr to right arg token
     LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
    APLUINT   ByteRes;
    APLSTYPE  aplTypeRht,
              aplTypeRes;
    APLNELM   aplNELMRht;
    APLRANK   aplRankRht;
    HGLOBAL   hGlbRht,
              hGlbRes;
    LPVOID    lpMemRht,
              lpMemRes;
    APLUINT   uRht;
    LPYYSTYPE lpYYRes,
              lpYYRes2;
    APLINT    apaOff,
              apaMul;
    BOOL      bRet = TRUE;

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Get right arg's global ptrs
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Handle immediate right arg (lpMemRht is NULL)
    if (lpMemRht EQ NULL)
    {
        lpYYRes2 = ExecFuncStr_EM (NULL,
                                   lpYYFcnStr,
                                   lptkRhtArg,
                                   lpplLocalVars);
        if (lpYYRes2)
        {
            // Enclose the item
            lpYYRes = PrimFnMonLeftShoe_EM (&lpYYFcnStr->tkToken, &lpYYRes2->tkToken, NULL, lpplLocalVars);
            FreeResult (&lpYYRes2->tkToken); YYFree (lpYYRes2); lpYYRes2 = NULL;
        } else
            lpYYRes = lpYYRes2;

        return lpYYRes;
    } // End IF

    // The result storage type is assumed to be NESTED,
    //   but we'll call TypeDemote at the end just in case.
    aplTypeRes = ARRAY_NESTED;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRht, aplRankRht);

    // Allocate space for the result
    // N.B. Conversion from APLUINT to UINT.
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                  &lpYYFcnStr->tkToken,
                                   lpplLocalVars);
        goto ERROR_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)

    // Fill in the header values
    lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->Perm       = 0;       // Already zero from GHND
////lpHeader->SysVar     = 0;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRht;
    lpHeader->Rank       = aplRankRht;

#undef  lpHeader

    // Copy the dimensions from the right arg to the result
    if (lpMemRht)
        CopyMemory (VarArrayBaseToDim (lpMemRes),
                    VarArrayBaseToDim (lpMemRht),
                    (UINT) aplRankRht * sizeof (APLDIM));

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRht);
    if (lpMemRht)
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    // Handle prototype
    if (aplNELMRht EQ 0)
    {
        HGLOBAL hGlbProto;
        UINT    immType;

        // If this function is a primitive scalar monadic function,
        //   the result is the prototype of the right arg converted
        //   to numeric

        immType = lpYYFcnStr->tkToken.tkFlags.ImmType;

        if (lpYYFcnStr->TknCount EQ 1
         && lpYYFcnStr->FcnCount EQ 1
         && lpYYFcnStr->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
         && (immType EQ IMMTYPE_PRIMFCN_SM
          || immType EQ IMMTYPE_PRIMFCN_SS))
        {
            // In case we fail
            *((LPAPLNESTED) lpMemRes) = PTR_REUSED;

            // If the right arg is simple, ...
            if (IsSimple (aplTypeRes))
            {
                LPPERTABDATA lpPTD;

                // Get the per tab handle for the active thread
                lpPTD = MyGlobalLock (lpplLocalVars->hGlbPTD);

                // Save the handle
                *((LPAPLNESTED) lpMemRes) = lpPTD->steZero;

                // We no longer need this ptr
                MyGlobalUnlock (lpplLocalVars->hGlbPTD); lpPTD = NULL;
            } else
            {
                // Make the prototype
                hGlbProto = MakePrototype_EM (hGlbRht,
                                             &lpYYFcnStr->tkToken,
                                              TRUE,         // Disallow non-empty CHARs
                                              lpplLocalVars);
                if (!hGlbProto)
                    goto ERROR_EXIT;

                // Save the handle
                *((LPAPLNESTED) lpMemRes) = MakeGlbTypeGlb (hGlbProto);
            } // End IF/ELSE
        } else
        {
            // The function is not a primitive scalar monadic function
            DbgBrk ();          // ***FINISHME***





        } // End IF/ELSE
    } else
    {
        TOKEN tkArg = {0};
        UINT  uBitMask;

        // Fill in the result with PTR_REUSED in case
        //   we fail at some intermediate point
        for (uRht = 0; uRht < aplNELMRht; uRht++)
            ((LPAPLNESTED) lpMemRes)[uRht] = PTR_REUSED;

        // Fill in arg token
        tkArg.tkFlags.TknType   = TKT_VARIMMED;
////////tkArg.tkFlags.ImmType   =       // To be filled in below
////////tkArg.tkFlags.NoDisplay = 0;    // Already zero from = {0}
////////tkArg.tkData.tkLongest  =       // To be filled in below
        tkArg.tkCharIndex       = lpYYFcnStr->tkToken.tkCharIndex;

        // Translate ARRAY_APA args to ARRAY_INT
        if (aplTypeRht EQ ARRAY_APA)
            tkArg.tkFlags.ImmType = ARRAY_INT;
        else
            tkArg.tkFlags.ImmType = aplTypeRht;

        // Split cases based upon the storage type of the right arg
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
                // Initialize the bit mask
                uBitMask = 0x01;

                // Loop through the right arg
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                {
                    // Copy the value to the arg token
                    tkArg.tkData.tkBoolean = (uBitMask & *(LPAPLBOOL) lpMemRht) ? 1 : 0;

                    // Shift over the bit mask
                    uBitMask <<= 1;

                    // Check for end-of-byte
                    if (uBitMask EQ END_OF_BYTE)
                    {
                        uBitMask = 0x01;            // Start over
                        ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                    } // End IF

                    // Execute the function on the arg token
                    if (!ExecMonFuncOnToken (&lpMemRes, lpYYFcnStr, &tkArg, lpplLocalVars))
                        goto ERROR_EXIT;
                } // End FOR

                break;

            case ARRAY_INT:
                // Loop through the right arg
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                {
                    // Copy the value to the arg token
                    tkArg.tkData.tkInteger = *((LPAPLINT) lpMemRht)++;

                    // Execute the function on the arg token
                    if (!ExecMonFuncOnToken (&lpMemRes, lpYYFcnStr, &tkArg, lpplLocalVars))
                        goto ERROR_EXIT;
                } // End FOR

                break;

            case ARRAY_FLOAT:
                // Loop through the right arg
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                {
                    // Copy the value to the arg token
                    tkArg.tkData.tkFloat = *((LPAPLFLOAT) lpMemRht)++;

                    // Execute the function on the arg token
                    if (!ExecMonFuncOnToken (&lpMemRes, lpYYFcnStr, &tkArg, lpplLocalVars))
                        goto ERROR_EXIT;
                } // End FOR

                break;

            case ARRAY_CHAR:
                // Loop through the right arg
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                {
                    // Copy the value to the arg token
                    tkArg.tkData.tkChar = *((LPAPLCHAR) lpMemRht)++;

                    // Execute the function on the arg token
                    if (!ExecMonFuncOnToken (&lpMemRes, lpYYFcnStr, &tkArg, lpplLocalVars))
                        goto ERROR_EXIT;
                } // End FOR

                break;

            case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemRht)

                apaOff = lpAPA->Off;
                apaMul = lpAPA->Mul;

#undef  lpAPA

                // Loop through the right arg
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                {
                    // Copy the value to the arg token
                    tkArg.tkData.tkInteger = apaOff + apaMul * uRht;

                    // Execute the function on the arg token
                    if (!ExecMonFuncOnToken (&lpMemRes, lpYYFcnStr, &tkArg, lpplLocalVars))
                        goto ERROR_EXIT;
                } // End FOR

                break;

            case ARRAY_HETERO:
            case ARRAY_NESTED:
                // Loop through the right arg
                for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLHETERO) lpMemRht)++)
                {
                    // Split cases based upon the ptr type
                    switch (GetPtrTypeInd (lpMemRht))
                    {
                        case PTRTYPE_STCONST:
                            // Set the token type
                            tkArg.tkFlags.TknType = TKT_VARIMMED;

                            // Get the immediate type from the STE
                            tkArg.tkFlags.ImmType = (*(LPSYMENTRY *) lpMemRht)->stFlags.ImmType;

                            // Copy the value to the arg token
                            tkArg.tkData.tkLongest = (*(LPSYMENTRY *) lpMemRht)->stData.stLongest;

                            break;

                        case PTRTYPE_HGLOBAL:
                            // Set the token & immediate type
                            tkArg.tkFlags.TknType = TKT_VARARRAY;
                            tkArg.tkFlags.ImmType = 0;

                            // Copy the value to the arg token
                            tkArg.tkData.tkGlbData = CopySymGlbInd ((LPAPLNESTED) lpMemRht);

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    // Execute the function on the arg token
                    bRet = ExecMonFuncOnToken (&lpMemRes, lpYYFcnStr, &tkArg, lpplLocalVars);

                    // Free the arg token
                    FreeResult (&tkArg);

                    if (!bRet)
                        goto ERROR_EXIT;
                } // End FOR

                break;

            defstop
                break;
        } // End SWITCH
    } // End IF/ELSE

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (TypeDemote (hGlbRes));
    lpYYRes->tkToken.tkCharIndex       = lpYYFcnStr->tkToken.tkCharIndex;

    goto NORMAL_EXIT;

ERROR_EXIT:
    bRet = FALSE;
NORMAL_EXIT:
    if (lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    if (lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    if (bRet)
        return lpYYRes;
    else
        return NULL;
} // End PrimOpMonEach_EM
#undef  APPEND_NAME


//***************************************************************************
//  ExecMonFuncOnToken
//
//  Execute a monadic function strand on a token
//***************************************************************************

BOOL ExecMonFuncOnToken
    (LPVOID       *lplpMemRes,      // Ptr to ptr to result memory
     LPYYSTYPE     lpYYFcnStr,      // Ptr to function strand
     LPTOKEN       lptkArg,         // Ptr to right arg token
     LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
    LPYYSTYPE lpYYRes;

    // Execute the function on the arg token
    lpYYRes = ExecFuncStr_EM (NULL,
                              lpYYFcnStr,
                              lptkArg,
                              lpplLocalVars);
    // If it succeeded, ...
    if (lpYYRes)
    {
        // Split cases based upon the token type of the result
        switch (lpYYRes->tkToken.tkFlags.TknType)
        {
            case TKT_VARIMMED:
                // Pass through the LPSYMENTRY
                *((LPAPLNESTED) *lplpMemRes)++ = lpYYRes->tkToken.tkData.tkSym;

                break;

            case TKT_VARARRAY:
                // Copy (increment the reference count of) the global object
                *((LPAPLNESTED) *lplpMemRes)++ = CopySymGlbDir (lpYYRes->tkToken.tkData.tkGlbData);

                break;

            defstop
                break;
        } // End SWITCH

        // Free the result of the function execution
        FreeResult (&lpYYRes->tkToken); YYFree (lpYYRes); lpYYRes = NULL;

        return TRUE;
    } // End IF

    return FALSE;
} // End ExecMonFuncOnToken


//***************************************************************************
//  PrimOpDydEach_EM
//
//  Primitive operator for dyadic derived function from Each ("each")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydEach_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpDydEach_EM
    (LPTOKEN       lptkLftArg,      // Ptr to left arg token (may be NULL if monadic)
     LPYYSTYPE     lpYYFcnStr,      // Ptr to function strand
     LPTOKEN       lptkRhtArg,      // Ptr to right arg token
     LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
    LPYYSTYPE lpYYRes;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();


    DbgBrk ();          // ***FINISHME***







    return lpYYRes;
} // End PrimOpDydEach_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: po_each.c
//***************************************************************************
