//***************************************************************************
//  NARS2000 -- Symbol Table
//***************************************************************************

/*

Hash Table

The hash table consists of a series of HSHENTRY entries
separated into initially DEF_HSHTAB_NBLKS number of blocks
each of DEF_HSHTAB_EPB entries per block.


Symbol Table

The symbol table consists of a series of SYMENTRY entries.

References:

Witold Litwin, Linear hashing: A new tool for file and table addressing,
Proc. 6th Conference on Very Large Databases, pages 212-223, 1980.

Per-Åke Larson, Dynamic Hash Tables, CACM 31(4):446-457, April 1988.

http://swig.stanford.edu/pub/summaries/database/linhash.html
http://www.cs.wisc.edu/~dbbook/openAccess/thirdEdition/slides/slides3ed-english/Ch11_Hash_Index.pdf#search=%22%22linear%20hashing%22%22
http://portal.acm.org/citation.cfm?id=3324


 */

//********************* HASH TABLE ******************************************

// Starting # blocks in hash table -- must be a power of two
//   so the hash mask is a contiguous series of 1s
//   (one less than a power of two).
#define DEF_HSHTAB_NBLKS       4    // 1024
#if (DEF_HSHTAB_NBLKS & (DEF_HSHTAB_NBLKS - 1)) != 0
#error DEF_HSHTAB_NBLKS is not a power of two.
#endif

// # entries in each block -- can be any integer > 1
// It can't be 1 as there needs to be at least one
//   overflow entry in each block so we never assign
//   a value to the PrinHash entry which doesn't have
//   the same hash value.
#define DEF_HSHTAB_EPB         2    //    8

// Maximum hash table size (# entries)
#define DEF_HSHTAB_MAXSIZE  (1024 * 1024 * DEF_HSHTAB_EPB)

// Starting hash table size (# entries)
#define DEF_HSHTAB_INITSIZE (DEF_HSHTAB_NBLKS * DEF_HSHTAB_EPB)

// Amount to resize -- this value must be a divisor of DEF_HSHTAB_INITSIZE
////#define DEF_HSHTAB_RESIZE   (DEF_HSHTAB_INITSIZE / 2)
#define DEF_HSHTAB_RESIZE   DEF_HSHTAB_INITSIZE

#if ((DEF_HSHTAB_INITSIZE / DEF_HSHTAB_RESIZE) * DEF_HSHTAB_RESIZE) != \
    DEF_HSHTAB_INITSIZE
#error DEF_HSHTAB_RESIZE not an integral divisor of DEF_HSHTAB_INITSIZE.
#endif

// Starting hash mask
#define DEF_HSHTAB_HASHMASK (DEF_HSHTAB_NBLKS - 1)

// The increment used when looking for the next available HSHENTRY
// This number must be relatively prime to each of
//   DEF_HSHTAB_INITSIZE + DEF_HSHTAB_RESIZE * {iota} DEF_HSHTAB_LARGEST / DEF_HSHTAB_RESIZE
// An easy way to make this happen is to use a prime guaranteed to be
//   larger than the largest symbol table size, and then use as an increment
//   DEF_HSHTAB_PRIME % iHshTabTotalSize.
#define DEF_HSHTAB_PRIME    ((UINT) 0x7FFFFFFF)  // (2^31 - 1)
#define DEF_HSHTAB_INCR     (DEF_HSHTAB_PRIME % DEF_HSHTAB_INITSIZE)


// Hash table flags
typedef struct tagHTFLAGS
{
    UINT Inuse:1,           // 1:  Inuse entry
         PrinHash:1,        // 2:  Entry with principal hash
         SymCopy:1          // 4:  Symbol table entry copy
#ifdef DEBUG
        ,Temp:1             // 8:  Temporary flag used for debugging
#endif
         ;
} HTFLAGS, *LPHTFLAGS;

// Hash table entry
typedef struct tagHSHENTRY
{
    struct tagHSHENTRY
            *NextSameHash,  // Next entry with same hash
            *PrevSameHash;  // Prev ...
    HTFLAGS htFlags;        // Flags
    UINT    uHash,          // The hash value for this entry
            uHashAndMask;   // uHash & the current mask
    HGLOBAL hGlbName;       // Handle of the entry's name (NULL if none)
    struct tagSYMENTRY
            *lpSymEntry;    // Ptr to the matching SYMENTRY
} HSHENTRY, *LPHSHENTRY;

#define LPHSHENTRY_NONE     ((LPHSHENTRY) -1)

//********************* SYMBOL TABLE ****************************************

// Maximum symbol table size (# entries)
#define DEF_SYMTAB_MAXSIZE  (1024 * 1024)

// Starting symbol table size (# entries)
#define DEF_SYMTAB_INITSIZE (4 * 1024)

// Amount to resize
#define DEF_SYMTAB_RESIZE   DEF_SYMTAB_INITSIZE

