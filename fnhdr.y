//***************************************************************************
//  NARS2000 -- Parser Grammar for function headers
//***************************************************************************

//***************************************************************************
//  Parse a line of pre-tokenized tokens.
//***************************************************************************

%{
#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "defnfcn.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

#ifdef DEBUG
#define YYERROR_VERBOSE
#define YYDEBUG 1
#define YYFPRINTF               fh_yyfprintf
#endif

#define    YYSTYPE     FH_YYSTYPE
#define  LPYYSTYPE     FH_LPYYSTYPE
#define tagYYSTYPE  tagFH_YYSTYPE

typedef struct tagYYSTYPE
{
    TOKEN  tkToken;         // Token info
    UINT   uStrandLen;      // # elements in this strand
    struct tagYYSTYPE *
           lpYYStrandBase;  // Ptr to this token's strand base
} YYSTYPE, *LPYYSTYPE;      // Data type of yacc stack

#define YYSTYPE_IS_DECLARED 1

// The following #defines are needed to allow multiple parses
//   to coexist in the same file
#define yy_symbol_print         fh_yy_symbol_print
#define yy_symbol_value_print   fh_yy_symbol_value_print
#define yy_reduce_print         fh_yy_reduce_print
#define yydestruct              fh_yydestruct

#define DbgMsgW2(a) DbgMsgW(a)
////#define DbgMsgW2(a) DbgMsgW(a); DbgBrk ()

typedef struct tagFHLOCALVARS       // Function Header Local Vars
{
    HWND        hWndEC;             // Window handle for Edit Control
    HGLOBAL     hGlbToken;          // Global memory handle
    UNION_TOKEN t2;                 // Locked base of hGlbToken
    LPTOKEN     lpStart,            // First available entry after the header
                lpNext;             // Next  ...
    UINT        tkErrorCharIndex,   // Error char index
                uFcnValence,        // Function valence (0 = niladic,
                                    //                   1 = monadic,
                                    //                   2 = dyadic,
                                    //                   3 = ambivalent)
                uOprValence;        // Operator ...     (0, 1, or 2) (0 = not an operator)
    LPYYSTYPE   lpYYStrandStart,    // Strand stack start (static)
                lpYYStrandBase,     // ...          base (dynamic)
                lpYYStrandNext,     // ...          next token (dynamic)
                lpYYResult,         // Ptr to result name or list
                lpYYLftArg,         // ...    left arg name or list
                lpYYLftOpr,         // ...    left operand name
                lpYYFcnName,        // ...    function/operator name
                lpYYRhtOpr,         // ...    right operand name
                lpYYRhtArg,         // ...    right arg name or list
                lpYYLocals;         // ...    locals name or list
} FHLOCALVARS, *LPFHLOCALVARS;

%}

%pure-parser
%name-prefix="fh_yy"
%parse-param {LPFHLOCALVARS lpfhLocalVars}
%lex-param   {LPFHLOCALVARS lpfhLocalVars}
%token NAMEUNK ASSIGN LINECONT UNK

%start Header

%%

/*

The set of cases we must handle is as follows:

                        FOO
                        FOO R
                        FOO (R)
                        FOO (R1 R2)
                      L FOO R
                      L FOO (R)
                      L FOO (R1 R2)
                    (L) FOO R
                    (L) FOO (R)
                    (L) FOO (R1 R2)
                (L1 L2) FOO R
                (L1 L2) FOO (R)
                (L1 L2) FOO (R1 R2)
                    [L] FOO R
                    [L] FOO (R)
                    [L] FOO (R1 R2)
                [L1 L2] FOO R
                [L1 L2] FOO (R)
                [L1 L2] FOO (R1 R2)
                       (FOO RO) R
                       (FOO RO) (R)
                       (FOO RO) (R1 R2)
                     L (FOO RO) R
                     L (FOO RO) (R)
                     L (FOO RO) (R1 R2)
                   (L) (FOO RO) R
                   (L) (FOO RO) (R)
                   (L) (FOO RO) (R1 R2)
               (L1 L2) (FOO RO) R
               (L1 L2) (FOO RO) (R)
               (L1 L2) (FOO RO) (R1 R2)
                   [L] (FOO RO) R
                   [L] (FOO RO) (R)
                   [L] (FOO RO) (R1 R2)
               [L1 L2] (FOO RO) R
               [L1 L2] (FOO RO) (R)
               [L1 L2] (FOO RO) (R1 R2)
                    (LO FOO RO) R
                    (LO FOO RO) (R)
                    (LO FOO RO) (R1 R2)
                  L (LO FOO RO) R
                  L (LO FOO RO) (R)
                  L (LO FOO RO) (R1 R2)
                (L) (LO FOO RO) R
                (L) (LO FOO RO) (R)
                (L) (LO FOO RO) (R1 R2)
            (L1 L2) (LO FOO RO) R
            (L1 L2) (LO FOO RO) (R)
            (L1 L2) (LO FOO RO) (R1 R2)
                [L] (LO FOO RO) R
                [L] (LO FOO RO) (R)
                [L] (LO FOO RO) (R1 R2)
            [L1 L2] (LO FOO RO) R
            [L1 L2] (LO FOO RO) (R)
            [L1 L2] (LO FOO RO) (R1 R2)

and then everything again three more times, prefaced with
one of the following phrases:

      Z {is}
    (Z) {is}
(Z1 Z2) {is}

that makes 3 x 55 = 165 cases.

 */


