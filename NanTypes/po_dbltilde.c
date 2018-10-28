//***************************************************************************
//  NARS2000 -- Primitive Operator -- ArbPrec
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

#define STRICT
#include <windows.h>
#include "headers.h"
#include "acb.h"
#include "po_dbltilde.mac"


#ifndef PROTO
PRIMSPEC PrimSpecDoubleTildeInter =
{
    // Monadic functions
    NULL,   // &PrimFnMon_EM_YY, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimSpecDoubleTildeInterStorageTypeMon, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterAPA_EM, -- Monadic DoubleTildeInter not scalar

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonDoubleTildeInterBisB, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterBisI, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterBisF, -- Monadic DoubleTildeInter not scalar

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeInterIisI, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterIisF, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterFisI, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterFisF, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterRisR, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterVisR, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterVisV, -- Monadic DoubleTildeInter not scalar

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonDoubleTildeInterHC2IisHC2I, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterHC2IisHC2F, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterHC2FisHC2I, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterHC2FisHC2F, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterHC2RisHC2R, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterHC2VisHC2R, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterHC2VisHC2V, -- Monadic DoubleTildeInter not scalar

    NULL,   // &PrimFnMonDoubleTildeInterHC4IisHC4I, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterHC4IisHC4F, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterHC4FisHC4I, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterHC4FisHC4F, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterHC4RisHC4R, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterHC4VisHC4R, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterHC4VisHC4V, -- Monadic DoubleTildeInter not scalar

    NULL,   // &PrimFnMonDoubleTildeInterHC8IisHC8I, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterHC8IisHC8F, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterHC8FisHC8I, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterHC8FisHC8F, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterHC8RisHC8R, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterHC8VisHC8R, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterHC8VisHC8V, -- Monadic DoubleTildeInter not scalar

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeInterFisHC2I, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterFisHC2F, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterVisHC2R, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterVisHC2V, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterFisHC4I, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterFisHC4F, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterVisHC4R, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterVisHC4V, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterFisHC8I, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterFisHC8F, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterVisHC8R, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterVisHC8V, -- Monadic DoubleTildeInter not scalar

    // Monadic BAxF result BAxF arg functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeInterBA1FisBA1F, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterBA2FisBA2F, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterBA4FisBA4F, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterBA8FisBA8F, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterBA1FisBA1F, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterARBisBA2F, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterARBisBA4F, -- Monadic DoubleTildeInter not scalar
    NULL,   // &PrimFnMonDoubleTildeInterARBisBA8F, -- Monadic DoubleTildeInter not scalar

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecDoubleTildeInterUnionStorageTypeDyd,
    NULL,   // &PrimFnDydDoubleTildeInterAPA_EM,

    // Dyadic Boolean result functions
    NULL,   // &PrimFnDydDoubleTildeInterBisBvB, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterBisIvI, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterBisFvF, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterBisCvC, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterBisHvH, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterBisRvR, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterBisVvV, -- Can't happen w/DoubleTildeInter

    NULL,   // &PrimFnDydDoubleTildeInterBisHC2IvHC2I, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterBisHC2FvHC2F, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterBisHC2RvHC2R, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterBisHC2VvHC2V, -- Can't happen w/DoubleTildeInter

    NULL,   // &PrimFnDydDoubleTildeInterBisHC4IvHC4I, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterBisHC4FvHC4F, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterBisHC4RvHC4R, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterBisHC4VvHC4V, -- Can't happen w/DoubleTildeInter

    NULL,   // &PrimFnDydDoubleTildeInterBisHC8IvHC8I, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterBisHC8FvHC8F, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterBisHC8RvHC8R, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterBisHC8VvHC8V, -- Can't happen w/DoubleTildeInter

    // Dyadic Boolean result functions with ARBs (indexable)
    NULL,   // &PrimFnDydDoubleTildeInterBisBA1FvBA1F, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterBisBA2FvBA2F, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterBisBA4FvBA4F, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterBisBA8FvBA8F, -- Can't happen w/DoubleTildeInter

    // Dyadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnDydDoubleTildeInterIisIvI, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterIisFvF, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterFisIvI, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterFisFvF, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterRisRvR, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterVisRvR, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterVisVvV, -- Can't happen w/DoubleTildeInter

    NULL,   // &PrimFnDydDoubleTildeInterHC2IisHC2IvHC2I, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterHC2IisHC2FvHC2F, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterHC2FisHC2IvHC2I, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterHC2FisHC2FvHC2F, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterHC2RisHC2RvHC2R, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterHC2VisHC2RvHC2R, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterHC2VisHC2VvHC2V, -- Can't happen w/DoubleTildeInter

    NULL,   // &PrimFnDydDoubleTildeInterHC4IisHC4IvHC4I, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterHC4IisHC4FvHC4F, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterHC4FisHC4IvHC4I, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterHC4FisHC4FvHC4F, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterHC4RisHC4RvHC4R, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterHC4VisHC4RvHC4R, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterHC4VisHC4VvHC4V, -- Can't happen w/DoubleTildeInter

    NULL,   // &PrimFnDydDoubleTildeInterHC8IisHC8IvHC8I, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterHC8IisHC8FvHC8F, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterHC8FisHC8IvHC8I, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterHC8FisHC8FvHC8F, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterHC8RisHC8RvHC8R, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterHC8VisHC8RvHC8R, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnDydDoubleTildeInterHC8VisHC8VvHC8V, -- Can't happen w/DoubleTildeInter

    // Dyadic BAxF result functions (indexable)
    &PrimFnDydDoubleTildeInterBA1FisBA1FvBA1F,
    &PrimFnDydDoubleTildeInterBA2FisBA2FvBA2F,
    &PrimFnDydDoubleTildeInterBA4FisBA4FvBA4F,
    &PrimFnDydDoubleTildeInterBA8FisBA8FvBA8F,

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonDoubleTildeInterB64isB64, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnMonDoubleTildeInterB32isB32, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnMonDoubleTildeInterB16isB16, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnMonDoubleTildeInterB08isB08, -- Can't happen w/DoubleTildeInter

    // Dyadic Boolean chunk functions
    NULL,   // &PrimFnMonDoubleTildeInterB64isB64vB64, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnMonDoubleTildeInterB32isB32vB32, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnMonDoubleTildeInterB16isB16vB16, -- Can't happen w/DoubleTildeInter
    NULL,   // &PrimFnMonDoubleTildeInterB08isB08vB08, -- Can't happen w/DoubleTildeInter

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpecInter = {&PrimSpecDoubleTildeInter};
#endif


#ifndef PROTO
PRIMSPEC PrimSpecDoubleTildeUnion =
{
    // Monadic functions
    NULL,   // &PrimFnMon_EM_YY, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimSpecDoubleTildeUnionStorageTypeMon, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionAPA_EM, -- Monadic DoubleTildeUnion not scalar

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonDoubleTildeUnionBisB, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionBisI, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionBisF, -- Monadic DoubleTildeUnion not scalar

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeUnionIisI, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionIisF, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionFisI, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionFisF, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionRisR, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionVisR, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionVisV, -- Monadic DoubleTildeUnion not scalar

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonDoubleTildeUnionHC2IisHC2I, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionHC2IisHC2F, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionHC2FisHC2I, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionHC2FisHC2F, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionHC2RisHC2R, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionHC2VisHC2R, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionHC2VisHC2V, -- Monadic DoubleTildeUnion not scalar

    NULL,   // &PrimFnMonDoubleTildeUnionHC4IisHC4I, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionHC4IisHC4F, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionHC4FisHC4I, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionHC4FisHC4F, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionHC4RisHC4R, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionHC4VisHC4R, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionHC4VisHC4V, -- Monadic DoubleTildeUnion not scalar

    NULL,   // &PrimFnMonDoubleTildeUnionHC8IisHC8I, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionHC8IisHC8F, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionHC8FisHC8I, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionHC8FisHC8F, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionHC8RisHC8R, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionHC8VisHC8R, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionHC8VisHC8V, -- Monadic DoubleTildeUnion not scalar

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeUnionFisHC2I, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionFisHC2F, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionVisHC2R, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionVisHC2V, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionFisHC4I, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionFisHC4F, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionVisHC4R, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionVisHC4V, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionFisHC8I, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionFisHC8F, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionVisHC8R, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionVisHC8V, -- Monadic DoubleTildeUnion not scalar

    // Monadic BAxF result BAxF arg functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeUnionBA1FisBA1F, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionBA2FisBA2F, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionBA4FisBA4F, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionBA8FisBA8F, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionBA1FisBA1F, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionARBisBA2F, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionARBisBA4F, -- Monadic DoubleTildeUnion not scalar
    NULL,   // &PrimFnMonDoubleTildeUnionARBisBA8F, -- Monadic DoubleTildeUnion not scalar

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecDoubleTildeInterUnionStorageTypeDyd,
    NULL,   // &PrimFnDydDoubleTildeUnionAPA_EM,

    // Dyadic Boolean result functions
    NULL,   // &PrimFnDydDoubleTildeUnionBisBvB, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionBisIvI, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionBisFvF, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionBisCvC, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionBisHvH, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionBisRvR, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionBisVvV, -- Can't happen w/DoubleTildeUnion

    NULL,   // &PrimFnDydDoubleTildeUnionBisHC2IvHC2I, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionBisHC2FvHC2F, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionBisHC2RvHC2R, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionBisHC2VvHC2V, -- Can't happen w/DoubleTildeUnion

    NULL,   // &PrimFnDydDoubleTildeUnionBisHC4IvHC4I, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionBisHC4FvHC4F, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionBisHC4RvHC4R, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionBisHC4VvHC4V, -- Can't happen w/DoubleTildeUnion

    NULL,   // &PrimFnDydDoubleTildeUnionBisHC8IvHC8I, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionBisHC8FvHC8F, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionBisHC8RvHC8R, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionBisHC8VvHC8V, -- Can't happen w/DoubleTildeUnion

    // Dyadic Boolean result functions with ARBs (indexable)
    NULL,   // &PrimFnDydDoubleTildeUnionBisBA1FvBA1F, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionBisBA2FvBA2F, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionBisBA4FvBA4F, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionBisBA8FvBA8F, -- Can't happen w/DoubleTildeUnion

    // Dyadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnDydDoubleTildeUnionIisIvI, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionIisFvF, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionFisIvI, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionFisFvF, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionRisRvR, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionVisRvR, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionVisVvV, -- Can't happen w/DoubleTildeUnion

    NULL,   // &PrimFnDydDoubleTildeUnionHC2IisHC2IvHC2I, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionHC2IisHC2FvHC2F, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionHC2FisHC2IvHC2I, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionHC2FisHC2FvHC2F, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionHC2RisHC2RvHC2R, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionHC2VisHC2RvHC2R, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionHC2VisHC2VvHC2V, -- Can't happen w/DoubleTildeUnion

    NULL,   // &PrimFnDydDoubleTildeUnionHC4IisHC4IvHC4I, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionHC4IisHC4FvHC4F, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionHC4FisHC4IvHC4I, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionHC4FisHC4FvHC4F, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionHC4RisHC4RvHC4R, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionHC4VisHC4RvHC4R, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionHC4VisHC4VvHC4V, -- Can't happen w/DoubleTildeUnion

    NULL,   // &PrimFnDydDoubleTildeUnionHC8IisHC8IvHC8I, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionHC8IisHC8FvHC8F, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionHC8FisHC8IvHC8I, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionHC8FisHC8FvHC8F, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionHC8RisHC8RvHC8R, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionHC8VisHC8RvHC8R, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnDydDoubleTildeUnionHC8VisHC8VvHC8V, -- Can't happen w/DoubleTildeUnion

    // Dyadic BAxF result functions (indexable)
    &PrimFnDydDoubleTildeUnionBA1FisBA1FvBA1F,
    &PrimFnDydDoubleTildeUnionBA2FisBA2FvBA2F,
    &PrimFnDydDoubleTildeUnionBA4FisBA4FvBA4F,
    &PrimFnDydDoubleTildeUnionBA8FisBA8FvBA8F,

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonDoubleTildeUnionB64isB64, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnMonDoubleTildeUnionB32isB32, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnMonDoubleTildeUnionB16isB16, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnMonDoubleTildeUnionB08isB08, -- Can't happen w/DoubleTildeUnion

    // Dyadic Boolean chunk functions
    NULL,   // &PrimFnMonDoubleTildeUnionB64isB64vB64, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnMonDoubleTildeUnionB32isB32vB32, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnMonDoubleTildeUnionB16isB16vB16, -- Can't happen w/DoubleTildeUnion
    NULL,   // &PrimFnMonDoubleTildeUnionB08isB08vB08, -- Can't happen w/DoubleTildeUnion

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpecUnion = {&PrimSpecDoubleTildeUnion};
#endif


#ifndef PROTO
PRIMSPEC PrimSpecDoubleTildeLt =
{
    // Monadic functions
    NULL,   // &PrimFnMon_EM_YY, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimSpecLeftCaretStorageTypeMon, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtAPA_EM, -- Monadic DoubleTildeLt not scalar

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonDoubleTildeLtBisB, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtBisI, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtBisF, -- Monadic DoubleTildeLt not scalar

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeLtIisI, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtIisF, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtFisI, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtFisF, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtRisR, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtVisR, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtVisV, -- Monadic DoubleTildeLt not scalar

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonDoubleTildeLtHC2IisHC2I, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtHC2IisHC2F, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtHC2FisHC2I, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtHC2FisHC2F, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtHC2RisHC2R, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtHC2VisHC2R, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtHC2VisHC2V, -- Monadic DoubleTildeLt not scalar

    NULL,   // &PrimFnMonDoubleTildeLtHC4IisHC4I, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtHC4IisHC4F, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtHC4FisHC4I, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtHC4FisHC4F, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtHC4RisHC4R, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtHC4VisHC4R, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtHC4VisHC4V, -- Monadic DoubleTildeLt not scalar

    NULL,   // &PrimFnMonDoubleTildeLtHC8IisHC8I, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtHC8IisHC8F, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtHC8FisHC8I, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtHC8FisHC8F, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtHC8RisHC8R, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtHC8VisHC8R, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtHC8VisHC8V, -- Monadic DoubleTildeLt not scalar

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeLtFisHC2I, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtFisHC2F, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtVisHC2R, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtVisHC2V, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtFisHC4I, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtFisHC4F, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtVisHC4R, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtVisHC4V, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtFisHC8I, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtFisHC8F, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtVisHC8R, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtVisHC8V, -- Monadic DoubleTildeLt not scalar

    // Monadic BAxF result BAxF arg functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeLtBA1FisBA1F, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtBA2FisBA2F, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtBA4FisBA4F, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtBA8FisBA8F, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtBA1FisBA1F, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtARBisBA2F, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtARBisBA4F, -- Monadic DoubleTildeLt not scalar
    NULL,   // &PrimFnMonDoubleTildeLtARBisBA8F, -- Monadic DoubleTildeLt not scalar

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecLeftCaretStorageTypeDyd,
    NULL,   // &PrimFnDydDoubleTildeLtAPA_EM,

    // Dyadic Boolean result functions
    NULL,   // &PrimFnDydDoubleTildeLtBisBvB, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtBisIvI, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtBisFvF, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtBisCvC, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtBisHvH, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtBisRvR, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtBisVvV, -- Can't happen w/DoubleTildeLt

    NULL,   // &PrimFnDydDoubleTildeLtBisHC2IvHC2I, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtBisHC2FvHC2F, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtBisHC2RvHC2R, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtBisHC2VvHC2V, -- Can't happen w/DoubleTildeLt

    NULL,   // &PrimFnDydDoubleTildeLtBisHC4IvHC4I, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtBisHC4FvHC4F, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtBisHC4RvHC4R, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtBisHC4VvHC4V, -- Can't happen w/DoubleTildeLt

    NULL,   // &PrimFnDydDoubleTildeLtBisHC8IvHC8I, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtBisHC8FvHC8F, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtBisHC8RvHC8R, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtBisHC8VvHC8V, -- Can't happen w/DoubleTildeLt

    // Dyadic Boolean result functions with ARBs (indexable)
    &PrimFnDydDoubleTildeLtBisBA1FvBA1F,
    NULL,   // &PrimFnDydDoubleTildeLtBisBA2FvBA2F, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtBisBA4FvBA4F, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtBisBA8FvBA8F, -- Can't happen w/DoubleTildeLt

    // Dyadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnDydDoubleTildeLtIisIvI, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtIisFvF, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtFisIvI, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtFisFvF, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtRisRvR, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtVisRvR, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtVisVvV, -- Can't happen w/DoubleTildeLt

    NULL,   // &PrimFnDydDoubleTildeLtHC2IisHC2IvHC2I, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtHC2IisHC2FvHC2F, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtHC2FisHC2IvHC2I, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtHC2FisHC2FvHC2F, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtHC2RisHC2RvHC2R, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtHC2VisHC2RvHC2R, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtHC2VisHC2VvHC2V, -- Can't happen w/DoubleTildeLt

    NULL,   // &PrimFnDydDoubleTildeLtHC4IisHC4IvHC4I, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtHC4IisHC4FvHC4F, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtHC4FisHC4IvHC4I, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtHC4FisHC4FvHC4F, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtHC4RisHC4RvHC4R, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtHC4VisHC4RvHC4R, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtHC4VisHC4VvHC4V, -- Can't happen w/DoubleTildeLt

    NULL,   // &PrimFnDydDoubleTildeLtHC8IisHC8IvHC8I, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtHC8IisHC8FvHC8F, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtHC8FisHC8IvHC8I, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtHC8FisHC8FvHC8F, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtHC8RisHC8RvHC8R, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtHC8VisHC8RvHC8R, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtHC8VisHC8VvHC8V, -- Can't happen w/DoubleTildeLt

    // Dyadic BAxF result functions (indexable)
    NULL,   // &PrimFnDydDoubleTildeLtBA1FisBA1FvBA1F, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtBA2FisBA2FvBA2F, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtBA4FisBA4FvBA4F, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnDydDoubleTildeLtBA8FisBA8FvBA8F, -- Can't happen w/DoubleTildeLt

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonDoubleTildeLtB64isB64, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnMonDoubleTildeLtB32isB32, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnMonDoubleTildeLtB16isB16, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnMonDoubleTildeLtB08isB08, -- Can't happen w/DoubleTildeLt

    // Dyadic Boolean chunk functions
    NULL,   // &PrimFnMonDoubleTildeLtB64isB64vB64, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnMonDoubleTildeLtB32isB32vB32, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnMonDoubleTildeLtB16isB16vB16, -- Can't happen w/DoubleTildeLt
    NULL,   // &PrimFnMonDoubleTildeLtB08isB08vB08, -- Can't happen w/DoubleTildeLt

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpecLt = {&PrimSpecDoubleTildeLt};
#endif


#ifndef PROTO
PRIMSPEC PrimSpecDoubleTildeLe =
{
    // Monadic functions
    NULL,   // &PrimFnMon_EM_YY, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimSpecLeftCaretUnderbarStorageTypeMon, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeAPA_EM, -- Monadic DoubleTildeLe not scalar

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonDoubleTildeLeBisB, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeBisI, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeBisF, -- Monadic DoubleTildeLe not scalar

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeLeIisI, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeIisF, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeFisI, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeFisF, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeRisR, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeVisR, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeVisV, -- Monadic DoubleTildeLe not scalar

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonDoubleTildeLeHC2IisHC2I, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeHC2IisHC2F, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeHC2FisHC2I, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeHC2FisHC2F, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeHC2RisHC2R, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeHC2VisHC2R, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeHC2VisHC2V, -- Monadic DoubleTildeLe not scalar

    NULL,   // &PrimFnMonDoubleTildeLeHC4IisHC4I, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeHC4IisHC4F, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeHC4FisHC4I, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeHC4FisHC4F, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeHC4RisHC4R, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeHC4VisHC4R, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeHC4VisHC4V, -- Monadic DoubleTildeLe not scalar

    NULL,   // &PrimFnMonDoubleTildeLeHC8IisHC8I, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeHC8IisHC8F, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeHC8FisHC8I, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeHC8FisHC8F, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeHC8RisHC8R, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeHC8VisHC8R, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeHC8VisHC8V, -- Monadic DoubleTildeLe not scalar

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeLeFisHC2I, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeFisHC2F, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeVisHC2R, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeVisHC2V, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeFisHC4I, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeFisHC4F, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeVisHC4R, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeVisHC4V, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeFisHC8I, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeFisHC8F, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeVisHC8R, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeVisHC8V, -- Monadic DoubleTildeLe not scalar

    // Monadic BAxF result BAxF arg functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeLeBA1FisBA1F, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeBA2FisBA2F, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeBA4FisBA4F, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeBA8FisBA8F, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeBA1FisBA1F, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeARBisBA2F, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeARBisBA4F, -- Monadic DoubleTildeLe not scalar
    NULL,   // &PrimFnMonDoubleTildeLeARBisBA8F, -- Monadic DoubleTildeLe not scalar

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecLeftCaretUnderbarStorageTypeDyd,
    NULL,   // &PrimFnDydDoubleTildeLeAPA_EM,

    // Dyadic Boolean result functions
    NULL,   // &PrimFnDydDoubleTildeLeBisBvB, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeBisIvI, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeBisFvF, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeBisCvC, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeBisHvH, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeBisRvR, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeBisVvV, -- Can't happen w/DoubleTildeLe

    NULL,   // &PrimFnDydDoubleTildeLeBisHC2IvHC2I, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeBisHC2FvHC2F, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeBisHC2RvHC2R, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeBisHC2VvHC2V, -- Can't happen w/DoubleTildeLe

    NULL,   // &PrimFnDydDoubleTildeLeBisHC4IvHC4I, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeBisHC4FvHC4F, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeBisHC4RvHC4R, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeBisHC4VvHC4V, -- Can't happen w/DoubleTildeLe

    NULL,   // &PrimFnDydDoubleTildeLeBisHC8IvHC8I, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeBisHC8FvHC8F, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeBisHC8RvHC8R, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeBisHC8VvHC8V, -- Can't happen w/DoubleTildeLe

    // Dyadic Boolean result functions with ARBs (indexable)
    &PrimFnDydDoubleTildeLeBisBA1FvBA1F,
    NULL,   // &PrimFnDydDoubleTildeLeBisBA2FvBA2F, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeBisBA4FvBA4F, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeBisBA8FvBA8F, -- Can't happen w/DoubleTildeLe

    // Dyadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnDydDoubleTildeLeIisIvI, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeIisFvF, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeFisIvI, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeFisFvF, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeRisRvR, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeVisRvR, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeVisVvV, -- Can't happen w/DoubleTildeLe

    NULL,   // &PrimFnDydDoubleTildeLeHC2IisHC2IvHC2I, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeHC2IisHC2FvHC2F, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeHC2FisHC2IvHC2I, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeHC2FisHC2FvHC2F, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeHC2RisHC2RvHC2R, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeHC2VisHC2RvHC2R, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeHC2VisHC2VvHC2V, -- Can't happen w/DoubleTildeLe

    NULL,   // &PrimFnDydDoubleTildeLeHC4IisHC4IvHC4I, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeHC4IisHC4FvHC4F, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeHC4FisHC4IvHC4I, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeHC4FisHC4FvHC4F, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeHC4RisHC4RvHC4R, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeHC4VisHC4RvHC4R, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeHC4VisHC4VvHC4V, -- Can't happen w/DoubleTildeLe

    NULL,   // &PrimFnDydDoubleTildeLeHC8IisHC8IvHC8I, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeHC8IisHC8FvHC8F, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeHC8FisHC8IvHC8I, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeHC8FisHC8FvHC8F, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeHC8RisHC8RvHC8R, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeHC8VisHC8RvHC8R, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeHC8VisHC8VvHC8V, -- Can't happen w/DoubleTildeLe

    // Dyadic BAxF result functions (indexable)
    NULL,   // &PrimFnDydDoubleTildeLeBA1FisBA1FvBA1F, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeBA2FisBA2FvBA2F, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeBA4FisBA4FvBA4F, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnDydDoubleTildeLeBA8FisBA8FvBA8F, -- Can't happen w/DoubleTildeLe

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonDoubleTildeLeB64isB64, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnMonDoubleTildeLeB32isB32, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnMonDoubleTildeLeB16isB16, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnMonDoubleTildeLeB08isB08, -- Can't happen w/DoubleTildeLe

    // Dyadic Boolean chunk functions
    NULL,   // &PrimFnMonDoubleTildeLeB64isB64vB64, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnMonDoubleTildeLeB32isB32vB32, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnMonDoubleTildeLeB16isB16vB16, -- Can't happen w/DoubleTildeLe
    NULL,   // &PrimFnMonDoubleTildeLeB08isB08vB08, -- Can't happen w/DoubleTildeLe

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpecLe = {&PrimSpecDoubleTildeLe};
#endif


#ifndef PROTO
PRIMSPEC PrimSpecDoubleTildeEq =
{
    // Monadic functions
    NULL,   // &PrimFnMon_EM_YY, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimSpecEqualStorageTypeMon, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqAPA_EM, -- Monadic DoubleTildeEq not scalar

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonDoubleTildeEqBisB, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqBisI, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqBisF, -- Monadic DoubleTildeEq not scalar

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeEqIisI, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqIisF, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqFisI, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqFisF, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqRisR, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqVisR, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqVisV, -- Monadic DoubleTildeEq not scalar

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonDoubleTildeEqHC2IisHC2I, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqHC2IisHC2F, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqHC2FisHC2I, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqHC2FisHC2F, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqHC2RisHC2R, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqHC2VisHC2R, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqHC2VisHC2V, -- Monadic DoubleTildeEq not scalar

    NULL,   // &PrimFnMonDoubleTildeEqHC4IisHC4I, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqHC4IisHC4F, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqHC4FisHC4I, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqHC4FisHC4F, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqHC4RisHC4R, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqHC4VisHC4R, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqHC4VisHC4V, -- Monadic DoubleTildeEq not scalar

    NULL,   // &PrimFnMonDoubleTildeEqHC8IisHC8I, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqHC8IisHC8F, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqHC8FisHC8I, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqHC8FisHC8F, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqHC8RisHC8R, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqHC8VisHC8R, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqHC8VisHC8V, -- Monadic DoubleTildeEq not scalar

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeEqFisHC2I, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqFisHC2F, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqVisHC2R, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqVisHC2V, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqFisHC4I, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqFisHC4F, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqVisHC4R, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqVisHC4V, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqFisHC8I, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqFisHC8F, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqVisHC8R, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqVisHC8V, -- Monadic DoubleTildeEq not scalar

    // Monadic BAxF result BAxF arg functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeEqBA1FisBA1F, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqBA2FisBA2F, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqBA4FisBA4F, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqBA8FisBA8F, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqBA1FisBA1F, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqARBisBA2F, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqARBisBA4F, -- Monadic DoubleTildeEq not scalar
    NULL,   // &PrimFnMonDoubleTildeEqARBisBA8F, -- Monadic DoubleTildeEq not scalar

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecEqualStorageTypeDyd,
    NULL,   // &PrimFnDydDoubleTildeEqAPA_EM,

    // Dyadic Boolean result functions
    NULL,   // &PrimFnDydDoubleTildeEqBisBvB, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqBisIvI, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqBisFvF, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqBisCvC, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqBisHvH, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqBisRvR, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqBisVvV, -- Can't happen w/DoubleTildeEq

    NULL,   // &PrimFnDydDoubleTildeEqBisHC2IvHC2I, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqBisHC2FvHC2F, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqBisHC2RvHC2R, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqBisHC2VvHC2V, -- Can't happen w/DoubleTildeEq

    NULL,   // &PrimFnDydDoubleTildeEqBisHC4IvHC4I, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqBisHC4FvHC4F, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqBisHC4RvHC4R, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqBisHC4VvHC4V, -- Can't happen w/DoubleTildeEq

    NULL,   // &PrimFnDydDoubleTildeEqBisHC8IvHC8I, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqBisHC8FvHC8F, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqBisHC8RvHC8R, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqBisHC8VvHC8V, -- Can't happen w/DoubleTildeEq

    // Dyadic Boolean result functions with ARBs (indexable)
    &PrimFnDydDoubleTildeEqBisBA1FvBA1F,
    &PrimFnDydDoubleTildeEqBisBA2FvBA2F,
    &PrimFnDydDoubleTildeEqBisBA4FvBA4F,
    &PrimFnDydDoubleTildeEqBisBA8FvBA8F,

    // Dyadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnDydDoubleTildeEqIisIvI, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqIisFvF, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqFisIvI, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqFisFvF, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqRisRvR, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqVisRvR, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqVisVvV, -- Can't happen w/DoubleTildeEq

    NULL,   // &PrimFnDydDoubleTildeEqHC2IisHC2IvHC2I, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqHC2IisHC2FvHC2F, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqHC2FisHC2IvHC2I, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqHC2FisHC2FvHC2F, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqHC2RisHC2RvHC2R, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqHC2VisHC2RvHC2R, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqHC2VisHC2VvHC2V, -- Can't happen w/DoubleTildeEq

    NULL,   // &PrimFnDydDoubleTildeEqHC4IisHC4IvHC4I, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqHC4IisHC4FvHC4F, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqHC4FisHC4IvHC4I, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqHC4FisHC4FvHC4F, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqHC4RisHC4RvHC4R, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqHC4VisHC4RvHC4R, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqHC4VisHC4VvHC4V, -- Can't happen w/DoubleTildeEq

    NULL,   // &PrimFnDydDoubleTildeEqHC8IisHC8IvHC8I, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqHC8IisHC8FvHC8F, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqHC8FisHC8IvHC8I, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqHC8FisHC8FvHC8F, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqHC8RisHC8RvHC8R, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqHC8VisHC8RvHC8R, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqHC8VisHC8VvHC8V, -- Can't happen w/DoubleTildeEq

    // Dyadic BAxF result functions (indexable)
    NULL,   // &PrimFnDydDoubleTildeEqBA1FisBA1FvBA1F, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqBA2FisBA2FvBA2F, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqBA4FisBA4FvBA4F, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnDydDoubleTildeEqBA8FisBA8FvBA8F, -- Can't happen w/DoubleTildeEq

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonDoubleTildeEqB64isB64, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnMonDoubleTildeEqB32isB32, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnMonDoubleTildeEqB16isB16, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnMonDoubleTildeEqB08isB08, -- Can't happen w/DoubleTildeEq

    // Dyadic Boolean chunk functions
    NULL,   // &PrimFnMonDoubleTildeEqB64isB64vB64, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnMonDoubleTildeEqB32isB32vB32, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnMonDoubleTildeEqB16isB16vB16, -- Can't happen w/DoubleTildeEq
    NULL,   // &PrimFnMonDoubleTildeEqB08isB08vB08, -- Can't happen w/DoubleTildeEq

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpecEq = {&PrimSpecDoubleTildeEq};
#endif


#ifndef PROTO
PRIMSPEC PrimSpecDoubleTildeGe =
{
    // Monadic functions
    NULL,   // &PrimFnMon_EM_YY, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimSpecRightCaretUnderbarStorageTypeMon, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeAPA_EM, -- Monadic DoubleTildeGe not scalar

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonDoubleTildeGeBisB, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeBisI, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeBisF, -- Monadic DoubleTildeGe not scalar

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeGeIisI, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeIisF, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeFisI, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeFisF, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeRisR, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeVisR, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeVisV, -- Monadic DoubleTildeGe not scalar

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonDoubleTildeGeHC2IisHC2I, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeHC2IisHC2F, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeHC2FisHC2I, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeHC2FisHC2F, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeHC2RisHC2R, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeHC2VisHC2R, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeHC2VisHC2V, -- Monadic DoubleTildeGe not scalar

    NULL,   // &PrimFnMonDoubleTildeGeHC4IisHC4I, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeHC4IisHC4F, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeHC4FisHC4I, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeHC4FisHC4F, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeHC4RisHC4R, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeHC4VisHC4R, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeHC4VisHC4V, -- Monadic DoubleTildeGe not scalar

    NULL,   // &PrimFnMonDoubleTildeGeHC8IisHC8I, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeHC8IisHC8F, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeHC8FisHC8I, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeHC8FisHC8F, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeHC8RisHC8R, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeHC8VisHC8R, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeHC8VisHC8V, -- Monadic DoubleTildeGe not scalar

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeGeFisHC2I, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeFisHC2F, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeVisHC2R, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeVisHC2V, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeFisHC4I, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeFisHC4F, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeVisHC4R, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeVisHC4V, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeFisHC8I, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeFisHC8F, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeVisHC8R, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeVisHC8V, -- Monadic DoubleTildeGe not scalar

    // Monadic BAxF result BAxF arg functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeGeBA1FisBA1F, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeBA2FisBA2F, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeBA4FisBA4F, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeBA8FisBA8F, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeBA1FisBA1F, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeARBisBA2F, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeARBisBA4F, -- Monadic DoubleTildeGe not scalar
    NULL,   // &PrimFnMonDoubleTildeGeARBisBA8F, -- Monadic DoubleTildeGe not scalar

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecRightCaretUnderbarStorageTypeDyd,
    NULL,   // &PrimFnDydDoubleTildeGeAPA_EM,

    // Dyadic Boolean result functions
    NULL,   // &PrimFnDydDoubleTildeGeBisBvB, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeBisIvI, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeBisFvF, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeBisCvC, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeBisHvH, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeBisRvR, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeBisVvV, -- Can't happen w/DoubleTildeGe

    NULL,   // &PrimFnDydDoubleTildeGeBisHC2IvHC2I, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeBisHC2FvHC2F, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeBisHC2RvHC2R, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeBisHC2VvHC2V, -- Can't happen w/DoubleTildeGe

    NULL,   // &PrimFnDydDoubleTildeGeBisHC4IvHC4I, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeBisHC4FvHC4F, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeBisHC4RvHC4R, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeBisHC4VvHC4V, -- Can't happen w/DoubleTildeGe

    NULL,   // &PrimFnDydDoubleTildeGeBisHC8IvHC8I, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeBisHC8FvHC8F, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeBisHC8RvHC8R, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeBisHC8VvHC8V, -- Can't happen w/DoubleTildeGe

    // Dyadic Boolean result functions with ARBs (indexable)
    &PrimFnDydDoubleTildeGeBisBA1FvBA1F,
    NULL,   // &PrimFnDydDoubleTildeGeBisBA2FvBA2F, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeBisBA4FvBA4F, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeBisBA8FvBA8F, -- Can't happen w/DoubleTildeGe

    // Dyadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnDydDoubleTildeGeIisIvI, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeIisFvF, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeFisIvI, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeFisFvF, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeRisRvR, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeVisRvR, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeVisVvV, -- Can't happen w/DoubleTildeGe

    NULL,   // &PrimFnDydDoubleTildeGeHC2IisHC2IvHC2I, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeHC2IisHC2FvHC2F, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeHC2FisHC2IvHC2I, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeHC2FisHC2FvHC2F, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeHC2RisHC2RvHC2R, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeHC2VisHC2RvHC2R, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeHC2VisHC2VvHC2V, -- Can't happen w/DoubleTildeGe

    NULL,   // &PrimFnDydDoubleTildeGeHC4IisHC4IvHC4I, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeHC4IisHC4FvHC4F, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeHC4FisHC4IvHC4I, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeHC4FisHC4FvHC4F, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeHC4RisHC4RvHC4R, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeHC4VisHC4RvHC4R, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeHC4VisHC4VvHC4V, -- Can't happen w/DoubleTildeGe

    NULL,   // &PrimFnDydDoubleTildeGeHC8IisHC8IvHC8I, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeHC8IisHC8FvHC8F, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeHC8FisHC8IvHC8I, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeHC8FisHC8FvHC8F, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeHC8RisHC8RvHC8R, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeHC8VisHC8RvHC8R, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeHC8VisHC8VvHC8V, -- Can't happen w/DoubleTildeGe

    // Dyadic BAxF result functions (indexable)
    NULL,   // &PrimFnDydDoubleTildeGeBA1FisBA1FvBA1F, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeBA2FisBA2FvBA2F, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeBA4FisBA4FvBA4F, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnDydDoubleTildeGeBA8FisBA8FvBA8F, -- Can't happen w/DoubleTildeGe

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonDoubleTildeGeB64isB64, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnMonDoubleTildeGeB32isB32, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnMonDoubleTildeGeB16isB16, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnMonDoubleTildeGeB08isB08, -- Can't happen w/DoubleTildeGe

    // Dyadic Boolean chunk functions
    NULL,   // &PrimFnMonDoubleTildeGeB64isB64vB64, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnMonDoubleTildeGeB32isB32vB32, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnMonDoubleTildeGeB16isB16vB16, -- Can't happen w/DoubleTildeGe
    NULL,   // &PrimFnMonDoubleTildeGeB08isB08vB08, -- Can't happen w/DoubleTildeGe

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpecGe = {&PrimSpecDoubleTildeGe};
#endif


#ifndef PROTO
PRIMSPEC PrimSpecDoubleTildeGt =
{
    // Monadic functions
    NULL,   // &PrimFnMon_EM_YY, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimSpecRightCaretStorageTypeMon, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtAPA_EM, -- Monadic DoubleTildeGt not scalar

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonDoubleTildeGtBisB, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtBisI, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtBisF, -- Monadic DoubleTildeGt not scalar

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeGtIisI, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtIisF, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtFisI, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtFisF, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtRisR, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtVisR, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtVisV, -- Monadic DoubleTildeGt not scalar

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonDoubleTildeGtHC2IisHC2I, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtHC2IisHC2F, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtHC2FisHC2I, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtHC2FisHC2F, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtHC2RisHC2R, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtHC2VisHC2R, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtHC2VisHC2V, -- Monadic DoubleTildeGt not scalar

    NULL,   // &PrimFnMonDoubleTildeGtHC4IisHC4I, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtHC4IisHC4F, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtHC4FisHC4I, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtHC4FisHC4F, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtHC4RisHC4R, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtHC4VisHC4R, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtHC4VisHC4V, -- Monadic DoubleTildeGt not scalar

    NULL,   // &PrimFnMonDoubleTildeGtHC8IisHC8I, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtHC8IisHC8F, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtHC8FisHC8I, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtHC8FisHC8F, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtHC8RisHC8R, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtHC8VisHC8R, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtHC8VisHC8V, -- Monadic DoubleTildeGt not scalar

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeGtFisHC2I, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtFisHC2F, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtVisHC2R, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtVisHC2V, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtFisHC4I, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtFisHC4F, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtVisHC4R, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtVisHC4V, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtFisHC8I, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtFisHC8F, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtVisHC8R, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtVisHC8V, -- Monadic DoubleTildeGt not scalar

    // Monadic BAxF result BAxF arg functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeGtBA1FisBA1F, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtBA2FisBA2F, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtBA4FisBA4F, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtBA8FisBA8F, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtBA1FisBA1F, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtARBisBA2F, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtARBisBA4F, -- Monadic DoubleTildeGt not scalar
    NULL,   // &PrimFnMonDoubleTildeGtARBisBA8F, -- Monadic DoubleTildeGt not scalar

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecRightCaretStorageTypeDyd,
    NULL,   // &PrimFnDydDoubleTildeGtAPA_EM,

    // Dyadic Boolean result functions
    NULL,   // &PrimFnDydDoubleTildeGtBisBvB, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtBisIvI, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtBisFvF, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtBisCvC, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtBisHvH, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtBisRvR, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtBisVvV, -- Can't happen w/DoubleTildeGt

    NULL,   // &PrimFnDydDoubleTildeGtBisHC2IvHC2I, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtBisHC2FvHC2F, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtBisHC2RvHC2R, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtBisHC2VvHC2V, -- Can't happen w/DoubleTildeGt

    NULL,   // &PrimFnDydDoubleTildeGtBisHC4IvHC4I, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtBisHC4FvHC4F, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtBisHC4RvHC4R, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtBisHC4VvHC4V, -- Can't happen w/DoubleTildeGt

    NULL,   // &PrimFnDydDoubleTildeGtBisHC8IvHC8I, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtBisHC8FvHC8F, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtBisHC8RvHC8R, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtBisHC8VvHC8V, -- Can't happen w/DoubleTildeGt

    // Dyadic Boolean result functions with ARBs (indexable)
    &PrimFnDydDoubleTildeGtBisBA1FvBA1F,
    NULL,   // &PrimFnDydDoubleTildeGtBisBA2FvBA2F, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtBisBA4FvBA4F, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtBisBA8FvBA8F, -- Can't happen w/DoubleTildeGt

    // Dyadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnDydDoubleTildeGtIisIvI, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtIisFvF, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtFisIvI, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtFisFvF, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtRisRvR, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtVisRvR, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtVisVvV, -- Can't happen w/DoubleTildeGt

    NULL,   // &PrimFnDydDoubleTildeGtHC2IisHC2IvHC2I, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtHC2IisHC2FvHC2F, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtHC2FisHC2IvHC2I, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtHC2FisHC2FvHC2F, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtHC2RisHC2RvHC2R, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtHC2VisHC2RvHC2R, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtHC2VisHC2VvHC2V, -- Can't happen w/DoubleTildeGt

    NULL,   // &PrimFnDydDoubleTildeGtHC4IisHC4IvHC4I, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtHC4IisHC4FvHC4F, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtHC4FisHC4IvHC4I, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtHC4FisHC4FvHC4F, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtHC4RisHC4RvHC4R, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtHC4VisHC4RvHC4R, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtHC4VisHC4VvHC4V, -- Can't happen w/DoubleTildeGt

    NULL,   // &PrimFnDydDoubleTildeGtHC8IisHC8IvHC8I, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtHC8IisHC8FvHC8F, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtHC8FisHC8IvHC8I, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtHC8FisHC8FvHC8F, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtHC8RisHC8RvHC8R, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtHC8VisHC8RvHC8R, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtHC8VisHC8VvHC8V, -- Can't happen w/DoubleTildeGt

    // Dyadic BAxF result functions (indexable)
    NULL,   // &PrimFnDydDoubleTildeGtBA1FisBA1FvBA1F, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtBA2FisBA2FvBA2F, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtBA4FisBA4FvBA4F, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnDydDoubleTildeGtBA8FisBA8FvBA8F, -- Can't happen w/DoubleTildeGt

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonDoubleTildeGtB64isB64, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnMonDoubleTildeGtB32isB32, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnMonDoubleTildeGtB16isB16, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnMonDoubleTildeGtB08isB08, -- Can't happen w/DoubleTildeGt

    // Dyadic Boolean chunk functions
    NULL,   // &PrimFnMonDoubleTildeGtB64isB64vB64, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnMonDoubleTildeGtB32isB32vB32, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnMonDoubleTildeGtB16isB16vB16, -- Can't happen w/DoubleTildeGt
    NULL,   // &PrimFnMonDoubleTildeGtB08isB08vB08, -- Can't happen w/DoubleTildeGt

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpecGt = {&PrimSpecDoubleTildeGt};
#endif


#ifndef PROTO
PRIMSPEC PrimSpecDoubleTildeNe =
{
    // Monadic functions
    NULL,   // &PrimFnMon_EM_YY, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimSpecNotEqualStorageTypeMon, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeAPA_EM, -- Monadic DoubleTildeNe not scalar

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonDoubleTildeNeBisB, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeBisI, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeBisF, -- Monadic DoubleTildeNe not scalar

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeNeIisI, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeIisF, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeFisI, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeFisF, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeRisR, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeVisR, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeVisV, -- Monadic DoubleTildeNe not scalar

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonDoubleTildeNeHC2IisHC2I, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeHC2IisHC2F, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeHC2FisHC2I, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeHC2FisHC2F, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeHC2RisHC2R, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeHC2VisHC2R, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeHC2VisHC2V, -- Monadic DoubleTildeNe not scalar

    NULL,   // &PrimFnMonDoubleTildeNeHC4IisHC4I, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeHC4IisHC4F, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeHC4FisHC4I, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeHC4FisHC4F, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeHC4RisHC4R, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeHC4VisHC4R, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeHC4VisHC4V, -- Monadic DoubleTildeNe not scalar

    NULL,   // &PrimFnMonDoubleTildeNeHC8IisHC8I, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeHC8IisHC8F, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeHC8FisHC8I, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeHC8FisHC8F, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeHC8RisHC8R, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeHC8VisHC8R, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeHC8VisHC8V, -- Monadic DoubleTildeNe not scalar

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeNeFisHC2I, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeFisHC2F, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeVisHC2R, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeVisHC2V, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeFisHC4I, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeFisHC4F, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeVisHC4R, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeVisHC4V, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeFisHC8I, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeFisHC8F, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeVisHC8R, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeVisHC8V, -- Monadic DoubleTildeNe not scalar

    // Monadic BAxF result BAxF arg functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeNeBA1FisBA1F, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeBA2FisBA2F, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeBA4FisBA4F, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeBA8FisBA8F, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeBA1FisBA1F, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeARBisBA2F, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeARBisBA4F, -- Monadic DoubleTildeNe not scalar
    NULL,   // &PrimFnMonDoubleTildeNeARBisBA8F, -- Monadic DoubleTildeNe not scalar

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecNotEqualStorageTypeDyd,
    NULL,   // &PrimFnDydDoubleTildeNeAPA_EM,

    // Dyadic Boolean result functions
    NULL,   // &PrimFnDydDoubleTildeNeBisBvB, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeBisIvI, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeBisFvF, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeBisCvC, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeBisHvH, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeBisRvR, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeBisVvV, -- Can't happen w/DoubleTildeNe

    NULL,   // &PrimFnDydDoubleTildeNeBisHC2IvHC2I, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeBisHC2FvHC2F, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeBisHC2RvHC2R, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeBisHC2VvHC2V, -- Can't happen w/DoubleTildeNe

    NULL,   // &PrimFnDydDoubleTildeNeBisHC4IvHC4I, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeBisHC4FvHC4F, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeBisHC4RvHC4R, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeBisHC4VvHC4V, -- Can't happen w/DoubleTildeNe

    NULL,   // &PrimFnDydDoubleTildeNeBisHC8IvHC8I, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeBisHC8FvHC8F, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeBisHC8RvHC8R, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeBisHC8VvHC8V, -- Can't happen w/DoubleTildeNe

    // Dyadic Boolean result functions with ARBs (indexable)
    &PrimFnDydDoubleTildeNeBisBA1FvBA1F,
    &PrimFnDydDoubleTildeNeBisBA2FvBA2F,
    &PrimFnDydDoubleTildeNeBisBA4FvBA4F,
    &PrimFnDydDoubleTildeNeBisBA8FvBA8F,

    // Dyadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnDydDoubleTildeNeIisIvI, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeIisFvF, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeFisIvI, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeFisFvF, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeRisRvR, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeVisRvR, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeVisVvV, -- Can't happen w/DoubleTildeNe

    NULL,   // &PrimFnDydDoubleTildeNeHC2IisHC2IvHC2I, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeHC2IisHC2FvHC2F, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeHC2FisHC2IvHC2I, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeHC2FisHC2FvHC2F, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeHC2RisHC2RvHC2R, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeHC2VisHC2RvHC2R, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeHC2VisHC2VvHC2V, -- Can't happen w/DoubleTildeNe

    NULL,   // &PrimFnDydDoubleTildeNeHC4IisHC4IvHC4I, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeHC4IisHC4FvHC4F, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeHC4FisHC4IvHC4I, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeHC4FisHC4FvHC4F, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeHC4RisHC4RvHC4R, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeHC4VisHC4RvHC4R, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeHC4VisHC4VvHC4V, -- Can't happen w/DoubleTildeNe

    NULL,   // &PrimFnDydDoubleTildeNeHC8IisHC8IvHC8I, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeHC8IisHC8FvHC8F, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeHC8FisHC8IvHC8I, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeHC8FisHC8FvHC8F, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeHC8RisHC8RvHC8R, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeHC8VisHC8RvHC8R, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeHC8VisHC8VvHC8V, -- Can't happen w/DoubleTildeNe

    // Dyadic BAxF result functions (indexable)
    NULL,   // &PrimFnDydDoubleTildeNeBA1FisBA1FvBA1F, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeBA2FisBA2FvBA2F, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeBA4FisBA4FvBA4F, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnDydDoubleTildeNeBA8FisBA8FvBA8F, -- Can't happen w/DoubleTildeNe

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonDoubleTildeNeB64isB64, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnMonDoubleTildeNeB32isB32, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnMonDoubleTildeNeB16isB16, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnMonDoubleTildeNeB08isB08, -- Can't happen w/DoubleTildeNe

    // Dyadic Boolean chunk functions
    NULL,   // &PrimFnMonDoubleTildeNeB64isB64vB64, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnMonDoubleTildeNeB32isB32vB32, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnMonDoubleTildeNeB16isB16vB16, -- Can't happen w/DoubleTildeNe
    NULL,   // &PrimFnMonDoubleTildeNeB08isB08vB08, -- Can't happen w/DoubleTildeNe

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpecNe = {&PrimSpecDoubleTildeNe};
#endif


#ifndef PROTO
PRIMSPEC PrimSpecDoubleTildeLs =
{
    // Monadic functions
    NULL,   // &PrimFnMon_EM_YY, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimSpecLsStorageTypeMon, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsAPA_EM, -- Monadic DoubleTildeLs not scalar

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonDoubleTildeLsBisB, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsBisI, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsBisF, -- Monadic DoubleTildeLs not scalar

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeLsIisI, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsIisF, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsFisI, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsFisF, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsRisR, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsVisR, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsVisV, -- Monadic DoubleTildeLs not scalar

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonDoubleTildeLsHC2IisHC2I, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsHC2IisHC2F, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsHC2FisHC2I, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsHC2FisHC2F, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsHC2RisHC2R, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsHC2VisHC2R, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsHC2VisHC2V, -- Monadic DoubleTildeLs not scalar

    NULL,   // &PrimFnMonDoubleTildeLsHC4IisHC4I, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsHC4IisHC4F, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsHC4FisHC4I, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsHC4FisHC4F, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsHC4RisHC4R, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsHC4VisHC4R, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsHC4VisHC4V, -- Monadic DoubleTildeLs not scalar

    NULL,   // &PrimFnMonDoubleTildeLsHC8IisHC8I, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsHC8IisHC8F, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsHC8FisHC8I, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsHC8FisHC8F, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsHC8RisHC8R, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsHC8VisHC8R, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsHC8VisHC8V, -- Monadic DoubleTildeLs not scalar

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeLsFisHC2I, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsFisHC2F, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsVisHC2R, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsVisHC2V, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsFisHC4I, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsFisHC4F, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsVisHC4R, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsVisHC4V, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsFisHC8I, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsFisHC8F, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsVisHC8R, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsVisHC8V, -- Monadic DoubleTildeLs not scalar

    // Monadic BAxF result BAxF arg functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeLsBA1FisBA1F, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsBA2FisBA2F, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsBA4FisBA4F, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsBA8FisBA8F, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsBA1FisBA1F, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsARBisBA2F, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsARBisBA4F, -- Monadic DoubleTildeLs not scalar
    NULL,   // &PrimFnMonDoubleTildeLsARBisBA8F, -- Monadic DoubleTildeLs not scalar

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecDoubleTildeLsRsStorageTypeDyd,
    NULL,   // &PrimFnDydDoubleTildeLsAPA_EM,

    // Dyadic Boolean result functions
    NULL,   // &PrimFnDydDoubleTildeLsBisBvB, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsBisIvI, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsBisFvF, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsBisCvC, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsBisHvH, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsBisRvR, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsBisVvV, -- Can't happen w/DoubleTildeLs

    NULL,   // &PrimFnDydDoubleTildeLsBisHC2IvHC2I, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsBisHC2FvHC2F, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsBisHC2RvHC2R, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsBisHC2VvHC2V, -- Can't happen w/DoubleTildeLs

    NULL,   // &PrimFnDydDoubleTildeLsBisHC4IvHC4I, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsBisHC4FvHC4F, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsBisHC4RvHC4R, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsBisHC4VvHC4V, -- Can't happen w/DoubleTildeLs

    NULL,   // &PrimFnDydDoubleTildeLsBisHC8IvHC8I, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsBisHC8FvHC8F, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsBisHC8RvHC8R, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsBisHC8VvHC8V, -- Can't happen w/DoubleTildeLs

    // Dyadic Boolean result functions with ARBs (indexable)
    &PrimFnDydDoubleTildeLsBisBA1FvBA1F,
    NULL,   // &PrimFnDydDoubleTildeLsBisBA2FvBA2F, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsBisBA4FvBA4F, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsBisBA8FvBA8F, -- Can't happen w/DoubleTildeLs

    // Dyadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnDydDoubleTildeLsIisIvI, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsIisFvF, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsFisIvI, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsFisFvF, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsRisRvR, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsVisRvR, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsVisVvV, -- Can't happen w/DoubleTildeLs

    NULL,   // &PrimFnDydDoubleTildeLsHC2IisHC2IvHC2I, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsHC2IisHC2FvHC2F, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsHC2FisHC2IvHC2I, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsHC2FisHC2FvHC2F, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsHC2RisHC2RvHC2R, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsHC2VisHC2RvHC2R, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsHC2VisHC2VvHC2V, -- Can't happen w/DoubleTildeLs

    NULL,   // &PrimFnDydDoubleTildeLsHC4IisHC4IvHC4I, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsHC4IisHC4FvHC4F, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsHC4FisHC4IvHC4I, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsHC4FisHC4FvHC4F, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsHC4RisHC4RvHC4R, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsHC4VisHC4RvHC4R, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsHC4VisHC4VvHC4V, -- Can't happen w/DoubleTildeLs

    NULL,   // &PrimFnDydDoubleTildeLsHC8IisHC8IvHC8I, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsHC8IisHC8FvHC8F, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsHC8FisHC8IvHC8I, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsHC8FisHC8FvHC8F, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsHC8RisHC8RvHC8R, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsHC8VisHC8RvHC8R, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsHC8VisHC8VvHC8V, -- Can't happen w/DoubleTildeLs

    // Dyadic BAxF result functions (indexable)
    NULL,   // &PrimFnDydDoubleTildeLsBA1FisBA1FvBA1F, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsBA2FisBA2FvBA2F, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsBA4FisBA4FvBA4F, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnDydDoubleTildeLsBA8FisBA8FvBA8F, -- Can't happen w/DoubleTildeLs

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonDoubleTildeLsB64isB64, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnMonDoubleTildeLsB32isB32, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnMonDoubleTildeLsB16isB16, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnMonDoubleTildeLsB08isB08, -- Can't happen w/DoubleTildeLs

    // Dyadic Boolean chunk functions
    NULL,   // &PrimFnMonDoubleTildeLsB64isB64vB64, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnMonDoubleTildeLsB32isB32vB32, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnMonDoubleTildeLsB16isB16vB16, -- Can't happen w/DoubleTildeLs
    NULL,   // &PrimFnMonDoubleTildeLsB08isB08vB08, -- Can't happen w/DoubleTildeLs

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpecLs = {&PrimSpecDoubleTildeLs};
#endif


#ifndef PROTO
PRIMSPEC PrimSpecDoubleTildeRs =
{
    // Monadic functions
    NULL,   // &PrimFnMon_EM_YY, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimSpecRsStorageTypeMon, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsAPA_EM, -- Monadic DoubleTildeRs not scalar

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonDoubleTildeRsBisB, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsBisI, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsBisF, -- Monadic DoubleTildeRs not scalar

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeRsIisI, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsIisF, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsFisI, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsFisF, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsRisR, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsVisR, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsVisV, -- Monadic DoubleTildeRs not scalar

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonDoubleTildeRsHC2IisHC2I, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsHC2IisHC2F, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsHC2FisHC2I, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsHC2FisHC2F, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsHC2RisHC2R, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsHC2VisHC2R, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsHC2VisHC2V, -- Monadic DoubleTildeRs not scalar

    NULL,   // &PrimFnMonDoubleTildeRsHC4IisHC4I, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsHC4IisHC4F, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsHC4FisHC4I, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsHC4FisHC4F, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsHC4RisHC4R, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsHC4VisHC4R, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsHC4VisHC4V, -- Monadic DoubleTildeRs not scalar

    NULL,   // &PrimFnMonDoubleTildeRsHC8IisHC8I, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsHC8IisHC8F, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsHC8FisHC8I, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsHC8FisHC8F, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsHC8RisHC8R, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsHC8VisHC8R, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsHC8VisHC8V, -- Monadic DoubleTildeRs not scalar

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeRsFisHC2I, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsFisHC2F, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsVisHC2R, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsVisHC2V, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsFisHC4I, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsFisHC4F, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsVisHC4R, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsVisHC4V, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsFisHC8I, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsFisHC8F, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsVisHC8R, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsVisHC8V, -- Monadic DoubleTildeRs not scalar

    // Monadic BAxF result BAxF arg functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeRsBA1FisBA1F, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsBA2FisBA2F, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsBA4FisBA4F, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsBA8FisBA8F, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsBA1FisBA1F, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsARBisBA2F, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsARBisBA4F, -- Monadic DoubleTildeRs not scalar
    NULL,   // &PrimFnMonDoubleTildeRsARBisBA8F, -- Monadic DoubleTildeRs not scalar

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecDoubleTildeLsRsStorageTypeDyd,
    NULL,   // &PrimFnDydDoubleTildeRsAPA_EM,

    // Dyadic Boolean result functions
    NULL,   // &PrimFnDydDoubleTildeRsBisBvB, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsBisIvI, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsBisFvF, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsBisCvC, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsBisHvH, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsBisRvR, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsBisVvV, -- Can't happen w/DoubleTildeRs

    NULL,   // &PrimFnDydDoubleTildeRsBisHC2IvHC2I, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsBisHC2FvHC2F, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsBisHC2RvHC2R, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsBisHC2VvHC2V, -- Can't happen w/DoubleTildeRs

    NULL,   // &PrimFnDydDoubleTildeRsBisHC4IvHC4I, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsBisHC4FvHC4F, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsBisHC4RvHC4R, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsBisHC4VvHC4V, -- Can't happen w/DoubleTildeRs

    NULL,   // &PrimFnDydDoubleTildeRsBisHC8IvHC8I, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsBisHC8FvHC8F, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsBisHC8RvHC8R, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsBisHC8VvHC8V, -- Can't happen w/DoubleTildeRs

    // Dyadic Boolean result functions with ARBs (indexable)
    &PrimFnDydDoubleTildeRsBisBA1FvBA1F,
    NULL,   // &PrimFnDydDoubleTildeRsBisBA2FvBA2F, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsBisBA4FvBA4F, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsBisBA8FvBA8F, -- Can't happen w/DoubleTildeRs

    // Dyadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnDydDoubleTildeRsIisIvI, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsIisFvF, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsFisIvI, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsFisFvF, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsRisRvR, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsVisRvR, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsVisVvV, -- Can't happen w/DoubleTildeRs

    NULL,   // &PrimFnDydDoubleTildeRsHC2IisHC2IvHC2I, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsHC2IisHC2FvHC2F, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsHC2FisHC2IvHC2I, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsHC2FisHC2FvHC2F, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsHC2RisHC2RvHC2R, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsHC2VisHC2RvHC2R, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsHC2VisHC2VvHC2V, -- Can't happen w/DoubleTildeRs

    NULL,   // &PrimFnDydDoubleTildeRsHC4IisHC4IvHC4I, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsHC4IisHC4FvHC4F, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsHC4FisHC4IvHC4I, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsHC4FisHC4FvHC4F, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsHC4RisHC4RvHC4R, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsHC4VisHC4RvHC4R, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsHC4VisHC4VvHC4V, -- Can't happen w/DoubleTildeRs

    NULL,   // &PrimFnDydDoubleTildeRsHC8IisHC8IvHC8I, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsHC8IisHC8FvHC8F, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsHC8FisHC8IvHC8I, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsHC8FisHC8FvHC8F, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsHC8RisHC8RvHC8R, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsHC8VisHC8RvHC8R, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsHC8VisHC8VvHC8V, -- Can't happen w/DoubleTildeRs

    // Dyadic BAxF result functions (indexable)
    NULL,   // &PrimFnDydDoubleTildeRsBA1FisBA1FvBA1F, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsBA2FisBA2FvBA2F, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsBA4FisBA4FvBA4F, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnDydDoubleTildeRsBA8FisBA8FvBA8F, -- Can't happen w/DoubleTildeRs

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonDoubleTildeRsB64isB64, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnMonDoubleTildeRsB32isB32, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnMonDoubleTildeRsB16isB16, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnMonDoubleTildeRsB08isB08, -- Can't happen w/DoubleTildeRs

    // Dyadic Boolean chunk functions
    NULL,   // &PrimFnMonDoubleTildeRsB64isB64vB64, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnMonDoubleTildeRsB32isB32vB32, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnMonDoubleTildeRsB16isB16vB16, -- Can't happen w/DoubleTildeRs
    NULL,   // &PrimFnMonDoubleTildeRsB08isB08vB08, -- Can't happen w/DoubleTildeRs

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpecRs = {&PrimSpecDoubleTildeRs};
#endif


#ifndef PROTO
PRIMSPEC PrimSpecDoubleTildeJot =
{
    // Monadic functions
    NULL,   // &PrimFnMon_EM_YY, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimSpecJotStorageTypeMon, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotAPA_EM, -- Monadic DoubleTildeJot not scalar

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonDoubleTildeJotBisB, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotBisI, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotBisF, -- Monadic DoubleTildeJot not scalar

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeJotIisI, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotIisF, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotFisI, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotFisF, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotRisR, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotVisR, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotVisV, -- Monadic DoubleTildeJot not scalar

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonDoubleTildeJotHC2IisHC2I, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotHC2IisHC2F, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotHC2FisHC2I, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotHC2FisHC2F, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotHC2RisHC2R, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotHC2VisHC2R, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotHC2VisHC2V, -- Monadic DoubleTildeJot not scalar

    NULL,   // &PrimFnMonDoubleTildeJotHC4IisHC4I, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotHC4IisHC4F, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotHC4FisHC4I, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotHC4FisHC4F, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotHC4RisHC4R, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotHC4VisHC4R, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotHC4VisHC4V, -- Monadic DoubleTildeJot not scalar

    NULL,   // &PrimFnMonDoubleTildeJotHC8IisHC8I, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotHC8IisHC8F, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotHC8FisHC8I, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotHC8FisHC8F, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotHC8RisHC8R, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotHC8VisHC8R, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotHC8VisHC8V, -- Monadic DoubleTildeJot not scalar

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeJotFisHC2I, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotFisHC2F, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotVisHC2R, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotVisHC2V, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotFisHC4I, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotFisHC4F, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotVisHC4R, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotVisHC4V, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotFisHC8I, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotFisHC8F, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotVisHC8R, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotVisHC8V, -- Monadic DoubleTildeJot not scalar

    // Monadic BAxF result BAxF arg functions (indexable)
    NULL,   // &PrimFnMonDoubleTildeJotBA1FisBA1F, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotBA2FisBA2F, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotBA4FisBA4F, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotBA8FisBA8F, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotBA1FisBA1F, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotARBisBA2F, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotARBisBA4F, -- Monadic DoubleTildeJot not scalar
    NULL,   // &PrimFnMonDoubleTildeJotARBisBA8F, -- Monadic DoubleTildeJot not scalar

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecDoubleTildeJotStorageTypeDyd,
    NULL,   // &PrimFnDydDoubleTildeJotAPA_EM,

    // Dyadic Boolean result functions
    NULL,   // &PrimFnDydDoubleTildeJotBisBvB, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotBisIvI, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotBisFvF, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotBisCvC, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotBisHvH, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotBisRvR, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotBisVvV, -- Can't happen w/DoubleTildeJot

    NULL,   // &PrimFnDydDoubleTildeJotBisHC2IvHC2I, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotBisHC2FvHC2F, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotBisHC2RvHC2R, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotBisHC2VvHC2V, -- Can't happen w/DoubleTildeJot

    NULL,   // &PrimFnDydDoubleTildeJotBisHC4IvHC4I, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotBisHC4FvHC4F, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotBisHC4RvHC4R, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotBisHC4VvHC4V, -- Can't happen w/DoubleTildeJot

    NULL,   // &PrimFnDydDoubleTildeJotBisHC8IvHC8I, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotBisHC8FvHC8F, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotBisHC8RvHC8R, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotBisHC8VvHC8V, -- Can't happen w/DoubleTildeJot

    // Dyadic Boolean result functions with ARBs (indexable)
    NULL,   // &PrimFnDydDoubleTildeJotBisBA1FvBA1F, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotBisBA2FvBA2F, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotBisBA4FvBA4F, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotBisBA8FvBA8F, -- Can't happen w/DoubleTildeJot

    // Dyadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnDydDoubleTildeJotIisIvI, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotIisFvF, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotFisIvI, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotFisFvF, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotRisRvR, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotVisRvR, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotVisVvV, -- Can't happen w/DoubleTildeJot

    NULL,   // &PrimFnDydDoubleTildeJotHC2IisHC2IvHC2I, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotHC2IisHC2FvHC2F, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotHC2FisHC2IvHC2I, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotHC2FisHC2FvHC2F, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotHC2RisHC2RvHC2R, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotHC2VisHC2RvHC2R, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotHC2VisHC2VvHC2V, -- Can't happen w/DoubleTildeJot

    NULL,   // &PrimFnDydDoubleTildeJotHC4IisHC4IvHC4I, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotHC4IisHC4FvHC4F, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotHC4FisHC4IvHC4I, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotHC4FisHC4FvHC4F, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotHC4RisHC4RvHC4R, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotHC4VisHC4RvHC4R, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotHC4VisHC4VvHC4V, -- Can't happen w/DoubleTildeJot

    NULL,   // &PrimFnDydDoubleTildeJotHC8IisHC8IvHC8I, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotHC8IisHC8FvHC8F, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotHC8FisHC8IvHC8I, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotHC8FisHC8FvHC8F, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotHC8RisHC8RvHC8R, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotHC8VisHC8RvHC8R, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotHC8VisHC8VvHC8V, -- Can't happen w/DoubleTildeJot

    // Dyadic BAxF result functions (indexable)
    &PrimFnDydDoubleTildeJotBA1FisBA1FvBA1F,
    NULL,   // &PrimFnDydDoubleTildeJotBA2FisBA2FvBA2F, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotBA4FisBA4FvBA4F, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnDydDoubleTildeJotBA8FisBA8FvBA8F, -- Can't happen w/DoubleTildeJot

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonDoubleTildeJotB64isB64, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnMonDoubleTildeJotB32isB32, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnMonDoubleTildeJotB16isB16, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnMonDoubleTildeJotB08isB08, -- Can't happen w/DoubleTildeJot

    // Dyadic Boolean chunk functions
    NULL,   // &PrimFnMonDoubleTildeJotB64isB64vB64, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnMonDoubleTildeJotB32isB32vB32, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnMonDoubleTildeJotB16isB16vB16, -- Can't happen w/DoubleTildeJot
    NULL,   // &PrimFnMonDoubleTildeJotB08isB08vB08, -- Can't happen w/DoubleTildeJot

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpecJot = {&PrimSpecDoubleTildeJot};
#endif


//***************************************************************************
//  $PrimFnDoubleTildeInter_EM_YY
//
//  Primitive function for dyadic derived functions from
//    monadic operator DoubleTilde-Intersection
//***************************************************************************

LPPL_YYSTYPE PrimFnDoubleTildeInter_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
    else
        return (*lpPrimSpecInter->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpecInter);
} // End PrimFnDoubleTildeInter_EM_YY


//***************************************************************************
//  $PrimFnDoubleTildeUnion_EM_YY
//
//  Primitive function for dyadic derived functions from
//    monadic operator DoubleTilde-Unionsection
//***************************************************************************

LPPL_YYSTYPE PrimFnDoubleTildeUnion_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
    else
        return (*lpPrimSpecUnion->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpecUnion);
} // End PrimFnDoubleTildeUnion_EM_YY


//***************************************************************************
//  $PrimProtoFnDoubleTilde_EM_YY
//
//  Generate a prototype for the derived functions from
//    monadic operator DoubleTilde
//***************************************************************************

LPPL_YYSTYPE PrimProtoFnDoubleTilde_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    Assert (lptkAxis EQ NULL);

    //***************************************************************
    // Called dyadically only
    //***************************************************************

    return
      PrimOpDydDoubleTilde_EM_YY (lptkLftArg,           // Ptr to left arg token
                   (LPPL_YYSTYPE) lptkFunc,             // lpYYFcnStrOpr,     // Ptr to operator function strand
                                  lptkRhtArg,           // Ptr to right arg token
                                  TRUE);                // TRUE iff prototyping
} // End PrimProtoFnDoubleTilde_EM_YY


//***************************************************************************
//  $PrimIdentOpDoubleTildeInter_EM_YY
//
//  Generate an identity element for the dyadic derived function from the
//    primitive operator DoubleTilde-Intersection
//***************************************************************************

LPPL_YYSTYPE PrimIdentOpDoubleTildeInter_EM_YY
    (LPTOKEN      lptkRhtOrig,      // Ptr to original right arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,    // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,       // Ptr to right arg token
     LPTOKEN      lptkAxisOpr)      // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE      lpYYFcnStrLft,        // Ptr to left operand function strand
                      lpYYRes = NULL;       // Ptr to result
    HGLOBAL           hGlbRes = NULL;       // Result global memory handle
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to result array header
    LPAPLARB          lpMemRes;             // Ptr to result global memory data

    // The right arg is the prototype item from
    //   the original empty arg.

    Assert (lptkRhtOrig   NE NULL);
    Assert (lpYYFcnStrOpr NE NULL);
    Assert (lptkRhtArg    NE NULL);

    // The (left and right) identity element for dyadic DoubleTilde Intersection is
    //   0±{infinity}

    // Set ptr to left operand skipping over the operator and axis token (if present)
    lpYYFcnStrLft = GetMonLftOper (lpYYFcnStrOpr, lptkAxisOpr); Assert (lpYYFcnStrLft NE NULL);

    // Check for left operand axis operator
    if (CheckAxisOper (lpYYFcnStrLft) NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Ensure the left operand is a function
    //   and not a Jot
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken)
     || IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_OPERAND_DOMAIN_EXIT;

    // Allocate space for the result
    hGlbRes =
      AllocateGlobalArray (ARRAY_ARB, 1, 0, NULL);
    // Check for errors
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (hGlbRes);

    // Point to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    Myarb_init (lpMemRes);
    mag_inf (arb_radref (lpMemRes));

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;     // Already 0 from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;             // Already 0 from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

    goto NORMAL_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxisOpr);
    goto ERROR_EXIT;

LEFT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    return lpYYRes;
} // End PrimIdentOpDoubleTildeInter_EM_YY


//***************************************************************************
//  $PrimSpecDoubleTildeInterUnionStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecDoubleTildeInterUnionStorageTypeDyd
    (APLNELM    aplNELMLft,
     LPAPLSTYPE lpaplTypeLft,
     LPTOKEN    lptkFunc,
     APLNELM    aplNELMRht,
     LPAPLSTYPE lpaplTypeRht)

{
    APLSTYPE aplTypeRes;

    // In case the left arg is an empty char,
    //   change its type to BOOL
    if (IsCharEmpty (*lpaplTypeLft, aplNELMLft))
        *lpaplTypeLft = ARRAY_BOOL;

    // In case the right arg is an empty char,
    //   change its type to BOOL
    if (IsCharEmpty (*lpaplTypeRht, aplNELMRht))
        *lpaplTypeRht = ARRAY_BOOL;

    // Calculate the storage type of the result
    aplTypeRes = StorageType (*lpaplTypeLft, lptkFunc, *lpaplTypeRht);

    // If the result is numeric, ...
    if (IsNumeric (aplTypeRes))
    {
        UINT uHCDimIndex;

        // Convert iHCDimRes to uHCDimIndex
        uHCDimIndex = aHCDimToIndex[TranslateArrayTypeToHCDim (aplTypeRes)];

        // Make it ARB at the same dimension
        aplTypeRes = ARRAY_ARB + uHCDimIndex;
    } // End IF

    return aplTypeRes;
} // End PrimSpecDoubleTildeInterUnionStorageTypeDyd


//***************************************************************************
//  $ArbInterBA1F_RE
//***************************************************************************

APLBA1F ArbInterBA1F_RE
    (APLBA1F    aplLft,             // Left arg
     APLBA1F    aplRht,             // Right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLBA1F aplRes = {0};           // The result
    int     iRes;                   // The resul from arb_intersection
    mp_limb_signed_t prec = ARB_PREC_FPC;

    Assert (lpPrimSpec->iHCDimIndexLft EQ 0);
    Assert (lpPrimSpec->iHCDimIndexRht EQ 0);

    // Initialize the result to 0/0
    Myarb_init (&aplRes);

    // Calculate the intersection
    iRes =
      arb_intersection (&aplRes,
                        &aplLft,
                        &aplRht,
                         prec);
    // If the result is invalid, ...
    if (iRes EQ 0)
        // Set the result to NaN
        arf_nan (arb_midref (&aplRes));

    return aplRes;
} // End ArbInterBA1F_RE


//***************************************************************************
//  $PrimFnDydDoubleTildeInterBA1FisBA1FvBA1F
//
//  Primitive scalar function dyadic DoubleTildeInter:  BA1F {is} BA1F fn BA1F
//***************************************************************************

void PrimFnDydDoubleTildeInterBA1FisBA1FvBA1F
    (LPAPLARB   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = ArbInterBA1F_RE (lpatLft->aplArb, lpatRht->aplArb, lpPrimSpec);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%s#%d)", MyGetExceptionStr (exCode), WFNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydDoubleTildeInterBA1FisBA1FvBA1F


//***************************************************************************
//  $ArbInterBA2F_RE
//***************************************************************************

APLBA2F ArbInterBA2F_RE
    (APLBA2F    aplLft,             // Left arg
     APLBA2F    aplRht,             // Right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ARB_INTER_MAC (2)
} // End ArbInterBA2F_RE


//***************************************************************************
//  $PrimFnDydDoubleTildeInterBA2FisBA2FvBA2F
//
//  Primitive scalar function dyadic DoubleTildeInter:  BA2F {is} BA2F fn BA2F
//***************************************************************************

void PrimFnDydDoubleTildeInterBA2FisBA2FvBA2F
    (LPAPLBA2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = ArbInterBA2F_RE (lpatLft->aplBA2F, lpatRht->aplBA2F, lpPrimSpec);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%s#%d)", MyGetExceptionStr (exCode), WFNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydDoubleTildeInterBA2FisBA2FvBA2F


//***************************************************************************
//  $ArbInterBA4F_RE
//***************************************************************************

APLBA4F ArbInterBA4F_RE
    (APLBA4F    aplLft,             // Left arg
     APLBA4F    aplRht,             // Right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ARB_INTER_MAC (4)
} // End ArbInterBA4F_RE


//***************************************************************************
//  $PrimFnDydDoubleTildeInterBA4FisBA4FvBA4F
//
//  Primitive scalar function dyadic DoubleTildeInter:  BA4F {is} BA4F fn BA4F
//***************************************************************************

void PrimFnDydDoubleTildeInterBA4FisBA4FvBA4F
    (LPAPLBA4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = ArbInterBA4F_RE (lpatLft->aplBA4F, lpatRht->aplBA4F, lpPrimSpec);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%s#%d)", MyGetExceptionStr (exCode), WFNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydDoubleTildeInterBA4FisBA4FvBA4F


//***************************************************************************
//  $ArbInterBA8F_RE
//***************************************************************************

APLBA8F ArbInterBA8F_RE
    (APLBA8F    aplLft,             // Left arg
     APLBA8F    aplRht,             // Right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ARB_INTER_MAC (8)
} // End ArbInterBA8F_RE


//***************************************************************************
//  $PrimFnDydDoubleTildeInterBA8FisBA8FvBA8F
//
//  Primitive scalar function dyadic DoubleTildeInter:  BA8F {is} BA8F fn BA8F
//***************************************************************************

void PrimFnDydDoubleTildeInterBA8FisBA8FvBA8F
    (LPAPLBA8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = ArbInterBA8F_RE (lpatLft->aplBA8F, lpatRht->aplBA8F, lpPrimSpec);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%s#%d)", MyGetExceptionStr (exCode), WFNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydDoubleTildeInterBA8FisBA8FvBA8F


//***************************************************************************
//  $ArbUnionBA1F_RE
//***************************************************************************

APLBA1F ArbUnionBA1F_RE
    (APLBA1F    aplLft,             // Left arg
     APLBA1F    aplRht,             // Right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLBA1F aplRes = {0};           // The result
    mp_limb_signed_t prec = ARB_PREC_FPC;

    Assert (lpPrimSpec->iHCDimIndexLft EQ 0);
    Assert (lpPrimSpec->iHCDimIndexRht EQ 0);

    // Initialize the result to 0/0
    Myarb_init (&aplRes);

    // Calculate the Union
    arb_union (&aplRes,
               &aplLft,
               &aplRht,
                prec);
    return aplRes;
} // End ArbUnionBA1F_RE


//***************************************************************************
//  $PrimFnDydDoubleTildeUnionBA1FisBA1FvBA1F
//
//  Primitive scalar function dyadic DoubleTildeInter:  BA1F {is} BA1F fn BA1F
//***************************************************************************

void PrimFnDydDoubleTildeUnionBA1FisBA1FvBA1F
    (LPAPLARB   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = ArbUnionBA1F_RE (lpatLft->aplArb, lpatRht->aplArb, lpPrimSpec);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%s#%d)", MyGetExceptionStr (exCode), WFNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydDoubleTildeUnionBA1FisBA1FvBA1F


//***************************************************************************
//  $ArbUnionBA2F_RE
//***************************************************************************

APLBA2F ArbUnionBA2F_RE
    (APLBA2F    aplLft,             // Left arg
     APLBA2F    aplRht,             // Right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLBA2F aplRes = {0};
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Initialize to 0/0
    acb_init ((acb_ptr) &aplRes);

    acb_union ((acb_ptr) &aplRes, (acb_ptr) &aplLft, (acb_ptr) &aplRht, prec);

    return aplRes;

////ARB_UNION_MAC (2)
} // End ArbUnionBA2F_RE


//***************************************************************************
//  $PrimFnDydDoubleTildeUnionBA2FisBA2FvBA2F
//
//  Primitive scalar function dyadic DoubleTildeInter:  BA2F {is} BA2F fn BA2F
//***************************************************************************

void PrimFnDydDoubleTildeUnionBA2FisBA2FvBA2F
    (LPAPLBA2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = ArbUnionBA2F_RE (lpatLft->aplBA2F, lpatRht->aplBA2F, lpPrimSpec);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%s#%d)", MyGetExceptionStr (exCode), WFNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydDoubleTildeUnionBA2FisBA2FvBA2F


//***************************************************************************
//  $ArbUnionBA4F_RE
//***************************************************************************

APLBA4F ArbUnionBA4F_RE
    (APLBA4F    aplLft,             // Left arg
     APLBA4F    aplRht,             // Right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ARB_UNION_MAC (4)
} // End ArbUnionBA4F_RE


//***************************************************************************
//  $PrimFnDydDoubleTildeUnionBA4FisBA4FvBA4F
//
//  Primitive scalar function dyadic DoubleTildeInter:  BA4F {is} BA4F fn BA4F
//***************************************************************************

void PrimFnDydDoubleTildeUnionBA4FisBA4FvBA4F
    (LPAPLBA4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = ArbUnionBA4F_RE (lpatLft->aplBA4F, lpatRht->aplBA4F, lpPrimSpec);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%s#%d)", MyGetExceptionStr (exCode), WFNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydDoubleTildeUnionBA4FisBA4FvBA4F


//***************************************************************************
//  $ArbUnionBA8F_RE
//***************************************************************************

APLBA8F ArbUnionBA8F_RE
    (APLBA8F    aplLft,             // Left arg
     APLBA8F    aplRht,             // Right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ARB_UNION_MAC (8)
} // End ArbUnionBA8F_RE


//***************************************************************************
//  $PrimFnDydDoubleTildeUnionBA8FisBA8FvBA8F
//
//  Primitive scalar function dyadic DoubleTildeInter:  BA8F {is} BA8F fn BA8F
//***************************************************************************

void PrimFnDydDoubleTildeUnionBA8FisBA8FvBA8F
    (LPAPLBA8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = ArbUnionBA8F_RE (lpatLft->aplBA8F, lpatRht->aplBA8F, lpPrimSpec);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%s#%d)", MyGetExceptionStr (exCode), WFNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydDoubleTildeUnionBA8FisBA8FvBA8F


//***************************************************************************
//  $PrimOpDoubleTilde_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    monadic operator DoubleTilde ("ArbPrec" and "ArbPrec")
//***************************************************************************

LPPL_YYSTYPE PrimOpDoubleTilde_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_DOUBLETILDE);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return
          PrimOpMonDoubleTilde_EM_YY (lpYYFcnStrOpr,    // Ptr to operator function strand
                                      lptkRhtArg,       // Ptr to right arg (may be NULL if niladic)
                                      FALSE);           // TRUE iff prototyping
    else
        return
          PrimOpDydDoubleTilde_EM_YY (lptkLftArg,       // Ptr to left arg token
                                      lpYYFcnStrOpr,    // Ptr to operator function strand
                                      lptkRhtArg,       // Ptr to right arg token
                                      FALSE);           // TRUE iff prototyping
} // End PrimOpDoubleTilde_EM_YY


//***************************************************************************
//  $PrimOpMonDoubleTilde_EM_YY
//
//  Common subroutine for primitive operator for monadic/dyadic derived functions
//    from DoubleTilde ("ArbPrec")
//***************************************************************************

LPPL_YYSTYPE PrimOpMonDoubleTilde_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token (may be NULL if niladic)
     UBOOL        bPrototyping)         // TRUE iff prototyping ***FIXME*** -- Unused as yet

{
    LPTOKEN      lptkAxisOpr,           // Ptr to operator axis token (may be NULL)
                 lptkAxisFcn;           // ...    function ...
    LPPL_YYSTYPE lpYYFcnStrLft;         // Ptr to left operand function strand
    UINT         uTknCount;             // Token count in the operand

    // Check for axis operator
    lptkAxisOpr = CheckAxisOper (lpYYFcnStrOpr);

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = GetMonLftOper (lpYYFcnStrOpr, lptkAxisOpr); Assert (lpYYFcnStrLft NE NULL);

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken))
        goto LEFT_SYNTAX_EXIT;

    // Check for axis operator
    lptkAxisFcn = CheckAxisOper (lpYYFcnStrLft);

    // Calculate the token count excluding the Axis Operator
    uTknCount = lpYYFcnStrLft->TknCount - (lptkAxisFcn NE NULL);

    // Check for a DoubleTilde-sensitive function
    if (uTknCount EQ 1
     && lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED)
    {
        // Split cases based upon the function symbol
        switch (lpYYFcnStrLft->tkToken.tkData.tkChar)
        {
            case UTF16_LEFTCARETUNDERBAR:
            case INDEX_DOUBLETILDE_LE:
                // If the operator axis token is present, ...
                if (lptkAxisOpr NE NULL)
                    goto AXIS_SYNTAX_EXIT;

                return
                  PrimFnDoubleTildeEP2MR_EM_YY (GetLftFcnTkn (lpYYFcnStrOpr),   // Ptr to function token
                                                lptkRhtArg,                     // Ptr to right arg token
                                                lptkAxisFcn);                   // Ptr to function axis token (may be NULL)
            case UTF16_RIGHTCARETUNDERBAR:
            case INDEX_DOUBLETILDE_GE:
                // If the operator axis token is present, ...
                if (lptkAxisOpr NE NULL)
                    goto AXIS_SYNTAX_EXIT;

                return
                  PrimFnDoubleTildeMR2EP_EM_YY (GetLftFcnTkn (lpYYFcnStrOpr),   // Ptr to function token
                                                lptkRhtArg,                     // Ptr to right arg token
                                                lptkAxisFcn);                   // Ptr to function axis token (may be NULL)
            default:
                goto NONCE_EXIT;
        } // End SWITCH
    } else
        goto NONCE_EXIT;

LEFT_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxisOpr);
    goto ERROR_EXIT;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    return NULL;
} // End PrimOpMonDoubleTilde_EM_YY


//***************************************************************************
//  $PrimFnDoubleTildeMR2EP_EM_YY
//
//  Derived function from DoubleTilde to convert Midpoint-Radius to EndPoint
//***************************************************************************

LPPL_YYSTYPE PrimFnDoubleTildeMR2EP_EM_YY
    (LPTOKEN lptkFunc,                      // Ptr to function token
     LPTOKEN lptkRhtArg,                    // Ptr to right arg token
     LPTOKEN lptkAxis)                      // Ptr to axis token (may be NULL)

{
    Assert (lptkFunc->tkData.tkChar EQ UTF16_RIGHTCARETUNDERBAR
         || lptkFunc->tkData.tkChar EQ INDEX_DOUBLETILDE_GE);

    // Call common outer routine
    return
      RightCaretUnd_MR2EP_Outer_EM_YY (lptkFunc,            // Ptr to function token
                                       lptkRhtArg,          // Ptr to right arg token
                                       lptkAxis,            // Ptr to axis token (may be NULL)
                                       MR2EP_Inner);        // Ptr to common inner processing routine
} // End PrimFnDoubleTildeMR2EP_EM_YY


//***************************************************************************
//  $MR2EP_Inner
//
//  Inner routine for <RightCaretUnd_MR2EP_Outer_EM_YY>
//
//  The right arg may be of any simple numeric type.
//  The result is of the same type as the right arg
//    except BOOL and APA -> INT
//       and ARB          -> VFP
//***************************************************************************

void MR2EP_Inner
    (APLSTYPE aplTypeRht,       // Right arg storage type
     APLUINT  uRes0,            // uAX EQ 0 index into the result
     APLUINT  uRes1,            // ...    1 ...
     APLUINT  uRht,             // Index into the right arg
     LPVOID   lpMemRht,         // Ptr to right arg global memory data
     LPVOID   lpMemRes)         // Ptr to result global memory data

{
    APLINT   aplInt;            // Temp
    APLFLOAT aplFlt;            // ...
    LPAPLRAT lpaplRat;          // ...
    LPAPLVFP lpaplVfp;          // ...
    LPAPLARB lpaplArb;          // ...
    int      iSizeofRht;        // # bytes in each right arg item

    // Get the size of an item in the right arg
    iSizeofRht = TranslateArrayTypeToSizeof (aplTypeRht);

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:
            if (IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht * iSizeofRht)))
                // Signal an error
                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            // Fall through to common code

        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            // Get the next INT
            aplInt = GetNextInteger (lpMemRht, aplTypeRht, uRht);

            // Save the Endpoints in the result
            ((LPAPLINT  ) lpMemRes)[uRes0] =
            ((LPAPLINT  ) lpMemRes)[uRes1] = aplInt;

            break;

        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:
            if (IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht * iSizeofRht)))
                // Signal an error
                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            // Fall through to common code

        case ARRAY_FLOAT:
            // Get the next FLT
            aplFlt = ((LPAPLFLOAT) lpMemRht)[uRht];

            // Save the Endpoints in the result
            ((LPAPLFLOAT) lpMemRes)[uRes0] =
            ((LPAPLFLOAT) lpMemRes)[uRes1] = aplFlt;

            break;

        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:
            if (IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht * iSizeofRht)))
                // Signal an error
                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            // Fall through to common code

        case ARRAY_RAT:
            // Get the next RAT
            lpaplRat = GetNextRatPtr_XFREE (lpMemRht, aplTypeRht, uRht);

            // Save the Endpoints in the result
            mpq_init_set (&((LPAPLRAT) lpMemRes)[uRes0], lpaplRat);
            mpq_init_set (&((LPAPLRAT) lpMemRes)[uRes1], lpaplRat);

            break;

        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:
            if (IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht * iSizeofRht)))
                // Signal an error
                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            // Fall through to common code

        case ARRAY_VFP:
            // Get the next VFP
            lpaplVfp = GetNextVfpPtr_XFREE (lpMemRht, aplTypeRht, uRht);

            // Save the Endpoints in the result
            mpfr_init_set (&((LPAPLVFP) lpMemRes)[uRes0], lpaplVfp, MPFR_RNDN);
            mpfr_init_set (&((LPAPLVFP) lpMemRes)[uRes1], lpaplVfp, MPFR_RNDN);

            break;

        case ARRAY_BA2F:
        case ARRAY_BA4F:
        case ARRAY_BA8F:
            if (IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht * iSizeofRht)))
                // Signal an error
                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            // Fall through to common code

        case ARRAY_ARB:
            // Get the next ARB
            lpaplArb = GetNextArbPtr_XFREE (lpMemRht, aplTypeRht, uRht);

            // Initialize to 0
            mpfr_init0 (&((LPAPLVFP) lpMemRes)[uRes0]);
            mpfr_init0 (&((LPAPLVFP) lpMemRes)[uRes1]);

            // Get the endpoints as VFPs
            arb_get_interval_mpfr (&((LPAPLVFP) lpMemRes)[uRes0],
                                   &((LPAPLVFP) lpMemRes)[uRes1],
                                    lpaplArb);
            break;

        defstop
            break;
    } // End SWITCH
} // End MR2EP_Inner


