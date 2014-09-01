//***************************************************************************
//  NARS2000 -- Parse A Line
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2014 Sudley Place Software

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

//***************************************************************************
//  This parser comes from "APL two by two-syntax analysis by pairwise reduction",
//    Bunda, J.D. and Gerth, J.A.
//    APL '84 Proceedings of the International Conference on APL.
//***************************************************************************

//***************************************************************************
//  TODO
//
//  Error Handling
//  Ctrl-Break
//
//***************************************************************************

//***************************************************************************
// Aliases
//***************************************************************************
#define plRedSA_F       plRedA_F
#define plRedNAM_F      plRedA_F
#define plRedIO_F       plRedA_F
#define plRedSA_FR      plRedA_F
#define plRedNAM_FR     plRedA_F
#define plRedIO_FR      plRedA_F
#define plRedA_FR       plRedA_F

#define plRedA_IO       plRedA_A
#define plRedNAM_IO     plRedA_A
#define plRedA_NAM      plRedA_A
#define plRedNAM_NAM    plRedA_A
#define plRedNAM_A      plRedA_A
#define plRedIO_A       plRedA_A
#define plRedIO_IO      plRedA_A
#define plRedIO_NAM     plRedA_A
#define plRedSA_IO      plRedSA_A
#define plRedSA_NAM     plRedSA_A

#define plRedMF_SA      plRedMF_A
#define plRedF_A        plRedMF_A
#define plRedF_SA       plRedMF_A
#define plRedF_IO       plRedMF_A
#define plRedMF_IO      plRedMF_A

#define plRedNF_IO      plRedNF_A
#define plRedNF_NAM     plRedNF_A

#define plRedF_NF       plRedMF_NF

#define plRedNAM_NF     plRedA_NF
#define plRedIO_NF      plRedA_NF

#define plRedSYSN_MF    plRedSYSN_F

#define plRedGO_SA      plRedGO_A
#define plRedGO_IO      plRedGO_A

#define plRedSA_HR      plRedA_HR
#define plRedNAM_HR     plRedA_HR
#define plRedIO_HR      plRedA_HR

#define plRedSA_HY      plRedA_HY
#define plRedIO_HY      plRedA_HY
#define plRedNAM_HY     plRedA_HY

#define plRedJD_HY      plRedDOP_HY
#define plRedDOPN_HY    plRedDOP_HY

#define plRedDOP_IDX    plRedF_IDX
#define plRedDOPN_IDX   plRedF_IDX


#define plRedDOP_SA     plRedDOP_RhtOper
#define plRedDOP_IO     plRedDOP_RhtOper
#define plRedDOPN_A     plRedDOP_RhtOper
#define plRedDOPN_SA    plRedDOP_RhtOper
#define plRedDOPN_IO    plRedDOP_RhtOper
#define plRedDOPN_F     plRedDOP_RhtOper
#define plRedDOPN_MF    plRedDOP_RhtOper
#define plRedDOPN_J     plRedDOP_RhtOper
#define plRedDOP_A      plRedDOP_RhtOper
#define plRedDOP_F      plRedDOP_RhtOper
#define plRedDOP_MF     plRedDOP_RhtOper
#define plRedDOPN_NF    plRedDOP_RhtOper
#define plRedDOP_NF     plRedDOP_RhtOper
#define plRedDOP_J      plRedDOP_RhtOper
#define plRedJ_A        plRedDOP_RhtOper
#define plRedJ_SA       plRedDOP_RhtOper
#define plRedJ_IO       plRedDOP_RhtOper
#define plRedJ_F        plRedDOP_RhtOper
#define plRedJ_NF       plRedDOP_RhtOper
#define plRedJD_F       plRedDOP_RhtOper
#define plRedJD_MF      plRedDOP_RhtOper
#define plRedJD_A       plRedDOP_RhtOper
#define plRedJD_SA      plRedDOP_RhtOper
#define plRedJD_IO      plRedDOP_RhtOper

#define plRedJ_MOP      plRedLftOper_MOP
#define plRedJ_MOPN     plRedLftOper_MOP
#define plRedSA_MR      plRedLftOper_MOP
#define plRedNAM_MR     plRedLftOper_MOP
#define plRedIO_MR      plRedLftOper_MOP
#define plRedSA_MOP     plRedLftOper_MOP
#define plRedNAM_MOP    plRedLftOper_MOP
#define plRedIO_MOP     plRedLftOper_MOP
#define plRedA_MOP      plRedLftOper_MOP
#define plRedSA_MOPN    plRedLftOper_MOP
#define plRedNAM_MOPN   plRedLftOper_MOP
#define plRedIO_MOPN    plRedLftOper_MOP
#define plRedA_MOPN     plRedLftOper_MOP
#define plRedA_MR       plRedLftOper_MOP
#define plRedF_MOP      plRedLftOper_MOP
#define plRedF_MOPN     plRedLftOper_MOP
#define plRedMF_MOP     plRedLftOper_MOP
#define plRedMF_MOPN    plRedLftOper_MOP
#define plRedF_MR       plRedLftOper_MOP
#define plRedHY_MR      plRedLftOper_MOP
#define plRedF_HR       plRedLftOper_MOP

#define plRedNF_MOPN    plRedNF_MOP

// Check the action of the following MOP vs. MOP
#define plRedMOP_MOP    plRedLftOper_MOP
#define plRedMOPN_MOP   plRedLftOper_MOP
#define plRedMOP_MOPN   plRedLftOper_MOP
#define plRedMOPN_MOPN  plRedLftOper_MOP

#define plRedF_RP       plRedCom_RP
#define plRedMOP_RP     plRedCom_RP
#define plRedDOP_RP     plRedCom_RP
#define plRedHY_RP      plRedCom_RP

#define plRedSP_SA      plRedSP_A
#define plRedMF_HY      plRedF_HY

#define plRedMOPN_IDX   plRedMOP_IDX

#define plRedSA_IDX     plRedA_IDX
#define plRedNAM_IDX    plRedA_IDX
#define plRedIO_IDX     plRedA_IDX

#define plRedMF_IDX     plRedF_IDX
#define plRedHY_IDX     plRedF_IDX

#define plRedSP_F       plRedSP_Com
#define plRedSP_FR      plRedSP_Com
#define plRedSP_MR      plRedSP_Com
#define plRedSP_DR      plRedSP_Com
#define plRedSP_HR      plRedSP_Com
#define plRedSP_HY      plRedSP_Com
#define plRedSP_J       plRedSP_Com
#define plRedSP_MOP     plRedSP_Com
#define plRedSP_MOPN    plRedSP_Com
#define plRedSP_DOP     plRedSP_Com
#define plRedSP_DOPN    plRedSP_Com
#define plRedSP_IO      plRedSP_Com

#define plRedNAM_SPA    plRedNAM_SPCom
#define plRedNAM_SPF    plRedNAM_SPCom
#define plRedNAM_SPM    plRedNAM_SPCom
#define plRedNAM_SPMN   plRedNAM_SPCom
#define plRedNAM_SPMR   plRedNAM_SPCom
#define plRedNAM_SPD    plRedNAM_SPCom
#define plRedNAM_SPDN   plRedNAM_SPCom
#define plRedNAM_SPDR   plRedNAM_SPCom
#define plRedNAM_SPHY   plRedNAM_SPCom
#define plRedNAM_SPHR   plRedNAM_SPCom
#define plRedNAM_SPFR   plRedNAM_SPCom


#define plRedA_ISPA     plRedNAM_ISPA

#define plRedNAM_NNR    plRedNAM_NR

#define plRedLP_FR      plRedLP_Com
#define plRedLP_MR      plRedLP_Com
#define plRedLP_DR      plRedLP_Com
#define plRedLP_HR      plRedLP_Com
#define plRedLP_AFR     plRedLP_Com

#define plRedAFOG_SA    plRedAFOG_A
#define plRedAFOG_SPA   plRedAFOG_A
#define plRedAFOG_IO    plRedAFOG_A
#define plRedAFOR_SA    plRedAFOR_A
#define plRedAFOR_SPA   plRedAFOR_A
#define plRedAFOR_IO    plRedAFOR_A

#define plRedSA_RBK     plRedA_RBK
#define plRedIO_RBK     plRedA_RBK
#define plRedSA_SRBK    plRedA_SRBK
#define plRedIO_SRBK    plRedA_SRBK

#define plRedF_ARBK     plRedMF_ARBK

#define plRedCS1_SA     plRedCS1_A
#define plRedCS1_SPA    plRedCS1_A

#define plRedCSI_SA     plRedCSI_A
#define plRedCS1_IO     plRedCS1_A
#define plRedCSI_IO     plRedCSI_A
#define plRedSA_FFR     plRedA_FFR
#define plRedNAM_FFR    plRedA_FFR
#define plRedIO_FFR     plRedA_FFR


#define STRICT
#include <windows.h>
#include "headers.h"
#include "parseline.h"
#define DEFINE_VALUES
#define EXTERN
#include "tokenso.h"
#undef  EXTERN
#undef  DEFINE_VALUES


#define   DIRECT    FALSE           // Flags for PushFcnStrand_YY
#define INDIRECT    TRUE            // ...

#define POPLEFT                     ((lpplOrgLftStk < &lpMemPTD->lpplLftStk[-1]) ? *--lpMemPTD->lpplLftStk : pl_yylex (&plLocalVars))
#define POPRIGHT                    *--lpMemPTD->lpplRhtStk
#define PUSHLEFT(a)                 *lpMemPTD->lpplLftStk++ = (a)
#define PUSHRIGHT(a)                *lpMemPTD->lpplRhtStk++ = (a)
#define LBIND(lftSynObj,curSynObj)  _BIND (lftSynObj, curSynObj)    // (((lftSynObj <= soLAST) && (curSynObj <= soLAST)) ? plBndStr[lftSynObj][curSynObj] : (DbgBrk (), 0xCCC))
#define RBIND(curSynObj,rhtSynObj)  _BIND (curSynObj, rhtSynObj)    // (((curSynObj <= soLAST) && (rhtSynObj <= soLAST)) ? plBndStr[curSynObj][rhtSynObj] : (DbgBrk (), 0xFFF))
#define LFTSYNOBJ                   (lpplOrgLftStk < &lpMemPTD->lpplLftStk[-1]) ? lpMemPTD->lpplLftStk[-1]->tkToken.tkSynObj : GetLftSynObj (&plLocalVars)
#define CURSYNOBJ                    lpplYYCurObj->tkToken.tkSynObj
#define RHTSYNOBJ                    lpMemPTD->lpplRhtStk[-1]->tkToken.tkSynObj
#define RHT2SYNOBJ                  ((RSTACKLEN > 1) ? lpMemPTD->lpplRhtStk[-2]->tkToken.tkSynObj : soNONE)
#define LSTSYNOBJ                    lpplYYLstRht->tkToken.tkSynObj
#define RSTACKLEN                   (lpMemPTD->lpplRhtStk - lpplOrgRhtStk)

#ifdef DEBUG
//#define DEBUG2
#endif

#ifdef DEBUG2
  #define TRACE(a,EVENT,soType,rhtSynObj)                         \
              dprintfWL0 (L"%s %s %3d %-4s %3d %s %s",            \
                          (a),                                    \
                          LSTACK (&plLocalVars, lpplOrgLftStk),   \
                          LBIND (LFTSYNOBJ, CURSYNOBJ),           \
                          soNames[soType],                        \
                          RBIND (CURSYNOBJ, rhtSynObj),           \
                          RSTACK (&plLocalVars, lpplOrgRhtStk),   \
                          EVENT)
  #define TRACE2(a,EVENT,soType,rhtSynObj)                        \
              dprintfWL0 (L"%s %s %3d %-4s %3d %s %s",            \
                          (a),                                    \
                          LSTACK (&plLocalVars, lpplOrgLftStk),   \
                          LBIND (LFTSYNOBJ, soType),             \
                          soNames[soType],                        \
                          RBIND (soType, rhtSynObj),              \
                          RSTACK (&plLocalVars, lpplOrgRhtStk),   \
                          EVENT)
  #define DbgPushVarStrand_YY(a)            _DbgPushVarStrand_YY    (a)
  #define DbgMakeVarStrand_EM_YY(a)         _DbgMakeVarStrand_EM_YY (a)
  #define DbgPushFcnStrand_YY(a,b,c)        _DbgPushFcnStrand_YY    (a,b,c)
  #define DbgMakeFcnStrand_EM_YY(a,b,c)     _DbgMakeFcnStrand_EM_YY (a,b,c)
#else
  #define TRACE(a,EVENT,soType,rhtSynObj)
  #define TRACE2(a,EVENT,soType,rhtSynObj)
  #define DbgPushVarStrand_YY(a)            PushVarStrand_YY    (a)
  #define DbgMakeVarStrand_EM_YY(a)         MakeVarStrand_EM_YY (a)
  #define DbgPushFcnStrand_YY(a,b,c)        PushFcnStrand_YY    (a,b,c)
  #define DbgMakeFcnStrand_EM_YY(a,b,c)     MakeFcnStrand_EM_YY (a,b,c)
#endif


//***************************************************************************
//  $_BIND
//***************************************************************************

#ifdef DEBUG
int _BIND
    (SO_ENUM lftSynObj,
     SO_ENUM rhtSynObj)

{
    if ((lftSynObj <= soLAST) && (rhtSynObj <= soLAST))
        return plBndStr[lftSynObj][rhtSynObj];

    DbgStop ();         // ***FIXME*** -- error in reduction/binding tables

    return -1;
} // End _BIND
#else
  #define   _BIND(lftSynObj,rhtSynObj)      plBndStr[lftSynObj][rhtSynObj]
#endif


//***************************************************************************
//  $plRedNAM_RP
//
//  Reduce "NAM RP"
//***************************************************************************

LPPL_YYSTYPE plRedNAM_RP
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // Set the token type
    lpplYYCurObj->tkToken.tkFlags.TknType = TKT_VARNAMED;

    // Initialize a name strand
    InitNameStrand (lpplYYCurObj);

    // Append this name to the strand
    lpYYRes =
      PushNameStrand_YY (lpplYYCurObj);

    // Check for error
    if (!lpYYRes)
        goto ERROR_EXIT;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;
ERROR_EXIT:
    // YYFree the last right & current objects
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return lpYYRes;
} // End plRedNAM_RP


//***************************************************************************
//  $plRedNAM_NR
//
//  Reduce "NAM NR"
//***************************************************************************

LPPL_YYSTYPE plRedNAM_NR
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // Set the token type
    lpplYYCurObj->tkToken.tkFlags.TknType = TKT_VARNAMED;

    // Append this name to the strand
    lpYYRes =
      PushNameStrand_YY (lpplYYCurObj);

    // Check for error
    if (!lpYYRes)
        goto ERROR_EXIT;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;
ERROR_EXIT:
    // YYFree the last right & current objects
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return lpYYRes;
} // End plRedNAM_NR


//***************************************************************************
//  $plRedLP_NR
//
//  Reduce "LP NR"
//***************************************************************************

LPPL_YYSTYPE plRedLP_NR
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;

    return lpplYYLstRht;
} // End plRedLP_NR


//***************************************************************************
//  $plRedLP_NNR
//
//  Reduce "LP NNR"
//***************************************************************************

LPPL_YYSTYPE plRedLP_NNR
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    lpYYRes =
      MakeNameStrand_EM_YY (lpplYYLstRht);

    // YYFree the last right & current objects
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // Check for error
    if (!lpYYRes)
        goto ERROR_EXIT;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;
ERROR_EXIT:
    return lpYYRes;
} // End plRedLP_NNR


//***************************************************************************
//  $plRedLNR_SPA
//
//  Reduce "LNR SPA"
//***************************************************************************

LPPL_YYSTYPE plRedLNR_SPA
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    UBOOL bRet;                         // TRUE iff the result is valid

    // Assign the names
    bRet = AssignNamedVars_EM (&lpplYYCurObj->tkToken, &lpplYYLstRht->tkToken);

    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // Check for error
    if (!bRet)
        goto ERROR_EXIT;

    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;

    return lpplYYLstRht;
ERROR_EXIT:
    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    return NULL;
} // End plRedLNR_SPA


//***************************************************************************
//  $plRedLBK_RBK
//
//  Reduce "LBK RBK"
//***************************************************************************

LPPL_YYSTYPE plRedLBK_RBK
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes = NULL,        // Ptr to the result
                 lpYYVar;               // ...    temp

    // Initialize an empty list
    lpYYVar =
      InitList0_YY (lpplYYLstRht);
    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    if (!lpYYVar)                       // If not defined, free args and YYERROR
        goto ERROR_EXIT;

    lpYYRes =
      MakeList_EM_YY (lpYYVar, TRUE);
    YYFree (lpYYVar); lpYYVar = NULL;

    if (!lpYYRes)                       // If not defined, free args and YYERROR
        goto ERROR_EXIT;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;
ERROR_EXIT:
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return lpYYRes;
} // End plRedLBK_RBK


//***************************************************************************
//  $plRedNF_RBK
//
//  Reduce "NF RBK"
//***************************************************************************

LPPL_YYSTYPE plRedNF_RBK
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Execute the niladic function returning an array
    lpplYYCurObj =
      plExecuteFn0 (lpplYYCurObj);

    // Check for error
    if (!lpplYYCurObj)
        goto ERROR_EXIT;

    // Call common code
    return
      plRedA_RBK (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
ERROR_EXIT:
    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    return NULL;
} // End plRedNF_RBK


//***************************************************************************
//  $plRedNF_SRBK
//
//  Reduce "NF SRBK"
//***************************************************************************

LPPL_YYSTYPE plRedNF_SRBK
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Execute the niladic function returning an array
    lpplYYCurObj =
      plExecuteFn0 (lpplYYCurObj);

    // Check for error
    if (!lpplYYCurObj)
        goto ERROR_EXIT;

    // Call common code
    return
      plRedA_SRBK (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
ERROR_EXIT:
    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    return NULL;
} // End plRedNF_SRBK


//***************************************************************************
//  $plRedA_RBK
//
//  Reduce "A RBK"
//***************************************************************************

LPPL_YYSTYPE plRedA_RBK
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes,               // Ptr to the result
                 lpYYVar;               // Ptr to a temp

    // If the current object is in the process of stranding, ...
    if (lpplYYCurObj->YYStranding)
    {
        // Turn this strand into a var
        lpYYVar =
          DbgMakeVarStrand_EM_YY (lpplYYCurObj);
        // YYFree the current object
        YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

        // If not defined, ...
        if (!lpYYVar)
            goto ERROR_EXIT;

        // Copy to the current object
        lpplYYCurObj = lpYYVar; // curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
        lpYYVar = NULL;
    } else
        // Unstrand the current object if necessary
        UnStrand (lpplYYCurObj);

    // Initialize a list with the arg
    lpYYRes =
      InitList1_YY (lpplYYCurObj, lpplYYCurObj);
////FreeResult (lpplYYCurObj);              // Copied w/o IncrRefCnt in PushList_YY

    // YYFree the last right & current objects
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // If not defined, ...
    if (!lpYYRes)
        goto ERROR_EXIT;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;
ERROR_EXIT:
    return lpYYRes;
} // End plRedA_RBK


//***************************************************************************
//  $plRedA_SRBK
//
//  Reduce "A SRBK"
//***************************************************************************

LPPL_YYSTYPE plRedA_SRBK
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes = NULL,        // Ptr to the result
                 lpYYVar;               // Ptr to a temp

    // If the current object is in the process of stranding, ...
    if (lpplYYCurObj->YYStranding)
    {
        // Turn this strand into a var
        lpYYVar =
          DbgMakeVarStrand_EM_YY (lpplYYCurObj);
        // YYFree the current object
        YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

        // If not defined, ...
        if (!lpYYVar)
            goto ERROR_EXIT;

        // Copy to the current object
        lpplYYCurObj = lpYYVar; // curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
        lpYYVar = NULL;
    } else
        // Unstrand the current object if necessary
        UnStrand (lpplYYCurObj);

    // Push an item onto the list
    lpYYRes =
      PushList_YY (lpplYYLstRht, lpplYYCurObj, lpplYYLstRht);
////FreeResult (lpplYYCurObj);              // Copied w/o IncrRefCnt in PushList_YY

    // YYFree the last right & current objects
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // If not defined, ...
    if (!lpYYRes)
        goto ERROR_EXIT;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;
ERROR_EXIT:
    return lpYYRes;
} // End plRedA_SRBK


//***************************************************************************
//  $plRedLBK_ARBK
//
//  Reduce "LBK ARBK"
//***************************************************************************

LPPL_YYSTYPE plRedLBK_ARBK
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    lpYYRes =
      MakeList_EM_YY (lpplYYLstRht, TRUE);

    // YYFree the last right & current objects
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // If not defined, ...
    if (!lpYYRes)
        goto ERROR_EXIT;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;

    // Split cases based upon the token type
    switch (lpYYRes->tkToken.tkFlags.TknType)
    {
        case TKT_LSTIMMED:
            lpYYRes->tkToken.tkFlags.TknType = TKT_VARIMMED;

            break;

        case TKT_LSTARRAY:
            lpYYRes->tkToken.tkFlags.TknType = TKT_VARARRAY;

            break;

        case TKT_LSTMULT:
            break;

        defstop
            break;
    } // End SWITCH
ERROR_EXIT:
    return lpYYRes;
} // End plRedLBK_ARBK


//***************************************************************************
//  $plRedLBK_SRBK
//
//  Reduce "LBK SRBK"
//***************************************************************************

LPPL_YYSTYPE plRedLBK_SRBK
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // Push an empty item onto the list
    lpYYRes =
      PushList_YY (lpplYYLstRht, NULL, lpplYYLstRht);

    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    return plRedLBK_ARBK (lpplLocalVars, lpplYYCurObj, lpYYRes, soType);
} // End plRedLBK_SRBK


//***************************************************************************
//  $plRedSEMI_RBK
//
//  Reduce "SEMI RBK"
//***************************************************************************

LPPL_YYSTYPE plRedSEMI_RBK
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // Initialize a list with an empty item
    lpYYRes =
      InitList1_YY (NULL, lpplYYCurObj);
////FreeResult (lpplYYCurObj);              // Copied w/o IncrRefCnt in PushList_YY

    // ***FIXME*** -- Do we need PushList (NULL) ???

    // If it succeeded, ...
    if (lpYYRes)
        // Change the tkSynObj
        lpYYRes->tkToken.tkSynObj = soType;

    // YYFree the last right & current objects
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return lpYYRes;
} // End plRedSEMI_RBK


//***************************************************************************
//  $plRedSEMI_ARBK
//
//  Reduce "SEMI ARBK"
//***************************************************************************

LPPL_YYSTYPE plRedSEMI_ARBK
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;

    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return lpplYYLstRht;
} // End plRedSEMI_ARBK


//***************************************************************************
//  $plRedSEMI_SRBK
//
//  Reduce "SEMI SRBK"
//***************************************************************************

LPPL_YYSTYPE plRedSEMI_SRBK
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // Push an empty item onto the list
    lpYYRes =
      PushList_YY (lpplYYLstRht, NULL, lpplYYLstRht);

    // YYFree the last right & current objects
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // If not defined, ...
    if (!lpYYRes)
        goto ERROR_EXIT;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;
ERROR_EXIT:
    return lpYYRes;
} // End plRedSEMI_SRBK


//***************************************************************************
//  $plRedA_A
//
//  Reduce "A A"
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- plRedA_A"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE plRedA_A
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // Ensure the current object has a value
    if (IsTknNamed (&lpplYYCurObj->tkToken)
     && !lpplYYCurObj->tkToken.tkData.tkSym->stFlags.Value)
        goto VALUE_CUR_EXIT;

    // Ensure the last right object has a value
    if (IsTknNamed (&lpplYYLstRht->tkToken)
     && !lpplYYLstRht->tkToken.tkData.tkSym->stFlags.Value)
        goto VALUE_LST_EXIT;

    Assert (!lpplYYCurObj->YYStranding);
    Assert (!lpplYYLstRht->YYStranding);

    // Prepare for stranding
    InitVarStrand (lpplYYCurObj);

    // Append the current object to the strand
    lpYYRes =
      DbgPushVarStrand_YY (lpplYYCurObj);

    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // If not defined, ...
    if (!lpYYRes)
        goto ERROR_EXIT;

    // Append the last right object to the strand
    return plRedSA_A (lpplLocalVars, lpYYRes, lpplYYLstRht, soType);

VALUE_CUR_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
                              &lpplYYCurObj->tkToken);
    goto ERROR_EXIT;

VALUE_LST_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
                              &lpplYYLstRht->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    // YYFree the current & last right objects
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    return NULL;
} // End plRedA_A
#undef  APPEND_NAME


//***************************************************************************
//  $plRedSA_A
//
//  Reduce "SA A"
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- plRedSA_A"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE plRedSA_A
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // Ensure the last right object has a value
    if (IsTknNamed (&lpplYYLstRht->tkToken)
     && !lpplYYLstRht->tkToken.tkData.tkSym->stFlags.Value)
        goto VALUE_LST_EXIT;

    Assert ( lpplYYCurObj->YYStranding);
    Assert (!lpplYYLstRht->YYStranding);

    // Append the last right object to the strand
    lpYYRes =
      DbgPushVarStrand_YY (lpplYYLstRht);

    // If not defined, ...
    if (!lpYYRes)
        goto ERROR_EXIT;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;

    goto NORMAL_EXIT;

VALUE_LST_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
                              &lpplYYLstRht->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    // Mark as in error
    lpYYRes = NULL;