OpenList:
               NAMEUNK          {DbgMsgW2 (L"%%OpenList:  NAMEUNK");
                                 InitHdrStrand (&$1);
                                 $$ = *PushHdrStrand (&$1);
                                }
    | OpenList NAMEUNK          {DbgMsgW2 (L"%%OpenList:  OpenList NAMEUNK");
                                 $$ = *PushHdrStrand (&$2);
                                }
    ;

List:
      '(' OpenList ')'          {DbgMsgW2 (L"%%List:  '(' OpenList ')'");
                                 $$ = *MakeHdrStrand (&$2);
                                }
    ;

Result:
      List    ASSIGN            {DbgMsgW2 (L"%%Result:  List ASSIGN");
                                 lpfhLocalVars->lpYYResult = &$1;
                                 $$ = $1;
                                }
    | NAMEUNK ASSIGN            {DbgMsgW2 (L"%%Result:  NAMEUNK ASSIGN");
                                 InitHdrStrand (&$1);
                                 PushHdrStrand (&$1);
                                 $$ = *MakeHdrStrand (&$1);
                                 lpfhLocalVars->lpYYResult = $1.lpYYStrandBase;
                                }
    ;

OptArg:
      '[' List     ']'          {DbgMsgW2 (L"%%OptArg:  '[' List ']'");
                                 $$ = $2;
                                }
    | '[' OpenList ']'          {DbgMsgW2 (L"%%OptArg:  '[' OpenList ']'");
                                 $$ = *MakeHdrStrand (&$2);
                                }
    ;

RhtArg:
      List                      {DbgMsgW2 (L"%%RhtArg:  List");
                                 $$ = $1;
                                }
    | NAMEUNK                   {DbgMsgW2 (L"%%RhtArg:  NAMEUNK");
                                 InitHdrStrand (&$1);
                                 PushHdrStrand (&$1);
                                 $$ = *MakeHdrStrand (&$1);
                                }
    ;

