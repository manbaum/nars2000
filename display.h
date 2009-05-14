//***************************************************************************
//  NARS2000 -- Display Header
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2009 Sudley Place Software

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

/***************************************************************************

The sequence of structs matches the structure of the array being
displayed/formatted.

The topmost struct is

(FMTHEADER) (nCols * FMTCOLSTR) (nRows * FMTROWSTR)

If the data for the first item is simple, it follows in the stream.
Otherwise, FMTHEADER appears and the process nests
Each item is treated the same way as above.

COLSTRS:
      FMTCOLSTR
    | COLSTRS FMTCOLSTR
    ;

ROWSTRS:
      FMTROWSTR
    | ROWSTRS FMTROWSTR
    ;

HEADER:
      FMTHEADER COLSTRS ROWSTRS
    | FMTHEADER COLSTRS ROWSTRS Data
    | FMTHEADER COLSTRS ROWSTRS HEADER
    ;

***************************************************************************/

typedef struct tagFMTHEADER
{
#ifdef DEBUG
  #define FMTHEADER_SIGNATURE 'HHHH'    // 48484848
      HEADER_SIGNATURE Sig;             // 00:  FMTHEADER signature
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
                uFmtChrs,               // 34:  ...         chars...           (+/FMTCOLSTR.uChrs)
                uFmtFrcs,               // 38:  ...         frcs ...           (+/FMTCOLSTR.uFrcs)
                uFmtTrBl;               // 3C:  ...         TrBl ...           (+/FMTCOLSTR.uTrBl)
    UINT        uDepth:31,              // 40:  7FFFFFFF:  Depth of this struct (0 = top)
                uMatRes:1;              //      80000000:  TRUE iff there is a rank > 1 item contained in this item
                                        // 44:  Length
} FMTHEADER, *LPFMTHEADER;

typedef enum tagCOL_TYPES
{
    COLTYPE_UNK = 0,                    // 00:  Type is unspecified
    COLTYPE_ALLCHAR,                    // 01:  Column consists of all character data
    COLTYPE_NOTCHAR,                    // 02:  Column is all numeric or mixed character and numeric
                                        // 03:  Available entry (2 bits)
} COLTYPES, *LPCOLTYPES;

// N.B.  The order of the values in the above enum is important such that
//       COLTYPE_NOTCHAR must have a higher value than COLTYPE_ALLCHAR
//       so we can use the max macro.

typedef struct tagFMTCOLSTR
{
#ifdef DEBUG
  #define FMTCOLSTR_SIGNATURE 'CCCC'    // 43434343
      HEADER_SIGNATURE Sig;             // 00:  FMTCOLSTR signature
#endif
    UINT        uLdBl,                  // 04:  # leading blanks
                uInts,                  // 08:  # Integer digits in Boolean/Integer/APA/Float column,
                                        //      including sign
                uChrs,                  // 0C:  # CHARs
                uFrcs,                  // 10:  # Fractional digits in Float column
                                        //      including decimal point
                uFrc2;                  // 14:  # Fractional digits in Float column
                                        //      including decimal point, except this
                                        //      one is propagated up the line if there's
                                        //      only one column
    UINT        uTrBl:30,               // 18:  3FFFFFFFF:  # trailing blanks
                colType:2;              //      C00000000:  column type (see COL_TYPES)
                                        // 1C:  Length
} FMTCOLSTR, *LPFMTCOLSTR;

typedef struct tagFMTROWSTR
{
#ifdef DEBUG
  #define FMTROWSTR_SIGNATURE 'RRRR'    // 52525252
      HEADER_SIGNATURE Sig;             // 00:  FMTROWSTR signature
      FMTCOLSTR   *lpFmtColUp;          // 04:  Ptr to parent FMTCOLSTR
#endif
    UINT        uFmtRows:30,            // 08:  3FFFFFFF:  # formatted rows in this row
                bRealRow:1,             //      40000000:  TRUE iff a real row (not from []TCLF)
                bBlank:1;               //      80000000:  TRUE iff this row is blank
    UINT        uColOff;                // 0C:  Column offset of this row
    UINT        uItemCount;             // 10:  # following items
    struct tagFMTROWSTR *lpFmtRowNxt;   // 14:  Ptr to next sibling FMTROWSTR
                                        // 18:  Length
} FMTROWSTR, *LPFMTROWSTR;

// ***FIXME*** -- Is this any better than filling as needed??
#define PREFILL         // With blanks

// Define char to display for infinity
#define CHR_INFINITY        UTF16_INFINITY


//***************************************************************************
//  End of File: display.h
//***************************************************************************
