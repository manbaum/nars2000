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
    {0x0000, 0x00C3, 0x0000, 0x0000, 0x0000, L'a'                    }, // A underbar
    {0x0000, 0x00C4, 0x0000, 0x0000, 0x0000, L'b'                    }, // B underbar
    {0x0000, 0x00C5, 0x0000, 0x0000, 0x0000, L'c'                    }, // C underbar
    {0x0000, 0x00C6, 0x0000, 0x0000, 0x0000, L'd'                    }, // D underbar
    {0x0000, 0x00C7, 0x0000, 0x0000, 0x0000, L'e'                    }, // E underbar
    {0x0000, 0x00C8, 0x0000, 0x0000, 0x0000, L'f'                    }, // F underbar
    {0x0000, 0x00C9, 0x0000, 0x0000, 0x0000, L'g'                    }, // G underbar
    {0x0000, 0x00CA, 0x0000, 0x0000, 0x0000, L'h'                    }, // H underbar
    {0x0000, 0x00CB, 0x0000, 0x0000, 0x0000, L'i'                    }, // I underbar
    {0x0000, 0x00CC, 0x0000, 0x0000, 0x0000, L'j'                    }, // J underbar
    {0x0000, 0x00CD, 0x0000, 0x0000, 0x0000, L'k'                    }, // K underbar
    {0x0000, 0x00CE, 0x0000, 0x0000, 0x0000, L'l'                    }, // L underbar
    {0x0000, 0x00CF, 0x0000, 0x0000, 0x0000, L'm'                    }, // M underbar
    {0x0000, 0x00D0, 0x0000, 0x0000, 0x0000, L'n'                    }, // N underbar
    {0x0000, 0x00D1, 0x0000, 0x0000, 0x0000, L'o'                    }, // O underbar
    {0x0000, 0x00D2, 0x0000, 0x0000, 0x0000, L'p'                    }, // P underbar
    {0x0000, 0x00D3, 0x0000, 0x0000, 0x0000, L'q'                    }, // Q underbar
    {0x0000, 0x00D4, 0x0000, 0x0000, 0x0000, L'r'                    }, // R underbar
    {0x0000, 0x00D5, 0x0000, 0x0000, 0x0000, L's'                    }, // S underbar
    {0x0000, 0x00D6, 0x0000, 0x0000, 0x0000, L't'                    }, // T underbar
    {0x0000, 0x00D7, 0x0000, 0x0000, 0x0000, L'u'                    }, // U underbar
    {0x0000, 0x00D8, 0x0000, 0x0000, 0x0000, L'v'                    }, // V underbar
    {0x0000, 0x00D9, 0x0000, 0x0000, 0x0000, L'w'                    }, // W underbar
    {0x0000, 0x00DA, 0x0000, 0x0000, 0x0000, L'x'                    }, // X underbar
    {0x0000, 0x00DB, 0x0000, 0x0000, 0x0000, L'y'                    }, // Y underbar
    {0x0000, 0x00DC, 0x0000, 0x0000, 0x0000, L'z'                    }, // Z underbar
    {0x2018, 0x2018, 0x2018, 0x2018, 0x2018, UTF16_SINGLEQUOTE       }, // Left single quotation mark
    {0x2019, 0x2019, 0x2019, 0x2019, 0x2019, UTF16_SINGLEQUOTE       }, // Right ...
////{0x00A7, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_PARA              }, // Section
////{0x20AC, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_EURO              }, // Euro
    {0x00BA, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_EPSILONUNDERBAR   }, // Epsilon underbar     Missing from ISO charset
    {0x00AA, 0x00C0, 0x0000, 0x0000, 0x0000, UTF16_DIAMOND           }, // Diamond
    {0x00A8, 0x00A1, 0x0000, 0x0000, 0x0000, UTF16_DIERESIS          }, // Dieresis
    {0x201E, 0x00BC, 0x0000, 0x0000, 0x0000, UTF16_LEFTARROW         }, // Left arrow
    {0x203A, 0x00BB, 0x0000, 0x0000, 0x0000, UTF16_LEFTSHOE          }, // Left shoe
    {0x0153, 0x00BA, 0x0000, 0x0000, 0x0000, UTF16_RIGHTSHOE         }, // Right shoe
    {0x00B5, 0x00EB, 0x0000, 0x0000, 0x0000, UTF16_CIRCLESTAR        }, // Circle star
    {0x005E, 0x00A4, 0x0000, 0x0000, 0x0000, UTF16_UPCARET           }, // Up caret
////{0x00E5, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // a circle
////{0x00E6, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // ae dipthong
////{0x00EC, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // i left
    {0x0090, 0x00E8, 0x0000, 0x0000, 0x0000, UTF16_DELTILDE          }, // Del tilde