NoResHdr:                       // N.B. that this production does not need to return a result (in $$)
                                //   because it calculates all the information we need into
                                //   lpfhLocalVars->lpYY... and lpfhLocalVars->u???Valence
              NAMEUNK           {DbgMsgW2 (L"%%NoResHdr:  NAMEUNK");                // Niladic function
                                 InitHdrStrand (&$1);
                                 PushHdrStrand (&$1);
                                 MakeHdrStrand (&$1);

                                 lpfhLocalVars->lpYYFcnName = $1.lpYYStrandBase;
                                 lpfhLocalVars->uFcnValence = 0;        // Mark as niladic
                                }

    |         NAMEUNK RhtArg    {DbgMsgW2 (L"%%NoResHdr:  NAMEUNK RhtArg");         // Monadic function

                                 InitHdrStrand (&$1);
                                 PushHdrStrand (&$1);
                                 MakeHdrStrand (&$1);

                                 lpfhLocalVars->lpYYFcnName = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $2.lpYYStrandBase;
                                 lpfhLocalVars->uFcnValence = 1;        // Mark as monadic
                                }

    | NAMEUNK NAMEUNK RhtArg    {DbgMsgW2 (L"%%NoResHdr:  NAMEUNK NAMEUNK RhtArg"); // Dyadic function

                                 InitHdrStrand (&$1);
                                 PushHdrStrand (&$1);
                                 MakeHdrStrand (&$1);

                                 InitHdrStrand (&$2);
                                 PushHdrStrand (&$2);
                                 MakeHdrStrand (&$2);

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYFcnName = $2.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->uFcnValence = 2;        // Mark as dyadic
                                }
    | List    NAMEUNK RhtArg    {DbgMsgW2 (L"%%NoResHdr:  List NAMEUNK RhtArg");    // Dyadic function

                                 InitHdrStrand (&$2);
                                 PushHdrStrand (&$2);
                                 MakeHdrStrand (&$2);

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYFcnName = $2.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->uFcnValence = 2;        // Mark as dyadic
                                }
    | OptArg  NAMEUNK RhtArg    {DbgMsgW2 (L"%%NoResHdr:  OptArg NAMEUNK RhtArg");  // Bivalent function

                                 InitHdrStrand (&$2);
                                 PushHdrStrand (&$2);
                                 MakeHdrStrand (&$2);

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYFcnName = $2.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->uFcnValence = 3;        // Mark as ambivalent
                                }
    |         List    RhtArg    {DbgMsgW2 (L"%%NoResHdr:  List RhtArg");            // Mon/Dyd operator, monadic function
                                 if (!GetOprName (&$1))
                                    YYERROR;

                                 lpfhLocalVars->lpYYRhtArg  = $2.lpYYStrandBase;
                                 lpfhLocalVars->uFcnValence = 1;        // Mark as monadic
                                }
    | NAMEUNK List    RhtArg    {DbgMsgW2 (L"%%NoResHdr:  NAMEUNK List RhtArg");    // Mon/Dyd operator, dyadic function
                                 if (!GetOprName (&$2))
                                    YYERROR;

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->uFcnValence = 2;        // Mark as dyadic
                                }
    | List    List    RhtArg    {DbgMsgW2 (L"%%NoResHdr:  List List RhtArg");       // Mon/Dyd operator, dyadic function
                                 if (!GetOprName (&$2))
                                    YYERROR;

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->uFcnValence = 2;        // Mark as dyadic
                                }
    | OptArg  List    RhtArg    {DbgMsgW2 (L"%%NoResHdr:  OptArg List RhtArg");     // Mon/Dyd operator, bivalent function
                                 if (!GetOprName (&$2))
                                    YYERROR;

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->uFcnValence = 3;        // Mark as ambivalent
                                }
    ;

Locals:
              LINECONT          {DbgMsgW2 (L"%%Locals:  LINECONT");
                                }
    |         ';'      NAMEUNK  {DbgMsgW2 (L"%%Locals:  ':' NAMEUNK");
                                 InitHdrStrand (&$2);
                                 $$ = *PushHdrStrand (&$2);
                                }
    | Locals  LINECONT          {DbgMsgW2 (L"%%Locals:  Locals LINECONT");
                                }
    | Locals  ';'      NAMEUNK  {DbgMsgW2 (L"%%Locals:  Locals ':' NAMEUNK");
                                 $$ = *PushHdrStrand (&$3);
                                }
    ;

Header:
              error             {DbgMsgW2 (L"%%Header:  error");
                                 YYABORT;
                                }

    |         NoResHdr          {DbgMsgW2 (L"%%Header:  NoResHdr");
#ifdef DEBUG
                                 DisplayFnHdr (lpfhLocalVars);
#endif
                                }
    |         NoResHdr Locals   {DbgMsgW2 (L"%%Header:  NoResHdr Locals");
                                 lpfhLocalVars->lpYYLocals = MakeHdrStrand (&$2);
#ifdef DEBUG
                                 DisplayFnHdr (lpfhLocalVars);
#endif
                                }
    | Result  NoResHdr          {DbgMsgW2 (L"%%Header:  Result NoResHdr");
#ifdef DEBUG
                                 DisplayFnHdr (lpfhLocalVars);
#endif
                                }
    | Result  NoResHdr Locals   {DbgMsgW2 (L"%%Header:  Result NoResHdr Locals");
                                 lpfhLocalVars->lpYYLocals = MakeHdrStrand (&$3);
#ifdef DEBUG
                                 DisplayFnHdr (lpfhLocalVars);
#endif
                                }
    ;

%%

//***************************************************************************
//  Start of C program
//***************************************************************************

#ifdef DEBUG
//***************************************************************************
//  DisplayFnHdr
//
//  Display the function header
//***************************************************************************

