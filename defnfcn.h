//***************************************************************************
//  NARS2000 -- Defined Function Header
//***************************************************************************

// Function line structure, one per function line
typedef struct tagFCNLINE
{
    HGLOBAL         hGlbLineText;   // Text of the line (APLCHAR) global memory handle
    HGLOBAL         hGlbTknLine;    // Tokenized line (TOKEN)     ...
    HGLOBAL         hGlbMonInfo;    // Monitor information (??)   ... (may be NULL)
} FCNLINE, *LPFCNLINE;


// Defined function header signature
#define DEFN_HEADER_SIGNATURE   'NFED'

// Function header structure
typedef struct tagDEFN_HEADER
{
    HEADER_SIGNATURE Sign;          // 00:  Defined function header signature
    UINT             Version,       // 04:  Version # of this header
                     uFcnLines;     // 08:  Offset to start of function lines (FCNLINE[nLines])
    UINT             Type:2,        // 0C:  Type:  0 = Unknown
                                    //             1 = Monadic operator
                                    //             2 = Dyadic operator
                                    //             3 = Function
                     Valence:2;     //      Valence:  0 = Niladic function
                                    //                1 = Monadic function/derived function
                                    //                2 = Dyadic  ...
                                    //                3 = Ambivalent ...
    UINT             RefCnt,        // 10:  Reference count
                     nLines,        // 14:  # lines in the function (not counting the header)
                     nProtoLine,    // 18:  Line # of the []PROTOTYPE label
                     nInverseLine,  // 1C:  Line # of the []INVERSE label
                     nSingletonLine;// 20:  Line # of the []SINGLETON label
    HGLOBAL          hGlbTxtHdr,    // 24:  Text of function header (APLCHAR) ...
                     hGlbTknHdr,    // 28:  Tokenized function header (TOKEN) ...
                     hGlbUndoBuff;  // 2C:  Undo buffer                       ... (may be NULL)
                                    // 30:  Array of function line structures (FCNLINE[nLines])
} DEFN_HEADER, *LPDEFN_HEADER;




//***************************************************************************
//  End of File: defnfcn.h
//***************************************************************************