//***************************************************************************
//  $PrimFnDoubleTildeEP2MR_EM_YY
//
//  Derived function from DoubleTilde to convert EndPoint to Midpoint-Radius
//***************************************************************************

LPPL_YYSTYPE PrimFnDoubleTildeEP2MR_EM_YY
    (LPTOKEN lptkFunc,                      // Ptr to function token
     LPTOKEN lptkRhtArg,                    // Ptr to right arg token
     LPTOKEN lptkAxis)                      // Ptr to axis token (may be NULL)

{
    Assert (lptkFunc->tkData.tkChar EQ UTF16_LEFTCARETUNDERBAR
         || lptkFunc->tkData.tkChar EQ INDEX_DOUBLETILDE_LE);

    // Call common outer routine
    return
      LeftCaretUnd_EP2MR_Outer_EM_YY (lptkFunc,             // Ptr to function token
                                      lptkRhtArg,           // Ptr to right arg token
                                      lptkAxis,             // Ptr to axis token (may be NULL)
                                      EP2MR_Inner);         // Ptr to common inner processing routine
} // End PrimFnDoubleTildeEP2MR_EM_YY


//***************************************************************************
//  $EP2MR_Inner
//
//  Inner routine for <LeftCaretUnd_EP2MR_Outer_EM_YY>
//
//  The right arg may be any simple numeric type.
//  The result is always an ARB.
//
//  If the first endpoint is not <= the second one, return FALSE.
//***************************************************************************

