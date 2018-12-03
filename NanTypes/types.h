//***************************************************************************
//  NARS2000 -- Types
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2018 Sudley Place Software

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

#ifndef NO_CHO_TYPES
#include "mpci.h"
#include "mphi.h"
#include "mpoi.h"
#include "mpcf.h"
#include "mphf.h"
#include "mpof.h"
#include "fpci.h"
#include "fphi.h"
#include "fpoi.h"
#include "fpcf.h"
#include "fphf.h"
#include "fpof.h"
#include "arb.h"
#include "acb.h"
#endif

typedef unsigned int   UBOOL, *LPUBOOL;
typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;

typedef UINT8  *LPUINT8 ;
typedef UINT16 *LPUINT16;
typedef UINT32 *LPUINT32;
typedef UINT64 *LPUINT64;

typedef  INT8  *LPINT8 ;
typedef  INT16 *LPINT16;
typedef  INT32 *LPINT32;
typedef  INT64 *LPINT64;

typedef unsigned __int8  uint8;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;

typedef ULONGLONG   APLRANK;            // The type of the rank element in an array
typedef  LONGLONG   APLRANKSIGN;        // ...             rank element ... (signed)
typedef ULONGLONG   APLNELM;            // ...             # elements   ...
typedef  LONGLONG   APLNELMSIGN;        // ...             # elements   ... (signed)
typedef ULONGLONG   APLDIM;             // ...             dimensions   ...
typedef UCHAR       APLBOOL;            // ...         an element in a Boolean array (unsigned)
typedef  LONGLONG   APLINT;             // ...         ...           an integer array
typedef ULONGLONG   APLUINT;            // ...         ...           an integer array (unsigned)
typedef WCHAR       APLCHAR;            // ...         ...           a character array
typedef float       APLFLT32;           // ...         ...           a Floating Point array
typedef double      APLFLT64;           // ...         ...           a Floating Point array
typedef double      APLFLOAT;           // ...         ...           a Floating Point array
typedef HGLOBAL     APLNESTED;          // ...         ...           a nested array
typedef struct tagSYMENTRY *APLHETERO;  // ...         ...           a HETERO array
typedef struct tagTOKEN APLLIST;        // ...                       a list
typedef MP_INT      APLMPI;             // ...         ...           an Arbitrary Precision Integer array
typedef MP_RAT      APLRAT;             // ...         ...           a Rational Number array
typedef __mpfr_struct APLVFP;           // ...         ...           a Variable-precision Floating Point array
typedef enum tagARRAY_TYPES APLSTYPE;   // Storage type (see ARRAY_TYPES)

#ifndef NO_CHO_TYPES
typedef __fpci_struct        APLHC2I;
typedef __fpci_struct*     LPAPLHC2I;

typedef __fphi_struct        APLHC4I;
typedef __fphi_struct*     LPAPLHC4I;

typedef __fpoi_struct        APLHC8I;
typedef __fpoi_struct*     LPAPLHC8I;

typedef __fpcf_struct        APLHC2F;
typedef __fpcf_struct*     LPAPLHC2F;

typedef __fphf_struct        APLHC4F;
typedef __fphf_struct*     LPAPLHC4F;

typedef __fpof_struct        APLHC8F;
typedef __fpof_struct*     LPAPLHC8F;

typedef __mpci_struct        APLHC2R;
typedef __mpci_struct*     LPAPLHC2R;

typedef __mphi_struct        APLHC4R;
typedef __mphi_struct*     LPAPLHC4R;

typedef __mpoi_struct        APLHC8R;
typedef __mpoi_struct*     LPAPLHC8R;

typedef __mpcf_struct        APLHC2V;
typedef __mpcf_struct*     LPAPLHC2V;

typedef __mphf_struct        APLHC4V;
typedef __mphf_struct*     LPAPLHC4V;

typedef __mpof_struct        APLHC8V;
typedef __mpof_struct*     LPAPLHC8V;
#endif

typedef ULONGLONG   APLB64;             // Boolean 64 bits
typedef uint32_t    APLB32;             // ...     32 ...
typedef uint16_t    APLB16;             // ...     16 ...
typedef uint8_t     APLB08;             // ...      8 ...

#if defined (_WIN64)
  typedef          __int64 APLI3264;    // Widest native signed value
  typedef unsigned __int64 APLU3264;    // ...           unsigned ...
#elif defined (_WIN32)
  typedef          __int32 APLI3264;    // Widest native signed value
  typedef unsigned __int32 APLU3264;    // ...           unsigned ...
#else
  #error Need code for this architecture.
#endif

// APLNESTED and APLHETERO may be either an LPSYMENTRY or
//   an HGLOBAL.

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
typedef APLFLT32  * LPAPLFLT32;
typedef APLFLT64  * LPAPLFLT64;
typedef APLHETERO * LPAPLHETERO;
typedef APLMPI    * LPAPLMPI;
typedef APLRAT    * LPAPLRAT;
typedef APLVFP    * LPAPLVFP;

typedef APLB64    * LPAPLB64;
typedef APLB32    * LPAPLB32;
typedef APLB16    * LPAPLB16;
typedef APLB08    * LPAPLB08;

typedef APLLONGEST* LPAPLLONGEST;

