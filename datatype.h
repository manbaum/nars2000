//***************************************************************************
//  NARS2000 -- Datatype Header File
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2008 Sudley Place Software

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
typedef LPVOID      APLLIST;            // ...                       a list
typedef UCHAR       APLSTYPE;           // Storage type (see ARRAY_TYPES)

// APLLIST and APLNESTED may be either an LPSYMENTRY or
//   an HGLOBAL.
// APLHETERO may be an LPSYMENTRY only.

typedef ULONGLONG   APLLONGEST;         // Longest datatype in TOKEN_DATA & SYMTAB_DATA

#define MAX_APLNELM 0xFFFFFFFFFFFFFFFF  // Largest APLNELM
#define MAX_APLDIM  0xFFFFFFFFFFFFFFFF  // ...     APLDIM
#define MAX_APLINT  0xFFFFFFFFFFFFFFFF  // ...     APLINT

#define SIGN_APLNELM(a)     ((a) >> 63)     // Sign bit of an APLNELM
#define SIGN_APLRANK(a)     ((a) >> 63)     // ...            APLRANK
#define SIGN_APLDIM(a)      ((a) >> 63)     // ...            APLDIM
#define SIGN_APLINT(a)      (((APLUINT) a) >> 63) // ...      APLINT
#define SIGN_APLUINT(a)     ((a) >> 63)     // ...            APLUINT
#define SIGN_APLLONGEST(a)  ((a) >> 63)     // ...            APLLONGEST

// # integers in an APLCHAR
#define APLCHAR_SIZE    (BIT0 << (NBIB * sizeof (APLCHAR)))

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
 ARRAY_MIXED = (APLSTYPE) -2,
 ARRAY_ERROR = (APLSTYPE) -1,

// Whenever changing this <enum>, be sure to make a
//   corresponding change to <StorageType> and <TypeDemote>
//   in <primfns.c>,  <uTypeMap> in <externs.h>,
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
#define BPE_VEC     1, 64, 64, 16, 0, 0, 0, 0

// Define macro for detecting simple array type
/////// IsSimple(a)         (a EQ ARRAY_BOOL || a EQ ARRAY_INT || a EQ ARRAY_APA || a EQ ARRAY_FLOAT || a EQ ARRAY_CHAR || a EQ ARRAY_HETERO)
#define IsSimple(a)         (uTypeMap[a] <= uTypeMap[ARRAY_HETERO])

// Define macro for detecting simple non-heterogeneous array type
/////// IsSimpleNH(a)       (a EQ ARRAY_BOOL || a EQ ARRAY_INT || a EQ ARRAY_APA || a EQ ARRAY_FLOAT || a EQ ARRAY_CHAR)
#define IsSimpleNH(a)       (uTypeMap[a] < uTypeMap[ARRAY_HETERO])

// Define macro for detecting simple numeric array type
/////// IsSimpleNum(a)      (a EQ ARRAY_BOOL || a EQ ARRAY_INT || a EQ ARRAY_APA || a EQ ARRAY_FLOAT)
#define IsSimpleNum(a)      (uTypeMap[a] < uTypeMap[ARRAY_CHAR])

// Define macro for detecting simple Boolean array type
#define IsSimpleBool(a)     (a EQ ARRAY_BOOL)

// Define macro for detecting simple integer-like array type
#define IsSimpleInt(a)      (a EQ ARRAY_BOOL || a EQ ARRAY_INT || a EQ ARRAY_APA)

// Define macro for detecting simple APA array type
#define IsSimpleAPA(a)      (a EQ ARRAY_APA)

// Define macro for detecting simple float array type
#define IsSimpleFlt(a)      (a EQ ARRAY_FLOAT)

// Define macro for detecting simple character array type
#define IsSimpleChar(a)     (a EQ ARRAY_CHAR)

// Define macro for detecting simple character or heterogeneous array type
#define IsSimpleCH(a)       (a EQ ARRAY_CHAR || a EQ ARRAY_HETERO)

// Define macro for detecting simple hetero array type
#define IsSimpleHet(a)      (a EQ ARRAY_HETERO)

// Define macro for detecting nested array type
#define IsNested(a)         (a EQ ARRAY_NESTED)

// Define macro for detecting simple Boolean value
#define IsBooleanValue(a)   (a EQ 0 || a EQ 1)

// Define macro for detecting a parenthetic list
#define IsTknParList(a)     ((a)->tkFlags.TknType EQ TKT_LISTPAR)

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
All function arrays in memory have a common header (FCNARRAY_HEADER).


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
                Values are either LPSYMENTRYs or HGLOBALs (which
                point to one of the other Array Types).

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
    PERMNDX_ZILDE,              // 01:  {zilde}
    PERMNDX_QUADEM,             // 02:  {quad}EM default
    PERMNDX_QUADDM,             // 03:  {quad}DM default
    PERMNDX_MTCHAR,             // 04:  ''
    PERMNDX_SACLEAR,            // 05:  'CLEAR'
    PERMNDX_SAERROR,            // 06:  'ERROR'
    PERMNDX_SAEXIT,             // 07:  'EXIT'
    PERMNDX_SAOFF,              // 08:  'OFF'
    PERMNDX_QUADFC,             // 09:  []FC default
    PERMNDX_QUADIC,             // 0A:  []IC default
    PERMNDX_QUADAV,             // 0B:  []AV
} PERM_NDX;

// Variable array header
#define VARARRAY_HEADER_SIGNATURE   'SRAV'

