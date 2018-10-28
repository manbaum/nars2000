//***************************************************************************
//  NARS2000 -- Type Promote/Demote Header
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

/*
This header defines several tables for information about APLSTYPE vs. APLSTYPE.

typedef enum tagARRAY_TYPES APLSTYPE;
APLSTYPE   aTypePromote        ARRAY_xxx Type Promotion result matrix
                                Use this table whenever you need to determine a promoted
                                  (but not demoted) APLSTYPE given two APLSTYPEs (say, a
                                  left and right arg APLSTYPE).

typedef void (*TPT_ACTION)   (LPTOKEN);
TPT_ACTION aTypeTknPromote     Type Promotion Token matrix
                                Use this table whenever you need to call a routine to
                                  promote (but not demote) a single value in a token from
                                  a given APLSTYPE to a given APLSTYPE storing the single
                                  value result back into the same token.

typedef void (*TP_ACTION)    (LPVOID, APLINT, LPALLTYPES);
TP_ACTION  aTypeActPromote     Type Promotion Action matrix
                                Use this table whenever you need to call a routine to
                                  promote (but not demote) a value from the first arg
                                  indexed by the second arg from a given APLSTYPE to
                                  a given APLSTYPE with the result stored in the third arg.

typedef void (*TC_ACTION)    (LPVOID, APLINT, LPALLTYPES, LPUBOOL);
TC_ACTION  aTypeActConvert     Type Conversion Action matrix allowing for type demotion
                                Use this table whenever you need to call a routine to
                                  promote or demote a value from the first arg indexed
                                  by the second arg from a given APLSTYPE to
                                  a given APLSTYPE with the result stored in the third arg,
                                  error flag returned in the fourth arg which may be NULL
 */

#ifndef DEFINE_VALUES
#define DEFINE_VALUES       // ***FIXME*** -- Why isn't this defined by the file in which it is <#include>d?
#endif


//***************************************************************************
//      TPF Definitions
//***************************************************************************

TPF_ACTION aTypeFree[ARRAY_LENGTH + 1]
#ifdef DEFINE_VALUES
= {TPF_IDENT,                       // 00:  Boolean
   TPF_IDENT,                       // 01:  Integer
   TPF_IDENT,                       // 02:  Floating point
   TPF_IDENT,                       // 03:  Character
   TPF_HETERO,                      // 04:  Simple heterogeneous (mixed numeric and character scalars)
   TPF_HETERO,                      // 05:  Nested
   TPF_IDENT,                       // 06:  List
   TPF_IDENT,                       // 07:  Arithmetic Progression Array
   TPF_RAT  ,                       // 08:  Multiprecision Rational Number
   TPF_VFP  ,                       // 09:  Variable-precision Float
   TPF_IDENT,                       // 0A:  Complex    INT coefficients
   TPF_IDENT,                       // 0B:  ...        FLT ...
   TPF_HC2R ,                       // 0C:  ...        RAT ...
   TPF_HC2V ,                       // 0D:  ...        VFP ...
   TPF_IDENT,                       // 0E:  Quaternion INT coefficients
   TPF_IDENT,                       // 0F:  ...        FLT ...
   TPF_HC4R ,                       // 10:  ...        RAT ...
   TPF_HC4V ,                       // 11:  ...        VFP ...
   TPF_IDENT,                       // 12:  Octonion   INT coefficients
   TPF_IDENT,                       // 13:  ...        FLT ...
   TPF_HC8R ,                       // 14:  ...        RAT ...
   TPF_HC8V ,                       // 15:  ...        VFP ...
   TPF_BA1F ,                       // 16:  Ball Arithmetic Real
   TPF_BA2F ,                       // 17:  ...             Complex
   TPF_BA4F ,                       // 18:  ...             Quaternion
   TPF_BA8F ,                       // 19:  ...             Octonion
  }
#endif
;
;

//***************************************************************************
//      TP Definitions
//***************************************************************************