UBOOL EP2MR_Inner
    (APLSTYPE aplTypeRht,       // Right arg storage type
     APLUINT  uRht0,            // uAX EQ 0 index into the right arg
     APLUINT  uRht1,            // ...    1 ...
     APLUINT  uDimAx,           // Length of the axis dimension
     LPVOID   lpMemRht,         // Ptr to right arg global memory data
     LPAPLARB lpMemRes)         // Ptr to result global memory data

{
    UINT     uBitMask0,         // Bit mask when looping through Booleans
             uBitMask1;         // ...
    APLVFP   mpfrLo,            // Low Endpoint
             mpfrHi;            // High ...
    LPAPLRAT lpaplRat;          // Temp
    LPAPLVFP lpaplVfp;          // ...
    LPAPLARB lpaplArb;          // ...
    mp_limb_signed_t prec = ARB_PREC_FPC;
    UBOOL    bRet = FALSE;      // TRUE iff the result is valid
    int      iSizeofRht;        // # bytes in each right arg item

    // Get the size of an item in the right arg
    iSizeofRht = TranslateArrayTypeToSizeof (aplTypeRht);

    // Initialize
    mpfr_inits (&mpfrLo, &mpfrHi, NULL);

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            uBitMask0  = BIT0 << (UINT) (uRht0 % NBIB);
            uBitMask1  = BIT0 << (UINT) (uRht1 % NBIB);

            // Extract the Endpoints as MPFRs
            mpfr_set_sj (&mpfrLo, ((uBitMask0 & ((LPAPLBOOL) lpMemRht)[uRht0 >> LOG2NBIB]) ? 1   : 0  ), MPFR_RNDN);

            // If there's a 2nd Endpoint, ...
            if (uDimAx EQ 2)
                mpfr_set_sj (&mpfrHi, ((uBitMask1 & ((LPAPLBOOL) lpMemRht)[uRht1 >> LOG2NBIB]) ? 1   : 0  ), MPFR_RNDN);
            break;

        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:
            if (IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht0 * iSizeofRht))
             || IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht1 * iSizeofRht)))
                // Signal an error
                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            // Fall through to common code

        case ARRAY_INT:
            // Extract the Endpoints as MPFRs
            mpfr_set_sj (&mpfrLo, GetNextInteger(lpMemRht, aplTypeRht, uRht0), MPFR_RNDN);

            // If there's a 2nd Endpoint, ...
            if (uDimAx EQ 2)
                mpfr_set_sj (&mpfrHi, GetNextInteger(lpMemRht, aplTypeRht, uRht1), MPFR_RNDN);
            break;

        case ARRAY_APA:
            // Extract the Endpoints as MPFRs
            mpfr_set_sj (&mpfrLo, GetNextInteger(lpMemRht, aplTypeRht, uRht0), MPFR_RNDN);

            // If there's a 2nd Endpoint, ...
            if (uDimAx EQ 2)
                mpfr_set_sj (&mpfrHi, GetNextInteger(lpMemRht, aplTypeRht, uRht1), MPFR_RNDN);
            break;

        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:
            if (IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht0 * iSizeofRht))
             || IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht1 * iSizeofRht)))
                // Signal an error
                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            // Fall through to common code

        case ARRAY_FLOAT:
            // Extract the Endpoints as MPFRs
            mpfr_set_d  (&mpfrLo, GetNextFloat  (lpMemRht, aplTypeRht, uRht0), MPFR_RNDN);

            // If there's a 2nd Endpoint, ...
            if (uDimAx EQ 2)
                mpfr_set_d  (&mpfrHi, GetNextFloat  (lpMemRht, aplTypeRht, uRht1), MPFR_RNDN);
            break;

        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:
            if (IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht0 * iSizeofRht))
             || IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht1 * iSizeofRht)))
                // Signal an error
                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            // Fall through to common code

        case ARRAY_RAT:
            // Get the next RAT w/o having to free it
            lpaplRat = GetNextRatPtr_XFREE (lpMemRht, aplTypeRht, uRht0);

            // Extract the Endpoints as MPFRs
            mpfr_set_q  (&mpfrLo, lpaplRat, MPFR_RNDN);

            // If there's a 2nd Endpoint, ...
            if (uDimAx EQ 2)
            {
                // Get the next RAT w/o having to free it
                lpaplRat = GetNextRatPtr_XFREE (lpMemRht, aplTypeRht, uRht1);

                mpfr_set_q  (&mpfrHi, lpaplRat, MPFR_RNDN);
            } // End IF

            break;

        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:
            if (IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht0 * iSizeofRht))
             || IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht1 * iSizeofRht)))
                // Signal an error
                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            // Fall through to common code

        case ARRAY_VFP:
            // Get the next VFP w/o having to free it
            lpaplVfp = GetNextVfpPtr_XFREE (lpMemRht, aplTypeRht, uRht0);

            // Extract the Endpoints as MPFRs
            mpfr_set    (&mpfrLo, lpaplVfp, MPFR_RNDN);

            // If there's a 2nd Endpoint, ...
            if (uDimAx EQ 2)
            {
                // Get the next VFP w/o having to free it
                lpaplVfp = GetNextVfpPtr_XFREE (lpMemRht, aplTypeRht, uRht1);

                mpfr_set    (&mpfrHi, lpaplVfp, MPFR_RNDN);
            } // End IF

            break;

        case ARRAY_BA2F:
        case ARRAY_BA4F:
        case ARRAY_BA8F:
            if (IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht0 * iSizeofRht))
             || IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht1 * iSizeofRht)))
                // Signal an error
                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            // Fall through to common code

        case ARRAY_ARB:
            // Get the next ARB w/o having to free it
            lpaplArb = GetNextArbPtr_XFREE (lpMemRht, aplTypeRht, uRht0);

            // Extract the Endpoints as MPFRs
            arf_get_mpfr2 (&mpfrLo, arb_midref (lpaplArb), MPFR_RNDN);

            // If there's a 2nd Endpoint, ...
            if (uDimAx EQ 2)
            {
                // Get the next ARB w/o having to free it
                lpaplArb = GetNextArbPtr_XFREE (lpMemRht, aplTypeRht, uRht1);

                arf_get_mpfr2 (&mpfrHi, arb_midref (lpaplArb), MPFR_RNDN);
            } // End IF

            break;

        defstop
            break;
    } // End SWITCH

    // If there's no 2nd Endpoint, ...
    if (uDimAx EQ 1)
        // Copy the 1st (and only) Endpoint
        mpfr_set    (&mpfrHi, &mpfrLo, MPFR_RNDN);

    // If the order is valid, ...
    if (mpfr_cmp (&mpfrLo, &mpfrHi) <= 0)
    {
        // Convert from Endpoints to Midpoint-Radius
        arb_set_interval_mpfr2 (lpMemRes, &mpfrLo, &mpfrHi, prec);

        // Mark as successful
        bRet = TRUE;
    } // End IF

    // We no longer need this storage
    mpfr_clears (&mpfrLo, &mpfrHi, NULL);

    return bRet;
} // End EP2MR_Inner