NORMAL_EXIT:
    // YYFree the last right & current objects
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return lpYYRes;
} // End plRedSA_A
#undef  APPEND_NAME


//***************************************************************************
//  $plRedA_F
//
//  Reduce "A F"  &  "SA F"
//***************************************************************************

LPPL_YYSTYPE plRedA_F
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYVar;               // Ptr to a temp

    Assert (lpplLocalVars->lpplYYArgCurry EQ NULL);

    // If the current object is in the process of stranding, ...
    if (lpplYYCurObj->YYStranding)
    {
        // Turn this strand into a var
        lpYYVar =
          DbgMakeVarStrand_EM_YY (lpplYYCurObj);
        // YYFree the current object
        YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

        // If not defined, ...
        if (!lpYYVar)
            goto ERROR_EXIT;

        // Copy to the current object
        lpplYYCurObj = lpYYVar; // curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
        lpYYVar = NULL;
    } else
        // Unstrand the current object if necessary
        UnStrand (lpplYYCurObj);

    // Copy to the left curry object
    lpplLocalVars->lpplYYArgCurry = lpplYYCurObj;

    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;

////YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;  // Do *NOT* free as it is still curried

    return lpplYYLstRht;
ERROR_EXIT:
    return NULL;
} // End plRedA_F


//***************************************************************************
//  $plRedA_HY
//
//  Reduce "A HY"
//***************************************************************************

LPPL_YYSTYPE plRedA_HY
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    Assert (lpplYYLstRht->tkToken.tkFlags.TknType EQ TKT_OP3IMMED);

    // Convert the token type of the last right object to an immediate function
    lpplYYLstRht->tkToken.tkFlags.TknType = TKT_FCNIMMED;
    lpplYYLstRht->tkToken.tkFlags.ImmType = IMMTYPE_PRIMFCN;
    lpplYYLstRht->tkToken.tkSynObj        = soF;

    // If the last right object is in the process of stranding, ...
    if (lpplYYLstRht->YYStranding)
    {
        Assert (lpplYYLstRht->lpYYFcnBase->tkToken.tkFlags.TknType EQ TKT_OP3IMMED);

        // Convert the token type of the last right object to an immediate function
        lpplYYLstRht->lpYYFcnBase->tkToken.tkFlags.TknType = TKT_FCNIMMED;
        lpplYYLstRht->lpYYFcnBase->tkToken.tkFlags.ImmType = IMMTYPE_PRIMFCN;
        lpplYYLstRht->lpYYFcnBase->tkToken.tkSynObj        = soF;
    } // End IF

    return plRedA_F (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
} // End plRedA_HY


//***************************************************************************
//  $plRedA_NF
//
//  Reduce "A NF"
//***************************************************************************

LPPL_YYSTYPE plRedA_NF
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    Assert (!lpplYYCurObj->YYStranding);

    // Execute the niladic function returning an array
    lpplYYLstRht =
      plExecuteFn0 (lpplYYLstRht);

    // Check for error
    if (!lpplYYLstRht)
        goto ERROR_EXIT;

    // Call common code
    return
      plRedA_A  (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
ERROR_EXIT:
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return NULL;
} // End plRedA_NF


//***************************************************************************
//  $plRedSA_NF
//
//  Reduce "SA NF"
//***************************************************************************

LPPL_YYSTYPE plRedSA_NF
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    Assert (lpplYYCurObj->YYStranding);

    // Execute the niladic function returning an array
    lpplYYLstRht =
      plExecuteFn0 (lpplYYLstRht);

    // Check for error
    if (!lpplYYLstRht)
        goto ERROR_EXIT;

    return
      plRedSA_A (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
ERROR_EXIT:
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return NULL;
} // End plRedSA_NF


//***************************************************************************
//  $plRedSYSN_F
//
//  Reduce "SYSN F"
//***************************************************************************

LPPL_YYSTYPE plRedSYSN_F
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Copy the system namespace level
    lpplYYLstRht->tkToken.tkFlags.SysNSLvl = lpplYYCurObj->tkToken.tkFlags.SysNSLvl;

    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;

    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return lpplYYLstRht;
} // End plRedSYSN_F


//***************************************************************************
//  $plRedDOP_RhtOper
//
//  Reduce "DOP RhtOper"
//***************************************************************************

LPPL_YYSTYPE plRedDOP_RhtOper
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes,               // Ptr to the result
                 lpYYRht,               // Ptr to a temp
                 lpYYVar;               // ...

    // The result is always the root of the function tree
    lpYYRes =
      DbgPushFcnStrand_YY (lpplYYCurObj,
                           2,
                           (IsFcnStrDirect (lpplYYCurObj)) ? DIRECT
                                                           : INDIRECT); // Dyadic operator
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    if (!lpYYRes)                           // If not defined, free args and YYERROR
    {
        // Free and YYFree the last right object
        FreeResNNU1 (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

        goto ERROR_EXIT;
    } // End IF

    // If the last right object is a var in the process of stranding, ...
    if (IsTknTypeVar (lpplYYLstRht->tkToken.tkFlags.TknType)
     && lpplYYLstRht->YYStranding)
    {
        // Turn this strand into a var
        lpYYVar =
          DbgMakeVarStrand_EM_YY (lpplYYLstRht);
        // YYFree the last right object
        YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

        // If not defined, ...
        if (!lpYYVar)
            goto ERROR_EXIT;

        // Copy to the last right object
        lpplYYLstRht = lpYYVar; // curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
        lpYYVar = NULL;
    } else
        // Unstrand the last right object if necessary
        UnStrand (lpplYYLstRht);

    // Append the right operand to the function strand
    lpYYRht =
      DbgPushFcnStrand_YY (lpplYYLstRht,
                           1,
                           (IsFcnStrDirect (lpplYYLstRht)) ? DIRECT
                                                           : INDIRECT); // Right operand
    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    if (!lpYYRht)                       // If not defined, free args and YYERROR
    {
        // Free the function (including YYFree)
        YYFreeArray (lpYYRes); FreeYYFcn1 (lpYYRes); lpYYRes = NULL;

        goto ERROR_EXIT;
    } // End IF

    // YYFree the right operand object
    YYFree (lpYYRht); lpYYRht = NULL;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;
ERROR_EXIT:
    return lpYYRes;
} // End plRedDOP_RhtOper


//***************************************************************************
//  $plRedDOP_HY
//
//  Reduce "DOP HY"
//***************************************************************************

LPPL_YYSTYPE plRedDOP_HY
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    Assert (lpplYYLstRht->tkToken.tkFlags.TknType EQ TKT_OP3IMMED);

    // Convert the token type of the last right object to an immediate function
    lpplYYLstRht->tkToken.tkFlags.TknType = TKT_FCNIMMED;
    lpplYYLstRht->tkToken.tkFlags.ImmType = IMMTYPE_PRIMFCN;
    lpplYYLstRht->tkToken.tkSynObj        = soF;

    return plRedDOP_RhtOper (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
} // End plRedDOP_HY


//***************************************************************************
//  $plRedJ_HY
//
//  Reduce "J HY"
//***************************************************************************

LPPL_YYSTYPE plRedJ_HY
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    Assert (lpplYYLstRht->tkToken.tkFlags.TknType EQ TKT_OP3IMMED);

    // Convert the token type of the last right object to an immediate function
    lpplYYLstRht->tkToken.tkFlags.TknType = TKT_FCNIMMED;
    lpplYYLstRht->tkToken.tkFlags.ImmType = IMMTYPE_PRIMFCN;
    lpplYYLstRht->tkToken.tkSynObj        = soF;

    return plRedLftOper_MOP (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
} // End plRedJ_HY


//***************************************************************************
//  $plRedGO_A
//
//  Reduce "GO A"  &  "GO SA"
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- plRedGO_A"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE plRedGO_A
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes = NULL,        // Ptr to the result
                 lpYYVar;               // Ptr to a temp
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory

    // If the last right object is in the process of stranding, ...
    if (lpplYYLstRht->YYStranding)
    {
        // Turn this strand into a var
        lpYYVar =
          DbgMakeVarStrand_EM_YY (lpplYYLstRht);
        // YYFree the last right object
        YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

        // If not defined, ...
        if (!lpYYVar)
            goto ERROR_EXIT;

        // Copy to the last right object
        lpplYYLstRht = lpYYVar; // lstSynObj = LSTSYNOBJ; Assert (lstSynObj <= soLAST);
        lpYYVar = NULL;
    } else
        // Unstrand the last right object if necessary
        UnStrand (lpplYYLstRht);

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
        goto ERROR_EXIT;
    else
    {
        lpplLocalVars->ExitType = GotoLine_EM (&lpplYYLstRht->tkToken, &lpplYYCurObj->tkToken);

        // Get ptr to PerTabData global memory
        lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

        // Make a PL_YYSTYPE NoValue entry
        lpYYVar =
          MakeNoValue_YY (&lpplYYLstRht->tkToken);
        lpMemPTD->YYResExec = *lpYYVar;
        YYFree (lpYYVar); lpYYVar = NULL;

        // Split cases based upon the Exit Type
        switch (lpplLocalVars->ExitType)
        {
            case EXITTYPE_GOTO_ZILDE:  // {zilde}
                // Make a NoValue entry as the result
                lpYYRes = MakeNoValue_YY (&lpplYYCurObj->tkToken);

                break;

            case EXITTYPE_GOTO_LINE:   // Valid line #
                // If we're not at a LABELSEP, EOS or EOL, YYERROR
                if (lpplLocalVars->lptkNext[-1].tkFlags.TknType NE TKT_LABELSEP
                 && lpplLocalVars->lptkNext[-1].tkFlags.TknType NE TKT_EOS
                 && lpplLocalVars->lptkNext[-1].tkFlags.TknType NE TKT_EOL)
                {
                    PrimFnSyntaxError_EM (&lpplYYCurObj->tkToken APPEND_NAME_ARG);

                    goto ERROR_EXIT;
                } else
                {
                    // Tell the caller to stop executing this line
                    lpplLocalVars->bStopExec = TRUE;

                    break;
                } //End IF

            case EXITTYPE_ERROR:       // Error
            case EXITTYPE_STACK_FULL:  // ...
                goto ERROR_EXIT;

            defstop
                break;
        } // End SWITCH
    } // End IF/ELSE

/// // Change the tkSynObj
/// lpYYRes->tkToken.tkSynObj = soType;

    goto NORMAL_EXIT;

ERROR_EXIT:
    // Mark as in error
    lpplLocalVars->ExitType = EXITTYPE_ERROR;
NORMAL_EXIT:
    // Free (unnamed) and YYFree the last right object
    FreeTempResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL;

    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return lpYYRes;
} // End plRedGO_A
#undef  APPEND_NAME


//***************************************************************************
//  $plRedGO_NF
//
//  Reduce "GO NF"
//***************************************************************************

LPPL_YYSTYPE plRedGO_NF
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Execute the niladic function returning an array
    lpplYYLstRht =
      plExecuteFn0 (lpplYYLstRht);

    // Check for error
    if (!lpplYYLstRht)
        goto ERROR_EXIT;

    // Call common code
    return
      plRedGO_A (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
ERROR_EXIT:
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return NULL;
} // End plRedGO_NF


//***************************************************************************
//  $plRedA_FFR
//
//  Reduce "A FFR" & "SA FFR"
//***************************************************************************

LPPL_YYSTYPE plRedA_FFR
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes = NULL,        // Ptr to the result
                 lpYYVar;               // Ptr to a temp

    // If the curent object is in the process of stranding, ...
    if (lpplYYCurObj->YYStranding)
    {
        // Turn this strand into a var
        lpYYVar =
          DbgMakeVarStrand_EM_YY (lpplYYCurObj);
        // YYFree the current object
        YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

        // If not defined, ...
        if (!lpYYVar)
            goto ERROR_EXIT;

        // Copy to the current object
        lpplYYCurObj = lpYYVar; // curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
        lpYYVar = NULL;
    } else
        // Unstrand the current object if necessary
        UnStrand (lpplYYCurObj);

    // Append the current object to the function strand
    lpYYVar =
      DbgPushFcnStrand_YY (lpplYYCurObj,
                           1,
                           (IsFcnStrDirect (lpplYYCurObj)) ? DIRECT
                                                           : INDIRECT); // Left operand
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // If not defined, ...
    if (!lpYYVar)
        goto ERROR_EXIT;

    // YYFree the left operand object
    YYFree (lpYYVar); lpYYVar = NULL;

    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;
ERROR_EXIT:
    return lpplYYLstRht;
} // End plRedA_FFR


//***************************************************************************
//  $plRedCSI_A
//
//  Reduce "CSI A"  &  "CSI SA"
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- plRedCSI_A"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE plRedCSI_A
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYVar;               // Ptr to a temp

    // If the last right object is in the process of stranding, ...
    if (lpplYYLstRht->YYStranding)
    {
        // Turn this strand into a var
        lpYYVar =
          DbgMakeVarStrand_EM_YY (lpplYYLstRht);
        // YYFree the last right object
        YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

        // If not defined, ...
        if (!lpYYVar)
            goto ERROR_EXIT;

        // Copy to the last right object
        lpplYYLstRht = lpYYVar; // lstSynObj = LSTSYNOBJ; Assert (lstSynObj <= soLAST);
        lpYYVar = NULL;
    } else
        // Unstrand the last right object if necessary
        UnStrand (lpplYYLstRht);

    // Check for Ctrl Strucs in AFO
    if (lpplLocalVars->bAfoCtrlStruc)  // FOR
        goto SYNTAX_EXIT;

    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;

    goto NORMAL_EXIT;

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpplYYCurObj->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    // If the last right object is defined, ...
    if (lpplYYLstRht)
    {
        // Free (unnamed) and YYFree the last right object
        FreeTempResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // curSynObj = soNONE;
    } // End IF
NORMAL_EXIT:
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return lpplYYLstRht;
} // End plRedCSI_A
#undef  APPEND_NAME


//***************************************************************************
//  $plRedCSF_NAM
//
//  Reduce "CSF NAM"
//***************************************************************************

LPPL_YYSTYPE plRedCSF_NAM
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Change the tkSynObj
    lpplYYCurObj->tkToken.tkSynObj = soType;

    Assert (lpMemPTD->ForToken.tkToken.tkFlags.TknType EQ TKT_UNUSED);
    Assert (lpMemPTD->ForName .tkToken.tkFlags.TknType EQ TKT_UNUSED);

    // Save the :FOR token and the NAM token
    lpMemPTD->ForToken = *lpplYYCurObj;
    lpMemPTD->ForName  = *lpplYYLstRht;

    // Free the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    return lpplYYCurObj;
} // End plRedCSF_NAM


//***************************************************************************
//  $plRedCSFN_CSIA
//
//  Reduce "CSFN CSIA"
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- plRedCSFN_CSIA"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE plRedCSFN_CSIA
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYRes = NULL;        // Ptr to the result

    // Get ptr to PerTabData global memory
    lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Check for Ctrl Strucs in AFO
    if (lpplLocalVars->bAfoCtrlStruc)
        goto SYNTAX_EXIT;

    // Handle FOR statement
    if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
        lpplLocalVars->bRet = FALSE;
    else
    {
        lpplLocalVars->bRet =
          CS_FOR_Stmt_EM (lpplLocalVars, &lpMemPTD->ForToken, &lpMemPTD->ForName, lpplYYLstRht);

        if (lpplLocalVars->bRet)
            // No return value needed
            lpYYRes = MakeNoValue_YY (&lpplYYCurObj->tkToken);
    } // End IF/ELSE

    //                    YYFree the current object
                                   YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;
    // Free (unnamed) and YYFree the last right object
    FreeTempResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    ZeroMemory (&lpMemPTD->ForToken, sizeof (lpMemPTD->ForToken));
    ZeroMemory (&lpMemPTD->ForName , sizeof (lpMemPTD->ForName ));

    if (!lpplLocalVars->bRet)
        // Mark as in error
        lpplLocalVars->ExitType = EXITTYPE_ERROR;

    return lpYYRes;

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lpplLocalVars->lptkNext);
    return NULL;
} // End plRedCSFN_CSIA
#undef  APPEND_NAME


//***************************************************************************
//  $plRedA_HR
//
//  Reduce "A HR"
//***************************************************************************

LPPL_YYSTYPE plRedA_HR
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    Assert (lpplYYLstRht->tkToken.tkFlags.TknType EQ TKT_OP3IMMED);

    // Convert the token type of the last right object to an immediate function
    lpplYYLstRht->tkToken.tkFlags.TknType = TKT_FCNIMMED;
    lpplYYLstRht->tkToken.tkFlags.ImmType = IMMTYPE_PRIMFCN;
    lpplYYLstRht->tkToken.tkSynObj        = soF;

    return plRedA_F (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
} // End plRedA_HR


//***************************************************************************
//  $plRedA_IDX
//
//  Reduce "A IDX"  &  "SA IDX"
//***************************************************************************

LPPL_YYSTYPE plRedA_IDX
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes = NULL,        // Ptr to the result
                 lpYYVar;               // Ptr to a temp

    // If the current object is in the process of stranding, ...
    if (lpplYYCurObj->YYStranding)
    {
        // Turn this strand into a var
        lpYYVar =
          DbgMakeVarStrand_EM_YY (lpplYYCurObj);
        // YYFree the current object
        YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

        // If not defined, ...
        if (!lpYYVar)
            goto ERROR_EXIT;

        // Copy to the current object
        lpplYYCurObj = lpYYVar; // curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
        lpYYVar = NULL;
    } else
        // Unstrand the current object if necessary
        UnStrand (lpplYYCurObj);

    if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
        lpYYRes = NULL;
    else
        lpYYRes = ArrayIndexRef_EM_YY (&lpplYYCurObj->tkToken, &lpplYYLstRht->tkToken);

    // Free (unnamed) and YYFree the current & last right objects
    FreeTempResult (lpplYYCurObj); YYFree (lpplYYCurObj); lpplYYCurObj = NULL;
    FreeTempResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL;

    // If it succeeded, ...
    if (lpYYRes)
        // Change the tkSynObj
        lpYYRes->tkToken.tkSynObj = soType;
ERROR_EXIT:
    return lpYYRes;
} // End plRedA_IDX


//***************************************************************************
//  $plRedMF_A
//
//  Reduce "MF A"  &  "MF SA"
//***************************************************************************

LPPL_YYSTYPE plRedMF_A
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPTOKEN      lptkLftArg;            // Ptr to left arg token
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // If the left arg is present, ...
    if (lpplLocalVars->lpplYYArgCurry)
    {
        // Set the left arg token ptr
        lptkLftArg = &lpplLocalVars->lpplYYArgCurry->tkToken;

        // If the left arg is a named var, ...
        if (IsTknNamedVar (lptkLftArg))
            // Increment the refcnt
            IncrRefCntTkn (lptkLftArg);
    } else
        // Set the left arg token ptr
        lptkLftArg = NULL;

    // If this function is in the process of being stranded, ...
    if (lpplYYCurObj->YYStranding)
    {
        // Turn this function strand into a function
        lpYYRes =
          DbgMakeFcnStrand_EM_YY (lpplYYCurObj, NAMETYPE_FN12, FALSE);
        // YYFree the current object
        YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

        // If not defined, ...
        if (!lpYYRes)
            goto ERROR_EXIT;

        // Copy to the current object
        lpplYYCurObj = lpYYRes; // curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
        lpYYRes = NULL;
    } else
    {
        // If this is a named non-direct fcn/opr or a DEL, ...
        if ((IsTknNamedFcnOpr (&lpplYYCurObj->tkToken)
          && !lpplYYCurObj->tkToken.tkData.tkSym->stFlags.FcnDir)
         || lpplYYCurObj->tkToken.tkFlags.TknType EQ TKT_DELAFO)
            // Increment the refcnt
            IncrRefCntTkn (&lpplYYCurObj->tkToken);
    } // End IF/ELSE

    // If the last right object is in the process of stranding, ...
    if (lpplYYLstRht->YYStranding)
    {
        // Turn this strand into a var
        lpYYRes =
          DbgMakeVarStrand_EM_YY (lpplYYLstRht);
        // YYFree the last right object
        YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

        // If not defined, ...
        if (!lpYYRes)
            goto ERROR_EXIT;

        // Copy to the last right object
        lpplYYLstRht = lpYYRes; // lstSynObj = LSTSYNOBJ; Assert (lstSynObj <= soLAST);
        lpYYRes = NULL;
    } else
    {
        // Unstrand the last right object if necessary
        UnStrand (lpplYYLstRht);

        // If this is a named var, ...
        if (IsTknNamedVar (&lpplYYLstRht->tkToken))
            // Increment the refcnt
            IncrRefCntTkn (&lpplYYLstRht->tkToken);
    } // End IF/ELSE

    Assert (!lpplYYCurObj->YYStranding && !lpplYYLstRht->YYStranding);

    // Execute the function between the curried left arg and the last right arg
    lpYYRes =
      ExecFunc_EM_YY (lptkLftArg, lpplYYCurObj, &lpplYYLstRht->tkToken);

    // If the left arg is present, ...
    if (lpplLocalVars->lpplYYArgCurry)
    {
        // Free the left arg
        FreeResult (lpplLocalVars->lpplYYArgCurry);

        // YYFree the curried object
        YYFree (lpplLocalVars->lpplYYArgCurry); lpplLocalVars->lpplYYArgCurry = NULL;
    } // End IF

    // Free YYCopyArray'ed elements
    YYFreeArray (lpplYYCurObj);

    // Free the function (including YYFree)
    FreeYYFcn1  (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // Free and YYFree the last right arg
    FreeResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    // If not defined, ...
    if (!lpYYRes)
        goto ERROR_EXIT;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;

    // Copy to the current object
    lpplYYCurObj = lpYYRes; // curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);

    return lpplYYCurObj;

ERROR_EXIT:
    if (lpplYYCurObj)
    {
        // YYFree the current object
        YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;
    } // End IF

    return NULL;
} // End plRedMF_A


//***************************************************************************
//  $plRedMF_ARBK
//
//  Reduce "MF ARBK"
//***************************************************************************

LPPL_YYSTYPE plRedMF_ARBK
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes,               // Ptr to the result
                 lpYYVar1,              // Ptr to temp
                 lpYYVar2;              // ...

    // If this function is in the process of being stranded, ...
    if (lpplYYCurObj->YYStranding)
    {
        // Turn this function strand into a function
        lpYYRes =
          DbgMakeFcnStrand_EM_YY (lpplYYCurObj, NAMETYPE_FN12, FALSE);
        // YYFree the current object
        YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

        // If not defined, ...
        if (!lpYYRes)
            goto ERROR_EXIT;

        // Copy to the current object
        lpplYYCurObj = lpYYRes; // curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
        lpYYRes = NULL;
    } // End IF

    // Pop the most recent item from the list
    //   returning the item and shortening the list in <lpplYYLstRht>
    lpYYVar1 =
      PopList_YY (lpplYYLstRht);

    // Check for error
    if (!lpYYVar1)
        goto ERROR_EXIT;

    // Execute the function on the var
    lpYYVar2 =
      plRedMF_A (lpplLocalVars, lpplYYCurObj, lpYYVar1, soA);

    // Check for error
    if (!lpYYVar2)
        goto ERROR_EXIT;

    // Push the result onto the list
    lpYYRes =
      PushList_YY (lpplYYLstRht, lpYYVar2, lpplYYLstRht);

    // YYFree the temp
    YYFree (lpYYVar2); lpYYVar2 = NULL;

    // Check for error
    if (!lpYYRes)
        goto ERROR_EXIT;

    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;

    return lpYYRes;
ERROR_EXIT:
    if (lpplYYCurObj)
    {
        // YYFree the current object
        YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;
    } // End IF

    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    return NULL;
} // End plRedMF_ARBK