#define TP_MAT                                                                                                                                                                                                                                                                                                                                                                      \
{/*     BOOL          INT          FLT         CHAR         HETERO       NESTED       LIST          APA          RAT          VFP         HC2I         HC2F         HC2R         HC2V         HC4I         HC4F         HC4R         HC4V         HC8I         HC8F         HC8R         HC8V         BA1F         BA2F         BA4F         BA8F         INIT     */               \
   {M(BOOL,BOOL),M(BOOL,INT ),M(BOOL,FLT ),M(BOOL,HETE),M(BOOL,HETE),M(BOOL,NEST),M(BOOL,ERR ),M(BOOL,INT ),M(BOOL,RAT ),M(BOOL,VFP ),M(BOOL,HC2I),M(BOOL,HC2F),M(BOOL,HC2R),M(BOOL,HC2V),M(BOOL,HC4I),M(BOOL,HC4F),M(BOOL,HC4R),M(BOOL,HC4V),M(BOOL,HC8I),M(BOOL,HC8F),M(BOOL,HC8R),M(BOOL,HC8V),M(BOOL,BA1F),M(BOOL,BA2F),M(BOOL,BA4F),M(BOOL,BA8F),M(BOOL,INIT)},   /* BOOL */   \
   {M(INT ,INT ),M(INT ,INT ),M(INT ,FLT ),M(INT ,HETE),M(INT ,HETE),M(INT ,NEST),M(INT ,ERR ),M(INT ,INT ),M(INT ,RAT ),M(INT ,VFP ),M(INT ,HC2I),M(INT ,HC2F),M(INT ,HC2R),M(INT ,HC2V),M(INT ,HC4I),M(INT ,HC4F),M(INT ,HC4R),M(INT ,HC4V),M(INT ,HC8I),M(INT ,HC8F),M(INT ,HC8R),M(INT ,HC8V),M(INT ,BA1F),M(INT ,BA2F),M(INT ,BA4F),M(INT ,BA8F),M(INT ,INIT)},   /* INT  */   \
   {M(FLT ,FLT ),M(FLT ,FLT ),M(FLT ,FLT ),M(FLT ,HETE),M(FLT ,HETE),M(FLT ,NEST),M(FLT ,ERR ),M(FLT ,FLT ),M(FLT ,VFP ),M(FLT ,VFP ),M(FLT ,HC2F),M(FLT ,HC2F),M(FLT ,HC2V),M(FLT ,HC2V),M(FLT ,HC4F),M(FLT ,HC4F),M(FLT ,HC4V),M(FLT ,HC4V),M(FLT ,HC8F),M(FLT ,HC8F),M(FLT ,HC8V),M(FLT ,HC8V),M(FLT ,BA1F),M(FLT ,BA2F),M(FLT ,BA4F),M(FLT ,BA8F),M(FLT ,INIT)},   /* FLT  */   \
   {M(CHAR,HETE),M(CHAR,HETE),M(CHAR,HETE),M(CHAR,CHAR),M(CHAR,HETE),M(CHAR,NEST),M(CHAR,ERR ),M(CHAR,HETE),M(CHAR,HETE),M(CHAR,HETE),M(CHAR,HETE),M(CHAR,HETE),M(CHAR,HETE),M(CHAR,HETE),M(CHAR,HETE),M(CHAR,HETE),M(CHAR,HETE),M(CHAR,HETE),M(CHAR,HETE),M(CHAR,HETE),M(CHAR,HETE),M(CHAR,HETE),M(CHAR,HETE),M(CHAR,HETE),M(CHAR,HETE),M(CHAR,HETE),M(CHAR,INIT)},   /* CHAR */   \
   {M(HETE,HETE),M(HETE,HETE),M(HETE,HETE),M(HETE,HETE),M(HETE,HETE),M(HETE,NEST),M(HETE,ERR ),M(HETE,HETE),M(HETE,HETE),M(HETE,HETE),M(HETE,HETE),M(HETE,HETE),M(HETE,HETE),M(HETE,HETE),M(HETE,HETE),M(HETE,HETE),M(HETE,HETE),M(HETE,HETE),M(HETE,HETE),M(HETE,HETE),M(HETE,HETE),M(HETE,HETE),M(HETE,HETE),M(HETE,HETE),M(HETE,HETE),M(HETE,HETE),M(HETE,INIT)},   /* HETE */   \
   {M(NEST,NEST),M(NEST,NEST),M(NEST,NEST),M(NEST,NEST),M(NEST,NEST),M(NEST,NEST),M(NEST,ERR ),M(NEST,NEST),M(NEST,NEST),M(NEST,NEST),M(NEST,NEST),M(NEST,NEST),M(NEST,NEST),M(NEST,NEST),M(NEST,NEST),M(NEST,NEST),M(NEST,NEST),M(NEST,NEST),M(NEST,NEST),M(NEST,NEST),M(NEST,NEST),M(NEST,NEST),M(NEST,NEST),M(NEST,NEST),M(NEST,NEST),M(NEST,NEST),M(NEST,INIT)},   /* NEST */   \
   {M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,INIT)},   /* LIST */   \
   {M(APA ,INT ),M(APA ,INT ),M(APA ,FLT ),M(APA ,HETE),M(APA ,HETE),M(APA ,NEST),M(APA ,ERR ),M(APA ,INT ),M(APA ,RAT ),M(APA ,VFP ),M(APA ,HC2I),M(APA ,HC2F),M(APA ,HC2R),M(APA ,HC2V),M(APA ,HC4I),M(APA ,HC4F),M(APA ,HC4R),M(APA ,HC4V),M(APA ,HC8I),M(APA ,HC8F),M(APA ,HC8R),M(APA ,HC8V),M(APA ,BA1F),M(APA ,BA2F),M(APA ,BA4F),M(APA ,BA8F),M(APA ,INIT)},   /* APA  */   \
   {M(RAT ,RAT ),M(RAT ,RAT ),M(RAT ,VFP ),M(RAT ,HETE),M(RAT ,HETE),M(RAT ,NEST),M(RAT ,ERR ),M(RAT ,RAT ),M(RAT ,RAT ),M(RAT ,VFP ),M(RAT ,HC2R),M(RAT ,HC2V),M(RAT ,HC2R),M(RAT ,HC2V),M(RAT ,HC4R),M(RAT ,HC4V),M(RAT ,HC4R),M(RAT ,HC4V),M(RAT ,HC8R),M(RAT ,HC8V),M(RAT ,HC8R),M(RAT ,HC8V),M(RAT ,BA1F),M(RAT ,BA2F),M(RAT ,BA4F),M(RAT ,BA8F),M(RAT ,INIT)},   /* RAT  */   \
   {M(VFP ,VFP ),M(VFP ,VFP ),M(VFP ,VFP ),M(VFP ,HETE),M(VFP ,HETE),M(VFP ,NEST),M(VFP ,ERR ),M(VFP ,VFP ),M(VFP ,VFP ),M(VFP ,VFP ),M(VFP ,HC2V),M(VFP ,HC2V),M(VFP ,HC2V),M(VFP ,HC2V),M(VFP ,HC4V),M(VFP ,HC4V),M(VFP ,HC4V),M(VFP ,HC4V),M(VFP ,HC8V),M(VFP ,HC8V),M(VFP ,HC8V),M(VFP ,HC8V),M(VFP ,BA1F),M(VFP ,BA2F),M(VFP ,BA4F),M(VFP ,BA8F),M(VFP ,INIT)},   /* VFP  */   \
   {M(HC2I,HC2I),M(HC2I,HC2I),M(HC2I,HC2F),M(HC2I,HETE),M(HC2I,HETE),M(HC2I,NEST),M(HC2I,ERR ),M(HC2I,HC2I),M(HC2I,HC2R),M(HC2I,HC2V),M(HC2I,HC2I),M(HC2I,HC2F),M(HC2I,HC2R),M(HC2I,HC2V),M(HC2I,HC4I),M(HC2I,HC4F),M(HC2I,HC4R),M(HC2I,HC4V),M(HC2I,HC8I),M(HC2I,HC8F),M(HC2I,HC8R),M(HC2I,HC8V),M(HC2I,BA2F),M(HC2I,BA2F),M(HC2I,BA4F),M(HC2I,BA8F),M(HC2I,INIT)},   /* HC2I */   \
   {M(HC2F,HC2F),M(HC2F,HC2F),M(HC2F,HC2F),M(HC2F,HETE),M(HC2F,HETE),M(HC2F,NEST),M(HC2F,ERR ),M(HC2F,HC2F),M(HC2F,HC2V),M(HC2F,HC2V),M(HC2F,HC2F),M(HC2F,HC2F),M(HC2F,HC2V),M(HC2F,HC2V),M(HC2F,HC4F),M(HC2F,HC4F),M(HC2F,HC4V),M(HC2F,HC4V),M(HC2F,HC8F),M(HC2F,HC8F),M(HC2F,HC8V),M(HC2F,HC8V),M(HC2F,BA2F),M(HC2F,BA2F),M(HC2F,BA4F),M(HC2F,BA8F),M(HC2F,INIT)},   /* HC2F */   \
   {M(HC2R,HC2R),M(HC2R,HC2R),M(HC2R,HC2V),M(HC2R,HETE),M(HC2R,HETE),M(HC2R,NEST),M(HC2R,ERR ),M(HC2R,HC2R),M(HC2R,HC2R),M(HC2R,HC2V),M(HC2R,HC2R),M(HC2R,HC2V),M(HC2R,HC2R),M(HC2R,HC2V),M(HC2R,HC4R),M(HC2R,HC4V),M(HC2R,HC4R),M(HC2R,HC4V),M(HC2R,HC8R),M(HC2R,HC8V),M(HC2R,HC8R),M(HC2R,HC8V),M(HC2R,BA2F),M(HC2R,BA2F),M(HC2R,BA4F),M(HC2R,BA8F),M(HC2R,INIT)},   /* HC2R */   \
   {M(HC2V,HC2V),M(HC2V,HC2V),M(HC2V,HC2V),M(HC2V,HETE),M(HC2V,HETE),M(HC2V,NEST),M(HC2V,ERR ),M(HC2V,HC2V),M(HC2V,HC2V),M(HC2V,HC2V),M(HC2V,HC2V),M(HC2V,HC2V),M(HC2V,HC2V),M(HC2V,HC2V),M(HC2V,HC4V),M(HC2V,HC4V),M(HC2V,HC4V),M(HC2V,HC4V),M(HC2V,HC8V),M(HC2V,HC8V),M(HC2V,HC8V),M(HC2V,HC8V),M(HC2V,BA2F),M(HC2V,BA2F),M(HC2V,BA4F),M(HC2V,BA8F),M(HC2V,INIT)},   /* HC2V */   \
   {M(HC4I,HC4I),M(HC4I,HC4I),M(HC4I,HC4F),M(HC4I,HETE),M(HC4I,HETE),M(HC4I,NEST),M(HC4I,ERR ),M(HC4I,HC4I),M(HC4I,HC4R),M(HC4I,HC4V),M(HC4I,HC4I),M(HC4I,HC4F),M(HC4I,HC4R),M(HC4I,HC4V),M(HC4I,HC4I),M(HC4I,HC4F),M(HC4I,HC4R),M(HC4I,HC4V),M(HC4I,HC8I),M(HC4I,HC8F),M(HC4I,HC8R),M(HC4I,HC8V),M(HC4I,BA4F),M(HC4I,BA4F),M(HC4I,BA4F),M(HC4I,BA8F),M(HC4I,INIT)},   /* HC4I */   \
   {M(HC4F,HC4F),M(HC4F,HC4F),M(HC4F,HC4F),M(HC4F,HETE),M(HC4F,HETE),M(HC4F,NEST),M(HC4F,ERR ),M(HC4F,HC4F),M(HC4F,HC4V),M(HC4F,HC4V),M(HC4F,HC4F),M(HC4F,HC4F),M(HC4F,HC4V),M(HC4F,HC4V),M(HC4F,HC4F),M(HC4F,HC4F),M(HC4F,HC4V),M(HC4F,HC4V),M(HC4F,HC8F),M(HC4F,HC8F),M(HC4F,HC8V),M(HC4F,HC8V),M(HC4F,BA4F),M(HC4F,BA4F),M(HC4F,BA4F),M(HC4F,BA8F),M(HC4F,INIT)},   /* HC4F */   \
   {M(HC4R,HC4R),M(HC4R,HC4R),M(HC4R,HC4V),M(HC4R,HETE),M(HC4R,HETE),M(HC4R,NEST),M(HC4R,ERR ),M(HC4R,HC4R),M(HC4R,HC4R),M(HC4R,HC4V),M(HC4R,HC4R),M(HC4R,HC4V),M(HC4R,HC4R),M(HC4R,HC4V),M(HC4R,HC4R),M(HC4R,HC4V),M(HC4R,HC4R),M(HC4R,HC4V),M(HC4R,HC8R),M(HC4R,HC8V),M(HC4R,HC8R),M(HC4R,HC8V),M(HC4R,BA4F),M(HC4R,BA4F),M(HC4R,BA4F),M(HC4R,BA8F),M(HC4R,INIT)},   /* HC4R */   \
   {M(HC4V,HC4V),M(HC4V,HC4V),M(HC4V,HC4V),M(HC4V,HETE),M(HC4V,HETE),M(HC4V,NEST),M(HC4V,ERR ),M(HC4V,HC4V),M(HC4V,HC4V),M(HC4V,HC4V),M(HC4V,HC4V),M(HC4V,HC4V),M(HC4V,HC4V),M(HC4V,HC4V),M(HC4V,HC4V),M(HC4V,HC4V),M(HC4V,HC4V),M(HC4V,HC4V),M(HC4V,HC8V),M(HC4V,HC8V),M(HC4V,HC8V),M(HC4V,HC8V),M(HC4V,BA4F),M(HC4V,BA4F),M(HC4V,BA4F),M(HC4V,BA8F),M(HC4V,INIT)},   /* HC4V */   \
   {M(HC8I,HC8I),M(HC8I,HC8I),M(HC8I,HC8F),M(HC8I,HETE),M(HC8I,HETE),M(HC8I,NEST),M(HC8I,ERR ),M(HC8I,HC8I),M(HC8I,HC8R),M(HC8I,HC8V),M(HC8I,HC8I),M(HC8I,HC8F),M(HC8I,HC8R),M(HC8I,HC8V),M(HC8I,HC8I),M(HC8I,HC8F),M(HC8I,HC8R),M(HC8I,HC8V),M(HC8I,HC8I),M(HC8I,HC8F),M(HC8I,HC8R),M(HC8I,HC8V),M(HC8I,BA8F),M(HC8I,BA8F),M(HC8I,BA8F),M(HC8I,BA8F),M(HC8I,INIT)},   /* HC8I */   \
   {M(HC8F,HC8F),M(HC8F,HC8F),M(HC8F,HC8F),M(HC8F,HETE),M(HC8F,HETE),M(HC8F,NEST),M(HC8F,ERR ),M(HC8F,HC8F),M(HC8F,HC8V),M(HC8F,HC8V),M(HC8F,HC8F),M(HC8F,HC8F),M(HC8F,HC8V),M(HC8F,HC8V),M(HC8F,HC8F),M(HC8F,HC8F),M(HC8F,HC8V),M(HC8F,HC8V),M(HC8F,HC8F),M(HC8F,HC8F),M(HC8F,HC8V),M(HC8F,HC8V),M(HC8F,BA8F),M(HC8F,BA8F),M(HC8F,BA8F),M(HC8F,BA8F),M(HC8F,INIT)},   /* HC8F */   \
   {M(HC8R,HC8R),M(HC8R,HC8R),M(HC8R,HC8V),M(HC8R,HETE),M(HC8R,HETE),M(HC8R,NEST),M(HC8R,ERR ),M(HC8R,HC8R),M(HC8R,HC8R),M(HC8R,HC8V),M(HC8R,HC8R),M(HC8R,HC8V),M(HC8R,HC8R),M(HC8R,HC8V),M(HC8R,HC8R),M(HC8R,HC8V),M(HC8R,HC8R),M(HC8R,HC8V),M(HC8R,HC8R),M(HC8R,HC8V),M(HC8R,HC8R),M(HC8R,HC8V),M(HC8R,BA8F),M(HC8R,BA8F),M(HC8R,BA8F),M(HC8R,BA8F),M(HC8R,INIT)},   /* HC8R */   \
   {M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HETE),M(HC8V,HETE),M(HC8V,NEST),M(HC8V,ERR ),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,BA8F),M(HC8V,BA8F),M(HC8V,BA8F),M(HC8V,BA8F),M(HC8V,INIT)},   /* HC8V */   \
   {M(BA1F,BA1F),M(BA1F,BA1F),M(BA1F,BA1F),M(BA1F,HETE),M(BA1F,HETE),M(BA1F,NEST),M(BA1F,ERR ),M(BA1F,BA1F),M(BA1F,BA1F),M(BA1F,BA1F),M(BA1F,BA2F),M(BA1F,BA2F),M(BA1F,BA2F),M(BA1F,BA2F),M(BA1F,BA4F),M(BA1F,BA4F),M(BA1F,BA4F),M(BA1F,BA4F),M(BA1F,BA8F),M(BA1F,BA8F),M(BA1F,BA8F),M(BA1F,BA8F),M(BA1F,BA1F),M(BA1F,BA2F),M(BA1F,BA4F),M(BA1F,BA8F),M(BA1F,INIT)},   /* BA1F */   \
   {M(BA2F,BA2F),M(BA2F,BA2F),M(BA2F,BA2F),M(BA2F,HETE),M(BA2F,HETE),M(BA2F,NEST),M(BA2F,ERR ),M(BA2F,BA2F),M(BA2F,BA2F),M(BA2F,BA2F),M(BA2F,BA2F),M(BA2F,BA2F),M(BA2F,BA2F),M(BA2F,BA2F),M(BA2F,BA4F),M(BA2F,BA4F),M(BA2F,BA4F),M(BA2F,BA4F),M(BA2F,BA8F),M(BA2F,BA8F),M(BA2F,BA8F),M(BA2F,BA8F),M(BA2F,BA2F),M(BA2F,BA2F),M(BA2F,BA4F),M(BA2F,BA8F),M(BA2F,INIT)},   /* BA2F */   \
   {M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,HETE),M(BA4F,HETE),M(BA4F,NEST),M(BA4F,ERR ),M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA8F),M(BA4F,BA8F),M(BA4F,BA8F),M(BA4F,BA8F),M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA8F),M(BA4F,INIT)},   /* BA4F */   \
   {M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,HETE),M(BA8F,HETE),M(BA8F,NEST),M(BA8F,ERR ),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,INIT)},   /* BA8F */   \
   {M(INIT,BOOL),M(INIT,INT ),M(INIT,FLT ),M(INIT,CHAR),M(INIT,HETE),M(INIT,NEST),M(INIT,ERR ),M(INIT,INT ),M(INIT,RAT ),M(INIT,VFP ),M(INIT,HC2I),M(INIT,HC2F),M(INIT,HC2R),M(INIT,HC2V),M(INIT,HC4I),M(INIT,HC4F),M(INIT,HC4R),M(INIT,HC4V),M(INIT,HC8I),M(INIT,HC8F),M(INIT,HC8R),M(INIT,HC8V),M(INIT,BA1F),M(INIT,BA2F),M(INIT,BA4F),M(INIT,BA8F),M(INIT,INIT)},   /* INIT */   \
}