typedef enum tagIMM_TYPES
{
    IMMTYPE_BOOL = 0,       // 0:
    IMMTYPE_INT,            // 1:
    IMMTYPE_FLOAT,          // 2:
    IMMTYPE_CHAR,           // 3:
    IMMTYPE_PRIMFCN,        // 4:
    IMMTYPE_PRIMOP1,        // 5:
    IMMTYPE_PRIMOP2,        // 6:
} IMM_TYPES;

// Symbol table flags
typedef struct tagSTFLAGS
{
    UINT Imm:1,             // 000001:  The data in SYMENTRY is Immediate numeric or character scalar
         ImmType:4,         // 00001E:  ...                     Immediate Boolean, Integer, Character, or Float (see IMM_TYPES)
         SysName:1,         // 000020:  ...                     NULL unless some other .Usr*** is set
         SysVar:1,          // 000040:  ...                     ...     System Variable
         SysFn0:1,          // 000080:  ...                     ...     System Function, niladic
         SysFn12:1,         // 000100:  ...                     ...     System Function, monadic or dyadic
         NotCase:1,         // 000200:  Case-insensitive name
         Perm:1,            // 000400:  Permanent entry
         Inuse:1,           // 000800:  Inuse entry
         Value:1,           // 001000:  Entry has a value
         UsrName:1,         // 002000:  The data in SYMENTRY is NULL unless some other .Usr*** is set
         UsrVar:1,          // 004000:  ...                     hGlb of Value, unless .Imm
         UsrFn0:1,          // 008000:  User-defined niladic function
         UsrFn12:1,         // 010000:  ...          monadic/dyadic function
         UsrOp1:1,          // 020000:  ...          monadic operator
         UsrOp2:1,          // 040000:  ...          dyadic operator
         SysVarValid:4;     // 780000:  Index to validation routine for System Vars
} STFLAGS, *LPSTFLAGS;

// When changing this struct, be sure to make
//   corresponding changes to <astFlagNames> in <sessman.c>,
//   and <TokenTypeFV> in <parse.y>.

// .Inuse and .PrinHash are valid for all entries.
// .Inuse   = 0 implies that all but .PrinHash are zero.
// .Imm     implies one and only one of the IMMTYPE_***s
// .Imm     = 1 implies that one and only one of aplBoolean, aplInteger, aplChar, or aplFloat is valid.
// .Imm     = 0 implies that stGlbData is valid.
// .Perm    is valid for .SysVar, .SysFn0, and .SysFn12 only.
// .NotCase is valid for .SysVar, .SysFn0, and .SysFn12 only.
// .Value   is valid for .SysVar and .UsrVar only, however
//          .SysVar should never be without a value.
// .SysName implies one and only one of .SysVar, .SysFn0, .SysFn12 is set.
// .UsrName is set for .UsrVar, .UsrFn0, .UsrFn12, .UsrOp1, .UsrOp2.
//          It is also set for a variable with no value.
// .UsrVar, .SysVar, .SysFn0, .SysFn12, .UsrFn0, .UsrFn12, .UsrOp1, and .UsrOp2
//          are mutually exclusive.
// hGlbName in SYMENTRY is set for .UsrVar, .UsrFn0, .UsrFn12, .UsrOp1, .UsrName,
//                                 .SysVar, .SysFn0, .SysFn12, .UsrOp2.
// .SysFn0 and .SysFn12 are both direct pointers to the code.  All other functions
//          are indirect (HGLOBAL) pointers.

typedef union tagSYMTAB_DATA    // Immediate data or a handle to global data
{
    APLBOOL    stBoolean;       // A number (Boolean)
    APLINT     stInteger;       // A number (Integer)
    APLFLOAT   stFloat;         // A floating point number
    APLCHAR    stChar;          // A character
    HGLOBAL    stGlbData;       // Handle of the entry's data
    LPVOID     lpVoid;          // An abritrary ptr
    LPPRIMFNS  stNameFcn;       // Ptr to a named function
    APLLONGEST stLongest;       // Longest datatype (so we can copy the entire data)
} SYMTAB_DATA, *LPSYMTAB_DATA;

// Symbol table entry
typedef struct tagSYMENTRY
{
    STFLAGS     stFlags;        // Flags
    SYMTAB_DATA stData;         // For immediates, the data value; for others, the HGLOBAL
    LPHSHENTRY  lpHshEntry;     // Ptr to the matching HSHENTRY
} SYMENTRY, *LPSYMENTRY;

#define LPSYMENTRY_NONE     ((LPSYMENTRY) -1)

/*

Reference Counts -- STEs & Arrays
---------------------------------

In a named array (TKT_STNAME) only, the array reference
count is used.

A named array is referenced (and its count incremented) when it is

* assigned to a user (not system) named variable

A STE or an array is dereferenced (and its count decremented) when

* the named variable to which it is assigned is erased or reassigned, or

When a named array is dereferenced and the reference count goes to zero,
the array may be freed.

 */

//***************************************************************************
//  End of File: symtab.h
//***************************************************************************
