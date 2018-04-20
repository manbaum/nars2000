//***************************************************************************
//  NARS2000 -- Primitive Operator -- Variant Header
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


//***************************************************************************
//  Variant enums, strucs, etc.
//***************************************************************************

typedef enum tagVARIANT_KEYS
{
    VARIANT_KEY_ALX = 0 ,   // 00:  []ALX
    VARIANT_KEY_CT      ,   // 01:  []CT
    VARIANT_KEY_LR      ,   // 02:  []LR
    VARIANT_KEY_DT      ,   // 03:  []DT
    VARIANT_KEY_ELX     ,   // 04:  []ALX
    VARIANT_KEY_FC      ,   // 05:  []FC
    VARIANT_KEY_FEATURE ,   // 06:  []FEATURE
    VARIANT_KEY_FPC     ,   // 07:  []FPC
    VARIANT_KEY_IO      ,   // 08:  []IO
    VARIANT_KEY_PP      ,   // 09:  []PP
    VARIANT_KEY_RL      ,   // 0A:  []PP
    VARIANT_KEY_LENGTH  ,   // 0B:  # entries
} VARIANTKEYS, *LPVARIANTKEYS;

#define VARIANT_KEY_ERROR   VARIANT_KEY_LENGTH

// N.B.:  Whenever changing the above enum (VARIANT_KEYS)
//   be sure to make a corresponding change to
//   <aVariantKeyStr> below.

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
    ENUM_VARIANT_EIG       ,        // 02:  {domino} Eigenvalues, vectors, and Schur vectors
    ENUM_VARIANT_DT        ,        // 03:  []DT
    ENUM_VARIANT_FPC       ,        // 04:  []FPC
    ENUM_VARIANT_IO        ,        // 05:  []IO
    ENUM_VARIANT_LR        ,        // 06:  []LR
    ENUM_VARIANT_PP        ,        // 07:  []PP
    ENUM_VARIANT_POCH      ,        // 08:  {shriek} Pochhammer k-symbol
    ENUM_VARIANT_INEX      ,        // 09:  {times} Interior/Exterior product
    ENUM_VARIANT_LENGTH             // 0A:  # enums
} ENUM_VARIANT, *LPENUM_VARIANT;

// N.B.:  Whenever changing the above enum (ENUM_VARIANT)
//   be sure to make a corresponding change to
//   <varOprType> in <po_variant.c>.


#define VAR_MAC(a,N1,N2,C1,C2,N3,N4,C3,C4)  \
    {UTF16_##a,                             \
     ENUM_VARIANT_##N1,                     \
     ENUM_VARIANT_##N2,                     \
     ENUM_VARIANT_##C1,                     \
     ENUM_VARIANT_##C2,                     \
     ENUM_VARIANT_##N3,                     \
     ENUM_VARIANT_##N4,                     \
     ENUM_VARIANT_##C3,                     \
     ENUM_VARIANT_##C4,                     \
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
} VARIANT_STR, *LPVARIANT_STR;

typedef struct tagALLSYSVARS
{
    // Old values
    APLBOOL  bQuadIO;           // 00:  []IO
    APLUINT  uQuadPP,           // 01:  []PP
             uEigenOld,         // 02:  Eigen
             uQuadFPC,          // 03:  []FPC
             uPochOld;          // 04:  Pochhammer
    APLFLOAT fQuadCT;           // 05:  []CT
    APLCHAR  cQuadDT,           // 06:  []DT
             cQuadLR;           // 07:  []LR
    // New values
    APLUINT    uEigenNew,       //
               uPochNew;        //
    ENUM_HCMUL eHCMul;          // Old value saved in <lpMemPTD->eHCMul>
} ALLSYSVARS_STR, *LPALLSYSVARS_STR;


//***************************************************************************
//  End of File: po_variant.h
//***************************************************************************