// ARRAY_xxx Type Promotion result matrix
APLSTYPE aTypePromote[ARRAY_LENGTH + 1][ARRAY_LENGTH + 1]
#ifdef DEFINE_VALUES

#define ARRAY_FLT       ARRAY_FLOAT
#define ARRAY_HETE      ARRAY_HETERO
#define ARRAY_NEST      ARRAY_NESTED
#define ARRAY_ERR       ARRAY_ERROR
#define ARRAY_INIT      ARRAY_ERROR

#define M(a,b)          ARRAY_##b
= TP_MAT
#undef  M

#undef  ARRAY_INIT
#undef  ARRAY_ERR
#undef  ARRAY_NEST
#undef  ARRAY_HETE
#undef  ARRAY_FLT

#endif
;

//***************************************************************************
//      TPT Definitions
//***************************************************************************

#define TPT_BOOL_INIT   TPT_IDENT
#define TPT_INT_INIT    TPT_IDENT
#define TPT_APA_INIT    TPT_IDENT
#define TPT_FLT_INIT    TPT_IDENT
#define TPT_CHAR_INIT   TPT_IDENT
#define TPT_HETE_INIT   TPT_IDENT
#define TPT_LIST_INIT   TPT_IDENT
#define TPT_NEST_INIT   TPT_IDENT
#define TPT_RAT_INIT    TPT_IDENT
#define TPT_VFP_INIT    TPT_IDENT
#define TPT_HC2I_INIT   TPT_IDENT
#define TPT_HC2F_INIT   TPT_IDENT
#define TPT_HC2R_INIT   TPT_IDENT
#define TPT_HC2V_INIT   TPT_IDENT
#define TPT_HC4I_INIT   TPT_IDENT
#define TPT_HC4F_INIT   TPT_IDENT
#define TPT_HC4R_INIT   TPT_IDENT
#define TPT_HC4V_INIT   TPT_IDENT
#define TPT_HC8I_INIT   TPT_IDENT
#define TPT_HC8F_INIT   TPT_IDENT
#define TPT_HC8R_INIT   TPT_IDENT
#define TPT_HC8V_INIT   TPT_IDENT
#define TPT_BA1F_INIT   TPT_IDENT
#define TPT_BA2F_INIT   TPT_IDENT
#define TPT_BA4F_INIT   TPT_IDENT
#define TPT_BA8F_INIT   TPT_IDENT

#define TPT_BOOL_BOOL   TPT_IDENT
#define TPT_INT_INT     TPT_IDENT
#define TPT_FLT_FLT     TPT_IDENT
#define TPT_CHAR_CHAR   TPT_IDENT
#define TPT_HETE_HETE   TPT_IDENT
#define TPT_HETE_NEST   TPT_IDENT
#define TPT_NEST_NEST   TPT_IDENT
#define TPT_RAT_RAT     TPT_IDENT
#define TPT_VFP_VFP     TPT_IDENT
#define TPT_HC2I_HC2I   TPT_IDENT
#define TPT_HC2F_HC2F   TPT_IDENT
#define TPT_HC2R_HC2R   TPT_IDENT
#define TPT_HC2V_HC2V   TPT_IDENT
#define TPT_HC4I_HC4I   TPT_IDENT
#define TPT_HC4F_HC4F   TPT_IDENT
#define TPT_HC4R_HC4R   TPT_IDENT
#define TPT_HC4V_HC4V   TPT_IDENT
#define TPT_HC8I_HC8I   TPT_IDENT
#define TPT_HC8F_HC8F   TPT_IDENT
#define TPT_HC8R_HC8R   TPT_IDENT
#define TPT_HC8V_HC8V   TPT_IDENT
#define TPT_BA1F_BA1F   TPT_IDENT
#define TPT_BA2F_BA2F   TPT_IDENT
#define TPT_BA4F_BA4F   TPT_IDENT
#define TPT_BA8F_BA8F   TPT_IDENT

#define TPT_BOOL_ERR    TPT_ERROR
#define TPT_INT_ERR     TPT_ERROR
#define TPT_FLT_ERR     TPT_ERROR
#define TPT_CHAR_ERR    TPT_ERROR
#define TPT_HETE_ERR    TPT_ERROR
#define TPT_NEST_ERR    TPT_ERROR
#define TPT_LIST_ERR    TPT_ERROR
#define TPT_APA_ERR     TPT_ERROR
#define TPT_RAT_ERR     TPT_ERROR
#define TPT_VFP_ERR     TPT_ERROR
#define TPT_HC2I_ERR    TPT_ERROR
#define TPT_HC2F_ERR    TPT_ERROR
#define TPT_HC2R_ERR    TPT_ERROR
#define TPT_HC2V_ERR    TPT_ERROR
#define TPT_HC4I_ERR    TPT_ERROR
#define TPT_HC4F_ERR    TPT_ERROR
#define TPT_HC4R_ERR    TPT_ERROR
#define TPT_HC4V_ERR    TPT_ERROR
#define TPT_HC8I_ERR    TPT_ERROR
#define TPT_HC8F_ERR    TPT_ERROR
#define TPT_HC8R_ERR    TPT_ERROR
#define TPT_HC8V_ERR    TPT_ERROR
#define TPT_BA1F_ERR    TPT_ERROR
#define TPT_BA2F_ERR    TPT_ERROR
#define TPT_BA4F_ERR    TPT_ERROR
#define TPT_BA8F_ERR    TPT_ERROR
#define TPT_INIT_ERR    TPT_ERROR

#define TPT_INIT_BOOL   TPT_ERROR
#define TPT_INIT_INT    TPT_ERROR
#define TPT_INIT_FLT    TPT_ERROR
#define TPT_INIT_CHAR   TPT_ERROR
#define TPT_INIT_HETE   TPT_ERROR
#define TPT_INIT_NEST   TPT_ERROR
#define TPT_INIT_LIST   TPT_ERROR
#define TPT_INIT_APA    TPT_ERROR
#define TPT_INIT_RAT    TPT_ERROR
#define TPT_INIT_VFP    TPT_ERROR
#define TPT_INIT_HC2I   TPT_ERROR
#define TPT_INIT_HC2F   TPT_ERROR
#define TPT_INIT_HC2R   TPT_ERROR
#define TPT_INIT_HC2V   TPT_ERROR
#define TPT_INIT_HC4I   TPT_ERROR
#define TPT_INIT_HC4F   TPT_ERROR
#define TPT_INIT_HC4R   TPT_ERROR
#define TPT_INIT_HC4V   TPT_ERROR
#define TPT_INIT_HC8I   TPT_ERROR
#define TPT_INIT_HC8F   TPT_ERROR
#define TPT_INIT_HC8R   TPT_ERROR
#define TPT_INIT_HC8V   TPT_ERROR
#define TPT_INIT_BA1F   TPT_ERROR
#define TPT_INIT_BA2F   TPT_ERROR
#define TPT_INIT_BA4F   TPT_ERROR
#define TPT_INIT_BA8F   TPT_ERROR
#define TPT_INIT_INIT   TPT_ERROR

#define TPT_BOOL_HETE   TPT_ERROR
#define TPT_BOOL_NEST   TPT_INT_NEST
#define TPT_INT_HETE    TPT_ERROR
//efine TPT_INT_NEST    TPT_ERROR
#define TPT_FLT_HETE    TPT_ERROR
//efine TPT_FLT_NEST    TPT_ERROR
#define TPT_CHAR_HETE   TPT_ERROR
#define TPT_CHAR_NEST   TPT_ERROR
#define TPT_APA_HETE    TPT_ERROR
#define TPT_APA_NEST    TPT_INT_NEST
#define TPT_RAT_HETE    TPT_ERROR
#define TPT_VFP_HETE    TPT_ERROR
#define TPT_BA1F_HETE   TPT_ERROR
#define TPT_BA2F_HETE   TPT_ERROR
#define TPT_BA4F_HETE   TPT_ERROR
#define TPT_BA8F_HETE   TPT_ERROR

