//****************************************************************************
//  NARS2000 -- Macros File
//****************************************************************************

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


#define MB(a)                   MessageBox  (NULL, (a),    APPNAME, MB_OK)
#define MBW(a)                  MessageBoxW (NULL, (a), WS_APPNAME, MB_OK)
#define MBC(a)              if (MessageBox  (NULL, (a),    APPNAME, MB_OKCANCEL) EQ IDCANCEL) DbgBrk ()
#define MBWC(a)             if (MessageBoxW (NULL, (a), WS_APPNAME, MB_OKCANCEL) EQ IDCANCEL) DbgBrk ()
#define IsGlbTypeVarDir(a)  (IsGlobalTypeArray (            (a), VARARRAY_HEADER_SIGNATURE))
#define IsGlbTypeVarInd(a)  (IsGlobalTypeArray (*(LPVOID *) (a), VARARRAY_HEADER_SIGNATURE))
#define IsGlbTypeFcnDir(a)  (IsGlobalTypeArray (            (a), FCNARRAY_HEADER_SIGNATURE))
#define IsGlbTypeFcnInd(a)  (IsGlobalTypeArray (*(LPVOID *) (a), FCNARRAY_HEADER_SIGNATURE))
#define IsGlbTypeDfnDir(a)  (IsGlobalTypeArray (            (a), DFN_HEADER_SIGNATURE))
#define IsGlbTypeNamDir(a)  (IsGlobalTypeArray (            (a), VARNAMED_HEADER_SIGNATURE))
#define IsGlbTypeLstDir(a)  (IsGlobalTypeArray (            (a), LSTARRAY_HEADER_SIGNATURE))
#define IsSymNoValue(a)     ((a)->stHshEntry EQ NULL                \
                          && (a)->stFlags.Perm                      \
                          && (a)->stFlags.Value EQ 0                \
                          && (a)->stFlags.ObjName EQ OBJNAME_NONE   \
                          && (a)->stFlags.stNameType EQ NAMETYPE_UNK)
#define IsTokenNoValue(a)   ((a)                                    \
                          && (a)->tkFlags.TknType EQ TKT_VARNAMED   \
                          && IsSymNoValue ((a)->tkData.tkSym))
#define IsSysName(a)        ((a)[0] EQ UTF16_QUAD  || (a)[0] EQ UTF16_QUOTEQUAD)
#define IsDirectName(a)     ((a)    EQ UTF16_ALPHA || (a)    EQ UTF16_OMEGA)

#define ByteAddr(a,b)       (&(((LPBYTE) (a))[b]))

#define AplModI(m,a) PrimFnDydStileIisIvI ((m), (a), NULL)
#define AplModF(m,a) PrimFnDydStileFisFvF ((m), (a), NULL)

#define LODWORD(x)          ( (DWORD) (   (x) & LOPART_DWORDLONG ) )
#define HIDWORD(x)          ( (DWORD) ( ( (x) & HIPART_DWORDLONG ) >> 32 ) )

#define LOSHORT(l)  ((short)((DWORD)(l) & 0xffff))
#define HISHORT(l)  ((short)((DWORD)(l) >> 16))

#define CheckSymEntries()   _CheckSymEntries (FNLN)

#ifdef DEBUG
  #define YYAlloc()     _YYAlloc(FNLN)

  #ifdef DEBUG_ALLOCFREE
    #define DbgGlobalAlloc(uFlags,ByteRes) \
    DbgGlobalAllocSub ((uFlags), (ByteRes), L"##GlobalAlloc in " APPEND_NAME L": %p (%S#%d)", FNLN)

    #define DbgGlobalFree(hGlbToken) \
    dprintfW (L"**GlobalFree  in " APPEND_NAME L": %p (%S#%d)", (hGlbToken), FNLN); \
    MyGlobalFree (hGlbToken)
  #else
    #define DbgGlobalAlloc(uFlags,ByteRes) \
    MyGlobalAlloc ((uFlags), (ByteRes))

    #define DbgGlobalFree(hGlbToken) \
    MyGlobalFree (hGlbToken)
  #endif

  #ifdef DEBUG_REFCNT
    #define DbgIncrRefCntDir(hGlbData) \
    dprintfW9 (L"##RefCnt++ in " APPEND_NAME L": %p (%S#%d)", ClrPtrTypeDir (hGlbData), FNLN); \
    IncrRefCntDir (hGlbData)

    #define DbgIncrRefCntInd(hGlbData) \
    dprintfW9 (L"##RefCnt++ in " APPEND_NAME L": %p (%S#%d)", ClrPtrTypeDir (hGlbData), FNLN); \
    IncrRefCntInd (hGlbData)

    #define DbgDecrRefCntDir(hGlbData) \
    dprintfW9 (L"##RefCnt-- in " APPEND_NAME L": %p (%S#%d)", ClrPtrTypeDir (hGlbData), FNLN); \
    DecrRefCntDir (hGlbData)

    #define DbgDecrRefCntInd(hGlbData) \
    dprintfW9 (L"##RefCnt-- in " APPEND_NAME L": %p (%S#%d)", ClrPtrTypeDir (hGlbData), FNLN); \
    DecrRefCntInd (hGlbData)
  #else
    #define DbgIncrRefCntDir(hGlbData) \
    IncrRefCntDir (hGlbData)

    #define DbgIncrRefCntInd(hGlbData) \
    IncrRefCntInd (hGlbData)

    #define DbgDecrRefCntDir(hGlbData) \
    DecrRefCntDir (hGlbData)

    #define DbgDecrRefCntInd(hGlbData) \
    DecrRefCntInd (hGlbData)
  #endif

  #define DbgMsgW2(a)                     {if (gDbgLvl > 2) {DbgMsgW(a);}}

  #define CheckMemStat()                  _CheckMemStat ()

  #define Assert(a)                       ((a) || (DbgBrk (), nop (), 0))
  #define CheckCtrlBreak(a)               _CheckCtrlBreak(a)