//***************************************************************************
//  $plRedCS1_A
//
//  Reduce "CS1 A"
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- plRedCS1_A"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE plRedCS1_A
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYVar;               // Ptr to a temp
    TOKEN        tkToken;               // The incoming token

    // Save the token type
    tkToken = lpplYYCurObj->tkToken;

    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // If the last right object is in the process of stranding, ...
    if (lpplYYLstRht->YYStranding)
    {
        // Turn this strand into a var
        lpYYVar =
          DbgMakeVarStrand_EM_YY (lpplYYLstRht);
        // YYFree the last right object
        YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

        // If not defined, ...
        if (!lpYYVar)
            goto ERROR_EXIT;

        // Copy to the last right object
        lpplYYLstRht = lpYYVar; // lstSynObj = LSTSYNOBJ; Assert (lstSynObj <= soLAST);
        lpYYVar = NULL;
    } else
        // Unstrand the last right object if necessary
        UnStrand (lpplYYLstRht);

    // Split cases based upon the token type
    switch (tkToken.tkFlags.TknType)
    {
        case TKT_CS_ANDIF:
        case TKT_CS_ELSEIF:
        case TKT_CS_IF:
        case TKT_CS_ORIF:
        case TKT_CS_UNTIL:
        case TKT_CS_WHILE:
            // Check for Ctrl Strucs in AFO
            if (lpplLocalVars->bAfoCtrlStruc)
                goto SYNTAX_EXIT;

            // Handle ANDIF/ELSEIF/IF/ORIF/UNTIL/WHILE statements
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_IF_Stmt_EM (lpplLocalVars, &tkToken, lpplYYLstRht);
            // Free (unnamed) and YYFree the last right object
            FreeTempResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

            if (lpplLocalVars->bRet)
                // No return value needed
                return MakeNoValue_YY (&tkToken);

            // Mark as in error
            lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed

            break;

        case TKT_CS_ASSERT:
            // Handle ASSERT statement
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_ASSERT_Stmt_EM (lpplLocalVars, lpplYYLstRht);
            // Free (unnamed) and YYFree the last right object
            FreeTempResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

            if (lpplLocalVars->bRet)
                // No return value needed
                return MakeNoValue_YY (&tkToken);

            // Mark as in error
            lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed
            goto ERROR_EXIT;    // Not SYNTAX ERROR as CS_ASSERT_Stmt_EM already set the error msg

        case TKT_CS_CASE:
        case TKT_CS_CASELIST:
            // Handle CASE/CASELIST statement
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_CASE_Stmt (lpplLocalVars, lpplYYLstRht);
            // Free (unnamed) and YYFree the last right object
            FreeTempResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

            if (lpplLocalVars->bRet)
                // No return value needed
                return MakeNoValue_YY (&tkToken);

            // Mark as in error
            lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed

            break;

        case TKT_CS_CONTINUEIF:
            // Handle CONTINUEIF statement
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_CONTINUEIF_Stmt_EM (lpplLocalVars, &tkToken, lpplYYLstRht);
            // Free (unnamed) and YYFree the last right object
            FreeTempResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

            if (lpplLocalVars->bRet)
                // No return value needed
                return MakeNoValue_YY (&tkToken);

            // Mark as in error
            lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed
            goto ERROR_EXIT;    // Not SYNTAX ERROR as CS_ASSERT_Stmt_EM already set the error msg

        case TKT_CS_GOTO:
            // Check for Ctrl Strucs in AFO
            if (lpplLocalVars->bAfoCtrlStruc)
                goto SYNTAX_EXIT;

            return plRedGO_A (lpplLocalVars, NULL, lpplYYLstRht, soType);

        case TKT_CS_LEAVEIF:
            // Handle LEAVEIF statement
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_LEAVEIF_Stmt_EM (lpplLocalVars, &tkToken, lpplYYLstRht);
            // Free (unnamed) and YYFree the last right object
            FreeTempResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

            if (lpplLocalVars->bRet)
                // No return value needed
                return MakeNoValue_YY (&tkToken);

            // Mark as in error
            lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed
            goto ERROR_EXIT;    // Not SYNTAX ERROR as CS_ASSERT_Stmt_EM already set the error msg

        case TKT_CS_SELECT:
            // Check for Ctrl Strucs in AFO
            if (lpplLocalVars->bAfoCtrlStruc)
                goto SYNTAX_EXIT;

            // Handle SELECT statement
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_SELECT_Stmt_EM (lpplLocalVars, &tkToken, lpplYYLstRht);
            // Free (unnamed) and YYFree the last right object
            FreeTempResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

            if (lpplLocalVars->bRet)
                // No return value needed
                return MakeNoValue_YY (&tkToken);

            // Mark as in error
            lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed
            goto ERROR_EXIT;    // Not SYNTAX ERROR as CS_ASSERT_Stmt_EM already set the error msg

        defstop
            break;
    } // End SWITCH

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lpplLocalVars->lptkNext);
    goto ERROR_EXIT;

ERROR_EXIT:
    return NULL;
} // End plRedCS1_A
#undef  APPEND_NAME


//***************************************************************************
//  $plRedMF_NF
//
//  Reduce "MF NF"
//***************************************************************************

LPPL_YYSTYPE plRedMF_NF
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // If this function is in the process of being stranded, ...
    if (lpplYYCurObj->YYStranding)
    {
        // Turn this function strand into a function
        lpYYRes =
          DbgMakeFcnStrand_EM_YY (lpplYYCurObj, NAMETYPE_FN12, FALSE);
        // YYFree the current object
        YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

        // If not defined, ...
        if (!lpYYRes)
            goto ERROR_EXIT;

        // Copy to the current object
        lpplYYCurObj = lpYYRes; // curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
        lpYYRes = NULL;
    } // End IF

    // Execute the niladic function returning an array
    lpplYYLstRht =
      plExecuteFn0 (lpplYYLstRht);

    // Check for error
    if (!lpplYYLstRht)
        goto ERROR_EXIT;

    // Call common code
    return
      plRedMF_A (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
ERROR_EXIT:
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return NULL;
} // End plRedMF_NF


//***************************************************************************
//  $plRedF_IDX
//
//  Reduce "F IDX"
//***************************************************************************

LPPL_YYSTYPE plRedF_IDX
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes,               // Ptr to the result
                 lpYYVar;               // Ptr to a temp

    // If the current object is not in the process of stranding, ...
    if (!lpplYYCurObj->YYStranding)
    {
        // The result is always the root of the function tree
        lpYYRes =
          DbgPushFcnStrand_YY (lpplYYCurObj,
                               2,
                               (IsFcnStrDirect (lpplYYCurObj)) ? DIRECT
                                                               : INDIRECT); // Any function
        // YYFree the current object
        YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

        if (!lpYYRes)                           // If not defined, free args and YYERROR
            goto ERROR_EXIT;

        // Copy to the current object
        lpplYYCurObj = lpYYRes; // curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
        lpYYRes = NULL;
    } else
        // Unstrand the current object if necessary
        UnStrand (lpplYYCurObj);

    // If the last right object is in the process of stranding, ...
    if (lpplYYLstRht->YYStranding)
    {
        // Turn this strand into a var
        lpYYVar =
          DbgMakeVarStrand_EM_YY (lpplYYLstRht);
        // YYFree the last right object
        YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

        // If not defined, ...
        if (!lpYYVar)
            goto ERROR_EXIT;

        // Copy to the last right object
        lpplYYLstRht = lpYYVar; // lstSynObj = LSTSYNOBJ; Assert (lstSynObj <= soLAST);
        lpYYVar = NULL;
    } else
        // Unstrand the last right object if necessary
        UnStrand (lpplYYLstRht);

    // Make it into an axis operand
    lpYYVar = MakeAxis_YY (lpplYYLstRht);

    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    // Append the index to the function strand
    lpYYRes =
      DbgPushFcnStrand_YY (lpYYVar,
                           1,
                           (IsFcnStrDirect (lpYYVar)) ? DIRECT
                                                      : INDIRECT);  // Right operand
    // YYFree the temp var
    YYFree (lpYYVar); lpYYVar = NULL;

    if (!lpYYRes)                       // If not defined, free args and YYERROR
        goto ERROR_EXIT;

    // YYFree the index object
    YYFree (lpYYRes); lpYYRes = NULL;

    // Return the current object
    lpYYRes = lpplYYCurObj;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;
ERROR_EXIT:
    return lpYYRes;
} // End plRedF_IDX


//***************************************************************************
//  $plRedLBC_RBC
//
//  Reduce "LBC RBC"
//***************************************************************************

LPPL_YYSTYPE plRedLBC_RBC
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes,               // Ptr to the result
                 lpYYRes2;              // ...

    // Allocate a new YYRes
    lpYYRes2 = YYAlloc ();

    Assert (lpplYYLstRht->lptkLftBrace[1].tkFlags.TknType EQ TKT_GLBDFN );

    // Get the global memory handle
    lpYYRes2->tkToken.tkData.tkGlbData = lpplYYLstRht->lptkLftBrace[1].tkData.tkGlbData;

    // Set the caret index to the left brace
    lpYYRes2->tkToken.tkCharIndex      = lpplYYCurObj->tkToken.tkCharIndex;

    // Set the UDFO/AFO properties
    plSetDfn (&lpYYRes2->tkToken, GetGlbDataToken (&lpYYRes2->tkToken));

    // The result is always the root of the function tree
    lpYYRes =
      DbgPushFcnStrand_YY (lpYYRes2,
                           1,
                           (IsFcnStrDirect (lpYYRes2)) ? DIRECT
                                                       : INDIRECT); // Function or operator
    // YYFree the temp, last right, and current objects
    YYFree (lpYYRes2);     lpYYRes2     = NULL;
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstRhtObj = soNONE
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return lpYYRes;
} // End plRedLBC_RBC


//***************************************************************************
//  $plRedF_HY
//
//  Reduce "F HY"
//***************************************************************************

LPPL_YYSTYPE plRedF_HY
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    Assert (lpplYYLstRht->tkToken.tkFlags.TknType EQ TKT_OP3IMMED);

    // Convert the token type of the last right object to an immediate monadic operator
    lpplYYLstRht->tkToken.tkFlags.TknType = TKT_OP1IMMED;
    lpplYYLstRht->tkToken.tkFlags.ImmType = IMMTYPE_PRIMOP1;
    lpplYYLstRht->tkToken.tkSynObj        = soMOP;

    return plRedLftOper_MOP (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
} // End plRedF_HY


//***************************************************************************
//  $plRedCom_RP
//
//  Reduce "Com RP"
//***************************************************************************

LPPL_YYSTYPE plRedCom_RP
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstRhtObj = soNONE;

    // Change the tkSynObj
    lpplYYCurObj->tkToken.tkSynObj = soType;

    return lpplYYCurObj;
} // End plRedCom_RP


//***************************************************************************
//  $plRedLftOper_MOP
//
//  Reduce "LftOper MOP"
//***************************************************************************

LPPL_YYSTYPE plRedLftOper_MOP
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes,               // Ptr to the result
                 lpYYLft,               // Ptr to a temp
                 lpYYVar;               // ...

    // The result is always the root of the function tree
    lpYYRes =
      DbgPushFcnStrand_YY (lpplYYLstRht,
                           2,
                           (IsFcnStrDirect (lpplYYLstRht)) ? DIRECT
                                                           : INDIRECT); // Monadic operator
    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    if (!lpYYRes)                           // If not defined, free args and YYERROR
    {
        // Free and YYFree the current object
        FreeResNNU1 (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

        goto ERROR_EXIT;
    } // End IF

    // If the current object is in the process of stranding, ...
    if (IsTknTypeVar (lpplYYCurObj->tkToken.tkFlags.TknType)
     && lpplYYCurObj->YYStranding)
    {
        // Turn this strand into a var
        lpYYVar =
          DbgMakeVarStrand_EM_YY (lpplYYCurObj);
        // YYFree the current object
        YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

        // If not defined, ...
        if (!lpYYVar)
            goto ERROR_EXIT;

        // Copy to the current object
        lpplYYCurObj = lpYYVar; // curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
        lpYYVar = NULL;
    } else
        // Unstrand the current object if necessary
        UnStrand (lpplYYCurObj);

    // Append the left operand to the function strand
    lpYYLft =
      DbgPushFcnStrand_YY (lpplYYCurObj,
                           1,
                           (IsFcnStrDirect (lpplYYCurObj)) ? DIRECT
                                                           : INDIRECT); // Left operand
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    if (!lpYYLft)                       // If not defined, free args and YYERROR
    {
        // Free the function (including YYFree)
        YYFreeArray (lpYYRes); FreeYYFcn1 (lpYYRes); lpYYRes = NULL;

        goto ERROR_EXIT;
    } // End IF

    // YYFree the left operand object
    YYFree (lpYYLft); lpYYLft = NULL;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;
ERROR_EXIT:
    return lpYYRes;
} // End plRedLftOper_MOP


//***************************************************************************
//  $plRedF_FR
//
//  Reduce "F FR"
//***************************************************************************

LPPL_YYSTYPE plRedF_FR
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes = NULL,        // Ptr to the result
                 lpYYOp1,               // Ptr to temp vars
                 lpYYVarL,              // ...
                 lpYYVarR;              // ...

    // Make a pseudo train operator
    lpYYOp1 =
      MakeTrainOp_YY (lpplYYCurObj);

    // If this function is in the process of being stranded, ...
    if (lpplYYCurObj->YYStranding)
    {
        // Turn this function strand into a function
        lpYYRes =
          DbgMakeFcnStrand_EM_YY (lpplYYCurObj, TranslateSOTypeToNameType (lpplYYCurObj->tkToken.tkSynObj), FALSE);
        // YYFree the current object
        YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

        // If not defined, ...
        if (!lpYYRes)
            goto ERROR_EXIT;

        // Copy to the current object
        lpplYYCurObj = lpYYRes; // curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
        lpYYRes = NULL;
    } else
        // Unstrand the current object if necessary
        UnStrand (lpplYYCurObj);

    // If the last right object is in the process of stranding, ...
    if (lpplYYLstRht->YYStranding)
    {
        // Turn this function strand into a function
        lpYYRes =
          DbgMakeFcnStrand_EM_YY (lpplYYLstRht, TranslateSOTypeToNameType (lpplYYLstRht->tkToken.tkSynObj), FALSE);
        // YYFree the last right object
        YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

        // If not defined, ...
        if (!lpYYRes)
            goto ERROR_EXIT;

        // Copy to the last right object
        lpplYYLstRht = lpYYRes; // lstSynObj = LSTSYNOBJ; Assert (lstSynObj <= soLAST);
        lpYYRes = NULL;
    } else
        // Unstrand the last right object if necessary
        UnStrand (lpplYYLstRht);

    lpYYRes =
      DbgPushFcnStrand_YY (lpYYOp1, 3, DIRECT);             // Monadic operator (Direct)
    YYFree (lpYYOp1); lpYYOp1 = NULL;

    lpYYVarR =
      DbgPushFcnStrand_YY (lpplYYLstRht, 1, DIRECT);        // Righthand function (Direct)
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL;

    lpYYVarL =
      DbgPushFcnStrand_YY (lpplYYCurObj, 1, DIRECT);        // Lefthand function (Direct)
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL;

    // YYFree the temp vars
    YYFree (lpYYVarL); lpYYVarL = NULL;
    YYFree (lpYYVarR); lpYYVarR = NULL;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;
ERROR_EXIT:
    if (lpplYYCurObj)
    {
        // YYFree the current object
        YYFree (lpplYYCurObj); lpplYYCurObj = NULL;
    } // End IF

    if (lpplYYLstRht)
    {
        // YYFree the last right object
        YYFree (lpplYYLstRht); lpplYYLstRht = NULL;
    } // End IF

    return lpYYRes;
} // End plRedF_FR


//***************************************************************************
//  $plRedF_FFR
//
//  Reduce "F FFR"
//***************************************************************************

LPPL_YYSTYPE plRedF_FFR
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // If this function is in the process of being stranded, ...
    if (lpplYYCurObj->YYStranding)
    {
        // Turn this function strand into a function
        lpYYRes =
          DbgMakeFcnStrand_EM_YY (lpplYYCurObj, TranslateSOTypeToNameType (lpplYYCurObj->tkToken.tkSynObj), FALSE);
        // YYFree the current object
        YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

        // If not defined, ...
        if (!lpYYRes)
            goto ERROR_EXIT;

        // Copy to the current object
        lpplYYCurObj = lpYYRes; // curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
        lpYYRes = NULL;
    } else
        // Unstrand the current object if necessary
        UnStrand (lpplYYCurObj);

    lpYYRes =
      DbgPushFcnStrand_YY (lpplYYCurObj, 1, DIRECT);        // Lefthand function (Direct)

    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // YYFree the temp vars
    YYFree (lpYYRes); lpYYRes = NULL;

    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;

    return lpplYYLstRht;
ERROR_EXIT:
    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL;

    return NULL;
} // End plRedF_FFR


//***************************************************************************
//  $plRedLP_FFR
//
//  Reduce "LP FFR"
//***************************************************************************

LPPL_YYSTYPE plRedLP_FFR
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // Initialize the function strand (Train) base
    lpplYYLstRht->lpYYStrandBase = lpplYYLstRht->lpYYFcnBase;

    lpYYRes =
      MakeFcnStrand_EM_YY (lpplYYLstRht, NAMETYPE_TRN, TRUE);

    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL;

    if (!lpYYRes)                       // If not defined, free args and YYERROR
        goto ERROR_EXIT;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;
ERROR_EXIT:
    return lpYYRes;
} // End plRedLP_FFR


//***************************************************************************
//  $plRedNF_A
//
//  Reduce "NF A"
//***************************************************************************

LPPL_YYSTYPE plRedNF_A
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Execute the niladic function returning an array
    lpplYYCurObj =
      plExecuteFn0 (lpplYYCurObj);

    // Check for error
    if (!lpplYYCurObj)
        goto ERROR_EXIT;

    // Call common code
    return
      plRedA_A (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
ERROR_EXIT:
    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    return NULL;
} // End plRedNF_A


//***************************************************************************
//  $plRedNF_F
//
//  Reduce "NF F"
//***************************************************************************

LPPL_YYSTYPE plRedNF_F
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // If the last right object is in the process of stranding, ...
    if (lpplYYLstRht->YYStranding)
    {
        // Turn this function strand into a function
        lpYYRes =
          DbgMakeFcnStrand_EM_YY (lpplYYLstRht, NAMETYPE_FN12, FALSE);
        // YYFree the last right object
        YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // curSynObj = soNONE;

        // If not defined, ...
        if (!lpYYRes)
            goto ERROR_EXIT;

        // Copy to the last right object
        lpplYYLstRht = lpYYRes; // lstSynObj = LSTSYNOBJ; Assert (lstSynObj <= soLAST);
        lpYYRes = NULL;
    } // End IF

    // Execute the niladic function returning an array
    lpplYYCurObj =
      plExecuteFn0 (lpplYYCurObj);

    // Check for error
    if (!lpplYYCurObj)
        goto ERROR_EXIT;

    // Call common code
    return
      plRedA_F (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
ERROR_EXIT:
    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    return NULL;
} // End plRedNF_F


//***************************************************************************
//  $plRedNF_MOP
//
//  Reduce "NF MOP"  &  "NF_MOPN"
//***************************************************************************

LPPL_YYSTYPE plRedNF_MOP
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Execute the niladic function returning an array
    lpplYYCurObj =
      plExecuteFn0 (lpplYYCurObj);

    // Check for error
    if (!lpplYYCurObj)
        goto ERROR_EXIT;

    // Call common code
    return
      plRedA_MOP (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
ERROR_EXIT:
    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    return NULL;
} // End plRedNF_MOP


//***************************************************************************
//  $plRedNF_HY
//
//  Reduce "NF HY"
//***************************************************************************

LPPL_YYSTYPE plRedNF_HY
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Execute the niladic function returning an array
    lpplYYCurObj =
      plExecuteFn0 (lpplYYCurObj);

    // Check for error
    if (!lpplYYCurObj)
        goto ERROR_EXIT;

    // Call common code
    return
      plRedA_HY (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
ERROR_EXIT:
    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    return NULL;
} // End plRedNF_HY


//***************************************************************************
//  $plRedNF_IDX
//
//  Reduce "NF IDX"
//***************************************************************************

LPPL_YYSTYPE plRedNF_IDX
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Execute the niladic function returning an array
    lpplYYCurObj =
      plExecuteFn0 (lpplYYCurObj);

    // Check for error
    if (!lpplYYCurObj)
        goto ERROR_EXIT;

    // Call common code
    return
      plRedA_IDX (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
ERROR_EXIT:
    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    return NULL;
} // End plRedNF_IDX


//***************************************************************************
//  $plRedNF_NF
//
//  Reduce "NF NF"
//***************************************************************************

LPPL_YYSTYPE plRedNF_NF
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Execute the niladic function returning an array
    lpplYYCurObj =
      plExecuteFn0 (lpplYYCurObj);

    // Check for error
    if (!lpplYYCurObj)
        goto ERROR_EXIT;

#ifdef DEBUG
    // Decrement the SI level of lpplYYCurObj
    //   so YYResIsEmpty won't complain
    lpplYYCurObj->SILevel--;
#endif
    // Execute the niladic function returning an array
    lpplYYLstRht =
      plExecuteFn0 (lpplYYLstRht);
#ifdef DEBUG
    // Restore the SI level of lpplYYCurObj
    lpplYYCurObj->SILevel++;
#endif
    // Check for error
    if (!lpplYYLstRht)
        goto ERROR_EXIT;

    // Call common code
    return
      plRedA_A (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
ERROR_EXIT:
    if (lpplYYCurObj)
    {
        // YYFree the current object
        YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;
    } // End IF

    if (lpplYYLstRht)
    {
        // YYFree the last right object
        YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;
    } // End IF

    return NULL;
} // End plRedNF_NF


//***************************************************************************
//  $plRedMOP_IDX
//
//  Reduce "MOP IDX"
//***************************************************************************

LPPL_YYSTYPE plRedMOP_IDX
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYVar;               // Ptr to a temp

    // If the last right object is in the process of stranding, ...
    if (lpplYYLstRht->YYStranding)
    {
        // Turn this strand into a var
        lpYYVar =
          DbgMakeVarStrand_EM_YY (lpplYYLstRht);
        // YYFree the last right object
        YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

        // If not defined, ...
        if (!lpYYVar)
            goto ERROR_EXIT;

        // Copy to the last right object
        lpplYYLstRht = lpYYVar; // lstSynObj = LSTSYNOBJ; Assert (lstSynObj <= soLAST);
        lpYYVar = NULL;
    } else
        // Unstrand the last right object if necessary
        UnStrand (lpplYYLstRht);

    // Make it into an axis operand
    lpYYVar = MakeAxis_YY (lpplYYLstRht);

    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    return plRedLftOper_MOP (lpplLocalVars, lpYYVar, lpplYYCurObj, soType);
ERROR_EXIT:
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return NULL;
} // End plRedMOP_IDX


//***************************************************************************
//  $plRedSP_A
//
//  Reduce "SP A"  &  "SP SA"
//***************************************************************************

LPPL_YYSTYPE plRedSP_A
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYVar;               // Ptr to a temp

    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // If the last right object is in the process of stranding, ...
    if (lpplYYLstRht->YYStranding)
    {
        // Turn this strand into a var
        lpYYVar =
          DbgMakeVarStrand_EM_YY (lpplYYLstRht);
        // YYFree the last right object
        YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // curSynObj = soNONE;

        // If not defined, ...
        if (!lpYYVar)
            goto ERROR_EXIT;

        // Copy to the last right object
        lpplYYLstRht = lpYYVar; // lstSynObj = LSTSYNOBJ; Assert (lstSynObj <= soLAST);
        lpYYVar = NULL;
    } else
        // Unstrand the last right object if necessary
        UnStrand (lpplYYLstRht);

    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;

    // Set the NoDisplay flag
    lpplYYLstRht->tkToken.tkFlags.NoDisplay = TRUE;

    return lpplYYLstRht;
ERROR_EXIT:
    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // curSynObj = soNONE;

    return NULL;
} // End plRedSP_A


//***************************************************************************
//  $plRedSP_NF
//
//  Reduce "SP NF"
//***************************************************************************

LPPL_YYSTYPE plRedSP_NF
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // If the last right object is a system function, ...
    if (lpplYYLstRht->tkToken.tkFlags.TknType EQ TKT_FCNNAMED
     && lpplYYLstRht->tkToken.tkData.tkSym->stFlags.FcnDir)
    {
        Assert (lpplYYLstRht->tkToken.tkData.tkSym->stFlags.stNameType EQ NAMETYPE_FN0);

        // Execute the niladic function returning an array
        lpplYYLstRht =
          plExecuteFn0 (lpplYYLstRht);

        // Check for error
        if (!lpplYYLstRht)
            goto ERROR_EXIT;

        // Call common code
        return
          plRedSP_A (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soSPA);
    } // End IF

    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;
ERROR_EXIT:
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return lpplYYLstRht;
} // End plRedSP_NF


//***************************************************************************
//  $plRedSP_Com
//
//  Reduce "SP Com"
//***************************************************************************

LPPL_YYSTYPE plRedSP_Com
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;

    return lpplYYLstRht;
} // End plRedSP_Com


//***************************************************************************
//  $plRedIO_SPA
//
//  Reduce "IO SPA"
//***************************************************************************

LPPL_YYSTYPE plRedIO_SPA
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    UBOOL bEndingCR;                    // TRUE iff last line has CR

    // Check for ending CR
    bEndingCR = lpplYYCurObj->tkToken.tkData.tkChar EQ UTF16_QUAD;

    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // Mark as NOT already displayed
    lpplYYLstRht->tkToken.tkFlags.NoDisplay = FALSE;
    if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
        lpplLocalVars->bRet = FALSE;
    else
        lpplLocalVars->bRet =
          ArrayDisplay_EM (&lpplYYLstRht->tkToken, bEndingCR, &lpplLocalVars->bCtrlBreak);

    if (!lpplLocalVars->bRet)
    {
        FreeResult (lpplYYLstRht);

        goto ERROR_EXIT;
    } // End IF

    // Mark as already displayed
    lpplYYLstRht->tkToken.tkFlags.NoDisplay = TRUE;

    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;

    return lpplYYLstRht;

ERROR_EXIT:
    return NULL;
} // End plRedIO_SPA


//***************************************************************************
//  $plRedNAM_SPNF
//
//  Reduce "NAM SPNF"
//***************************************************************************

LPPL_YYSTYPE plRedNAM_SPNF
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Execute the niladic function returning an array
    lpplYYLstRht =
      plExecuteFn0 (lpplYYLstRht);

    // Check for error
    if (!lpplYYLstRht)
        goto ERROR_EXIT;

    // Call common code
    return
      plRedNAM_SPA (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
ERROR_EXIT:
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return NULL;
} // End plRedNAM_SPNF


//***************************************************************************
//  $plRedIO_SPNF
//
//  Reduce "IO SPNF"
//***************************************************************************

LPPL_YYSTYPE plRedIO_SPNF
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Execute the niladic function returning an array
    lpplYYLstRht =
      plExecuteFn0 (lpplYYLstRht);

    // Check for error
    if (!lpplYYLstRht)
        goto ERROR_EXIT;

    // Call common code
    return
      plRedIO_SPA (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
ERROR_EXIT:
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return NULL;
} // End plRedIO_SPNF