void DisplayFnHdr
    (LPFHLOCALVARS lpfhLocalVars)

{
    WCHAR     wszTemp[1024];
    HGLOBAL   hGlbName;
    LPAPLCHAR lpMemName;
    UINT      uLen, uItm;

    // Ensure properly terminated
    wszTemp[0] = L'\0';

    // Check for result
    if (lpfhLocalVars->lpYYResult)
    {
        // Get the strand length
        uLen = lpfhLocalVars->lpYYResult->uStrandLen;

        // Append a separator
        if (uLen > 1)
            lstrcatW (wszTemp, L"(");

        for (uItm = 0; uItm < uLen; uItm++)
        {
            // Get the Name's global memory handle
            hGlbName = lpfhLocalVars->lpYYResult[uItm].tkToken.tkData.tkSym->lpHshEntry->hGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLock (hGlbName);

            // Copy the name
            lstrcatW (wszTemp, lpMemName);

            // If we're not at the last item, separate with a space
            if (uItm < (uLen - 1))
                lstrcatW (wszTemp, L" ");

            // We no longer need this ptr
            MyGlobalUnlock (hGlbName); lpMemName = NULL;
        } // End FOR

        // Append a separator
        if (uLen > 1)
            lstrcatW (wszTemp, L")");

        lstrcatW (wszTemp, WS_UTF16_LEFTARROW);
    } // End IF

    // Check for left argument
    if (lpfhLocalVars->lpYYLftArg)
    {
        // Get the strand length
        uLen = lpfhLocalVars->lpYYLftArg->uStrandLen;

        // Append a separator
        if (lpfhLocalVars->uFcnValence EQ 3)
            lstrcatW (wszTemp, L"[");
        else
        if (uLen > 1)
            lstrcatW (wszTemp, L"(");

        for (uItm = 0; uItm < uLen; uItm++)
        {
            // Get the Name's global memory handle
            hGlbName = lpfhLocalVars->lpYYLftArg[uItm].tkToken.tkData.tkSym->lpHshEntry->hGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLock (hGlbName);

            // Copy the name
            lstrcatW (wszTemp, lpMemName);

            // If we're not at the last item, separate with a space
            if (uItm < (uLen - 1))
                lstrcatW (wszTemp, L" ");

            // We no longer need this ptr
            MyGlobalUnlock (hGlbName); lpMemName = NULL;
        } // End FOR

        // Append a separator
        if (lpfhLocalVars->uFcnValence EQ 3)
            lstrcatW (wszTemp, L"]");
        else
        if (uLen > 1)
            lstrcatW (wszTemp, L")");
    } // End IF

    // Check for operator
    if (lpfhLocalVars->uOprValence > 0)
    {
        // Append a separator
        lstrcatW (wszTemp, L"(");

        // Check for left operand
        if (lpfhLocalVars->lpYYLftOpr)
        {
            // Get the Name's global memory handle
            hGlbName = lpfhLocalVars->lpYYLftOpr->tkToken.tkData.tkSym->lpHshEntry->hGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLock (hGlbName);

            // Copy the name
            lstrcatW (wszTemp, lpMemName);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbName); lpMemName = NULL;

            // Append a separator
            lstrcatW (wszTemp, L" ");
        } // End IF

        // Get the Name's global memory handle
        hGlbName = lpfhLocalVars->lpYYFcnName->tkToken.tkData.tkSym->lpHshEntry->hGlbName;

        // Lock the memory to get a ptr to it
        lpMemName = MyGlobalLock (hGlbName);

        // Copy the name
        lstrcatW (wszTemp, lpMemName);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbName); lpMemName = NULL;

        // Get the Name's global memory handle
        hGlbName = lpfhLocalVars->lpYYRhtOpr->tkToken.tkData.tkSym->lpHshEntry->hGlbName;

        // Lock the memory to get a ptr to it
        lpMemName = MyGlobalLock (hGlbName);

        // Copy the name
        lstrcatW (wszTemp, lpMemName);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbName); lpMemName = NULL;

        // Append a separator
        lstrcatW (wszTemp, L")");
    } else
    {
        // Get the Name's global memory handle
        hGlbName = lpfhLocalVars->lpYYFcnName->tkToken.tkData.tkSym->lpHshEntry->hGlbName;

        // Lock the memory to get a ptr to it
        lpMemName = MyGlobalLock (hGlbName);

        // Copy the name
        lstrcatW (wszTemp, lpMemName);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbName); lpMemName = NULL;
    } // End IF/ELSE

    // Check for right argument
    if (lpfhLocalVars->lpYYRhtArg)
    {
        // Append a separator
        lstrcatW (wszTemp, L" ");

        // Get the strand length
        uLen = lpfhLocalVars->lpYYRhtArg->uStrandLen;

        // Append a separator
        if (uLen > 1)
            lstrcatW (wszTemp, L"(");

        for (uItm = 0; uItm < uLen; uItm++)
        {
            // Get the Name's global memory handle
            hGlbName = lpfhLocalVars->lpYYRhtArg[uItm].tkToken.tkData.tkSym->lpHshEntry->hGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLock (hGlbName);

            // Copy the name
            lstrcatW (wszTemp, lpMemName);

            // If we're not at the last item, separate with a space
            if (uItm < (uLen - 1))
                lstrcatW (wszTemp, L" ");

            // We no longer need this ptr
            MyGlobalUnlock (hGlbName); lpMemName = NULL;
        } // End FOR

        // Append a separator
        if (uLen > 1)
            lstrcatW (wszTemp, L")");
    } // End IF

    // Display it in the debug window
    DbgMsgW (wszTemp);
} // End DisplayFnHdr
#endif


