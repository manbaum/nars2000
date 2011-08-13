//***************************************************************************
//  NARS2000 -- Type Promote/Demote Header
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2011 Sudley Place Software

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

typedef void (*TPT_ACTION)  (LPTOKEN);

#ifdef DEFINE_VALUES
void           TPT_IDENT    (LPTOKEN);
void           TPT_ERROR    (LPTOKEN);
void           TPT_BOOL2INT (LPTOKEN);
void           TPT_BOOL2FLT (LPTOKEN);
void           TPT_BOOL2RAT (LPTOKEN);
void           TPT_BOOL2VFP (LPTOKEN);
void           TPT_INT2FLT  (LPTOKEN);
void           TPT_INT2RAT  (LPTOKEN);
void           TPT_INT2VFP  (LPTOKEN);
void           TPT_FLT2VFP  (LPTOKEN);
void           TPT_APA2FLT  (LPTOKEN);
void           TPT_APA2RAT  (LPTOKEN);
void           TPT_APA2VFP  (LPTOKEN);
void           TPT_RAT2VFP  (LPTOKEN);
#endif

#define M(a,b)          TPT_##a##2##b

#define TPT_BOOL2BOOL   TPT_IDENT
#define TPT_INT2INT     TPT_IDENT
#define TPT_FLT2FLT     TPT_IDENT
#define TPT_CHAR2CHAR   TPT_IDENT
#define TPT_HETE2HETE   TPT_IDENT
#define TPT_HETE2NEST   TPT_IDENT
#define TPT_NEST2NEST   TPT_IDENT
#define TPT_RAT2RAT     TPT_IDENT
#define TPT_VFP2VFP     TPT_IDENT

#define TPT_BOOL2ERR    TPT_ERROR
#define TPT_INT2ERR     TPT_ERROR
#define TPT_FLT2ERR     TPT_ERROR
#define TPT_CHAR2ERR    TPT_ERROR
#define TPT_HETE2ERR    TPT_ERROR
#define TPT_NEST2ERR    TPT_ERROR
#define TPT_LIST2ERR    TPT_ERROR
#define TPT_APA2ERR     TPT_ERROR
#define TPT_RAT2ERR     TPT_ERROR
#define TPT_VFP2ERR     TPT_ERROR
#define TPT_INIT2ERR    TPT_ERROR

#define TPT_INIT2BOOL   TPT_ERROR
#define TPT_INIT2INT    TPT_ERROR
#define TPT_INIT2FLT    TPT_ERROR
#define TPT_INIT2CHAR   TPT_ERROR
#define TPT_INIT2HETE   TPT_ERROR
#define TPT_INIT2NEST   TPT_ERROR
#define TPT_INIT2LIST   TPT_ERROR
#define TPT_INIT2APA    TPT_ERROR
#define TPT_INIT2RAT    TPT_ERROR
#define TPT_INIT2VFP    TPT_ERROR
#define TPT_INIT2INIT   TPT_ERROR

#define TPT_BOOL2HETE   TPT_IDENT
#define TPT_BOOL2NEST   TPT_IDENT
#define TPT_INT2HETE    TPT_IDENT
#define TPT_INT2NEST    TPT_IDENT
#define TPT_FLT2HETE    TPT_IDENT
#define TPT_FLT2NEST    TPT_IDENT
#define TPT_CHAR2HETE   TPT_IDENT
#define TPT_CHAR2NEST   TPT_IDENT
#define TPT_APA2HETE    TPT_IDENT
#define TPT_APA2NEST    TPT_IDENT
#define TPT_RAT2HETE    TPT_IDENT
#define TPT_RAT2NEST    TPT_IDENT
#define TPT_VFP2HETE    TPT_IDENT
#define TPT_VFP2NEST    TPT_IDENT

#define TPT_APA2INT     TPT_IDENT

// Type Promotion Token matrix
EXTERN
TPT_ACTION aTypeTknPromote[ARRAY_LENGTH + 1][ARRAY_LENGTH + 1]
#ifdef DEFINE_VALUES
 = TP_MAT
#endif
;

#undef  TPT_APA2INT

