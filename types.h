//***************************************************************************
//  NARS2000 -- Types
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2010 Sudley Place Software

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


typedef unsigned int   UBOOL, *LPUBOOL;
typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;


typedef ULONGLONG   APLRANK;            // The type of the rank element in an array
typedef  LONGLONG   APLRANKSIGN;        // ...             rank element ... (signed)
typedef ULONGLONG   APLNELM;            // ...             # elements   ...
typedef  LONGLONG   APLNELMSIGN;        // ...             # elements   ... (signed)
typedef ULONGLONG   APLDIM;             // ...             dimensions   ...
typedef UCHAR       APLBOOL;            // ...         an element in a Boolean array (unsigned)
typedef  LONGLONG   APLINT;             // ...         ...           an integer array
typedef ULONGLONG   APLUINT;            // ...         ...           an integer array (unsigned)
typedef WCHAR       APLCHAR;            // ...         ...           a character
typedef double      APLFLOAT;           // ...         ...           a Floating Point array
typedef HGLOBAL     APLNESTED;          // ...         ...           a nested array
typedef struct tagSYMENTRY *APLHETERO;  // ...         ...           a HETERO array
typedef struct tagTOKEN APLLIST;        // ...                       a list
typedef UCHAR       APLSTYPE;           // Storage type (see ARRAY_TYPES)

#if defined (_WIN64)
  typedef          __int64 APLI3264;    // Widest native signed value
  typedef unsigned __int64 APLU3264;    // ...           unsigned ...
#elif defined (_WIN32)
  typedef          __int32 APLI3264;    // Widest native signed value
  typedef unsigned __int32 APLU3264;    // ...           unsigned ...
#else
  #error Need code for this architecture.
#endif

// APLNESTED may be either an LPSYMENTRY or
//   an HGLOBAL.
// APLHETERO may be an LPSYMENTRY only.

typedef ULONGLONG   APLLONGEST;         // Longest datatype in TOKEN_DATA & SYMTAB_DATA

// Define ptrs
typedef char      * LPCHAR;
typedef WCHAR     * LPWCHAR;
typedef HGLOBAL   * LPLIST;
typedef UCHAR     * LPUCHAR;
typedef APLSTYPE  * LPAPLSTYPE;

typedef APLRANK   * LPAPLRANK;
typedef APLNELM   * LPAPLNELM;
typedef APLDIM    * LPAPLDIM;
typedef APLBOOL   * LPAPLBOOL;
typedef APLINT    * LPAPLINT;
typedef APLUINT   * LPAPLUINT;
typedef APLCHAR   * LPAPLCHAR;
typedef APLNESTED * LPAPLNESTED;
typedef APLLIST   * LPAPLLIST;
typedef APLFLOAT  * LPAPLFLOAT;
typedef APLHETERO * LPAPLHETERO;

typedef APLLONGEST* LPAPLLONGEST;


//***************************************************************************
//  End of File: types.h
//***************************************************************************
