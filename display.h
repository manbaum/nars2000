//***************************************************************************
//	NARS2000 -- Display Header
//***************************************************************************


////typedef enum tagFMT_TYPES
////{
////	FMTTYPE_INT = 0,					// Integer format, e.g. -1234
////	FMTTYPE_FIX,						// Fixed point ...		-1234.56
////	FMTTYPE_EXP,						// Exponential ...		-1234.0E-19
////} FMT_TYPES;
////
typedef struct tagFMTHEADER
{
#ifdef DEBUG
#define FMTHEADER_SIGNATURE 'HHHH'      // 48484848
	HEADER_SIGNATURE Sig;				// 00:	FMTHEADER signature
#endif
	struct tagFMTHEADER *lpFmtHeadUp;	// 04:	Ptr to next chain up
	struct tagFMTROWSTR *lpFmtRowUp;	// 08:	Ptr to parent FMTROWSTR
	struct tagFMTCOLSTR *lpFmtColUp;	// 0C:	Ptr to parent FMTCOLSTR
	struct tagFMTROWSTR *lpFmtRow1st;	// 10:	Ptr to 1st child FMTROWSTR
	struct tagFMTCOLSTR *lpFmtCol1st;	// 14:	Ptr to 1st child FMTCOLSTR
	UINT		uRows,					// 18:	# (FMTROWSTRs, DATAs) to follow
				uCols,					// 1C:	# FMTCOLSTRs to follow
				uFmtRows,				// 20:	# formatted rows in this block (+/FMTROWSTR.uFmtRows)
				uFmtInts,				// 24:	... 		ints ...		   (+/FMTCOLSTR.uInts)
				uFmtFrcs;				// 28:	... 		frcs ...		   (+/FMTCOLSTR.uFrcs)
	UINT		uDepth:30,				// 2C:	3FFFFFFF:  Depth of this struct (0 = top)
				uFmtTrBl:1, 			//		40000000:  TRUE iff trailing blank surrounding this item
				uMatRes:1;				//		80000000:  TRUE iff there is a rank > 1 item contained in this item
} FMTHEADER, *LPFMTHEADER;

typedef struct tagFMTCOLSTR
{
#ifdef DEBUG
#define FMTCOLSTR_SIGNATURE 'CCCC'      // 43434343
	HEADER_SIGNATURE Sig;				// 00:	FMTCOLSTR signature
#endif
////UINT		uFmtType;				//		Format type (see FMT_TYPES)
	UINT		uInts,					// 04:	# Integer digits in Boolean/Integer/APA/Float column,
										//		or # chars in Char column
										//		including sign and leading spaces
				uFrcs;					// 08:	# Fractional digits in Float column
										//		including decimal point
										// 0C:	Length
} FMTCOLSTR, *LPFMTCOLSTR;

typedef struct tagFMTROWSTR
{
#ifdef DEBUG
#define FMTROWSTR_SIGNATURE 'RRRR'      // 52525252
	HEADER_SIGNATURE Sig;				// 00:	FMTROWSTR signature
#endif
	UINT		uFmtRows:31,			// 04:	7FFFFFFF:  # formatted rows in this row
				uBlank:1;				//		80000000:  TRUE iff this row is blank
	struct tagFMTROWSTR *lpFmtRowNxt;	// 08:	Ptr to next sibling FMTROWSTR
} FMTROWSTR, *LPFMTROWSTR;

// ***FIXME*** -- Is this any better than filling as needed??
#define PREFILL 		// With blanks


//***************************************************************************
//	End of File: display.h
//***************************************************************************