//***************************************************************************
//  $plRedF_SPA
//
//  Reduce "F SPA"
//***************************************************************************

LPPL_YYSTYPE plRedF_SPA
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    Assert (lpplLocalVars->lpplYYFcnCurry EQ NULL);

    // If this function is in the process of being stranded, ...
    if (lpplYYCurObj->YYStranding)
    {
        // Turn this function strand into a function
        lpYYRes =
          DbgMakeFcnStrand_EM_YY (lpplYYCurObj, NAMETYPE_FN12, FALSE);
        // YYFree the current object
        YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

        // If not defined, ...
        if (!lpYYRes)
            goto ERROR_EXIT;

        // Copy to the current object
        lpplYYCurObj = lpYYRes; // curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
        lpYYRes = NULL;
    } else
        // Unstrand the current object if necessary
        UnStrand (lpplYYCurObj);

    // Copy to the function curry object
    lpplLocalVars->lpplYYFcnCurry = lpplYYCurObj;

    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;

////YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;  // Do *NOT* free as it is still curried

    return lpplYYLstRht;

ERROR_EXIT:
    return NULL;
} // End plRedF_SPA


//***************************************************************************
//  $plRedIDX_SPA
//
//  Reduce "IDX SPA"
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- plRedIDX_SPA"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE plRedIDX_SPA
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    Assert (lpplLocalVars->lpplYYIdxCurry EQ NULL);

    // Copy to the left curry object
    lpplLocalVars->lpplYYIdxCurry = lpplYYCurObj;

    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;

////YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;  // Do *NOT* free as it is still curried

    return lpplYYLstRht;
} // End plRedIDX_SPA
#undef  APPEND_NAME


//***************************************************************************
//  $plRedA_SPA
//
//  Reduce "A SPA"
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- plRedA_SPA"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE plRedA_SPA
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // If it's a named var, ...
    if (lpplYYCurObj->tkToken.tkFlags.TknType EQ TKT_VARNAMED)
    {
        // Change the tkSynObj
        lpplYYCurObj->tkToken.tkSynObj = soNAM;

        return plRedNAM_SPCom (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
    } // End IF

    // YYFree the current object
                               YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;
    // Free and YYFree the last right object
    FreeResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lpplLocalVars->lptkNext);
    return NULL;
} // End plRedA_SPA
#undef  APPEND_NAME


//***************************************************************************
//  $plRedNAM_ISPA
//
//  Reduce "NAM ISPA"
//***************************************************************************

LPPL_YYSTYPE plRedNAM_ISPA
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    UBOOL bRet;                         // TRUE iff the result is valid

    Assert (lpplLocalVars->lpplYYIdxCurry NE NULL);

    // Assign the value to the indexed name
    bRet =
      ArrayIndexSet_EM (&lpplYYCurObj->tkToken,
                        &lpplLocalVars->lpplYYIdxCurry->tkToken,
                        &lpplYYLstRht->tkToken);
    // YYFree the current & curried objects
                                                YYFree (lpplYYCurObj);                  lpplYYCurObj = NULL; // curSynObj = soNONE;
    FreeResult (lpplLocalVars->lpplYYIdxCurry); YYFree (lpplLocalVars->lpplYYIdxCurry); lpplLocalVars->lpplYYIdxCurry = NULL;

    if (!bRet)
    {
        FreeResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL;
    } else
    {
        // Change the tkSynObj
        lpplYYLstRht->tkToken.tkSynObj = soType;

        // Mark as already displayed
        lpplYYLstRht->tkToken.tkFlags.NoDisplay = TRUE;
    } // End IF

    return lpplYYLstRht;
} // End plRedNAM_ISPA


//***************************************************************************
//  $plRedNAM_SPCom
//
//  Reduce "NAM SPA"  & "NAM SPF", etc.
//***************************************************************************

LPPL_YYSTYPE plRedNAM_SPCom
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // If the last right object is a var in the process of stranding, ...
    if (IsTknTypeVar (lpplYYLstRht->tkToken.tkFlags.TknType)
     && lpplYYLstRht->YYStranding)
    {
        // Turn this strand into a var
        lpYYRes =
          DbgMakeVarStrand_EM_YY (lpplYYLstRht);
        // YYFree the last right object
        YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

        // If not defined, ...
        if (!lpYYRes)
            goto ERROR_EXIT;

        // Copy to the last right object
        lpplYYLstRht = lpYYRes; // curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
        lpYYRes = NULL;
    } else
    // If the last right object is a function/operator in the process of being stranded, ...
    if (IsTknTypeFcnOpr (lpplYYLstRht->tkToken.tkFlags.TknType)
     && lpplYYLstRht->YYStranding)
    {
        // Turn this function strand into a function
        lpYYRes =
          DbgMakeFcnStrand_EM_YY (lpplYYLstRht, TranslateSOTypeToNameType (lpplYYLstRht->tkToken.tkSynObj), TRUE);
        // YYFree the last right object
        YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

        // If not defined, ...
        if (!lpYYRes)
            goto ERROR_EXIT;

        // Copy to the last right object
        lpplYYLstRht = lpYYRes; // lstSynObj = LSTSYNOBJ; Assert (lstSynObj <= soLAST);
        lpYYRes = NULL;
    } else
        // Unstrand the last right object if necessary
        UnStrand (lpplYYLstRht);

    // If the left fcn is present (select spec as in NAM F {is} A), ...
    if (lpplLocalVars->lpplYYFcnCurry)
    {
        Assert (!lpplYYCurObj->YYStranding && !lpplLocalVars->lpplYYFcnCurry->YYStranding);

        if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
            lpYYRes = NULL;
        else
            lpYYRes =
              ExecFunc_EM_YY (&lpplYYCurObj->tkToken, lpplLocalVars->lpplYYFcnCurry, &lpplYYLstRht->tkToken);
        // Free the function (including YYFree)
        YYFreeArray (lpplLocalVars->lpplYYFcnCurry); FreeYYFcn1 (lpplLocalVars->lpplYYFcnCurry); lpplLocalVars->lpplYYFcnCurry = NULL;

        // Free (unnamed) and YYFree the last right object
        FreeTempResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL;

        // If not defined, ...
        if (!lpYYRes)
            goto ERROR_EXIT;

        // Copy to the last right object
        lpplYYLstRht = lpYYRes; // lstSynObj = LSTSYNOBJ; Assert (lstSynObj <= soLAST);
        lpYYRes = NULL;
    } // End IF

    // Assign this token to this name
    AssignName_EM (&lpplYYCurObj->tkToken, &lpplYYLstRht->tkToken);
#ifdef DEBUG2
    dprintfWL0 (L"AssignName_EM %s = %p",
                lpplYYCurObj->tkToken.tkData.tkSym->stHshEntry->lpwCharName,
                lpplYYCurObj->tkToken.tkData.tkSym->stData.stGlbData);
#endif
    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;

    // Mark as assigned
    lpplYYLstRht->tkToken.tkFlags.NoDisplay = TRUE;
ERROR_EXIT:
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return lpplYYLstRht;
} // End plRedNAM_SPCom


//***************************************************************************
//  $plRedLP_Com
//
//  Common code for "LP FR" & "LP MR" & "LP DR" & "LP HR" & "LP FFR" & "LP AFR"
//***************************************************************************

LPPL_YYSTYPE plRedLP_Com
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;

    return lpplYYLstRht;
} // End plRedLP_Com


//***************************************************************************
//  $plRedAFOG_A
//
//  Reduce "AFOG A"  &  "AFOG SA"
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- plRedAFOG_A"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE plRedAFOG_A
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes = NULL,        // Ptr to the result
                 lpYYVar;               // Ptr to a temp

    // If the last right object is in the process of stranding, ...
    if (lpplYYLstRht->YYStranding)
    {
        // Turn this strand into a var
        lpYYVar =
          DbgMakeVarStrand_EM_YY (lpplYYLstRht);
        // YYFree the last right object
        YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

        // If not defined, ...
        if (!lpYYVar)
            goto ERROR_EXIT;

        // Copy to the last right object
        lpplYYLstRht = lpYYVar; // lstSynObj = LSTSYNOBJ; Assert (lstSynObj <= soLAST);
        lpYYVar = NULL;
    } else
        // Unstrand the last right object if necessary
        UnStrand (lpplYYLstRht);

    // Check the guard value
    if (!AfoGuard (lpplLocalVars, &lpplYYLstRht->tkToken))
        goto ERROR_EXIT;

    // Make a NoValue entry as the result
    lpYYRes = MakeNoValue_YY (&lpplYYCurObj->tkToken);

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;

    goto NORMAL_EXIT;

ERROR_EXIT:
    // Mark as in error
    lpplLocalVars->ExitType = EXITTYPE_ERROR;
NORMAL_EXIT:
    // Free and YYFree the last right object
    FreeResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL;

    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return lpYYRes;
} // End plRedAFOG_A
#undef  APPEND_NAME


//***************************************************************************
//  $plRedAFOR_A
//
//  Reduce "AFOR A"  &  "AFOR SA"
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- plRedAFOR_A"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE plRedAFOR_A
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYVar;               // Ptr to a temp

    // If the last right object is in the process of stranding, ...
    if (lpplYYLstRht->YYStranding)
    {
        // Turn this strand into a var
        lpYYVar =
          DbgMakeVarStrand_EM_YY (lpplYYLstRht);
        // YYFree the last right object
        YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

        // If not defined, ...
        if (!lpYYVar)
            goto ERROR_EXIT;

        // Copy to the last right object
        lpplYYLstRht = lpYYVar; // lstSynObj = LSTSYNOBJ; Assert (lstSynObj <= soLAST);
        lpYYVar = NULL;
    } else
        // Unstrand the last right object if necessary
        UnStrand (lpplYYLstRht);

    // Check the return value
    AfoReturn (lpplLocalVars, lpplYYLstRht);

    // Tell the caller to stop executing this line
    lpplLocalVars->bStopExec = TRUE;

////// Change the tkSynObj
////lpYYRes->tkToken.tkSynObj = soType;

    goto NORMAL_EXIT;

ERROR_EXIT:
    // Mark as in error
    lpplLocalVars->ExitType = EXITTYPE_ERROR;
NORMAL_EXIT:
    // Free (unnamed) and YYFree the last right object
    FreeTempResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL;

    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return NULL;
} // End plRedAFOR_A
#undef  APPEND_NAME


//***************************************************************************
//  $ParseLine
//
//  Parse a line
//  The result of parsing the line is in the return value (plLocalVars.ExitType)
//    as well as in lpMemPTD->YYResExec if there is a value (EXITTYPE_DISPLAY
//    or EXITTYPE_NODISPLAY).
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ParseLine"
#else
#define APPEND_NAME
#endif

EXIT_TYPES ParseLine
    (HWND           hWndSM,                 // Session Manager window handle
     LPTOKEN_HEADER lpMemTknHdr,            // Ptr to tokenized line global memory header
     HGLOBAL        hGlbTxtLine,            // Text of tokenized line global memory handle
     LPWCHAR        lpwszLine,              // Ptr to the line text (may be NULL)
     LPPERTABDATA   lpMemPTD,               // Ptr to PerTabData global memory
     UINT           uLineNum,               // Function line #
     UINT           uTknNum,                // Starting token # in the above function line
     HGLOBAL        hGlbDfnHdr,             // User-defined function/operator global memory handle (NULL = execute/immexec)
     UBOOL          bActOnErrors,           // TRUE iff errors are acted upon
     UBOOL          bExec1Stmt,             // TRUE iff executing only one stmt
     UBOOL          bNoDepthCheck)          // TRUE iff we're to skip the depth check

