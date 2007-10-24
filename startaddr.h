//***************************************************************************
//  NARS2000 -- Starting Adddresses
//***************************************************************************


#define START_ADDRESSES_ENABLE      // Comment out to disable

#ifdef START_ADDRESSES_ENABLE

typedef struct tagSTART_ADDRESSES
{
    char *StartAddressName;
    UINT  StartAddressAddr;
} START_ADDRESSES, *LPSTART_ADDRESSES;


#ifndef PROTO
START_ADDRESSES StartAddresses[] =
{
    // except.c
    "MyGetExceptionCode"        , (UINT) &MyGetExceptionCode,
    "MySetExceptionCode"        , (UINT) &MySetExceptionCode,
    "CheckException"            , (UINT) &CheckException,


    // primfns.c
    "FillSISNxt"                , (UINT) &FillSISNxt,

    // yyfns.c
    "YYAlloc"                   , (UINT) &YYAlloc,
    "YYCopy"                    , (UINT) &YYCopy,
    "YYCopyFreeDst"             , (UINT) &YYCopyFreeDst,
    "YYFree"                    , (UINT) &YYFree,
#ifdef DEBUG
    "YYResIsEmpty"              , (UINT) YYResIsEmpty,
#endif
    "YYCopyFcn"                 , (UINT) YYCopyFcn,
#ifdef DEBUG
    "YYCheckInuse"              , (UINT) YYCheckInuse,
#endif

    // debug.c
#ifdef DEBUG
    "Assert"                    , (UINT) &Assert,
    "CreateDebuggerWindow"      , (UINT) &CreateDebuggerWindow,
    "CreateDebuggerInThread"    , (UINT) &CreateDebuggerInThread,
    "DB_Create"                 , (UINT) DB_Create,
    "DB_Delete"                 , (UINT) DB_Delete,
    "DBWndProc"                 , (UINT) DBWndProc,
    "LclListboxWndProc"         , (UINT) &LclListboxWndProc,
    "DbgMsg"                    , (UINT) &DbgMsg,
    "DbgMsgW"                   , (UINT) &DbgMsgW,
    "DbgClr"                    , (UINT) &DbgClr,
    "dprintf"                   , (UINT) &dprintf,
    "dprintfW"                  , (UINT) &dprintfW,
    "DbgGlobalAllocSub"         , (UINT) &DbgGlobalAllocSub,
    "FileNameOnly"              , (UINT) &FileNameOnly,

#endif

    // C API routines
    "memset"                    , (UINT) &memset,
////"_aulldiv"                  , (UINT) &_aulldiv,
    "memmove"                   , (UINT) &memmove,


    // trailer
    "<no name>"                 , NEG1U,
};

#define START_ADDRESSES_LENGTH      ((sizeof (StartAddresses) / sizeof (StartAddresses[0])) - 1)
#endif
#endif


//***************************************************************************
//  End of File: startaddr.h
//***************************************************************************
