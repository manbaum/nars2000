//***************************************************************************
//  NARS2000 -- Display Header
//***************************************************************************


////typedef enum tagFMT_TYPES
////{
////    FMTTYPE_INT = 0,                    // Integer format, e.g. -1234
////    FMTTYPE_FIX,                        // Fixed point ...      -1234.56
////    FMTTYPE_EXP,                        // Exponential ...      -1234.0E-19
////} FMT_TYPES;
////
typedef struct tagFMTHEADER
{
#ifdef DEBUG
#define FMTHEADER_SIGNATURE 'HHHH'      // 48484848
    UINT        Signature;              // FMTHEADER signature
#endif
    struct tagFMTHEADER *lpFmtHeadUp;   // Ptr to next chain up
    struct tagFMTROWSTR *lpFmtRowUp;    // Ptr to parent FMTROWSTR
    struct tagFMTCOLSTR *lpFmtColUp;    // Ptr to parent FMTCOLSTR
    struct tagFMTROWSTR *lpFmtRow1st;   // Ptr to 1st child FMTROWSTR
    struct tagFMTCOLSTR *lpFmtCol1st;   // Ptr to 1st child FMTCOLSTR
    UINT        uRows,                  // # (FMTROWSTRs, DATAs) to follow
                uCols,                  // # FMTCOLSTRs to follow
                uFmtRows,               // # formatted rows in this block (+/FMTROWSTR.uFmtRows)
                uFmtInts,               // ...         ints ...           (+/FMTCOLSTR.uInts)
                uFmtFrcs;               // ...         frcs ...           (+/FMTCOLSTR.uFrcs)
    UINT        uDepth:29,              // Depth of this struct (0 = top)
                uFmtTrBl:1,             // TRUE iff trailing blank surrounding this item
                uMatRes:1;              // TRUE iff there is a rank > 1 item contained in this item
} FMTHEADER, *LPFMTHEADER;

typedef struct tagFMTCOLSTR
{
#ifdef DEBUG
#define FMTCOLSTR_SIGNATURE 'CCCC'      // 43434343
    UINT        Signature;              // FMTCOLSTR signature
#endif
////UINT        uFmtType;               // Format type (see FMT_TYPES)
    UINT        uInts,                  // # Integer digits in Boolean/Integer/APA/Float column,
                                        //   or # chars in Char column
                                        //   including sign and leading spaces
                uFrcs;                  // # Fractional digits in Float column
                                        //   including decimal point
} FMTCOLSTR, *LPFMTCOLSTR;

typedef struct tagFMTROWSTR
{
#ifdef DEBUG
#define FMTROWSTR_SIGNATURE 'RRRR'      // 52525252
    UINT        Signature;              // FMTROWSTR signature
#endif
    UINT        uFmtRows:31,            // # formatted rows in this row
                uBlank:1;               // TRUE iff this row is blank
    struct tagFMTROWSTR *lpFmtRowNxt;   // Ptr to next sibling FMTROWSTR
} FMTROWSTR, *LPFMTROWSTR;

// ***FIXME*** -- Is this any better than filling as needed??
#define PREFILL         // With blanks


//***************************************************************************
//  End of File: display.h
//***************************************************************************
