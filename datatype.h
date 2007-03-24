//***************************************************************************
//	NARS2000 -- Datatype Header File
//***************************************************************************

typedef ULONGLONG	APLRANK;	// The type of the rank element in an array
typedef  LONGLONG	APLRANKSIGN;// ...			   rank element ... (signed)
typedef ULONGLONG	APLNELM;	// ...			   # elements	...
typedef  LONGLONG	APLNELMSIGN;// ...			   # elements	... (signed)
typedef ULONGLONG	APLDIM; 	// ...			   dimensions	...
typedef UCHAR		APLBOOL;	// ...		   an element in a Boolean array (unsigned)
typedef  LONGLONG	APLINT; 	// ...		   ...			 an integer array
typedef ULONGLONG	APLUINT;	// ...		   ...			 an integer array (unsigned)
typedef WCHAR		APLCHAR;	// ...		   ...			 a character
typedef double		APLFLOAT;	// ...		   ...			 a Floating Point array
typedef HGLOBAL 	APLNESTED;	// ...		   ...			 a nested array
typedef LPVOID		APLHETERO;	// ...		   ...			 a HETERO array
typedef LPVOID		APLLIST;	// ...						 a list
typedef UCHAR		APLSTYPE;	// Storage type (see ARRAY_TYPES)

// APLLIST, APLHETERO, and APLNESTED may be either an LPSYMENTRY or
//	 an HGLOBAL.

typedef ULONGLONG	APLLONGEST; 		// Longest datatype in tagTOKEN_DATA & tagSYMTAB_DATA

#define MAX_APLNELM 0xFFFFFFFFFFFFFFFF	// Largest APLNELM
#define MAX_APLDIM	0xFFFFFFFFFFFFFFFF	// ...	   APLDIM
#define MAX_APLINT	0xFFFFFFFFFFFFFFFF	// ...	   APLINT

#define SIGN_APLNELM(a) ((a) >> 63) 	// Sign bit of an APLNELM
#define SIGN_APLDIM(a)	((a) >> 63) 	// ...			  APLDIM
#define SIGN_APLINT(a)	((a) >> 63) 	// ...			  APLINT


// Define ptrs
typedef char	  * LPCHAR;
typedef WCHAR	  * LPWCHAR;
typedef HGLOBAL   * LPLIST;
typedef UCHAR	  * LPUCHAR;
typedef APLSTYPE  * LPAPLSTYPE;

typedef APLRANK   * LPAPLRANK;
typedef APLNELM   * LPAPLNELM;
typedef APLDIM	  * LPAPLDIM;
typedef APLBOOL   * LPAPLBOOL;
typedef APLINT	  * LPAPLINT;
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
 STRAND_INIT = 0,	//	0:	Initial state
 STRAND_BOOL,		//	1:	Boolean
 STRAND_INT,		//	2:	Integer
 STRAND_FLOAT,		//	3:	Floating point
 STRAND_CHAR,		//	4:	Character
 STRAND_CHARST, 	//	5:	Character strand	 (two or more character scalars in a row)
 STRAND_STRING, 	//	6:	Character vector
 STRAND_HETERO, 	//	7:	Simple heterogeneous (mixed numeric and character scalar)
 STRAND_NESTED, 	//	8:	Nested

 STRAND_LENGTH		//	9:	# elements in this enum
					//		*MUST* be the last entry
} STRAND_TYPES;

// Array types -- used to identify array storage type in memory
typedef enum tagARRAY_TYPES
{
 ARRAY_BOOL = 0,			//	0:	Boolean
 ARRAY_INT, 				//	1:	Integer
 ARRAY_FLOAT,				//	2:	Floating point
 ARRAY_CHAR,				//	3:	Character
 ARRAY_HETERO,				//	4:	Simple heterogeneous (mixed numeric and character scalars)
 ARRAY_NESTED,				//	5:	Nested
 ARRAY_LIST,				//	6:	List
 ARRAY_APA, 				//	7:	Arithmetic Progression Array

 ARRAY_LENGTH,				//	8:	# elements in this enum
							//		*MUST* be the last non-error entry
 ARRAY_MIXED = (APLSTYPE) -2,
 ARRAY_ERROR = (APLSTYPE) -1,

// Whenever changing this <enum>, be sure to make a
//	 corresponding change to <StorageType> and <TypeDemote>
//	 in <primfns.c>,  <uTypeMap> in <externs.h>, and
//	 <IsSimpleNH> and <IsSimpleNum> macros in <datatype.h>.

} ARRAY_TYPES;

