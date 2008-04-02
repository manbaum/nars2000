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

#ifndef DEBUG

typedef struct tagSTART_ADDRESSES
{
    char   *StartAddressName;
    LPUCHAR StartAddressAddr;
} START_ADDRESSES, *LPSTART_ADDRESSES;


#ifndef PROTO
START_ADDRESSES StartAddresses[] =
{
    // assign.c
    "AssignName_EM"             , (LPUCHAR) &AssignName_EM             ,
    "GetNameType"               , (LPUCHAR) &GetNameType               ,
    "AssignArrayCommon"         , (LPUCHAR) &AssignArrayCommon         ,
    "AssignSelectSpec_EM"       , (LPUCHAR) &AssignSelectSpec_EM       ,
    "ModifyAssignNameVals_EM"   , (LPUCHAR) &ModifyAssignNameVals_EM   ,

    // bjhash.c
    "hashword"                  , (LPUCHAR) &hashword                  ,
    "hashlittle"                , (LPUCHAR) &hashlittle                ,
    "hashlittle2"               , (LPUCHAR) &hashlittle2               ,
    "hashbig"                   , (LPUCHAR) &hashbig                   ,

    // common.c

    // convert.c

    // debug.c
#ifdef DEBUG
    "Assert"                    , (LPUCHAR) &Assert                    ,
    "CreateDebuggerWindow"      , (LPUCHAR) &CreateDebuggerWindow      ,
    "CreateDebuggerInThread"    , (LPUCHAR) &CreateDebuggerInThread    ,
    "DB_Create"                 , (LPUCHAR) DB_Create                  ,
    "DB_Delete"                 , (LPUCHAR) DB_Delete                  ,
    "DBWndProc"                 , (LPUCHAR) DBWndProc                  ,
    "LclListboxWndProc"         , (LPUCHAR) &LclListboxWndProc         ,
    "DbgMsg"                    , (LPUCHAR) &DbgMsg                    ,
    "DbgMsgW"                   , (LPUCHAR) &DbgMsgW                   ,
    "DbgClr"                    , (LPUCHAR) &DbgClr                    ,
    "dprintf"                   , (LPUCHAR) &dprintf                   ,
    "dprintfW"                  , (LPUCHAR) &dprintfW                  ,
    "DbgGlobalAllocSub"         , (LPUCHAR) &DbgGlobalAllocSub         ,
    "FileNameOnly"              , (LPUCHAR) &FileNameOnly              ,
#endif

    // dfnhdr.c

    // dispdbg.c

    // display.c

    // dtoa.c

    // editfcn.c

    // editmat.c

    // editvec.c

    // errmsg.c

    // except.c
    "MyGetExceptionCode"        , (LPUCHAR) &MyGetExceptionCode        ,
    "MySetExceptionCode"        , (LPUCHAR) &MySetExceptionCode        ,
    "MySetExceptionAddr"        , (LPUCHAR) &MySetExceptionAddr        ,
    "MySetExceptionText"        , (LPUCHAR) &MySetExceptionText        ,
    "CheckException"            , (LPUCHAR) &CheckException            ,
    "CompareStartAddresses"     , (LPUCHAR) &CompareStartAddresses     ,
    "_DisplayException"         , (LPUCHAR) &_DisplayException         ,

    // execdfn.c

    // fastbool.c

    // free.c

    // g_fmt.c

    // getreent.c

    // goto.c

    // immexec.c

    // main.c

    // parse.c
    "pl_yyparse"                , (LPUCHAR) &pl_yyparse                ,
    "ParseLine"                 , (LPUCHAR) &ParseLine                 ,
    "SymbTypeVFO"               , (LPUCHAR) &SymbTypeVFO               ,
    "LookaheadSurround"         , (LPUCHAR) &LookaheadSurround         ,
    "LookaheadAdjacent"         , (LPUCHAR) &LookaheadAdjacent         ,
    "LookaheadDyadicOp"         , (LPUCHAR) &LookaheadDyadicOp         ,
    "pl_yylex"                  , (LPUCHAR) &pl_yylex                  ,
    "pl_yyerror"                , (LPUCHAR) &pl_yyerror                ,
    "pl_yyfprintf"              , (LPUCHAR) &pl_yyfprintf              ,
    "WaitForInput"              , (LPUCHAR) &WaitForInput              ,
    "AmbOpSwap_EM"              , (LPUCHAR) &AmbOpSwap_EM              ,
    "AmbOpToFcn"                , (LPUCHAR) &AmbOpToFcn                ,

    // pf_*.c

    // po_*.c

    // primfns.c
    "FillSISNxt"                , (LPUCHAR) &FillSISNxt                ,

    // primspec.c

    // qf_*.c

    // refcnt.c

    // reg.c

    // resdebug.c

    // savews.c

    // sessman.c

    // sort.c

    // strand.c

    // symtab.c

    // symtrans.c

    // syscmds.c

    // tabctrl.c

    // tokenize.c

    // translate.c

    // ver.c

    // yyfns.c
    "YYAlloc"                   , (LPUCHAR) &YYAlloc                   ,
    "YYCopy"                    , (LPUCHAR) &YYCopy                    ,
    "YYCopyFreeDst"             , (LPUCHAR) &YYCopyFreeDst             ,
    "YYFree"                    , (LPUCHAR) &YYFree                    ,
#ifdef DEBUG
    "YYResIsEmpty"              , (LPUCHAR) YYResIsEmpty               ,
#endif
    "YYCopyFcn"                 , (LPUCHAR) YYCopyFcn                  ,
#ifdef DEBUG
    "YYCheckInuse"              , (LPUCHAR) YYCheckInuse               ,
#endif

    // C API routines
    "memset"                    , (LPUCHAR) &memset                    ,
////"_aulldiv"                  , (LPUCHAR) &_aulldiv                  ,
    "memmove"                   , (LPUCHAR) &memmove                   ,




    // trailer
    "<no name>"                 , (LPUCHAR) NEG1U                      ,
};

#define START_ADDRESSES_LENGTH      ((sizeof (StartAddresses) / sizeof (StartAddresses[0])) - 1)
#endif
#endif


//***************************************************************************
//  End of File: startaddr.h
//***************************************************************************