#undef  TPT_VFP2NEST
#undef  TPT_VFP2HETE
#undef  TPT_RAT2NEST
#undef  TPT_RAT2HETE
#undef  TPT_APA2NEST
#undef  TPT_APA2HETE
#undef  TPT_CHAR2NEST
#undef  TPT_CHAR2HETE
#undef  TPT_FLT2NEST
#undef  TPT_FLT2HETE
#undef  TPT_INT2NEST
#undef  TPT_INT2HETE
#undef  TPT_BOOL2NEST
#undef  TPT_BOOL2HETE

#undef  TPT_INIT2INIT
#undef  TPT_INIT2VFP
#undef  TPT_INIT2RAT
#undef  TPT_INIT2APA
#undef  TPT_INIT2LIST
#undef  TPT_INIT2NEST
#undef  TPT_INIT2HETE
#undef  TPT_INIT2CHAR
#undef  TPT_INIT2FLT
#undef  TPT_INIT2INT
#undef  TPT_INIT2BOOL

#undef  TPT_INIT2ERR
#undef  TPT_VFP2ERR
#undef  TPT_RAT2ERR
#undef  TPT_APA2ERR
#undef  TPT_LIST2ERR
#undef  TPT_NEST2ERR
#undef  TPT_HETE2ERR
#undef  TPT_CHAR2ERR
#undef  TPT_FLT2ERR
#undef  TPT_INT2ERR
#undef  TPT_BOOL2ERR

#undef  TPT_VFP2VFP
#undef  TPT_RAT2RAT
#undef  TPT_NEST2NEST
#undef  TPT_HETE2NEST
#undef  TPT_HETE2HETE
#undef  TPT_CHAR2CHAR
#undef  TPT_FLT2FLT
#undef  TPT_INT2INT
#undef  TPT_BOOL2BOOL

#undef  M


typedef void (*TP_ACTION)    (LPVOID, APLINT, LPALLTYPES);

#ifdef DEFINE_VALUES
void           TPA_ERROR     (LPVOID      , APLINT, LPALLTYPES);
void           TPA_BOOL2BOOL (LPAPLBOOL   , APLINT, LPALLTYPES);
void           TPA_BOOL2INT  (LPAPLBOOL   , APLINT, LPALLTYPES);
void           TPA_BOOL2FLT  (LPAPLBOOL   , APLINT, LPALLTYPES);
void           TPA_BOOL2HETE (LPAPLBOOL   , APLINT, LPALLTYPES);
void           TPA_BOOL2NEST (LPAPLBOOL   , APLINT, LPALLTYPES);
void           TPA_BOOL2RAT  (LPAPLBOOL   , APLINT, LPALLTYPES);
void           TPA_BOOL2VFP  (LPAPLBOOL   , APLINT, LPALLTYPES);
void           TPA_INT2INT   (LPAPLINT    , APLINT, LPALLTYPES);
void           TPA_INT2FLT   (LPAPLINT    , APLINT, LPALLTYPES);
void           TPA_INT2HETE  (LPAPLINT    , APLINT, LPALLTYPES);
void           TPA_INT2NEST  (LPAPLINT    , APLINT, LPALLTYPES);
void           TPA_INT2RAT   (LPAPLINT    , APLINT, LPALLTYPES);
void           TPA_INT2VFP   (LPAPLINT    , APLINT, LPALLTYPES);
void           TPA_APA2INT   (LPAPLAPA    , APLINT, LPALLTYPES);
void           TPA_APA2FLT   (LPAPLAPA    , APLINT, LPALLTYPES);
void           TPA_APA2HETE  (LPAPLAPA    , APLINT, LPALLTYPES);
void           TPA_APA2NEST  (LPAPLAPA    , APLINT, LPALLTYPES);
void           TPA_APA2RAT   (LPAPLAPA    , APLINT, LPALLTYPES);
void           TPA_APA2VFP   (LPAPLAPA    , APLINT, LPALLTYPES);
void           TPA_FLT2FLT   (LPAPLFLOAT  , APLINT, LPALLTYPES);
void           TPA_FLT2HETE  (LPAPLFLOAT  , APLINT, LPALLTYPES);
void           TPA_FLT2NEST  (LPAPLFLOAT  , APLINT, LPALLTYPES);
void           TPA_FLT2VFP   (LPAPLFLOAT  , APLINT, LPALLTYPES);
void           TPA_CHAR2CHAR (LPAPLCHAR   , APLINT, LPALLTYPES);
void           TPA_CHAR2HETE (LPAPLCHAR   , APLINT, LPALLTYPES);
void           TPA_CHAR2NEST (LPAPLCHAR   , APLINT, LPALLTYPES);
void           TPA_HETE2HETE (LPAPLHETERO , APLINT, LPALLTYPES);
void           TPA_HETE2NEST (LPAPLHETERO , APLINT, LPALLTYPES);
void           TPA_NEST2NEST (LPAPLNESTED , APLINT, LPALLTYPES);
void           TPA_RAT2HETE  (LPAPLRAT    , APLINT, LPALLTYPES);
void           TPA_RAT2NEST  (LPAPLRAT    , APLINT, LPALLTYPES);
void           TPA_RAT2RAT   (LPAPLRAT    , APLINT, LPALLTYPES);
void           TPA_RAT2VFP   (LPAPLRAT    , APLINT, LPALLTYPES);
void           TPA_VFP2HETE  (LPAPLVFP    , APLINT, LPALLTYPES);
void           TPA_VFP2NEST  (LPAPLVFP    , APLINT, LPALLTYPES);
void           TPA_VFP2VFP   (LPAPLVFP    , APLINT, LPALLTYPES);
#endif

