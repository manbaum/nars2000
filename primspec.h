//***************************************************************************
//  NARS2000 -- Primitive Function Special Header
//***************************************************************************


// Call this function for each monadic primitive function
typedef LPYYSTYPE PRIMFN_MON
    (LPTOKEN lptkFunc,                      // Ptr to function token
     LPTOKEN lptkRhtArg,                    // Ptr to right arg token
     LPTOKEN lptkAxis,                      // Ptr to axis token (may be NULL)
     struct tagPrimSpec *lpPrimSpec,        // Ptr to local PRIMSPEC
     struct tagPLLOCALVARS *lpplLocalVars); // Ptr to local plLocalVars

typedef PRIMFN_MON *LPPRIMFN_MON;

// Call this function for each dyadic primitive function
typedef LPYYSTYPE PRIMFN_DYD
    (LPTOKEN lptkLftArg,                    // Ptr to left arg token
     LPTOKEN lptkFunc,                      // Ptr to function token
     LPTOKEN lptkRhtArg,                    // Ptr to right arg token
     LPTOKEN lptkAxis,                      // Ptr to axis token (may be NULL)
     struct tagPrimSpec *lpPrimSpec,        // Ptr to local PRIMSPEC
     struct tagPLLOCALVARS *lpplLocalVars); // Ptr to local plLocalVars

typedef PRIMFN_DYD *LPPRIMFN_DYD;

typedef BOOL PRIMFN_DYD_SNvSN
    (LPYYSTYPE lpYYRes,

     LPTOKEN   lptkLftArg,
     LPTOKEN   lptkFunc,
     LPTOKEN   lptkRhtArg,

     HGLOBAL   hGlbLft,
     HGLOBAL   hGlbRht,
     HGLOBAL  *lphGlbRes,

     LPVOID    lpMemLft,            // Points to sign.ature
     LPVOID    lpMemRht,            // ...
     LPVOID    lpMemRes,            // ...

     LPAPLUINT lpMemAxisHead,
     LPAPLUINT lpMemAxisTail,

     APLRANK   aplRankLft,
     APLRANK   aplRankRht,
     APLRANK   aplRankRes,

     APLSTYPE  aplTypeLft,
     APLSTYPE  aplTypeRht,
     APLSTYPE  aplTypeRes,

     APLNELM   aplNELMLft,
     APLNELM   aplNELMRht,
     APLNELM   aplNELMRes,
     APLNELM   aplNELMAxis,
     struct tagPrimSpec *lpPrimSpec,        // Ptr to local PRIMSPEC
     struct tagPLLOCALVARS *lpplLocalVars); // Ptr to local plLocalVars

typedef PRIMFN_DYD_SNvSN *LPPRIMFN_DYD_SNvSN;

// Call this function to determine the storage type of the monadic result
typedef APLSTYPE STORAGE_TYPE_MON
        (APLNELM    aplNELMRht,
         LPAPLSTYPE lpaplTypeRht,
         LPTOKEN    lptkFunc);

typedef STORAGE_TYPE_MON *LPSTORAGE_TYPE_MON;

// Call this function to determine the storage type of the dyadic result
typedef APLSTYPE STORAGE_TYPE_DYD
        (APLNELM    aplNELMLft,
         LPAPLSTYPE lpaplTypeLft,
         LPTOKEN    lptkFunc,
         APLNELM    aplNELMRht,
         LPAPLSTYPE lpaplTypeRht);

typedef STORAGE_TYPE_DYD *LPSTORAGE_TYPE_DYD;

// Call this function if the monadic result is APA
typedef BOOL APARESULT_MON
        (LPYYSTYPE lpYYRes,                     // Ptr to result token (may be NULL)

         LPTOKEN   lptkFunc,                    // Ptr to function token

         HGLOBAL   hGlbRht,                     // HGLOBAL of right arg
         HGLOBAL  *lphGlbRes,                   // ...        result

         LPVOID   *lplpMemRes,                  // Ptr to ptr to result memory (initially at sign.ature)

         APLRANK   aplRankRht,                  // Right arg rank

         struct tagPrimSpec *lpPrimSpec,        // Ptr to local PRIMSPEC
         struct tagPLLOCALVARS *lpplLocalVars); // Ptr to local plLocalVars

typedef APARESULT_MON *LPAPARESULT_MON;

// Call this function if the dyadic result is APA
typedef BOOL APARESULT_DYD
        (LPYYSTYPE lpYYRes,                     // Ptr to result token (may be NULL)

         LPTOKEN   lptkFunc,                    // Ptr to function token

         HGLOBAL   hGlbLft,                     // HGLOBAL of left arg (may be NULL if simple)
         HGLOBAL   hGlbRht,                     // ...        right ...
         HGLOBAL  *lphGlbRes,                   // ...        result

         LPVOID   *lplpMemRes,                  // Ptr to ptr to result memory (initially at sign.ature)

         APLRANK   aplRankLft,                  // Left arg rank
         APLRANK   aplRankRht,                  // Right ...

         APLNELM   aplNELMLft,                  // Left arg NELM
         APLNELM   aplNELMRht,                  // Right ...

         APLINT    aplInteger,                  // The integer from the simple side
         struct tagPrimSpec *lpPrimSpec,        // Ptr to local PRIMSPEC
         struct tagPLLOCALVARS *lpplLocalVars); // Ptr to local plLocalVars