//***************************************************************************
//  $PrimOpDydDoubleTilde_EM_YY
//
//  Common subroutine for primitive operator for monadic/dyadic derived functions
//    from DoubleTilde ("ArbPrec")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydDoubleTilde_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token (may be NULL if niladic)
     UBOOL        bPrototyping)         // TRUE iff prototyping ***FIXME*** -- Unused as yet

{
    LPTOKEN      lptkAxisOpr,           // Ptr to operator axis token (may be NULL)
                 lptkAxisFcn;           // ...    function ...
    LPPL_YYSTYPE lpYYFcnStrLft;         // Ptr to left operand function strand
    UINT         uTknCount;             // Token count in the operand

    // Check for axis operator
    lptkAxisOpr = CheckAxisOper (lpYYFcnStrOpr);

    // If the operator axis token is present, ...
    if (lptkAxisOpr NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = GetMonLftOper (lpYYFcnStrOpr, lptkAxisOpr); Assert (lpYYFcnStrLft NE NULL);

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken))
        goto LEFT_SYNTAX_EXIT;

    // Check for axis operator
    lptkAxisFcn = CheckAxisOper (lpYYFcnStrLft);

    // Calculate the token count excluding the Axis Operator
    uTknCount = lpYYFcnStrLft->TknCount - (lptkAxisFcn NE NULL);

    // Check for a DoubleTilde-sensitive function
    if (uTknCount EQ 1
     && (lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
      || lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FILLJOT))
    {
        // Split cases based upon the function symbol
        switch (lpYYFcnStrLft->tkToken.tkData.tkChar)
        {
            case UTF16_LEFTCARET:
            case INDEX_DOUBLETILDE_LT:
                // Ensure called dyadically
                if (lptkLftArg EQ NULL)
                    goto VALENCE_EXIT;

                return
                  PrimFnDoubleTildeLt_EM_YY (lptkLftArg,                    // Ptr to left arg token
                                             GetLftFcnTkn (lpYYFcnStrOpr),  // Ptr to function token
                                             lptkRhtArg,                    // Ptr to right arg token
                                             lptkAxisFcn);                  // Ptr to function axis token (may be NULL)
            case UTF16_LEFTCARETUNDERBAR:
            case INDEX_DOUBLETILDE_LE:
                // Ensure called dyadically
                if (lptkLftArg EQ NULL)
                    goto VALENCE_EXIT;

                return
                  PrimFnDoubleTildeLe_EM_YY (lptkLftArg,                    // Ptr to left arg token
                                             GetLftFcnTkn (lpYYFcnStrOpr),  // Ptr to function token
                                             lptkRhtArg,                    // Ptr to right arg token
                                             lptkAxisFcn);                  // Ptr to function axis token (may be NULL)
            case UTF16_EQUAL:
            case INDEX_DOUBLETILDE_EQ:
                // Ensure called dyadically
                if (lptkLftArg EQ NULL)
                    goto VALENCE_EXIT;

                return
                  PrimFnDoubleTildeEq_EM_YY (lptkLftArg,                    // Ptr to left arg token
                                             GetLftFcnTkn (lpYYFcnStrOpr),  // Ptr to function token
                                             lptkRhtArg,                    // Ptr to right arg token
                                             lptkAxisFcn);                  // Ptr to function axis token (may be NULL)
            case UTF16_RIGHTCARETUNDERBAR:
            case INDEX_DOUBLETILDE_GE:
                // Ensure called dyadically
                if (lptkLftArg EQ NULL)
                    goto VALENCE_EXIT;

                return
                  PrimFnDoubleTildeGe_EM_YY (lptkLftArg,                    // Ptr to left arg token
                                             GetLftFcnTkn (lpYYFcnStrOpr),  // Ptr to function token
                                             lptkRhtArg,                    // Ptr to right arg token
                                             lptkAxisFcn);                  // Ptr to function axis token (may be NULL)
            case UTF16_RIGHTCARET:
            case INDEX_DOUBLETILDE_GT:
                // Ensure called dyadically
                if (lptkLftArg EQ NULL)
                    goto VALENCE_EXIT;

                return
                  PrimFnDoubleTildeGt_EM_YY (lptkLftArg,                    // Ptr to left arg token
                                             GetLftFcnTkn (lpYYFcnStrOpr),  // Ptr to function token
                                             lptkRhtArg,                    // Ptr to right arg token
                                             lptkAxisFcn);                  // Ptr to function axis token (may be NULL)
            case UTF16_NOTEQUAL:
            case INDEX_DOUBLETILDE_NE:
                // Ensure called dyadically
                if (lptkLftArg EQ NULL)
                    goto VALENCE_EXIT;

                return
                  PrimFnDoubleTildeNe_EM_YY (lptkLftArg,                    // Ptr to left arg token
                                             GetLftFcnTkn (lpYYFcnStrOpr),  // Ptr to function token
                                             lptkRhtArg,                    // Ptr to right arg token
                                             lptkAxisFcn);                  // Ptr to function axis token (may be NULL)
            case UTF16_DOWNSHOE:
            case INDEX_DOUBLETILDE_UNION:
                // Ensure called dyadically
                if (lptkLftArg EQ NULL)
                    goto VALENCE_EXIT;

                return
                  PrimFnDoubleTildeUnion_EM_YY (lptkLftArg,                     // Ptr to left arg token
                                                GetLftFcnTkn (lpYYFcnStrOpr),   // Ptr to function token
                                                lptkRhtArg,                     // Ptr to right arg token
                                                lptkAxisOpr);                   // Ptr to operator axis token (may be NULL)
            case UTF16_UPSHOE:
            case INDEX_DOUBLETILDE_INTER:
                // Ensure called dyadically
                if (lptkLftArg EQ NULL)
                    goto VALENCE_EXIT;

                return
                  PrimFnDoubleTildeInter_EM_YY (lptkLftArg,                     // Ptr to left arg token
                                                GetLftFcnTkn (lpYYFcnStrOpr),   // Ptr to function token
                                                lptkRhtArg,                     // Ptr to right arg token
                                                lptkAxisOpr);                   // Ptr to operator axis token (may be NULL)
            case UTF16_LEFTSHOE:
            case INDEX_DOUBLETILDE_LS:
                // Ensure called dyadically
                if (lptkLftArg EQ NULL)
                    goto VALENCE_EXIT;

                return
                  PrimFnDoubleTildeLs_EM_YY (lptkLftArg,                    // Ptr to left arg token
                                             GetLftFcnTkn (lpYYFcnStrOpr),  // Ptr to function token
                                             lptkRhtArg,                    // Ptr to right arg token
                                             lptkAxisFcn);                  // Ptr to function axis token (may be NULL)
            case UTF16_RIGHTSHOE:
            case INDEX_DOUBLETILDE_RS:
                // Ensure called dyadically
                if (lptkLftArg EQ NULL)
                    goto VALENCE_EXIT;

                return
                  PrimFnDoubleTildeRs_EM_YY (lptkLftArg,                    // Ptr to left arg token
                                             GetLftFcnTkn (lpYYFcnStrOpr),  // Ptr to function token
                                             lptkRhtArg,                    // Ptr to right arg token
                                             lptkAxisFcn);                  // Ptr to function axis token (may be NULL)
            case UTF16_EQUALUNDERBAR:
            case UTF16_NOTEQUALUNDERBAR:
                // Ensure called dyadically
                if (lptkLftArg EQ NULL)
                    goto VALENCE_EXIT;

                return
                  PrimFnDoubleTildeMatch_EM_YY (lptkLftArg,                     // Ptr to left arg token
                                                GetLftFcnTkn (lpYYFcnStrOpr),   // Ptr to function token
                                                lptkRhtArg,                     // Ptr to right arg token
                                                lptkAxisOpr);                   // Ptr to operator axis token (may be NULL)
            case UTF16_JOT:
            case INDEX_DOUBLETILDE_JOT:
                // Ensure called dyadically
                if (lptkLftArg EQ NULL)
                    goto VALENCE_EXIT;

                return
                  PrimFnDoubleTildeJot_EM_YY   (lptkLftArg,                     // Ptr to left arg token
                                                GetLftFcnTkn (lpYYFcnStrOpr),   // Ptr to function token
                                                lptkRhtArg,                     // Ptr to right arg token
                                                lptkAxisOpr);                   // Ptr to operator axis token (may be NULL)
            default:
                goto NONCE_EXIT;
        } // End SWITCH
    } else
        goto NONCE_EXIT;

