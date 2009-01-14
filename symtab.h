//***************************************************************************
//  NARS2000 -- Symbol Table
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
#define DEF_HSHTAB_NBLKS       4096
#if (DEF_HSHTAB_NBLKS & (DEF_HSHTAB_NBLKS - 1)) != 0
  #error DEF_HSHTAB_NBLKS is not a power of two.
#endif

// # entries in each block -- can be any integer > 1
// It can't be 1 as there needs to be at least one
//   overflow entry in each block so we never assign
//   a value to the PrinHash entry which doesn't have
//   the same hash value.
#define DEF_HSHTAB_EPB         8

// Maximum hash table size (# entries)
#define DEF_HSHTAB_MAXSIZE  (  64 * 1024 * DEF_HSHTAB_EPB)

// Starting hash table size (# entries)
#define DEF_HSHTAB_INITSIZE (DEF_HSHTAB_NBLKS * DEF_HSHTAB_EPB)

// Amount to resize -- this value must be a divisor of DEF_HSHTAB_INITSIZE
#define DEF_HSHTAB_INCRSIZE DEF_HSHTAB_INITSIZE

#if ((DEF_HSHTAB_INITSIZE / DEF_HSHTAB_INCRSIZE) * DEF_HSHTAB_INCRSIZE) != \
      DEF_HSHTAB_INITSIZE
  #error DEF_HSHTAB_INCRSIZE not an integral divisor of DEF_HSHTAB_INITSIZE.
#endif

// Starting hash mask
#define DEF_HSHTAB_HASHMASK (DEF_HSHTAB_NBLKS - 1)

// The increment used when looking for the next available HSHENTRY
// This number must be relatively prime to each of
//   DEF_HSHTAB_INITSIZE + DEF_HSHTAB_INCRSIZE * {iota} DEF_HSHTAB_LARGEST / DEF_HSHTAB_INCRSIZE
// An easy way to make this happen is to use a prime guaranteed to be
//   larger than the largest symbol table size, and then use as an increment
//   DEF_HSHTAB_PRIME % iHshTabTotalSize.
#define DEF_HSHTAB_PRIME    ((UINT) 0x7FFFFFFF)  // (2^31 - 1)
#define DEF_HSHTAB_INCR     (DEF_HSHTAB_PRIME % DEF_HSHTAB_INITSIZE)


// Hash table flags
typedef struct tagHTFLAGS
{
    UINT Inuse:1,           // 00000001:  Inuse entry
         PrinHash:1,        // 00000002:  Entry with principal hash
         CharIsValid:1,     // 00000004:  htChar is valid (and lpCharName is a
                            //            ptr to the name)
#ifdef DEBUG
         Temp:1,            // 00000008:  Temporary flag used for debugging
         Avail:11,          // 0000FFF0:  Available bits
#else
         Avail:12,          // 0000FFF8:  Available bits
#endif
         htChar:16;         // FFFF0000:  Char if CharIsValid is TRUE
} HTFLAGS, *LPHTFLAGS;

// N.B.:  Whenever changing the above struct (HTFLAGS),
//   be sure to make a corresponding change to
//   <ahtFlagNames> in <display.c>.


#ifdef DEBUG
typedef struct tagAPLCHAR6
{
    APLCHAR aplChar[6];
} APLCHAR6, *LPAPLCHAR6;
#endif


// Hash table entry
typedef struct tagHSHENTRY
{
    struct tagHSHENTRY
            *NextSameHash,  // 00:  Next entry with same hash
            *PrevSameHash;  // 04:  Prev ...
    HTFLAGS htFlags;        // 08:  Flags
    UINT    uHash,          // 0C:  The hash value for this entry
            uHashAndMask;   // 10:  uHash & the current mask
    union                   // 14:  Handle of the entry's name (NULL if none)
    {
        HGLOBAL htGlbName;          // Use only if CharIsValid EQ FALSE
        LPWCHAR lpwCharName;        // Use only if CharIsValid EQ TRUE
#ifdef DEBUG
        LPAPLCHAR6 *lplpaplChar6;   // For debugging only
#endif
    };
    struct tagSYMENTRY
            *htSymEntry;    // 18:  Ptr to the matching SYMENTRY
                            // 1C:  Length
} HSHENTRY, *LPHSHENTRY;