////ine TPT_BOOL_HC2I
////ine TPT_BOOL_HC2F
////ine TPT_BOOL_HC2R
////ine TPT_BOOL_HC2V
////ine TPT_BOOL_HC4I
////ine TPT_BOOL_HC4F
////ine TPT_BOOL_HC4R
////ine TPT_BOOL_HC4V
////ine TPT_BOOL_HC8I
////ine TPT_BOOL_HC8F
////ine TPT_BOOL_HC8R
////ine TPT_BOOL_HC8V

////ine TPT_INT_HC2I
////ine TPT_INT_HC2F
////ine TPT_INT_HC2R
////ine TPT_INT_HC2V
////ine TPT_INT_HC4I
////ine TPT_INT_HC4F
////ine TPT_INT_HC4R
////ine TPT_INT_HC4V
////ine TPT_INT_HC8I
////ine TPT_INT_HC8F
////ine TPT_INT_HC8R
////ine TPT_INT_HC8V

////ine TPT_FLT_HC2F
////ine TPT_FLT_HC2V
////ine TPT_FLT_HC4F
////ine TPT_FLT_HC4V
////ine TPT_FLT_HC8F
////ine TPT_FLT_HC8V

#define TPT_APA_HC2I    TPT_ERROR
#define TPT_APA_HC2F    TPT_ERROR
#define TPT_APA_HC2R    TPT_ERROR
#define TPT_APA_HC2V    TPT_ERROR
#define TPT_APA_HC4I    TPT_ERROR
#define TPT_APA_HC4F    TPT_ERROR
#define TPT_APA_HC4R    TPT_ERROR
#define TPT_APA_HC4V    TPT_ERROR
#define TPT_APA_HC8I    TPT_ERROR
#define TPT_APA_HC8F    TPT_ERROR
#define TPT_APA_HC8R    TPT_ERROR
#define TPT_APA_HC8V    TPT_ERROR

////ine TPT_RAT_HC2R
////ine TPT_RAT_HC2V
////ine TPT_RAT_HC4R
////ine TPT_RAT_HC4V
////ine TPT_RAT_HC8R
////ine TPT_RAT_HC8V

////ine TPT_VFP_HC2V
////ine TPT_VFP_HC4V
////ine TPT_VFP_HC8V

#define TPT_HC2I_HETE   TPT_ERROR
#define TPT_HC2F_HETE   TPT_ERROR
#define TPT_HC2R_HETE   TPT_ERROR
#define TPT_HC2V_HETE   TPT_ERROR
#define TPT_HC4I_HETE   TPT_ERROR
#define TPT_HC4F_HETE   TPT_ERROR
#define TPT_HC4R_HETE   TPT_ERROR
#define TPT_HC4V_HETE   TPT_ERROR
#define TPT_HC8I_HETE   TPT_ERROR
#define TPT_HC8F_HETE   TPT_ERROR
#define TPT_HC8R_HETE   TPT_ERROR
#define TPT_HC8V_HETE   TPT_ERROR

#define TPT_HC2I_NEST   TPT_ERROR
#define TPT_HC2F_NEST   TPT_ERROR
#define TPT_HC2R_NEST   TPT_ERROR
#define TPT_HC2V_NEST   TPT_ERROR
#define TPT_HC4I_NEST   TPT_ERROR
#define TPT_HC4F_NEST   TPT_ERROR
#define TPT_HC4R_NEST   TPT_ERROR
#define TPT_HC4V_NEST   TPT_ERROR
#define TPT_HC8I_NEST   TPT_ERROR
#define TPT_HC8F_NEST   TPT_ERROR
#define TPT_HC8R_NEST   TPT_ERROR
#define TPT_HC8V_NEST   TPT_ERROR

////ine TPT_HC2I_HC2I
#define TPT_HC2I_HC2F   TPT_ERROR
#define TPT_HC2I_HC2R   TPT_ERROR
#define TPT_HC2I_HC2V   TPT_ERROR
#define TPT_HC2I_BA2F   TPT_ERROR

#define TPT_HC2I_HC4I   TPT_ERROR
#define TPT_HC2I_HC4F   TPT_ERROR
#define TPT_HC2I_HC4R   TPT_ERROR
#define TPT_HC2I_HC4V   TPT_ERROR
#define TPT_HC2I_BA4F   TPT_ERROR

#define TPT_HC2I_HC8I   TPT_ERROR
#define TPT_HC2I_HC8F   TPT_ERROR
#define TPT_HC2I_HC8R   TPT_ERROR
#define TPT_HC2I_HC8V   TPT_ERROR
#define TPT_HC2I_BA8F   TPT_ERROR

////ine TPT_HC2F_HC2F
#define TPT_HC2F_HC2V   TPT_ERROR
#define TPT_HC2F_HC4F   TPT_ERROR
#define TPT_HC2F_HC4V   TPT_ERROR
#define TPT_HC2F_HC8F   TPT_ERROR
#define TPT_HC2F_HC8V   TPT_ERROR
#define TPT_HC2F_BA2F   TPT_ERROR
#define TPT_HC2F_BA4F   TPT_ERROR
#define TPT_HC2F_BA8F   TPT_ERROR

////ine TPT_HC2R_HC2R
#define TPT_HC2R_HC2V   TPT_ERROR
#define TPT_HC2R_HC4R   TPT_ERROR
#define TPT_HC2R_HC4V   TPT_ERROR
#define TPT_HC2R_HC8R   TPT_ERROR
#define TPT_HC2R_HC8V   TPT_ERROR
#define TPT_HC2R_BA2F   TPT_ERROR
#define TPT_HC2R_BA4F   TPT_ERROR
#define TPT_HC2R_BA8F   TPT_ERROR

////ine TPT_HC2V_HC2V
#define TPT_HC2V_HC4V   TPT_ERROR
#define TPT_HC2V_HC8V   TPT_ERROR
#define TPT_HC2V_BA2F   TPT_ERROR
#define TPT_HC2V_BA4F   TPT_ERROR
#define TPT_HC2V_BA8F   TPT_ERROR

////ine TPT_HC4I_HC4I
#define TPT_HC4I_HC4F   TPT_ERROR
#define TPT_HC4I_HC4R   TPT_ERROR
#define TPT_HC4I_HC4V   TPT_ERROR
#define TPT_HC4I_BA4F   TPT_ERROR

#define TPT_HC4I_HC8I   TPT_ERROR
#define TPT_HC4I_HC8F   TPT_ERROR
#define TPT_HC4I_HC8R   TPT_ERROR
#define TPT_HC4I_HC8V   TPT_ERROR
#define TPT_HC4I_BA8F   TPT_ERROR

////ine TPT_HC4F_HC4F
#define TPT_HC4F_HC4V   TPT_ERROR
#define TPT_HC4F_BA4F   TPT_ERROR

#define TPT_HC4F_HC8F   TPT_ERROR
#define TPT_HC4F_HC8V   TPT_ERROR
#define TPT_HC4F_BA8F   TPT_ERROR

////ine TPT_HC4R_HC4R
#define TPT_HC4R_HC4V   TPT_ERROR
#define TPT_HC4R_BA4F   TPT_ERROR

#define TPT_HC4R_HC8R   TPT_ERROR
#define TPT_HC4R_HC8V   TPT_ERROR
#define TPT_HC4R_BA8F   TPT_ERROR

#define TPT_HC4V_HC8V   TPT_ERROR
#define TPT_HC4V_BA4F   TPT_ERROR
#define TPT_HC4V_BA8F   TPT_ERROR

////ine TPT_HC8I_HC8I
#define TPT_HC8I_HC8F   TPT_ERROR
#define TPT_HC8I_HC8R   TPT_ERROR
#define TPT_HC8I_HC8V   TPT_ERROR
#define TPT_HC8I_BA8F   TPT_ERROR

////ine TPT_HC8F_HC8F
#define TPT_HC8F_HC8V   TPT_ERROR
#define TPT_HC8F_BA8F   TPT_ERROR

////ine TPT_HC8R_HC8R
#define TPT_HC8R_HC8V   TPT_ERROR
#define TPT_HC8R_BA8F   TPT_ERROR

////ine TPT_HC8V_HC8V
#define TPT_HC8V_BA8F   TPT_ERROR


#define TPT_BA2F_HETE   TPT_ERROR
#define TPT_BA2F_NEST   TPT_ERROR
#define TPT_BA2F_BA4F   TPT_ERROR
#define TPT_BA2F_BA8F   TPT_ERROR
#define TPT_BA4F_HETE   TPT_ERROR
#define TPT_BA4F_NEST   TPT_ERROR
#define TPT_BA4F_BA8F   TPT_ERROR
#define TPT_BA8F_HETE   TPT_ERROR
#define TPT_BA8F_NEST   TPT_ERROR

// Type Promotion Token matrix
TPT_ACTION aTypeTknPromote[ARRAY_LENGTH + 1][ARRAY_LENGTH + 1]
#ifdef DEFINE_VALUES

#define M(a,b)          TPT_##a##_##b
 = TP_MAT
#undef  M

#endif
;

#undef  TPT_HC8F_HC8V
////ef  TPT_HC8F_HC8F

#undef  TPT_HC8R_HC8V
////ef  TPT_HC8R_HC8R

#undef  TPT_HC8I_HC8V
#undef  TPT_HC8I_HC8R
#undef  TPT_HC8I_HC8F
////ef  TPT_HC8I_HC8I

#undef  TPT_HC4V_HC8V

#undef  TPT_HC4R_HC8V
#undef  TPT_HC4R_HC8R
#undef  TPT_HC4R_HC4V
////ef  TPT_HC4R_HC4R

#undef  TPT_HC4F_HC8V
#undef  TPT_HC4F_HC8F
#undef  TPT_HC4F_HC4V
////ef  TPT_HC4F_HC4F

#undef  TPT_HC4I_HC8V
#undef  TPT_HC4I_HC8R
#undef  TPT_HC4I_HC8F
#undef  TPT_HC4I_HC8I

#undef  TPT_HC4I_HC4V
#undef  TPT_HC4I_HC4R
#undef  TPT_HC4I_HC4F
////ef  TPT_HC4I_HC4I

#undef  TPT_HC2V_HC8V
#undef  TPT_HC2V_HC4V
////ef  TPT_HC2V_HC2V

#undef  TPT_HC2R_HC8V
#undef  TPT_HC2R_HC8R
#undef  TPT_HC2R_HC4V
#undef  TPT_HC2R_HC4R
#undef  TPT_HC2R_HC2V
////ef  TPT_HC2R_HC2R