LEFT_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxisOpr);
    goto ERROR_EXIT;

VALENCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALENCE_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    goto ERROR_EXIT;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    return NULL;
} // End PrimOpDydDoubleTilde_EM_YY


//***************************************************************************
//  $PrimFnDoubleTildeLt_EM_YY
//
//  DoubleTilde on LeftCaret Comparison function
//***************************************************************************

LPPL_YYSTYPE PrimFnDoubleTildeLt_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to function axis token (may be NULL)

{
    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
    else
        return (*lpPrimSpecLt->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpecLt);
} //End PrimFnDoubleTildeLt_EM_YY


//***************************************************************************
//  $PrimFnDydDoubleTildeLtBisBA1FvBA1F
//
//  Primitive scalar function dyadic DoubleTildeLt:  B {is} BA1F fn BA1F
//***************************************************************************

void PrimFnDydDoubleTildeLtBisBA1FvBA1F
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes >> LOG2NBIB] |= arb_lt (&lpatLft->aplArb, &lpatRht->aplArb) << (MASKLOG2NBIB & (UINT) uRes);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%s#%d)", MyGetExceptionStr (exCode), WFNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydDoubleTildeLtBisBA1FvBA1F


//***************************************************************************
//  $PrimFnDoubleTildeLe_EM_YY
//
//  DoubleTilde on LeftCaretUnderbar Comparison function
//***************************************************************************

