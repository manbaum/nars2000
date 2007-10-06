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
    UINT        uRows,                  // 1C:  # (FMTROWSTRs, DATAs) to follow
                uCols,                  // 20:  # FMTCOLSTRs to follow
                uFmtRows,               // 24:  # formatted rows in this block (+/FMTROWSTR.uFmtRows)
                uFmtInts,               // 28:  ...         ints ...           (+/FMTCOLSTR.uInts)
                uFmtFrcs;               // 2C:  ...         frcs ...           (+/FMTCOLSTR.uFrcs)
    UINT        uDepth:30,              // 30:  3FFFFFFF:  Depth of this struct (0 = top)
                uFmtTrBl:1,             //      40000000:  TRUE iff trailing blank surrounding this item
                uMatRes:1;              //      80000000:  TRUE iff there is a rank > 1 item contained in this item
                                        // 34:  Length
} FMTHEADER, *LPFMTHEADER;

typedef struct tagFMTCOLSTR
{
#ifdef DEBUG
#define FMTCOLSTR_SIGNATURE 'CCCC'      // 43434343
    HEADER_SIGNATURE Sig;               // 00:  FMTCOLSTR signature
#endif
////UINT        uFmtType;               //      Format type (see FMT_TYPES)
    UINT        uLdBl:16,               // 04:  0000FFFF:  # leading blanks
                uTrBl:16;               //      FFFF0000:  # trailing blanks
    UINT        uInts,                  // 08:  # Integer digits in Boolean/Integer/APA/Float column,
                                        //      or # chars in Char column
                                        //      including sign
                uFrcs;                  // 0C:  # Fractional digits in Float column
                                        //      including decimal point
                                        // 10:  Length
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
    struct tagFMTROWSTR *lpFmtRowNxt;   // 0C:  Ptr to next sibling FMTROWSTR
    struct tagFMTROWSTR *lpFmtRowRht;   // 10:  Ptr to right adjacent row
                                        // 14:  Length
} FMTROWSTR, *LPFMTROWSTR;

typedef struct tagFMTITMSTR
{
#ifdef DEBUG
#define FMTITMSTR_SIGNATURE 'IIII'      // 49494949
    HEADER_SIGNATURE Sig;               // 00:  FMTITMSTR signature
    FMTCOLSTR   *lpFmtColUp;            // 04:  Ptr to parent FMTCOLSTR
#endif
    UINT        uFmtRows:31,            // 08:  7FFFFFFF:  # formatted rows in this row
                uBlank:1;               //      80000000:  TRUE iff this row is blank
    struct tagFMTROWSTR *lpFmtRowNxt;   // 0C:  Ptr to next sibling FMTROWSTR
    struct tagFMTITMSTR *lpFmtItmRht;   // 10:  Ptr to right adjacent item
                                        // 14:  Length
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