#undef  TPT_HC2F_HC8V
#undef  TPT_HC2F_HC8F
#undef  TPT_HC2F_HC4V
#undef  TPT_HC2F_HC4F
#undef  TPT_HC2F_HC2V
////ef  TPT_HC2F_HC2F

#undef  TPT_HC2I_HC8V
#undef  TPT_HC2I_HC8R
#undef  TPT_HC2I_HC8F
#undef  TPT_HC2I_HC8I

#undef  TPT_HC2I_HC4V
#undef  TPT_HC2I_HC4R
#undef  TPT_HC2I_HC4F
#undef  TPT_HC2I_HC4I

#undef  TPT_HC2I_HC2V
#undef  TPT_HC2I_HC2R
#undef  TPT_HC2I_HC2F
////ef  TPT_HC2I_HC2I

#undef  TPT_HC8V_NEST
#undef  TPT_HC8R_NEST
#undef  TPT_HC8F_NEST
#undef  TPT_HC8I_NEST
#undef  TPT_HC4V_NEST
#undef  TPT_HC4R_NEST
#undef  TPT_HC4F_NEST
#undef  TPT_HC4I_NEST
#undef  TPT_HC2V_NEST
#undef  TPT_HC2R_NEST
#undef  TPT_HC2F_NEST
#undef  TPT_HC2I_NEST

#undef  TPT_HC8V_HETE
#undef  TPT_HC8R_HETE
#undef  TPT_HC8F_HETE
#undef  TPT_HC8I_HETE
#undef  TPT_HC4V_HETE
#undef  TPT_HC4R_HETE
#undef  TPT_HC4F_HETE
#undef  TPT_HC4I_HETE
#undef  TPT_HC2V_HETE
#undef  TPT_HC2R_HETE
#undef  TPT_HC2F_HETE
#undef  TPT_HC2I_HETE

#undef  TPT_VFP_HC8V
#undef  TPT_VFP_HC4V
#undef  TPT_VFP_HC2V

#undef  TPT_RAT_HC8V
#undef  TPT_RAT_HC8R
#undef  TPT_RAT_HC4V
#undef  TPT_RAT_HC4R
#undef  TPT_RAT_HC2V
#undef  TPT_RAT_HC2R

#undef  TPT_APA_HC8V
#undef  TPT_APA_HC8R
#undef  TPT_APA_HC8F
#undef  TPT_APA_HC8I
#undef  TPT_APA_HC4V
#undef  TPT_APA_HC4R
#undef  TPT_APA_HC4F
#undef  TPT_APA_HC4I
#undef  TPT_APA_HC2V
#undef  TPT_APA_HC2R
#undef  TPT_APA_HC2F
#undef  TPT_APA_HC2I

#undef  TPT_FLT_HC8V
#undef  TPT_FLT_HC8R
#undef  TPT_FLT_HC8F
#undef  TPT_FLT_HC4V
#undef  TPT_FLT_HC4R
#undef  TPT_FLT_HC4F
#undef  TPT_FLT_HC2V
#undef  TPT_FLT_HC2R
#undef  TPT_FLT_HC2F

#undef  TPT_INT_HC8V
#undef  TPT_INT_HC8R
#undef  TPT_INT_HC8F
#undef  TPT_INT_HC8I
#undef  TPT_INT_HC4V
#undef  TPT_INT_HC4R
#undef  TPT_INT_HC4F
#undef  TPT_INT_HC4I
#undef  TPT_INT_HC2V
#undef  TPT_INT_HC2R
#undef  TPT_INT_HC2F
#undef  TPT_INT_HC2I

#undef  TPT_BOOL_HC8V
#undef  TPT_BOOL_HC8R
#undef  TPT_BOOL_HC8F
#undef  TPT_BOOL_HC8I
#undef  TPT_BOOL_HC4V
#undef  TPT_BOOL_HC4R
#undef  TPT_BOOL_HC4F
#undef  TPT_BOOL_HC4I
#undef  TPT_BOOL_HC2V
#undef  TPT_BOOL_HC2R
#undef  TPT_BOOL_HC2F
#undef  TPT_BOOL_HC2I

#undef  TPT_VFP_NEST
#undef  TPT_VFP_HETE
#undef  TPT_RAT_NEST
#undef  TPT_RAT_HETE
#undef  TPT_APA_NEST
#undef  TPT_APA_HETE
#undef  TPT_CHAR_NEST
#undef  TPT_CHAR_HETE
#undef  TPT_FLT_NEST
#undef  TPT_FLT_HETE
#undef  TPT_INT_NEST
#undef  TPT_INT_HETE
#undef  TPT_BOOL_NEST
#undef  TPT_BOOL_HETE


#undef  TPT_INIT_INIT
#undef  TPT_INIT_HC8V
#undef  TPT_INIT_HC8R
#undef  TPT_INIT_HC8F
#undef  TPT_INIT_HC4V
#undef  TPT_INIT_HC4R
#undef  TPT_INIT_HC4F
#undef  TPT_INIT_HC2V
#undef  TPT_INIT_HC2R
#undef  TPT_INIT_HC2F
#undef  TPT_INIT_VFP
#undef  TPT_INIT_RAT
#undef  TPT_INIT_APA
#undef  TPT_INIT_LIST
#undef  TPT_INIT_NEST
#undef  TPT_INIT_HETE
#undef  TPT_INIT_CHAR
#undef  TPT_INIT_FLT
#undef  TPT_INIT_INT
#undef  TPT_INIT_BOOL

#undef  TPT_INIT_ERR
#undef  TPT_HC8V_ERR
#undef  TPT_HC8R_ERR
#undef  TPT_HC8F_ERR
#undef  TPT_HC8I_ERR
#undef  TPT_HC4V_ERR
#undef  TPT_HC4R_ERR
#undef  TPT_HC4F_ERR
#undef  TPT_HC4I_ERR
#undef  TPT_HC2V_ERR
#undef  TPT_HC2R_ERR
#undef  TPT_HC2F_ERR
#undef  TPT_HC2I_ERR
#undef  TPT_VFP_ERR
#undef  TPT_RAT_ERR
#undef  TPT_APA_ERR
#undef  TPT_LIST_ERR
#undef  TPT_NEST_ERR
#undef  TPT_HETE_ERR
#undef  TPT_CHAR_ERR
#undef  TPT_FLT_ERR
#undef  TPT_INT_ERR
#undef  TPT_BOOL_ERR

#undef  TPT_HC8VHC8V
#undef  TPT_HC8RHC8R
#undef  TPT_HC8FHC8F
#undef  TPT_HC8IHC8I
#undef  TPT_HC4VHC4V
#undef  TPT_HC4RHC4R
#undef  TPT_HC4FHC4F
#undef  TPT_HC4IHC4I
#undef  TPT_HC2VHC2V
#undef  TPT_HC2RHC2R
#undef  TPT_HC2FHC2F
#undef  TPT_HC2IHC2I
#undef  TPT_VFP_VFP
#undef  TPT_RAT_RAT
#undef  TPT_NEST_NEST
#undef  TPT_HETE_NEST
#undef  TPT_HETE_HETE
#undef  TPT_CHAR_CHAR
#undef  TPT_FLT_FLT
#undef  TPT_INT_INT
#undef  TPT_BOOL_BOOL


//***************************************************************************
//      TPA Definitions
//***************************************************************************

#define TPA_BOOL_ERR    TPA_ERROR
#define TPA_INT_ERR     TPA_ERROR
#define TPA_FLT_ERR     TPA_ERROR
#define TPA_CHAR_ERR    TPA_ERROR
#define TPA_HETE_ERR    TPA_ERROR
#define TPA_NEST_ERR    TPA_ERROR
#define TPA_LIST_ERR    TPA_ERROR
#define TPA_APA_ERR     TPA_ERROR
#define TPA_RAT_ERR     TPA_ERROR
#define TPA_VFP_ERR     TPA_ERROR
#define TPA_BA1F_ERR    TPA_ERROR
#define TPA_BA2F_ERR    TPA_ERROR
#define TPA_BA4F_ERR    TPA_ERROR
#define TPA_BA8F_ERR    TPA_ERROR
#define TPA_INIT_ERR    TPA_ERROR

#define TPA_BOOL_INIT   TPA_ERROR
#define TPA_INT_INIT    TPA_ERROR
#define TPA_FLT_INIT    TPA_ERROR
#define TPA_CHAR_INIT   TPA_ERROR
#define TPA_HETE_INIT   TPA_ERROR
#define TPA_NEST_INIT   TPA_ERROR
#define TPA_LIST_INIT   TPA_ERROR
#define TPA_APA_INIT    TPA_ERROR
#define TPA_RAT_INIT    TPA_ERROR
#define TPA_VFP_INIT    TPA_ERROR
#define TPA_HC2I_INIT   TPA_ERROR
#define TPA_HC2F_INIT   TPA_ERROR
#define TPA_HC2R_INIT   TPA_ERROR
#define TPA_HC2V_INIT   TPA_ERROR
#define TPA_HC4I_INIT   TPA_ERROR
#define TPA_HC4F_INIT   TPA_ERROR
#define TPA_HC4R_INIT   TPA_ERROR
#define TPA_HC4V_INIT   TPA_ERROR
#define TPA_HC8I_INIT   TPA_ERROR
#define TPA_HC8F_INIT   TPA_ERROR
#define TPA_HC8R_INIT   TPA_ERROR
#define TPA_HC8V_INIT   TPA_ERROR

#define TPA_BA1F_INIT   TPA_ERROR
#define TPA_BA2F_INIT   TPA_ERROR
#define TPA_BA4F_INIT   TPA_ERROR
#define TPA_BA8F_INIT   TPA_ERROR

#define TPA_INIT_BOOL   TPA_ERROR
#define TPA_INIT_INT    TPA_ERROR
#define TPA_INIT_FLT    TPA_ERROR
#define TPA_INIT_CHAR   TPA_ERROR
#define TPA_INIT_HETE   TPA_ERROR
#define TPA_INIT_NEST   TPA_ERROR
#define TPA_INIT_LIST   TPA_ERROR
#define TPA_INIT_APA    TPA_ERROR
#define TPA_INIT_RAT    TPA_ERROR
#define TPA_INIT_VFP    TPA_ERROR
#define TPA_INIT_BA1F   TPA_ERROR
#define TPA_INIT_BA2F   TPA_ERROR
#define TPA_INIT_BA4F   TPA_ERROR
#define TPA_INIT_BA8F   TPA_ERROR
#define TPA_INIT_INIT   TPA_ERROR