#else
  #define YYAlloc()     _YYAlloc()

  #define DbgGlobalAlloc(uFlags,ByteRes)  MyGlobalAlloc ((uFlags), (ByteRes))

  #define DbgGlobalFree(hGlbToken)        MyGlobalFree (hGlbToken)

  #define DbgIncrRefCntDir(hGlbData)      IncrRefCntDir (hGlbData)

  #define DbgIncrRefCntInd(hGlbData)      IncrRefCntInd (hGlbData)

  #define DbgDecrRefCntDir(hGlbData)      DecrRefCntDir (hGlbData)

  #define DbgDecrRefCntInd(hGlbData)      DecrRefCntInd (hGlbData)

  #define DbgMsg(a)
  #define DbgMsgW(a)
  #define DbgMsgW2(a)

  #define CheckMemStat()

  #define Assert(a)                       ((void) 0)
////  #define Assert(a) ((a) || (AssertPrint(#a, FNLN), 0))
  #define CheckCtrlBreak(a)               (a)
#endif

#define imul64(a,b)         _imul64 ((a), (b), NULL)
#define iadd64(a,b)         _iadd64 ((a), (b), NULL)
#define isub64(a,b)         _isub64 ((a), (b), NULL)

#define SIGN_APLNELM(a)     ((a) >> 63)     // Sign bit of an APLNELM
#define SIGN_APLRANK(a)     ((a) >> 63)     // ...            APLRANK
#define SIGN_APLDIM(a)      ((a) >> 63)     // ...            APLDIM
#define SIGN_APLINT(a)      (((APLUINT) (a)) >> 63) // ...      APLINT
#define SIGN_APLUINT(a)     ((a) >> 63)     // ...            APLUINT
#define SIGN_APLLONGEST(a)  ((a) >> 63)     // ...            APLLONGEST


// Define macro for detecting simple array type
/////// IsSimple(ArrType)               ((ArrType) EQ ARRAY_BOOL || (ArrType) EQ ARRAY_INT || (ArrType) EQ ARRAY_APA || (ArrType) EQ ARRAY_FLOAT || (ArrType) EQ ARRAY_CHAR || (ArrType) EQ ARRAY_HETERO)
#define IsSimple(ArrType)               (uTypeMap[ArrType] <= uTypeMap[ARRAY_HETERO])

// Define macro for detecting simple non-heterogeneous array type
/////// IsSimpleNH(ArrType)             ((ArrType) EQ ARRAY_BOOL || (ArrType) EQ ARRAY_INT || (ArrType) EQ ARRAY_APA || (ArrType) EQ ARRAY_FLOAT || (ArrType) EQ ARRAY_CHAR)
#define IsSimpleNH(ArrType)             (uTypeMap[ArrType] < uTypeMap[ARRAY_HETERO])

// Define macro for detecting simple numeric array type
/////// IsSimpleNum(ArrType)            ((ArrType) EQ ARRAY_BOOL || (ArrType) EQ ARRAY_INT || (ArrType) EQ ARRAY_APA || (ArrType) EQ ARRAY_FLOAT)
#define IsSimpleNum(ArrType)            (uTypeMap[ArrType] < uTypeMap[ARRAY_CHAR])

// Define macro for detecting simple Boolean array type
#define IsSimpleBool(ArrType)           ((ArrType) EQ ARRAY_BOOL)

// Define macro for detecting simple integer-like array type
#define IsSimpleInt(ArrType)            ((ArrType) EQ ARRAY_BOOL || (ArrType) EQ ARRAY_INT || (ArrType) EQ ARRAY_APA)

