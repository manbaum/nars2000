//***************************************************************************
//  NARS2000 -- Datatype Header File
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

#if defined (_WIN32)
  typedef          __int32 APLI3264;    // Widest native signed value
  typedef unsigned __int32 APLU3264;    // ...           unsigned ...
#elif defined (_WIN64)
  typedef          __int64 APLI3264;    // Widest native signed value
  typedef unsigned __int64 APLU3264;    // ...           unsigned ...
#endif

// APLNESTED may be either an LPSYMENTRY or
//   an HGLOBAL.
// APLHETERO may be an LPSYMENTRY only.

typedef ULONGLONG   APLLONGEST;         // Longest datatype in TOKEN_DATA & SYMTAB_DATA

#define MAX_APLUINT     0xFFFFFFFFFFFFFFFF                      // Largest  APLINT
#define MAX_APLINT      0x7FFFFFFFFFFFFFFF                      // ...      APLINT
#define MAX_APLNELM     MAX_APLINT                              // ...      APLNELM
#define MAX_APLRANK     MAX_APLINT                              // ...      APLRANK
#define MAX_APLDIM      MAX_APLINT                              // ...      APLDIM
#define MIN_APLINT      0x8000000000000000                      // Smallest APLINT
#define MIN_APLINT_STR  WS_UTF16_OVERBAR L"9223372036854775808" // ...      ...    as a string
#define MAX_UNICODE     0x1FFFFF                                // Largest allowable Unicode code point

// # bits in an APLxxx
#define BITS_IN_APLCHAR (NBIB * (sizeof (APLCHAR)))
#define BITS_IN_APLINT  (NBIB * (sizeof (APLINT )))

// # integers in an APLCHAR
#define APLCHAR_SIZE    (BIT0 << BITS_IN_APLCHAR)

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

// Strand types -- used during strand constuction
typedef enum tagSTRAND_TYPES
{
 STRAND_INIT = 0,   // 00:  Initial state
 STRAND_BOOL,       // 01:  Boolean
 STRAND_INT,        // 02:  Integer
 STRAND_FLOAT,      // 03:  Floating point
 STRAND_CHAR,       // 04:  Character
 STRAND_CHARST,     // 05:  Character strand     (two or more character scalars in a row)
 STRAND_STRING,     // 06:  Character vector
 STRAND_HETERO,     // 07:  Simple heterogeneous (mixed numeric and character scalar)
 STRAND_NESTED,     // 08:  Nested

 STRAND_LENGTH      // 09:  # elements in this enum
                    //      *MUST* be the last entry
} STRAND_TYPES;

// Array types -- used to identify array storage type in memory
typedef enum tagARRAY_TYPES
{
 ARRAY_BOOL = 0,            // 00:  Boolean
 ARRAY_INT,                 // 01:  Integer
 ARRAY_FLOAT,               // 02:  Floating point
 ARRAY_CHAR,                // 03:  Character
 ARRAY_HETERO,              // 04:  Simple heterogeneous (mixed numeric and character scalars)
 ARRAY_NESTED,              // 05:  Nested
 ARRAY_LIST,                // 06:  List
 ARRAY_APA,                 // 07:  Arithmetic Progression Array

 ARRAY_LENGTH,              // 08:  # elements in this enum
                            //      *MUST* be the last non-error entry
                            // 09-0F:  Available entries (4 bits)
 ARRAY_INIT  = ARRAY_LENGTH,
 ARRAY_ERROR = (APLSTYPE) -1,

// Whenever changing this <enum>, be sure to make a
//   corresponding change to <StorageType> and <TypeDemote> in <primfns.c>,
//   <aTypePromote> and <uTypeMap> in <externs.h>,
//   <IsSimpleNH> and <IsSimpleNum> macros
//   and BPE_VEC in <datatype.h>.

} ARRAY_TYPES;

// Translate an array type to a char
// Note that the order of the chars in this #define
//   depends upon the ordering of the above enum
#define ArrayTypeAsChar     L"BIFCHNLA"

// Bites per element vector
// N.B. the order of elements in this vector matches
//   the order of elements in the above ARRAY_TYPES enum.
//                  B   I   F   C  H  N  L   A
#define BPE_VEC     1, 64, 64, 16, 0, 0, 0, 64

// Define APA structure
typedef struct tagAPLAPA    // Offset + Multiplier {times} {iota} Length (origin-0)
{
    APLINT  Off,        // Offset
            Mul;        // Multiplier
} APLAPA, * LPAPLAPA;

/*

Array Storage
-------------

Arrays are stored either in memory or, if it's a simple scalar,
in the symbol table.  If it's in the symbol table, it is identified
by its symbol table index.

All variable arrays in memory have a common header (VARARRAY_HEADER).
All function ...                                   (FCNARRAY_HEADER).
All list     ...                                   (LSTARRAY_HEADER).


Array Contents
--------------

The contents of a nested or a heterogeneous array may be
an LPSYMENTRY or an HGLOBAL.  These two types are
distinguished by their low-order two bits.  Because we know that
LPSYMENTRY and HGLOBAL are pointers and that these pointers are
aligned on a dword boundary, their two low-order bits are zero,
and thus available.


Array Types
-----------

ARRAY_BOOL      One value per bit, stored sequentially with
                the 1st value in bit position 0.

ARRAY_INT       One value per APLINT, stored sequentially.

ARRAY_FLOAT     One value per APLFLOAT, stored sequentially.

ARRAY_CHAR      One value per APLCHAR, stored sequentially.

ARRAY_HETERO    One value per APLHETERO, stored sequentially.
                The array contents are all LPSYMENTRYs.

ARRAY_NESTED    One value per APLNESTED, stored sequentially.
                The array contents may be a mixture of LPSYMENTRYs and
                HGLOBALs (which point to one of the other Array Types),
                all of which are distinguished by the low-order two bits.

ARRAY_LIST      One value per APLLIST, stored sequentially.
                Values are either VARIMMED, VARARRAY, or LISTSEP tokens.

ARRAY_APA       An APA is a representation of a1 + a2 {times} {iota} a3
                in origin-0.

                These three elements are stored in the array in succession:

                1.  a1 = Offset
                2.  a2 = Multiplier
                3.  a3 = Length

 */