{
    PLLOCALVARS   plLocalVars = {0};        // SyntaxAnalyze local vars
    LPPLLOCALVARS oldTlsPlLocalVars;        // Ptr to previous value of dwTlsPlLocalVars
    UINT          oldTlsType,               // Previous value of dwTlsType
                  uRet;                     // The result from pl_yyparse
    UBOOL         bPLBracket = FALSE,       // TRUE iff we came from PARSELINE_MP_BRACKET
                  bRP_BRK;                  // TRUE if we should recurse into () or []
    ERROR_CODES   uError = ERRORCODE_NONE;  // Error code
    UBOOL         bOldExecuting,            // Old value of bExecuting
                  bAssignName,              // TRUE iff the result is from an assigned name
                  bSink;                    // TRUE iff the result is from sink
    HWND          hWndEC;                   // Edit Ctrl window handle
    LPSIS_HEADER  lpSISCur,                 // Ptr to current SI Stack Header
                  lpSISPrv;                 // Ptr to previous ...
    LPDFN_HEADER  lpMemDfnHdr;              // Ptr to user-defined function/operator header ...
    LPPL_YYSTYPE *lpplOrgLftStk,            // Original ptr to left stack
                 *lpplOrgRhtStk,            // ...             right ...
                  lpYYRes = NULL;           // Ptr to the result
    int           lftBndStr,                // Left binding strength
                  rhtBndStr;                // Right ...
////HGLOBAL       hGlbPL_YY = NULL;         // Global memory handle for PL_YYSTYPEs
    SO_ENUM       lftSynObj,                // Left stack's current element Syntax Object value
                  curSynObj,                // Current ...
                  rhtSynObj,                // Right ...
                  lstSynObj;                // Last right ...
    LPPL_REDSTR   lpplCurStr;               // Ptr to current plRedStr
    LPPL_YYSTYPE  lpplYYCurObj,             // Ptr to the current token object
                  lpplYYLstRht;             // The last POPRIGHT token in REDUCE

    static PL_YYSTYPE plYYEOS = {                   // tkToken
                                    {{TKT_EOS},     //   tkFlags
                                     soEOS,         //   tkSynObj
                                     {NULL},        //   tkData
                                     0},            //   tkCharIndex
                                    1,              // TknCount
                                    FALSE,          // YYInuse
                                    FALSE,          // YYIndirect
                                    FALSE,          // YYCopyArray
                                    TRUE ,          // YYPerm
                                };  // EOS PL_YYSTYPE

    static PL_YYSTYPE plYYSOS = {                   // tkToken
                                    {{TKT_SOS},     //   tkFlags
                                     soSOS,         //   tkSynObj
                                     {NULL},        //   tkData
                                     0},            //   tkCharIndex
                                    1,              // TknCount
                                    FALSE,          // YYInuse
                                    FALSE,          // YYIndirect
                                    FALSE,          // YYCopyArray
                                    TRUE ,          // YYPerm
                                };  // EOS PL_YYSTYPE
#ifdef DEBUG
    // Simplify certain names
    soNames[soSP] = WS_UTF16_LEFTARROW;
    soNames[soGO] = WS_UTF16_RIGHTARROW;
#endif
    // Save the previous value of dwTlsType
    oldTlsType = PtrToUlong (TlsGetValue (dwTlsType));

    // Save the thread type ('PL')
    TlsSetValue (dwTlsType, TLSTYPE_PL);

    // Save the previous value of dwTlsPlLocalVars
    oldTlsPlLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Save ptr to SyntaxAnalyze local vars
    TlsSetValue (dwTlsPlLocalVars, (LPVOID) &plLocalVars);

    // Save ptr to PerTabData global memory
    TlsSetValue (dwTlsPerTabData, (LPVOID) lpMemPTD);

    // Get the Edit Ctrl window handle
    (HANDLE_PTR) hWndEC = GetWindowLongPtrW (hWndSM, GWLSF_HWNDEC);

    // Indicate that we're executing
    bOldExecuting = lpMemPTD->bExecuting; SetExecuting (lpMemPTD, TRUE);

    EnterCriticalSection (&CSOPL);

    // Increment the depth counter
    lpMemPTD->uExecDepth++;

    // Save the original left & right stack ptrs
    lpplOrgLftStk = lpMemPTD->lpplLftStk;
    lpplOrgRhtStk = lpMemPTD->lpplRhtStk;

    // Link this plLocalVars into the chain of such objects
    plLocalVars.lpPLPrev = lpMemPTD->lpPLCur;

    // If there's a previous ptr, transfer its Ctrl-Break flag
    if (lpMemPTD->lpPLCur)
        plLocalVars.bCtrlBreak = lpMemPTD->lpPLCur->bCtrlBreak;

    // Save as new current ptr
    lpMemPTD->lpPLCur = &plLocalVars;

    LeaveCriticalSection (&CSOPL);

    // Initialize the error & return codes
    uError = ERRORCODE_NONE;
    uRet   = 0;

    // If we don't have a valid ptr, ...
    if (!lpMemTknHdr)
    {
        plLocalVars.ExitType = EXITTYPE_ERROR;

        goto NORMAL_EXIT;
    } // End IF
#ifdef DEBUG
    // Display the tokens so far
    DisplayTokens (lpMemTknHdr);
#endif

    // If there's a UDFO global memory handle, ...
    if (hGlbDfnHdr)
    {
        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);

        // Save value in LocalVars
        plLocalVars.bAFO          = lpMemDfnHdr->bAFO;
        plLocalVars.bAfoCtrlStruc = lpMemDfnHdr->bAfoCtrlStruc;
        plLocalVars.bMFO          = lpMemDfnHdr->bMFO;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
    } // End IF

    // Save values in the LocalVars
    plLocalVars.lpMemPTD       = lpMemPTD;
    plLocalVars.hWndSM         = hWndSM;
    plLocalVars.hGlbTxtLine    = hGlbTxtLine;
    plLocalVars.lpMemTknHdr    = lpMemTknHdr;
    plLocalVars.lpwszLine      = lpwszLine;
    plLocalVars.bLookAhead     = FALSE;
    plLocalVars.ExitType       = EXITTYPE_NONE;
    plLocalVars.uLineNum       = uLineNum;
    plLocalVars.hGlbDfnHdr     = hGlbDfnHdr;
    plLocalVars.bExec1Stmt     = bExec1Stmt;

    // Get # tokens in the line
    plLocalVars.uTokenCnt = plLocalVars.lpMemTknHdr->TokenCnt;

    // If the starting token # is outside the token count, ...
    if (uTknNum >= plLocalVars.uTokenCnt)
    {
        // Set the exit type to exit normally
        plLocalVars.ExitType = EXITTYPE_GOTO_ZILDE;

        goto NORMAL_EXIT;
    } // End IF

    // Skip over TOKEN_HEADER
    plLocalVars.lptkStart = TokenBaseToStart (plLocalVars.lpMemTknHdr);
    plLocalVars.lptkEnd   = &plLocalVars.lptkStart[plLocalVars.uTokenCnt];

    // Save a ptr to the EOS/EOL token
    plLocalVars.lptkEOS = plLocalVars.lptkStart;

    Assert (plLocalVars.lptkStart->tkFlags.TknType EQ TKT_EOL
         || plLocalVars.lptkStart->tkFlags.TknType EQ TKT_EOS);

    // If we're not starting at the first token, ...
    if (uTknNum)
    {
        // Skip to the starting token
        plLocalVars.lptkNext  = &plLocalVars.lptkStart[uTknNum];

        // If this token is an EOS/EOL, skip to the end of the stmt
        //   and start executing there
        if (plLocalVars.lptkNext->tkFlags.TknType EQ TKT_EOS
         || plLocalVars.lptkNext->tkFlags.TknType EQ TKT_EOL)
            plLocalVars.lptkNext = &plLocalVars.lptkNext[plLocalVars.lptkNext->tkData.tkIndex - 1];
    } else
        // Skip to end of 1st stmt
        plLocalVars.lptkNext  = &plLocalVars.lptkStart[plLocalVars.lptkStart->tkData.tkIndex];

    // Start off with no error
    plLocalVars.tkErrorCharIndex =
    plLocalVars.tkLACharIndex    = NEG1U;

    // Initialize the strand starts
    if (oldTlsPlLocalVars)
    {
        plLocalVars.lpYYStrArrStart[STRAND_VAR] = oldTlsPlLocalVars->lpYYStrArrNext[STRAND_VAR];
        plLocalVars.lpYYStrArrStart[STRAND_FCN] = oldTlsPlLocalVars->lpYYStrArrNext[STRAND_FCN];
        plLocalVars.lpYYStrArrStart[STRAND_LST] = oldTlsPlLocalVars->lpYYStrArrNext[STRAND_LST];
        plLocalVars.lpYYStrArrStart[STRAND_NAM] = oldTlsPlLocalVars->lpYYStrArrNext[STRAND_NAM];
    } else
    {
        plLocalVars.lpYYStrArrStart[STRAND_VAR] = lpMemPTD->lpStrand[STRAND_VAR];
        plLocalVars.lpYYStrArrStart[STRAND_FCN] = lpMemPTD->lpStrand[STRAND_FCN];
        plLocalVars.lpYYStrArrStart[STRAND_LST] = lpMemPTD->lpStrand[STRAND_LST];
        plLocalVars.lpYYStrArrStart[STRAND_NAM] = lpMemPTD->lpStrand[STRAND_NAM];
    } // End IF/ELSE

    // Initialize the base & next strand ptrs
    plLocalVars.lpYYStrArrStart[STRAND_VAR]->lpYYStrandBase =
    plLocalVars.lpYYStrArrBase [STRAND_VAR]                 =
    plLocalVars.lpYYStrArrNext [STRAND_VAR]                 = plLocalVars.lpYYStrArrStart[STRAND_VAR];
    plLocalVars.lpYYStrArrStart[STRAND_FCN]->lpYYStrandBase =
    plLocalVars.lpYYStrArrBase [STRAND_FCN]                 =
    plLocalVars.lpYYStrArrNext [STRAND_FCN]                 = plLocalVars.lpYYStrArrStart[STRAND_FCN];
    plLocalVars.lpYYStrArrStart[STRAND_LST]->lpYYStrandBase =
    plLocalVars.lpYYStrArrBase [STRAND_LST]                 =
    plLocalVars.lpYYStrArrNext [STRAND_LST]                 = plLocalVars.lpYYStrArrStart[STRAND_LST];
    plLocalVars.lpYYStrArrStart[STRAND_NAM]->lpYYStrandBase =
    plLocalVars.lpYYStrArrBase [STRAND_NAM]                 =
    plLocalVars.lpYYStrArrNext [STRAND_NAM]                 = plLocalVars.lpYYStrArrStart[STRAND_NAM];

    __try
    {
        __try
        {
            //***************************************************************************
            //  Start of 2by2 code
            //***************************************************************************

            Assert (plLocalVars.lptkStart[plLocalVars.uTokenCnt - 1].tkFlags.TknType EQ TKT_SOS);

            // Initialize the left & right stacks with an EOS & SOS
            *lpMemPTD->lpplLftStk++ = &plYYSOS;
            *lpMemPTD->lpplRhtStk++ = &plYYEOS;

            //***************************************************************************
            //  lpplLftStk and lpplRhtStk always point to the next available slot
            //***************************************************************************

            // Initialize the lpplYYLstRht var
            lpplYYLstRht = &plYYEOS; lstSynObj = LSTSYNOBJ; Assert (lstSynObj <= soLAST);

            // Parse the line, check for errors
            //   0 = success
            //   1 = YYABORT or APL error
            //   2 = memory exhausted
            // uRet = pl_yyparse (&plLocalVars);
PARSELINE_START:
            // Get the current object
            lpplYYCurObj = POPLEFT; // curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
#ifdef DEBUG2
            {
                LPWCHAR lpwszLine2;

                if (hGlbTxtLine)
                    lpwszLine2 = MyGlobalLock (hGlbTxtLine);
                else
                    lpwszLine2 = lpwszLine - 2;
                dprintfWL0 (L"Starting line:  %s",
                           &lpwszLine2[2]);
                TRACE (L"Starting:", L"", CURSYNOBJ, RHTSYNOBJ);

                if (hGlbTxtLine)
                {
                    MyGlobalUnlock (hGlbTxtLine); lpwszLine2 = NULL;
                } // End IF
            }
#endif
            // If this is a niladic function, ...
            if (CURSYNOBJ EQ soNF
             && !lpplYYCurObj->YYStranding                              // Not in stranding
             && IsTknNamedFcnOpr (&lpplYYCurObj->tkToken)               // Is a named function/operator
             && !lpplYYCurObj->tkToken.tkData.tkSym->stFlags.FcnDir)    // Not a direct fcn/opr
            {
                // Ensure this is not an AFO

                // Lock the memory to get a ptr to it
                lpMemDfnHdr = MyGlobalLock (lpplYYCurObj->tkToken.tkData.tkSym->stData.stGlbData);

                // If it's not an AFO, ...
                if (!lpMemDfnHdr->bAFO)
                {
                    // The result is always the root of the function tree
                    lpYYRes =
                      DbgPushFcnStrand_YY (lpplYYCurObj,
                                           1,
                                           (IsFcnStrDirect (lpplYYCurObj)) ? DIRECT
                                                                           : INDIRECT); // Niladic function
                    // YYFree the current object
                    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; curSynObj = soNONE;

                    // If not defined, ...
                    if (!lpYYRes)
                        goto PARSELINE_ERROR;

                    // Copy to the current object
                    lpplYYCurObj = lpYYRes; curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
                    lpYYRes = NULL;
                } // End IF

                // We no longer need this tr
                MyGlobalUnlock (lpplYYCurObj->tkToken.tkData.tkSym->stData.stGlbData); lpMemDfnHdr = NULL;
            } else
            // If this is an EOS, ...
            if (CURSYNOBJ EQ soEOS)
            {
                // Make a NoValue entry as the result
                lpYYRes = MakeNoValue_YY (&lpplYYCurObj->tkToken);

                // Change the tkSynObj
                lpYYRes->tkToken.tkSynObj = soEOS;

                // YYFree the current object
                YYFree (lpplYYCurObj); lpplYYCurObj = NULL; curSynObj = soNONE;

                // Copy to the current object
                lpplYYCurObj = lpYYRes; curSynObj = soEOS; Assert (curSynObj <= soLAST);
                lpYYRes = NULL;

                goto PARSELINE_DONE;
            } else
            // If this is a SYNTAX ERROR, ...
            if (CURSYNOBJ EQ soSYNR)
                goto PARSELINE_SYNTERR;

            goto PARSELINE_SCAN;

PARSELINE_SHIFT:
            PUSHRIGHT (lpplYYCurObj); lpplYYCurObj = POPLEFT; // curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);

            TRACE (L"Shifted: ", L"", CURSYNOBJ, RHTSYNOBJ);
PARSELINE_SCAN:
            // Get the left, current, and right SynObj values
            lftSynObj = LFTSYNOBJ; Assert (lftSynObj <= soLAST);
            curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
            rhtSynObj = RHTSYNOBJ; Assert (rhtSynObj <= soLAST);

            TRACE (L"Scanning:", L"", CURSYNOBJ, RHTSYNOBJ);
PARSELINE_SCAN1:
            // Get the left & right binding strengths
            lftBndStr = LBIND (lftSynObj, curSynObj);   // Ensure that lftSynObj & curSynObj are set
            rhtBndStr = RBIND (curSynObj, rhtSynObj);   // ...         rhtSynObj & ...

            TRACE (L"Binding: ", L"", CURSYNOBJ, RHTSYNOBJ);

            // If lftSynObj is neither soIDX nor soSRBK, ...
            if (lftSynObj NE soIDX          // LFTSYNOBJ NE IDX
             && lftSynObj NE soSRBK         // LFTSYNOBJ NE SRBK
             && lftBndStr < rhtBndStr)      // LBIND < RBIND
                goto PARSELINE_REDUCE;
            else
            if (lftBndStr > rhtBndStr)      // LBIND > RBIND
                goto PARSELINE_SHIFT;
            else                            // LBIND EQ RBIND
            // If the left (or right) binding strength is non-zero, ...
            if (lftBndStr NE 0)
                goto PARSELINE_SHIFT;       // Not both zero

            // Fall through if both binding strengths are zero

            // MATCH_PAIR

            // Check for left & right parens
            if (lftSynObj EQ soLP && rhtSynObj EQ soRP)
                goto PARSELINE_MP_PAREN;
            else
            // Check for left & right brackets
            if (lftSynObj EQ soLBK && rhtSynObj EQ soRBK)
                goto PARSELINE_MP_BRACKET;
            else
            // Check for left & right EOS
            if (lftSynObj EQ soEOS && rhtSynObj EQ soEOS)
                goto PARSELINE_MP_DONE;

            // Check for SYNTAX ERROR
            if (curSynObj EQ soSYNR)
                goto PARSELINE_SYNTERR;

            // Check for VALUE ERROR
            if (curSynObj EQ soVALR)
                goto PARSELINE_VALUEERR;

            // Check for shift
            if (curSynObj NE soEOS)
                goto PARSELINE_SHIFT;

            goto PARSELINE_SYNTERR;

PARSELINE_MP_BRACKET:
            // Mark as coming from PARSELINE_MP_BRACKET
            bPLBracket = TRUE;
PARSELINE_MP_PAREN:
            // Remove the parens/brackets from the left & right stacks
            YYFree (POPRIGHT); YYFree (POPLEFT);

            TRACE (L"MatchPair", L"", CURSYNOBJ, RHTSYNOBJ);

            // If the current object is a Niladic Function, ...
            if (curSynObj EQ soNF)
            {
                // Execute the niladic function returning an array
                lpYYRes =
                  plExecuteFn0 (lpplYYCurObj);

                // Check for error
                if (!lpYYRes)
                    goto PARSELINE_ERROR;

                // Copy to the current object
                lpplYYCurObj = lpYYRes; curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
                lpYYRes = NULL;
            } // End IF

            // If the current object is in the process of stranding, ...
            if (lpplYYCurObj->YYStranding)
            {
                // Turn this strand into a var
                lpYYRes =
                  DbgMakeVarStrand_EM_YY (lpplYYCurObj);
                // YYFree the current object
                YYFree (lpplYYCurObj); lpplYYCurObj = NULL; curSynObj = soNONE;

                // If not defined, ...
                if (!lpYYRes)
                    goto PARSELINE_ERROR;

                // Copy to the current object
                lpplYYCurObj = lpYYRes; curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
                lpYYRes = NULL;
            } else
            // If the current object is a numeric/character strand, ...
            if (lpplYYCurObj->tkToken.tkFlags.TknType EQ TKT_NUMSTRAND
             || lpplYYCurObj->tkToken.tkFlags.TknType EQ TKT_CHRSTRAND)
            {
                // Unstrand the current object
                UnStrand (lpplYYCurObj);
            } else
            // If the current object is a numeric scalar, ...
            if (lpplYYCurObj->tkToken.tkFlags.TknType EQ TKT_NUMSCALAR)
                lpplYYCurObj->tkToken.tkFlags.TknType =  TKT_VARIMMED;

            // If we came from PARSELINE_MP_BRACKET, ...
            if (bPLBracket)
            {
                // Change the tkSynObj value to IDX
                lpplYYCurObj->tkToken.tkSynObj = soIDX;

                // Clear the flag
                bPLBracket = FALSE;
            } // End IF

            // Get the left, current, and right SynObj values
            lftSynObj = LFTSYNOBJ; Assert (lftSynObj <= soLAST);
            curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
            rhtSynObj = RHTSYNOBJ; Assert (rhtSynObj <= soLAST);

            goto PARSELINE_SCAN1;

PARSELINE_MP_DONE:
#ifdef DEBUG2
            dprintfWL0 (L"MatchPair Done:  curSynObj (%s)",
                        soNames[curSynObj]);
#endif
            // If the current object is a Niladic Function, ...
            if (curSynObj EQ soNF)
            {
                UBOOL NoDisplay;            // NoDisplay flag

                // Save the NoDisplay flag
                NoDisplay = lpplYYCurObj->tkToken.tkFlags.NoDisplay;

                // If this function is in the process of being stranded, ...
                if (lpplYYCurObj->YYStranding)
                {
                    // Turn this function strand into a function
                    lpYYRes =
                      DbgMakeFcnStrand_EM_YY (lpplYYCurObj, NAMETYPE_FN0, FALSE);
                    // YYFree the current object
                    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; curSynObj = soNONE;

                    // If not defined, ...
                    if (!lpYYRes)
                        goto PARSELINE_ERROR;

                    // Transfer the NoDisplay flag
                    lpYYRes->tkToken.tkFlags.NoDisplay = NoDisplay;

                    // Copy to the current object
                    lpplYYCurObj = lpYYRes; curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
                    lpYYRes = NULL;

                    // Set the tkSynObj
                    lpplYYCurObj->tkToken.tkSynObj = curSynObj;
                } // End IF

                // If the current object is not the result of an assignment, ...
                if (!NoDisplay)
                {
                    // Execute the niladic function returning an array
                    lpYYRes =
                      plExecuteFn0 (lpplYYCurObj);

                    // Check for error
                    if (!lpYYRes)
                        goto PARSELINE_ERROR;

                    // Copy to the current object
                    lpplYYCurObj = lpYYRes; curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
                    lpYYRes = NULL;
                } // End IF
            } else
            // If the current object is a CS0, ...
            if (curSynObj EQ soCS0)
            {
                // Execute the CS0 Function and replace the current object with the result
                lpYYRes =
                  ExecuteCS0 (&plLocalVars, lpplYYCurObj);

                // Check for error
                if (!lpYYRes)
                    goto PARSELINE_ERROR;

                // If it's NoValue, ...

                // Copy to the current object
                lpplYYCurObj = lpYYRes; curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
                lpYYRes = NULL;
            } else
            // If the current object is a soGO, ...
            if (curSynObj EQ soGO)
                plLocalVars.ExitType = EXITTYPE_RESET_ONE;
            else
                // Unstrand the current object if necessary
                UnStrand (lpplYYCurObj);

            goto PARSELINE_DONE;

PARSELINE_REDUCE:
            // CUROBJ = REDUCE

            Assert (curSynObj EQ CURSYNOBJ); Assert (curSynObj <= soLAST);

            // Recurse into () or [] ?
            bRP_BRK = (curSynObj EQ soRP
                    || curSynObj EQ soRBK
                    || curSynObj EQ soARBK
                    || curSynObj EQ soSRBK);
            // If we should recurse into () or [], ...
            if (bRP_BRK)
            {
                PUSHRIGHT (lpplYYCurObj); lpplYYCurObj = POPLEFT; curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);

                TRACE (L"Recursing", L"", CURSYNOBJ, RHTSYNOBJ);
            } else
            {
                RESET_FLAGS  resetFlag;             // The current Reset Flag

                lpplYYLstRht = POPRIGHT; lstSynObj = LSTSYNOBJ; Assert (lstSynObj <= soLAST);

                // If we're recursing into "A HY", instead change the HY to F, push it back onto RSTACK, and shift left
                if (curSynObj EQ soA
                 && lstSynObj EQ soHY)
                {
                    // Change the HY to F
                    lpplYYLstRht->tkToken.tkSynObj = lstSynObj = soF;

                    // If it's a named function/operator, ...
                    if (IsTknNamedFcnOpr (&lpplYYLstRht->tkToken))
                        // Change it to a primitive function
                        lpplYYLstRht->tkToken.tkData.tkSym->stFlags.ImmType = IMMTYPE_PRIMFCN;
                    else
                    {
                        // Change it to a primitive function
                        lpplYYLstRht->tkToken.tkFlags.TknType = TKT_FCNIMMED;
                        lpplYYLstRht->tkToken.tkFlags.ImmType = IMMTYPE_PRIMFCN;
                        lpplYYLstRht->tkToken.tkSynObj        = soF;

                        // If the last right object is in the process of stranding, ...
                        if (lpplYYLstRht->YYStranding)
                        {
                            UBOOL bOp1,             // TRUE iff the token is a monadic operator
                                  bOp2;             // ...                     dyadic  ...

                            Assert (lpplYYLstRht->TknCount > 1);
                            Assert (lpplYYLstRht->lpYYFcnBase NE NULL);

                            // Check the operator status
                            bOp1 = IsTknOp1 (&lpplYYLstRht->lpYYFcnBase->tkToken);
                            bOp2 = IsTknOp2 (&lpplYYLstRht->lpYYFcnBase->tkToken);

                            // If the function base is a hybrid, ...
                            if (lpplYYLstRht->lpYYFcnBase->tkToken.tkFlags.TknType EQ TKT_OP3IMMED
                             && lpplYYLstRht->lpYYFcnBase->tkToken.tkFlags.ImmType EQ IMMTYPE_PRIMOP3)
                            {
                                // Change it to a primitive function
                                lpplYYLstRht->lpYYFcnBase->tkToken.tkFlags.TknType = TKT_FCNIMMED;
                                lpplYYLstRht->lpYYFcnBase->tkToken.tkFlags.ImmType = IMMTYPE_PRIMFCN;
                                lpplYYLstRht->lpYYFcnBase->tkToken.tkSynObj        = soF;
                            } else
                            // If the function base is a monadic or dyadic operator, ...
                            if ((bOp1 || bOp2)
                             && lpplYYLstRht->lpYYFcnBase[1].tkToken.tkFlags.TknType EQ TKT_OP3IMMED
                             && lpplYYLstRht->lpYYFcnBase[1].tkToken.tkFlags.ImmType EQ IMMTYPE_PRIMOP3)
                            {
                                // Change it to a primitive function
                                lpplYYLstRht->lpYYFcnBase[1].tkToken.tkFlags.TknType = TKT_FCNIMMED;
                                lpplYYLstRht->lpYYFcnBase[1].tkToken.tkFlags.ImmType = IMMTYPE_PRIMFCN;
                                lpplYYLstRht->lpYYFcnBase[1].tkToken.tkSynObj        = soF;
                            } else
                            // If the function base is a dyadic operator, ...
                            if (bOp2
                             && lpplYYLstRht->lpYYFcnBase[2].tkToken.tkFlags.TknType EQ TKT_OP3IMMED
                             && lpplYYLstRht->lpYYFcnBase[2].tkToken.tkFlags.ImmType EQ IMMTYPE_PRIMOP3)
                            {
                                // Change it to a primitive function
                                lpplYYLstRht->lpYYFcnBase[2].tkToken.tkFlags.TknType = TKT_FCNIMMED;
                                lpplYYLstRht->lpYYFcnBase[2].tkToken.tkFlags.ImmType = IMMTYPE_PRIMFCN;
                                lpplYYLstRht->lpYYFcnBase[2].tkToken.tkSynObj        = soF;
                            } // End IF/ELSE/...
                        } // End IF
                    } // End IF/ELSE

                    // Push it back onto RSTACK
                    PUSHRIGHT (lpplYYLstRht);

                    goto LEFTSHIFT;
                } // End IF

                // If the current object is INPOUT, and
                //   the last right object is neither SPA nor SPNF
                if (curSynObj EQ soIO
                 && lstSynObj NE soSPA
                 && lstSynObj NE soSPNF)
                {
                    lpYYRes =
                      WaitForInput (plLocalVars.hWndSM,
                                    lpplYYCurObj->tkToken.tkData.tkChar EQ UTF16_QUOTEQUAD,
                                   &lpplYYCurObj->tkToken);
                    // YYFree the current object
                    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

                    // Get the Reset Flag
                    resetFlag = plLocalVars.lpMemPTD->lpSISCur->ResetFlag;

                    // If we're resetting, ...
                    if (resetFlag NE RESETFLAG_NONE)
                    {
                        plLocalVars.ExitType = TranslateResetFlagToExitType (resetFlag);

                        goto PARSELINE_ERROR;
                    } // End IF

                    if (!lpYYRes)                   // If not defined, free args and YYERROR
                        goto PARSELINE_ERROR;

                    // Set the new tkSynObj
                    lpYYRes->tkToken.tkSynObj = soA;

                    // Copy to the current object
                    lpplYYCurObj = lpYYRes; curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
                    lpYYRes = NULL;
                } // End IF

                // If the last right object is INPOUT, ...
                if (lstSynObj EQ soIO)
                {
                    lpYYRes =
                      WaitForInput (plLocalVars.hWndSM,
                                    lpplYYLstRht->tkToken.tkData.tkChar EQ UTF16_QUOTEQUAD,
                                   &lpplYYLstRht->tkToken);
                    // YYFree the last right object
                    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

                    // Get the Reset Flag
                    resetFlag = plLocalVars.lpMemPTD->lpSISCur->ResetFlag;

                    // If we're resetting, ...
                    if (resetFlag NE RESETFLAG_NONE)
                    {
                        plLocalVars.ExitType = TranslateResetFlagToExitType (resetFlag);

                        goto PARSELINE_ERROR;
                    } // End IF

                    if (!lpYYRes)                   // If not defined, free args and YYERROR
                        goto PARSELINE_ERROR;

                    // Set the new tkSynObj
                    lpYYRes->tkToken.tkSynObj = soA;

                    // Copy to the current object
                    lpplYYLstRht = lpYYRes;  lstSynObj = LSTSYNOBJ; Assert (lstSynObj <= soLAST);
                    lpYYRes = NULL;
                } // End IF

                // Point to the PL_REDSTR
                lpplCurStr = &plRedStr[curSynObj][lpplYYLstRht->tkToken.tkSynObj];
#ifdef DEBUG
                if (lpplCurStr->lpplRedFcn EQ NULL)
                {
                    MessageBoxW (NULL,
                                 L"ASSERTION ERROR",
                                 lpwszAppName,
                                 MB_OK | MB_ICONWARNING | MB_APPLMODAL);
                    DbgStop ();     // ***FINISHME*** -- can this ever occur??
                } // End IF

                {
                    WCHAR EVENT[64];

                    wsprintfW (EVENT,
                               L"- \"%s %s\" " WS_UTF16_RIGHTARROW L" %s",
                               soNames[curSynObj],
                               soNames[lstSynObj],
                               soNames[lpplCurStr->soType]);
                    TRACE2 (L"Reducing:", EVENT, lpplCurStr->soType, lstSynObj);
                }
#endif
                // Reduce the stmt
                // *** N.B.  This call might change the contents of lpplYYLstRht ***
                lpYYRes =
                  (*lpplCurStr->lpplRedFcn) (&plLocalVars,
                                              lpplYYCurObj,
                                              lpplYYLstRht,
                                              lpplCurStr->soType);
                // Check for stop execution of this line
                if (plLocalVars.bStopExec)
                {
                    // Check for NoValue
                    if (IsTokenNoValue (&lpYYRes->tkToken))
                    {
                        // YYFree the result
                        YYFree (lpYYRes); lpYYRes = NULL;
                    } // End IF

                    // Set the curSynObj to something harmless
                    curSynObj = soEOS;

                    goto PARSELINE_DONE;
                } // End IF

                // If we're restarting, ...
                if (plLocalVars.bRestart)
                {
                    plLocalVars.bRestart = FALSE;

                    // The current token is SOS

                    // Check for NoValue
                    if (IsTokenNoValue (&lpYYRes->tkToken))
                    {
                        // YYFree the result
                        YYFree (lpYYRes); lpYYRes = NULL;
                    } // End IF

                    goto PARSELINE_START;
                } // Ebd IF

                // Check for error
                if (!lpYYRes)
                    goto PARSELINE_ERROR;

#ifdef DEBUG
                // In case the result is from a lower SI level (such as Execute)
                //   reset the SI level
                lpYYRes->SILevel = lpMemPTD->SILevel;
#endif

                // Check for ResetFlag
                if (lpMemPTD->lpSISCur->ResetFlag NE RESETFLAG_NONE)
                    goto PARSELINE_ERROR;

                // Get the left, current, and right SynObj values
                lftSynObj = LFTSYNOBJ; Assert (lftSynObj <= soLAST);
                curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
                rhtSynObj = RHTSYNOBJ; Assert (rhtSynObj <= soLAST);

                // Check for NoValue
                if (IsTokenNoValue (&lpYYRes->tkToken)
                 && lftSynObj EQ soEOS)
                {
                    // Copy to the current object
                    lpplYYCurObj = lpYYRes; lpplYYCurObj->tkToken.tkSynObj = curSynObj = soEOS; Assert (curSynObj <= soLAST);
                    lpYYRes = NULL;
                } else
                {
                    // Copy to the current object
                    lpplYYCurObj = lpYYRes; curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
                    lpYYRes = NULL;
                } // End IF/ELSE

                // If it's not a SYNTAX ERROR, ...
                if (curSynObj <= soLAST)
                {
                    // If a shift right is required, ...
////////////////////if (2 < RSTACKLEN && LBIND (lftSynObj, curSynObj) >= RBIND (curSynObj, rhtSynObj))
                    while (TRUE)
                    if (1 < RSTACKLEN
                     && curSynObj NE soRP
                     && curSynObj NE soRBK
                     && curSynObj NE soARBK
                     && curSynObj NE soSRBK)
                    {
                        SO_ENUM rht2SynObj;

                        rht2SynObj = RHT2SYNOBJ; Assert (rht2SynObj <= soLAST);

                        // Check for left shift
                        if (RBIND (curSynObj, rhtSynObj) >= RBIND (rhtSynObj, rht2SynObj))
                        {
                            if (rhtSynObj NE soRP
                             && rhtSynObj NE soRBK
                             && rhtSynObj NE soARBK
                             && rhtSynObj NE soSRBK)
                            {
                                if (LBIND (lftSynObj, curSynObj) <= RBIND (curSynObj, rhtSynObj))
                                    break;
                            } else
                            {
                                if (LBIND (lftSynObj, curSynObj) <= RBIND (curSynObj, rhtSynObj)
                                 && RBIND (curSynObj, rhtSynObj) <= RBIND (rhtSynObj, rht2SynObj))
                                    break;
                            } // End IF/ELSE
                        } // End IF
LEFTSHIFT:
                        PUSHLEFT (lpplYYCurObj); lpplYYCurObj = POPRIGHT; curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);

                        rhtSynObj  = RHTSYNOBJ; Assert (rhtSynObj <= soLAST);
                        rht2SynObj = RHT2SYNOBJ;

                        TRACE (L"PostRed: ", L"- LftShift", CURSYNOBJ, rhtSynObj);
                    } else
                        break;
                } // End IF
            } // End IF/ELSE

            // Check for SYNTAX ERROR
            if (curSynObj <= soLAST)
            {
                // Get the left, current, and right SynObj values
                lftSynObj = LFTSYNOBJ; Assert (lftSynObj <= soLAST);
////////////////curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
                rhtSynObj = RHTSYNOBJ; Assert (rhtSynObj <= soLAST);

                goto PARSELINE_SCAN1;
            } else
                goto PARSELINE_SYNTERR;

PARSELINE_VALUEERR:
            ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
                                      &lpplYYCurObj->tkToken);
            goto PARSELINE_ERROR;

PARSELINE_SYNTERR:
            ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                      &lpplYYCurObj->tkToken);
            goto PARSELINE_ERROR;

PARSELINE_ERROR:
            // If lpYYRes is defined, ...
            if (lpYYRes)
            {
                // YYFree the temp
                YYFree (lpYYRes); lpYYRes = NULL;
            } // End IF

            // Loop through the right stack freeing temps
            while (RSTACKLEN > 1)
            {
                LPPL_YYSTYPE lpYYRes;

                // Get the next item from the right stack
                lpYYRes = POPRIGHT;

                // If it's a function/operator, ...
                if (IsTknFcnOpr (&lpYYRes->tkToken))
                {
                    // If this function is in the process of being stranded, ...
                    if (lpYYRes->YYStranding)
                    {
                        LPPL_YYSTYPE lpYYRes2;

                        // Turn this function strand into a function
                        lpYYRes2 =
                          DbgMakeFcnStrand_EM_YY (lpYYRes, NAMETYPE_FN12, FALSE);
                        // YYFree the function
                        YYFree (lpYYRes); lpYYRes = NULL;

                        // If not defined, ...
                        if (!lpYYRes2)
                            goto PARSELINE_ERROR;

                        // Copy to the function
                        lpYYRes = lpYYRes2;
                        lpYYRes2 = NULL;
                    } // End IF

                    // Free the function (including YYFree)
                    YYFreeArray (lpYYRes); FreeYYFcn1 (lpYYRes); lpYYRes = NULL;
                } else
                // If it's a var, ...
                if (IsTknTypeVar (lpYYRes->tkToken.tkFlags.TknType))
                {
                    // Unstrand the temp if necessary
                    UnStrand (lpYYRes);

                    // Free & YYFree the temp
                    FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;
                } else
                {
                    // YYFree the temp
                    YYFree (lpYYRes); lpYYRes = NULL;
                } // End IF/ELSE/...
            } // End WHILE

            // If the current object is in use, ...
            if (lpplYYCurObj
             && lpplYYCurObj->YYInuse)
            {
                // YYFree the current object
                YYFree (lpplYYCurObj); lpplYYCurObj = NULL; curSynObj = soNONE;
            } // End IF

            // If the last right object is in use, ...
            if (lpplYYLstRht
             && lpplYYLstRht->YYInuse)
            {
                // YYFree the last right object
                YYFree (lpplYYLstRht); lpplYYLstRht = NULL; lstSynObj = soNONE;
            } // End IF

            // If a curried function is in use
            if (plLocalVars.lpplYYFcnCurry)
            {
                // Free the function (including YYFree)
                YYFreeArray (plLocalVars.lpplYYFcnCurry); FreeYYFcn1 (plLocalVars.lpplYYFcnCurry); plLocalVars.lpplYYFcnCurry = NULL;
            } // End IF

            // If a curried index is in use
            if (plLocalVars.lpplYYIdxCurry)
            {
                // Free the function (including YYFree)
                YYFreeArray (plLocalVars.lpplYYIdxCurry); FreeYYFcn1 (plLocalVars.lpplYYIdxCurry); plLocalVars.lpplYYIdxCurry = NULL;
            } // End IF

            // If a curried left arg is in use, ...
            if (plLocalVars.lpplYYArgCurry
             && plLocalVars.lpplYYArgCurry->YYInuse)
            {
                // YYFree the curried left arg
                YYFree (plLocalVars.lpplYYArgCurry); plLocalVars.lpplYYArgCurry = NULL;
            } // End IF

            // Check for ResetFlag
            if (lpMemPTD->lpSISCur
             && lpMemPTD->lpSISCur->ResetFlag NE RESETFLAG_NONE)
            {
                // Set the exit type
                plLocalVars.ExitType = TranslateResetFlagToExitType (lpMemPTD->lpSISCur->ResetFlag);

                // Clear this flag so we don't execute []ELX
                bActOnErrors = FALSE;
            } else
                // Set the exit type
                plLocalVars.ExitType = EXITTYPE_ERROR;

            // Mark as in error
            uRet = 1;
            uError = ERRORCODE_ELX;

            goto PARSELINE_END;

PARSELINE_DONE:
            // If the current object is NoValue and EOS, ...
            if (IsTokenNoValue (&lpplYYCurObj->tkToken)
             && curSynObj EQ soEOS)
            {
                // YYFree the current object
                YYFree (lpplYYCurObj);
            } // End IF

            Assert (lpplOrgLftStk EQ &lpMemPTD->lpplLftStk[-1]);
            Assert (lpplOrgRhtStk EQ &lpMemPTD->lpplRhtStk[-1]);

            // N.B.:  DO NOT RELOAD lstSynObj as we are relying on the old value
