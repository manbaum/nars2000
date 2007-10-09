//***************************************************************************
//  NARS2000 -- Display Header
//***************************************************************************

typedef struct tagFMTHEADER
{
#ifdef DEBUG
#define FMTHEADER_SIGNATURE 'HHHH'      // 48484848
    HEADER_SIGNATURE Sig;               // 00:  FMTHEADER signature
#endif
    struct tagFMTHEADER *lpFmtHeadUp;   // 04:  Ptr to next chain up
    struct tagFMTROWSTR *lpFmtRowUp;    // 08:  Ptr to parent FMTROWSTR
    struct tagFMTCOLSTR *lpFmtColUp;    // 0C:  Ptr to parent FMTCOLSTR
    struct tagFMTROWSTR *lpFmtRow1st;   // 10:  Ptr to 1st child FMTROWSTR
    struct tagFMTROWSTR *lpFmtRowLst;   // 14:  Ptr to last child FMTROWSTR
    struct tagFMTCOLSTR *lpFmtCol1st;   // 18:  Ptr to 1st child FMTCOLSTR
    UINT        uRealRows,              // 1C:  # rows in the array's dimensions (top level only)
                uActRows,               // 20:  # FMTROWSTRs to follow
                uActCols,               // 24:  # FMTCOLSTRs to follow
                uFmtRows,               // 28:  # formatted rows in this block (+/FMTROWSTR.uFmtRows)
                uFmtLdBl,               // 2C:  ...         LdBl ...           (+/FMTCOLSTR.uLdBl)
                uFmtInts,               // 30:  ...         ints ...           (+/FMTCOLSTR.uInts)
                uFmtFrcs,               // 34:  ...         frcs ...           (+/FMTCOLSTR.uFrcs)
                uFmtTrBl;               // 38:  ...         TrBl ...           (+/FMTCOLSTR.uTrBl)
    UINT        uDepth:31,              // 3C:  7FFFFFFF:  Depth of this struct (0 = top)
                uMatRes:1;              //      80000000:  TRUE iff there is a rank > 1 item contained in this item
                                        // 40:  Length
} FMTHEADER, *LPFMTHEADER;

typedef struct tagFMTCOLSTR
{
#ifdef DEBUG
#define FMTCOLSTR_SIGNATURE 'CCCC'      // 43434343
    HEADER_SIGNATURE Sig;               // 00:  FMTCOLSTR signature
#endif
    UINT        uLdBl,                  // 04:  # leading blanks
                uInts,                  // 08:  # Integer digits in Boolean/Integer/APA/Float column,
                                        //      or # chars in Char column
                                        //      including sign
                uFrcs,                  // 0C:  # Fractional digits in Float column
                                        //      including decimal point
                uTrBl;                  // 10:  # trailing blanks
                                        // 14:  Length
} FMTCOLSTR, *LPFMTCOLSTR;

typedef struct tagFMTROWSTR
{
#ifdef DEBUG
#define FMTROWSTR_SIGNATURE 'RRRR'      // 52525252
    HEADER_SIGNATURE Sig;               // 00:  FMTROWSTR signature
    FMTCOLSTR   *lpFmtColUp;            // 04:  Ptr to parent FMTCOLSTR
#endif
    UINT        uFmtRows:30,            // 08:  3FFFFFFF:  # formatted rows in this row
                bRealRow:1,             //      40000000:  TRUE iff a real row (not from []TCLF)
                bBlank:1;               //      80000000:  TRUE iff this row is blank
    UINT        uColOff;                // 0C:  Column offset of this row
    struct tagFMTROWSTR *lpFmtRowNxt;   // 10:  Ptr to next sibling FMTROWSTR
                                        // 14:  Length
} FMTROWSTR, *LPFMTROWSTR;

////typedef enum tagFMT_TYPES
////{
////    FMTTYPE_INT = 0,                    // Integer format, e.g. -1234
////    FMTTYPE_FIX,                        // Fixed point ...      -1234.56
////    FMTTYPE_EXP,                        // Exponential ...      -1234.0E-19
////} FMT_TYPES;

// ***FIXME*** -- Is this any better than filling as needed??
#define PREFILL         // With blanks


//***************************************************************************
//  End of File: display.h
//***************************************************************************