typedef APARESULT_DYD *LPAPARESULT_DYD;


typedef APLBOOL  BISB   (APLBOOL , struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISI   (APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISF   (APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISC   (APLCHAR , struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISI   (APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISF   (APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISC   (APLCHAR , struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISI   (APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISF   (APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISC   (APLCHAR , struct tagPrimSpec *lpPrimSpec);


typedef APLBOOL  BISBVB (APLBOOL , APLBOOL , struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISBVI (APLBOOL , APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISBVF (APLBOOL , APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISBVC (APLBOOL , APLCHAR , struct tagPrimSpec *lpPrimSpec);

typedef APLBOOL  BISIVB (APLINT  , APLBOOL , struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISIVI (APLINT  , APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISIVF (APLINT  , APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISIVC (APLINT  , APLCHAR , struct tagPrimSpec *lpPrimSpec);

typedef APLBOOL  BISFVB (APLFLOAT, APLBOOL , struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISFVI (APLFLOAT, APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISFVF (APLFLOAT, APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISFVC (APLFLOAT, APLCHAR , struct tagPrimSpec *lpPrimSpec);

typedef APLBOOL  BISCVB (APLCHAR , APLBOOL , struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISCVI (APLCHAR , APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISCVF (APLCHAR , APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISCVC (APLCHAR , APLCHAR , struct tagPrimSpec *lpPrimSpec);

typedef APLINT   IISBVB (APLBOOL , APLBOOL , struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISBVI (APLBOOL , APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISBVF (APLBOOL , APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISBVC (APLBOOL , APLCHAR , struct tagPrimSpec *lpPrimSpec);

typedef APLINT   IISIVB (APLINT  , APLBOOL , struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISIVI (APLINT  , APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISIVF (APLINT  , APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISIVC (APLINT  , APLCHAR , struct tagPrimSpec *lpPrimSpec);

typedef APLINT   IISFVB (APLFLOAT, APLBOOL , struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISFVI (APLFLOAT, APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISFVF (APLFLOAT, APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISFVC (APLFLOAT, APLCHAR , struct tagPrimSpec *lpPrimSpec);

typedef APLINT   IISCVB (APLCHAR , APLBOOL , struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISCVI (APLCHAR , APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISCVF (APLCHAR , APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISCVC (APLCHAR , APLCHAR , struct tagPrimSpec *lpPrimSpec);

typedef APLFLOAT FISBVB (APLBOOL , APLBOOL , struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISBVI (APLBOOL , APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISBVF (APLBOOL , APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISBVC (APLBOOL , APLCHAR , struct tagPrimSpec *lpPrimSpec);

typedef APLFLOAT FISIVB (APLINT  , APLBOOL , struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISIVI (APLINT  , APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISIVF (APLINT  , APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISIVC (APLINT  , APLCHAR , struct tagPrimSpec *lpPrimSpec);

typedef APLFLOAT FISFVB (APLFLOAT, APLBOOL , struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISFVI (APLFLOAT, APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISFVF (APLFLOAT, APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISFVC (APLFLOAT, APLCHAR , struct tagPrimSpec *lpPrimSpec);

typedef APLFLOAT FISCVB (APLCHAR , APLBOOL , struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISCVI (APLCHAR , APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISCVF (APLCHAR , APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISCVC (APLCHAR , APLCHAR , struct tagPrimSpec *lpPrimSpec);


// General structure passed to all primitive functions
typedef struct tagPrimSpec
{
    // Monadic functions
    LPPRIMFN_MON        PrimFnMon_EM;
    LPSTORAGE_TYPE_MON  StorageTypeMon;
    LPAPARESULT_MON     ApaResultMon_EM;

    BISB               *BisB;
    BISI               *BisI;
    BISF               *BisF;

////IISB               *IisB;   // Handled via type promotion (to IisI)
    IISI               *IisI;
    IISF               *IisF;

////FISB               *FisB;   // Handled via type promotion (to FisF)
    FISI               *FisI;
    FISF               *FisF;

    // Dyadic functions
    LPPRIMFN_DYD        PrimFnDyd_EM;
    LPSTORAGE_TYPE_DYD  StorageTypeDyd;
    LPAPARESULT_DYD     ApaResultDyd_EM;

    BISBVB             *BisBvB;
    BISIVI             *BisIvI;
    BISFVF             *BisFvF;
    BISCVC             *BisCvC;

////IISBVB             *IisBvB;     // Handled via type promotion (to IisIvI)
    IISIVI             *IisIvI;
    IISFVF             *IisFvF;

////FISBVB             *FisBvB;     // Handled via type promotion (to FisFvF)
    FISIVI             *FisIvI;
    FISFVF             *FisFvF;

    // Miscellaneous
    EXEC_CODES         *lpExecCode;
} PRIMSPEC, * LPPRIMSPEC;


//***************************************************************************
//  End of File: primspec.h
//***************************************************************************