// Define macro for detecting simple APA array type
#define IsSimpleAPA(ArrType)            ((ArrType) EQ ARRAY_APA)

// Define macro for detecting simple float array type
#define IsSimpleFlt(ArrType)            ((ArrType) EQ ARRAY_FLOAT)

// Define macro for detecting simple character array type
#define IsSimpleChar(ArrType)           ((ArrType) EQ ARRAY_CHAR)

// Define macro for detecting simple character or heterogeneous array type
#define IsSimpleCH(ArrType)             ((ArrType) EQ ARRAY_CHAR || (ArrType) EQ ARRAY_HETERO)

// Define macro for detecting simple hetero array type
#define IsSimpleHet(ArrType)            ((ArrType) EQ ARRAY_HETERO)

// Define macro for detecting nested array type
#define IsNested(ArrType)               ((ArrType) EQ ARRAY_NESTED)

// Define macro for detecting list array type
#define IsList(ArrType)                 ((ArrType) EQ ARRAY_LIST)

// Define macro for detecting permuation vectors
#define IsPermVector(lpHeader)          (lpHeader->PV0 || lpHeader->PV1)

// Define macro for detecting scalars
#define IsScalar(ArrRank)               ((ArrRank) EQ 0)

// Define macro for detecting vectors
#define IsVector(ArrRank)               ((ArrRank) EQ 1)

// Define macro for detecting matrices
#define IsMatrix(ArrRank)               ((ArrRank) EQ 2)

// Define macro for detecting matrices or higher rank arrays
#define IsMultiRank(ArrRank)            ((ArrRank) >  1)

// Define macro for detecting rank 3 or higher rank arrays
#define IsRank3P(ArrRank)               ((ArrRank) >  2)

// Define macro for detecting empty arrays
#define IsEmpty(ArrNELM)                ((ArrNELM) EQ 0)

// Define macro for detecting singletons
#define IsSingleton(ArrNELM)            ((ArrNELM) EQ 1)

// Define macro for detecting multiple element arrays
#define IsMultiNELM(ArrNELM)            ((ArrNELM) >  1)

// Define macro for detecting singleton vectors
#define IsVectorSing(ArrNELM,ArrRank)   (IsSingleton (ArrNELM) && IsVector (ArrRank))

// Define macro for detecting simple scalars
#define IsSimpleScalar(ArrType,ArrRank) (IsSimple (ArrType) && IsScalar (ArrRank))

// Define macro for detecting zero dimensions
#define IsZeroDim(ArrDim)               ((ArrDim) EQ 0)

// Define macro for detecting unit dimensions
#define IsUnitDim(ArrDim)               ((ArrDim) EQ 1)

// Define macro for detecting length > 1 dimensions
#define IsMultiDim(ArrDim)              ((ArrDim) >  1)

// Define macro for detecting simple Boolean value
#define IsBooleanValue(Val)             ((Val) EQ 0 || (Val) EQ 1)

// Define macro for detecting a parenthetic list
#define IsTknParList(Tkn)               ((Tkn)->tkFlags.TknType EQ TKT_LISTPAR)

// Macro to skip over the TOKEN_HEADER
#define TokenBaseToStart(base)  (LPTOKEN) (ByteAddr (base, sizeof (TOKEN_HEADER)))

// Macros to skip from the variable array base to either the dimensions or the data
#define VarArrayBaseToDim(lpMem)          (LPAPLDIM) (((LPCHAR) (lpMem)) + sizeof (VARARRAY_HEADER)                              )
#define VarArrayBaseToData(lpMem,aplRank) (LPVOID)   (((LPCHAR) (lpMem)) + sizeof (VARARRAY_HEADER) + sizeof (APLDIM) * (aplRank))
#define VarArrayDimToData(lpMem,aplRank)  (LPVOID)   (((LPCHAR) (lpMem))                            + sizeof (APLDIM) * (aplRank))

// Macro to skip from the function array base to the data
#define FcnArrayBaseToData(lpMem) (LPVOID)   (((LPCHAR) (lpMem)) + sizeof (FCNARRAY_HEADER))

// Macro to skip from the named array base to the data
#define VarNamedBaseToData(lpMem) (LPVOID)   (((LPCHAR) (lpMem)) + sizeof (VARNAMED_HEADER))

// Macro to skip from the list array base to the data
#define LstArrayBaseToData(lpMem) (LPVOID)   (((LPCHAR) (lpMem)) + sizeof (LSTARRAY_HEADER))

