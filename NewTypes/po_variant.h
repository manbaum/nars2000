//***************************************************************************
//  NARS2000 -- Primitive Operator -- Variant Header
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2019 Sudley Place Software

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


//***************************************************************************
//  Variant enums, strucs, etc.
//***************************************************************************

typedef enum tagVARIANT_KEYS
{
    VARIANT_KEY_CT  = 0 ,   // 00:  []CT
    VARIANT_KEY_DT      ,   // 01:  []DT
    VARIANT_KEY_FPC     ,   // 02:  []FPC
    VARIANT_KEY_IO      ,   // 03:  []IO
    VARIANT_KEY_LR      ,   // 04:  []LR
    VARIANT_KEY_PP      ,   // 05:  []PP
    VARIANT_KEY_RL      ,   // 06:  []RL
    VARIANT_KEY_LENGTH  ,   // 07:  # entries
} VARIANTKEYS, *LPVARIANTKEYS;

#define VARIANT_KEY_ERROR   VARIANT_KEY_LENGTH

// N.B.:  Whenever changing the above enum (VARIANT_KEYS)
//   be sure to make a corresponding change to
//   <aVariantKeyStr> in <po_variant.c>.

typedef struct tagVARIANT_KEY_STR
{
    LPAPLCHAR         lpKeyName;        // 00:  Ptr to key name (ASCIIZ)
    SYS_VARS          sysVarIndex;      // 04:  Sys var index (see SYSVARS)
    ASYSVARVALIDSET   aSysVarValidSet;  // 08:  Ptr to validation function
} VARIANTKEYSTR, LPVARIANTKEYSTR;



typedef struct tagVARIANT_USE_STR
{
    UBOOL    bInuse;            // 00:  TRUE iff this entry is in use
    SYMENTRY OldSymEntry;       // 04:  Old SYMENTRY
} VARIANTUSESTR, *LPVARIANTUSESTR;


typedef enum tagENUM_VARIANT
{
    ENUM_VARIANT_UNK    = 0,        // 00:  No value
    ENUM_VARIANT_CT        ,        // 01:  []CT
    ENUM_VARIANT_DT        ,        // 02:  []DT
    ENUM_VARIANT_FPC       ,        // 03:  []FPC
    ENUM_VARIANT_IO        ,        // 04:  []IO
    ENUM_VARIANT_LR        ,        // 05:  []LR
    ENUM_VARIANT_PP        ,        // 06:  []PP
    ENUM_VARIANT_RL        ,        // 07:  []RL
    ENUM_VARIANT_EIG       ,        // 08:  {domino} Eigenvalues, vectors, and Schur vectors
    ENUM_VARIANT_POCH      ,        // 09:  {shriek} Pochhammer k-symbol
    ENUM_VARIANT_HCM       ,        // 0A:  {times} Interior/Exterior/Cross/Dot/Conjugation product
    ENUM_VARIANT_SUB       ,        // 0B:  Ascending/descending subsequence
    ENUM_VARIANT_GALL      ,        // 0C:  Grade all arrays
    ENUM_VARIANT_LENGTH             // 0D:  # enums
} ENUM_VARIANT, *LPENUM_VARIANT;

// N.B.:  Whenever changing the above enum (ENUM_VARIANT)
//   be sure to make a corresponding change to
//   <varOprType> and <varOprValTab> in <po_variant.c>.


#define VAR_MAC(a,N1,N2,C1,C2,N3,N4,C3,C4,OP)       \
    {UTF16_##a,                                     \
     ENUM_VARIANT_##N1,                             \
     ENUM_VARIANT_##N2,                             \
     ENUM_VARIANT_##C1,                             \
     ENUM_VARIANT_##C2,                             \
     ENUM_VARIANT_##N3,                             \
     ENUM_VARIANT_##N4,                             \
     ENUM_VARIANT_##C3,                             \
     ENUM_VARIANT_##C4,                             \
     OP,                                            \
    }

typedef struct tagVARIANT_STR
{
    WCHAR           wc;                     // The UTF16_xxx character
                                            //                           Right Operand
                                            //                           Validator Enum
    ENUM_VARIANT    enumVarMonN1,           // wc as a Monadic function, Num1 in Num1 or Num1 Num2 or Num1 Chr2
                    enumVarMonN2,           // ...                       Num2 in         Num1 Num2 or Chr1 Num2
                    enumVarMonC1,           // ...                       Chr1 in Chr1 or Chr1 Chr2 or Chr1 Num2
                    enumVarMonC2,           // ...                       Chr2 in         Chr1 Chr2 or Num1 Chr2
                    enumVarDydN1,           // wc as a Dyadic  function, Num1 in Num1 or Num1 Num2 or Num1 Chr1
                    enumVarDydN2,           // ...                       Num2 in         Num1 Num2 or Chr1 Num2
                    enumVarDydC1,           // ...                       Chr1 in Chr1 or Chr1 Chr2 or Chr1 Num2
                    enumVarDydC2;           // ...                       Chr2 in         Chr1 Chr2 or Num1 Chr2
    UINT            (*SYMTRANS) (WCHAR);    // Ptr to translation function (FcnTrans/OprTrans)
} VARIANT_STR, *LPVARIANT_STR;

typedef struct tagALLSYSVARS
{
    // Old values
    APLBOOL    bQuadIO;         // 00:  []IO
    APLUINT    uQuadPP,         // 01:  []PP
               uQuadRL,         // 02:  []RL
               uQuadFPC;        // 03:  []FPC
    APLFLOAT   fQuadCT;         // 04:  []CT
    APLCHAR    cQuadDT,         // 05:  []DT
               cQuadLR;         // 06:  []LR
    ENUM_HCMUL eHCMul;          // 07:  New HCM value saved in <lpMemPTD->eHCMul>

    // New values
    APLCHAR    cSubNew;         //      Ascending/descending subsequence
    APLUINT    uEigenNew,       //      Eigenvalues & -vectors, etc.
               uPochNew;        //      Pochhammer k-symbol
} ALLSYSVARS_STR, *LPALLSYSVARS_STR;


typedef enum tagENUMNCF     // Num/Chr Found state
{
    ENUM_NCF_UNK = 0    ,   // 00:  Unknown value
    ENUM_NCF_NUM        ,   // 01:  Num found
    ENUM_NCF_CHR        ,   // 02:  Chr ...
} ENUM_NCF, *LPENUM_NCF;


//***************************************************************************
//  End of File: po_variant.h
//***************************************************************************