#define LPHSHENTRY_NONE     ((LPHSHENTRY) -1)

//********************* SYMBOL TABLE ****************************************

// Maximum symbol table size (# entries)
#define DEF_SYMTAB_MAXSIZE  (  64*1024)

// Starting symbol table size (# entries)
#define DEF_SYMTAB_INITSIZE (   4*1024)

// Amount to resize
#define DEF_SYMTAB_INCRSIZE DEF_SYMTAB_INITSIZE

typedef enum tagIMM_TYPES
{
    IMMTYPE_ERROR = 0,      // 00:  Error (not an immediate type)
    IMMTYPE_BOOL,           // 01:  Boolean
    IMMTYPE_INT,            // 02:  Integer
    IMMTYPE_FLOAT,          // 03:  Floating point
    IMMTYPE_CHAR,           // 04:  Character
    IMMTYPE_PRIMFCN,        // 05:  Primitive monadic/dyadic function
    IMMTYPE_PRIMOP1,        // 06:  Primitive monadic operator
    IMMTYPE_PRIMOP2,        // 07:  ...       dyadic  ...
    IMMTYPE_PRIMOP3,        // 08:  ...       ambiguous ...
                            // 09-0F:  Available entries (4 bits)
} IMM_TYPES, *LPIMM_TYPES;

// N.B.:  Whenever changing the above enum (IMM_TYPES),
//   be sure to make a corresponding change to
//   <TranslateImmTypeToChar> in <translate.c>,
//   the macros <IsImmXXX> in <macros.h>, the
//   #define ImmTypeAsChar just below here.

// Translate an immediate type to a char
// Note that the order of the chars in this #define
//   depends upon the ordering of the above enum
#define ImmTypeAsChar   L"!BIFC?123"

// Name types
typedef enum tagNAME_TYPES
{
    NAMETYPE_UNK = 0,       // 00:  Name is unknown
    NAMETYPE_VAR,           // 01:  ...     a variable
    NAMETYPE_FN0,           // 02:  ...       niladic function
    NAMETYPE_FN12,          // 03:  ...       monadic/dyadic/ambivalent function
    NAMETYPE_OP1,           // 04:  ...       monadic operator
    NAMETYPE_OP2,           // 05:  ...       dyadic operator
    NAMETYPE_OP3,           // 06:  ...       ambiguous function/operator
    NAMETYPE_FILL1,         // 07:  ...       filler
    NAMETYPE_LST,           // 08:  ...       list
                            // 09-0F:  Available entries (4 bits)
} NAME_TYPES;

// N.B.:  Whenever changing the above enum (NAME_TYPES),
//   be sure to make a corresponding change to
//   NAMETYPEMASK_FN and NAMETYPEMASK_OP, and
//   the macros <IsNameTypeXXX> in <macros.h>.

#define NAMETYPE_STRING     "?VNF123?L"
//                              0       1       2       3       4       5       6       7       8
#define NAMETYPE_STRPTR     { "Unk",  "Var",  "Nil",  "Fcn",  "Op1",  "Op2",  "Op3",  "???",  "Lst"}
#define NAMETYPE_WSTRPTR    {L"Unk", L"Var", L"Nil", L"Fcn", L"Op1", L"Op2", L"Op3", L"???", L"Lst"}

// The above enum is constructed so as to allow the following masks to be used:
#define NAMETYPEMASK_FN     0x02    // Name is a function
#define NAMETYPEMASK_OP     0x04    // Name is an operator

typedef enum tagOBJ_NAMES
{
    OBJNAME_NONE = 0,       // 00:  Unnamed
    OBJNAME_USR,            // 01:  User name
    OBJNAME_SYS,            // 02:  System name (starts with a Quad or Quote-quad)
    OBJNAME_MF,             // 03:  Magic Function
    OBJNAME_LOD,            // 04:  )LOAD and )COPY HGLOBAL
                            // 05-07:  Available entries (3 bits)
} OBJ_NAMES;

#define OBJNAME_WSTRPTR     {L"None", L"USR", L"SYS", L"MF", L"LOD"}

