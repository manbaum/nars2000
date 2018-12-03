//***************************************************************************
//  NARS2000 -- Hypercomplex Stuff
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
//  Hypercomplex separators, indices, dimensions, etc.
//***************************************************************************

#define HC8SEP  (OptionFlags.bDispOctoDig ? hc8SepDig : hc8SepDef)

EXTERN
LPWCHAR hc2Sep[1]
#ifdef DEFINE_VALUES
 = {DEF_HC2SEP_WS}
#endif
,
        hc4Sep[3]
#ifdef DEFINE_VALUES
 = {DEF_HC4SEP}
#endif
,
        hc8SepDef[7]
#ifdef DEFINE_VALUES
 = {DEF_HC8SEPDEF}
#endif
,
        hc8SepDig[7]
#ifdef DEFINE_VALUES
 = {DEF_HC8SEPDIG}
#endif
;

EXTERN
APLHC8I confpoi_0
#ifdef DEFINE_VALUES
 = {0, 0, 0, 0, 0, 0, 0, 0}
#endif
,
        confpoi_1
#ifdef DEFINE_VALUES
 = {1, 0, 0, 0, 0, 0, 0, 0}
#endif
,
        confpoi_N1
#ifdef DEFINE_VALUES
 = {-1, 0, 0, 0, 0, 0, 0, 0}
#endif
,
        confpoi_I
#ifdef DEFINE_VALUES
 = {0, 1, 0, 0, 0, 0, 0, 0}
#endif
,
        confpoi_NI
#ifdef DEFINE_VALUES
 = {0, -1, 0, 0, 0, 0, 0, 0}
#endif
;

EXTERN
APLHC8F confpof_0
#ifdef DEFINE_VALUES
 = {0, 0, 0, 0, 0, 0, 0, 0}
#endif
,
        confpof_1
#ifdef DEFINE_VALUES
 = {1, 0, 0, 0, 0, 0, 0, 0}
#endif
,
        confpof_N1
#ifdef DEFINE_VALUES
 = {-1, 0, 0, 0, 0, 0, 0, 0}
#endif
,
        confpof_I
#ifdef DEFINE_VALUES
 = {0, 1, 0, 0, 0, 0, 0, 0}
#endif
,
        confpof_NI
#ifdef DEFINE_VALUES
 = {0, -1, 0, 0, 0, 0, 0, 0}
#endif
,
        aplHC8F_Pi,
        aplHC8F_E,
        aplHC8F_Gamma
;

#define fpciZero    confpoi_0.partsLo->partsLo[0]
#define fphiZero    confpoi_0.partsLo[0]
#define fpoiZero    confpoi_0

#define fpcfZero    confpof_0.partsLo->partsLo[0]
#define fphfZero    confpof_0.partsLo[0]
#define fpofZero    confpof_0

#define fpciOne     confpoi_1.partsLo->partsLo[0]
#define fphiOne     confpoi_1.partsLo[0]
#define fpoiOne     confpoi_1

#define fpcfOne     confpof_1.partsLo->partsLo[0]
#define fphfOne     confpof_1.partsLo[0]
#define fpofOne     confpof_1

#define mpciZero    conmpoi_0.partsLo.partsLo
#define mphiZero    conmpoi_0.partsLo
#define mpoiZero    conmpoi_0

#define mpcfZero    conmpof_0.partsLo.partsLo
#define mphfZero    conmpof_0.partsLo
#define mpofZero    conmpof_0

#define mpciOne     conmpoi_1.partsLo.partsLo
#define mphiOne     conmpoi_1.partsLo
#define mpoiOne     conmpoi_1

#define mpcfOne     conmpof_1.partsLo.partsLo
#define mphfOne     conmpof_1.partsLo
#define mpofOne     conmpof_1

EXTERN
APLHC8R conmpoi_0
#ifdef DEFINE_VALUES
= {0}
#endif
,
        conmpoi_1
#ifdef DEFINE_VALUES
= {0}
#endif
,
        conmpoi_N1
#ifdef DEFINE_VALUES
= {0}
#endif
,
        conmpoi_I
#ifdef DEFINE_VALUES
= {0}
#endif
,
        conmpoi_NI
#ifdef DEFINE_VALUES
= {0}
#endif
;

#define mpciZero    conmpoi_0.partsLo.partsLo
#define mphiZero    conmpoi_0.partsLo
#define mpoiZero    conmpoi_0

#define mpcfZero    conmpof_0.partsLo.partsLo
#define mphfZero    conmpof_0.partsLo
#define mpofZero    conmpof_0

