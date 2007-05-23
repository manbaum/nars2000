//***************************************************************************
//  NARS2000 -- Primitive Functions Header
//***************************************************************************

// Common TYPEDEF used to call all primitive functions
typedef struct tagYYSTYPE *(*LPPRIMFNS) (struct tagTOKEN *,
                                         struct tagTOKEN *,
                                         struct tagTOKEN *,
                                         struct tagTOKEN *);

// Common TYPEDEF used to call all primitive operators
typedef struct tagYYSTYPE *(*LPPRIMOPS) (struct tagTOKEN *,
                                         struct tagYYSTYPE *,
                                         struct tagTOKEN *,
                                         struct tagTOKEN *);


//***************************************************************************
//  End of File: primfns.h
//***************************************************************************
