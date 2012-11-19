//***************************************************************************
//  NARS2000 -- Display Header
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2012 Sudley Place Software

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
    struct tagFMTHEADER *lpFmtHeadUp;   // 00:  Ptr to next chain up
    struct tagFMTROWSTR *lpFmtRowUp;    // 04:  Ptr to parent FMTROWSTR
    struct tagFMTCOLSTR *lpFmtColUp;    // 08:  Ptr to parent FMTCOLSTR
    struct tagFMTROWSTR *lpFmtRow1st;   // 0C:  Ptr to 1st child FMTROWSTR
    struct tagFMTROWSTR *lpFmtRowLst;   // 10:  Ptr to last child FMTROWSTR
    struct tagFMTCOLSTR *lpFmtCol1st;   // 14:  Ptr to 1st child FMTCOLSTR
    UINT        uRealRows,              // 18:  # rows in the array's dimensions (top level only)
                uActRows,               // 1C:  # FMTROWSTRs to follow
                uActCols,               // 20:  # FMTCOLSTRs to follow
                uFmtRows,               // 24:  # formatted rows in this block (+/FMTROWSTR.uFmtRows)
                uFmtLdBl,               // 28:  ...         LdBl ...           (+/FMTCOLSTR.uLdBl)
                uFmtInts,               // 2C:  ...         ints ...           (+/FMTCOLSTR.uInts)
                uFmtChrs,               // 30:  ...         chars...           (+/FMTCOLSTR.uChrs)
                uFmtFrcs,               // 34:  ...         frcs ...           (+/FMTCOLSTR.uFrcs)
                uFmtTrBl;               // 38:  ...         TrBl ...           (+/FMTCOLSTR.uTrBl)
    UINT        uDepth:31,              // 3C:  7FFFFFFF:  Depth of this struct (0 = top)
                uMatRes:1;              //      80000000:  TRUE iff there is a rank > 1 item contained in this item
                                        // 40:  Length
#ifdef DEBUG
  #define FMTHEADER_SIGNATURE 'HHHH'    // 48484848
      HEADER_SIGNATURE Sig;             // 40:  FMTHEADER signature
                                        // 44:  Length
#endif
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
    UINT        uLdBl,                  // 00:  # Leading blanks
                uTrBl,                  // 04:  # Trailing blanks
                uInts,                  // 08:  # Integer digits in Boolean/Integer/APA/Float column,
                                        //      including sign
                uChrs,                  // 0C:  # CHARs
                uFrcs,                  // 10:  # Fractional digits in Float column
                                        //      including decimal point
                uFrc2;                  // 14:  # Fractional digits in Float column
                                        //      including decimal point, except this
                                        //      one is propagated up the line if there's
                                        //      only one column
    COLTYPES    colType;                // 18   Column type (see COLTYPES)
                                        // 1C:  Length
#ifdef DEBUG
  #define FMTCOLSTR_SIGNATURE 'CCCC'    // 43434343
      HEADER_SIGNATURE Sig;             // 1C:  FMTCOLSTR signature
                                        // 20:  Length
#endif
} FMTCOLSTR, *LPFMTCOLSTR;

typedef struct tagFMTROWSTR
{

    UINT        bRptCol:1,              // 00:  10000000:  TRUE iff we're to repeat a col in this row
                bDone:1,                //      20000000:  TRUE iff this row is done with output
                bRealRow:1,             //      40000000:  TRUE iff a real row (not from []TCLF)
                bBlank:1;               //      80000000:  TRUE iff this row is blank
    UINT        uFmtRows,               // 00:  # formatted rows in this row
                uColOff,                // 04:  Column offset of this row
                uItemCount,             // 08:  # following items
                uAccWid;                // 0C:  Accumulated width for this row
    struct tagFMTROWSTR *lpFmtRowNxt;   // 10:  Ptr to next sibling FMTROWSTR
    LPAPLCHAR   lpNxtChar,              // 12:  Ptr to next entry for raw output
                lpEndChar;              // 18:  ...    byte after last entry ...
                                        // 1C:  Length
#ifdef DEBUG
  #define FMTROWSTR_SIGNATURE 'RRRR'    // 52525252
      HEADER_SIGNATURE Sig;             // 1C:  FMTROWSTR signature
      FMTCOLSTR   *lpFmtColUp;          // 20:  Ptr to parent FMTCOLSTR
                                        // 24:  Length
#endif
} FMTROWSTR, *LPFMTROWSTR;

// ***FIXME*** -- Is this any better than filling as needed??
#define PREFILL         // With blanks


//***************************************************************************
//  End of File: display.h
//***************************************************************************
