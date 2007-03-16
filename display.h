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
    struct tagFMTHEADER *lpFmtHeadUp;   // Ptr to next chain up
    struct tagFMTROWSTR *lpFmtRowUp;    // Ptr to parent FMTROWSTR
    struct tagFMTCOLSTR *lpFmtColUp;    // Ptr to parent FMTCOLSTR
    struct tagFMTROWSTR *lpFmtRow1st;   // Ptr to 1st child FMTROWSTR
    struct tagFMTCOLSTR *lpFmtCol1st;   // Ptr to 1st child FMTCOLSTR
    UINT        uRows,                  // # FMTROWSTRs to follow
                uCols,                  // # FMTCOLSTRs to follow
                uFmtRows,               // # formatted rows in this block (+/FMTROWSTR.nFtmRows)
                uFmtInts,               // ...         ints ...           (+/FMTCOLSTR.uIntDigits)
                uFmtFrcs,               // ...         frcs ...           (+/FMTCOLSTR.uFrcDigits)
                uDepth;                 // Depth of this struct (0 = top)
} FMTHEADER, *LPFMTHEADER;

typedef struct tagFMTCOLSTR
{
    UCHAR       uFmtType,               // Format type (see FMT_TYPES)
                uIntDigits,             // # Integer digits in Boolean/Integer/APA/Float column,
                                        //   or # chars in Char column
                                        //   including sign and leading spaces
                uFrcDigits,             // # Fractional digits in Float column
                                        //   including decimal point
                uForAlignment;          // ***ALIGNME***
} FMTCOLSTR, *LPFMTCOLSTR;

typedef struct tagFMTROWSTR
{
    UINT        uFmtRows:31,            // # formatted rows in this row
                uBlank:1;               // TRUE iff this row is blank
    struct tagFMTROWSTR *lpFmtRowNxt;   // Ptr to next sibling FMTROWSTR
} FMTROWSTR, *LPFMTROWSTR;


//***************************************************************************
//  End of File: display.h
//***************************************************************************