#define mpciOne     conmpoi_1.partsLo.partsLo
#define mphiOne     conmpoi_1.partsLo
#define mpoiOne     conmpoi_1

#define mpcfOne     conmpof_1.partsLo.partsLo
#define mphfOne     conmpof_1.partsLo
#define mpofOne     conmpof_1

EXTERN
APLHC8V conmpof_0
#ifdef DEFINE_VALUES
= {0}
#endif
,
        conmpof_1
#ifdef DEFINE_VALUES
= {0}
#endif
,
        conmpof_N1
#ifdef DEFINE_VALUES
= {0}
#endif
,
        conmpof_I
#ifdef DEFINE_VALUES
= {0}
#endif
,
        conmpof_NI
#ifdef DEFINE_VALUES
= {0}
#endif
;

typedef LPAPLCHAR (*HCI_FCN) (LPWCHAR, LPVOID);
typedef LPAPLCHAR (*HCF_FCN) (LPWCHAR, LPVOID, APLUINT);
typedef LPAPLCHAR (*HCR_FCN) (LPWCHAR, LPVOID);
typedef LPAPLCHAR (*HCV_FCN) (LPWCHAR, LPVOID, APLUINT);

EXTERN
ENUM_HCFI aArrayTypeToHCDimIndex[ARRAY_LENGTH]
#ifdef DEFINE_VALUES
 = {
    ENUM_HCFI1,     // ARRAY_BOOL
    ENUM_HCFI1,     // ARRAY_INT
    ENUM_HCFI1,     // ARRAY_FLOAT
    ENUM_HCFI1,     // ARRAY_CHAR
    ENUM_HCFI1,     // ARRAY_HETERO
    ENUM_HCFI1,     // ARRAY_NESTED
            -1,     // ARRAY_LIST
    ENUM_HCFI1,     // ARRAY_APA
    ENUM_HCFI1,     // ARRAY_RAT
    ENUM_HCFI1,     // ARRAY_VFP
    ENUM_HCFI2,     // ARRAY_HC2I
    ENUM_HCFI2,     // ARRAY_HC2F
    ENUM_HCFI2,     // ARRAY_HC2R
    ENUM_HCFI2,     // ARRAY_HC2V
    ENUM_HCFI4,     // ARRAY_HC4I
    ENUM_HCFI4,     // ARRAY_HC4F
    ENUM_HCFI4,     // ARRAY_HC4R
    ENUM_HCFI4,     // ARRAY_HC4V
    ENUM_HCFI8,     // ARRAY_HC8I
    ENUM_HCFI8,     // ARRAY_HC8F
    ENUM_HCFI8,     // ARRAY_HC8R
    ENUM_HCFI8,     // ARRAY_HC8V
}
#endif
;

// N.B.: Whenever changing the above array (aArrayTypeToHCDimIndex),
//   be sure to make a corresponding change to
//   the four <HCx_FCN>s in <hc_proc.h>.


// Same order as in ARRAY_TYPES
EXTERN
UINT uSizeofMap[ARRAY_LENGTH]
#ifdef DEFINE_VALUES
 = {sizeof (APLBOOL  ),
    sizeof (APLINT   ),
    sizeof (APLFLOAT ),
    sizeof (APLCHAR  ),
    sizeof (APLHETERO),
    sizeof (APLNESTED),
    sizeof (APLLIST  ),
    sizeof (APLAPA   ),
    sizeof (APLRAT   ),
    sizeof (APLVFP   ),
    sizeof (APLHC2I  ),
    sizeof (APLHC2F  ),
    sizeof (APLHC2R  ),
    sizeof (APLHC2V  ),
    sizeof (APLHC4I  ),
    sizeof (APLHC4F  ),
    sizeof (APLHC4R  ),
    sizeof (APLHC4V  ),
    sizeof (APLHC8I  ),
    sizeof (APLHC8F  ),
    sizeof (APLHC8R  ),
    sizeof (APLHC8V  ),
   }
#endif
;

#define TranslateArrayTypeToSizeof(a)   uSizeofMap[(a)]

EXTERN
int aHCDimIndexToHCDim[4]
#ifdef DEFINE_VALUES
 = {1,                  // 0
    2,                  // 1
    4,                  // 2
    8,                  // 3
   }
#endif
;

#define TranslateArrayTypeToHCDim(a)    aHCDimIndexToHCDim[aArrayTypeToHCDimIndex[(a)]]
#define TranslateNumTypeToHCDim(a)      aHCDimIndexToHCDim[aArrayTypeToHCDimIndex[aNumTypeToArrayType[(a)]]]