LPPL_YYSTYPE PrimFnDoubleTildeLe_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to function axis token (may be NULL)

{
    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
    else
        return (*lpPrimSpecLe->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpecLe);
} //End PrimFnDoubleTildeLe_EM_YY


//***************************************************************************
//  $PrimFnDydDoubleTildeLeBisBA1FvBA1F
//
//  Primitive scalar function dyadic DoubleTildeLe:  B {is} BA1F fn BA1F
//***************************************************************************

void PrimFnDydDoubleTildeLeBisBA1FvBA1F
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes >> LOG2NBIB] |= arb_le (&lpatLft->aplArb, &lpatRht->aplArb) << (MASKLOG2NBIB & (UINT) uRes);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%s#%d)", MyGetExceptionStr (exCode), WFNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydDoubleTildeLeBisBA1FvBA1F


//***************************************************************************
//  $PrimFnDoubleTildeEq_EM_YY
//
//  DoubleTilde on Equal Comparison function
//***************************************************************************

LPPL_YYSTYPE PrimFnDoubleTildeEq_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to function axis token (may be NULL)

{
    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
    else
        return (*lpPrimSpecEq->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpecEq);
} //End PrimFnDoubleTildeEq_EM_YY


//***************************************************************************
//  $PrimFnDydDoubleTildeEqBisBA1FvBA1F
//
//  Primitive scalar function dyadic DoubleTildeEq:  B {is} BA1F fn BA1F
//***************************************************************************

