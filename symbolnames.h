//***************************************************************************
//  NARS2000 -- Symbol Names
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2009 Sudley Place Software

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

#include "Unicode.h"

typedef struct tagSYMBOLNAMES
{
    WCHAR  Symbol;      // Symbol
    WCHAR *lpwName;     // Ptr to name
} SYMBOLNAMES, *LPSYMBOLNAMES;


// Translate table for symbols to names
SYMBOLNAMES aSymbolNames[]
#ifdef DEFINE_VALUES
=
{
// No keystroke equivalents for these as yet
  {UTF16_DIERESISDOT          , L"{dieresisdot}"        },
  {UTF16_NOTEQUALUNDERBAR     , L"{notequalunderbar}"   },
  {UTF16_IBEAM                , L"{ibeam}"              },
  {UTF16_QUADJOT              , L"{quadjot}"            },
  {UTF16_QUADSLOPE            , L"{quadslope}"          },
  {UTF16_QUADLEFTARROW        , L"{quadleftarrow}"      },
  {UTF16_QUADRIGHTARROW       , L"{quadrightarrow}"     },
  {UTF16_QUADUPARROW          , L"{quaduparrow}"        },
  {UTF16_QUADDOWNARROW        , L"{quaddownarrow}"      },

// _alpbahet_ characters
  {UTF16_A_                   , L"{A underbar}"         },
  {UTF16_B_                   , L"{B underbar}"         },
  {UTF16_C_                   , L"{C underbar}"         },
  {UTF16_D_                   , L"{D underbar}"         },
  {UTF16_E_                   , L"{E underbar}"         },
  {UTF16_F_                   , L"{F underbar}"         },
  {UTF16_G_                   , L"{G underbar}"         },
  {UTF16_H_                   , L"{H underbar}"         },
  {UTF16_I_                   , L"{I underbar}"         },
  {UTF16_J_                   , L"{J underbar}"         },
  {UTF16_K_                   , L"{K underbar}"         },
  {UTF16_L_                   , L"{L underbar}"         },
  {UTF16_M_                   , L"{M underbar}"         },
  {UTF16_N_                   , L"{N underbar}"         },
  {UTF16_O_                   , L"{O underbar}"         },
  {UTF16_P_                   , L"{P underbar}"         },
  {UTF16_Q_                   , L"{Q underbar}"         },
  {UTF16_R_                   , L"{R underbar}"         },
  {UTF16_S_                   , L"{S underbar}"         },
  {UTF16_T_                   , L"{T underbar}"         },
  {UTF16_U_                   , L"{U underbar}"         },
  {UTF16_V_                   , L"{V underbar}"         },
  {UTF16_W_                   , L"{W underbar}"         },
  {UTF16_X_                   , L"{X underbar}"         },
  {UTF16_Y_                   , L"{Y underbar}"         },
  {UTF16_Z_                   , L"{Z underbar}"         },

// The alphabet, unshifted
  {UTF16_ALPHA                , L"{alpha}"              },  // Alt-'a' - alpha
  {UTF16_UPTACK               , L"{uptack}"             },  // Alt-'b' - base
  {UTF16_UPTACK               , L"{basevalue}"          },  // ...
  {UTF16_UPSHOE               , L"{upshoe}"             },  // Alt-'c' - intersection
  {UTF16_DOWNSTILE            , L"{downstile}"          },  // Alt-'d' - floor
  {UTF16_DOWNSTILE            , L"{min}"                },  // ...
  {UTF16_DOWNSTILE            , L"{floor}"              },  // ...
  {UTF16_EPSILON              , L"{epsilon}"            },  // Alt-'e' - epsilon
  {UTF16_INFINITY             , L"{infinity}"           },  // Alt-'f' - infinity
  {UTF16_DEL                  , L"{del}"                },  // Alt-'g' - del
  {UTF16_DELTA                , L"{delta}"              },  // Alt-'h' - delta
  {UTF16_IOTA                 , L"{iota}"               },  // Alt-'i' - iota
  {UTF16_JOT                  , L"{jot}"                },  // Alt-'j' - jot
  {UTF16_APOSTROPHE           , L"{apostrophe}"         },  // Alt-'k' - single-quote
  {UTF16_QUAD                 , L"{quad}"               },  // Alt-'l' - quad (9109??)
  {UTF16_STILE                , L"{stile}"              },  // Alt-'m' - modulus
  {UTF16_STILE                , L"{mod}"                },  // ...
  {UTF16_DOWNTACK             , L"{downtack}"           },  // Alt-'n' - representation
  {UTF16_DOWNTACK             , L"{represent}"          },  // ...
  {UTF16_CIRCLE               , L"{circle}"             },  // Alt-'o' - circle
  {UTF16_CIRCLE               , L"{pitimes}"            },  // ...
  {UTF16_STAR                 , L"{star}"               },  // Alt-'p' - power
  {UTF16_STAR                 , L"{power}"              },  // ...
  {UTF16_QUERY                , L"{query}"              },  // Alt-'q' - question-mark
  {UTF16_RHO                  , L"{rho}"                },  // Alt-'r' - rho
  {UTF16_RHO                  , L"{shape}"              },  // ...
  {UTF16_RHO                  , L"{reshape}"            },  // ...
  {UTF16_UPSTILE              , L"{upstile}"            },  // Alt-'s' - ceiling
  {UTF16_UPSTILE              , L"{max}"                },  // ...
  {UTF16_UPSTILE              , L"{ceiling}"            },  // ...
  {UTF16_TILDE                , L"{tilde}"              },  // Alt-'t' - tilde
  {UTF16_TILDE                , L"{not}"                },  // ...
  {UTF16_TILDE                , L"{without}"            },  // ...
  {UTF16_DOWNARROW            , L"{downarrow}"          },  // Alt-'u' - down arrow
  {UTF16_DOWNARROW            , L"{drop}"               },  // ...
  {UTF16_DOWNSHOE             , L"{downshoe}"           },  // Alt-'v' - union
  {UTF16_DOWNSHOE             , L"{union}"              },  // ...
  {UTF16_OMEGA                , L"{omega}"              },  // Alt-'w' - omega
  {UTF16_RIGHTSHOE            , L"{rightshoe}"          },  // Alt-'x' - disclose
  {UTF16_RIGHTSHOE            , L"{pick}"               },  // ...
  {UTF16_UPARROW              , L"{uparrow}"            },  // Alt-'y' - up arrow
  {UTF16_UPARROW              , L"{take}"               },  // ...
  {UTF16_LEFTSHOE             , L"{leftshoe}"           },  // Alt-'z' - enclose

// The alphabet, shifted
//{UTF16_                     ,                         },  // Alt-'A' - (none)
//{UTF16_                     ,                         },  // Alt-'B' - (none)
//{UTF16_                     ,                         },  // Alt-'C' - (none)
//{UTF16_                     ,                         },  // Alt-'D' - (none)
  {UTF16_EPSILONUNDERBAR      , L"{epsilonunderbar}"    },  // Alt-'E' - epsilon-underbar
  {UTF16_EPSILONUNDERBAR      , L"{find}"               },  // ...
//{UTF16_                     ,                         },  // Alt-'F' - (none)
  {UTF16_DIERESISDEL          , L"{dieresisdel}"        },  // Alt-'G' - Dual operator        (frog)
  {UTF16_DELTAUNDERBAR        , L"{deltaunderbar}"      },  // Alt-'H' - delta-underbar
  {UTF16_IOTAUNDERBAR         , L"{iotaunderbar}"       },  // Alt-'I' - iota-underbar
  {UTF16_DIERESISJOT          , L"{dieresisjot}"        },  // Alt-'J' - Rank operator        (hoot)
  {UTF16_DIERESISJOT          , L"{hoot}"               },  // ...
  {UTF16_DIERESISJOT          , L"{paw}"                },  // ...
  {UTF16_DIERESISJOT          , L"{av136}"              },  // ...
//{UTF16_                     ,                         },  // Alt-'K' - (none)
  {UTF16_SQUAD                , L"{squad}"              },  // Alt-'L' - squad
  {UTF16_STILETILDE           , L"{stiletilde}"         },  // Alt-'M' - Partition operator   (dagger)
  {UTF16_DIERESISDOWNTACK     , L"{dieresisdowntack}"   },  // Alt-'N' - Convolution operator (snout)
  {UTF16_DIERESISCIRCLE       , L"{dieresiscircle}"     },  // Alt-'O' - Compose              (holler)
  {UTF16_DIERESISCIRCLE       , L"{holler}"             },  // ...
  {UTF16_DIERESISCIRCLE       , L"{hoof}"               },  // ...
  {UTF16_DIERESISSTAR         , L"{dieresisstar}"       },  // Alt-'P' - Power operator       (sourpuss)
//{UTF16_                     ,                         },  // Alt-'Q' - (none)
//{UTF16_                     ,                         },  // Alt-'R' - (none)
//{UTF16_                     ,                         },  // Alt-'S' - (none)
  {UTF16_DIERESISTILDE        , L"{dieresistilde}"      },  // Alt-'T' - Commute operator     (frown)
//{UTF16_                     ,                         },  // Alt-'U' - (none)
//{UTF16_                     ,                         },  // Alt-'V' - (none)
//{UTF16_                     ,                         },  // Alt-'W' - (none)
//{UTF16_                     ,                         },  // Alt-'X' - (none)
//{UTF16_                     ,                         },  // Alt-'Y' - (none)
//{UTF16_                     ,                         },  // Alt-'Z' - (none)

// Top row, unshifted
  {UTF16_DIAMOND              , L"{diamond}"            },  // Alt-'`' - diamond (9674??)
  {UTF16_DIERESIS             , L"{dieresis}"           },  // Alt-'1' - dieresis
  {UTF16_DIERESIS             , L"{each}"               },  // ...
  {UTF16_OVERBAR              , L"{overbar}"            },  // Alt-'2' - high minus
  {UTF16_OVERBAR              , L"{neg}"                },  // ...
  {UTF16_LEFTCARET            , L"{leftcaret}"          },  // Alt-'3' - less
  {UTF16_LEFTCARETUNDERBAR    , L"{leftcaretunderbar}"  },  // Alt-'4' - not more
  {UTF16_LEFTCARETUNDERBAR    , L"{<=}"                 },  // ...
  {UTF16_EQUAL                , L"{equal}"              },  // Alt-'5' - equal
  {UTF16_RIGHTCARETUNDERBAR   , L"{rightcaretunderbar}" },  // Alt-'6' - not less
  {UTF16_RIGHTCARETUNDERBAR   , L"{>=}"                 },  // ...
  {UTF16_RIGHTCARET           , L"{rightcaret}"         },  // Alt-'7' - more
  {UTF16_NOTEQUAL             , L"{notequal}"           },  // Alt-'8' - not equal
  {UTF16_NOTEQUAL             , L"{/=}"                 },  // ...
  {UTF16_DOWNCARET            , L"{downcaret}"          },  // Alt-'9' - or
  {UTF16_DOWNCARET            , L"{or}"                 },  // ...
  {UTF16_UPCARET              , L"{upcaret}"            },  // Alt-'0' - and (94??)
  {UTF16_UPCARET              , L"{and}"                },  // ...
  {UTF16_TIMES                , L"{times}"              },  // Alt-'-' - times
  {UTF16_TIMES                , L"{signum}"             },  // ...
  {UTF16_COLONBAR             , L"{colonbar}"           },  // Alt-'=' - divide
  {UTF16_COLONBAR             , L"{divide}"             },  // ...

// Top row, shifted
  {UTF16_COMMABAR             , L"{commabar}"           },  // Alt-'~' - comma-bar
  {UTF16_EQUALUNDERBAR        , L"{equalunderbar}"      },  // Alt-'!' - match
  {UTF16_EQUALUNDERBAR        , L"{match}"              },  // ...
  {UTF16_CIRCLEMIDDLEDOT      , L"{circlemiddledot}"    },  // Alt-'@' - circle-middle-dot
  {UTF16_DELSTILE             , L"{delstile}"           },  // Alt-'#' - grade-down
  {UTF16_DELSTILE             , L"{gradedown}"          },  // ...
  {UTF16_DELTASTILE           , L"{deltastile}"         },  // Alt-'$' - grade-up
  {UTF16_DELTASTILE           , L"{gradeup}"            },  // ...
  {UTF16_CIRCLESTILE          , L"{circlestile}"        },  // Alt-'%' - rotate
  {UTF16_CIRCLESTILE          , L"{rotate}"             },  // ...
  {UTF16_CIRCLESTILE          , L"{reverse}"            },  // ...
  {UTF16_CIRCLESLOPE          , L"{circleslope}"        },  // Alt-'^' - transpose
  {UTF16_CIRCLESLOPE          , L"{transpose}"          },  // ...
  {UTF16_CIRCLEBAR            , L"{circlebar}"          },  // Alt-'&' - circle-bar
  {UTF16_CIRCLEBAR            , L"{rotatebar}"          },  // ...
  {UTF16_CIRCLEBAR            , L"{reversebar}"         },  // ...
  {UTF16_CIRCLESTAR           , L"{circlestar}"         },  // Alt-'*' - log
  {UTF16_CIRCLESTAR           , L"{log}"                },  // ...
  {UTF16_CIRCLESTAR           , L"{ln}"                 },  // ...
  {UTF16_DOWNCARETTILDE       , L"{downcarettilde}"     },  // Alt-'(' - nor
  {UTF16_DOWNCARETTILDE       , L"{nor}"                },  // ...
  {UTF16_UPCARETTILDE         , L"{upcarettilde}"       },  // Alt-')' - nand
  {UTF16_UPCARETTILDE         , L"{nand}"               },  // ...
  {UTF16_QUOTEDOT             , L"{quotedot}"           },  // Alt-'_' - quote-dot
  {UTF16_DOMINO               , L"{domino}"             },  // Alt-'+' - domino

// Second row, unshifted
  {UTF16_LEFTARROW            , L"{leftarrow}"          },  // Alt-'[' - left arrow
  {UTF16_LEFTARROW            , L"{<-}"                 },  // ...
  {UTF16_LEFTARROW            , L"{is}"                 },  // ...
  {UTF16_RIGHTARROW           , L"{rightarrow}"         },  // Alt-']' - right arrow
  {UTF16_RIGHTARROW           , L"{->}"                 },  // ...
  {UTF16_RIGHTARROW           , L"{goto}"               },  // ...
  {UTF16_LEFTTACK             , L"{lefttack}"           },  // Alt-'\' - left tack

// Second row, shifted
  {UTF16_QUOTEQUAD            , L"{quotequad}"          },  // Alt-'{' - quote-quad
  {UTF16_ZILDE                , L"{zilde}"              },  // Alt-'}' - zilde
  {UTF16_RIGHTTACK            , L"{righttack}"          },  // Alt-'|' - right tack

// Third row, unshifted
  {UTF16_UPTACKJOT            , L"{uptackjot}"          },  // Alt-';' - execute
  {UTF16_UPTACKJOT            , L"{execute}"            },  // ...
  {UTF16_DOWNTACKJOT          , L"{downtackjot}"        },  // Alt-'\''- format
  {UTF16_DOWNTACKJOT          , L"{format}"             },  // ...

// Third row, shifted
//{UTF16_                     ,                         },  // Alt-':' - (none)
//{UTF16_                     ,                         },  // Alt-'"' - (none)

// Fourth row, unshifted
  {UTF16_LAMP                 , L"{lamp}"               },  // Alt-',' - comment
  {UTF16_SLOPEBAR             , L"{slopebar}"           },  // Alt-'.' - slope-bar
  {UTF16_SLOPEBAR             , L"{backslashbar}"       },  // ...
  {UTF16_SLASHBAR             , L"{slashbar}"           },  // Alt-'/' - slash-bar

// Fourth row, shifted
//{UTF16_                     ,                         },  // Alt-'<' - (none)
//{UTF16_                     ,                         },  // Alt-'>' - (none)
//{UTF16_                     ,                         },  // Alt-'?' - (none)

  {L' '                       , L"{space}"              },

// Non-Alt key equivalents (these are the only ones we need for SaveWS)
  {UTF16_DOUBLEQUOTE          , L"{doublequote}"        },  // 22:  Double quote
  {UTF16_POUND                , L"{pound}"              },  // 23:  Pound
  {UTF16_COMMA                , L"{comma}"              },  // 2C:  Comma
  {UTF16_SLOPE                , L"{slope}"              },  // 5C:  Slope
  {UTF16_SLOPE                , L"{backslash}"          },  // 5C:  ...
  {UTF16_CIRCUMFLEX           , L"{circumflex}"         },  // 5E:  Circumflex
  {UTF16_LEFTBRACE            , L"{leftbrace}"          },  // 7B:  Left brace
  {UTF16_STILE2               , L"{stile2}"             },  // 7C:  Stile (a.k.a. 0x2223)
  {UTF16_RIGHTBRACE           , L"{rightbrace}"         },  // 7D:  Right brace
}
#endif
;

//***************************************************************************
//  End of File: symbolnames.h
//***************************************************************************