////{0x00F9, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // u left
////{0x00F2, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // o left
    {0x00D0, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_ZILDE             }, // Zilde                Missing from ISO charset
    {0x00BE, 0x00B9, 0x0000, 0x0000, 0x0000, UTF16_OMEGA             }, // Omega
    {0x2020, 0x00F5, 0x0000, 0x0000, 0x0000, UTF16_UPARROW           }, // Up arrow
    {0x2021, 0x00B7, 0x0000, 0x0000, 0x0000, UTF16_DOWNARROW         }, // Down arrow
    {0x2026, 0x00BE, 0x0000, 0x0000, 0x0000, UTF16_RIGHTARROW        }, // Right arrow
    {0x0081, 0x00C1, 0x0000, 0x0000, 0x0000, UTF16_LEFTTACK          }, // Left tack
    {0x00A4, 0x00BD, 0x0000, 0x0000, 0x0000, UTF16_RIGHTTACK         }, // Right tack
    {0x201C, 0x00E6, 0x0000, 0x0000, 0x0000, UTF16_DELTASTILE        }, // Delta stile
    {0x201D, 0x00E7, 0x0000, 0x0000, 0x0000, UTF16_DELSTILE          }, // Del stile
//  {0x00A6, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // broken stile
//  {0x00C7, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // C cedilla
//  {0x00FC, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // u umlaut
//  {0x00E9, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // e right
//  {0x00E2, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // a circumflex
//  {0x00E4, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // a umlaut
//  {0x00E0, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // a left
    {0x00AC, 0x00A5, 0x0000, 0x0000, 0x0000, UTF16_NOTEQUAL          }, // Not equal
//  {0x00E7, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // c cedilla
//  {0x00EA, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // e circumflex
//  {0x00EB, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // e umlaut
//  {0x00E8, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // e left
//  {0x00EF, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // i umlaut
//  {0x00EE, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // i circumflex
    {0x2014, 0x00B6, 0x0000, 0x0000, 0x0000, UTF16_UPSTILE           }, // Upstile
//  {0x00C4, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // A umlaut
    {0x02DC, 0x00AC, 0x0000, 0x0000, 0x0000, UTF16_DOWNSTILE         }, // Downstile
//  {0x00C9, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // E right
    {0x2018, 0x00AF, 0x0000, 0x0000, 0x0000, UTF16_DELTA             }, // Delta
    {0x00D7, 0x00A7, 0x0000, 0x0000, 0x0000, UTF16_TIMES             }, // Times
//  {0x00F4, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // o circumflex
//  {0x00F6, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // o umlaut
    {0x0152, 0x00B2, 0x0000, 0x0000, 0x0000, UTF16_QUAD              }, // Quad
//  {0x00FB, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // u circumflex
    {0x008D, 0x00E4, 0x0000, 0x0000, 0x0000, UTF16_QUOTEQUAD         }, // Quote-quad
    {0x017D, 0x00E1, 0x0000, 0x0000, 0x0000, UTF16_DOMINO            }, // Domino
//  {0x00D6, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // O umlaut
//  {0x00A2, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // cents
//  {0x00A3, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // pounds
//  {0x00A5, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // yen
    {0x00AE, 0x00EF, 0x0000, 0x0000, 0x0000, UTF16_COMMABAR          }, // commabar
    {0x00FE, 0x00A1, 0x0000, 0x0000, 0x0000, UTF16_DIERESISTILDE     }, // dieresis-tilde
//  {0x00E1, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // a right
//  {0x00ED, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // i right
//  {0x00F3, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // o right
//  {0x00FA, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // i right
//  {0x00F1, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // n tilde
//  {0x00D1, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // N tilde
    {0x00A9, 0x00DD, 0x0000, 0x0000, 0x0000, UTF16_LAMP              }, // Lamp
    {0x2122, 0x00F1, 0x0000, 0x0000, 0x0000, UTF16_SLOPEBAR          }, // slope-bar
//  {0x00BF, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // upside down query
    {0x00DE, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_SQUAD             }, // Squad                Missing from ISO charset
//  {0x00F5, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // o tilde
//  {0x00F8, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // 0 slash
//  {0x00FD, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // y right
//  {0x00A1, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // upside down shreik
//  {0x00AB, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // left chevron
//  {0x00BB, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // right chevron
//  {0x00C0, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // A left
//  {0x00C1, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // A right
//  {0x00C2, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // A circumflex
//  {0x00C3, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // A tilde
//  {0x00C4, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // A umlaut
//  {0x00C5, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // A circle
//  {0x00C6, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // AE dipthong
//  {0x00C7, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // C cedilla
//  {0x00C8, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // E left
//  {0x00C9, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // E right
//  {0x00CA, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // E circumflex
//  {0x00CB, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // E umlaut
//  {0x00CC, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // I left
//  {0x00CD, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // I right
//  {0x00CE, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // I circumflex
//  {0x00CF, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // I umlaut
//  {0x00D1, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // N tilde
//  {0x00D2, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // O left
//  {0x00D3, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // O right
//  {0x00D4, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // O circumflex
//  {0x00D5, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // O tilde
//  {0x00D6, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // O umlaut
//  {0x00D8, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // O slash
//  {0x00D9, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // U left
//  {0x00DA, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // U right
//  {0x00DB, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // U circumflex
//  {0x00DC, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // U umlaut
//  {0x00DD, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // Y right
//  {0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // y umlaut
    {0x00B8, 0x00A9, 0x0000, 0x0000, 0x0000, UTF16_ALPHA             }, // Alpha