//***************************************************************************
//  $ParseHeader
//
//  Parse a function header
//***************************************************************************

BOOL ParseHeader
    (HWND    hWndEC,            // Window handle of Edit Control
     HGLOBAL hGlbToken)         // Token global memory handle

{
    BOOL        bRet = FALSE;   // TRUE iff result is valid
    FHLOCALVARS fhLocalVars = {0};  // Re-entrant vars

    // Save the window handle
    fhLocalVars.hWndEC = hWndEC;

    // Save the thread's ptr to local vars
    TlsSetValue (dwTlsFhLocalVars, (LPVOID) &fhLocalVars);

    // Lock the handle and set variables
    fhLocalVars.hGlbToken = hGlbToken;
    UTLockAndSet (fhLocalVars.hGlbToken, &fhLocalVars.t2);

    // Skip over TOKEN_HEADER
    fhLocalVars.lpStart = TokenBaseToStart (fhLocalVars.t2.lpBase);

////// Skip to end of 1st stmt
////fhLocalVars.lpNext  = &fhLocalVars.lpStart[fhLocalVars.lpStart->tkData.tkIndex];
    // Skip over the starting EOL
    fhLocalVars.lpNext  = &fhLocalVars.lpStart[1];

    // Put a terminating EOL at the end
    fhLocalVars.lpStart[fhLocalVars.lpStart->tkData.tkChar] = *fhLocalVars.lpStart;

    // Start off with no error
    fhLocalVars.tkErrorCharIndex = NEG1U;

    // Allocate virtual memory for the Variable Strand accumulator
    fhLocalVars.lpYYStrandStart =
        VirtualAlloc (NULL,      // Any address
                      DEF_STRAND_MAXSIZE * sizeof (YYSTYPE),
                      MEM_RESERVE,
                      PAGE_READWRITE);
    if (!fhLocalVars.lpYYStrandStart)
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsg ("ParseHeader:  VirtualAlloc for <fhocalVars.lpYYStrandStart> failed");

        goto ERROR_EXIT;    // Mark as failed
    } // End IF

    // Commit the intial size
    VirtualAlloc (fhLocalVars.lpYYStrandStart,
                  DEF_STRAND_INITSIZE * sizeof (YYSTYPE),
                  MEM_COMMIT,
                  PAGE_READWRITE);
    // Initialize the base & next strand ptrs
    fhLocalVars.lpYYStrandBase =
    fhLocalVars.lpYYStrandNext = fhLocalVars.lpYYStrandStart;

    // Use VirtualAlloc for the parser stack
    // ***FIXME***














#if YYDEBUG
    // Enable debugging
    yydebug = 1;
#endif

    // Parse the header, check for errors
    bRet = fh_yyparse (&fhLocalVars) EQ 0;
ERROR_EXIT:
    // Free the virtual memory we allocated above
    if (fhLocalVars.lpYYStrandStart)
    {
        VirtualFree (fhLocalVars.lpYYStrandStart, 0, MEM_RELEASE); fhLocalVars.lpYYStrandStart = NULL;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (fhLocalVars.hGlbToken); fhLocalVars.t2.lpBase = NULL;

    return bRet;
} // End ParseHeader


