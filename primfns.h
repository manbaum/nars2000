//***************************************************************************
//  NARS2000 -- Primitive Functions Header
//***************************************************************************

// Common TYPEDEF used to call all primitive functions
typedef struct tagPL_YYSTYPE *(*LPPRIMFNS) (struct tagTOKEN *,
                                            struct tagTOKEN *,
                                            struct tagTOKEN *,
                                            struct tagTOKEN *);

// Common TYPEDEF used to call all primitive operators
typedef struct tagPL_YYSTYPE *(*LPPRIMOPS) (struct tagTOKEN *,
                                            struct tagPL_YYSTYPE *,
                                            struct tagTOKEN *,
                                            struct tagTOKEN *);


//***************************************************************************
//  End of File: primfns.h
//***************************************************************************