#ifdef DEBUG2
            dprintfWL0 (L"Stmt Done:  curSynObj (%s), lstSynObj (%s)",
                        soNames[curSynObj],
                        soNames[lstSynObj]);
#endif
            // If we're restarting, ...
            if (plLocalVars.bRestart)
            {
                plLocalVars.bRestart = FALSE;

                goto PARSELINE_START;
            } // End IF

            // Set flag for assigned name
            bAssignName = 0
             || (curSynObj EQ soA    && lstSynObj EQ soSPA )
             || (curSynObj EQ soSA   && lstSynObj EQ soSPA )
             || (curSynObj EQ soF    && lstSynObj EQ soSPF )
             || (curSynObj EQ soNF   && lstSynObj EQ soSPNF)
             || (curSynObj EQ soMOP  && lstSynObj EQ soSPM )
             || (curSynObj EQ soMOPN && lstSynObj EQ soSPMN)
             || (curSynObj EQ soDOP  && lstSynObj EQ soSPD )
             || (curSynObj EQ soDOPN && lstSynObj EQ soSPDN)
             || (curSynObj EQ soHY   && lstSynObj EQ soSPHY);

            // Set flag for sink
            bSink = (curSynObj EQ soSPA) && !bAssignName;

            // Check for SYNTAX ERROR
            if ( curSynObj EQ soA
             ||  curSynObj EQ soSA
             ||  curSynObj EQ soNF
             ||  curSynObj EQ soEOS
             ||  curSynObj EQ soGO
             ||  curSynObj EQ soCS0
             ||  curSynObj EQ soCS1
             ||  curSynObj EQ soAFOG
             ||  curSynObj EQ soAFOR
/////////////|| (curSynObj EQ soA    && lstSynObj EQ soSPA )    // Already handled via curSynObj EQ soA
/////////////|| (curSynObj EQ soSA   && lstSynObj EQ soSPA )    // Already handled via curSynObj EQ soSA
             ||  bSink
             ||  bAssignName)
            {
                UBOOL bEOL;             // TRUE iff we're at the EOL (or equivalent)

                // If the current object is a var, ...
                if (IsTknTypeVar (lpplYYCurObj->tkToken.tkFlags.TknType))
                {
                    // If the var is in the process of stranding, ...
                    if (lpplYYCurObj->YYStranding)
                    {
                        // Turn this strand into a var
                        lpYYRes =
                          DbgMakeVarStrand_EM_YY (lpplYYCurObj);
                        // YYFree the current object
                        YYFree (lpplYYCurObj); lpplYYCurObj = NULL; curSynObj = soNONE;

                        // If not defined, ...
                        if (!lpYYRes)
                            goto PARSELINE_ERROR;

                        // Change the tkSynObj
                        lpYYRes->tkToken.tkSynObj = soA;

                        // Copy to the current object
                        lpplYYCurObj = lpYYRes; curSynObj = CURSYNOBJ; Assert (curSynObj <= soLAST);
                        lpYYRes = NULL;
                    } else
                        // Unstrand the current object if necessary
                        UnStrand (lpplYYCurObj);
                } // End IF

                // If the current token is an array or from sink, ...
                if (curSynObj EQ soA || bSink)
                {
                    HGLOBAL hGlbDfnHdr = NULL;      // UDFO/AFO global memory handle

                    // Get a ptr to the current SIS header
                    lpSISCur = lpMemPTD->lpSISCur;

                    // Do not display if caller is Execute or Quad
                    //   and the current statement is the last one on the line
                    if (IsLastStmt (&plLocalVars, lpplYYCurObj->tkToken.tkCharIndex)
                     && lpSISCur
                     && (lpSISCur->DfnType EQ DFNTYPE_EXEC
                      || (lpSISCur->DfnType EQ DFNTYPE_IMM
                       && lpSISCur->lpSISPrv NE NULL
                       && (lpSISCur->lpSISPrv->DfnType EQ DFNTYPE_EXEC
                        || lpSISCur->lpSISPrv->DfnType EQ DFNTYPE_QUAD))))
                        // Handle ArrExpr if caller is Execute or quad
                        ArrExprCheckCaller (&plLocalVars, lpSISCur, lpplYYCurObj, FALSE);
                    else
                    // Do not display if we're parsing an AFO, ...
                    if (hGlbDfnHdr = SISAfo (lpMemPTD))
                    {
                        // AFO display it
                        plLocalVars.bRet =
                          AfoDisplay_EM (&lpplYYCurObj->tkToken, FALSE, &plLocalVars, hGlbDfnHdr);
                    } else
                        plLocalVars.bRet =
                          ArrayDisplay_EM (&lpplYYCurObj->tkToken, TRUE, &plLocalVars.bCtrlBreak);

                    // Check for NoValue
                    if (IsTokenNoValue (&lpplYYCurObj->tkToken))
                        // Set the exit type
                        plLocalVars.ExitType = EXITTYPE_NOVALUE;
                    else
                    // Check for NoDisplay
                    if (lpplYYCurObj->tkToken.tkFlags.NoDisplay)
                        // Set the exit type
                        plLocalVars.ExitType = EXITTYPE_NODISPLAY;
                    else
                        // Set the exit type
                        plLocalVars.ExitType = EXITTYPE_DISPLAY;

                    // Free the current object
                    FreeResult (lpplYYCurObj);

                    // YYFree the current object
                    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; curSynObj = soNONE;
                } else
                // If the current object is a function, ...
                if (IsTknTypeFcnOpr (lpplYYCurObj->tkToken.tkFlags.TknType))
                {
                    // Free the function (including YYFree)
                    YYFreeArray (lpplYYCurObj); FreeYYFcn1  (lpplYYCurObj); lpplYYCurObj = NULL; curSynObj = soNONE;
                } else
                // If from an assigned name,
                //   or naked goto, ...
                if (bAssignName
                 || curSynObj EQ soGO)
                {
                    // YYFree the current object
                    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; curSynObj = soNONE;
#ifdef DEBUG
                } else
                {
                // If we didn't come from AFOR, EOS, {goto}, CS0, or CS1, ...
                if (curSynObj NE soAFOR
                 && curSynObj NE soEOS
                 && curSynObj NE soGO
                 && curSynObj NE soCS0
                 && curSynObj NE soCS1)
                    DbgStop ();     // Can this ever occur??
#endif
                } // End IF/ELSE/...

                // If there are no preceding tokens, ...
                if (plLocalVars.lptkEOS EQ plLocalVars.lptkNext)
                    // Skip to one past the EOS
                    plLocalVars.lptkNext++;

                // If the previous token is a NOP, ...
                if (plLocalVars.lptkNext[-1].tkFlags.TknType EQ TKT_NOP)
                    // Skip to the previous token (NOP) before which is (EOS/EOL)
                    plLocalVars.lptkNext--;

                Assert (plLocalVars.lptkNext[-1].tkFlags.TknType EQ TKT_EOS
                     || plLocalVars.lptkNext[-1].tkFlags.TknType EQ TKT_EOL
                     || plLocalVars.lptkNext[-1].tkFlags.TknType EQ TKT_LABELSEP);

                // If we're at the end of the line, ...
                bEOL = (plLocalVars.lptkNext[-1].tkFlags.TknType EQ TKT_EOL
                     || plLocalVars.bStopExec
                     || plLocalVars.bExec1Stmt
                     || ( plLocalVars.lptkNext[-1].tkFlags.TknType EQ TKT_LABELSEP
                      && &plLocalVars.lptkNext[-3] >= plLocalVars.lptkStart
                      &&  plLocalVars.lptkNext[-3].tkFlags.TknType EQ TKT_EOL));

                // If we're not at the end of the line, ...
                if (!bEOL)
                {
                    // Skip to the previous token (EOS/EOL)
                    plLocalVars.lptkNext--;

                    Assert (plLocalVars.lptkNext->tkFlags.TknType EQ TKT_EOS
                         || plLocalVars.lptkNext->tkFlags.TknType EQ TKT_EOL
                         || plLocalVars.lptkNext->tkFlags.TknType EQ TKT_LABELSEP);
                    // If we're at label separator, ...
                    if (plLocalVars.lptkNext->tkFlags.TknType EQ TKT_LABELSEP)
                    {
                        Assert (plLocalVars.lptkNext[-1].tkFlags.TknType EQ TKT_VARNAMED);

                        // Skip to the EOS/EOL token
                        plLocalVars.lptkNext -= 2;

                        Assert (plLocalVars.lptkNext->tkFlags.TknType EQ TKT_EOS
                             || plLocalVars.lptkNext->tkFlags.TknType EQ TKT_EOL);
                    } // End IF

                    // Skip to end of the current stmt
                    plLocalVars.lptkNext = &plLocalVars.lptkNext[plLocalVars.lptkNext->tkData.tkIndex];

                    Assert (plLocalVars.lptkNext->tkFlags.TknType EQ TKT_EOS
                         || plLocalVars.lptkNext->tkFlags.TknType EQ TKT_EOL);

                    // Save a ptr to the EOS/EOL token
                    plLocalVars.lptkEOS =   plLocalVars.lptkNext;

                    // And again to the end of the next stmt
                    plLocalVars.lptkNext = &plLocalVars.lptkNext[plLocalVars.lptkNext->tkData.tkIndex];

                    goto PARSELINE_START;
                } // End IF

                goto PARSELINE_END;
            } else
            // Check for VALUE ERROR
            if (curSynObj EQ soVALR)
                goto PARSELINE_VALUEERR;
            else
                goto PARSELINE_SYNTERR;

            //***************************************************************************
            //  End of 2by2 code
            //***************************************************************************
        } __except (CheckVirtAlloc (GetExceptionInformation (),
                                    L"ParseLine"))
        {} // End __try/__except
    } __except (CheckException (GetExceptionInformation (), L"ParseLine"))
    {
        // Split cases based upon the ExceptionCode
        switch (MyGetExceptionCode ())
        {
            case EXCEPTION_STACK_OVERFLOW:
                // Set the exit type
                plLocalVars.ExitType = EXITTYPE_STACK_FULL;

                break;

            case EXCEPTION_WS_FULL:
                // Set the exit type
                plLocalVars.ExitType = EXITTYPE_ERROR;

                // Mark as in error
                uRet = 1;
                uError = ERRORCODE_ELX;
                ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                           NULL);
                break;

            default:
                // Display message for unhandled exception
                DisplayException ();

                // Mark as in error
                uRet = 1;
                uError = ERRORCODE_ELX;

                goto NORMAL_EXIT;
        } // End SWITCH
    } // End __try/__except
PARSELINE_END:
    // In case we exited through an error, ...
    bPLBracket = FALSE;

    Assert (YYResIsEmpty ());

    if (!plLocalVars.bCtrlBreak
     && !plLocalVars.bLookAhead)
    // Split cases based upon the exit type
    switch (plLocalVars.ExitType)
    {
        case EXITTYPE_QUADERROR_INIT:
            // Set the reset flag
            lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_QUADERROR_INIT;

            break;

        case EXITTYPE_RESET_ALL:
            // Set the reset flag
            lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_ALL;

            break;

        case EXITTYPE_RESET_ONE:
            // Set the reset flag
            lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_ONE;

            break;

        case EXITTYPE_RESET_ONE_INIT:
            // Get a ptr to the current SIS header
            lpSISCur = lpMemPTD->lpSISCur;

            // Peel back to the first non-Exec layer
            while (lpSISCur->DfnType EQ DFNTYPE_EXEC)
               lpSISCur = lpSISCur->lpSISPrv;

            if (!lpSISCur->ItsEC)
                // Set the reset flag
                lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_ONE_INIT;

            break;

        case EXITTYPE_QUADERROR_EXEC:
            // Set the exit type
            plLocalVars.ExitType = EXITTYPE_ERROR;

            // Set the reset flag
            lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_NONE;

            // Fall through to common code

        case EXITTYPE_ERROR:        // Mark user-defined function/operator as suspended
        case EXITTYPE_STACK_FULL:   // ...
        case EXITTYPE_STOP:         // ...
            // If it's STACK FULL, ...
            if (plLocalVars.ExitType EQ EXITTYPE_STACK_FULL)
            {
                // Mark as in error
                uRet = 1;
                uError = ERRORCODE_NONE;
                ErrorMessageIndirectToken (ERRMSG_STACK_FULL APPEND_NAME,
                                           NULL);
                // Set the reset flag
                lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_STOP;
            } // End IF

            // Get a ptr to the current SIS header
            lpSISCur = lpMemPTD->lpSISCur;

            // If it's a permanent function (i.e. Magic Function/Operator),
            //   or an AFO,
            // but not STACK FULL, ...
            if ((lpSISCur->PermFn
              || plLocalVars.bAFO)
             && plLocalVars.ExitType NE EXITTYPE_STACK_FULL)
            {
                // Don't suspend at this level

                // Set the exit type
                plLocalVars.ExitType = EXITTYPE_QUADERROR_INIT;

                // Set the reset flag
                lpSISCur->ResetFlag  = RESETFLAG_QUADERROR_INIT;

                break;
            } // End IF

            // If this level or an adjacent preceding level is from
            //   Execute, immediate execution mode, or an AFO,
            //   peel back to the preceding level
            while (lpSISCur
                && (lpSISCur->DfnType EQ DFNTYPE_EXEC
                 || lpSISCur->DfnType EQ DFNTYPE_IMM
                 || lpSISCur->bAFO))
                lpSISCur = lpSISCur->lpSISPrv;

            // If this level is a user-defined function/operator,
            //   mark it as suspended
            if (lpSISCur
             && (lpSISCur->DfnType EQ DFNTYPE_OP1
              || lpSISCur->DfnType EQ DFNTYPE_OP2
              || lpSISCur->DfnType EQ DFNTYPE_FCN))
                lpSISCur->Suspended = TRUE;
            break;

        case EXITTYPE_DISPLAY:      // Nothing more to do with these types
        case EXITTYPE_NODISPLAY:    // ...
        case EXITTYPE_NOVALUE:      // ...
        case EXITTYPE_GOTO_ZILDE:   // ...
        case EXITTYPE_GOTO_LINE:    // ...
        case EXITTYPE_NONE:         // ...
            // Check on user-defined function/operator exit error
            if (CheckDfnExitError_EM (lpMemPTD))
            {
                // Mark as an APL error
                plLocalVars.ExitType = EXITTYPE_ERROR;
                uRet = 1;
            } // End IF

            break;

        case EXITTYPE_RETURNxLX:
            uRet = 0;

            break;

        defstop
            break;
    } // End IF/SWITCH

    // If Ctrl-Break was pressed, ...
    if (plLocalVars.bCtrlBreak)
    {
        // Mark as in error
        uError = ERRORCODE_ALX;

        // Reset the flag
        plLocalVars.bCtrlBreak = FALSE;

        // Peel back to first user-defined function/operator
        for (lpSISCur = lpMemPTD->lpSISCur;
             lpSISCur && lpSISCur NE lpMemPTD->lpSISNxt;
             lpSISCur = lpSISCur->lpSISPrv)
        if (!lpSISCur->PermFn
         && !lpSISCur->bAFO
         && (lpSISCur->DfnType EQ DFNTYPE_OP1
          || lpSISCur->DfnType EQ DFNTYPE_OP2
          || lpSISCur->DfnType EQ DFNTYPE_FCN))
            break;

        // Signal an error
        BreakMessage (hWndSM,
                      (lpSISCur && lpSISCur NE lpMemPTD->lpSISNxt) ? lpSISCur
                                                                   : NULL);
    } // End IF

    if (uRet EQ 0 || uError EQ ERRORCODE_ALX)
        goto NORMAL_EXIT;

    // If we're not resetting, ...
    if (lpMemPTD->lpSISCur->ResetFlag EQ RESETFLAG_NONE)
    {
        // If called from Immediate Execution/Execute, ...
        if (lpwszLine)
            // Create []DM & []EM
            ErrorMessageDirect (lpMemPTD->lpwszErrorMessage,    // Ptr to error message text
                                lpwszLine,                      // Ptr to the line which generated the error
                                plLocalVars.tkErrorCharIndex);  // Position of caret (origin-0)
        else
        {
            LPMEMTXT_UNION lpMemTxtLine;

            // Lock the memory to get a ptr to it
            lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

            // Create []DM & []EM
            ErrorMessageDirect (lpMemPTD->lpwszErrorMessage,    // Ptr to error message text
                               &lpMemTxtLine->C,                // Ptr to the line which generated the error
                                plLocalVars.tkErrorCharIndex);  // Position of caret (origin-0)
            // We no longer need this ptr
            MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;
        } // End IF/ELSE

        // If not already set, ...
        if (uError EQ ERRORCODE_NONE)
            // Mark as in error
            uError = ERRORCODE_ELX;
    } // End IF
NORMAL_EXIT:
    EnterCriticalSection (&CSOPL);

    // Unlink this plLocalVars from the chain of such objects
    lpMemPTD->lpPLCur = plLocalVars.lpPLPrev;

    // Transfer the Ctrl-Break flag in case it hasn't been acted upon
    if (lpMemPTD->lpPLCur)
        lpMemPTD->lpPLCur->bCtrlBreak = plLocalVars.bCtrlBreak;

    LeaveCriticalSection (&CSOPL);

    // Restore the previous value of dwTlsType
    TlsSetValue (dwTlsType, ULongToPtr (oldTlsType));

    // If there's an error to be signalled, ...
    if (uError NE ERRORCODE_NONE
     && bActOnErrors
     && lpMemPTD->lpSISCur->lpSISErrCtrl EQ NULL)
    {
        EXIT_TYPES exitType;        // Return code from PrimFnMonUpTackJotCSPLParse

#ifdef DEBUG
        // Split cases based upon the error code
        switch (uError)
        {
            case ERRORCODE_ALX:
                DbgMsgW2 (L"~~Start []ALX on");

                break;

            case ERRORCODE_ELX:
                DbgMsgW2 (L"~~Start []ELX on");

                break;

            case ERRORCODE_DM:
                DbgMsgW2 (L"~~Display []DM on");

                break;

            defstop
                break;
        } // End SWITCH
#endif
        // If there's an immediate text line, ...
        if (lpwszLine)
        {
            DbgMsgW2 (lpwszLine);

            // Create []DM & []EM
            ErrorMessageDirect (lpMemPTD->lpwszErrorMessage,    // Ptr to error message text
                                lpwszLine,                      // Ptr to the line which generated the error
                                plLocalVars.tkErrorCharIndex);  // Position of caret (origin-0)
        } else
        {
            LPMEMTXT_UNION lpMemTxtLine;

            // Lock the memory to get a ptr to it
            lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

            // Display the function line
            DbgMsgW2 (&lpMemTxtLine->C);

            // Create []DM & []EM
            ErrorMessageDirect (lpMemPTD->lpwszErrorMessage,    // Ptr to error message text
                               &lpMemTxtLine->C,                // Ptr to the line which generated the error
                                plLocalVars.tkErrorCharIndex);  // Position of caret (origin-0)
            // We no longer need this ptr
            MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;
        } // End IF

        // Set the text for the line
        // Split cases based upon the error code
        switch (uError)
        {
            case ERRORCODE_ALX:
                lpwszLine = WS_UTF16_UPTACKJOT $QUAD_ALX;
                bNoDepthCheck = FALSE;

                break;

            case ERRORCODE_ELX:
                lpwszLine = WS_UTF16_UPTACKJOT $QUAD_ELX;
                bNoDepthCheck = FALSE;

                break;

            case ERRORCODE_DM:
                lpwszLine = $QUAD_DM;
                bNoDepthCheck = TRUE;

                break;

            defstop
                break;
        } // End SWITCH

        // Split cases based upon the error code
        switch (uError)
        {
            case ERRORCODE_ALX:
            case ERRORCODE_ELX:
                // Execute the statement
                exitType =
                  PrimFnMonUpTackJotCSPLParse (hWndEC,          // Edit Ctrl window handle
                                               lpMemPTD,        // Ptr to PerTabData global memory
                                               lpwszLine,       // Ptr to text of line to execute
                                               lstrlenW (lpwszLine), // Length of the line to execute
                                               TRUE,            // TRUE iff we should act on errors
                                               bNoDepthCheck,   // TRUE iff we're to skip the depth check
                                               lpMemPTD->lpSISCur->DfnType, // DfnType for FillSISNxt
                                               NULL);           // Ptr to function token
                break;

            case ERRORCODE_DM:
                // Display []DM
                DisplayGlbArr_EM (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_DM]->stData.stGlbData,   // Global memory handle to display
                                  TRUE,                                                         // TRUE iff last line has CR
                                 &plLocalVars.bCtrlBreak,                                       // Ptr to Ctrl-Break flag
                                  NULL);                                                        // Ptr to function token
                // Return code as already displayed
                exitType = EXITTYPE_NODISPLAY;

                break;

            defstop
                break;
        } // End SWITCH

        // Split cases based upon the error code
        switch (uError)
        {
            case ERRORCODE_ALX:
                // If we exited normally, ...
                if (exitType EQ EXITTYPE_DISPLAY
                 || exitType EQ EXITTYPE_NODISPLAY
                 || exitType EQ EXITTYPE_NOVALUE)
                {
                    // Stop execution so we can display the break message
                    plLocalVars.ExitType =
                    exitType             = EXITTYPE_STOP;

                    // Set the reset flag
                    lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_STOP;

                    // If the Execute/Quad result is present, clear it
                    if (lpMemPTD->YYResExec.tkToken.tkFlags.TknType)
                    {
                        // Free the result
                        FreeResult (&lpMemPTD->YYResExec);

                        // We no longer need these values
                        ZeroMemory (&lpMemPTD->YYResExec, sizeof (lpMemPTD->YYResExec));
                    } // End IF
                } // End IF

                break;

            case ERRORCODE_ELX:
            case ERRORCODE_DM:
                break;

            defstop
                break;
        } // End SWITCH

        // Split cases based upon the exit type
        switch (exitType)
        {
            case EXITTYPE_GOTO_LINE:
            case EXITTYPE_RESET_ALL:
            case EXITTYPE_RESET_ONE:
            case EXITTYPE_RESET_ONE_INIT:
            case EXITTYPE_QUADERROR_INIT:
                // Pass on to caller
                plLocalVars.ExitType = exitType;

                break;

            case EXITTYPE_NODISPLAY:    // Display the result (if any)
            case EXITTYPE_DISPLAY:      // ...
                // If the Execute/Quad result is present, display it
                if (lpMemPTD->YYResExec.tkToken.tkFlags.TknType)
                {
                    HGLOBAL hGlbDfnHdr;                 // AFO DfnHdr global memory handle

                    // If it's not from executing []ALX,
                    //   and we're parsing an AFO, ...
                    if (uError NE ERRORCODE_ALX
                     && (hGlbDfnHdr = SISAfo (lpMemPTD)))
                        AfoDisplay_EM (&lpMemPTD->YYResExec.tkToken, FALSE, &plLocalVars, hGlbDfnHdr);
                    else
                        // Display the array
                        ArrayDisplay_EM (&lpMemPTD->YYResExec.tkToken, TRUE, &plLocalVars.bCtrlBreak);

                    // Free the result
                    FreeResult (&lpMemPTD->YYResExec);

                    // We no longer need these values
                    ZeroMemory (&lpMemPTD->YYResExec, sizeof (lpMemPTD->YYResExec));
                } // End IF

                // Fall through to common code

            case EXITTYPE_ERROR:        // Display the prompt unless called by Quad or User fcn/opr
            case EXITTYPE_STACK_FULL:   // ...
            case EXITTYPE_STOP:         // ...
            case EXITTYPE_NOVALUE:      // ...
                // Get a ptr to the current SIS header
                lpSISPrv =
                lpSISCur = lpMemPTD->lpSISCur;

                // If this level is an AFO, ...
                while (lpSISPrv
                    && lpSISPrv->bAFO)
                    // Peel back
                    lpSISPrv = lpSISPrv->lpSISPrv;

                // If this level is Immediate Execution Mode, ...
                while (lpSISPrv
                    && lpSISPrv->DfnType EQ DFNTYPE_IMM)
                    // Peel back
                    lpSISPrv = lpSISPrv->lpSISPrv;

                // If this level is Execute, skip the prompt
                //   as Execute will handle that
                if (lpSISPrv
                 && lpSISPrv->DfnType EQ DFNTYPE_EXEC)
                {
                    // If the result of {execute}[]xLX is a normal
                    //   result (EXITTYPE_NODISPLAY),
                    //   return  EXITTYPE_RETURNxLX so the caller
                    //   can avoid displaying a prompt until the
                    //   result of executing []xLX is handled
                    if (exitType EQ EXITTYPE_NODISPLAY)
                        exitType =  EXITTYPE_RETURNxLX;

                    // Pass on this exit type to the caller
                    plLocalVars.ExitType = exitType;
                } // End IF

                break;

            case EXITTYPE_GOTO_ZILDE:   // Nothing more to do with these
            case EXITTYPE_NONE:         // ...
                break;

            defstop
                break;
        } // End SWITCH
    } // End IF

    EnterCriticalSection (&CSOPL);

    // Restore the previous left & right stack ptrs
    lpMemPTD->lpplLftStk = lpplOrgLftStk;
    lpMemPTD->lpplRhtStk = lpplOrgRhtStk;

    // Restore the previous value of dwTlsPlLocalVars
    TlsSetValue (dwTlsPlLocalVars, oldTlsPlLocalVars);

    // Restore the previous executing state
    SetExecuting (lpMemPTD, bOldExecuting);

    // Decrement the depth counter
    lpMemPTD->uExecDepth--;

    LeaveCriticalSection (&CSOPL);

    return plLocalVars.ExitType;
} // End ParseLine
#undef  APPEND_NAME