#define M(a,b)          TPA_##a##2##b

#define TPA_BOOL2ERR    TPA_ERROR
#define TPA_INT2ERR     TPA_ERROR
#define TPA_FLT2ERR     TPA_ERROR
#define TPA_CHAR2ERR    TPA_ERROR
#define TPA_HETE2ERR    TPA_ERROR
#define TPA_NEST2ERR    TPA_ERROR
#define TPA_LIST2ERR    TPA_ERROR
#define TPA_APA2ERR     TPA_ERROR
#define TPA_RAT2ERR     TPA_ERROR
#define TPA_VFP2ERR     TPA_ERROR
#define TPA_INIT2ERR    TPA_ERROR

#define TPA_INIT2BOOL   TPA_ERROR
#define TPA_INIT2INT    TPA_ERROR
#define TPA_INIT2FLT    TPA_ERROR
#define TPA_INIT2CHAR   TPA_ERROR
#define TPA_INIT2HETE   TPA_ERROR
#define TPA_INIT2NEST   TPA_ERROR
#define TPA_INIT2LIST   TPA_ERROR
#define TPA_INIT2APA    TPA_ERROR
#define TPA_INIT2RAT    TPA_ERROR
#define TPA_INIT2VFP    TPA_ERROR
#define TPA_INIT2INIT   TPA_ERROR

// Type Promotion Action matrix
TP_ACTION aTypeActPromote[ARRAY_LENGTH + 1][ARRAY_LENGTH + 1]
#ifdef DEFINE_VALUES
 = TP_MAT
#endif
;

#undef  TPA_INIT2INIT
#undef  TPA_INIT2VFP
#undef  TPA_INIT2RAT
#undef  TPA_INIT2APA
#undef  TPA_INIT2LIST
#undef  TPA_INIT2NEST
#undef  TPA_INIT2HETE
#undef  TPA_INIT2CHAR
#undef  TPA_INIT2FLT
#undef  TPA_INIT2INT
#undef  TPA_INIT2BOOL

#undef  TPA_INIT2ERR
#undef  TPA_VFP2ERR
#undef  TPA_RAT2ERR
#undef  TPA_APA2ERR
#undef  TPA_LIST2ERR
#undef  TPA_NEST2ERR
#undef  TPA_HETE2ERR
#undef  TPA_CHAR2ERR
#undef  TPA_FLT2ERR
#undef  TPA_INT2ERR
#undef  TPA_BOOL2ERR

#undef  M


//***************************************************************************
//  End of File: typemote.h
//***************************************************************************
