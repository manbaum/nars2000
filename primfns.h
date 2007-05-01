//***************************************************************************
//  NARS2000 -- Primitive Functions Header
//***************************************************************************

// Common TYPEDEF used to call all primitive functions
////typedef LPYYSTYPE (*LPPRIMFNS) (LPTOKEN, LPTOKEN, LPTOKEN, LPTOKEN, LPPLLOCALVARS);
typedef struct tagYYSTYPE *(*LPPRIMFNS) (struct tagTOKEN *,
                                         struct tagTOKEN *,
                                         struct tagTOKEN *,
                                         struct tagTOKEN *,
                                         struct tagPLLOCALVARS *);


//***************************************************************************
//  End of File: primfns.h
//***************************************************************************
