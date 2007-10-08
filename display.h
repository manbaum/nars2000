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
    UINT        uActRows,               // 1C:  # FMTROWSTRs to follow
                uActCols,               // 20:  # FMTCOLSTRs to follow
                uFmtRows,               // 24:  # formatted rows in this block (+/FMTROWSTR.uFmtRows)
                uFmtLdBl,               // 28:  ...         LdBl ...           (+/FMTCOLSTR.uLdBl)
                uFmtInts,               // 2C:  ...         ints ...           (+/FMTCOLSTR.uInts)
                uFmtFrcs,               // 30:  ...         frcs ...           (+/FMTCOLSTR.uFrcs)
                uFmtTrBl;               // 34:  ...         TrBl ...           (+/FMTCOLSTR.uTrBl)
    UINT        uDepth:31,              // 38:  7FFFFFFF:  Depth of this struct (0 = top)
                uMatRes:1;              //      80000000:  TRUE iff there is a rank > 1 item contained in this item
                                        // 3C:  Length
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
    UINT        uFmtRows:31,            // 08:  7FFFFFFF:  # formatted rows in this row
                uBlank:1;               //      80000000:  TRUE iff this row is blank
    UINT        uColOff;                // 0C:  Column offset of this row
    struct tagFMTROWSTR *lpFmtRowNxt;   // 10:  Ptr to next sibling FMTROWSTR
                                        // 14:  Length
} FMTROWSTR, *LPFMTROWSTR;

typedef struct tagFMTITMSTR
{
#ifdef DEBUG
#define FMTITMSTR_SIGNATURE 'IIII'      // 49494949
    HEADER_SIGNATURE Sig;               // 00:  FMTITMSTR signature
#endif
    struct tagFMTITMSTR *lpFmtItmRht;   // 04:  Ptr to right adjacent item
                                        // 08:  Length
} FMTITMSTR, *LPFMTITMSTR;



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