#define TPA_BOOL_HC2I   TPA_BOOL_INT
#define TPA_BOOL_HC2F   TPA_BOOL_FLT
////ine TPA_BOOL_HC2R
////ine TPA_BOOL_HC2V
#define TPA_BOOL_HC4I   TPA_BOOL_HC2I
#define TPA_BOOL_HC4F   TPA_BOOL_HC2F
////ine TPA_BOOL_HC4R
////ine TPA_BOOL_HC4V
#define TPA_BOOL_HC8I   TPA_BOOL_HC2I
#define TPA_BOOL_HC8F   TPA_BOOL_HC2F
////ine TPA_BOOL_HC8R
////ine TPA_BOOL_HC8V

#define TPA_INT_HC2I    TPA_INT_INT
#define TPA_INT_HC2F    TPA_INT_FLT
////ine TPA_INT_HC2R
////ine TPA_INT_HC2V
#define TPA_INT_HC4I    TPA_INT_HC2I
#define TPA_INT_HC4F    TPA_INT_HC2F
////ine TPA_INT_HC4R
////ine TPA_INT_HC4V
#define TPA_INT_HC8I    TPA_INT_HC2I
#define TPA_INT_HC8F    TPA_INT_HC2F
////ine TPA_INT_HC8R
////ine TPA_INT_HC8V

#define TPA_FLT_HC2F    TPA_FLT_FLT
////ine TPA_FLT_HC2V
#define TPA_FLT_HC4F    TPA_FLT_FLT
////ine TPA_FLT_HC4V
#define TPA_FLT_HC8F    TPA_FLT_FLT
////ine TPA_FLT_HC8V

#define TPA_APA_HC2I    TPA_APA_INT
#define TPA_APA_HC2F    TPA_APA_FLT
////ine TPA_APA_HC2R
////ine TPA_APA_HC2V
#define TPA_APA_HC4I    TPA_APA_INT
#define TPA_APA_HC4F    TPA_APA_FLT
////ine TPA_APA_HC4R
////ine TPA_APA_HC4V
#define TPA_APA_HC8I    TPA_APA_INT
#define TPA_APA_HC8F    TPA_APA_FLT
////ine TPA_APA_HC8R
////ine TPA_APA_HC8V

////ine TPA_RAT_HC2R
////ine TPA_RAT_HC2V
////ine TPA_RAT_HC4R
////ine TPA_RAT_HC4V
////ine TPA_RAT_HC8R
////ine TPA_RAT_HC8V

////ine TPA_VFP_HC2V
////ine TPA_VFP_HC4V
////ine TPA_VFP_HC8V

////ine TPA_HC2I_HETE
////ine TPA_HC2I_NEST
#define TPA_HC2I_ERR    TPA_ERROR
////ine TPA_HC2I_HC2I
////ine TPA_HC2I_HC2F
////ine TPA_HC2I_HC2R
////ine TPA_HC2I_HC2V
#define TPA_HC2I_HC4I   TPA_HC2I_HC2I
#define TPA_HC2I_HC4F   TPA_HC2I_HC2F
////ine TPA_HC2I_HC4R
////ine TPA_HC2I_HC4V
#define TPA_HC2I_HC8I   TPA_HC2I_HC2I
#define TPA_HC2I_HC8F   TPA_HC2I_HC2F
////ine TPA_HC2I_HC8R
////ine TPA_HC2I_HC8V

////ine TPA_HC2F_HC2F
////ine TPA_HC2F_HETE
////ine TPA_HC2F_NEST
#define TPA_HC2F_ERR    TPA_ERROR
////ine TPA_HC2F_HC2V
#define TPA_HC2F_HC4F   TPA_HC2F_HC2F
////ine TPA_HC2F_HC4V
#define TPA_HC2F_HC8F   TPA_HC2F_HC2F
////ine TPA_HC2F_HC8V

////ine TPA_HC4I_HETE
////ine TPA_HC4I_NEST
#define TPA_HC4I_ERR    TPA_ERROR
////ine TPA_HC4I_HC4I
////ine TPA_HC4I_HC4F
////ine TPA_HC4I_HC4R
////ine TPA_HC4I_HC4V
#define TPA_HC4I_HC8I   TPA_HC4I_HC4I
#define TPA_HC4I_HC8F   TPA_HC4I_HC4F
////ine TPA_HC4I_HC8R
////ine TPA_HC4I_HC8V

////ine TPA_HC4F_HETE
////ine TPA_HC4F_NEST
#define TPA_HC4F_ERR    TPA_ERROR
////ine TPA_HC4F_HC4F
////ine TPA_HC4F_HC4V
#define TPA_HC4F_HC8F   TPA_HC4F_HC4F
////ine TPA_HC4F_HC8V

////ine TPA_HC8I_HETE
////ine TPA_HC8I_NEST
#define TPA_HC8I_ERR    TPA_ERROR
////ine TPA_HC8I_HC8I
////ine TPA_HC8I_HC8F
////ine TPA_HC8I_HC8R
////ine TPA_HC8I_HC8V

////ine TPA_HC8F_HETE
////ine TPA_HC8F_NEST
#define TPA_HC8F_ERR    TPA_ERROR
////ine TPA_HC8F_HC8F
////ine TPA_HC8F_HC8V

////ine TPA_HC2R_HETE
////ine TPA_HC2R_NEST
#define TPA_HC2R_ERR    TPA_ERROR
////ine TPA_HC2R_HC2R
////ine TPA_HC2R_HC2V
////ine TPA_HC2R_HC4R
////ine TPA_HC2R_HC4V
////ine TPA_HC2R_HC8R
////ine TPA_HC2R_HC8V

////ine TPA_HC4R_HETE
////ine TPA_HC4R_NEST
#define TPA_HC4R_ERR    TPA_ERROR
////ine TPA_HC4R_HC4R
////ine TPA_HC4R_HC4V
////ine TPA_HC4R_HC8R
////ine TPA_HC4R_HC8V

////ine TPA_HC8R_HETE
////ine TPA_HC8R_NEST
#define TPA_HC8R_ERR    TPA_ERROR
////ine TPA_HC8R_HC8R
////ine TPA_HC8R_HC8V

////ine TPA_HC2V_HETE
////ine TPA_HC2V_NEST
#define TPA_HC2V_ERR    TPA_ERROR
////ine TPA_HC2V_HC2V
////ine TPA_HC2V_HC4V
////ine TPA_HC2V_HC8V

////ine TPA_HC4V_HETE
////ine TPA_HC4V_NEST
#define TPA_HC4V_ERR    TPA_ERROR
////ine TPA_HC4V_HC4V
////ine TPA_HC4V_HC8V

////ine TPA_HC8V_HETE
////ine TPA_HC8V_NEST
#define TPA_HC8V_ERR    TPA_ERROR
////ine TPA_HC8V_HC8V

#define TPA_INIT_HC2I   TPA_ERROR
#define TPA_INIT_HC2F   TPA_ERROR
#define TPA_INIT_HC2R   TPA_ERROR
#define TPA_INIT_HC2V   TPA_ERROR

#define TPA_INIT_HC4I   TPA_ERROR
#define TPA_INIT_HC4F   TPA_ERROR
#define TPA_INIT_HC4R   TPA_ERROR
#define TPA_INIT_HC4V   TPA_ERROR

#define TPA_INIT_HC8I   TPA_ERROR
#define TPA_INIT_HC8F   TPA_ERROR
#define TPA_INIT_HC8R   TPA_ERROR
#define TPA_INIT_HC8V   TPA_ERROR


// Type Promotion Action matrix
TP_ACTION aTypeActPromote[ARRAY_LENGTH + 1][ARRAY_LENGTH + 1]
#ifdef DEFINE_VALUES

#define M(a,b)          TPA_##a##_##b
 = TP_MAT
#undef  M

#endif
;

#undef  TPA_INIT_INIT
#undef  TPA_INIT_VFP
#undef  TPA_INIT_RAT
#undef  TPA_INIT_APA
#undef  TPA_INIT_LIST
#undef  TPA_INIT_NEST
#undef  TPA_INIT_HETE
#undef  TPA_INIT_CHAR
#undef  TPA_INIT_FLT
#undef  TPA_INIT_INT
#undef  TPA_INIT_BOOL

#undef  TPA_INIT_ERR
#undef  TPA_VFP_ERR
#undef  TPA_RAT_ERR
#undef  TPA_APA_ERR
#undef  TPA_LIST_ERR
#undef  TPA_NEST_ERR
#undef  TPA_HETE_ERR
#undef  TPA_CHAR_ERR
#undef  TPA_FLT_ERR
#undef  TPA_INT_ERR
#undef  TPA_BOOL_ERR

//***************************************************************************
//      TCA Definitions
//***************************************************************************