//***************************************************************************
//  $pl_yylex
//
//  Lexical analyzer for Bison
//***************************************************************************

LPPL_YYSTYPE pl_yylex
    (LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
    Assert (!lpplLocalVars->bRestart);

    return _pl_yylex (lpplLocalVars, FALSE);
} // End pl_yylex


//***************************************************************************
//  $GetLftSynObj
//
//  Return the left stack SynObj value without popping the token stack
//***************************************************************************

SO_ENUM GetLftSynObj
    (LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
    LPPL_YYSTYPE lpplYYLval;        // Ptr to current object
    SO_ENUM      tkSynObj;          // The result

    if (lpplLocalVars->lptkStart >= lpplLocalVars->lptkNext)
        return soEOS;

    // Get the PL_YYSTYPE result
    lpplYYLval = _pl_yylex (lpplLocalVars, TRUE);

    if (lpplYYLval)
    {
        // Get the result
        tkSynObj = lpplYYLval->tkToken.tkSynObj;

        // YYFree the YYRes
        YYFree (lpplYYLval); lpplYYLval = NULL;
    } else
        // Return something harmless
        tkSynObj = soEOS;

    return tkSynObj;
} // End GetLftSynObj


//***************************************************************************
//  $_pl_yylex
//
//  Lexical analyzer for Bison
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- _pl_yylex"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE _pl_yylex
    (LPPLLOCALVARS lpplLocalVars,   // Ptr to local plLocalVars
     UBOOL         bRestoreStk)     // TRUE iff we restore the token stack upon exit