typedef struct tagHEADER_SIGNATURE
{
    UINT             nature;    // 00:  Array header signature (common to all types of arrays)
} HEADER_SIGNATURE, *LPHEADER_SIGNATURE;

typedef enum tagPERM_NDX
{
    PERMNDX_NONE = 0,           // 00:  Not a permanent array
    PERMNDX_QUADA,              // 01:  []A
    PERMNDX_QUADAV,             // 02:  []AV
    PERMNDX_QUADDM,             // 03:  []DM default
    PERMNDX_QUADEM,             // 04:  []EM default
    PERMNDX_QUADFC,             // 05:  []FC default
    PERMNDX_QUADIC,             // 06:  []IC default
    PERMNDX_SACLEAR,            // 07:  'CLEAR'
    PERMNDX_SAERROR,            // 08:  'ERROR'
    PERMNDX_SAEXIT,             // 09:  'EXIT'
    PERMNDX_SAOFF,              // 0A:  'OFF'
    PERMNDX_V0CHAR,             // 0B:  ''
    PERMNDX_ZILDE,              // 0C:  {zilde}
                                // 0D:  Length
} PERM_NDX;

// Variable array header
#define VARARRAY_HEADER_SIGNATURE   'SRAV'

typedef struct tagVARARRAY_HEADER
{
    HEADER_SIGNATURE Sig;           // 00:  Array header signature
    UINT             ArrType:4,     // 04:  0000000F:  The type of the array (see ARRAY_TYPES)
                     PermNdx:4,     //      000000F0:  Permanent array index (e.g., PERMNDX_ZILDE for {zilde})
                     SysVar:1,      //      00000100:  Izit for a Sysvar (***DEBUG*** only)?
                     PV0:1,         //      00000200:  Permutation Vector in origin-0
                     PV1:1,         //      00000400:  ...                          1
                     bSelSpec:1,    //      00000800:  Select Specification array
                     Avail:20;      //      FFFFF000:  Available bits
    UINT             RefCnt;        // 08:  Reference count
    APLNELM          NELM;          // 0C:  # elements in the array
    APLRANK          Rank;          // 10:  The rank of the array
                                    //      followed by the dimensions
                                    // 14:  Length
} VARARRAY_HEADER, *LPVARARRAY_HEADER;

// List array header
#define LSTARRAY_HEADER_SIGNATURE   'TSIL'

typedef struct tagLSTARRAY_HEADER
{
    HEADER_SIGNATURE Sig;       // 00:  Array header signature
    APLNELM          NELM;      // 04:  # elements in the array (8 bytes)
                                // 0C:  Length
} LSTARRAY_HEADER, *LPLSTARRAY_HEADER;

// Function array header signature
#define FCNARRAY_HEADER_SIGNATURE   'SNCF'

// Function array header
typedef struct tagFCNARRAY_HEADER
{
    HEADER_SIGNATURE Sig;           // 00:  Array header signature
    UINT             fnNameType:4,  // 04:  0000000F:  The type of the array (see NAME_TYPES)
                     Avail:28;      //      FFFFFFF0:  Available bits
    UINT             RefCnt,        // 08:  Reference count
                     tknNELM;       // 0C:  # tokens in the array (each of which may point to additional arrays)
    HGLOBAL          hGlbTxtLine;   // 10:  Line text global memory handle (may be NULL)
    FILETIME         ftCreation,    // 14:  Time of last creation (8 bytes)
                     ftLastMod;     // 1C:  Time of last modification (8 bytes)
                                    // 24:  Length
} FCNARRAY_HEADER, *LPFCNARRAY_HEADER;

// Named strand header
#define VARNAMED_HEADER_SIGNATURE   'RTSN'

typedef struct tagVARNAMED_HEADER
{
    HEADER_SIGNATURE Sig;       // 00:  Array header signature
    APLNELM          NELM;      // 04:  # elements in the array (8 bytes)
                                // 0C:  Length
} VARNAMED_HEADER, *LPVARNAMED_HEADER;

// Distinguish between immediate LPSYMENTRY and HGLOBAL in an array
typedef enum tagPTR_TYPES
{
 PTRTYPE_STCONST = 0,   // 00:  *MUST* be first (and thus zero) as we can't afford to clear all the time
 PTRTYPE_HGLOBAL,       // 01:  This ptr is an HGLOBAL
 PTRTYPE_LENGTH         // 02:  # entries in this enum (1 bit)
} PTR_TYPES;            // No available entries (1 bit)

#define PTRTYPE_MASK    1       // This masks the one low-order bit

// For LPSYMENTRY and HGLOBAL values in a temporary array, sometimes
//   those values can be re-used in another array without having
//   to make a copy.  In that case, the original value is replaced
//   by this which is checked for before trying to free it.
#define PTR_REUSED  ((LPVOID) (HANDLE_PTR) 0xFFFFFFFF)


//***************************************************************************
//  End of File: datatype.h
//***************************************************************************
