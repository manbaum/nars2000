//***************************************************************************
//  NARS2000 -- Token To Syntax Object Header
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2017 Sudley Place Software

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

typedef struct tagTOKEN_SO
{
    TOKEN_TYPES tkType;             // Token
    SO_ENUM     tkSynObj;           // Matching Syntax Object
} TOKEN_SO, *LPTOKEN_SO;

// The following struc includes only those tokens that are generated by the Tokenizer
// Token to Syntax Object mapping
EXTERN
TOKEN_SO tokenSo[]
#ifdef DEFINE_VALUES
=
{{TKT_UNUSED        , soNONE },         // 00
 {TKT_VARNAMED      , soA    },         // 01
 {TKT_CHRSTRAND     , soA    },         // 02
 {TKT_NUMSTRAND     , soA    },         // 03
 {TKT_NUMSCALAR     , soA    },         // 04
 {TKT_VARIMMED      , soA    },         // 05
 {TKT_ASSIGN        , soSP   },         // 06
 {TKT_LISTSEP       , soSEMI },         // 07
 {TKT_LABELSEP      , soEOS  },         // 08
 {TKT_COLON         , soNONE },         // 09  ??
 {TKT_FCNIMMED      , soF    },         // 0A
 {TKT_OP1IMMED      , soMOP  },         // 0B
 {TKT_OP2IMMED      , soDOP  },         // 0C
 {TKT_OP3IMMED      , soHY   },         // 0D
 {TKT_OPJOTDOT      , soJD   },         // 0E
 {TKT_LEFTPAREN     , soLP   },         // 0F
 {TKT_RIGHTPAREN    , soRP   },         // 10
 {TKT_LEFTBRACKET   , soLBK  },         // 11
 {TKT_RIGHTBRACKET  , soRBK  },         // 12
 {TKT_LEFTBRACE     , soLBC  },         // 13
 {TKT_RIGHTBRACE    , soRBC  },         // 14
 {TKT_EOS           , soEOS  },         // 15
 {TKT_EOL           , soEOS  },         // 16
 {TKT_SOS           , soNONE },         // 17  ??
 {TKT_LINECONT      , soNONE },         // 18  ??
 {TKT_INPOUT        , soIO   },         // 19
 {TKT_VARARRAY      , soA    },         // 1A
 {TKT_CS_ANDIF      , soCS1  },         // 1B
 {TKT_CS_ASSERT     , soCS1  },         // 1C
 {TKT_CS_CASE       , soCS1  },         // 1D
 {TKT_CS_CASELIST   , soCS1  },         // 1E
 {TKT_CS_CONTINUE   , soCS0  },         // 1F
 {TKT_CS_CONTINUEIF , soCS1  },         // 20
 {TKT_CS_ELSE       , soCS0  },         // 21
 {TKT_CS_ELSEIF     , soCS1  },         // 22
 {TKT_CS_END        , soCS0  },         // 23
 {TKT_CS_ENDFOR     , soCS0  },         // 24
 {TKT_CS_ENDFORLCL  , soCS0  },         // 25
 {TKT_CS_ENDIF      , soEOS  },         // 26
 {TKT_CS_ENDREPEAT  , soCS0  },         // 27
 {TKT_CS_ENDSELECT  , soEOS  },         // 28
 {TKT_CS_ENDWHILE   , soCS0  },         // 29
 {TKT_CS_FOR        , soCSF  },         // 2A
 {TKT_CS_FOR2       , soCS0  },         // 2B
 {TKT_CS_FORLCL     , soCSF  },         // 2C
 {TKT_CS_GOTO       , soCS1  },         // 2D
 {TKT_CS_IF         , soCS1  },         // 2E
 {TKT_CS_IF2        , soCS0  },         // 2F
 {TKT_CS_IN         , soCSI  },         // 30
 {TKT_CS_LEAVE      , soCS0  },         // 31
 {TKT_CS_LEAVEIF    , soCS1  },         // 32
 {TKT_CS_ORIF       , soCS1  },         // 33
 {TKT_CS_REPEAT     , soCS0  },         // 34
 {TKT_CS_REPEAT2    , soCS0  },         // 35
 {TKT_CS_RETURN     , soCS0  },         // 36
 {TKT_CS_SELECT     , soCS1  },         // 37
 {TKT_CS_SELECT2    , soCS0  },         // 38
 {TKT_CS_UNTIL      , soCS1  },         // 39
 {TKT_CS_WHILE      , soCS1  },         // 3A
 {TKT_CS_WHILE2     , soCS0  },         // 3B
 {TKT_CS_SKIPCASE   , soCS0  },         // 3C
 {TKT_CS_SKIPEND    , soCS0  },         // 3D
 {TKT_SYS_NS        , soSYSN },         // 3E
 {TKT_SYNTERR       , soSYNR },         // 3F
 {TKT_SETALPHA      , soCS0  },         // 40
 {TKT_DEL           , soF    },         // 41
 {TKT_DELDEL        , soNONE },         // 42  Could be MOPN, DOP or DOPN
 {TKT_DELAFO        , soF    },         // 43
 {TKT_FCNAFO        , soF    },         // 44
 {TKT_OP1AFO        , soMOP  },         // 45
 {TKT_OP2AFO        , soDOP  },         // 46
 {TKT_GLBDFN        , soNONE },         // ??
 {TKT_NOP           , soNONE },         // ??
 {TKT_AFOGUARD      , soAFOG },         // 49
 {TKT_AFORETURN     , soAFOR },         // 4A
 {TKT_STRAND        , soSA   },         // 4B
 {TKT_LISTINT       , soNONE },         // 4C  ??
 {TKT_LISTPAR       , soNONE },         // 4D  ??
 {TKT_LSTIMMED      , soNONE },         // 4E  ??
 {TKT_LSTARRAY      , soNONE },         // 4F  ??
 {TKT_LSTMULT       , soNONE },         // 50  ??
 {TKT_AXISIMMED     , soA    },         // 51
 {TKT_AXISARRAY     , soA    },         // 52
 {TKT_FCNARRAY      , soF    },         // 53
 {TKT_FCNNAMED      , soF    },         // 54
 {TKT_OP1NAMED      , soMOP  },         // 55
 {TKT_OP2NAMED      , soDOP  },         // 56
 {TKT_OP3NAMED      , soHY   },         // 57
 {TKT_STRNAMED      , soNONE },         // 58  ??
 {TKT_CS_NEC        , soNONE },         // 59  ??
 {TKT_CS_EOL        , soNONE },         // 5A  ??
 {TKT_CS_ENS        , soNONE },         // 5B  ??
 {TKT_FILLJOT       , soNF   },         // 5C
 {TKT_FCNDFN        , soF    },         // 5D
 {TKT_OP1DFN        , soMOP  },         // 5E
 {TKT_OP2DFN        , soDOP  },         // 5F
}
#endif
;


// Count the # entries so we can measure against it
EXTERN
UINT tokenSoLen
#ifdef DEFINE_VALUES
= countof (tokenSo)
#endif
;


//***************************************************************************
//  End of File: tokenso.h
//***************************************************************************