// Macros to clear the low-order bits of either an LPSYMENTRY,
//   or HGLOBAL (luckily, both types of ptrs are the same size).
// These macros come in either direct (Dir) or indirect (Ind) form
#define ClrPtrTypeDir(lpMem)                       ((~PTRTYPE_MASK) &  (HANDLE_PTR  ) (lpMem))
#define ClrPtrTypeDirAsSym(lpMem)     (LPSYMENTRY) ((~PTRTYPE_MASK) &  (HANDLE_PTR  ) (lpMem))
#define ClrPtrTypeDirAsGlb(lpMem)     (HGLOBAL)    ((~PTRTYPE_MASK) &  (HANDLE_PTR  ) (lpMem))
#define ClrPtrTypeDirAsFcn(lpMem)     (LPPRIMFNS)  ((~PTRTYPE_MASK) &  (HANDLE_PTR  ) (lpMem))
#define ClrPtrTypeInd(lpMem)                       ((~PTRTYPE_MASK) & *(HANDLE_PTR *) (lpMem))
#define ClrPtrTypeIndAsSym(lpMem)     (LPSYMENTRY) ((~PTRTYPE_MASK) & *(HANDLE_PTR *) (lpMem))
#define ClrPtrTypeIndAsGlb(lpMem)     (HGLOBAL)    ((~PTRTYPE_MASK) & *(HANDLE_PTR *) (lpMem))

// Macro to extract the low-order bits of a memory ptr used
//   to distinguish between the various pointer types.
// NOTE:  THIS MACRO CALLS ITS ARGUMENT *TWICE*, HENCE IT WILL WORK DIFFERENTLY
//        IN THE DEBUG VERSION FROM THE NON-DEBUG VERSION IF THE ARGUMENT HAS
//        ANY SIDE EFFECTS SUCH AS PRE- OR POST-INCREMENT/DECREMENT, OR THE LIKE.
#ifdef DEBUG
  #define GetPtrTypeDir(lpMem)        (BYTE) ((lpMem EQ NULL) ? PTRTYPE_AVAIL : (  PTRTYPE_MASK  &  (HANDLE_PTR  ) (lpMem)))
  #define GetPtrTypeInd(lpMem)        (BYTE) ((lpMem EQ NULL) ? PTRTYPE_AVAIL : (  PTRTYPE_MASK  & *(HANDLE_PTR *) (lpMem)))
#else
  #define GetPtrTypeDir(lpMem)        (BYTE)       (  PTRTYPE_MASK  &  (HANDLE_PTR  ) (lpMem))
  #define GetPtrTypeInd(lpMem)        (BYTE)       (  PTRTYPE_MASK  & *(HANDLE_PTR *) (lpMem))
#endif
// Macro to create a masked LPSYMENTRY
#define MakePtrTypeSym(lpMem)         (LPSYMENTRY) (PTRTYPE_STCONST |  (HANDLE_PTR  ) (lpMem))

// Macro to create a masked HGLOBAL
#define MakePtrTypeGlb(lpMem)         (HGLOBAL)    (PTRTYPE_HGLOBAL |  (HANDLE_PTR  ) (lpMem))

// Macros to check on PTR_REUSED
#define PtrReusedDir(lpMem)                  ((lpMem) EQ PTR_REUSED)
#define PtrReusedInd(lpMem)     ((*(LPVOID *) lpMem)  EQ PTR_REUSED)

// Note that the following macros depend upon
//   the ordering of the enum IMM_TYPES in <symtab.h>
#define IsImmBool(a)    ((a) EQ IMMTYPE_BOOL)
#define IsImmInt(a)     (IMMTYPE_ERROR < (a) && (a) < IMMTYPE_FLOAT)
#define IsImmNum(a)     (IMMTYPE_ERROR < (a) && (a) < IMMTYPE_CHAR)
#define IsImmFlt(a)     ((a) EQ IMMTYPE_FLOAT)
#define IsImmChr(a)     ((a) EQ IMMTYPE_CHAR)

// The enum NAME_TYPES in <symtab.h> is constructed to allow
//  the following macros to be used.
#define IsNameTypeFn(a)     ((a) &  NAMETYPEMASK_FN                   )
#define IsNameTypeOp(a)     ((a) &                    NAMETYPEMASK_OP )
#define IsNameTypeFnOp(a)   ((a) & (NAMETYPEMASK_FN | NAMETYPEMASK_OP))
#define IsNameTypeVar(a)    ((a) EQ NAMETYPE_VAR)
#define IsNameTypeName(a)   (NAMETYPE_VAR <= (a) && (a) <= NAMETYPE_OP3)


#define GetSignatureMem(a)  (((LPHEADER_SIGNATURE) (a))->nature)


// Macros to get sizeof and lengthof for items
#define itemsizeof(a)       (sizeof (a) / sizeof ((a)[0]))
#define itemlengthof(a)     (itemsizeof (a) - 1)


//***************************************************************************
//  End of File: macros.h
//***************************************************************************