#define TranslateNumTypeToHCNumType(a,b)    (a + b * (PN_NUMTYPE_HC4R - PN_NUMTYPE_HC2R))

// Translate table to convert HCxy to the corresponding simple type (IFRV±)
//   converting APA to INT
EXTERN
ARRAY_TYPES aToSimple[ARRAY_LENGTH]
#ifdef DEFINE_VALUES
 = {
       ARRAY_INT   ,    // ARRAY_BOOL
       ARRAY_INT   ,    // ARRAY_INT
       ARRAY_FLOAT ,    // ARRAY_FLOAT
       ARRAY_ERROR ,    // ARRAY_CHAR
       ARRAY_ERROR ,    // ARRAY_HETERO
       ARRAY_ERROR ,    // ARRAY_NESTED
       ARRAY_ERROR ,    // ARRAY_LIST
       ARRAY_INT   ,    // ARRAY_APA
       ARRAY_RAT   ,    // ARRAY_RAT
       ARRAY_VFP   ,    // ARRAY_VFP
       ARRAY_INT   ,    // ARRAY_HC2I
       ARRAY_FLOAT ,    // ARRAY_HC2F
       ARRAY_RAT   ,    // ARRAY_HC2R
       ARRAY_VFP   ,    // ARRAY_HC2V
       ARRAY_INT   ,    // ARRAY_HC4I
       ARRAY_FLOAT ,    // ARRAY_HC4F
       ARRAY_RAT   ,    // ARRAY_HC4R
       ARRAY_VFP   ,    // ARRAY_HC4V
       ARRAY_INT   ,    // ARRAY_HC8I
       ARRAY_FLOAT ,    // ARRAY_HC8F
       ARRAY_RAT   ,    // ARRAY_HC8R
       ARRAY_VFP   ,    // ARRAY_HC8V
}
#endif
;

// Translate table to convert HCxy to the corresponding simple type (IFRV±)
//   leaving APA alone
EXTERN
ARRAY_TYPES aToSimpleAPA[ARRAY_LENGTH]
#ifdef DEFINE_VALUES
 = {
       ARRAY_INT   ,    // ARRAY_BOOL
       ARRAY_INT   ,    // ARRAY_INT
       ARRAY_FLOAT ,    // ARRAY_FLOAT
       ARRAY_ERROR ,    // ARRAY_CHAR
       ARRAY_ERROR ,    // ARRAY_HETERO
       ARRAY_ERROR ,    // ARRAY_NESTED
       ARRAY_ERROR ,    // ARRAY_LIST
       ARRAY_APA   ,    // ARRAY_APA
       ARRAY_RAT   ,    // ARRAY_RAT
       ARRAY_VFP   ,    // ARRAY_VFP
       ARRAY_INT   ,    // ARRAY_HC2I
       ARRAY_FLOAT ,    // ARRAY_HC2F
       ARRAY_RAT   ,    // ARRAY_HC2R
       ARRAY_VFP   ,    // ARRAY_HC2V
       ARRAY_INT   ,    // ARRAY_HC4I
       ARRAY_FLOAT ,    // ARRAY_HC4F
       ARRAY_RAT   ,    // ARRAY_HC4R
       ARRAY_VFP   ,    // ARRAY_HC4V
       ARRAY_INT   ,    // ARRAY_HC8I
       ARRAY_FLOAT ,    // ARRAY_HC8F
       ARRAY_RAT   ,    // ARRAY_HC8R
       ARRAY_VFP   ,    // ARRAY_HC8V
}
#endif
;

#define TranslateArrayTypeToIFRV(a)     aToSimple[(a)]

// Round up an index (0 to 7) to the next higher power of 2
EXTERN
int   iHCDimPof2[8]
#ifdef DEFINE_VALUES
 = {1,                  // 0
    2,                  // 1
    4,                  // 2
    4,                  // 3
    8,                  // 4
    8,                  // 5
    8,                  // 6
    8,                  // 7
   }
#endif
,     iHCDimLog2[9]
#ifdef DEFINE_VALUES
 = {-1,                 // 0
     0,                 // 1
     1,                 // 2
    -1,                 // 3
     2,                 // 4
    -1,                 // 5
    -1,                 // 6
    -1,                 // 7
     3,                 // 8
   }
#endif
;