// Symbol table flags
typedef struct tagSTFLAGS
{
    UINT Imm:1,             // 00000001:  The data in .stData is Immediate simple numeric or character scalar
         ImmType:4,         // 0000001E:  ...                    Immediate Boolean, Integer, Character, or Float (see IMM_TYPES)
         Perm:1,            // 00000020:  Permanent entry
         Inuse:1,           // 00000040:  Inuse entry
         Value:1,           // 00000080:  Entry has a value
         ObjName:3,         // 00000700:  The data in .stData is NULL if .stNameType is NAMETYPE_UNK; value, address, or HGLOBAL otherwise
                            //            (see OBJ_NAMES)
         stNameType:4,      // 00007800:  The data in .stdata is value (if .Imm), address (if .FcnDir), or HGLOBAL (otherwise)
                            //            (see NAME_TYPES)
         SysVarValid:5,     // 000F8000:  Index to validation routine for System Vars (see SYS_VARS)
         UsrDfn:1,          // 00100000:  User-defined function/operator
         DfnLabel:1,        // 00200000:  User-defined function/operator label        (valid only if .Value is set)
         DfnSysLabel:1,     // 00400000:  User-defined function/operator system label (valid only if .Value is set)
         DfnAxis:1,         // 00800000:  User-defined function/operator accepts axis value
         FcnDir:1,          // 01000000:  Direct function/operator               (stNameFcn is valid)
         Avail:7;           // FE000000:  Available bits
} STFLAGS, *LPSTFLAGS;

// N.B.:  Whenever changing the above struct (STFLAGS),
//   be sure to make a corresponding change to
//   <astFlagNames> in <dispdbg.c>.

// .Inuse and .PrinHash are valid for all entries.
// .Inuse   = 0 implies that all but .PrinHash are zero.
// .Imm     implies one and only one of the IMMTYPE_***s
// .Imm     = 1 implies that one and only one of aplBoolean, aplInteger, aplChar, or aplFloat is valid.
// .Imm     = 0 implies that stGlbData is valid.
// .Perm    is valid for OBJNAME_SYS and OBJNAME_MF only.
// .Value   is valid for NAMETYPE_VAR only, however .stNameType EQ NAMETYPE_VAR
//          should never be without a value.
// .UsrDfn  is set for .UsrType when the function is user-defined.
// .FcnDir  may be set for any function/operator in .SysType or .UsrType; it is a
//          direct pointer to the code.
// htGlbName in HSHENTRY is set for .UsrType and .SysType when .Imm and .FcnDir are clear.

// Immediate data or a handle to global data
typedef union tagSYMTAB_DATA
{
    APLBOOL    stBoolean;       // 00:  A number (Boolean)
    APLINT     stInteger;       // 00:  A number (Integer)
    APLFLOAT   stFloat;         // 00:  A floating point number
    APLCHAR    stChar;          // 00:  A character
    HGLOBAL    stGlbData;       // 00:  Handle of the entry's data
    LPVOID     stVoid;          // 00:  An abritrary ptr
    LPPRIMFNS  stNameFcn;       // 00:  Ptr to a named function
    APLLONGEST stLongest;       // 00:  Longest datatype (so we can copy the entire data)
                                // 08:  Length
} SYMTAB_DATA, *LPSYMTAB_DATA;

// Symbol table entry
typedef struct tagSYMENTRY
{
    STFLAGS     stFlags;        // 00:  Flags
    SYMTAB_DATA stData;         // 04:  For immediates, the data value;
                                //        for others, the HGLOBAL (8 bytes)
    LPHSHENTRY  stHshEntry;     // 0C:  Ptr to the matching HSHENTRY
    struct tagSYMENTRY
               *stPrvEntry,     // 10:  Ptr to previous (shadowed) STE (NULL = none)
               *stSymLink;      // 14:  Ptr to next entry in linked list of
                                //        similarly grouped entries (NULL = none)
    UINT        stSILevel;      // 18:  State Indicator Level for this STE
                                // 1C:  Length
} SYMENTRY, *LPSYMENTRY;

#define LPSYMENTRY_NONE     ((LPSYMENTRY) -1)


//***************************************************************************
//  End of File: symtab.h
//***************************************************************************
