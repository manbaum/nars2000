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


LRESULT WINAPI EditWndProcA(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI EditWndProcW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#ifndef PROTO
  START_ADDRESSES StartAddresses[] =
  {
    // assign.c
    "AssignName_EM"             , (LPUCHAR) &AssignName_EM              ,
    "GetNameType"               , (LPUCHAR) &GetNameType                ,
    "AssignArrayCommon"         , (LPUCHAR) &AssignArrayCommon          ,
    "AssignSelectSpec_EM"       , (LPUCHAR) &AssignSelectSpec_EM        ,
    "ModifyAssignNameVals_EM"   , (LPUCHAR) &ModifyAssignNameVals_EM    ,

    // axisfns.c
    "CheckAxisOper"             , (LPUCHAR) &CheckAxisOper              ,

    // bjhash.c
    "hashlittle"                , (LPUCHAR) &hashlittle                 ,

    // chkesp.c

    // common.c

    // convert.c

    // crash.c
    "CCWndProc"                 , (LPUCHAR) &CCWndProc                  ,
    "LclCCListboxWndProc"       , (LPUCHAR) &LclCCListboxWndProc        ,

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
  #ifdef DEBUG
    "DisplayHshTab"             , (LPUCHAR) &DisplayHshTab              ,
  #endif
    "FormatHTE"                 , (LPUCHAR) &FormatHTE                  ,
  #ifdef DEBUG
    "DisplaySymTab"             , (LPUCHAR) &DisplaySymTab              ,
    "UpdateDBWindow"            , (LPUCHAR) &UpdateDBWindow             ,
    "DisplayGlobals"            , (LPUCHAR) &DisplayGlobals             ,
    "DisplayTokens"             , (LPUCHAR) &DisplayTokens              ,
  #endif
    "FormatSTE"                 , (LPUCHAR) &FormatSTE                  ,

    // display.c
    "DisplayGlbArr"             , (LPUCHAR) &DisplayGlbArr              ,

    // dtoa.c

    // editctrl.c
    "EditWndProcW"              , (LPUCHAR) &EditWndProcW               ,
    "EditWndProcA"              , (LPUCHAR) &EditWndProcA               ,

    // editfcn.c
    "CreateFcnWindow"           , (LPUCHAR) &CreateFcnWindow            ,
    "FEWndProc"                 , (LPUCHAR) &FEWndProc                  ,
    "LclEditCtrlWndProc"        , (LPUCHAR) &LclEditCtrlWndProc         ,

    // editmat.c
    "MEWndProc"                 , (LPUCHAR) &MEWndProc                  ,

    // editvec.c
    "VEWndProc"                 , (LPUCHAR) &VEWndProc                  ,

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
    "ExecDfnGlbProto_EM_YY"     , (LPUCHAR) &ExecDfnGlbProto_EM_YY      ,
    "ExecDfnGlb_EM_YY"          , (LPUCHAR) &ExecDfnGlb_EM_YY           ,
    "ExecDfnOprGlb_EM_YY"       , (LPUCHAR) &ExecDfnOprGlb_EM_YY        ,
    "LocalizeAll"               , (LPUCHAR) &LocalizeAll                ,
    "_CheckSymEntries"          , (LPUCHAR) &_CheckSymEntries           ,
    "ExecuteFunction_EM_YY"     , (LPUCHAR) &ExecuteFunction_EM_YY      ,
    "DisplayFcnLine"            , (LPUCHAR) &DisplayFcnLine             ,
    "CheckDfnExitError_EM"      , (LPUCHAR) &CheckDfnExitError_EM       ,
    "Unlocalize"                , (LPUCHAR) &Unlocalize                 ,
    "LocalizeLabels"            , (LPUCHAR) &LocalizeLabels             ,
    "InitVarSTEs"               , (LPUCHAR) &InitVarSTEs                ,
    "InitFcnSTEs"               , (LPUCHAR) &InitFcnSTEs                ,
    "LocalizeSymEntries"        , (LPUCHAR) &LocalizeSymEntries         ,

    // execfns.c
    "ExecuteFn0"                , (LPUCHAR) &ExecuteFn0                 ,
    "ExecFunc_EM_YY"            , (LPUCHAR) &ExecFunc_EM_YY             ,
    "ExecFcnGlb_EM_YY"          , (LPUCHAR) &ExecFcnGlb_EM_YY           ,
    "ExecFuncStr_EM_YY"         , (LPUCHAR) &ExecFuncStr_EM_YY          ,
    "ExecOp1_EM_YY"             , (LPUCHAR) &ExecOp1_EM_YY              ,
    "ExecOp2_EM_YY"             , (LPUCHAR) &ExecOp2_EM_YY              ,

    // execmfn.c
    "ExecuteMagicFunction_EM_YY", (LPUCHAR) &ExecuteMagicFunction_EM_YY ,

    // fastbool.c
    "FastBoolScan"              , (LPUCHAR) &FastBoolScan               ,

    // free.c
    "FreeResultName"            , (LPUCHAR) &FreeResultName             ,

    // g_fmt.c

    // getfns.c
    "GetFirstItemToken"         , (LPUCHAR) &GetFirstItemToken          ,

    // getreent.c
    "__getreent"                , (LPUCHAR) &__getreent                 ,

    // goto.c
    "GotoLine_EM"               , (LPUCHAR) &GotoLine_EM                ,

    // immexec.c
    "WaitForImmExecStmt"        , (LPUCHAR) &WaitForImmExecStmt         ,
    "ImmExecStmtInThread"       , (LPUCHAR) &ImmExecStmtInThread        ,

    // initdata.c
    "InitPrimTabs"              , (LPUCHAR) &InitPrimTabs               ,

    // main.c
    "EnumCallbackPassMsg"       , (LPUCHAR) &EnumCallbackPassMsg        ,
    "MFWndProc"                 , (LPUCHAR) &MFWndProc                  ,

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

    // pf_utackjot.c
    "PrimFnMonUptackJotInThread", (LPUCHAR) &PrimFnMonUpTackJotInThread ,

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
    "PrimProtoOpSlash_EM_YY"    , (LPUCHAR) &PrimProtoOpSlash_EM_YY     ,
    "PrimOpMonSlash_EM_YY"      , (LPUCHAR) &PrimOpMonSlash_EM_YY       ,
    "PrimOpMonSlashCommon_EM_YY", (LPUCHAR) &PrimOpMonSlashCommon_EM_YY ,
    "PrimOpMonSlashScalar_EM_YY", (LPUCHAR) &PrimOpMonSlashScalar_EM_YY ,
    "SymGlbToToken"             , (LPUCHAR) &SymGlbToToken              ,
    "PrimOpDydSlash_EM_YY"      , (LPUCHAR) &PrimOpDydSlash_EM_YY       ,
    "PrimOpDydSlashCommon_EM_YY", (LPUCHAR) &PrimOpDydSlashCommon_EM_YY ,
    "PrimOpDydSlashInsertDim_EM", (LPUCHAR) &PrimOpDydSlashInsertDim_EM ,
    "PrimOpDydSlashAllocate_EM" , (LPUCHAR) &PrimOpDydSlashAllocate_EM  ,

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
    "CmdXload_EM"               , (LPUCHAR) &CmdXload_EM                ,
    "CmdLoadCom_EM"             , (LPUCHAR) &CmdLoadCom_EM              ,
    "LoadWorkspace_EM"          , (LPUCHAR) &LoadWorkspace_EM           ,
    "DisplayWorkspaceStamp"     , (LPUCHAR) &DisplayWorkspaceStamp      ,
    "SendMessageLastTab"        , (LPUCHAR) &SendMessageLastTab         ,
    "ParseSavedWsFcn_EM"        , (LPUCHAR) &ParseSavedWsFcn_EM         ,
    "ParseSavedWsVar_EM"        , (LPUCHAR) &ParseSavedWsVar_EM         ,
    "LoadWorkspaceGlobal_EM"    , (LPUCHAR) &LoadWorkspaceGlobal_EM     ,
    "LoadWsGlbVarConv"          , (LPUCHAR) &LoadWsGlbVarConv           ,

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
    "SMWndProc"                 , (LPUCHAR) &SMWndProc                  ,

    // sort.c
    "ShellSort"                 , (LPUCHAR) &ShellSort                  ,

    // strand.c
    "InitVarStrand"             , (LPUCHAR) &InitVarStrand              ,
    "MakeList_EM_YY"            , (LPUCHAR) &MakeList_EM_YY             ,
    "CopyImmToken_EM"           , (LPUCHAR) &CopyImmToken_EM            ,

    // symtab.c
  #ifdef DEBUG
    "HshTabFrisk"               , (LPUCHAR) &HshTabFrisk                ,
  #endif
    "MaskTheHash"               , (LPUCHAR) &MaskTheHash               ,
    "HshTabDelink"              , (LPUCHAR) &HshTabDelink              ,
    "HshTabLink"                , (LPUCHAR) &HshTabLink                ,
    "HshTabResize_EM"           , (LPUCHAR) &HshTabResize_EM           ,
    "SymTabResize_EM"           , (LPUCHAR) &SymTabResize_EM           ,
    "HshTabSplitNextEntry_EM"   , (LPUCHAR) &HshTabSplitNextEntry_EM   ,
    "FindNextFreeUsingHash_SPLIT_EM", (LPUCHAR) FindNextFreeUsingHash_SPLIT_EM ,
    "FindNextFreeUsingHTE_EM"   , (LPUCHAR) &FindNextFreeUsingHTE_EM   ,

    // symtrans.c
    "FcnTrans"                  , (LPUCHAR) &FcnTrans                   ,

    // syscmds.c
    "IncorrectCommand"          , (LPUCHAR) &IncorrectCommand           ,

    // sysvars.c
    "MakePermVars"              , (LPUCHAR) &MakePermVars               ,

    // tabctrl.c
    "EnumCallbackShowHide"      , (LPUCHAR) &EnumCallbackShowHide       ,
    "LclTabCtrlWndProc"         , (LPUCHAR) &LclTabCtrlWndProc          ,
    "CreateNewTabInThread"      , (LPUCHAR) &CreateNewTabInThread       ,

    // tokenize.c
    "UTLockAndSet"              , (LPUCHAR) &UTLockAndSet               ,
    "Tokenize_EM"               , (LPUCHAR) &Tokenize_EM                ,
    "Untokenize"                , (LPUCHAR) &Untokenize                 ,

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
    "AboutDlgProc"              , (LPUCHAR) &AboutDlgProc               ,
    "LclStaticWndProc"          , (LPUCHAR) &LclStaticWndProc           ,

    // yyfns.c
    "_YYAlloc"                  , (LPUCHAR) &_YYAlloc                   ,
    "YYCopy"                    , (LPUCHAR) &YYCopy                     ,
    "YYCopyFreeDst"             , (LPUCHAR) &YYCopyFreeDst              ,
    "YYFree"                    , (LPUCHAR) &YYFree                     ,
    "YYCountFcnStr"             , (LPUCHAR) &YYCountFcnStr              ,
    "YYIsFcnStrAxis"            , (LPUCHAR) &YYIsFcnStrAxis             ,
  #ifdef DEBUG
    "YYResIsEmpty"              , (LPUCHAR) YYResIsEmpty                ,
  #endif
    "YYCopyFcn"                 , (LPUCHAR) YYCopyFcn                   ,
  #ifdef DEBUG
    "YYCheckInuse"              , (LPUCHAR) YYCheckInuse                ,
  #endif

    // DBGBRK.ASM
    "DbgBrk"                    , (LPUCHAR) &DbgBrk                     ,
    "DbgStop"                   , (LPUCHAR) &DbgStop                    ,

    // C API routines
    "memset"                    , (LPUCHAR) &memset                     ,
////"_aulldiv"                  , (LPUCHAR) &_aulldiv                   ,
    "memmove"                   , (LPUCHAR) &memmove                    ,




    // trailer
    "<no name>"                 , (LPUCHAR) (HANDLE_PTR) -1             ,
  };

  #define START_ADDRESSES_LENGTH      ((sizeof (StartAddresses) / sizeof (StartAddresses[0])) - 1)
#endif


//***************************************************************************
//  End of File: startaddr.h
//***************************************************************************
