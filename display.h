//***************************************************************************
//  NARS2000 -- Display Header
//***************************************************************************


typedef enum tagFMT_TYPES
{
    FMTTYPE_INT = 0,                    // Integer format, e.g. -1234
    FMTTYPE_FIX,                        // Fixed point ...      -1234.56
    FMTTYPE_EXP,                        // Exponential ...      -1234.0E-19
} FMT_TYPES;

typedef struct tagFMTHEADER
{
#ifdef DEBUG
#define FMTHEADER_SIGNATURE 'HHHH'
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
    UINT        uFmtTrBl:31,            // # trailing blanks              (0 or 1)
                uMatRes:1;              // TRUE iff there is a rank > 1 item
    UINT        uDepth;                 // Depth of this struct (0 = top)
} FMTHEADER, *LPFMTHEADER;

typedef struct tagFMTCOLSTR
{
#ifdef DEBUG
#define FMTCOLSTR_SIGNATURE 'CCCC'
    UINT        Signature;              // FMTCOLSTR signature
#endif
    UCHAR       uFmtType,               // Format type (see FMT_TYPES)
                uInts,                  // # Integer digits in Boolean/Integer/APA/Float column,
                                        //   or # chars in Char column
                                        //   including sign and leading spaces
                uFrcs,                  // # Fractional digits in Float column
                                        //   including decimal point
                uFmtRows;               // # formatted rows in this col
////////////////uAlignMe;               // ***ALIGNME***
} FMTCOLSTR, *LPFMTCOLSTR;

typedef struct tagFMTROWSTR
{
#ifdef DEBUG
#define FMTROWSTR_SIGNATURE 'RRRR'
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