#define TC_MAT                                                                                                                                                                                                                                                                                                                                                          \
{/*     BOOL          INT          FLT         CHAR         HETERO       NESTED       LIST          APA          RAT          VFP       HC2I         HC2F         HC2R         HC2V         HC4I         HC4F         HC4R         HC4V         HC8I         HC8F         HC8R         HC8V         BA1F         BA2F         BA4F         BA8F       */                \
   {M(BOOL,BOOL),M(BOOL,INT ),M(BOOL,FLT ),M(BOOL,ERR ),M(BOOL,ERR ),M(BOOL,ERR ),M(BOOL,ERR ),M(BOOL,ERR ),M(BOOL,RAT ),M(BOOL,VFP ),M(BOOL,HC2I),M(BOOL,HC2F),M(BOOL,HC2R),M(BOOL,HC2V),M(BOOL,HC4I),M(BOOL,HC4F),M(BOOL,HC4R),M(BOOL,HC4V),M(BOOL,HC8I),M(BOOL,HC8F),M(BOOL,HC8R),M(BOOL,HC8V),M(BOOL,BA1F),M(BOOL,BA2F),M(BOOL,BA4F),M(BOOL,BA8F)},     /* BOOL */  \
   {M(INT ,BOOL),M(INT ,INT ),M(INT ,FLT ),M(INT ,ERR ),M(INT ,ERR ),M(INT ,ERR ),M(INT ,ERR ),M(INT ,ERR ),M(INT ,RAT ),M(INT ,VFP ),M(INT ,HC2I),M(INT ,HC2F),M(INT ,HC2R),M(INT ,HC2V),M(INT ,HC4I),M(INT ,HC4F),M(INT ,HC4R),M(INT ,HC4V),M(INT ,HC8I),M(INT ,HC8F),M(INT ,HC8R),M(INT ,HC8V),M(INT ,BA1F),M(INT ,BA2F),M(INT ,BA4F),M(INT ,BA8F)},     /* INT  */  \
   {M(FLT ,BOOL),M(FLT ,INT ),M(FLT ,FLT ),M(FLT ,ERR ),M(FLT ,ERR ),M(FLT ,ERR ),M(FLT ,ERR ),M(FLT ,ERR ),M(FLT ,RAT ),M(FLT ,VFP ),M(FLT ,HC2I),M(FLT ,HC2F),M(FLT ,HC2R),M(FLT ,HC2V),M(FLT ,HC4I),M(FLT ,HC4F),M(FLT ,HC4R),M(FLT ,HC4V),M(FLT ,HC8I),M(FLT ,HC8F),M(FLT ,HC8R),M(FLT ,HC8V),M(FLT ,BA1F),M(FLT ,BA2F),M(FLT ,BA4F),M(FLT ,BA8F)},     /* FLT  */  \
   {M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,CHAR),M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,ERR )},     /* CHAR */  \
   {M(HETE,BOOL),M(HETE,INT ),M(HETE,FLT ),M(HETE,CHAR),M(HETE,HETE),M(HETE,ERR ),M(HETE,ERR ),M(HETE,ERR ),M(HETE,ERR ),M(HETE,ERR ),M(HETE,ERR ),M(HETE,ERR ),M(HETE,ERR ),M(HETE,ERR ),M(HETE,ERR ),M(HETE,ERR ),M(HETE,ERR ),M(HETE,ERR ),M(HETE,ERR ),M(HETE,ERR ),M(HETE,ERR ),M(HETE,ERR ),M(HETE,ERR ),M(HETE,ERR ),M(HETE,ERR ),M(HETE,ERR )},     /* HETE */  \
   {M(NEST,BOOL),M(NEST,INT ),M(NEST,FLT ),M(NEST,CHAR),M(NEST,ERR ),M(NEST,NEST),M(NEST,ERR ),M(NEST,ERR ),M(NEST,ERR ),M(NEST,ERR ),M(NEST,ERR ),M(NEST,ERR ),M(NEST,ERR ),M(NEST,ERR ),M(NEST,ERR ),M(NEST,ERR ),M(NEST,ERR ),M(NEST,ERR ),M(NEST,ERR ),M(NEST,ERR ),M(NEST,ERR ),M(NEST,ERR ),M(NEST,ERR ),M(NEST,ERR ),M(NEST,ERR ),M(NEST,ERR )},     /* NEST */  \
   {M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR )},     /* LIST */  \
   {M(APA ,BOOL),M(APA ,INT ),M(APA ,FLT ),M(APA ,ERR ),M(APA ,ERR ),M(APA ,ERR ),M(APA ,ERR ),M(APA ,ERR ),M(APA ,RAT ),M(APA ,VFP ),M(APA ,HC2I),M(APA ,HC2F),M(APA ,HC2R),M(APA ,HC2V),M(APA ,HC4I),M(APA ,HC4F),M(APA ,HC4R),M(APA ,HC4V),M(APA ,HC8I),M(APA ,HC8F),M(APA ,HC8R),M(APA ,HC8V),M(APA ,BA1F),M(APA ,BA2F),M(APA ,BA4F),M(APA ,BA8F)},     /* APA  */  \
   {M(RAT ,BOOL),M(RAT ,INT ),M(RAT ,FLT ),M(RAT ,ERR ),M(RAT ,ERR ),M(RAT ,ERR ),M(RAT ,ERR ),M(RAT ,ERR ),M(RAT ,RAT ),M(RAT ,VFP ),M(RAT ,HC2I),M(RAT ,HC2F),M(RAT ,HC2R),M(RAT ,HC2V),M(RAT ,HC4I),M(RAT ,HC4F),M(RAT ,HC4R),M(RAT ,HC4V),M(RAT ,HC8I),M(RAT ,HC8F),M(RAT ,HC8R),M(RAT ,HC8V),M(RAT ,BA1F),M(RAT ,BA2F),M(RAT ,BA4F),M(RAT ,BA8F)},     /* RAT  */  \
   {M(VFP ,BOOL),M(VFP ,INT ),M(VFP ,FLT ),M(VFP ,ERR ),M(VFP ,ERR ),M(VFP ,ERR ),M(VFP ,ERR ),M(VFP ,ERR ),M(VFP ,RAT ),M(VFP ,VFP ),M(VFP ,HC2I),M(VFP ,HC2F),M(VFP ,HC2R),M(VFP ,HC2V),M(VFP ,HC4I),M(VFP ,HC4F),M(VFP ,HC4R),M(VFP ,HC4V),M(VFP ,HC8I),M(VFP ,HC8F),M(VFP ,HC8R),M(VFP ,HC8V),M(VFP ,BA1F),M(VFP ,BA2F),M(VFP ,BA4F),M(VFP ,BA8F)},     /* VFP  */  \
   {M(HC2I,BOOL),M(HC2I,INT ),M(HC2I,FLT ),M(HC2I,ERR ),M(HC2I,ERR ),M(HC2I,ERR ),M(HC2I,ERR ),M(HC2I,ERR ),M(HC2I,RAT ),M(HC2I,VFP ),M(HC2I,HC2I),M(HC2I,HC2F),M(HC2I,HC2R),M(HC2I,HC2V),M(HC2I,HC4I),M(HC2I,HC4F),M(HC2I,HC4R),M(HC2I,HC4V),M(HC2I,HC8I),M(HC2I,HC8F),M(HC2I,HC8R),M(HC2I,HC8V),M(HC2I,BA1F),M(HC2I,BA2F),M(HC2I,BA4F),M(HC2I,BA8F)},     /* HC2I */  \
   {M(HC2F,BOOL),M(HC2F,INT ),M(HC2F,FLT ),M(HC2F,ERR ),M(HC2F,ERR ),M(HC2F,ERR ),M(HC2F,ERR ),M(HC2F,ERR ),M(HC2F,RAT ),M(HC2F,VFP ),M(HC2F,HC2I),M(HC2F,HC2F),M(HC2F,HC2R),M(HC2F,HC2V),M(HC2F,HC4I),M(HC2F,HC4F),M(HC2F,HC4R),M(HC2F,HC4V),M(HC2F,HC8I),M(HC2F,HC8F),M(HC2F,HC8R),M(HC2F,HC8V),M(HC2F,BA1F),M(HC2F,BA2F),M(HC2F,BA4F),M(HC2F,BA8F)},     /* HC2F */  \
   {M(HC2R,BOOL),M(HC2R,INT ),M(HC2R,FLT ),M(HC2R,ERR ),M(HC2R,ERR ),M(HC2R,ERR ),M(HC2R,ERR ),M(HC2R,ERR ),M(HC2R,RAT ),M(HC2R,VFP ),M(HC2R,HC2I),M(HC2R,HC2F),M(HC2R,HC2R),M(HC2R,HC2V),M(HC2R,HC4I),M(HC2R,HC4F),M(HC2R,HC4R),M(HC2R,HC4V),M(HC2R,HC8I),M(HC2R,HC8F),M(HC2R,HC8R),M(HC2R,HC8V),M(HC2R,BA1F),M(HC2R,BA2F),M(HC2R,BA4F),M(HC2R,BA8F)},     /* HC2R */  \
   {M(HC2V,BOOL),M(HC2V,INT ),M(HC2V,FLT ),M(HC2V,ERR ),M(HC2V,ERR ),M(HC2V,ERR ),M(HC2V,ERR ),M(HC2V,ERR ),M(HC2V,RAT ),M(HC2V,VFP ),M(HC2V,HC2I),M(HC2V,HC2F),M(HC2V,HC2R),M(HC2V,HC2V),M(HC2V,HC4I),M(HC2V,HC4F),M(HC2V,HC4R),M(HC2V,HC4V),M(HC2V,HC8I),M(HC2V,HC8F),M(HC2V,HC8R),M(HC2V,HC8V),M(HC2V,BA1F),M(HC2V,BA2F),M(HC2V,BA4F),M(HC2V,BA8F)},     /* HC2V */  \
   {M(HC4I,BOOL),M(HC4I,INT ),M(HC4I,FLT ),M(HC4I,ERR ),M(HC4I,ERR ),M(HC4I,ERR ),M(HC4I,ERR ),M(HC4I,ERR ),M(HC4I,RAT ),M(HC4I,VFP ),M(HC4I,HC2I),M(HC4I,HC2F),M(HC4I,HC2R),M(HC4I,HC2V),M(HC4I,HC4I),M(HC4I,HC4F),M(HC4I,HC4R),M(HC4I,HC4V),M(HC4I,HC8I),M(HC4I,HC8F),M(HC4I,HC8R),M(HC4I,HC8V),M(HC4I,BA1F),M(HC4I,BA2F),M(HC4I,BA4F),M(HC4I,BA8F)},     /* HC4I */  \
   {M(HC4F,BOOL),M(HC4F,INT ),M(HC4F,FLT ),M(HC4F,ERR ),M(HC4F,ERR ),M(HC4F,ERR ),M(HC4F,ERR ),M(HC4F,ERR ),M(HC4F,RAT ),M(HC4F,VFP ),M(HC4F,HC2I),M(HC4F,HC2F),M(HC4F,HC2R),M(HC4F,HC2V),M(HC4F,HC4I),M(HC4F,HC4F),M(HC4F,HC4R),M(HC4F,HC4V),M(HC4F,HC8I),M(HC4F,HC8F),M(HC4F,HC8R),M(HC4F,HC8V),M(HC4F,BA1F),M(HC4F,BA2F),M(HC4F,BA4F),M(HC4F,BA8F)},     /* HC4F */  \
   {M(HC4R,BOOL),M(HC4R,INT ),M(HC4R,FLT ),M(HC4R,ERR ),M(HC4R,ERR ),M(HC4R,ERR ),M(HC4R,ERR ),M(HC4R,ERR ),M(HC4R,RAT ),M(HC4R,VFP ),M(HC4R,HC2I),M(HC4R,HC2F),M(HC4R,HC2R),M(HC4R,HC2V),M(HC4R,HC4I),M(HC4R,HC4F),M(HC4R,HC4R),M(HC4R,HC4V),M(HC4R,HC8I),M(HC4R,HC8F),M(HC4R,HC8R),M(HC4R,HC8V),M(HC4R,BA1F),M(HC4R,BA2F),M(HC4R,BA4F),M(HC4R,BA8F)},     /* HC4R */  \
   {M(HC4V,BOOL),M(HC4V,INT ),M(HC4V,FLT ),M(HC4V,ERR ),M(HC4V,ERR ),M(HC4V,ERR ),M(HC4V,ERR ),M(HC4V,ERR ),M(HC4V,RAT ),M(HC4V,VFP ),M(HC4V,HC2I),M(HC4V,HC2F),M(HC4V,HC2R),M(HC4V,HC2V),M(HC4V,HC4I),M(HC4V,HC4F),M(HC4V,HC4R),M(HC4V,HC4V),M(HC4V,HC8I),M(HC4V,HC8F),M(HC4V,HC8R),M(HC4V,HC8V),M(HC4V,BA1F),M(HC4V,BA2F),M(HC4V,BA4F),M(HC4V,BA8F)},     /* HC4V */  \
   {M(HC8I,BOOL),M(HC8I,INT ),M(HC8I,FLT ),M(HC8I,ERR ),M(HC8I,ERR ),M(HC8I,ERR ),M(HC8I,ERR ),M(HC8I,ERR ),M(HC8I,RAT ),M(HC8I,VFP ),M(HC8I,HC2I),M(HC8I,HC2F),M(HC8I,HC2R),M(HC8I,HC2V),M(HC8I,HC4I),M(HC8I,HC4F),M(HC8I,HC4R),M(HC8I,HC4V),M(HC8I,HC8I),M(HC8I,HC8F),M(HC8I,HC8R),M(HC8I,HC8V),M(HC8I,BA1F),M(HC8I,BA2F),M(HC8I,BA4F),M(HC8I,BA8F)},     /* HC8I */  \
   {M(HC8F,BOOL),M(HC8F,INT ),M(HC8F,FLT ),M(HC8F,ERR ),M(HC8F,ERR ),M(HC8F,ERR ),M(HC8F,ERR ),M(HC8F,ERR ),M(HC8F,RAT ),M(HC8F,VFP ),M(HC8F,HC2I),M(HC8F,HC2F),M(HC8F,HC2R),M(HC8F,HC2V),M(HC8F,HC4I),M(HC8F,HC4F),M(HC8F,HC4R),M(HC8F,HC4V),M(HC8F,HC8I),M(HC8F,HC8F),M(HC8F,HC8R),M(HC8F,HC8V),M(HC8F,BA1F),M(HC8F,BA2F),M(HC8F,BA4F),M(HC8F,BA8F)},     /* HC8F */  \
   {M(HC8R,BOOL),M(HC8R,INT ),M(HC8R,FLT ),M(HC8R,ERR ),M(HC8R,ERR ),M(HC8R,ERR ),M(HC8R,ERR ),M(HC8R,ERR ),M(HC8R,RAT ),M(HC8R,VFP ),M(HC8R,HC2I),M(HC8R,HC2F),M(HC8R,HC2R),M(HC8R,HC2V),M(HC8R,HC4I),M(HC8R,HC4F),M(HC8R,HC4R),M(HC8R,HC4V),M(HC8R,HC8I),M(HC8R,HC8F),M(HC8R,HC8R),M(HC8R,HC8V),M(HC8R,BA1F),M(HC8R,BA2F),M(HC8R,BA4F),M(HC8R,BA8F)},     /* HC8R */  \
   {M(HC8V,BOOL),M(HC8V,INT ),M(HC8V,FLT ),M(HC8V,ERR ),M(HC8V,ERR ),M(HC8V,ERR ),M(HC8V,ERR ),M(HC8V,ERR ),M(HC8V,RAT ),M(HC8V,VFP ),M(HC8V,HC2I),M(HC8V,HC2F),M(HC8V,HC2R),M(HC8V,HC2V),M(HC8V,HC4I),M(HC8V,HC4F),M(HC8V,HC4R),M(HC8V,HC4V),M(HC8V,HC8I),M(HC8V,HC8F),M(HC8V,HC8R),M(HC8V,HC8V),M(HC8V,BA1F),M(HC8V,BA2F),M(HC8V,BA4F),M(HC8V,BA8F)},     /* HC8V */  \
   {M(BA1F,BOOL),M(BA1F,INT ),M(BA1F,FLT ),M(BA1F,ERR ),M(BA1F,ERR ),M(BA1F,ERR ),M(BA1F,ERR ),M(BA1F,ERR ),M(BA1F,RAT ),M(BA1F,VFP ),M(BA1F,HC2I),M(BA1F,HC2F),M(BA1F,HC2R),M(BA1F,HC2V),M(BA1F,HC4I),M(BA1F,HC4F),M(BA1F,HC4R),M(BA1F,HC4V),M(BA1F,HC8I),M(BA1F,HC8F),M(BA1F,HC8R),M(BA1F,HC8V),M(BA1F,BA1F),M(BA1F,BA2F),M(BA1F,BA4F),M(BA1F,BA8F)},     /* BA1F */  \
   {M(BA2F,BOOL),M(BA2F,INT ),M(BA2F,FLT ),M(BA2F,ERR ),M(BA2F,ERR ),M(BA2F,ERR ),M(BA2F,ERR ),M(BA2F,ERR ),M(BA2F,RAT ),M(BA2F,VFP ),M(BA2F,HC2I),M(BA2F,HC2F),M(BA2F,HC2R),M(BA2F,HC2V),M(BA2F,HC4I),M(BA2F,HC4F),M(BA2F,HC4R),M(BA2F,HC4V),M(BA2F,HC8I),M(BA2F,HC8F),M(BA2F,HC8R),M(BA2F,HC8V),M(BA2F,BA1F),M(BA2F,BA2F),M(BA2F,BA4F),M(BA2F,BA8F)},     /* BA2F */  \
   {M(BA4F,BOOL),M(BA4F,INT ),M(BA4F,FLT ),M(BA4F,ERR ),M(BA4F,ERR ),M(BA4F,ERR ),M(BA4F,ERR ),M(BA4F,ERR ),M(BA4F,RAT ),M(BA4F,VFP ),M(BA4F,HC2I),M(BA4F,HC2F),M(BA4F,HC2R),M(BA4F,HC2V),M(BA4F,HC4I),M(BA4F,HC4F),M(BA4F,HC4R),M(BA4F,HC4V),M(BA4F,HC8I),M(BA4F,HC8F),M(BA4F,HC8R),M(BA4F,HC8V),M(BA4F,BA1F),M(BA4F,BA2F),M(BA4F,BA4F),M(BA4F,BA8F)},     /* BA4F */  \
   {M(BA8F,BOOL),M(BA8F,INT ),M(BA8F,FLT ),M(BA8F,ERR ),M(BA8F,ERR ),M(BA8F,ERR ),M(BA8F,ERR ),M(BA8F,ERR ),M(BA8F,RAT ),M(BA8F,VFP ),M(BA8F,HC2I),M(BA8F,HC2F),M(BA8F,HC2R),M(BA8F,HC2V),M(BA8F,HC4I),M(BA8F,HC4F),M(BA8F,HC4R),M(BA8F,HC4V),M(BA8F,HC8I),M(BA8F,HC8F),M(BA8F,HC8R),M(BA8F,HC8V),M(BA8F,BA1F),M(BA8F,BA2F),M(BA8F,BA4F),M(BA8F,BA8F)},     /* BA8F */  \
}

