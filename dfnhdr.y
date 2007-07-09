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
#include "dfnhdr.h"

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

// The following #defines are needed to allow multiple parses
//   to coexist in the same file
#define yy_symbol_print         fh_yy_symbol_print
#define yy_symbol_value_print   fh_yy_symbol_value_print
#define yy_reduce_print         fh_yy_reduce_print
#define yydestruct              fh_yydestruct

#define DbgMsgW2(a) DbgMsgW(a)
////#define DbgMsgW2(a) DbgMsgW(a); DbgBrk ()

%}

%pure-parser
%name-prefix="fh_yy"
%parse-param {LPFHLOCALVARS lpfhLocalVars}
%lex-param   {LPFHLOCALVARS lpfhLocalVars}
%token NAMEUNK NAMESYS ASSIGN LINECONT UNK

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
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_NIL;       // Mark as niladic
                                }

    |         NAMEUNK RhtArg    {DbgMsgW2 (L"%%NoResHdr:  NAMEUNK RhtArg");         // Monadic function
                                 InitHdrStrand (&$1);
                                 PushHdrStrand (&$1);
                                 MakeHdrStrand (&$1);

                                 lpfhLocalVars->lpYYFcnName = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $2.lpYYStrandBase;
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_MON;       // Mark as monadic
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
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_DYD;       // Mark as dyadic
                                }
    | List    NAMEUNK RhtArg    {DbgMsgW2 (L"%%NoResHdr:  List NAMEUNK RhtArg");    // Dyadic function
                                 InitHdrStrand (&$2);
                                 PushHdrStrand (&$2);
                                 MakeHdrStrand (&$2);

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYFcnName = $2.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_DYD;       // Mark as dyadic
                                }
    | OptArg  NAMEUNK RhtArg    {DbgMsgW2 (L"%%NoResHdr:  OptArg NAMEUNK RhtArg");  // Bivalent function
                                 InitHdrStrand (&$2);
                                 PushHdrStrand (&$2);
                                 MakeHdrStrand (&$2);

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYFcnName = $2.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_AMB;       // Mark as ambivalent
                                }
    |         List    RhtArg    {DbgMsgW2 (L"%%NoResHdr:  List RhtArg");            // Mon/Dyd operator, monadic derived function
                                 if (!GetOprName (&$1))
                                     YYERROR;

                                 lpfhLocalVars->lpYYRhtArg  = $2.lpYYStrandBase;
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_MON;       // Mark as monadic
                                }
    | NAMEUNK List    RhtArg    {DbgMsgW2 (L"%%NoResHdr:  NAMEUNK List RhtArg");    // Mon/Dyd operator, dyadic derived function
                                 if (!GetOprName (&$2))
                                     YYERROR;

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_MON;       // Mark as monadic
                                }
    | List    List    RhtArg    {DbgMsgW2 (L"%%NoResHdr:  List List RhtArg");       // Mon/Dyd operator, dyadic derived function
                                 if (!GetOprName (&$2))
                                     YYERROR;

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_DYD;       // Mark as dyadic
                                }
    | OptArg  List    RhtArg    {DbgMsgW2 (L"%%NoResHdr:  OptArg List RhtArg");     // Mon/Dyd operator, ambivalent derived function
                                 if (!GetOprName (&$2))
                                     YYERROR;

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_AMB;       // Mark as ambivalent
                                }
    ;