void PrimFnDydDoubleTildeEqBisBA1FvBA1F
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call common function
    PrimFnDydDoubleTildeEqBisBAxFvBAxF (lpMemRes, uRes, lpatLft, lpatRht, 1, lpPrimSpec);
} // End PrimFnDydDoubleTildeEqBisBA1FvBA1F


//***************************************************************************
//  $PrimFnDydDoubleTildeEqBisBA2FvBA2F
//
//  Primitive scalar function dyadic DoubleTildeEq:  B {is} BA2F fn BA2F
//***************************************************************************

void PrimFnDydDoubleTildeEqBisBA2FvBA2F
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call common function
    PrimFnDydDoubleTildeEqBisBAxFvBAxF (lpMemRes, uRes, lpatLft, lpatRht, 2, lpPrimSpec);
} // End PrimFnDydDoubleTildeEqBisBA2FvBA2F


//***************************************************************************
//  $PrimFnDydDoubleTildeEqBisBA4FvBA4F
//
//  Primitive scalar function dyadic DoubleTildeEq:  B {is} BA4F fn BA4F
//***************************************************************************

void PrimFnDydDoubleTildeEqBisBA4FvBA4F
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call common function
    PrimFnDydDoubleTildeEqBisBAxFvBAxF (lpMemRes, uRes, lpatLft, lpatRht, 4, lpPrimSpec);
} // End PrimFnDydDoubleTildeEqBisBA4FvBA4F


//***************************************************************************
//  $PrimFnDydDoubleTildeEqBisBA8FvBA8F
//
//  Primitive scalar function dyadic DoubleTildeEq:  B {is} BA8F fn BA8F
//***************************************************************************

void PrimFnDydDoubleTildeEqBisBA8FvBA8F
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call common function
    PrimFnDydDoubleTildeEqBisBAxFvBAxF (lpMemRes, uRes, lpatLft, lpatRht, 8, lpPrimSpec);
} // End PrimFnDydDoubleTildeEqBisBA8FvBA8F


//***************************************************************************
//  $PrimFnDydDoubleTildeEqBisBAxFvBAxF
//
//  Primitive scalar function dyadic DoubleTildeEq:  B {is} BAxF fn BAxF
//***************************************************************************

void PrimFnDydDoubleTildeEqBisBAxFvBAxF
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     int        iHCDimRes,          // HC Dimension (1, 2, 4, 8)
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;
    int             i;                              // Loop counter
    UBOOL           bRet = TRUE;                    // The result

    __try
    {
        // Loop through all of the parts
        for (i = 0; i < iHCDimRes; i++)
            // Call subfunction
            bRet &= arb_eq2 (&lpatLft->aplBA8F.parts[i], &lpatRht->aplBA8F.parts[i]);

        // Save in the result
        lpMemRes[uRes >> LOG2NBIB] |= bRet << (MASKLOG2NBIB & (UINT) uRes);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%s#%d)", MyGetExceptionStr (exCode), WFNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydDoubleTildeEqBisBAxFvBAxF


//***************************************************************************
//  $PrimFnDoubleTildeGe_EM_YY
//
//  DoubleTilde on RightCaretUnderbar Comparison function
//***************************************************************************

LPPL_YYSTYPE PrimFnDoubleTildeGe_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to function axis token (may be NULL)

{
    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
    else
        return (*lpPrimSpecGe->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpecGe);
} //End PrimFnDoubleTildeGe_EM_YY


//***************************************************************************
//  $PrimFnDydDoubleTildeGeBisBA1FvBA1F
//
//  Primitive scalar function dyadic DoubleTildeGe:  B {is} BA1F fn BA1F
//***************************************************************************

void PrimFnDydDoubleTildeGeBisBA1FvBA1F
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes >> LOG2NBIB] |= arb_ge (&lpatLft->aplArb, &lpatRht->aplArb) << (MASKLOG2NBIB & (UINT) uRes);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%s#%d)", MyGetExceptionStr (exCode), WFNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydDoubleTildeGeBisBA1FvBA1F


//***************************************************************************
//  $PrimFnDoubleTildeGt_EM_YY
//
//  DoubleTilde on RightCaret Comparison function
//***************************************************************************

LPPL_YYSTYPE PrimFnDoubleTildeGt_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to function axis token (may be NULL)

{
    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
    else
        return (*lpPrimSpecGt->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpecGt);
} //End PrimFnDoubleTildeGt_EM_YY


//***************************************************************************
//  $PrimFnDydDoubleTildeGtBisBA1FvBA1F
//
//  Primitive scalar function dyadic DoubleTildeGt:  B {is} BA1F fn BA1F
//***************************************************************************

void PrimFnDydDoubleTildeGtBisBA1FvBA1F
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes >> LOG2NBIB] |= arb_gt (&lpatLft->aplArb, &lpatRht->aplArb) << (MASKLOG2NBIB & (UINT) uRes);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%s#%d)", MyGetExceptionStr (exCode), WFNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydDoubleTildeGtBisBA1FvBA1F


//***************************************************************************
//  $PrimFnDoubleTildeNe_EM_YY
//
//  DoubleTilde on NotEqual Comparison function
//***************************************************************************

LPPL_YYSTYPE PrimFnDoubleTildeNe_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to function axis token (may be NULL)

{
    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
    else
        return (*lpPrimSpecNe->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpecNe);
} //End PrimFnDoubleTildeNe_EM_YY


//***************************************************************************
//  $PrimFnDydDoubleTildeNeBisBA1FvBA1F
//
//  Primitive scalar function dyadic DoubleTildeNe:  B {is} BA1F fn BA1F
//***************************************************************************

void PrimFnDydDoubleTildeNeBisBA1FvBA1F
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call common function
    PrimFnDydDoubleTildeNeBisBAxFvBAxF (lpMemRes, uRes, lpatLft, lpatRht, 1, lpPrimSpec);
} // End PrimFnDydDoubleTildeNeBisBA1FvBA1F


//***************************************************************************
//  $PrimFnDydDoubleTildeNeBisBA2FvBA2F
//
//  Primitive scalar function dyadic DoubleTildeNe:  B {is} BA2F fn BA2F
//***************************************************************************

void PrimFnDydDoubleTildeNeBisBA2FvBA2F
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call common function
    PrimFnDydDoubleTildeNeBisBAxFvBAxF (lpMemRes, uRes, lpatLft, lpatRht, 2, lpPrimSpec);
} // End PrimFnDydDoubleTildeNeBisBA2FvBA2F


//***************************************************************************
//  $PrimFnDydDoubleTildeNeBisBA4FvBA4F
//
//  Primitive scalar function dyadic DoubleTildeNe:  B {is} BA4F fn BA4F
//***************************************************************************

void PrimFnDydDoubleTildeNeBisBA4FvBA4F
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call common function
    PrimFnDydDoubleTildeNeBisBAxFvBAxF (lpMemRes, uRes, lpatLft, lpatRht, 4, lpPrimSpec);
} // End PrimFnDydDoubleTildeNeBisBA4FvBA4F


//***************************************************************************
//  $PrimFnDydDoubleTildeNeBisBA8FvBA8F
//
//  Primitive scalar function dyadic DoubleTildeNe:  B {is} BA8F fn BA8F
//***************************************************************************

void PrimFnDydDoubleTildeNeBisBA8FvBA8F
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call common function
    PrimFnDydDoubleTildeNeBisBAxFvBAxF (lpMemRes, uRes, lpatLft, lpatRht, 8, lpPrimSpec);
} // End PrimFnDydDoubleTildeNeBisBA8FvBA8F


//***************************************************************************
//  $PrimFnDydDoubleTildeNeBisBAxFvBAxF
//
//  Primitive scalar function dyadic DoubleTildeNe:  B {is} BAxF fn BAxF
//***************************************************************************