#define TCA_BOOL_ERR    TCA_ERROR
#define TCA_INT_ERR     TCA_ERROR
#define TCA_FLT_ERR     TCA_ERROR
#define TCA_CHAR_ERR    TCA_ERROR
#define TCA_HETE_ERR    TCA_ERROR
#define TCA_HETE_BOOL   TCA_NEST_BOOL
#define TCA_HETE_INT    TCA_NEST_INT
#define TCA_HETE_FLT    TCA_NEST_FLT
#define TCA_HETE_CHAR   TCA_NEST_CHAR
#define TCA_NEST_ERR    TCA_ERROR
#define TCA_LIST_ERR    TCA_ERROR
#define TCA_APA_ERR     TCA_ERROR
#define TCA_RAT_ERR     TCA_ERROR
#define TCA_VFP_ERR     TCA_ERROR
#define TCA_HC2I_ERR    TCA_ERROR
#define TCA_HC2F_ERR    TCA_ERROR
#define TCA_HC2R_ERR    TCA_ERROR
#define TCA_HC2V_ERR    TCA_ERROR
#define TCA_HC4I_ERR    TCA_ERROR
#define TCA_HC4F_ERR    TCA_ERROR
#define TCA_HC4R_ERR    TCA_ERROR
#define TCA_HC4V_ERR    TCA_ERROR
#define TCA_HC8I_ERR    TCA_ERROR
#define TCA_HC8F_ERR    TCA_ERROR
#define TCA_HC8R_ERR    TCA_ERROR
#define TCA_HC8V_ERR    TCA_ERROR
#define TCA_BA1F_ERR    TCA_ERROR
#define TCA_BA2F_ERR    TCA_ERROR
#define TCA_BA4F_ERR    TCA_ERROR
#define TCA_BA8F_ERR    TCA_ERROR


// Type Conversion Action matrix allowing for type demotion
TC_ACTION aTypeActConvert[ARRAY_LENGTH][ARRAY_LENGTH]
#ifdef DEFINE_VALUES

#define M(a,b)          TCA_##a##_##b
 = TC_MAT
#undef  M

#endif
;

#undef  TCA_BA8F_ERR
#undef  TCA_BA4F_ERR
#undef  TCA_BA2F_ERR
#undef  TCA_BA1F_ERR
#undef  TCA_HC8V_ERR
#undef  TCA_HC8R_ERR
#undef  TCA_HC8F_ERR
#undef  TCA_HC8I_ERR
#undef  TCA_HC4V_ERR
#undef  TCA_HC4R_ERR
#undef  TCA_HC4F_ERR
#undef  TCA_HC4I_ERR
#undef  TCA_HC2R_ERR
#undef  TCA_HC2V_ERR
#undef  TCA_HC2F_ERR
#undef  TCA_HC2I_ERR
#undef  TCA_VFP_ERR
#undef  TCA_RAT_ERR
#undef  TCA_APA_ERR
#undef  TCA_LIST_ERR
#undef  TCA_NEST_ERR
#undef  TCA_HETE_ERR
#undef  TCA_CHAR_ERR
#undef  TCA_FLT_ERR
#undef  TCA_INT_ERR
#undef  TCA_BOOL_ERR


//***************************************************************************
//  End of File: typemote.h
//***************************************************************************