//***************************************************************************
//  $fh_yylex
//
//  Lexical analyzer for YACC
//***************************************************************************

int fh_yylex
    (LPYYSTYPE     lpYYLval,            // Ptr to lval
     LPFHLOCALVARS lpfhLocalVars)       // Ptr to local fhLocalVars

{
    // Return the current token
    lpYYLval->tkToken        = *lpfhLocalVars->lpNext;
    lpYYLval->uStrandLen     = 0;
    lpYYLval->lpYYStrandBase = lpfhLocalVars->lpYYStrandBase;

    // Split cases based upon the token type
    switch (lpfhLocalVars->lpNext++->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            return NAMEUNK;

        case TKT_ASSIGN:
            return ASSIGN;

        case TKT_LISTSEP:
            return ';';

        case TKT_LPAREN:
            return '(';

        case TKT_RPAREN:
            return ')';

        case TKT_LBRACKET:
            return '[';

        case TKT_RBRACKET:
            return ']';

        case TKT_EOL:
            return '\0';

        default:
            return UNK;
    } // End SWITCH
} // End fh_yylex


//***************************************************************************
//  $fh_yyerror
//
//  Error callback from YACC
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- fh_yyerror"
#else
#define APPEND_NAME
#endif

void fh_yyerror                     // Called for yacc syntax error
    (LPFHLOCALVARS lpfhLocalVars,   // Ptr to local fhLocalVars
     LPCHAR        s)               // Ptr to error msg

{
    char szTemp[10244], *p;
    UINT uCharIndex;

#ifdef DEBUG
    DbgMsg (s);
#endif

    // Get and save the character index position
    uCharIndex = lpfhLocalVars->lpNext->tkCharIndex;
    lpfhLocalVars->tkErrorCharIndex = uCharIndex;

    // Check for SYNTAX ERROR
#define ERR     "syntax error"
    lstrcpyn (szTemp, s, sizeof (ERR));     // Note: Terminates the string, too
    if (lstrcmp (szTemp, ERR) EQ 0)
    {
        wsprintf (szTemp, "Syntax Error in line 0, position %d -- function NOT saved.", uCharIndex);
        p = szTemp;

        goto DISPLAYCAT;
#undef  ERR
    } // End IF

#define ERR     "memory exhausted"
    lstrcpyn (szTemp, s, sizeof (ERR));     // Note: Terminates the string, too
    if (lstrcmp (szTemp, ERR) EQ 0)
    {
        wsprintf (szTemp, "Insufficient memory -- function NOT saved.");
        p = szTemp;

        goto DISPLAYCAT;
#undef  ERR
    } // End IF

    // Use the error message as given
    p = s;

    goto DISPLAY;

DISPLAYCAT:
#ifdef DEBUG
    lstrcat (szTemp, "(");
    lstrcat (szTemp,  s );
    lstrcat (szTemp, ")");
#endif
DISPLAY:
    // Display a message box
    MessageBox (lpfhLocalVars->hWndEC,
                p,
                pszAppName,
                MB_OK | MB_ICONWARNING | MB_APPLMODAL);
} // End fh_yyerror
#undef  APPEND_NAME


//***************************************************************************
//  $fh_yyfprintf
//
//  Debugger output
//***************************************************************************

void fh_yyfprintf
    (FILE  *hfile,          // Ignore this
     LPCHAR lpszFmt,        // Format string
     ...)                   // Zero or more arguments

{
#if FALSE
    va_list vl;
    int     i1, i2, i3;
    static char szTemp[256] = {'\0'};

    va_start (vl, lpszFmt);

    // Bison uses no more than three arguments.
    // Note we must grab them separately this way
    //   as using va_arg in the argument list to
    //   wsprintf pushes the arguments in reverse
    //   order.
    i1 = va_arg (vl, int);
    i2 = va_arg (vl, int);
    i3 = va_arg (vl, int);

    wsprintf (lpszDebug,
              lpszFmt,
              i1, i2, i3);
    // Accumulate into local buffer because
    //   Bison calls this function multiple
    //   times for the same line, terminating
    //   the last call for the line with a LF.
    lstrcat (szTemp, lpszDebug);

    // Check last character.
    i1 = lstrlen (szTemp);

    // If it's a LF, it's time to flush the buffer.
    if (szTemp[i1 - 1] EQ '\n')
    {
        szTemp[i1 - 1] = '\0';  // Remove trailing LF
                                //   because we're displaying
                                //   in a GUI.
        DbgMsg (szTemp);        // Display in my debugger window.

        szTemp[0] = '\0';       // Restart the buffer
    } // End IF/ELSE

    va_end (vl);
#endif
} // End fh_yyfprintf