#define   APLHC1I     APLINT
#define   APLHC1F     APLFLOAT
#define   APLHC1R     APLRAT
#define   APLHC1V     APLVFP
#define LPAPLHC1I   LPAPLINT
#define LPAPLHC1F   LPAPLFLOAT
#define LPAPLHC1R   LPAPLRAT
#define LPAPLHC1V   LPAPLVFP

#pragma pack(push,4)
typedef struct tagAPLI_SPLIT
{
    UINT lo;
     int hi;
} APLI_SPLIT, *LPAPLI_SPLIT;

typedef struct tagAPLU_SPLIT
{
    UINT lo,
         hi;
} APLU_SPLIT, *LPAPLU_SPLIT;

typedef union tagISPLIT
{
    APLINT  aplInt;
    struct {UINT lo;
             int hi;};
} ISPLIT, *LPISPLIT;

typedef union tagUSPLIT
{
    APLUINT aplInt;
    struct {UINT lo;
            UINT hi;};
} USPLIT, *LPUSPLIT;
#pragma pack(pop)

// This typedef is used for common routines to convert HCxy to an APLINT
typedef APLINT (*HCxyTOAPLINT) (LPVOID, APLUINT, LPUBOOL);

// This typedef and struc are used for common routine in <pf_lshoe.c>
typedef void (*LFTSHOEGLBCOPY) (LPVOID, APLNELM, LPVOID, APLNELM);

typedef struct tagLFTSHOEGLBCOMVARS
{
    LPUBOOL          lpbCtrlBreak;       // Ptr to Ctrl-Break flag
    APLUINT          ByteRes;           // # bytes in the result
    APLSTYPE         aplTypeRht;        // Right arg storage type
    APLNELM          aplNELMSub;        // Subarray NELM
    APLNELM          aplNELMAxis;       // Axis NELM
    APLRANK          aplRankRes;        // Result rank
    APLRANK          aplRankRht;        // Right arg rank
    LPVOID          *lplpMemRes;        // Ptr to ptr to result global memory
    LPAPLDIM         lpMemDimRht;       // Ptr to right arg dimensions
    LPAPLUINT        lpMemAxisHead;     // Ptr to axis values, fleshed out by CheckAxis_EM
    LPAPLUINT        lpMemAxisTail;     // Ptr to grade up of AxisHead
    HGLOBAL         *lphGlbSub;         // Ptr to subarray global memory handle
    LPVOID          *lplpMemRht;        // Ptr to ptr to right arg global memory
    LPVOID          *lplpMemSub;        // Ptr to ptr to subarray global memory
    struct tagTOKEN *lptkFunc;          // Ptr to function token
    LPAPLUINT        lpMemOdo;          // Ptr to odometer vector
    LPAPLUINT        lpMemWVec;         // Ptr to weighting vector
    LFTSHOEGLBCOPY   LftShoeGlbCopy;    // Ptr to data copy routine
} LFTSHOEGLBCOMVARS, *LPLFTSHOEGLBCOMVARS;

// These typedefs are used for PRIMSPEC items
typedef void     ATISAT    (LPVOID, APLUINT, struct tagALLTYPES *,                       struct tagPRIMSPEC *);
typedef          ATISAT    *LPATISAT;
typedef void     ATISATVAT (LPVOID, APLUINT, struct tagALLTYPES *, struct tagALLTYPES *, struct tagPRIMSPEC *);
typedef          ATISATVAT *LPATISATVAT;

#ifndef NO_CHO_TYPES

// These #defines are used for Ball Arithmetic
typedef arb_struct    APLARB ,    APLBA1F,
                   *LPAPLARB , *LPAPLBA1F;

typedef union tagBA2F       // Ball Arithmetic Real Complex
{
    arb_struct parts[2];
    struct tagBA2FPARTS
    {
        arb_struct partsLo,
                   partsHi;
    };
} __ba2f_struct, ba2f_str;

typedef __ba2f_struct        APLBA2F;
typedef __ba2f_struct*     LPAPLBA2F;

typedef union tagBA4F       // Ball Arithmetic Real Quaternion
{
    arb_struct parts[4];
    struct tagBA4FPARTS
    {
        __ba2f_struct partsLo,
                      partsHi;
    };
} __ba4f_struct, ba4f_str;

typedef __ba4f_struct        APLBA4F;
typedef __ba4f_struct*     LPAPLBA4F;

typedef union tagBA8F       // Ball Arithmetic Real Octonion
{
    arb_struct parts[8];
    struct tagBA8FPARTS
    {
        __ba4f_struct partsLo,
                      partsHi;
    };
} __ba8f_struct, ba8f_str;

typedef __ba8f_struct        APLBA8F;
typedef __ba8f_struct*     LPAPLBA8F;
#endif


typedef UBOOL (*LEFTCARETUND_EP2MR_INNER)
    (APLSTYPE,
     APLUINT,
     APLUINT,
     APLUINT,
     LPVOID,
     struct tagARB_STRUCT *);


typedef void (*RIGHTCARETUND_MR2EP_INNER)
    (APLSTYPE,
     APLUINT,
     APLUINT,
     APLUINT,
     LPVOID,
     LPVOID);


//***************************************************************************
//  End of File: types.h
//***************************************************************************