Locals:
              LINECONT          {DbgMsgW2 (L"%%Locals:  LINECONT");
                                }
    |         ';'      NAMEUNK  {DbgMsgW2 (L"%%Locals:  ':' NAMEUNK");
                                 InitHdrStrand (&$2);
                                 $$ = *PushHdrStrand (&$2);
                                }
    |         ';'      NAMESYS  {DbgMsgW2 (L"%%Locals:  ':' NAMESYS");
                                 if (NameIsAxis (&$2.tkToken))
                                     lpfhLocalVars->DfnAxis = 1;
                                 else
                                 if (!$2.tkToken.tkData.tkSym->stFlags.Value)
                                     YYERROR;
                                 InitHdrStrand (&$2);
                                 $$ = *PushHdrStrand (&$2);
                                }
    | Locals  LINECONT          {DbgMsgW2 (L"%%Locals:  Locals LINECONT");
                                }
    | Locals  ';'      NAMEUNK  {DbgMsgW2 (L"%%Locals:  Locals ';' NAMEUNK");
                                 $$ = *PushHdrStrand (&$3);
                                }
    | Locals  ';'      NAMESYS  {DbgMsgW2 (L"%%Locals:  Locals ':' NAMESYS");
                                 if (NameIsAxis (&$3.tkToken))
                                     lpfhLocalVars->DfnAxis = 1;
                                 else
                                 if (!$3.tkToken.tkData.tkSym->stFlags.Value)
                                     YYERROR;
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

//***************************************************************************
//  $ParseHeader
//
//  Parse a function header
//***************************************************************************

BOOL ParseHeader
    (HWND          hWndEC,          // Window handle of Edit Control
     HGLOBAL       hGlbTknHdr,      // Tokenized header global memory handle
     LPFHLOCALVARS lpfhLocalVars)   // Local vars

{
    BOOL bRet = FALSE;          // TRUE iff result is valid

    // Save the window handle
    lpfhLocalVars->hWndEC = hWndEC;

    // Save the thread's ptr to local vars
    TlsSetValue (dwTlsFhLocalVars, (LPVOID) lpfhLocalVars);

    // Save the token header global memory handle
    lpfhLocalVars->hGlbTknHdr = hGlbTknHdr;

    // Lock the handle to get a ptr to it, and set the variables
    UTLockAndSet (lpfhLocalVars->hGlbTknHdr, &lpfhLocalVars->t2);

    // Initialize the base & next strand ptrs
    lpfhLocalVars->lpYYStrandBase =
    lpfhLocalVars->lpYYStrandNext = lpfhLocalVars->lpYYStrandStart;

    // Skip over TOKEN_HEADER
    lpfhLocalVars->lpStart = TokenBaseToStart (lpfhLocalVars->t2.lpBase);

    // Skip over the starting EOL
    lpfhLocalVars->lpNext  = &lpfhLocalVars->lpStart[1];

    // Mark the stopping point
    lpfhLocalVars->lpStop  = &lpfhLocalVars->lpStart[lpfhLocalVars->lpStart->tkData.tkChar];

    // Start off with no error
    lpfhLocalVars->tkErrorCharIndex = NEG1U;

    // Use VirtualAlloc for the parser stack
    // ***FIXME***














#if YYDEBUG
    // Enable debugging
    yydebug = 1;
#endif

    // Parse the header, check for errors
    bRet = fh_yyparse (lpfhLocalVars) EQ 0;

    // We no longer need this ptr
    MyGlobalUnlock (lpfhLocalVars->hGlbTknHdr); lpfhLocalVars->t2.lpBase = NULL;

    return bRet;
} // End ParseHeader


//***************************************************************************
//  $ValidSysName
//
//  Validate the name as a system variable name
//***************************************************************************

BOOL ValidSysName
    (LPTOKEN lptkName)      // Ptr to name token

{
    return (lptkName->tkData.tkSym->stFlags.ObjName EQ OBJNAME_SYS);
} // End ValidSysName


//***************************************************************************
//  $NameIsAxis
//
//  Return TRUE if the token's name is {quad}axis
//***************************************************************************

BOOL NameIsAxis
    (LPTOKEN lptkName)      // Ptr to name token

{
    HGLOBAL   hGlbName;     // HTE name global memory handle
    LPAPLCHAR lpMemName;    // Ptr to STE name global memory
    BOOL      bRet;         // TRUE iff the name is {quad}axis

    // tkData is an LPSYMENTRY
    Assert (GetPtrTypeDir (lptkName->tkData.tkVoid) EQ PTRTYPE_STCONST);

    // Get the STE name's global memory handle
    hGlbName = lptkName->tkData.tkSym->stHshEntry->htGlbName;

    // Lock the memory to get a ptr to it
    lpMemName = MyGlobalLock (hGlbName);

    // Compare the two names, case insensitive
    bRet = lstrcmpiW (lpMemName, WS_UTF16_QUAD L"axis") EQ 0;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbName); lpMemName = NULL;

    return bRet;
} // End NameIsAxis


//***************************************************************************
//  $fh_yylex
//
//  Lexical analyzer for Bison
//***************************************************************************

int fh_yylex
    (LPYYSTYPE     lpYYLval,            // Ptr to lval
     LPFHLOCALVARS lpfhLocalVars)       // Ptr to local fhLocalVars

{
    // Return the current token
    lpYYLval->tkToken        = *lpfhLocalVars->lpNext;
    lpYYLval->uStrandLen     = 0;
    lpYYLval->lpYYStrandBase = lpfhLocalVars->lpYYStrandBase;

    // Check for stopping point
    if (lpfhLocalVars->lpStop EQ lpfhLocalVars->lpNext)
        return '\0';

    // Split cases based upon the token type
    switch (lpfhLocalVars->lpNext++->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            if (lpfhLocalVars->lpNext[-1].tkData.tkSym->stFlags.ObjName EQ OBJNAME_SYS)
                return NAMESYS;
            else
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

        default:
            return UNK;
    } // End SWITCH
} // End fh_yylex


//***************************************************************************
//  $fh_yyerror
//
//  Error callback from Bison
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- fh_yyerror"
#else
#define APPEND_NAME
#endif

void fh_yyerror                     // Called for Bison syntax error
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
                lpszAppName,
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
#ifdef DEBUG
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

    va_end (vl);

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
//  $GetOprName
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
            lpfhLocalVars->DfnType     = DFNTYPE_OP1;
            lpfhLocalVars->lpYYFcnName = &lpYYArg->lpYYStrandBase[0];
            lpfhLocalVars->lpYYRhtOpr  = &lpYYArg->lpYYStrandBase[1];

            return TRUE;

        case 3:         // Dyadic operator
            lpfhLocalVars->DfnType     = DFNTYPE_OP2;
            lpfhLocalVars->lpYYLftOpr  = &lpYYArg->lpYYStrandBase[0];
            lpfhLocalVars->lpYYFcnName = &lpYYArg->lpYYStrandBase[1];
            lpfhLocalVars->lpYYRhtOpr  = &lpYYArg->lpYYStrandBase[2];

            return TRUE;

        default:
            return FALSE;
    } // End SWITCH
} // End GetOprName


//***************************************************************************
//  End of File: dfnhdr.y
//***************************************************************************