typedef struct tagVARARRAY_HEADER
{
    HEADER_SIGNATURE Sig;       // 00:  Array header signature
    UINT             ArrType:4, // 04:  0000000F:  The type of the array (see ARRAY_TYPES)
                     PermNdx:4, //      000000F0:  Permanent array index (e.g., PERMNDX_ZILDE for {zilde})
                     SysVar:1,  //      00000100:  Izit for a Sysvar (***DEBUG*** only)?
                     Avail:23;  //      FFFFFE00:  Available bits
    UINT             RefCnt;    // 08:  Reference count
    APLNELM          NELM;      // 0C:  # elements in the array
    APLRANK          Rank;      // 10:  The rank of the array
                                //      followed by the dimensions
                                // 14:  Length
} VARARRAY_HEADER, *LPVARARRAY_HEADER;

// Macros to skip from the array base to either the dimensions or the data
#define VarArrayBaseToDim(lpMem)          (LPAPLDIM) (((LPCHAR) lpMem) + sizeof (VARARRAY_HEADER)                            )
#define VarArrayBaseToData(lpMem,aplRank) (LPVOID)   (((LPCHAR) lpMem) + sizeof (VARARRAY_HEADER) + sizeof (APLDIM) * aplRank)
#define VarArrayDimToData(lpMem,aplRank)  (LPVOID)   (((LPCHAR) lpMem)                            + sizeof (APLDIM) * aplRank)

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
    FILETIME         ftCreation;    // 14:  Time of last creation
                                    // 14:  Length
} FCNARRAY_HEADER, *LPFCNARRAY_HEADER;

// Macros to skip from the array base to the data
#define FcnArrayBaseToData(lpMem) (LPVOID)   (((LPCHAR) lpMem) + sizeof (FCNARRAY_HEADER))

// Named strand header
#define VARNAMED_HEADER_SIGNATURE   'RTSN'

typedef struct tagVARNAMED_HEADER
{
    HEADER_SIGNATURE Sig;       // 00:  Array header signature
    APLNELM          NELM;      // 04:  # elements in the array
                                // 0C:  Length
} VARNAMED_HEADER, *LPVARNAMED_HEADER;

// Macros to skip from the array base to the data
#define VarNamedBaseToData(lpMem) (LPVOID)   (((LPCHAR) lpMem) + sizeof (VARNAMED_HEADER))

// Distinguish between immediate LPSYMENTRY and HGLOBAL in an array
typedef enum tagPTR_TYPES
{
 PTRTYPE_STCONST = 0,   // 00:  *MUST* be first (and thus zero) as we can't afford to clear all the time
 PTRTYPE_HGLOBAL,       // 01:  This ptr is an HGLOBAL
 PTRTYPE_AVAIL,         // 02:  Available entries (2 bits)
 PTRTYPE_REUSED,        // 03:  Used by PTR_REUSED
} PTR_TYPES;            // No available entries (2 bits)

#define PTRTYPE_MASK     3      // This masks the two low-order bits

// Macros to clear the low-order bits of either an LPSYMENTRY,
//   or HGLOBAL (luckily, both types of ptrs are the same size).
// These macros come in either direct (Dir) or indirect (Ind) form
#define ClrPtrTypeDir(lpMem)                       ((~PTRTYPE_MASK) &  ( UINT)     lpMem)
#define ClrPtrTypeDirAsSym(lpMem)     (LPSYMENTRY) ((~PTRTYPE_MASK) &  ( UINT)     lpMem)
#define ClrPtrTypeDirAsGlb(lpMem)     (HGLOBAL)    ((~PTRTYPE_MASK) &  ( UINT)     lpMem)
#define ClrPtrTypeDirAsFcn(lpMem)     (LPPRIMFNS)  ((~PTRTYPE_MASK) &  ( UINT)     lpMem)
#define ClrPtrTypeInd(lpMem)                       ((~PTRTYPE_MASK) & *(PUINT_PTR) lpMem)
#define ClrPtrTypeIndAsSym(lpMem)     (LPSYMENTRY) ((~PTRTYPE_MASK) & *(PUINT_PTR) lpMem)
#define ClrPtrTypeIndAsGlb(lpMem)     (HGLOBAL)    ((~PTRTYPE_MASK) & *(PUINT_PTR) lpMem)

// Macro to extract the low-order bits of a memory ptr used
//   to distinguish between the various pointer types.
#define GetPtrTypeInd(lpMem)    (  PTRTYPE_MASK  & *(PUINT_PTR) lpMem)
#define GetPtrTypeDir(lpMem)    (  PTRTYPE_MASK  &  ( UINT)     lpMem)

// Macro to create a masked LPSYMENTRY
#define MakePtrTypeSym(lpMem)        ((LPSYMENTRY) (PTRTYPE_STCONST | (UINT) lpMem))

// Macro to create a masked HGLOBAL
#define MakePtrTypeGlb(lpMem)        ((HGLOBAL)    (PTRTYPE_HGLOBAL | (UINT) lpMem))

// For LPSYMENTRY and HGLOBAL values in a temporary array, sometimes
//   those values can be re-used in another array without having
//   to make a copy.  In that case, the original value is replaced
//   by this which is checked for before trying to free it.
#define PTR_REUSED  ((LPVOID) (0xFFFFFFF0 + PTRTYPE_REUSED))

#define PtrReusedDir(lpMem)                  (lpMem  EQ PTR_REUSED)
#define PtrReusedInd(lpMem)     ((*(LPVOID *) lpMem) EQ PTR_REUSED)


//***************************************************************************
//  End of File: datatype.h
//***************************************************************************