{
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPSIS_HEADER lpSISCur;          // Ptr to current SIS layer
    LPPL_YYSTYPE lpplYYLval;        // Ptr to current object
    LPTOKEN      lptkOrigNext;      // Ptr to original token stack
    LPDFN_HEADER lpMemDfnHdr;       // Ptr to UDFO/AFO header
    HGLOBAL      hGlbData;          // Global memory handle

    // Save the original token stack ptr
    lptkOrigNext = lpplLocalVars->lptkNext;

    // Get ptr to PerTabData global memory
    lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Allocate a new YYRes
    lpplYYLval = YYAlloc ();
PL_YYLEX_START:
    Assert (IsValidPtr (lpplLocalVars->lptkNext, sizeof (lpplLocalVars->lptkNext[0])));
    Assert ((*(LPUINT) lpplLocalVars->lptkNext) NE 0xfeeefeee);

    // Because we're parsing the stmt from right to left
    lpplLocalVars->lptkNext--;

#if (defined (DEBUG)) && (defined (YYLEX_DEBUG))
    dprintfWL9 (L"==pl_yylex:  TknType = %S, CharIndex = %d",
                 GetTokenTypeName (lpplLocalVars->lptkNext->tkFlags.TknType),
                 lpplLocalVars->lptkNext->tkCharIndex);
#endif

    // Return the current token
    lpplYYLval->tkToken         = *lpplLocalVars->lptkNext;

    // Initialize the rest of the fields
////lpplYYLval->TknCount        =               // Already zero from ZeroMemory
////lpplYYLval->YYInuse         =               // ...
////lpplYYLval->YYIndirect      = 0;            // ...
////lpplYYLval->YYCopyArray     = FALSE;        // ...
////lpplYYLval->YYStranding     = FALSE;        // ...
////lpplYYLval->lpYYFcnBase     = NULL;         // ...
////lpplYYLval->lpYYStrandBase  = NULL;         // ...

#ifdef DEBUG
    // If we should not restore the token stack ptr, ...
    if (!bRestoreStk)
        nop ();
#endif
    // Split cases based upon the token type
    switch (lpplLocalVars->lptkNext->tkFlags.TknType)
    {
        case TKT_VARIMMED:
        case TKT_VARARRAY:
            break;

        case TKT_INPOUT:
            break;

        case TKT_FCNNAMED:
PL_YYLEX_FCNNAMED:
            if (IsTknNamedFcnOpr (&lpplYYLval->tkToken)             // Is a named function/operator
             && !lpplYYLval->tkToken.tkData.tkSym->stFlags.FcnDir)  // Not a direct fcn/opr
            {
                // If it's an immediate function, ...
                if (lpplYYLval->tkToken.tkData.tkSym->stFlags.Imm)
                {
                    // If the function is a hybrid, ...
                    if (lpplYYLval->tkToken.tkData.tkSym->stFlags.ImmType EQ IMMTYPE_PRIMOP3)
                        // Mark is as a hybrid
                        lpplYYLval->tkToken.tkSynObj = soHY;
                } else
                // If it's a niladic UDFO, ...
                if (lpplYYLval->tkToken.tkData.tkSym->stFlags.UsrDfn
                 && lpplYYLval->tkToken.tkData.tkSym->stFlags.stNameType EQ NAMETYPE_FN0)
                {
                    // If we should not restore the token stack ptr, ...
                    if (!bRestoreStk)
                    {
                        LPPL_YYSTYPE lpYYFcn;           // Ptr to temp fcn

                        // Get the global memory handle
                        hGlbData = lpplYYLval->tkToken.tkData.tkSym->stData.stGlbData;

                        // Ensure this is not an AFO

                        // Lock the memory to get a ptr to it
                        lpMemDfnHdr = MyGlobalLock (hGlbData);

                        // If it's not an AFO, ...
                        if (!lpMemDfnHdr->bAFO)
                        {
                            // The result is always the root of the function tree
                            lpYYFcn =
                              DbgPushFcnStrand_YY (lpplYYLval,
                                                   1,
                                                   (IsFcnStrDirect (lpplYYLval)) ? DIRECT
                                                                                 : INDIRECT); // Named function
                            // YYFree the current object
                            YYFree (lpplYYLval); lpplYYLval = NULL;

                            // If not defined, ...
                            if (!lpYYFcn)
                                goto ERROR_EXIT;

                            // Copy to the current object
                            lpplYYLval = lpYYFcn;
                            lpYYFcn = NULL;
                } // End IF

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbData); lpMemDfnHdr = NULL;
                    } // End IF
                } // End IF
////////////////{
////////////////    // If we should not restore the token stack ptr, ...
////////////////    if (!bRestoreStk)
////////////////        nop ();
////////////////
////////////////    // Convert to an unnamed function
////////////////    lpplYYLval->tkToken.tkFlags.TknType  = TKT_FCNAFO;
////////////////    lpplYYLval->tkToken.tkData.tkGlbData = lpplYYLval->tkToken.tkData.tkSym->stData.stGlbData;
////////////////} else
////            {
////                // If we should not restore the token stack ptr, ...
////                if (!bRestoreStk)
////                {
////                    LPPL_YYSTYPE lpYYFcn;           // Ptr to temp fcn
////
////                    // Get the global memory handle
////                    hGlbData = lpplYYLval->tkToken.tkData.tkSym->stData.stGlbData;
////
////                    // Ensure this is not an AFO
////
////                    // Lock the memory to get a ptr to it
////                    lpMemDfnHdr = MyGlobalLock (hGlbData);
////
////                    // If it's not an AFO, ...
////                    if (!lpMemDfnHdr->bAFO)
////                    {
////                        // The result is always the root of the function tree
////                        lpYYFcn =
////                          DbgPushFcnStrand_YY (lpplYYLval,
////                                               1,
////                                               (IsFcnStrDirect (lpplYYLval)) ? DIRECT
////                                                                             : INDIRECT); // Named function
////                        // YYFree the current object
////                        YYFree (lpplYYLval); lpplYYLval = NULL;
////
////                        // If not defined, ...
////                        if (!lpYYFcn)
////                            goto ERROR_EXIT;
////
////                        // Copy to the current object
////                        lpplYYLval = lpYYFcn;
////                        lpYYFcn = NULL;
////                    } // End IF
////
////                    // We no longer need this ptr
////                    MyGlobalUnlock (hGlbData); lpMemDfnHdr = NULL;
////                } // End IF
////            } // End IF/ELSE
            } // End IF

            break;

        case TKT_VARNAMED:
        {
            STFLAGS stFlags;        // STE flags

            // Get the STE flags
            stFlags = lpplLocalVars->lptkNext->tkData.tkSym->stFlags;

            // Split cases based upon the NAMETYPE_xxx
            switch (stFlags.stNameType)
            {
                case NAMETYPE_UNK:
                    if (stFlags.DfnSysLabel)
                        // Mark it as an Array so it can't be re-assigned.
                        lpplYYLval->tkToken.tkSynObj = soA;
                    else
                    if (lpplLocalVars->lptkNext->tkFlags.bAssignName)
                        // Mark it as named as it'll be re-assigned shortly
                        lpplYYLval->tkToken.tkSynObj = soNAM;
                    else
                        // Mark is as a VALUE ERROR
                        lpplYYLval->tkToken.tkSynObj = soVALR;
                    break;

                case NAMETYPE_VAR:
                    // Late catch of NAM F {is} A
                    if (lpplLocalVars->lpMemPTD->lpplRhtStk[-1]->tkToken.tkSynObj EQ soSPA)
                        // Mark as being assigned into
                        lpplYYLval->tkToken.tkFlags.bAssignName = TRUE;

                    // If we should not restore the token stack ptr,
                    //   and if we're not assigning into this name,
                    //   and we're not in the middle of "for I :in", ...
                    if (!bRestoreStk
                     && !lpplYYLval->tkToken.tkFlags.bAssignName
                     &&  lpplLocalVars->lptkNext[1].tkFlags.TknType NE TKT_CS_IN)
                    {
                        // If it's an immediate, ...
                        if (lpplYYLval->tkToken.tkData.tkSym->stFlags.Imm)
                        {
                            // Convert this to an unnamed immediate var
                            lpplYYLval->tkToken.tkFlags.TknType   = TKT_VARIMMED;
                            lpplYYLval->tkToken.tkFlags.ImmType   = lpplYYLval->tkToken.tkData.tkSym->stFlags.ImmType;
                            lpplYYLval->tkToken.tkData .tkLongest = lpplYYLval->tkToken.tkData.tkSym->stData.stLongest;
                        } else
                        {
                            // Convert this to an unnamed global var
                            lpplYYLval->tkToken.tkFlags.TknType   = TKT_VARARRAY;
                            lpplYYLval->tkToken.tkData .tkGlbData = lpplYYLval->tkToken.tkData.tkSym->stData.stGlbData;

                            // Increment the refcnt
                            IncrRefCntTkn (&lpplYYLval->tkToken);
                        } // End IF/ELSE

                        // Mark it as an Array so it can't be re-assigned.
                        lpplYYLval->tkToken.tkSynObj = soA;
                    } else
                        lpplYYLval->tkToken.tkSynObj = soNAM;

                    break;

                case NAMETYPE_FN0:
                    // Call this one TKT_FCNNAMED
                    lpplYYLval->tkToken.tkFlags.TknType = TKT_FCNNAMED;
                    lpplYYLval->tkToken.tkSynObj = soNF;

                    // If we're not assigning into this name,
                    //   and we're not in the middle of "for I :in", ...
                    if (!lpplYYLval->tkToken.tkFlags.bAssignName
                     &&  lpplLocalVars->lptkNext[1].tkFlags.TknType NE TKT_CS_IN)
                        goto PL_YYLEX_FCNNAMED;
                    break;

                case NAMETYPE_FN12:
                    // Call this one TKT_FCNNAMED
                    lpplYYLval->tkToken.tkFlags.TknType = TKT_FCNNAMED;

                    // If it's not an immediate, ...
                    // If the function is not direct, ...
                    if (!lpplYYLval->tkToken.tkData.tkSym->stFlags.Imm
                     && !lpplYYLval->tkToken.tkData.tkSym->stFlags.FcnDir)
                    {
                        HGLOBAL hGlbUDFO;

                        // Get the global handle
                        hGlbUDFO = lpplYYLval->tkToken.tkData.tkSym->stData.stGlbData;

                        // Set the UDFO/AFO properties
                        plSetDfn (&lpplYYLval->tkToken, hGlbUDFO);

                        Assert (lpplYYLval->tkToken.tkSynObj EQ soF);
                    } else
                        lpplYYLval->tkToken.tkSynObj = soF;

                    // If we're not assigning into this name,
                    //   and we're not in the middle of "for I :in", ...
                    if (!lpplYYLval->tkToken.tkFlags.bAssignName
                     &&  lpplLocalVars->lptkNext[1].tkFlags.TknType NE TKT_CS_IN)
                        goto PL_YYLEX_FCNNAMED;
                    break;

                case NAMETYPE_OP1:
                    // Call this one TKT_OP1NAMED
                    lpplYYLval->tkToken.tkFlags.TknType = TKT_OP1NAMED;

                    // If we're not assigning into this name,
                    //   and we're not in the middle of "for I :in", ...
                    if (!lpplYYLval->tkToken.tkFlags.bAssignName
                     &&  lpplLocalVars->lptkNext[1].tkFlags.TknType NE TKT_CS_IN)
                        goto PL_YYLEX_NAMEOP1;
                    break;

                case NAMETYPE_OP2:
                    // Call this one TKT_OP2NAMED
                    lpplYYLval->tkToken.tkFlags.TknType = TKT_OP2NAMED;

                    // If we're not assigning into this name,
                    //   and we're not in the middle of "for I :in", ...
                    if (!lpplYYLval->tkToken.tkFlags.bAssignName
                     &&  lpplLocalVars->lptkNext[1].tkFlags.TknType NE TKT_CS_IN)
                        goto PL_YYLEX_NAMEOP2;
                    break;

                case NAMETYPE_OP3:
                    // Call this one TKT_OP3NAMED
                    lpplYYLval->tkToken.tkFlags.TknType = TKT_OP3NAMED;

                    // If we're not assigning into this name,
                    //   and we're not in the middle of "for I :in", ...
                    if (!lpplYYLval->tkToken.tkFlags.bAssignName
                     &&  lpplLocalVars->lptkNext[1].tkFlags.TknType NE TKT_CS_IN)
                        goto PL_YYLEX_NAMEOP3;
                    break;

                case NAMETYPE_TRN:
                    // Call this one TKT_FCNNAMED
                    lpplYYLval->tkToken.tkFlags.TknType = TKT_FCNNAMED;

                    lpplYYLval->tkToken.tkSynObj = soF;

                    // If we're not assigning into this name,
                    //   and we're not in the middle of "for I :in", ...
                    if (!lpplYYLval->tkToken.tkFlags.bAssignName
                     &&  lpplLocalVars->lptkNext[1].tkFlags.TknType NE TKT_CS_IN)
                        goto PL_YYLEX_FCNNAMED;
                    break;

                case NAMETYPE_LST:
                defstop
                    break;
            } // End SWITCH

            // If the name is assigned into, or
            //   the next token is TKT_CS_IN, ...
            if (lpplLocalVars->lptkNext->tkFlags.bAssignName
             || lpplLocalVars->lptkNext[1].tkFlags.TknType EQ TKT_CS_IN)
                // Make it a NAME
                lpplYYLval->tkToken.tkSynObj = soNAM;

            break;
        } // End TKT_VARNAMED

        case TKT_ASSIGN:
            // If the next token is a right paren, we're starting Selective Specification
            if (lpplLocalVars->lptkNext[-1].tkFlags.TknType EQ TKT_RIGHTPAREN)
            {
                // If we're already doing Selective Specification, signal an error
                if (lpplLocalVars->bSelSpec)
                {
                    break;          // ***FINISHME***
                } // End IF

                // Mark as doing Selective Specification
                lpplLocalVars->bSelSpec =
                lpplLocalVars->bIniSpec = TRUE;
            } // End IF

            Assert (lpplYYLval->tkToken.tkSynObj EQ soSP);

            break;

        case TKT_LISTSEP:           // List separator
            Assert (lpplYYLval->tkToken.tkSynObj EQ soSEMI);

            break;

        case TKT_FCNAFO:            // Anonymous function
            Assert (lpplYYLval->tkToken.tkSynObj EQ soF);

            break;

        case TKT_OP1AFO:            // Anonymous monadic operator
            Assert (lpplYYLval->tkToken.tkSynObj EQ soMOP);

            break;

        case TKT_OP2AFO:            // Anonymous dyadic operator
            Assert (lpplYYLval->tkToken.tkSynObj EQ soDOP);

            break;

        case TKT_AFOGUARD:          // AFO guard
            break;

        case TKT_AFORETURN:         // AFO return
            break;

        case TKT_DELAFO:            // Del Anon -- either a function, or a monadic or dyadic operator, bound to its operands
            break;

        case TKT_DEL:               // Del -- always a function
            // Search up the SIS chain to see what this is
            lpSISCur = SrchSISForDfn (lpMemPTD);

            // If the ptr is valid, ...
            if (lpSISCur)
            {
                // Fill in the ptr to the function header
                //   in both the return value and the token stream
                     lpplYYLval->tkToken.tkData.tkVoid   =
                lpplLocalVars->lptkNext->tkData.tkVoid   = MakePtrTypeGlb (lpSISCur->hGlbDfnHdr);
                     lpplYYLval->tkToken.tkFlags.TknType =
                lpplLocalVars->lptkNext->tkFlags.TknType = TKT_DELAFO;
                     lpplYYLval->tkToken.tkSynObj        =
                lpplLocalVars->lptkNext->tkSynObj        = soF;
            } else
            {
                // Mark it as a SYNTAX ERROR
                // Change the token type
                lpplYYLval->tkToken.tkSynObj = soSYNR;

                break;
            } // End IF/ELSE

            break;

        case TKT_DELDEL:            // Del Del -- either a monadic or dyadic operator
            // Search up the SIS chain to see what this is
            lpSISCur = SrchSISForDfn (lpMemPTD);

            // If the ptr is valid, ...
            if (lpSISCur)
            {
                // Fill in the ptr to the function header
                //   in both the return value and the token stream
                     lpplYYLval->tkToken.tkData.tkVoid =
                lpplLocalVars->lptkNext->tkData.tkVoid = MakePtrTypeGlb (lpSISCur->hGlbDfnHdr);

                // Split case based upon the function type
                switch (lpSISCur->DfnType)
                {
////////////////////case DFNTYPE_FCN:
////////////////////    // Change it into an anonymous function
////////////////////    //   in both the return value and the token stream
////////////////////         lpplYYLval->tkToken.tkFlags.TknType =
////////////////////    lpplLocalVars->lptkNext->tkFlags.TknType = TKT_FCNAFO;
////////////////////    lpplLocalVars->lptkNext->tkFlags.TknType = TKT_FCNAFO;
////////////////////         lpplYYLval->tkToken.tkSynObj        =
////////////////////    lpplLocalVars->lptkNext->tkSynObj        = soF;
////////////////////
////////////////////    break
////////////////////
                    case DFNTYPE_OP1:
                        // Change it into an anonymous monadic operator
                        //   in both the return value and the token stream
                             lpplYYLval->tkToken.tkFlags.TknType =
                        lpplLocalVars->lptkNext->tkFlags.TknType = TKT_OP1AFO;
                             lpplYYLval->tkToken.tkSynObj        =
                        lpplLocalVars->lptkNext->tkSynObj        = soMOP;

                        break;

                    case DFNTYPE_OP2:
                        // Change it into an anonymous dyadic operator
                        //   in both the return value and the token stream
                             lpplYYLval->tkToken.tkFlags.TknType =
                        lpplLocalVars->lptkNext->tkFlags.TknType = TKT_OP2AFO;
                             lpplYYLval->tkToken.tkSynObj        =
                        lpplLocalVars->lptkNext->tkSynObj        = soDOP;

                        break;

                    default:
                        // Mark it as a SYNTAX ERROR
                        // Change the token type
                        lpplYYLval->tkToken.tkSynObj = soSYNR;

                        break;
                } // End SWITCH
            } else
            {
                // Mark it as a SYNTAX ERROR
                // Change the token type
                lpplYYLval->tkToken.tkSynObj = soSYNR;

                break;
            } // End IF/ELSE

            break;

        case TKT_SETALPHA:          // Set {alpha}
            Assert (lpplLocalVars->lptkNext[1].tkFlags.TknType EQ TKT_SOS);

            // If {alpha} has a value, ...
            if (lpplYYLval->tkToken.tkData.tkSym->stFlags.Value)
                // Back off to the token to the right of the EOS/EOL
                //   so as to skip this stmt entirely
                lpplLocalVars->lptkNext -= (lpplLocalVars->lptkNext[1].tkData.tkIndex - 2);

            goto PL_YYLEX_START;    // Go around again

        case TKT_FCNIMMED:
            Assert (lpplYYLval->tkToken.tkSynObj EQ soF
                 || lpplYYLval->tkToken.tkSynObj EQ soGO);
            break;

        case TKT_SOS:
        case TKT_LINECONT:
        case TKT_NOP:               // NOP
            goto PL_YYLEX_START;    // Ignore these tokens

        case TKT_CHRSTRAND:
            Assert (lpplYYLval->tkToken.tkSynObj EQ soA);

            break;

        case TKT_NUMSTRAND:
        case TKT_NUMSCALAR:
            Assert (lpplYYLval->tkToken.tkSynObj EQ soA);

            break;

        case TKT_OP1NAMED:
PL_YYLEX_NAMEOP1:
            Assert (GetPtrTypeDir (lpplYYLval->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's NOT an immediate value, ...
            if (!lpplYYLval->tkToken.tkData.tkSym->stFlags.Imm)
            {
                // Lock the memory to get a ptr to it
                lpMemDfnHdr = MyGlobalLock (lpplYYLval->tkToken.tkData.tkSym->stData.stGlbData);

                // Check the function valence
                if (lpMemDfnHdr->FcnValence EQ FCNVALENCE_NIL)
                    lpplYYLval->tkToken.tkSynObj = soMOPN;
                else
                    lpplYYLval->tkToken.tkSynObj = soMOP;

                // We no longer need this ptr
                MyGlobalUnlock (lpplYYLval->tkToken.tkData.tkSym->stData.stGlbData); lpMemDfnHdr = NULL;
            } else
                // Set the tkSynObj
                lpplYYLval->tkToken.tkSynObj = soMOP;

            // If the name is assigned into, ...
            if (lpplLocalVars->lptkNext->tkFlags.bAssignName)
                lpplYYLval->tkToken.tkSynObj = soNAM;

            break;

        case TKT_OP2NAMED:
PL_YYLEX_NAMEOP2:
            Assert (GetPtrTypeDir (lpplYYLval->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's NOT an immediate value, ...
            if (!lpplYYLval->tkToken.tkData.tkSym->stFlags.Imm)
            {
                // Lock the memory to get a ptr to it
                lpMemDfnHdr = MyGlobalLock (lpplYYLval->tkToken.tkData.tkSym->stData.stGlbData);

                // Check the function valence
                if (lpMemDfnHdr->FcnValence EQ FCNVALENCE_NIL)
                    lpplYYLval->tkToken.tkSynObj = soDOPN;
                else
                    lpplYYLval->tkToken.tkSynObj = soDOP;

                // We no longer need this ptr
                MyGlobalUnlock (lpplYYLval->tkToken.tkData.tkSym->stData.stGlbData); lpMemDfnHdr = NULL;
            } else
                // Set the tkSynObj
                lpplYYLval->tkToken.tkSynObj = soDOP;

            // If the name is assigned into, ...
            if (lpplLocalVars->lptkNext->tkFlags.bAssignName)
                lpplYYLval->tkToken.tkSynObj = soNAM;

            break;

        case TKT_OP3NAMED:
PL_YYLEX_NAMEOP3:
            Assert (GetPtrTypeDir (lpplYYLval->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

            // Convert the named OP3 to an immediate OP3
            lpplYYLval->tkToken.tkFlags.TknType = TKT_OP3IMMED;
            lpplYYLval->tkToken.tkFlags.ImmType = IMMTYPE_PRIMOP3;
            lpplYYLval->tkToken.tkSynObj        = soHY;
            lpplYYLval->tkToken.tkData.tkChar   = lpplYYLval->tkToken.tkData.tkSym->stData.stChar;

            // If the name is assigned into, ...
            if (lpplLocalVars->lptkNext->tkFlags.bAssignName)
                lpplYYLval->tkToken.tkSynObj = soNAM;

            goto PL_YYLEX_OP3IMMED;

        case TKT_OP1IMMED:
            Assert (lpplYYLval->tkToken.tkSynObj EQ soMOP);

            break;

        case TKT_OP2IMMED:
            // Split cases based upon the operator symbol
            switch (lpplYYLval->tkToken.tkData.tkChar)
            {
                case UTF16_JOT:
                    // If either the next or previous token is OP2NAMED,
                    //   or the previous token is OP1NAMED, ...
                    if (lpplLocalVars->lptkNext[-1].tkFlags.TknType EQ TKT_OP2NAMED
                     || lpplLocalVars->lptkNext[ 1].tkFlags.TknType EQ TKT_OP2NAMED
                     || lpplLocalVars->lptkNext[ 1].tkFlags.TknType EQ TKT_OP1NAMED

                    // If the previous token is VARNAMED and the name type is OP1 or OP2, ...
                     || (lpplLocalVars->lptkNext[ 1].tkFlags.TknType EQ TKT_VARNAMED
                      && (lpplLocalVars->lptkNext[ 1].tkData.tkSym->stFlags.stNameType EQ NAMETYPE_OP1
                       || lpplLocalVars->lptkNext[ 1].tkData.tkSym->stFlags.stNameType EQ NAMETYPE_OP2))

                    // If the next     token is VARNAMED and the name type is OP2, ...
                     || (lpplLocalVars->lptkNext[-1].tkFlags.TknType EQ TKT_VARNAMED
                      && lpplLocalVars->lptkNext[-1].tkData.tkSym->stFlags.stNameType EQ NAMETYPE_OP2)

                    // If the previous token is OP2IMMED and the value is not JOT, ...
                     || (lpplLocalVars->lptkNext[ 1].tkFlags.TknType EQ TKT_OP2IMMED
                      && lpplLocalVars->lptkNext[ 1].tkData.tkChar NE UTF16_JOT)

                    // If the next     token is OP2IMMED and the value is not JOT, ...
                     || (lpplLocalVars->lptkNext[-1].tkFlags.TknType EQ TKT_OP2IMMED
                      && lpplLocalVars->lptkNext[-1].tkData.tkChar NE UTF16_JOT)

                    // If the previous token is OP1IMMED, ...
                     || (lpplLocalVars->lptkNext[ 1].tkFlags.TknType EQ TKT_OP1IMMED))
                    {
                        // Change the token type
                        lpplYYLval->tkToken.tkFlags.TknType = TKT_FILLJOT;
                        lpplYYLval->tkToken.tkSynObj = soF;
                    } // End IF

                    break;

                default:
                    break;
            } // End SWITCH

            break;

        case TKT_OP3IMMED:
PL_YYLEX_OP3IMMED:
            break;

        case TKT_OPJOTDOT:
            break;

        case TKT_LEFTPAREN:
            break;

        case TKT_RIGHTPAREN:
            break;

        case TKT_LEFTBRACKET:
            break;

        case TKT_RIGHTBRACKET:
            break;

        case TKT_LABELSEP:
            // Fall through to common code

        case TKT_EOS:
            break;

        case TKT_EOL:
            break;

        case TKT_COLON:
            break;

        case TKT_LEFTBRACE:
            break;

        case TKT_RIGHTBRACE:
            // Point to the right brace token
            lpplYYLval->lptkRhtBrace  =  lpplLocalVars->lptkNext;

            // Point to the matching left brace token
            lpplYYLval->lptkLftBrace  = &lpplLocalVars->lptkStart[lpplYYLval->lptkRhtBrace->tkData.tkIndex];

            // If we should not restore the token stack ptr, ...
            if (!bRestoreStk)
            {
                MakeAfo_EM_YY (lpplYYLval, lpplLocalVars);

                // Point the next token to one to the right of the matching left brace
                //   so that the next token we process is the left brace
                lpplLocalVars->lptkNext = &lpplYYLval->lptkLftBrace[1];
            } // End IF

            break;

        case TKT_SYS_NS:
            break;

        case TKT_CS_ANDIF:          // Control structure:  ANDIF
            break;

        case TKT_CS_ASSERT:         // Control Structure:  ASSERT
            break;

        case TKT_CS_CASE:           // Control Structure:  CASE
            break;

        case TKT_CS_CASELIST:       // Control Structure:  CASELIST
            break;

        case TKT_CS_CONTINUE:       // Control Structure:  CONTINUE
            break;

        case TKT_CS_CONTINUEIF:     // Control Structure:  CONTINUEIF
            break;

        case TKT_CS_ELSE:           // Control Structure:  ELSE
            break;

        case TKT_CS_ELSEIF:         // Control Structure:  ELSEIF
            break;

        case TKT_CS_ENDFOR:         // Control Structure:  ENDFOR
            break;

        case TKT_CS_ENDFORLCL:      // Control Structure:  ENDFORLCL
            break;

        case TKT_CS_ENDREPEAT:      // Control Structure:  ENDREPEAT
            break;

        case TKT_CS_ENDWHILE:       // Control Structure:  ENDWHILE
            break;

        case TKT_CS_FOR:            // Control Structure:  FOR
            break;

        case TKT_CS_FORLCL:         // Control Structure:  FORLCL
            break;

        case TKT_CS_GOTO:           // Control Structure:  GOTO
            break;

        case TKT_CS_IF:             // Control Structure:  IF
            break;

        case TKT_CS_IN:             // Control Structure:  IN
            break;

        case TKT_CS_LEAVE:          // Control Structure:  LEAVE
            break;

        case TKT_CS_LEAVEIF:        // Control Structure:  LEAVEIF
            break;

        case TKT_CS_ORIF:           // Control Structure:  ORIF
            break;

        case TKT_CS_RETURN:         // Control Structure:  RETURN
            break;

        case TKT_CS_SELECT:         // Control Structure:  SELECT
            break;

        case TKT_CS_SKIPCASE:       // Control Structure:  Special token
            break;

        case TKT_CS_SKIPEND:        // Control Structure:  Special token
            break;

        case TKT_CS_UNTIL:          // Control Structure:  UNTIL
            break;

        case TKT_CS_WHILE:          // Control Structure:  WHILE
            break;

        case TKT_CS_REPEAT:         // Control Structure:  REPEAT
        case TKT_CS_ENDIF:          // Control Structure:  ENDIF
        case TKT_CS_ENDSELECT:      // ...                 ENDSELECT
        case TKT_CS_FOR2:           // ...                 FOR2
        case TKT_CS_IF2:            // ...                 IF2
        case TKT_CS_REPEAT2:        // ...                 REPEAT2
        case TKT_CS_SELECT2:        // ...                 SELECT2
        case TKT_CS_WHILE2:         // ...                 WHILE2
        case TKT_GLBDFN:            // Placeholder for hGlbDfnHdr
            goto PL_YYLEX_START;    // Ignore these tokens

        case TKT_SYNTERR:           // Syntax Error
            // Change the token type
            lpplYYLval->tkToken.tkSynObj = soSYNR;

            break;

        case TKT_CS_END:            // Control Structure:  END
        defstop
            break;
    } // End SWITCH

    // If we should restore the token stack ptr, ...
    if (bRestoreStk)
        lpplLocalVars->lptkNext = lptkOrigNext;

    return lpplYYLval;

ERROR_EXIT:
    // If the result is defined, ...
    if (lpplYYLval)
    {
        // YYFree the result
        YYFree (lpplYYLval); lpplYYLval = NULL;
    } // End IF

    return NULL;
} // End _pl_yylex
#undef  APPEND_NAME


#ifdef DEBUG
//***************************************************************************
//  $PadMemoryW
//
//  Padd out memory to a specified multiple
//***************************************************************************

void PadMemoryW
    (LPWCHAR lpwszTemp,         // Original location
     APLUINT uLen)              // Length of original location

{
    // Skip to the end of the name
    lpwszTemp += uLen;

    // Use modulus
    uLen = SONAMES_MAXLEN - (uLen % SONAMES_MAXLEN);

    while (uLen--)
        *lpwszTemp++ = L' ';
} // End PadMemoryW
#endif


#ifdef DEBUG
//***************************************************************************
//  $LSTACK
//
//  Display the left stack
//***************************************************************************

LPWCHAR LSTACK
    (LPPLLOCALVARS lpplLocalVars,   // Ptr to local plLocalVars
     LPPL_YYSTYPE *lpplOrgLftStk)   // Ptr to start of left stack

{
    LPPERTABDATA  lpMemPTD;                 // Ptr to PerTabData global memory
    static  WCHAR wszTemp[100 * SONAMES_MAXLEN + 1] = {0};
    LPWCHAR       lpwszTemp,
                  lpwszEnd,
                  lpwszName;
    LPTOKEN       lptkNext;
    LPPL_YYSTYPE *lplpYYNext;
    SO_ENUM       tkSynObj;

    // Get the PerTabData ptr
    lpMemPTD = lpplLocalVars->lpMemPTD;

    // Get the ptr to the end of the stack text
    lpwszEnd  =
    lpwszTemp = &wszTemp[countof (wszTemp) - 2];

    // Get the next left stack ptr
    lplpYYNext = lpMemPTD->lpplLftStk;

    // Loop through the left stack (in memory)
    while (--lplpYYNext > lpplOrgLftStk)
    {
        // Get the token's syntax object
        tkSynObj = (*lplpYYNext)->tkToken.tkSynObj;

        Assert (tkSynObj <= soLAST);

        if (tkSynObj EQ soEOS)
            break;

        // Back off to the next text location
        lpwszTemp -= SONAMES_MAXLEN;

        // Get ptr to name
        lpwszName = soNames[tkSynObj];

        // Append the next name
        lstrcpyW (lpwszTemp, lpwszName);

        // Pad the name with spaces to a multiple of SONAMES_MAXLEN
        PadMemoryW (lpwszTemp, lstrlenW (lpwszName));
    } // End WHILE

    // Get the next left stack ptr
    lptkNext = lpplLocalVars->lptkNext;

    // Loop through the left stack (from pl_yylex)
    while (lpplLocalVars->lptkEOS < --lptkNext)
    // Filter out various tokens
    if (TRUE
     && lptkNext->tkFlags.TknType NE TKT_CS_END
     && lptkNext->tkFlags.TknType NE TKT_CS_ENDFOR
     && lptkNext->tkFlags.TknType NE TKT_CS_ENDFORLCL
     && lptkNext->tkFlags.TknType NE TKT_CS_ENDIF
     && lptkNext->tkFlags.TknType NE TKT_CS_ENDREPEAT
     && lptkNext->tkFlags.TknType NE TKT_CS_ENDSELECT
     && lptkNext->tkFlags.TknType NE TKT_CS_ENDWHILE
     && lptkNext->tkFlags.TknType NE TKT_CS_FOR2
     && lptkNext->tkFlags.TknType NE TKT_CS_IF2
     && lptkNext->tkFlags.TknType NE TKT_CS_REPEAT2
     && lptkNext->tkFlags.TknType NE TKT_CS_SELECT2
     && lptkNext->tkFlags.TknType NE TKT_CS_SKIPCASE
     && lptkNext->tkFlags.TknType NE TKT_CS_SKIPEND
     && lptkNext->tkFlags.TknType NE TKT_CS_WHILE2
//// && lptkNext->tkFlags.TknType NE TKT_EOS
     && lptkNext->tkFlags.TknType NE TKT_GLBDFN
     && lptkNext->tkFlags.TknType NE TKT_LABELSEP
     && lptkNext->tkFlags.TknType NE TKT_NOP
     && lptkNext->tkFlags.TknType NE TKT_SETALPHA
     && lptkNext->tkFlags.TknType NE TKT_SOS
     && lptkNext->tkFlags.TknType NE TKT_SYNTERR
       )
    {
        if (lptkNext->tkFlags.TknType EQ TKT_DELDEL)
        {
            LPSIS_HEADER lpSISCur;          // Ptr to current SIS layer

            // Search up the SIS chain to see what this is
            lpSISCur = SrchSISForDfn (lpMemPTD);

            // If the ptr is valid, ...
            if (lpSISCur)
            {
                // Split case based upon the function type
                switch (lpSISCur->DfnType)
                {
                    case DFNTYPE_FCN:
                        lptkNext->tkSynObj = soF;

                        break;

                    case DFNTYPE_OP1:
                        lptkNext->tkSynObj = soMOP;

                        break;

                    case DFNTYPE_OP2:
                        lptkNext->tkSynObj = soDOP;

                        break;

                    default:
                        // Mark it as a SYNTAX ERROR
                        DbgStop ();

                        break;
                } // End SWITCH
            } // End IF
        } // End IF

        // Get the token's syntax object
        tkSynObj = lptkNext->tkSynObj;

        Assert (tkSynObj <= soLAST);

        // Back off to the next text location
        lpwszTemp -= SONAMES_MAXLEN;

        // Get ptr to name
        lpwszName = soNames[tkSynObj];

        // Append the next name
        lstrcpyW (lpwszTemp, lpwszName);

        // Pad the name with spaces to a multiple of SONAMES_MAXLEN
        PadMemoryW (lpwszTemp, lstrlenW (lpwszName));

        // Stop if we're at an EOS/EOL
        if (lptkNext->tkFlags.TknType EQ TKT_EOS
         || lptkNext->tkFlags.TknType EQ TKT_EOL)
            break;
    } // End WHILE

    // Get ptr to start
    lpwszEnd = &lpwszEnd[-SONAMES_MAXLEN * 4];

    // If we're to pad the buffer, ...
    while (lpwszTemp > lpwszEnd)
        // Pad the buffer
        *--lpwszTemp = L' ';

    return lpwszEnd;
} // End LSTACK
#endif


#ifdef DEBUG
//***************************************************************************
//  $RSTACK
//
//  Display the right stack
//***************************************************************************

LPWCHAR RSTACK
    (LPPLLOCALVARS lpplLocalVars,   // Ptr to local plLocalVars
     LPPL_YYSTYPE *lpplOrgRhtStk)   // Ptr to start of right stack

{
    LPPERTABDATA  lpMemPTD;                 // Ptr to PerTabData global memory
    static  WCHAR wszTemp[100 * SONAMES_MAXLEN + 1] = {0};
    LPWCHAR       lpwszTemp,
                  lpwszEnd,
                  lpwszName;
    LPPL_YYSTYPE *lplpYYNext;

    // Get the PerTabData ptr
    lpMemPTD = lpplLocalVars->lpMemPTD;

    // Get the ptr to the start of the stack text
    lpwszTemp = &wszTemp[0];

    // Get the next right stack ptr
    lplpYYNext = lpMemPTD->lpplRhtStk;

    // Loop through the right stack
    while (--lplpYYNext > lpplOrgRhtStk)
    {
        Assert ((*lplpYYNext)->tkToken.tkSynObj <= soLAST);

        // Get ptr to name
        lpwszName = soNames[(*lplpYYNext)->tkToken.tkSynObj];

        // Append the next name
        lstrcpyW (lpwszTemp, lpwszName);

        // Pad the name with spaces to a multiple of SONAMES_MAXLEN
        PadMemoryW (lpwszTemp, lstrlenW (lpwszName));

        // Skip to the next text location
        lpwszTemp += SONAMES_MAXLEN;
    } // End WHILE

    // Get ptr to end
    lpwszEnd = &wszTemp[SONAMES_MAXLEN * 3];

    // If we're to pad the buffer, ...
    while (lpwszTemp < lpwszEnd)
        // Pad the buffer
        *lpwszTemp++ = L' ';

    // Ensure properly terminated
    lpwszTemp[0] = WC_EOS;

    return &wszTemp[0];
} // End RSTACK
#endif


//***************************************************************************
//  $plSetDfn
//
//  Subroutine to set UDFO/AFO properties
//***************************************************************************

NAME_TYPES plSetDfn
    (LPTOKEN lptkRes,                   // Ptr to the result
     HGLOBAL hGlbUDFO)                  // UDFO/AFO global memory handle

{
    UBOOL             bAFO,             // TRUE iff function is an AFO
                      bAfoArgs;         // TRUE iff the UDFO/AFO needs args
    LPDFN_HEADER      lpMemDfnHdr;      // Ptr to UDFO/AFO global memory header
    LPFCNARRAY_HEADER lpMemHdrFcn;      // ...    FCNARRAY ...
    DFN_TYPES         DfnType;          // DFN type (see DFN_TYPES)
    NAME_TYPES        nameType;         // The resulting NAMETYPE_xxx

    // Split cases based upon the signature
    switch (GetSignatureGlb_PTB (hGlbUDFO))
    {
        case DFN_HEADER_SIGNATURE:
            // Lock the memory to get a ptr to it
            lpMemDfnHdr = MyGlobalLock (hGlbUDFO);

            // Get the AFO flag
            bAFO = lpMemDfnHdr->bAFO;

            // Get the niladic function flag
            bAfoArgs = lpMemDfnHdr->FcnValence NE FCNVALENCE_NIL;

            // Get the DFN type
            DfnType = lpMemDfnHdr->DfnType;

            // We no longer need this ptr
            MyGlobalUnlock (hGlbUDFO); lpMemDfnHdr = NULL;

            break;

        case FCNARRAY_HEADER_SIGNATURE:
            // Lock the memory to get a otr to it
            lpMemHdrFcn = MyGlobalLock (hGlbUDFO);

            // Mark as not an AFO
            bAFO = FALSE;

            // But with arguments
            bAfoArgs = TRUE;

            // Split cases based upon the name type
            switch (lpMemHdrFcn->fnNameType)
            {
                case NAMETYPE_FN0:
                case NAMETYPE_FN12:
                case NAMETYPE_TRN:
                    DfnType = DFNTYPE_FCN;

                    break;

                case NAMETYPE_OP1:
                    DfnType = DFNTYPE_OP1;

                    break;

                case NAMETYPE_OP2:
                    DfnType = DFNTYPE_OP2;

                    break;

                case NAMETYPE_OP3:
                    DfnType = DFNTYPE_OP1;

                    break;

                defstop
                    break;
            } // End SWITCH

            // We no longer need this ptr
            MyGlobalUnlock (hGlbUDFO); lpMemHdrFcn = NULL;

            break;

        defstop
            break;
    } // End SWITCH

    // Split cases based upon the DFNTYPE_xxx
    switch (DfnType)
    {
        case DFNTYPE_FCN:
            if (bAFO)
                lptkRes->tkFlags.TknType = TKT_FCNAFO;
            lptkRes->tkSynObj            = bAfoArgs ? soF   : soNF;
            nameType                     = bAfoArgs ? NAMETYPE_FN12 : NAMETYPE_FN0;

            break;

        case DFNTYPE_OP1:
            if (bAFO)
                lptkRes->tkFlags.TknType = TKT_OP1AFO;
            lptkRes->tkSynObj            = bAfoArgs ? soMOP : soMOPN;
            nameType                     = NAMETYPE_OP1;

            break;

        case DFNTYPE_OP2:
            if (bAFO)
                lptkRes->tkFlags.TknType = TKT_OP2AFO;
            lptkRes->tkSynObj            = bAfoArgs ? soDOP : soDOPN;
            nameType                     = NAMETYPE_OP2;

            break;

        defstop
            break;
    } // End SWITCH

    return nameType;
} // End plSetDfn


//***************************************************************************
//  $ExecuteCS0
//
//  Execute a CS0
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecuteCS0"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecuteCS0
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to local plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj)        // Ptr to current PL_YYSTYPE

{
    LPPL_YYSTYPE lpYYRes = NULL,        // Ptr to the result
                 lpYYVar,               // ...    a temp
                 lpYYTmp;               // ...
    TOKEN        tkToken;               // The incoming token

    // Save the token type
    tkToken = lpplYYCurObj->tkToken;

    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // Split cases based upon the token type
    switch (tkToken.tkFlags.TknType)
    {
        case TKT_CS_CONTINUE:
            // Handle CONTINUE statement
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_CONTINUE_Stmt (lpplLocalVars, &tkToken);

            if (lpplLocalVars->bRet)
            {
                // Make a NoValue entry as the result
                lpYYRes = MakeNoValue_YY (&tkToken);

                // Change the tkSynObj
                lpYYRes->tkToken.tkSynObj = soEOS;
            } else
                // Mark as in error
                lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed

            break;

        case TKT_CS_ELSE:
            // Handle ELSE statement
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_ELSE_Stmt (lpplLocalVars, &tkToken);

            if (lpplLocalVars->bRet)
            {
                // Make a NoValue entry as the result
                lpYYRes = MakeNoValue_YY (&tkToken);

                // Change the tkSynObj
                lpYYRes->tkToken.tkSynObj = soEOS;
            } else
                // Mark as in error
                lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed

            break;

        case TKT_CS_ENDFOR:
        case TKT_CS_ENDFORLCL:
            // Handle ENDFOR statement
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_ENDFOR_Stmt_EM (lpplLocalVars, &tkToken, tkToken.tkFlags.TknType EQ TKT_CS_ENDFORLCL);

            if (lpplLocalVars->bRet)
            {
                // Make a NoValue entry as the result
                lpYYRes = MakeNoValue_YY (&tkToken);

                // Change the tkSynObj
                lpYYRes->tkToken.tkSynObj = soEOS;
            } else
                // Mark as in error
                lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed

            break;

        case TKT_CS_ENDREPEAT:
            // Handle ENDREPEAT statement
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_ENDREPEAT_Stmt (lpplLocalVars, &tkToken);

            if (lpplLocalVars->bRet)
            {
                // Make a NoValue entry as the result
                lpYYRes = MakeNoValue_YY (&tkToken);

                // Change the tkSynObj
                lpYYRes->tkToken.tkSynObj = soEOS;
            } else
                // Mark as in error
                lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed

            break;

        case TKT_CS_ENDWHILE:
            // Handle ENDWHILE statement
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_ENDWHILE_Stmt (lpplLocalVars, &tkToken);

            if (lpplLocalVars->bRet)
            {
                // Make a NoValue entry as the result
                lpYYRes = MakeNoValue_YY (&tkToken);

                // Change the tkSynObj
                lpYYRes->tkToken.tkSynObj = soEOS;
            } else
                // Mark as in error
                lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed

            break;

        case TKT_CS_LEAVE:
            // Handle LEAVE statement
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_LEAVE_Stmt (lpplLocalVars, &tkToken);

            if (lpplLocalVars->bRet)
            {
                // Make a NoValue entry as the result
                lpYYRes = MakeNoValue_YY (&tkToken);

                // Change the tkSynObj
                lpYYRes->tkToken.tkSynObj = soEOS;
            } else
                // Mark as in error
                lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed

            break;

        case TKT_CS_RETURN:
            // Check for Ctrl Strucs in AFO
            if (lpplLocalVars->bAfoCtrlStruc)   // RETURN
                goto SYNTAX_EXIT;

            // Allocate a new YYRes
            lpYYTmp = YYAlloc ();
            lpYYVar = YYAlloc ();

            // Manufacture a constant 0 in the last right object
            lpYYTmp->tkToken.tkFlags.TknType = TKT_VARIMMED;
            lpYYTmp->tkToken.tkFlags.ImmType = IMMTYPE_BOOL;
            lpYYTmp->tkToken.tkData.tkBoolean = 0;

            // Make the token into a YYSTYPE
            lpYYVar->tkToken = tkToken;

            // Call common {goto} code
            lpYYRes = plRedGO_A (lpplLocalVars, lpYYVar, lpYYTmp, soEOS);

            // YYFree the temps
            YYFree (lpYYVar); lpYYVar = NULL;
            YYFree (lpYYTmp); lpYYTmp = NULL;

            break;

        case TKT_CS_SKIPCASE:
            // Handle SKIPCASE statement
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_SKIPCASE_Stmt (lpplLocalVars, &tkToken);

            if (lpplLocalVars->bRet)
            {
                // Make a NoValue entry as the result
                lpYYRes = MakeNoValue_YY (&tkToken);

                // Change the tkSynObj
                lpYYRes->tkToken.tkSynObj = soEOS;
            } else
                // Mark as in error
                lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed

            break;

        case TKT_CS_SKIPEND:
            // Handle SKIPEND statement
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_SKIPEND_Stmt (lpplLocalVars, &tkToken);

            if (lpplLocalVars->bRet)
            {
                // Make a NoValue entry as the result
                lpYYRes = MakeNoValue_YY (&tkToken);

                // Change the tkSynObj
                lpYYRes->tkToken.tkSynObj = soEOS;
            } else
                // Mark as in error
                lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed

            break;

        case TKT_CS_REPEAT:
            // Check for Ctrl Strucs in AFO
            if (lpplLocalVars->bAfoCtrlStruc)   // RETURN
                goto SYNTAX_EXIT;

            break;

///     case TKT_CS_ENDIF:          // CS0  Handled in pl_yylex
///     case TKT_CS_ENDSELECT:      // ...
///     case TKT_CS_FOR2:           // ...
///     case TKT_CS_IF2:            // ...
///     case TKT_CS_REPEAT2:        // ...
///     case TKT_CS_SELECT2:        // ...
///     case TKT_CS_WHILE2:         // ...
        defstop
            break;
    } // End SWITCH

    return lpYYRes;

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lpplLocalVars->lptkNext);
    return NULL;
} // End ExecuteCS0
#undef  APPEND_NAME


//***************************************************************************
//  $plExecuteFn0
//
//  Execute a niladic function in the context of ParseLine
//***************************************************************************

LPPL_YYSTYPE plExecuteFn0
    (LPPL_YYSTYPE lpYYFn0)              // Ptr to niladic function

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // If this function is in the process of being stranded, ...
    if (lpYYFn0->YYStranding)
    {
        // Turn this function strand into a function
        lpYYRes =
          DbgMakeFcnStrand_EM_YY (lpYYFn0, NAMETYPE_FN0, FALSE);
        // YYFree the argument object
        YYFree (lpYYFn0); lpYYFn0 = NULL; // curSynObj = soNONE;

        // If not defined, ...
        if (!lpYYRes)
            goto ERROR_EXIT;

        // Copy to the argument ptr
        lpYYFn0 = lpYYRes;
        lpYYRes = NULL;
    } // End IF

    // Execute the Niladic Function and replace the argument object with the result
    lpYYRes =
      ExecuteFn0 (lpYYFn0);

    // Free the function (including YYFree)
    YYFreeArray (lpYYFn0); FreeYYFcn1  (lpYYFn0); lpYYFn0 = NULL;

    // Check for error
    if (lpYYRes)
        lpYYRes->tkToken.tkSynObj = soA;
ERROR_EXIT:
    return lpYYRes;     // Might be NULL
} // End plExecuteFn0


//***************************************************************************
//  End of File: parseline.c
//***************************************************************************