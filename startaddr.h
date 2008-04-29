//***************************************************************************
//  NARS2000 -- Starting Adddresses
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

typedef struct tagSTART_ADDRESSES
{
    char   *StartAddressName;
    LPUCHAR StartAddressAddr;
} START_ADDRESSES, *LPSTART_ADDRESSES;


#ifndef PROTO
  START_ADDRESSES StartAddresses[] =
  {
    // assign.c
    "AssignName_EM"             , (LPUCHAR) &AssignName_EM              ,
    "GetNameType"               , (LPUCHAR) &GetNameType                ,
    "AssignArrayCommon"         , (LPUCHAR) &AssignArrayCommon          ,
    "AssignSelectSpec_EM"       , (LPUCHAR) &AssignSelectSpec_EM        ,
    "ModifyAssignNameVals_EM"   , (LPUCHAR) &ModifyAssignNameVals_EM    ,

    // bjhash.c
    "hashword"                  , (LPUCHAR) &hashword                   ,
    "hashlittle"                , (LPUCHAR) &hashlittle                 ,
    "hashlittle2"               , (LPUCHAR) &hashlittle2                ,
    "hashbig"                   , (LPUCHAR) &hashbig                    ,

    // chkesp.c

    // common.c

    // convert.c

    // debug.c
  #ifdef DEBUG
    "CreateDebuggerWindow"      , (LPUCHAR) &CreateDebuggerWindow       ,
    "CreateDebuggerInThread"    , (LPUCHAR) &CreateDebuggerInThread     ,
    "DB_Create"                 , (LPUCHAR) DB_Create                   ,
    "DB_Delete"                 , (LPUCHAR) DB_Delete                   ,
    "DBWndProc"                 , (LPUCHAR) DBWndProc                   ,
    "LclListboxWndProc"         , (LPUCHAR) &LclListboxWndProc          ,
    "DbgMsg"                    , (LPUCHAR) &DbgMsg                     ,
    "DbgMsgW"                   , (LPUCHAR) &DbgMsgW                    ,
    "DbgClr"                    , (LPUCHAR) &DbgClr                     ,
    "dprintf"                   , (LPUCHAR) &dprintf                    ,
    "dprintfW"                  , (LPUCHAR) &dprintfW                   ,
    "DbgGlobalAllocSub"         , (LPUCHAR) &DbgGlobalAllocSub          ,
    "FileNameOnly"              , (LPUCHAR) &FileNameOnly               ,
  #endif

    // dfnhdr.c/y

    // dispdbg.c

    // display.c

    // dtoa.c

    // editfcn.c
    "CreateFcnWindow"           , (LPUCHAR) &CreateFcnWindow            ,
    "FEWndProc"                 , (LPUCHAR) &FEWndProc                  ,
    "LclEditCtrlWndProc"        , (LPUCHAR) &LclEditCtrlWndProc         ,

    // editmat.c

    // editvec.c

    // errmsg.c

    // except.c
    "MyGetExceptionCode"        , (LPUCHAR) &MyGetExceptionCode         ,
    "MySetExceptionCode"        , (LPUCHAR) &MySetExceptionCode         ,
    "MySetExceptionAddr"        , (LPUCHAR) &MySetExceptionAddr         ,
    "MySetExceptionText"        , (LPUCHAR) &MySetExceptionText         ,
    "CheckException"            , (LPUCHAR) &CheckException             ,
    "CompareStartAddresses"     , (LPUCHAR) &CompareStartAddresses      ,
    "_DisplayException"         , (LPUCHAR) &DisplayException           ,

    // execdfn.c

    // execfns.c

    // execmfn.c

    // fastbool.c

    // free.c

    // g_fmt.c

    // getfns.c
    "GetFirstItemToken"         , (LPUCHAR) &GetFirstItemToken          ,

    // getreent.c
    "__getreent"                , (LPUCHAR) &__getreent                 ,

    // goto.c
    "GotoLine_EM"               , (LPUCHAR) &GotoLine_EM                ,

    // immexec.c
    "WaitForImmExecStmt"        , (LPUCHAR) &WaitForImmExecStmt         ,

    // initdata.c
    "InitPrimTabs"              , (LPUCHAR) &InitPrimTabs               ,

    // main.c
    "EnumCallbackPassMsg"       , (LPUCHAR) &EnumCallbackPassMsg        ,

    // parse.c/y
    "pl_yyparse"                , (LPUCHAR) &pl_yyparse                 ,
    "ParseLine"                 , (LPUCHAR) &ParseLine                  ,
    "SymbTypeVFO"               , (LPUCHAR) &SymbTypeVFO                ,
    "LookaheadSurround"         , (LPUCHAR) &LookaheadSurround          ,
    "LookaheadAdjacent"         , (LPUCHAR) &LookaheadAdjacent          ,
    "LookaheadDyadicOp"         , (LPUCHAR) &LookaheadDyadicOp          ,
    "pl_yylex"                  , (LPUCHAR) &pl_yylex                   ,
    "pl_yyerror"                , (LPUCHAR) &pl_yyerror                 ,
    "pl_yyfprintf"              , (LPUCHAR) &pl_yyfprintf               ,
    "WaitForInput"              , (LPUCHAR) &WaitForInput               ,
    "AmbOpSwap_EM"              , (LPUCHAR) &AmbOpSwap_EM               ,
    "AmbOpToFcn"                , (LPUCHAR) &AmbOpToFcn                 ,

    // pf_*.c

    // pf_dtackjot.c
    "PrimFnDownTackJot_EM_YY"   , (LPUCHAR) &PrimFnDownTackJot_EM_YY    ,

    // pf_equalund.c
    "PrimFnEqualUnderbar_EM_YY" , (LPUCHAR) &PrimFnEqualUnderbar_EM_YY  ,

    // pf_index.c
    "ArrayIndexRef_EM_YY"       , (LPUCHAR) &ArrayIndexRef_EM_YY        ,

    // pf_plus.c
    "PrimFnPlus_EM_YY"          , (LPUCHAR) &PrimFnPlus_EM_YY           ,

    // pf_qdot.c
    "PrimFnQuoteDot_EM_YY"      , (LPUCHAR) &PrimFnQuoteDot_EM_YY       ,

    // pf_query.c
    "PrimFnQuery_EM_YY"         , (LPUCHAR) &PrimFnQuery_EM_YY          ,
    "PrimFnDydQuery_EM_YY"      , (LPUCHAR) &PrimFnDydQuery_EM_YY       ,

    // pf_utack.c
    "PrimFnUpTack_EM_YY"        , (LPUCHAR) &PrimFnUpTack_EM_YY         ,
    "PrimFnMonUpTack_EM_YY"     , (LPUCHAR) &PrimFnMonUpTack_EM_YY      ,
    "PrimFnDydUpTack_EM_YY"     , (LPUCHAR) &PrimFnDydUpTack_EM_YY      ,

    // po_cmiddot.c
    "PrimOpDydCircleMiddleDotCommon_EM_YY", (LPUCHAR) &PrimOpDydCircleMiddleDotCommon_EM_YY,

    // po_dicircle.c
    "PrimOpDieresisCircle_EM_YY", (LPUCHAR) &PrimOpDieresisCircle_EM_YY ,

    // po_dieresis.c
    "PrimOpDieresis_EM_YY"      , (LPUCHAR) &PrimOpDieresis_EM_YY       ,

    // po_dijot.c
    "PrimOpDieresisJot_EM_YY"   , (LPUCHAR) &PrimOpDieresisJot_EM_YY    ,

    // po_ditilde.c
    "PrimOpDieresisTilde_EM_YY" , (LPUCHAR) &PrimOpDieresisTilde_EM_YY  ,

    // po_dot.c
    "PrimOpDot_EM_YY"           , (LPUCHAR) &PrimOpDot_EM_YY            ,

    // po_jot.c
    "PrimOpJot_EM_YY"           , (LPUCHAR) &PrimOpJot_EM_YY            ,

    // po_jotdot.c
    "PrimOpJotDot_EM_YY"        , (LPUCHAR) &PrimOpJotDot_EM_YY         ,

    // po_slash.c
    "PrimOpSlash_EM_YY"         , (LPUCHAR) &PrimOpSlash_EM_YY          ,

    // po_slope.c
    "PrimOpSlope_EM_YY"         , (LPUCHAR) &PrimOpSlope_EM_YY          ,

    // primfns.c
    "FillSISNxt"                , (LPUCHAR) &FillSISNxt                 ,

    // primspec.c

    // qf_*.c

    // refcnt.c

    // reg.c

    // resdebug.c

    // savefcn.c
    "SaveFunction"              , (LPUCHAR) &SaveFunction               ,

    // sc_copy.c
    "CmdCopy_EM"                , (LPUCHAR) &CmdCopy_EM                 ,

    // sc_drop.c
    "CmdDrop_EM"                , (LPUCHAR) &CmdDrop_EM                 ,

    // sc_erase.c
    "CmdErase_EM"               , (LPUCHAR) &CmdErase_EM                ,

    // sc_fnov.c
    "CmdFns_EM"                 , (LPUCHAR) &CmdFns_EM                  ,

    // sc_lib.c
    "CmdLib_EM"                 , (LPUCHAR) &CmdLib_EM                  ,

    // sc_load.c
    "CmdLoad_EM"                , (LPUCHAR) &CmdLoad_EM                 ,

    // sc_reset.c
    "CmdReset_EM"               , (LPUCHAR) &CmdReset_EM                ,

    // sc_save.c
    "CmdSave_EM"                , (LPUCHAR) &CmdSave_EM                 ,

    // sc_si.c
    "CmdSi_EM"                  , (LPUCHAR) &CmdSi_EM                   ,

    // sc_wsid.c
    "CmdWsid_EM"                , (LPUCHAR) &CmdWsid_EM                 ,

    // sessman.c
    "SetAttrs"                  , (LPUCHAR) &SetAttrs                   ,

    // sort.c
    "ShellSort"                 , (LPUCHAR) &ShellSort                  ,

    // strand.c
    "InitVarStrand"             , (LPUCHAR) &InitVarStrand              ,

    // symtab.c
  #ifdef DEBUG
    "HshTabFrisk"               , (LPUCHAR) &HshTabFrisk                ,
  #endif

    // symtrans.c
    "FcnTrans"                  , (LPUCHAR) &FcnTrans                   ,

    // syscmds.c
    "IncorrectCommand"          , (LPUCHAR) &IncorrectCommand           ,

    // sysvars.c
    "MakePermVars"              , (LPUCHAR) &MakePermVars               ,

    // tabctrl.c
    "EnumCallbackShowHide"      , (LPUCHAR) &EnumCallbackShowHide       ,

    // tokenize.c
    "UTLockAndSet"              , (LPUCHAR) &UTLockAndSet               ,

    // translate.c
    "TranslateTknTypeToTknTypeNamed", (LPUCHAR) &TranslateTknTypeToTknTypeNamed,

    // typemote.c
    "TypeDemote"                , (LPUCHAR) &TypeDemote                 ,
    "DemoteData"                , (LPUCHAR) &DemoteData                 ,
    "TypePromote_EM"            , (LPUCHAR) &TypePromote_EM             ,
    "TypePromoteGlb_EM"         , (LPUCHAR) &TypePromoteGlb_EM          ,
    "QueryPromote"              , (LPUCHAR) &QueryPromote               ,

    // ver.c
    "LclFileVersionStrW"        , (LPUCHAR) &LclFileVersionStrW         ,

    // yyfns.c
    "YYAlloc"                   , (LPUCHAR) &_YYAlloc                   ,
    "YYCopy"                    , (LPUCHAR) &YYCopy                     ,
    "YYCopyFreeDst"             , (LPUCHAR) &YYCopyFreeDst              ,
    "YYFree"                    , (LPUCHAR) &YYFree                     ,
  #ifdef DEBUG
    "YYResIsEmpty"              , (LPUCHAR) YYResIsEmpty                ,
  #endif
    "YYCopyFcn"                 , (LPUCHAR) YYCopyFcn                   ,
  #ifdef DEBUG
    "YYCheckInuse"              , (LPUCHAR) YYCheckInuse                ,
  #endif

    // C API routines
    "memset"                    , (LPUCHAR) &memset                     ,
////"_aulldiv"                  , (LPUCHAR) &_aulldiv                   ,
    "memmove"                   , (LPUCHAR) &memmove                    ,




    // trailer
    "<no name>"                 , (LPUCHAR) NEG1U                       ,
  };

  #define START_ADDRESSES_LENGTH      ((sizeof (StartAddresses) / sizeof (StartAddresses[0])) - 1)
#endif


//***************************************************************************
//  End of File: startaddr.h
//***************************************************************************