//  {0x00DF, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // Beta
    {0x00BC, 0x00B0, 0x0000, 0x0000, 0x0000, UTF16_IOTA              }, // Iota
    {0x00F0, 0x00F8, 0x0000, 0x0000, 0x0000, UTF16_DIERESISJOT       }, // Dieresis jot
//  {0x00E3, 0x0000, 0x0000, 0x0000, 0x0000, UTF16_                  }, // a tilde
    {0x2039, 0x00E9, 0x0000, 0x0000, 0x0000, UTF16_DOWNCARETTILDE    }, // Downcaret tilde
    {0x0192, 0x00AA, 0x0000, 0x0000, 0x0000, UTF16_UPTACK            }, // Up tack
    {0x201A, 0x00B3, 0x0000, 0x0000, 0x0000, UTF16_DOWNTACK          }, // Down tack
    {0x00B2, 0x00EC, 0x0000, 0x0000, 0x0000, UTF16_CIRCLESTILE       }, // Circle stile
    {0x00B4, 0x00EE, 0x0000, 0x0000, 0x0000, UTF16_CIRCLEBAR         }, // Circle bar
    {0x0160, 0x00EA, 0x0000, 0x0000, 0x0000, UTF16_UPCARETTILDE      }, // Upcaret tilde
    {0x0161, 0x00F0, 0x0000, 0x0000, 0x0000, UTF16_SLASHBAR          }, // Slash bar
    {0x2019, 0x00AE, 0x0000, 0x0000, 0x0000, UTF16_DEL               }, // Del
    {0x00B3, 0x00ED, 0x0000, 0x0000, 0x0000, UTF16_CIRCLESLOPE       }, // Circle slope
    {0x00B9, 0x00AD, 0x0000, 0x0000, 0x0000, UTF16_EPSILON           }, // Epsilon
    {0x009D, 0x00AB, 0x0000, 0x0000, 0x0000, UTF16_UPSHOE            }, // Up shoe
    {0x00AD, 0x00F4, 0x0000, 0x0000, 0x0000, UTF16_EQUALUNDERBAR     }, // Equal underbar
    {0x008F, 0x00C2, 0x0000, 0x0000, 0x0000, UTF16_DELTAUNDERBAR     }, // Delta underbar
    {0x2030, 0x00BF, 0x0000, 0x0000, 0x0000, UTF16_RIGHTCARETUNDERBAR}, // Right caret underbar
    {0x02C6, 0x00A2, 0x0000, 0x0000, 0x0000, UTF16_LEFTCARETUNDERBAR }, // Left caret underbar
    {0x2022, 0x00E0, 0x0000, 0x0000, 0x0000, UTF16_DOWNTACKJOT       }, // Down tack jot
    {0x2013, 0x00DF, 0x0000, 0x0000, 0x0000, UTF16_UPTACKJOT         }, // Up tack jot
    {0x00F7, 0x00A6, 0x0000, 0x0000, 0x0000, UTF16_COLONBAR          }, // Colon bar
    {0x00B0, 0x00B1, 0x0000, 0x0000, 0x0000, UTF16_JOT               }, // Jot
    {0x00B1, 0x00B4, 0x0000, 0x0000, 0x0000, UTF16_CIRCLE            }, // Circle
    {0x0178, 0x00A3, 0x0000, 0x0000, 0x0000, UTF16_DOWNCARET         }, // Down caret
    {0x00BD, 0x00B5, 0x0000, 0x0000, 0x0000, UTF16_RHO               }, // Rho
    {0x017E, 0x00B8, 0x0000, 0x0000, 0x0000, UTF16_DOWNSHOE          }, // Down shoe
    {0x00AF, 0x00A8, 0x0000, 0x0000, 0x0000, UTF16_OVERBAR           }, // Overbar
    {0x007C, 0x007C, 0x0000, 0x0000, 0x0000, UTF16_STILE             }, // Stile
};

// The # rows in the above table
#define UNITRANS_NROWS  (sizeof (uniTrans) / sizeof (uniTrans[0]))

//***************************************************************************
//  End of File: Unitrans.h
//***************************************************************************