// Conversion table from iHCDim to uHCDimIndex
EXTERN
ENUM_HCFI aHCDimToIndex[9]
#ifdef DEFINE_VALUES
 = {ENUM_HCFI_ERR ,     // 0 == -1
    ENUM_HCFI1    ,     // 1 ==  0
    ENUM_HCFI2    ,     // 2 ==  1
    ENUM_HCFI4    ,     // 3 ==  2
    ENUM_HCFI4    ,     // 4 ==  2
    ENUM_HCFI8    ,     // 5 ==  3
    ENUM_HCFI8    ,     // 6 ==  3
    ENUM_HCFI8    ,     // 7 ==  3
    ENUM_HCFI8    ,     // 8 ==  3
  }
#endif
;

// Conversion table from uHCDimIndex to INT ARRAY_TYPE
EXTERN
ARRAY_TYPES uHCxI[4]
#ifdef DEFINE_VALUES
 = {ARRAY_HC1I,
    ARRAY_HC2I,
    ARRAY_HC4I,
    ARRAY_HC8I,
   }
#endif
;

// Conversion table from uHCDimIndex to FLT ARRAY_TYPE
EXTERN
ARRAY_TYPES uHCxF[4]
#ifdef DEFINE_VALUES
 = {ARRAY_HC1F,
    ARRAY_HC2F,
    ARRAY_HC4F,
    ARRAY_HC8F,
   }
#endif
;

// Conversion table from uHCDimIndex to RAT ARRAY_TYPE
EXTERN
ARRAY_TYPES uHCxR[4]
#ifdef DEFINE_VALUES
 = {ARRAY_HC1R,
    ARRAY_HC2R,
    ARRAY_HC4R,
    ARRAY_HC8R,
   }
#endif
;

// Conversion table from uHCDimIndex to VFP ARRAY_TYPE
EXTERN
ARRAY_TYPES uHCxV[4]
#ifdef DEFINE_VALUES
 = {ARRAY_HC1V,
    ARRAY_HC2V,
    ARRAY_HC4V,
    ARRAY_HC8V,
   }
#endif
;

LPAPLCHAR FormatAplInt    (LPWCHAR, LPVOID);
LPAPLCHAR FormatAplFlt    (LPWCHAR, LPVOID, APLUINT);
LPAPLCHAR FormatAplRat    (LPWCHAR, LPVOID);
LPAPLCHAR FormatAplVfp    (LPWCHAR, LPVOID, APLUINT);
LPAPLCHAR FormatAplHC2I   (LPWCHAR, LPVOID);
LPAPLCHAR FormatAplHC2F   (LPWCHAR, LPVOID, APLUINT);
LPAPLCHAR FormatAplHC2R   (LPWCHAR, LPVOID);
LPAPLCHAR FormatAplHC2V   (LPWCHAR, LPVOID, APLUINT);
LPAPLCHAR FormatAplHC4I   (LPWCHAR, LPVOID);
LPAPLCHAR FormatAplHC4F   (LPWCHAR, LPVOID, APLUINT);
LPAPLCHAR FormatAplHC4R   (LPWCHAR, LPVOID);
LPAPLCHAR FormatAplHC4V   (LPWCHAR, LPVOID, APLUINT);
LPAPLCHAR FormatAplHC8I   (LPWCHAR, LPVOID);
LPAPLCHAR FormatAplHC8F   (LPWCHAR, LPVOID, APLUINT);
LPAPLCHAR FormatAplHC8R   (LPWCHAR, LPVOID);
LPAPLCHAR FormatAplHC8V   (LPWCHAR, LPVOID, APLUINT);

// Arrays of HCx functions
HCI_FCN hcIFcn[ENUM_HCFI_LENGTH]
#ifdef DEFINE_VALUES
= {FormatAplInt, FormatAplHC2I, FormatAplHC4I, FormatAplHC8I}
#endif
;
HCF_FCN hcFFcn[ENUM_HCFI_LENGTH]
#ifdef DEFINE_VALUES
= {FormatAplFlt, FormatAplHC2F, FormatAplHC4F, FormatAplHC8F}
#endif
;
HCR_FCN hcRFcn[ENUM_HCFI_LENGTH]
#ifdef DEFINE_VALUES
= {FormatAplRat, FormatAplHC2R, FormatAplHC4R, FormatAplHC8R}
#endif
;
HCV_FCN hcVFcn[ENUM_HCFI_LENGTH]
#ifdef DEFINE_VALUES
= {FormatAplVfp, FormatAplHC2V, FormatAplHC4V, FormatAplHC8V}
#endif
;


#include "hc_vars.h"


//***************************************************************************
//  End of File: hc_proc.h
//***************************************************************************