// Define macro for detecting simple array type
/////// IsSimple(a)    (a EQ ARRAY_BOOL || a EQ ARRAY_INT || a EQ ARRAY_APA || a EQ ARRAY_FLOAT || a EQ ARRAY_CHAR || a EQ ARRAY_HETERO)
#define IsSimple(a)    (uTypeMap[a] <= uTypeMap[ARRAY_HETERO])

// Define macro for detecting simple non-heterogeneous array type
/////// IsSimpleNH(a)  (a EQ ARRAY_BOOL || a EQ ARRAY_INT || a EQ ARRAY_APA || a EQ ARRAY_FLOAT || a EQ ARRAY_CHAR)
#define IsSimpleNH(a)  (uTypeMap[a] < uTypeMap[ARRAY_HETERO])

// Define macro for detecting simple numeric array type
/////// IsSimpleNum(a) (a EQ ARRAY_BOOL || a EQ ARRAY_INT || a EQ ARRAY_APA || a EQ ARRAY_FLOAT)
#define IsSimpleNum(a) (uTypeMap[a] < uTypeMap[ARRAY_CHAR])

// Define macro for detecting simple integer-like array type
#define IsSimpleInt(a) (a EQ ARRAY_BOOL || a EQ ARRAY_INT || a EQ ARRAY_APA)

// Define macro for detecting simple character or heterogeneous array type
#define IsSimpleCH(a)  (a EQ ARRAY_CHAR || a EQ ARRAY_HETERO)

// Define macro for detecting simple Boolean value
#define IsBooleanValue(a) (a EQ 0 || a EQ 1)


