//***************************************************************************
//  NARS2000 -- Starting Adddresses
//***************************************************************************


#ifndef DEBUG

typedef struct tagSTART_ADDRESSES
{
    char *StartAddressName;
    UINT  StartAddressAddr;
} START_ADDRESSES, *LPSTART_ADDRESSES;


#ifndef PROTO
START_ADDRESSES StartAddresses[] =
{
    // assign.c
    "AssignName_EM"             , (UINT) &AssignName_EM             ,
    "GetNameType"               , (UINT) &GetNameType               ,
    "AssignArrayCommon"         , (UINT) &AssignArrayCommon         ,
    "AssignSelectSpec_EM"       , (UINT) &AssignSelectSpec_EM       ,
    "ModifyAssignNameVals_EM"   , (UINT) &ModifyAssignNameVals_EM   ,

    // bjhash.c
    "hashword"                  , (UINT) &hashword                  ,
    "hashlittle"                , (UINT) &hashlittle                ,
    "hashlittle2"               , (UINT) &hashlittle2               ,
    "hashbig"                   , (UINT) &hashbig                   ,

    // common.c

    // convert.c

    // debug.c
#ifdef DEBUG
    "Assert"                    , (UINT) &Assert                    ,
    "CreateDebuggerWindow"      , (UINT) &CreateDebuggerWindow      ,
    "CreateDebuggerInThread"    , (UINT) &CreateDebuggerInThread    ,
    "DB_Create"                 , (UINT) DB_Create                  ,
    "DB_Delete"                 , (UINT) DB_Delete                  ,
    "DBWndProc"                 , (UINT) DBWndProc                  ,
    "LclListboxWndProc"         , (UINT) &LclListboxWndProc         ,
    "DbgMsg"                    , (UINT) &DbgMsg                    ,
    "DbgMsgW"                   , (UINT) &DbgMsgW                   ,
    "DbgClr"                    , (UINT) &DbgClr                    ,
    "dprintf"                   , (UINT) &dprintf                   ,
    "dprintfW"                  , (UINT) &dprintfW                  ,
    "DbgGlobalAllocSub"         , (UINT) &DbgGlobalAllocSub         ,
    "FileNameOnly"              , (UINT) &FileNameOnly              ,
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
    "MyGetExceptionCode"        , (UINT) &MyGetExceptionCode        ,
    "MySetExceptionCode"        , (UINT) &MySetExceptionCode        ,
    "MySetExceptionAddr"        , (UINT) &MySetExceptionAddr        ,
    "MySetExceptionText"        , (UINT) &MySetExceptionText        ,
    "CheckException"            , (UINT) &CheckException            ,
    "CompareStartAddresses"     , (UINT) &CompareStartAddresses     ,
    "_DisplayException"         , (UINT) &_DisplayException         ,

    // execdfn.c

    // fastbool.c

    // free.c

    // g_fmt.c

    // getreent.c

    // goto.c

    // immexec.c

    // main.c

    // parse.c
    "pl_yyparse"                , (UINT) &pl_yyparse                ,
    "ParseLine"                 , (UINT) &ParseLine                 ,
    "SymbTypeVFO"               , (UINT) &SymbTypeVFO               ,
    "LookaheadSurround"         , (UINT) &LookaheadSurround         ,
    "LookaheadAdjacent"         , (UINT) &LookaheadAdjacent         ,
    "LookaheadDyadicOp"         , (UINT) &LookaheadDyadicOp         ,
    "pl_yylex"                  , (UINT) &pl_yylex                  ,
    "pl_yyerror"                , (UINT) &pl_yyerror                ,
    "pl_yyfprintf"              , (UINT) &pl_yyfprintf              ,
    "WaitForInput"              , (UINT) &WaitForInput              ,
    "AmbOpSwap_EM"              , (UINT) &AmbOpSwap_EM              ,
    "AmbOpToFcn"                , (UINT) &AmbOpToFcn                ,

    // pf_*.c

    // po_*.c

    // primfns.c
    "FillSISNxt"                , (UINT) &FillSISNxt                ,

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
    "YYAlloc"                   , (UINT) &YYAlloc                   ,
    "YYCopy"                    , (UINT) &YYCopy                    ,
    "YYCopyFreeDst"             , (UINT) &YYCopyFreeDst             ,
    "YYFree"                    , (UINT) &YYFree                    ,
#ifdef DEBUG
    "YYResIsEmpty"              , (UINT) YYResIsEmpty               ,
#endif
    "YYCopyFcn"                 , (UINT) YYCopyFcn                  ,
#ifdef DEBUG
    "YYCheckInuse"              , (UINT) YYCheckInuse               ,
#endif

    // C API routines
    "memset"                    , (UINT) &memset                    ,
////"_aulldiv"                  , (UINT) &_aulldiv                  ,
    "memmove"                   , (UINT) &memmove                   ,




    // trailer
    "<no name>"                 , NEG1U                             ,
};

#define START_ADDRESSES_LENGTH      ((sizeof (StartAddresses) / sizeof (StartAddresses[0])) - 1)
#endif
#endif


//***************************************************************************
//  End of File: startaddr.h
//***************************************************************************
