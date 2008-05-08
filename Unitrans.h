//****************************************************************************
//  NARS2000 -- Unicode Translation Constants
//****************************************************************************

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

// Translation table for converting chars from
//   various APL systems to NARS.

#define NA  0x0000

typedef enum tagUNI_TRANS
{
    UNITRANS_APLWIN = 0,    // 00:  APL+WIN 3.6 (and maybe later)
    UNITRANS_ISO,           // 01:  ISO Standard (What Typeface is this???)
    UNITRANS_APL2,          // 02:  APL2 ???     ***FINISHME***
    UNITRANS_DYALOG,        // 03:  Dyalog ???   ***FINISHME***
    UNITRANS_NORMAL,        // 04:  Normal paste translation (from Ctrl-v)
    UNITRANS_NARS,          // 05:  NARS2000
    UNITRANS_LENGTH,        // 06:  Length of the enum *MUST* be last
} UNI_TRANS;

WCHAR uniTrans[][UNITRANS_LENGTH] =
{// APL+WIN    ISO     APL2  Dyalog  Normal  NARS
    {  NA  , 0x00C3,   NA  ,   NA  ,   NA  , L'a'                    }, // A underbar
    {  NA  , 0x00C4,   NA  ,   NA  ,   NA  , L'b'                    }, // B underbar
    {  NA  , 0x00C5,   NA  ,   NA  ,   NA  , L'c'                    }, // C underbar
    {  NA  , 0x00C6,   NA  ,   NA  ,   NA  , L'd'                    }, // D underbar
    {  NA  , 0x00C7,   NA  ,   NA  ,   NA  , L'e'                    }, // E underbar
    {  NA  , 0x00C8,   NA  ,   NA  ,   NA  , L'f'                    }, // F underbar
    {  NA  , 0x00C9,   NA  ,   NA  ,   NA  , L'g'                    }, // G underbar
    {  NA  , 0x00CA,   NA  ,   NA  ,   NA  , L'h'                    }, // H underbar
    {  NA  , 0x00CB,   NA  ,   NA  ,   NA  , L'i'                    }, // I underbar
    {  NA  , 0x00CC,   NA  ,   NA  ,   NA  , L'j'                    }, // J underbar
    {  NA  , 0x00CD,   NA  ,   NA  ,   NA  , L'k'                    }, // K underbar
    {  NA  , 0x00CE,   NA  ,   NA  ,   NA  , L'l'                    }, // L underbar
    {  NA  , 0x00CF,   NA  ,   NA  ,   NA  , L'm'                    }, // M underbar
    {  NA  , 0x00D0,   NA  ,   NA  ,   NA  , L'n'                    }, // N underbar
    {  NA  , 0x00D1,   NA  ,   NA  ,   NA  , L'o'                    }, // O underbar
    {  NA  , 0x00D2,   NA  ,   NA  ,   NA  , L'p'                    }, // P underbar
    {  NA  , 0x00D3,   NA  ,   NA  ,   NA  , L'q'                    }, // Q underbar
    {  NA  , 0x00D4,   NA  ,   NA  ,   NA  , L'r'                    }, // R underbar
    {  NA  , 0x00D5,   NA  ,   NA  ,   NA  , L's'                    }, // S underbar
    {  NA  , 0x00D6,   NA  ,   NA  ,   NA  , L't'                    }, // T underbar
    {  NA  , 0x00D7,   NA  ,   NA  ,   NA  , L'u'                    }, // U underbar
    {  NA  , 0x00D8,   NA  ,   NA  ,   NA  , L'v'                    }, // V underbar
    {  NA  , 0x00D9,   NA  ,   NA  ,   NA  , L'w'                    }, // W underbar
    {  NA  , 0x00DA,   NA  ,   NA  ,   NA  , L'x'                    }, // X underbar
    {  NA  , 0x00DB,   NA  ,   NA  ,   NA  , L'y'                    }, // Y underbar
    {  NA  , 0x00DC,   NA  ,   NA  ,   NA  , L'z'                    }, // Z underbar
    {0x2018, 0x2018, 0x2018, 0x2018, 0x2018, UTF16_SINGLEQUOTE       }, // Left single quotation mark
    {0x2019, 0x2019, 0x2019, 0x2019, 0x2019, UTF16_SINGLEQUOTE       }, // Right ...
////{0x00A7,   NA  ,   NA  ,   NA  ,   NA  , UTF16_PARA              }, // Section
////{0x20AC,   NA  ,   NA  ,   NA  ,   NA  , UTF16_EURO              }, // Euro
    {0x00BA,   NA  ,   NA  ,   NA  ,   NA  , UTF16_EPSILONUNDERBAR   }, // Epsilon underbar     Missing from ISO charset
    {0x00AA, 0x00C0,   NA  ,   NA  ,   NA  , UTF16_DIAMOND           }, // Diamond
    {0x00A8, 0x00A1,   NA  ,   NA  ,   NA  , UTF16_DIERESIS          }, // Dieresis
    {0x201E, 0x00BC,   NA  ,   NA  ,   NA  , UTF16_LEFTARROW         }, // Left arrow
    {0x203A, 0x00BB,   NA  ,   NA  ,   NA  , UTF16_LEFTSHOE          }, // Left shoe
    {0x0153, 0x00BA,   NA  ,   NA  ,   NA  , UTF16_RIGHTSHOE         }, // Right shoe
    {0x00B5, 0x00EB,   NA  ,   NA  ,   NA  , UTF16_CIRCLESTAR        }, // Circle star
    {0x005E, 0x00A4,   NA  ,   NA  ,   NA  , UTF16_UPCARET           }, // Up caret
////{0x00E5,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // a circle
////{0x00E6,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // ae dipthong
////{0x00EC,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // i left
    {0x0090, 0x00E8,   NA  ,   NA  ,   NA  , UTF16_DELTILDE          }, // Del tilde
////{0x00F9,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // u left
////{0x00F2,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // o left
    {0x00D0,   NA  ,   NA  ,   NA  ,   NA  , UTF16_ZILDE             }, // Zilde                Missing from ISO charset
    {0x00BE, 0x00B9,   NA  ,   NA  ,   NA  , UTF16_OMEGA             }, // Omega
    {0x2020, 0x00F5,   NA  ,   NA  ,   NA  , UTF16_UPARROW           }, // Up arrow
    {0x2021, 0x00B7,   NA  ,   NA  ,   NA  , UTF16_DOWNARROW         }, // Down arrow
    {0x2026, 0x00BE,   NA  ,   NA  ,   NA  , UTF16_RIGHTARROW        }, // Right arrow
    {0x0081, 0x00C1,   NA  ,   NA  ,   NA  , UTF16_LEFTTACK          }, // Left tack
    {0x00A4, 0x00BD,   NA  ,   NA  ,   NA  , UTF16_RIGHTTACK         }, // Right tack
    {0x201C, 0x00E6,   NA  ,   NA  ,   NA  , UTF16_DELTASTILE        }, // Delta stile
    {0x201D, 0x00E7,   NA  ,   NA  ,   NA  , UTF16_DELSTILE          }, // Del stile
//  {0x00A6,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // broken stile
//  {0x00C7,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // C cedilla
//  {0x00FC,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // u umlaut
//  {0x00E9,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // e right
//  {0x00E2,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // a circumflex
//  {0x00E4,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // a umlaut
//  {0x00E0,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // a left
    {0x00AC, 0x00A5,   NA  ,   NA  ,   NA  , UTF16_NOTEQUAL          }, // Not equal
//  {0x00E7,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // c cedilla
//  {0x00EA,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // e circumflex
//  {0x00EB,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // e umlaut
//  {0x00E8,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // e left
//  {0x00EF,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // i umlaut
//  {0x00EE,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // i circumflex
    {0x2014, 0x00B6,   NA  ,   NA  ,   NA  , UTF16_UPSTILE           }, // Upstile
//  {0x00C4,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // A umlaut
    {0x02DC, 0x00AC,   NA  ,   NA  ,   NA  , UTF16_DOWNSTILE         }, // Downstile
//  {0x00C9,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // E right
    {0x2018, 0x00AF,   NA  ,   NA  ,   NA  , UTF16_DELTA             }, // Delta
    {0x00D7, 0x00A7, 0x000D,   NA  ,   NA  , UTF16_TIMES             }, // Times
//  {0x00F4,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // o circumflex
//  {0x00F6,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // o umlaut
    {0x0152, 0x00B2,   NA  ,   NA  ,   NA  , UTF16_QUAD              }, // Quad
//  {0x00FB,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // u circumflex
    {0x008D, 0x00E4,   NA  ,   NA  ,   NA  , UTF16_QUOTEQUAD         }, // Quote-quad
    {0x017D, 0x00E1,   NA  ,   NA  ,   NA  , UTF16_DOMINO            }, // Domino
//  {0x00D6,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // O umlaut
//  {0x00A2,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // cents
//  {0x00A3,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // pounds
//  {0x00A5,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // yen
    {0x00AE, 0x00EF,   NA  ,   NA  ,   NA  , UTF16_COMMABAR          }, // commabar
    {0x00FE, 0x00A1,   NA  ,   NA  ,   NA  , UTF16_DIERESISTILDE     }, // dieresis-tilde
//  {0x00E1,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // a right
//  {0x00ED,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // i right
//  {0x00F3,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // o right
//  {0x00FA,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // i right
//  {0x00F1,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // n tilde
//  {0x00D1,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // N tilde
    {0x00A9, 0x00DD,   NA  ,   NA  ,   NA  , UTF16_LAMP              }, // Lamp
    {0x2122, 0x00F1,   NA  ,   NA  ,   NA  , UTF16_SLOPEBAR          }, // slope-bar
//  {0x00BF,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // upside down query
    {0x00DE,   NA  ,   NA  ,   NA  ,   NA  , UTF16_SQUAD             }, // Squad                Missing from ISO charset
//  {0x00F5,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // o tilde
//  {0x00F8,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // 0 slash
//  {0x00FD,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // y right
//  {0x00A1,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // upside down shreik
//  {0x00AB,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // left chevron
//  {0x00BB,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // right chevron
//  {0x00C0,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // A left
//  {0x00C1,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // A right
//  {0x00C2,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // A circumflex
//  {0x00C3,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // A tilde
//  {0x00C4,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // A umlaut
//  {0x00C5,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // A circle
//  {0x00C6,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // AE dipthong
//  {0x00C7,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // C cedilla
//  {0x00C8,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // E left
//  {0x00C9,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // E right
//  {0x00CA,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // E circumflex
//  {0x00CB,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // E umlaut
//  {0x00CC,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // I left
//  {0x00CD,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // I right
//  {0x00CE,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // I circumflex
//  {0x00CF,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // I umlaut
//  {0x00D1,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // N tilde
//  {0x00D2,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // O left
//  {0x00D3,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // O right
//  {0x00D4,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // O circumflex
//  {0x00D5,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // O tilde
//  {0x00D6,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // O umlaut
//  {0x00D8,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // O slash
//  {0x00D9,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // U left
//  {0x00DA,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // U right
//  {0x00DB,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // U circumflex
//  {0x00DC,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // U umlaut
//  {0x00DD,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // Y right
//  {0x00FF,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // y umlaut
    {0x00B8, 0x00A9,   NA  ,   NA  ,   NA  , UTF16_ALPHA             }, // Alpha
//  {0x00DF,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // Beta
    {0x00BC, 0x00B0,   NA  ,   NA  ,   NA  , UTF16_IOTA              }, // Iota
    {0x00F0, 0x00F8,   NA  ,   NA  ,   NA  , UTF16_DIERESISJOT       }, // Dieresis jot
//  {0x00E3,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                  }, // a tilde
    {0x2039, 0x00E9,   NA  ,   NA  ,   NA  , UTF16_DOWNCARETTILDE    }, // Downcaret tilde
    {0x0192, 0x00AA,   NA  ,   NA  ,   NA  , UTF16_UPTACK            }, // Up tack
    {0x201A, 0x00B3,   NA  ,   NA  ,   NA  , UTF16_DOWNTACK          }, // Down tack
    {0x00B2, 0x00EC,   NA  ,   NA  ,   NA  , UTF16_CIRCLESTILE       }, // Circle stile
    {0x00B4, 0x00EE,   NA  ,   NA  ,   NA  , UTF16_CIRCLEBAR         }, // Circle bar
    {0x0160, 0x00EA,   NA  ,   NA  ,   NA  , UTF16_UPCARETTILDE      }, // Upcaret tilde
    {0x0161, 0x00F0,   NA  ,   NA  ,   NA  , UTF16_SLASHBAR          }, // Slash bar
    {0x2019, 0x00AE,   NA  ,   NA  ,   NA  , UTF16_DEL               }, // Del
    {0x00B3, 0x00ED,   NA  ,   NA  ,   NA  , UTF16_CIRCLESLOPE       }, // Circle slope
    {0x00B9, 0x00AD,   NA  ,   NA  ,   NA  , UTF16_EPSILON           }, // Epsilon
    {0x009D, 0x00AB,   NA  ,   NA  ,   NA  , UTF16_UPSHOE            }, // Up shoe
    {0x00AD, 0x00F4,   NA  ,   NA  ,   NA  , UTF16_EQUALUNDERBAR     }, // Equal underbar
    {0x008F, 0x00C2,   NA  ,   NA  ,   NA  , UTF16_DELTAUNDERBAR     }, // Delta underbar
    {0x2030, 0x00BF,   NA  ,   NA  ,   NA  , UTF16_RIGHTCARETUNDERBAR}, // Right caret underbar
    {0x02C6, 0x00A2,   NA  ,   NA  ,   NA  , UTF16_LEFTCARETUNDERBAR }, // Left caret underbar
    {0x2022, 0x00E0,   NA  ,   NA  ,   NA  , UTF16_DOWNTACKJOT       }, // Down tack jot
    {0x2013, 0x00DF,   NA  ,   NA  ,   NA  , UTF16_UPTACKJOT         }, // Up tack jot
    {0x00F7, 0x00A6,   NA  ,   NA  ,   NA  , UTF16_COLONBAR          }, // Colon bar
    {0x00B0, 0x00B1,   NA  ,   NA  ,   NA  , UTF16_JOT               }, // Jot
    {0x00B1, 0x00B4,   NA  ,   NA  ,   NA  , UTF16_CIRCLE            }, // Circle
    {0x0178, 0x00A3,   NA  ,   NA  ,   NA  , UTF16_DOWNCARET         }, // Down caret
    {0x00BD, 0x00B5, 0x03C1,   NA  ,   NA  , UTF16_RHO               }, // Rho
    {0x017E, 0x00B8,   NA  ,   NA  ,   NA  , UTF16_DOWNSHOE          }, // Down shoe
    {0x00AF, 0x00A8,   NA  ,   NA  ,   NA  , UTF16_OVERBAR           }, // Overbar
    {0x007C, 0x007C,   NA  ,   NA  ,   NA  , UTF16_STILE             }, // Stile
    {  NA  ,   NA  , 0x001E,   NA  ,   NA  , UTF16_DOT               }, // Dot
 // APL+WIN    ISO     APL2  Dyalog  Normal  NARS
};

#undef  NA

// The # rows in the above table
#define UNITRANS_NROWS  (sizeof (uniTrans) / sizeof (uniTrans[0]))

//***************************************************************************
//  End of File: Unitrans.h
//***************************************************************************