// Define APA structure
typedef struct tagAPLAPA	// Offset + Multiplier {times} {iota} Length (origin-0)
{
	APLINT	Off,		// Offset
			Mul;		// Multiplier
	APLUINT Len;		// Length
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
distinguished by their low-order two bits.	Because we know that
LPSYMENTRY and HGLOBAL are pointers and that these pointers are
aligned on a dword boundary, their two low-order bits are zero,
and thus available.


Array Types
-----------

ARRAY_BOOL		One value per bit, stored sequentially with
				the 1st value in bit position 0.

ARRAY_INT		One value per APLINT, stored sequentially.

ARRAY_FLOAT 	One value per APLFLOAT, stored sequentially.

ARRAY_CHAR		One value per APLCHAR, stored sequentially.

ARRAY_HETERO	One value per APLHETERO, stored sequentially.
				The array contents are all LPSYMENTRYs.

ARRAY_NESTED	One value per APLNESTED, stored sequentially.
				The array contents may be a mixture of LPSYMENTRYs and
				HGLOBALs (which point to one of the other Array Types),
				all of which are distinguished by the low-order two bits.

ARRAY_LIST		One value per APLLIST, stored sequentially.
				Values are either LPSYMENTRYs or HGLOBALs (which
				point to one of the other Array Types).

ARRAY_APA		An APA is a representation of a1 + a2 {times} {iota} a3
				in origin-0.

				These three elements are stored in the array in succession:

				1.	a1 = Offset
				2.	a2 = Multiplier
				3.	a3 = Length

 */

typedef struct tagHEADER_SIGNATURE
{
	UINT			 ature; 	// Array header signature (common to all types of arrays)
} HEADER_SIGNATURE, * LPHEADER_SIGNATURE;

// Variable array header
#define VARARRAY_HEADER_SIGNATURE	'SRAV'

typedef struct tagVARARRAY_HEADER
{
	HEADER_SIGNATURE Sign;		// Array header signature
	UINT			 ArrType:4, // The type of the array (see ARRAY_TYPES)
					 Perm:1,	// Permanent array (e.g., {zilde})
					 SysVar:1;	// Izit for a Sysvar (***DEBUG*** only)?
	UINT			 RefCnt;	// Reference count
	APLNELM 		 NELM;		// # elements in the array
	APLRANK 		 Rank;		// The rank of the array
								//	 followed by the dimensions
} VARARRAY_HEADER, *LPVARARRAY_HEADER;

// Macros to skip from the array base to either the dimensions or the data
#define VarArrayBaseToDim(lpMem)		  (LPAPLDIM) (((LPCHAR) lpMem) + sizeof (VARARRAY_HEADER)							 )
#define VarArrayBaseToData(lpMem,aplRank) (LPVOID)	 (((LPCHAR) lpMem) + sizeof (VARARRAY_HEADER) + sizeof (APLDIM) * aplRank)

// Function types
#define FCNTYPE_FIRST 1

typedef enum tagFCN_TYPES
{
	FCNTYPE_FCN = FCNTYPE_FIRST,// 1:  Primtive and user-defined functions
	FCNTYPE_OP1,				// 2:  ...						 monadic operators
	FCNTYPE_OP2,				// 3:  ...						 dyadic  ...
	FCNTYPE_AXISFCN,			// 4:  Primitive axis functions (is this useful?)
} FCN_TYPES;

// Function array header
#define FCNARRAY_HEADER_SIGNATURE	'SNCF'

typedef struct tagFCNARRAY_HEADER
{
	HEADER_SIGNATURE Sign;		// Array header signature
	UINT			 FcnType:4; // The type of the array (see FCN_TYPES)
	UINT			 RefCnt;	// Reference count
	APLNELM 		 NELM;		// # elements in the array
} FCNARRAY_HEADER, *LPFCNARRAY_HEADER;

// Macros to skip from the array base to the data
#define FcnArrayBaseToData(lpMem) (LPVOID)	 (((LPCHAR) lpMem) + sizeof (FCNARRAY_HEADER))

// Named strand header
#define VARNAMED_HEADER_SIGNATURE	'RTSN'

typedef struct tagVARNAMED_HEADER
{
	HEADER_SIGNATURE Sign;		// Array header signature
	APLNELM 		 NELM;		// # elements in the array
} VARNAMED_HEADER, *LPVARNAMED_HEADER;

// Macros to skip from the array base to the data
#define VarNamedBaseToData(lpMem) (LPVOID)	 (((LPCHAR) lpMem) + sizeof (VARNAMED_HEADER))

// Distinguish between immediate LPSYMENTRY and HGLOBAL in an array
typedef enum tagPTR_TYPES
{
 PTRTYPE_STCONST = 0,	// 0:  *MUST* be first (and thus zero) as we can't afford to clear all the time
 PTRTYPE_HGLOBAL,		// 1:  This ptr is an HGLOBAL
 PTRTYPE_UNUSED2,		// 2:  Unused and available
 PTRTYPE_UNUSED3,		// 3:  Unused and available
} PTR_TYPES;

#define PTRTYPE_MASK	 3		// This masks the two low-order bits

// Macros to clear the low-order bits of either an LPSYMENTRY,
//	 or HGLOBAL (luckily, both types of ptrs are the same size).
// These macros come in either direct (Dir) or indirect (Ind) form
#define ClrPtrTypeDir(lpMem)					 ((~PTRTYPE_MASK) &  (UINT)   lpMem)
#define ClrPtrTypeDirSym(lpMem) 	(LPSYMENTRY) ((~PTRTYPE_MASK) &  (UINT)   lpMem)
#define ClrPtrTypeDirGlb(lpMem) 	(HGLOBAL)	 ((~PTRTYPE_MASK) &  (UINT)   lpMem)
#define ClrPtrTypeDirFcn(lpMem) 	(LPPRIMFNS)  ((~PTRTYPE_MASK) &  (UINT)   lpMem)
#define ClrPtrTypeInd(lpMem)					 ((~PTRTYPE_MASK) & *(UINT *) lpMem)
#define ClrPtrTypeIndSym(lpMem) 	(LPSYMENTRY) ((~PTRTYPE_MASK) & *(UINT *) lpMem)
#define ClrPtrTypeIndGlb(lpMem) 	(HGLOBAL)	 ((~PTRTYPE_MASK) & *(UINT *) lpMem)

// Macro to extract the low-order bits of a memory ptr used
//	 to distinguish between the various pointer types.
#define GetPtrTypeInd(lpMem)	(  PTRTYPE_MASK  & *(UINT *) lpMem)
#define GetPtrTypeDir(lpMem)	(  PTRTYPE_MASK  &	(UINT)	 lpMem)

// Macro to create a masked LPSYMENTRY
#define MakeSymType(lpMem)		(PTRTYPE_STCONST | (UINT) lpMem)
#define MakeSymTypeSym(lpMem)	((LPSYMENTRY) MakeSymType (lpMem))

// Macro to create a masked HGLOBAL
#define MakeGlbType(lpMem)		(PTRTYPE_HGLOBAL | (UINT) lpMem)
#define MakeGlbTypeGlb(lpMem)	((HGLOBAL) MakeGlbType (lpMem))

// For LPSYMENTRY and HGLOBAL values in a temporary array, sometimes
//	 those values can be re-used in another array without having
//	 to make a copy.  In that case, the original value is replaced
//	 by this which is checked for before trying to free it.
#define PTR_REUSED	((LPVOID) 0xFFFFFFF3)

#define PtrReusedDir(lpMem) 				 (lpMem  EQ PTR_REUSED)
#define PtrReusedInd(lpMem) 	((*(LPVOID *) lpMem) EQ PTR_REUSED)


//***************************************************************************
//	End of File: datatype.h
//***************************************************************************