void PrimFnDydDoubleTildeNeBisBAxFvBAxF
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     int        iHCDimRes,          // HC Dimension (1, 2, 4, 8)
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;
    int             i;                              // Loop counter
    UBOOL           bRet = FALSE;                   // The result

    __try
    {
#ifdef DEBUG
////    ArbOut (L"Lft = ", &lpatLft->aplBA8F.parts[0]);
////    ArbOut (L"Rht = ", &lpatRht->aplBA8F.parts[0]);
#endif
        // Loop through all of the parts
        for (i = 0; i < iHCDimRes; i++)
            // Call subfunction
            bRet |= arb_ne2 (&lpatLft->aplBA8F.parts[i], &lpatRht->aplBA8F.parts[i]);

        // Save in the result
        lpMemRes[uRes >> LOG2NBIB] |= bRet << (MASKLOG2NBIB & (UINT) uRes);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%s#%d)", MyGetExceptionStr (exCode), WFNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydDoubleTildeNeBisBAxFvBAxF


//***************************************************************************
//  $PrimFnDoubleTildeMatch_EM_YY
//
//  DoubleTilde on Match/Mismatch Comparison function
//***************************************************************************

LPPL_YYSTYPE PrimFnDoubleTildeMatch_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to function axis token (may be NULL)

{
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to result
    APLSTYPE          aplTypeLft,           // Left arg storage type
                      aplTypeRht,           // Right ...
                      aplTypeCom;           // Common ...
    HGLOBAL           hGlbLft = NULL,       // Left arg global memory handle
                      hGlbRht = NULL;       // Right ...
    LPVARARRAY_HEADER lpMemHdrLft = NULL,   // Ptr to left arg global memory header
                      lpMemHdrRht = NULL;   // ...    right ...
    APLLONGEST        aplLongestLft,        // Left arg if immediate
                      aplLongestRht;        // Right ...
    LPVOID            lpMemLft,             // Ptr to left arg global memory data
                      lpMemRht;             // ...    right ...
    int               iHCDimCom,            // HC Dimension (1, 2, 4, 8)
                      i;                    // Loop counter
    APLBOOL           bRet = TRUE;          // The immediate result
    ALLTYPES          atLft = {0},          // The left arg as an ALLTYPE
                      atRht = {0};          // ... right ...

    Assert (lptkFunc->tkFlags.TknType EQ TKT_FCNIMMED);

    Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUALUNDERBAR
         || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUALUNDERBAR);

    // These functions are not sensitive to he Axis operator
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Get the attributes (Type, NELM, and Rank) of the left and right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, NULL, NULL, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, NULL, NULL, NULL);

    // Calculate the common storage type
    aplTypeCom = aTypePromote[aplTypeLft][aplTypeRht];

    // Get left and right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If the left arg is not immediate, ...
    if (lpMemHdrLft NE NULL)
        // Point to the data
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
    else
        // Point to the data
        lpMemLft = &aplLongestLft;

    // If the right arg is not immediate, ...
    if (lpMemHdrRht NE NULL)
        // Point to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    else
        // Point to the data
        lpMemRht = &aplLongestRht;

    // Promote the left and right args to the common storage type
    (*aTypeActPromote[aplTypeLft][aplTypeCom]) (lpMemLft, 0, &atLft);
    (*aTypeActPromote[aplTypeRht][aplTypeCom]) (lpMemRht, 0, &atRht);

    // If the left arg is not immediate, ...
    if (lpMemHdrLft NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;
    } // End IF

    // If the right arg is not immediate, ...
    if (lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    // Calculate the common dimension
    iHCDimCom = TranslateArrayTypeToHCDim (aplTypeCom);

    // Loop through all of the parts
    for (i = 0; i < iHCDimCom; i++)
        // Use <arb_eq2> for MATCH and <arb_ne2> for MISMATCH
        if (lptkFunc->tkData.tkChar EQ UTF16_EQUALUNDERBAR)
            // Calculate the result
            bRet &= arb_eq2 (&atLft.aplBA8F.parts[i], &atRht.aplBA8F.parts[i]);
        else
            // Calculate the result
            bRet &= arb_ne2 (&atLft.aplBA8F.parts[i], &atRht.aplBA8F.parts[i]);

    // We no longer need this storage
    (*aTypeFree[aplTypeCom]) (&atLft, 0);
    (*aTypeFree[aplTypeCom]) (&atRht, 0);

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_BOOL;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;             // Already 0 from YYAlloc
    lpYYRes->tkToken.tkData.tkBoolean  = bRet;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

ERROR_EXIT:
    return NULL;
} //End PrimFnDoubleTildeMatch_EM_YY


//***************************************************************************
//  $PrimSpecDoubleTildeLsRsStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecDoubleTildeLsRsStorageTypeDyd
    (APLNELM    aplNELMLft,
     LPAPLSTYPE lpaplTypeLft,
     LPTOKEN    lptkFunc,
     APLNELM    aplNELMRht,
     LPAPLSTYPE lpaplTypeRht)

{
    APLSTYPE aplTypeRes;

    // In case the left arg is an empty char,
    //   change its type to BOOL
    if (IsCharEmpty (*lpaplTypeLft, aplNELMLft))
        *lpaplTypeLft = ARRAY_BOOL;

    // In case the right arg is an empty char,
    //   change its type to BOOL
    if (IsCharEmpty (*lpaplTypeRht, aplNELMRht))
        *lpaplTypeRht = ARRAY_BOOL;

    // Calculate the storage type of the result
    aplTypeRes = StorageType (*lpaplTypeLft, lptkFunc, *lpaplTypeRht);

    // Is the result HC?
    if (IsHCAny (aplTypeRes))
        // Tell the caller to demote the data to HC dimension 1
        return ARRAY_REALONLY;

    // All numeric results are Boolean
    if (IsNumeric (aplTypeRes))
        // Mark it as a Boolean
        aplTypeRes = ARRAY_BOOL;

    return aplTypeRes;
} // End PrimSpecDoubleTildeLsRsStorageTypeDyd


//***************************************************************************
//  $PrimIdentOpDoubleTildeLsRs_EM_YY
//
//  Generate an identity element for the dyadic derived function from the
//    primitive operator DoubleTilde-Ls/Rs
//***************************************************************************

LPPL_YYSTYPE PrimIdentOpDoubleTildeLsRs_EM_YY
    (LPTOKEN      lptkRhtOrig,      // Ptr to original right arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,    // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,       // Ptr to right arg token
     LPTOKEN      lptkAxisOpr)      // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE      lpYYFcnStrLft,        // Ptr to left operand function strand
                      lpYYRes = NULL;       // Ptr to result
    HGLOBAL           hGlbRes = NULL;       // Result global memory handle
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to result array header
    LPAPLARB          lpMemRes;             // Ptr to result global memory data

    // The right arg is the prototype item from
    //   the original empty arg.

    Assert (lptkRhtOrig   NE NULL);
    Assert (lpYYFcnStrOpr NE NULL);
    Assert (lptkRhtArg    NE NULL);

    DbgStop ();

    // The (left) identity function for dyadic DoubleTilde is
    //   0±{infinity}

    // Set ptr to left operand skipping over the operator and axis token (if present)
    lpYYFcnStrLft = GetMonLftOper (lpYYFcnStrOpr, lptkAxisOpr); Assert (lpYYFcnStrLft NE NULL);

    // Check for left operand axis operator
    if (CheckAxisOper (lpYYFcnStrLft) NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Ensure the left operand is a function
    //   and not a Jot
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken)
     || IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_OPERAND_DOMAIN_EXIT;

    // Allocate space for the result
    hGlbRes =
      AllocateGlobalArray (ARRAY_ARB, 1, 0, NULL);
    // Check for errors
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (hGlbRes);

    // Point to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    Myarb_init (lpMemRes);
    mag_inf (arb_radref (lpMemRes));

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;     // Already 0 from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;             // Already 0 from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

    goto NORMAL_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxisOpr);
    goto ERROR_EXIT;

LEFT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    return lpYYRes;
} // End PrimIdentOpDoubleTildeLsRs_EM_YY


//***************************************************************************
//  $PrimFnDoubleTildeLs_EM_YY
//
//  DoubleTilde with LeftShoe left operand
//***************************************************************************

LPPL_YYSTYPE PrimFnDoubleTildeLs_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to function axis token (may be NULL)

{
    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
    else
        return (*lpPrimSpecLs->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpecLs);
} //End PrimFnDoubleTildeLs_EM_YY


//***************************************************************************
//  $PrimFnDydDoubleTildeLsBisBA1FvBA1F
//
//  Primitive scalar function dyadic DoubleTildeLs:  B {is} BA1F fn BA1F
//***************************************************************************

void PrimFnDydDoubleTildeLsBisBA1FvBA1F
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes >> LOG2NBIB] |= ArbLsBA1F_RE (lpatLft->aplArb, lpatRht->aplArb, lpPrimSpec) << (MASKLOG2NBIB & (UINT) uRes);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%s#%d)", MyGetExceptionStr (exCode), WFNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydDoubleTildeLsBisBA1FvBA1F


//***************************************************************************
//  $ArbLsBA1F_RE
//
//  LeftShoe == Is Contained In
//***************************************************************************

APLBOOL ArbLsBA1F_RE
    (APLBA1F    aplLft,             // Left arg
     APLBA1F    aplRht,             // Right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Note switched arguments
    return
      ArbRsBA1F_RE (aplRht, aplLft, lpPrimSpec);
} // End ArbLsBA1F_RE


//***************************************************************************
//  $PrimFnDoubleTildeRs_EM_YY
//
//  DoubleTilde withRighftShoe left operand
//***************************************************************************

LPPL_YYSTYPE PrimFnDoubleTildeRs_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to function axis token (may be NULL)

{
    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
    else
        return (*lpPrimSpecRs->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpecRs);
} //End PrimFnDoubleTildeRs_EM_YY


//***************************************************************************
//  $PrimFnDydDoubleTildeRsBisBA1FvBA1F
//
//  Primitive scalar function dyadic DoubleTildeRs:  B {is} BA1F fn BA1F
//***************************************************************************

void PrimFnDydDoubleTildeRsBisBA1FvBA1F
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes >> LOG2NBIB] |= ArbRsBA1F_RE (lpatLft->aplArb, lpatRht->aplArb, lpPrimSpec) << (MASKLOG2NBIB & (UINT) uRes);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%s#%d)", MyGetExceptionStr (exCode), WFNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydDoubleTildeRsBisBA1FvBA1F


//***************************************************************************
//  $ArbRsBA1F_RE
//
//  RightShoe == Contains
//***************************************************************************

APLBOOL ArbRsBA1F_RE
    (APLBA1F    aplLft,             // Left arg
     APLBA1F    aplRht,             // Right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    UBOOL bRet;                     // The result

    // Call the API
    bRet = (arb_contains (&aplLft, &aplRht) NE 0);

    // If TRUE, ...
    if (bRet)
    {
        mpfr_t L0, L1, R0, R1;

        // Initialize
        mpfr_inits (L0, L1, R0, R1, NULL);

        // Get the endpoints
        arb_get_interval_mpfr (L0, L1, &aplLft);
        arb_get_interval_mpfr (R0, R1, &aplRht);

        // Eliminate equal endpoints
        bRet &= !(mpfr_equal_p (L0, R0) || mpfr_equal_p (L1, R1));

        // We no longer need this storage
        mpfr_clears (L0, L1, R0, R1, NULL);
    } // End IF

    return bRet;
} // End ArbRsBA1F_RE


//***************************************************************************
//  $PrimSpecDoubleTildeJotStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecDoubleTildeJotStorageTypeDyd
    (APLNELM    aplNELMLft,
     LPAPLSTYPE lpaplTypeLft,
     LPTOKEN    lptkFunc,
     APLNELM    aplNELMRht,
     LPAPLSTYPE lpaplTypeRht)

{
    APLSTYPE aplTypeRes;

    // In case the left arg is an empty char,
    //   change its type to BOOL
    if (IsCharEmpty (*lpaplTypeLft, aplNELMLft))
        *lpaplTypeLft = ARRAY_BOOL;

    // In case the right arg is an empty char,
    //   change its type to BOOL
    if (IsCharEmpty (*lpaplTypeRht, aplNELMRht))
        *lpaplTypeRht = ARRAY_BOOL;

    // Calculate the storage type of the result
    aplTypeRes = StorageType (*lpaplTypeLft, lptkFunc, *lpaplTypeRht);

    // Is the result HC?
    if (IsHCAny (aplTypeRes))
        // Tell the caller to demote the data to HC dimension 1
        return ARRAY_REALONLY;

    // All numeric results are ARB
    if (IsNumeric (aplTypeRes))
        // Mark it as an ARB
        aplTypeRes = ARRAY_ARB;

    return aplTypeRes;
} // End PrimSpecDoubleTildeJotStorageTypeDyd


//***************************************************************************
//  $PrimFnDoubleTildeJot_EM_YY
//
//  DoubleTilde with Jot left operand
//***************************************************************************

LPPL_YYSTYPE PrimFnDoubleTildeJot_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to function axis token (may be NULL)

{
    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
    else
        return (*lpPrimSpecJot->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpecJot);
} //End PrimFnDoubleTildeJot_EM_YY


//***************************************************************************
//  $PrimFnDydDoubleTildeJotBA1FisBA1FvBA1F
//
//  Primitive scalar function dyadic DoubleTildeJot:  BA1F {is} BA1F fn BA1F
//***************************************************************************

void PrimFnDydDoubleTildeJotBA1FisBA1FvBA1F
    (LPAPLARB   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = ArbJotBA1F_RE (lpatLft->aplArb, lpatRht->aplArb, lpPrimSpec);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%s#%d)", MyGetExceptionStr (exCode), WFNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydDoubleTildeJotBA1FisBA1FvBA1F


//***************************************************************************
//  $ArbJotBA1F_RE
//
//  There are thirteen ways in which two intervals can be positioned
//    w.r.t. each other.
//***************************************************************************

APLARB ArbJotBA1F_RE
    (APLBA1F    aplLft,             // Left arg
     APLBA1F    aplRht,             // Right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    mp_limb_signed_t prec = ARB_PREC_FPC;
    mpfr_t L0, L1,
           R0, R1;
    int    iRes;                    // The result
    APLARB arbRes = {0};            // ...

    // Initialize
    mpfr_inits (L0, L1, R0, R1, NULL);

    // Get the endpoints
    arb_get_interval_mpfr (L0, L1, &aplLft);
    arb_get_interval_mpfr (R0, R1, &aplRht);

    //  0:  L = R
    if (arb_eq2 (&aplLft, &aplRht))
        iRes =  0;
    else
    //  1:  L0 = R0 && L1 > R1
    if (mpfr_cmp (L0, R0) == 0
     && mpfr_cmp (L1, R1) >  0)
        iRes =  1;
    else
    // -1:  L0 = R0 && L1 < R1
    if (mpfr_cmp (L0, R0) == 0
     && mpfr_cmp (L1, R1) <  0)
        iRes = -1;
    else
    //  2:  L0 < R0 && L1 = R1
    if (mpfr_cmp (L0, R0) < 0
     && mpfr_cmp (L1, R1) == 0)
        iRes =  2;
    else
    // -2:  L0 > R0 && L1 = R1
    if (mpfr_cmp (L0, R0) > 0
     && mpfr_cmp (L1, R1) == 0)
        iRes = -2;
    else
    //  3:  L0 < R0 && L1 > R1
    if (mpfr_cmp (L0, R0) < 0
     && mpfr_cmp (L1, R1) > 0)
        iRes =  3;
    else
    // -3:  L0 > R0 && L1 < R1
    if (mpfr_cmp (L0, R0) > 0
     && mpfr_cmp (L1, R1) < 0)
        iRes = -3;
    else
    //  4:  L0 < R0 && L1 < R1 && L1 > R0
    if (mpfr_cmp (L0, R0) < 0
     && mpfr_cmp (L1, R1) < 0
     && mpfr_cmp (L1, R0) > 0)
        iRes =  4;
    else
    // -4:  L0 > R0 && L1 > R1 && L0 < R1
    if (mpfr_cmp (L0, R0) > 0
     && mpfr_cmp (L1, R1) > 0
     && mpfr_cmp (L0, R1) < 0)
        iRes = -4;
    else
    //  5:  L1 = R0
    if (mpfr_cmp (L1, R0) == 0)
        iRes =  5;
    else
    // -5:  L0 = R1
    if (mpfr_cmp (L0, R1) == 0)
        iRes = -5;
    else
    //  6:  L1 < R0
    if (mpfr_cmp (L1, R0) < 0)
        iRes =  6;
    else
    // -6:  L0 > R1
    if (mpfr_cmp (L0, R1) > 0)
        iRes = -6;
    else
        DbgStop ();

    // We no longer need this storage
    mpfr_clears (L0, L1, R0, R1, NULL);

    // Initialize to 0/0
    Myarb_init (&arbRes);

    // Save in the result
    arb_set_si (&arbRes, iRes);

    return arbRes;
} // End ArbJotBA1F_RE


//***************************************************************************
//  End of File: po_dbltilde.c
//***************************************************************************