//***************************************************************************
//  $InitHdrStrand
//
//  Initialize the header strand stack.
//***************************************************************************

void InitHdrStrand
    (LPYYSTYPE lpYYArg)             // Ptr to the incoming argument

{
    LPFHLOCALVARS lpfhLocalVars;    // Ptr to local fhLocalVars

    // Get this thread's LocalVars ptr
    lpfhLocalVars = (LPFHLOCALVARS) TlsGetValue (dwTlsFhLocalVars);

    // Set the base of this strand to the next available location
    lpYYArg->lpYYStrandBase       =
    lpfhLocalVars->lpYYStrandBase = lpfhLocalVars->lpYYStrandNext;
} // End InitHdrStrand


//***************************************************************************
//  $PushHdrStrand_YY
//
//  Push a header token onto the strand stack.
//***************************************************************************

LPYYSTYPE PushHdrStrand
    (LPYYSTYPE lpYYArg)             // Ptr to the incoming argument

{
    LPFHLOCALVARS lpfhLocalVars;    // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpfhLocalVars = (LPFHLOCALVARS) TlsGetValue (dwTlsFhLocalVars);

    // Copy the strand base to the result
    lpYYArg->lpYYStrandBase = lpfhLocalVars->lpYYStrandBase;

    // Save this token on the strand stack
    //   and skip over it
    *lpfhLocalVars->lpYYStrandNext++ = *lpYYArg;

    return lpYYArg;
} // End PushHdrStrand


//***************************************************************************
//  $MakeHdrStrand
//
//  Make a header strand
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeHdrStrand"
#else
#define APPEND_NAME
#endif

LPYYSTYPE MakeHdrStrand
    (LPYYSTYPE lpYYArg)             // Ptr to incoming token

{
    LPFHLOCALVARS lpfhLocalVars;    // Ptr to local fhLocalVars

    // Get this thread's LocalVars ptr
    lpfhLocalVars = (LPFHLOCALVARS) TlsGetValue (dwTlsFhLocalVars);

    // Save the # elements in the strand
    lpYYArg->lpYYStrandBase->uStrandLen =
      lpfhLocalVars->lpYYStrandNext - lpYYArg->lpYYStrandBase;

    // Save the next position as the new base
    lpfhLocalVars->lpYYStrandBase = lpfhLocalVars->lpYYStrandNext;

    // Return the base of the current strand
    return lpYYArg->lpYYStrandBase;
} // End MakeHdrStrand
#undef  APPEND_NAME


//***************************************************************************
//  GetOprName
//
//  Extract the operator name and valence from a list
//***************************************************************************

BOOL GetOprName
    (LPYYSTYPE lpYYArg)

{
    LPFHLOCALVARS lpfhLocalVars;    // Ptr to local fhLocalVars

    // Get this thread's LocalVars ptr
    lpfhLocalVars = (LPFHLOCALVARS) TlsGetValue (dwTlsFhLocalVars);

    // Split cases based upon the strand length
    switch (lpYYArg->uStrandLen)
    {
        case 2:         // Monadic operator
            lpfhLocalVars->uOprValence = 1;
            lpfhLocalVars->lpYYFcnName = &lpYYArg->lpYYStrandBase[0];
            lpfhLocalVars->lpYYRhtOpr  = &lpYYArg->lpYYStrandBase[1];

            return TRUE;

        case 3:         // Dyadic operator
            lpfhLocalVars->uOprValence = 2;
            lpfhLocalVars->lpYYLftOpr  = &lpYYArg->lpYYStrandBase[0];
            lpfhLocalVars->lpYYFcnName = &lpYYArg->lpYYStrandBase[1];
            lpfhLocalVars->lpYYRhtOpr  = &lpYYArg->lpYYStrandBase[2];

            return TRUE;

        default:
            return FALSE;
    } // End SWITCH
} // End GetOprName


//***************************************************************************
//  End of File: fnhdr.y
//***************************************************************************
