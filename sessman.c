//***************************************************************************
//  NARS2000 -- Session Manager
//***************************************************************************

#pragma pack (1)
#define STRICT
#define _WIN32_WINNT  0x0500
#include <windows.h>
#include <colors.h>
#include "Unicode.h"
#include "main.h"
#include "datatype.h"
#include "resdebug.h"
#include "resource.h"
#include "symtab.h"
#include "tokens.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

/*

The Session Manager (SM) window consists of <iNumLines> lines of
APL statements, some block of which are displayed in the window
at any one time.

When the cursor moves to a line, the contents of the lpGlbHist
value are copied to <lpwszCurLine>.

1.  Assume that the user edits the line:
    a.  The edit changes are made to <lpwszCurLine>.
    b.  If the user hits Enter, the contents of
        <lpwszCurLine> are copied to lpGlbHist[iLastLine].
    c.  If the user moves the cursor off the current line,
        the contents of <lpwszCurLine> are copied to
        lpGlbHist[iCurLine].hGlb.
 */

// Default definitions
#define DEF_FONTNAME        "APL385 Unicode"
//efine DEF_FONTNAME        "SImPL"
#define DEF_INDENT          6           // Prompt indent
#define DEF_PTSIZE         13           // Point size for APL font
//efine DEF_FONTWEIGHT     FW_THIN      // Font weight
#define DEF_TABS            4           // Tab insertion
#define DEF_CURWID_INS      2           // Cursor width for insert mode
#define DEF_CURWID_REP      5           // ...              replace ...
#define DEF_HISTLINES    3000           // # lines in history buffer
#define DEF_MAXLINELEN   1024           // Maximum line length
#define DEF_TEXT_FG_COLOR   COLOR_RED
#define DEF_TEXT_BG_COLOR   COLOR_WHITE
#define DEF_NUMALP_MAXSIZE  64*1024*sizeof (char)   // Max size for lpszNumAlp
#define DEF_NUMALP_INITSIZE 64*1024*sizeof (char)   // Initial ...
#define DEF_STRING_MAXSIZE  64*1024*sizeof (WCHAR)  // Max size for lpwszString
#define DEF_STRING_INITSIZE 64*1024*sizeof (WCHAR)  // Initial ...
#define DEF_TOKENSTACK_MAXSIZE 64*1024*sizeof (TOKEN)   // Maximum size of token stack
#define DEF_TOKENSTACK_INITSIZE 64*1024*sizeof (TOKEN)  // Initial ...

#define SCROLL_FIRST        0           // Constants for ScrollHorz & ScrollVert
#define SCROLL_LAST         1           // ...

extern HWND hWndSM, hWndMF, hWndDB;
extern char pszAppName[];
extern HINSTANCE _hInstance;

HGLOBAL ghGlbToken;         // Save area for current token memory handle
HBITMAP hBitMapLineCont;    // Bitmap for the line continuation char
int     iLCWidth;           // Width of the line continuation column
BITMAP  bmLineCont;         // Bitmap metrics for the line continuation char

LPEXCEPTION_POINTERS lpExp;

COLORREF crTextColor = DEF_TEXT_FG_COLOR,
         crBkColor   = DEF_TEXT_BG_COLOR;

typedef struct
{
    char  nrm;      // Normal           (shifted & unshifted) (unused)
    WCHAR alt;      // Alt key pressed  (shifted & unshifted)
} CHARCODE;

CHARCODE aCharCode[] =          // This ordering follows the ASCII charset
{                               //   from 32 to 126 inclusive
//Nrm Alt
{' ', 0                   },    // Space             32
{'!', UCS2_EQUALUNDERBAR  },    // Shreik            33
{'"', 0                   },    // Quotation mark    34
{'#', UCS2_DELSTILE       },    // Number sign       35
{'$', UCS2_DELTASTILE     },    // Dollar sign       36
{'%', UCS2_CIRCLESTILE    },    // Percent sign      37
{'&', UCS2_CIRCLEBAR      },    // Ampersand         38
{'\'',UCS2_HYDRANT        },    // Apostrophe        39
{'(', UCS2_DOWNCARETTILDE },    // Left paren        40
{')', UCS2_UPCARETTILDE   },    // Right paren       41
{'*', UCS2_CIRCLESTAR     },    // Star              42
{'+', UCS2_DOMINO         },    // Plus sign         43
{',', UCS2_LAMP           },    // Comma             44
{'-', UCS2_TIMES          },    // Bar               45
{'.', UCS2_SLOPEBAR       },    // Dot               46
{'/', UCS2_SLASHBAR       },    // Slash             47
{'0', UCS2_UPCARET        },    // 0                 48
{'1', UCS2_DIERESIS       },    // 1                 49
{'2', UCS2_OVERBAR        },    // 2                 50
{'3', UCS2_LEFTCARET      },    // 3                 51
{'4', UCS2_NOTMORE        },    // 4                 52
{'5', UCS2_EQUAL          },    // 5                 53
{'6', UCS2_NOTLESS        },    // 6                 54
{'7', UCS2_RIGHTCARET     },    // 7                 55
{'8', UCS2_NOTEQUAL       },    // 8                 56
{'9', UCS2_DOWNCARET      },    // 9                 57
{':', 0                   },    // Colon             58
{';', UCS2_THORN          },    // Semicolon         59
{'<', 0                   },    // Less              60
{'=', UCS2_DIVIDE         },    // Equal             61
{'>', 0                   },    // More              62
{'?', 0                   },    // Query             63
{'@', UCS2_DELTILDE       },    // At sign           64
{'A', 0                   },    // A                 65
{'B', 0                   },    // B                 66
{'C', 0                   },    // C                 67
{'D', 0                   },    // D                 68
{'E', UCS2_EPSILONUNDERBAR},    // E                 69
{'F', 0                   },    // F                 70
{'G', 0                   },    // G                 71
{'H', UCS2_DELTAUNDERBAR  },    // H                 72
{'I', UCS2_IOTAUNDERBAR   },    // I                 73
{'J', 0                   },    // J                 74
{'K', 0                   },    // K                 75
{'L', UCS2_SQUAD          },    // L                 76
{'M', 0                   },    // M                 77
{'N', 0                   },    // N                 78
{'O', 0                   },    // O                 79
{'P', 0                   },    // P                 80
{'Q', 0                   },    // Q                 81
{'R', 0                   },    // R                 82
{'S', 0                   },    // S                 83
{'T', 0                   },    // T                 84
{'U', 0                   },    // U                 85
{'V', 0                   },    // V                 86
{'W', 0                   },    // W                 87
{'X', 0                   },    // X                 88
{'Y', 0                   },    // Y                 89
{'Z', 0                   },    // Z                 90
{'[', UCS2_LEFTARROW      },    // Left bracket      91
{'\\',UCS2_LEFTTACK       },    // Slope             92
{']', UCS2_RIGHTARROW     },    // Right bracket     93
{'^', UCS2_CIRCLESLOPE    },    // Up caret          94
{'_', UCS2_SHREIK         },    // Underbar          95
{'`', UCS2_DIAMOND        },    // Grave accent      96
{'a', UCS2_ALPHA          },    // a                 97
{'b', UCS2_DOWNTACK       },    // b                 98
{'c', UCS2_UPSHOE         },    // c                 99
{'d', UCS2_DOWNSTILE      },    // d                100
{'e', UCS2_EPSILON        },    // e                101
{'f', UCS2_UNDERBAR       },    // f                102
{'g', UCS2_DEL            },    // g                103
{'h', UCS2_DELTA          },    // h                104
{'i', UCS2_IOTA           },    // i                105
{'j', UCS2_JOT            },    // j                106
{'k', UCS2_APOSTROPHE     },    // k                107
{'l', UCS2_QUAD           },    // l                108
{'m', UCS2_STILE          },    // m                109
{'n', UCS2_UPTACK         },    // n                110
{'o', UCS2_CIRCLE         },    // o                111
{'p', UCS2_STAR           },    // p                112
{'q', UCS2_QUERY          },    // q                113
{'r', UCS2_RHO            },    // r                114
{'s', UCS2_UPSTILE        },    // s                115
{'t', UCS2_TILDE          },    // t                116
{'u', UCS2_DNARROW        },    // u                117
{'v', UCS2_DOWNSHOE       },    // v                118
{'w', UCS2_OMEGA          },    // w                119
{'x', UCS2_RIGHTSHOE      },    // x                120
{'y', UCS2_UPARROW        },    // y                121
{'z', UCS2_LEFTSHOE       },    // z                122
{'{', UCS2_QUOTEQUAD      },    // Left brace       123
{'|', UCS2_RIGHTTACK      },    // Stile            124
{'}', UCS2_ZILDE          },    // Right brace      125
{'~', UCS2_COMMABAR       },    // Tilde            126
};

/*

The vertical window and history buffer has several
points of interest -- all origin-0 indices into lpGlbHist:

---->       First line in History buffer        iFirstBufferLine (0)

---->       First line in the window            iFirstWindowLine

---->       Current line (always visible)       iCurLine

---->       Last line in the window             iLastWindowLine

---->       Last line in History buffer         iLastBufferLine (DEF_HISTLINES-1)

In addition, there is the index of the last valid line in the
history buffer (iLastValidLine), that is, the last line for
which memory is allocated.

iFirstBufferLine <= iCurLine <= (iLastValidLine + 1)
iLastValidLine <= iLastBufferLine


The horizontal window has several points of interest -- all
origin-0 indices into lpwszCurLine (transposed for convenience):

---->       First char in buffer                iFirstBufferChar (0)

---->       First char in the window            iFirstWindowChar

---->       Current char (always visible)       iCurChar

---->       Last char in the window             iLastWindowChar

---->       Last char in buffer                 iLastBufferChar (DEF_MAXLINELEN - 1)

In addition, there is the index of the last valid char in
lpwszCurLine (iCurLineLen), that is, lstrlenW (lpwszCurLine).

 */

int iFirstBufferLine = 0,       // Index into lpGlbHist of the first line
                                //   in the history buffer (ALWAYS 0).
    iLastBufferLine =           // Index into lpGlbHist of the last line
       DEF_HISTLINES-1,         //   in the history buffer (ALWAYS DEF_HISTLINES-1)
    iFirstWindowLine = 0,       // Index into lpGlbHist of the top line
                                //   in the window -- changed by vertical
                                //   scrolling.
    iCurLine = 0,               // Index into lpGlbHist of the line with
                                //   the cursor on it -- changed by up and
                                //   down cursor movements.
    iLastWindowLine,            // Index into lpGlbHist of the bottom line
                                //   in the window -- changed by vertical
                                //   scrolling or window resizing.
    iLastValidLine = -1,        // Index into lpGlbHist of the last line
                                //   which is defined (contents are not NULL)
                                //   -- changed by the user pressing Enter
                                //   (and thus executing a statement), or by
                                //   normal program or system command
                                //   output or by error messages.
    iFirstBufferChar = 0,       // Index into lpwszCurLine of the first char
                                //   in the buffer (ALWAYS 0)
    iFirstWindowChar,           // Index into lpwszCurLine of the first char
                                //   in the window -- changed by horizontal
                                //   scrolling.
    iCurChar,                   // Horizontal position of the caret (in chars)
                                // The vertical position is iCurLine - iFirstWindowLine (in chars).
    iLastWindowChar,            // Index into lpwszCurLine of the last char
                                //   in the window -- changed by horizontal
                                //   scrolling or window resizing.
    iLastBufferChar =           // Index into lpwszCurLine of the last char
       DEF_MAXLINELEN - 1,      //   in the buffer (always (DEF_MAXLINELEN - 1))
    iCurLineLen,                // Length of lpwszCurLine (not including the trailing zero)
    iLogPixelsX, iLogPixelsY;   // # logical pixels/inch in screen X- and Y-dimensions
LPWCHAR lpwszCurLine;           // The contents of the line
                                //   with the cursor on it.
                                // This contents of this var are changed
                                //   by editing the line in the window,
                                //   but the contents of lpGlbHist[iCurLine].hGlb
                                //   are not changed
WCHAR wszEmpty[] = {L'\0'};     // Empty wide string
LOGFONT lfAPL;                  // LOGFONT struc for the APL font
NEWTEXTMETRIC ntmAPL;           // NEWTEXTMETRIC ...
BOOL bAPLfont = FALSE;          // Flag used by FindAplFontEnumProc
LONG cxAveChar, cyAveChar;      // Size of an average character in the APL font

HGLOBAL hGlbHist;               // Global handle to array of history lines
HFONT hFontAPL;                 // Handle to APL font
CHOOSEFONT cfAPL;               // Global for ChooseFont

////LPTOKEN lptkStackBase;          // Ptr to base of token stack used in parsing

BOOL bCurLineChanged;           // Has the current line changed?

// Text selection variables
BOOL bSelText = FALSE;          // Are we selecting text?
int xSelText, ySelText,         // Starting x- & y-positions of selected text
    xSelChar, ySelChar;         // Starting x- & y- offsets into history buffer (in chars)
enum SELTYPE {SEL_CHARS, SEL_LINES, SEL_BLOCKS};
enum SELTYPE eSelType;          // Selection type
RECT rcSelInit, rcSelText;      // Selection rectangles

int cxWindowPixels,             // Size of SM client area in pixels
    cyWindowPixels,             // ...
    nWindowChars,               // ...                    in chars
    nWindowLines;               // ...                       lines
extern LPCHAR  lpszTemp;        // Used for temporary storage
extern LPWCHAR lpwszTemp;       // ...

typedef struct
{
    UINT  Shift:1,              // Left- or right-shift key up(0) or down(1)
////     lShift:1,
////     rShift:1,
          Alt:1,                // Left- or right-Alt key up(0) or down(1)
////     lAlt:1,
////     rAlt:1,
          Ctl:1,                // Left or -right Ctl key up(0) or down(1)
////     lCtl:1,
////     rCtl:1,
          Ins:1;                // Replace(0) or insert(1)
} VKSTATE;

VKSTATE vkState;

// Hash table variables
LPHSHENTRY lpHshTab,            // Ptr to start of hash table
           lpHshTabSplitNext;   // ...    next HTE to split (incremented by DEF_HSHTAB_NBLKS)

// Symbol table variables
LPSYMENTRY lpSymTab,            // Ptr to start of symbol table
           lpSymTabNext;        // Ptr to next available STE

LPCHAR  lpszNumAlp;             // Accumulator for integers & floating points & names
LPWCHAR lpwszString;            // ...             strings
int     iMaxNumAlp = DEF_NUMALP_MAXSIZE,    // Maximum # chars in lpszNumAlp
        iMaxString = DEF_STRING_MAXSIZE;    // ...       WCHARs in lpwszString


//***************************************************************************
//  SetAttrs
//
//  Set attributes for a DC
//***************************************************************************

void SetAttrs
    (HDC hDC)

{
    // Set the
    SetMapMode (hDC, MM_TEXT);

    // Select the font into the DC
    if (hFontAPL)
        SelectObject (hDC, hFontAPL);

    // Set the color of the foreground text
    SetTextColor (hDC, crTextColor);

    // Set the color of the background text
    SetBkColor   (hDC, crBkColor);
} // End SetAttrs


//***************************************************************************
//  InitChooseFont
//
//  Initialize CHOOSEFONT values
//***************************************************************************

void InitChooseFont
    (void)
{
    memset (&cfAPL, 0, sizeof (CHOOSEFONT));

    cfAPL.lStructSize = sizeof (CHOOSEFONT);
////cfAPL.hDC =                     // Only w/CF_PRINTERFONTS
    cfAPL.lpLogFont = &lfAPL;
    cfAPL.iPointSize = DEF_PTSIZE;  // APL Font point size
////cfAPL.iPointSize =              // Output only
    cfAPL.Flags = CF_INITTOLOGFONTSTRUCT
                | CF_FORCEFONTEXIST
                | CF_SCREENFONTS;
////cfAPL.rgbColors =               // Only w/CF_EFFECTS
////cfAPL.lCustData =               // Only w/CF_ENABLEHOOK
////cfAPL.lpfnHook =                // Only w/CF_ENABLEHOOK
////cfAPL.lpTemplateName =          // Only w/CF_ENABLETEMPLATE
////cfAPL.hInstance =               // Only w/CF_ENABLETEMPLATE
////cfAPL.lpszStyle =               // Only w/CF_USESTYLE
////cfAPL.nFontType =               // Output only
////cfAPL.nSizeMin =                // Only w/CF_LIMITSIZE
////cfAPL.nSizeMax =                // Only w/CF_LIMITSIZE

} // End InitChooseFont


//***************************************************************************
//  EnumFontFamilies callback function
//***************************************************************************

int CALLBACK FindAplFontEnumProc
    (ENUMLOGFONT *lpELF,    // Ptr to ENUMLOGFONT struc
     NEWTEXTMETRIC *lpNTM,  // Ptr to TEXTMETRIC struc
     int iFontType,         // Font type
     LPARAM lParam)         // Application-defined data
{
    // We found a match
    // Copy the LOGFONT struc to the data area
    memmove (&lfAPL, &lpELF->elfLogFont, sizeof (lfAPL));

    // Copy the NEWTEXTMETRIC struc to the data area
    memmove (&ntmAPL, lpNTM, sizeof (ntmAPL));

    bAPLfont = TRUE;        // Mark as found

    return FALSE;           // Stop enumerating
} // End FindAplFontEnumProc


//***************************************************************************
//  MyChooseFont
//
//  Choose a font
//***************************************************************************

void MyChooseFont
    (void)

{
    if (ChooseFont (&cfAPL))    // Choose it
    {
        HDC hDC;

        // ChooseFont helpfully returns the point size
        //   in 1/10th of a point.  Rescale it here.
        cfAPL.iPointSize /= 10;

        // Call EnumFontFamilies because ChooseFont
        //   helpfully zeros lfAPL.iPointSize, so
        //   we need to fill it in.
        hDC = MyGetDC (hWndSM);
        SetAttrs (hDC);
        EnumFontFamilies (hDC,
                          lfAPL.lfFaceName,
                          (FONTENUMPROC) FindAplFontEnumProc,
                          0);
        // Release the DC
        MyReleaseDC (hWndSM, hDC); hDC = NULL;

        // Initialize variables for a new font
        NewFont ();

        // Repaint the SM window
        InvalidateRect (hWndSM, NULL, TRUE);
    } // End IF
} // End MyChooseFont


//***************************************************************************
//  NewFont
//
//  Initialize variables for a new font.
//***************************************************************************

void NewFont
    (void)

{
    // New height
    cyAveChar = MulDiv (cfAPL.iPointSize, iLogPixelsY, 72);

    // New width (same aspect ratio as old)
    cxAveChar = MulDiv (lfAPL.lfWidth, cyAveChar, lfAPL.lfHeight);
    lfAPL.lfWidth  =  cxAveChar;
    lfAPL.lfHeight = -cyAveChar;
////lfAPL.lfWeight = DEF_FONTWEIGHT;

    // Because cxAveChar & cyAveChar changed, we need to reposition
    //   the caret as it depends upon those two vars.
    MoveCaret ();

    // Recalculate the # horizontal characters
    //   and vertical lines.
    nWindowChars = cxWindowPixels / cxAveChar;
    nWindowLines = cyWindowPixels / cyAveChar;

    // Delete the previous handle (if any)
    if (hFontAPL)
    {
        // Delete the APL font handle
        MyDeleteObject (hFontAPL); hFontAPL = NULL;
    } // End IF

    // Create the font we'll use
    hFontAPL = MyCreateFontIndirect (&lfAPL);

    // Use a monospace font in the debugger's listbox
    PostMessage (hWndDB, WM_SETFONT, (WPARAM) hFontAPL, 0);
} // End NewFont


//***************************************************************************
//  SetVScrollRange
//
//  Set the vertical scroll range
//***************************************************************************

void SetVScrollRange
    (void)

{
    SetScrollRange (hWndSM, SB_VERT, iFirstBufferLine, iLastValidLine + 1, TRUE);
} // End SetVScrollRange


//***************************************************************************
//  SetHScrollRange
//
//  Set the horizontal scroll range
//***************************************************************************

void SetHScrollRange
    (void)

{
    SetScrollRange (hWndSM, SB_HORZ, iFirstBufferChar, iLastBufferChar, TRUE);
} // End SetHScrollRange


//***************************************************************************
//  SetVScrollPos
//
//  Set the vertical scroll position
//***************************************************************************

void SetVScrollPos
    (void)

{
    SetScrollPos (hWndSM, SB_VERT, iFirstWindowLine, TRUE);
} // End SetVScrollPos


//***************************************************************************
//  SetHScrollPos
//
//  Set the horizontal scroll position
//***************************************************************************

void SetHScrollPos
    (void)

{
    SetScrollPos (hWndSM, SB_HORZ, iFirstWindowChar, TRUE);
} // End SetHScrollPos


//***************************************************************************
//  ScrollVert
//
//  Scroll the window vertically
//***************************************************************************

void ScrollVert
    (BOOL bScroll,
     int iLineCnt)

{
    int iNewWindowLine;

    // Set new first & last window lines
    switch (bScroll)
    {
        case SCROLL_FIRST:
////////////wsprintf (lpszTemp,
////////////          "VSCROLL_FIRSTa:  %d",
////////////          iLineCnt);
////////////DbgMsg (lpszTemp);

            // Calculate the new position
            iNewWindowLine = iFirstWindowLine + iLineCnt;

            if (iNewWindowLine < iFirstBufferLine)
                iLineCnt += iFirstBufferLine - iNewWindowLine;

            if ((iNewWindowLine + nWindowLines) > iLastBufferLine)
                iLineCnt += iLastBufferLine - (iNewWindowLine + nWindowLines);

            iFirstWindowLine += iLineCnt;
            iLastWindowLine   = iFirstWindowLine + nWindowLines;

////        wsprintf (lpszTemp,
////                  "VSCROLL_FIRSTz:  %d",
////                  iLineCnt);
////        DbgMsg (lpszTemp);

            break;

        case SCROLL_LAST:
////////////wsprintf (lpszTemp,
////////////          "VSCROLL_LASTa:  %d",
////////////          iLineCnt);
////////////DbgMsg (lpszTemp);

            // Calculate the new position
            iNewWindowLine = iLastWindowLine + iLineCnt;

            if (iNewWindowLine < iFirstBufferLine)
                iLineCnt += iFirstBufferLine - iNewWindowLine;

            if ((iNewWindowLine + nWindowLines) > iLastBufferLine)
                iLineCnt += iLastBufferLine - (iNewWindowLine + nWindowLines);

            iLastWindowLine += iLineCnt;
            iFirstWindowLine = iLastWindowLine - nWindowLines;

////        wsprintf (lpszTemp,
////                  "VSCROLL_LASTz:  %d",
////                  iLineCnt);
////        DbgMsg (lpszTemp);

            break;
    } // End SWITCH

    // For vertical scrolling only, don't let
    //   the first line get beyond iLastValidLine + 1
    if (iFirstWindowLine > (iLastValidLine + 1))
    {
        iFirstWindowLine = iLastValidLine + 1;
        iLastWindowLine  = iFirstWindowLine + nWindowLines;
    } // End IF

    // Ensure first & last are within buffer limits
    iFirstWindowLine  = max (iFirstWindowLine, iFirstBufferLine);
    iLastWindowLine   = min (iLastWindowLine,  iLastBufferLine);

    // If the current line has changed,
    //   save it into the history buffer
    if (bCurLineChanged)
        ReplaceLine (iCurLine);

    // Ensure the caret is always visible
    if (iCurLine < iFirstWindowLine)
        iCurLine = iFirstWindowLine;
    else
    if (iCurLine > iLastWindowLine)
        iCurLine = iLastWindowLine;

    // Copy the current line from the
    //   history buffer into lpwszCurLine;
    CopyLine (iCurLine);

    // In case we changed iCurLine or iFirstWindowLine,
    //   move the caret
    MoveCaret ();

    // As we changed iFirstWindowLine,
    //   move the thumb on the scrollbar
    SetVScrollPos ();

    // Redraw the entire client area
    InvalidateRect (hWndSM, NULL, TRUE);
} // End ScrollVert


//***************************************************************************
//  ScrollHorz
//
//  Scroll the window horizontally
//***************************************************************************

void ScrollHorz
    (BOOL bScroll,
     int iCharCnt)

{
    int iNewWindowChar;

    // Horizontal scrolling should be relative to the longest line in the history buffer
    // ***FIXME***


    // Set new first & last window chars
    switch (bScroll)
    {
        case SCROLL_FIRST:
////        wsprintf (lpszTemp,
////                  "HSCROLL_FIRST:  %d",
////                  iCharCnt);
////        DbgMsg (lpszTemp);

            // Calculate the new position
            iNewWindowChar = iFirstWindowChar + iCharCnt;

            if (iNewWindowChar < iFirstBufferChar)
                iCharCnt += iFirstBufferChar - iNewWindowChar;

            if ((iNewWindowChar + nWindowChars) > iLastBufferChar)
                iCharCnt += iLastBufferChar - (iNewWindowChar + nWindowChars);

            iFirstWindowChar += iCharCnt;
            iLastWindowChar   = iFirstWindowChar + nWindowChars;

            break;

        case SCROLL_LAST:
////        wsprintf (lpszTemp,
////                  "HSCROLL_LAST:  %d",
////                  iCharCnt);
////        DbgMsg (lpszTemp);

            // Calculate the new position
            iNewWindowChar = iLastWindowChar + iCharCnt;

            if (iNewWindowChar < iFirstBufferChar)
                iCharCnt += iFirstBufferChar - iNewWindowChar;

            if ((iNewWindowChar + nWindowChars) > iLastBufferChar)
                iCharCnt += iLastBufferChar - (iNewWindowChar + nWindowChars);

            iLastWindowChar += iCharCnt;
            iFirstWindowChar = iLastWindowChar - nWindowChars;

            break;
    } // End SWITCH

    // Ensure first & last are within buffer limits
    iFirstWindowChar  = max (iFirstWindowChar, iFirstBufferChar);
    iLastWindowChar   = min (iLastWindowChar,  iLastBufferChar);

    // In case we changed iFirstWindowChar,
    //   move the caret
    MoveCaret ();

    // In case we changed iFirstWindowChar,
    //   move the thumb on the scrollbar
    SetHScrollPos ();

    // Redraw the entire client area
    InvalidateRect (hWndSM, NULL, TRUE);
} // End ScrollHorz


//***************************************************************************
//  AppendLine
//
//  Append lpwszCurLine to the history buffer
//***************************************************************************

void AppendLine
    (LPWCHAR lpwszLine,
     BOOL    bLineCont)

{
    // Ensure there are enough lines or we need to wrap the buffer
    // ***FIXME*** -- implement a circular history buffer

    lstrcpyW (lpwszCurLine, lpwszLine);
    iCurLineLen = lstrlenW (lpwszCurLine);
    bCurLineChanged = TRUE;

////wsprintf (lpszTemp,
////          "lstrlenW (lpwszCurLine) = %d",
////          lstrlenW (lpwszCurLine));
////DbgMsg (lpszTemp);

    ReplaceLine (iLastValidLine + 1);   // iLastValidLine changed

    // Account for a new line
    iCurLine = iLastValidLine + 1;

    // As we changed iLastValidLine,
    //   reset the scrollbar range.
    SetVScrollRange ();

    // Initialize the Current Line
    InitCurLine (bLineCont);

    // As we changed iCurLine, move the caret
    MoveCaret ();

    // Scroll the window as necessary
    // The "+1" is to cover the line the cursor is on
    if ((iCurLine + 1) > iLastWindowLine)
        ScrollVert (SCROLL_LAST, (iCurLine + 1) - iLastWindowLine);
} // End AppendLine


//***************************************************************************
//  ReplaceLine
//
//  Replace lpwszCurLine in the history buffer
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ReplaceLine"
#else
#define APPEND_NAME
#endif

void ReplaceLine
    (int iLine)

{
    LPGLBHIST lpGlbHist;
    HGLOBAL   hGlb;
    LPWCHAR   wszLine;

    // Get the global handle to the history buffer
    lpGlbHist = (LPGLBHIST) MyGlobalLock (hGlbHist);

////wsprintf (lpszTemp,
////          "iLine = %d, iLastValidLine = %d",
////          iLine,
////          iLastValidLine);
////DbgMsg (lpszTemp);

    // Free the old entry (if any)
    if (iLine <= iLastBufferLine
     && lpGlbHist[iLine].hGlb)
    {
        DbgGlobalFree (lpGlbHist[iLine].hGlb); lpGlbHist[iLine].hGlb = NULL;
    } else
    {
        // Increase iLastValidLine if necessary
        iLastValidLine = max (iLastValidLine, iLine);
    } // End IF/ELSE

    // Allocate memory for the last line
    hGlb = MyGlobalAlloc (GHND, (iCurLineLen + 1) * sizeof (WCHAR));

    if (!hGlb)
    {
        // ***FIXME*** -- WS FULL
        DbgMsg ("ASSERTION FAILED:  ReplaceLine:  MyGlobalAlloc failed");
    } else
    {
////////wsprintf (lpszTemp,
////////          "iLastValidLine = %d",
////////          iLastValidLine);
////////DbgMsg (lpszTemp);

        // Save the handle for later use
        lpGlbHist[iLine].hGlb = hGlb;

        // Get a ptr to the memory
        wszLine = (LPWCHAR) MyGlobalLock (hGlb);

        // Copy the contents of lpwszCurLine to global memory
        memmove (wszLine,
                 lpwszCurLine,
                 iCurLineLen * sizeof (WCHAR));

        bCurLineChanged = FALSE;

        // Unlock the handle
        MyGlobalUnlock (hGlb); wszLine = NULL;
    } // End IF/ELSE

    // Unlock the handle
    MyGlobalUnlock (hGlbHist); lpGlbHist = NULL;

    // Redraw the line
    InvalidateLine (iLine);
} // End ReplaceLine
#undef  APPEND_NAME


//***************************************************************************
//  DeleteLine
//
//  Delete a line from the history buffer
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- DeleteLine"
#else
#define APPEND_NAME
#endif

void DeleteLine
    (int iLine)

{
    LPGLBHIST lpGlbHist;

    // Get the global handle to the history buffer
    lpGlbHist = (LPGLBHIST) MyGlobalLock (hGlbHist);

    // Free the old entry (if any)
    if (lpGlbHist[iLine].hGlb)
    {
        DbgGlobalFree (lpGlbHist[iLine].hGlb); lpGlbHist[iLine].hGlb = NULL;
    } // End IF

////wsprintf (lpszTemp,
////          "DeleteLine:  iLine = %d, iLastValidLine = %d",
////          iLine,
////          iLastValidLine);
////DbgMsg (lpszTemp);

    // Move the entries down over the deleted one
    if (iLine <= iLastValidLine)
    {
        memmove (&lpGlbHist[iLine],
                 &lpGlbHist[iLine + 1],
                 (iLastValidLine - iLine) * sizeof (WCHAR));
        lpGlbHist[iLastValidLine].hGlb = NULL;
        lpGlbHist[iLastValidLine].ContPrev =
        lpGlbHist[iLastValidLine].ContNext = FALSE;
    } // End IF

    if (iLine <= iLastValidLine)
    {
        // Set last valid line
        iLastValidLine--;
////////DbgMsg ("Decrementing iLastValidLine");

        // As we changed iLastValidLine,
        //   reset the scrollbar range.
        SetVScrollRange ();
    } // End IF

    // Unlock the handle
    MyGlobalUnlock (hGlbHist); lpGlbHist = NULL;
} // End DeleteLine
#undef  APPEND_NAME


//***************************************************************************
//  CopyLine
//
//  Copy a line from the history buffer into lpwszCurLine.
//***************************************************************************

void CopyLine
    (int iLine)

{
    LPGLBHIST lpGlbHist;
    HGLOBAL   hGlb;
    LPWCHAR   wszLine;

    // Get the global handle to the history buffer
    lpGlbHist = (LPGLBHIST) MyGlobalLock (hGlbHist);

    // If the memory exists, copy the contents to the local buffer
    if (lpGlbHist[iLine].hGlb)
    {
        // Lock the memory
        hGlb = lpGlbHist[iLine].hGlb;
        wszLine = (LPWCHAR) MyGlobalLock (hGlb);

        // Set the line length
        iCurLineLen = lstrlenW (wszLine);

        // Copy the contents of global memory to lpwszCurLine
        memmove (lpwszCurLine,
                 wszLine,
                 (iCurLineLen + 1) * sizeof (WCHAR));
        // Unlock the handle
        MyGlobalUnlock (hGlb); wszLine = NULL;

        bCurLineChanged = FALSE;
    } else
        // Otherwise, initialize the local buffer
        InitCurLine (lpGlbHist->ContPrev);

    // Unlock the handle
    MyGlobalUnlock (hGlbHist); lpGlbHist = NULL;
} // End CopyLine


//***************************************************************************
//  SM_Create
//
//  Perform window-specific initialization
//***************************************************************************

void SM_Create
    (HWND hWnd)

{
} // End SM_Create


//***************************************************************************
//  SM_Delete
//
//  Perform window-specific uninitialization
//***************************************************************************

void SM_Delete
    (HWND hWnd)

{
} // End SM_Delete


//***************************************************************************
//  DrawLineCont
//
//  Draw a line continuation char
//***************************************************************************

void DrawLineCont
    (HDC hDC,
     int iLineNum)

{
    DrawBitmap (hDC,
                hBitMapLineCont,
                0,
                (iLineNum * cyAveChar)
              + (cyAveChar - bmLineCont.bmHeight) / 2   // Vertically centered
               );
} // End DrawLineCont


//***************************************************************************
//  DrawLine
//
//  Draw a line in the window from within a WM_PAINT message.
//***************************************************************************

void DrawLine
     (LPWCHAR lpwszLine,
      int     iLine,
      HDC     hDC,
      BOOL    bLineCont)
{
    RECT rc, rcInter;
    int iLen;

    iLen = lstrlenW (lpwszLine);

    rc.left   = iLCWidth;
    rc.right  = rc.left + cxAveChar * iLen;
    rc.top    = iLine   * cyAveChar;
    rc.bottom = rc.top  + cyAveChar;

    if (bSelText)
    {
        // ***FIXME***
        // Handle V & H scrolling
        // Handle change of screen font (point size)
        switch (eSelType)
        {
            case SEL_CHARS:
                break;

            case SEL_LINES:
                if (IntersectRect (&rcInter, &rc, &rcSelText))
                {
                } // End IF

                break;

            case SEL_BLOCKS:
                break;
        } // End SWITCH
    } else
    {
        // Draw the text
        DrawTextW (hDC,
                   lpwszLine,
                   iLen,
                   &rc,
                   DT_NOPREFIX
                 | DT_SINGLELINE
                 | DT_NOCLIP
              );
        if (bLineCont)
            // Draw a continuation char
            DrawLineCont (hDC, iLine);
    } // End IF/ELSE
} // End DrawLine


//***************************************************************************
//  DrawBitmap
//
//  Draw a bitmap
//***************************************************************************

void DrawBitmap
    (HDC     hDC,
     HBITMAP hBitmap,
     UINT    xDstOrg,
     UINT    yDstOrg) // Destin bit origin (upper left corner)

{
    BITMAP  bm;
    HDC     hDCMem;
    POINT   ptSize, ptOrg;
    HBITMAP hBitmapMem, hBitmapOld;

    // Get the size of the bitmap
    GetObject (hBitmap, sizeof (BITMAP), (LPSTR) &bm);

    // Create a compatible DC and bitmap
    hDCMem = MyCreateCompatibleDC (hDC);    // Get device context handle
#ifdef USE_COPYIMAGE
    hBitmapMem = CopyImage (hBitmap, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG);
    hBitmapOld = SelectObject (hDCMem, hBitmapMem);
#else
    // To avoid screen flicker, we use a temporary DC
    hBitmapMem = MyCreateCompatibleBitmap (hDC,
                                           bm.bmWidth,
                                           bm.bmHeight);
    hBitmapOld = SelectObject (hDCMem, hBitmapMem);

    {
        HDC hDCTmp;
        HBITMAP hBitmapTmp;

        // Create a temporary compatible DC
        // and select our bitmap into it
        hDCTmp = MyCreateCompatibleDC (hDC);
        hBitmapTmp = SelectObject (hDCTmp, hBitmap);

        // Copy the original bitmap from the temporary DC to the memory DC
        BitBlt (hDCMem,
                0,
                0,
                bm.bmWidth,
                bm.bmHeight,
                hDCTmp,
                0,
                0,
                SRCCOPY);
        SelectObject (hDCTmp, hBitmapTmp);
        MyDeleteDC (hDCTmp); hDCTmp = NULL;
    }
#endif
    SetMapMode (hDCMem, GetMapMode (hDC));  // Set the mapping mode

    // Convert the bitmap size from device units to logical units
    ptSize.x = bm.bmWidth;
    ptSize.y = bm.bmHeight;
    DPtoLP (hDC, &ptSize, 1);

    ptOrg.x = ptOrg.y = 0;
    DPtoLP (hDCMem, &ptOrg, 1);

    // Copy the memory DC to the screen DC
    BitBlt (hDC,
            xDstOrg, yDstOrg,
            ptSize.x, ptSize.y,
            hDCMem,
            ptOrg.x, ptOrg.y,
            SRCCOPY);
    // Put the old one in place before we delete the DC
    //   or we'll delete the new bitmap when we delete the DC.
    SelectObject (hDCMem, hBitmapOld);

    // Free resources
    MyDeleteObject (hBitmapMem); hBitmapMem = NULL;
    MyDeleteDC (hDCMem); hDCMem = NULL;
} // End DrawBitmap ()


//***************************************************************************
//  strchrW
//
//  The wide form of <strchr>
//***************************************************************************

LPWCHAR strchrW
    (LPWCHAR wp,
     WCHAR   wch)

{

    for (; *wp; wp++)
    if (wch EQ *wp)
        return wp;

    return NULL;
} // End strchrW


//***************************************************************************
//  InvalidateLine
//
//  Invalidate a single line so W calls WM_PAINT
//    to redraw the line.
//***************************************************************************

void InvalidateLine
    (int iLine)

{
    RECT rc;

    rc.left = 0;
    rc.right  = 65535;      // Use any non-zero value as the
                            //   code in WM_PAINT looks at the
                            //   rc.top and rc.bottom only
    rc.top    = (iLine - iFirstWindowLine) * cyAveChar;
    rc.bottom = rc.top                     + cyAveChar;

////wsprintf (lpszTemp,
////          "InvalidateLine:  %d, L = %d, T = %d, R = %d, B = %d",
////          iLine,
////          rc.left,
////          rc.top,
////          rc.right,
////          rc.bottom);
////DbgMsg (lpszTemp);

#if (defined (DEBUG)) && 0
    { // ***DEBUG***
        wsprintf (lpszTemp,
                  "InvalidateLine:  %d",
                  iLine);
        DbgMsg (lpszTemp);
    } // ***DEBUG*** END
#endif

    InvalidateRect (hWndSM, &rc, TRUE);
} // End InvalidateLine


//***************************************************************************
//  InvalidateRange
//
//  Invalidate a range of lines so W calls WM_PAINT
//    to redraw the line.
//***************************************************************************

void InvalidateRange
    (int iTopLine,
     int iBotLine)

{
    RECT rc;

    rc.left = 0;
    rc.right  = 65535;      // Use any non-zero value as the
                            //   code in WM_PAINT looks at the
                            //   rc.top and rc.bottom only
    rc.top    = (    iTopLine - iFirstWindowLine) * cyAveChar;
    rc.bottom = (1 + iBotLine - iFirstWindowLine) * cyAveChar;

////wsprintf (lpszTemp,
////          "InvalidateRange:  %d - %d, L = %d, T = %d, R = %d, B = %d",
////          iTopLine, iBotLine,
////          rc.left,
////          rc.top,
////          rc.right,
////          rc.bottom);
////DbgMsg (lpszTemp);

#if (defined (DEBUG)) & 0
    { // ***DEBUG***
        wsprintf (lpszTemp,
                  "InvalidateRange:  %d-%d",
                  iTopLine,
                  iBotLine);
        DbgMsg (lpszTemp);
    } // ***DEBUG*** END
#endif
    InvalidateRect (hWndSM, &rc, TRUE);
} // End InvalidateRange


//***************************************************************************
//  MoveCaret
//
//  Move the caret into position
//***************************************************************************

void MoveCaret
    (void)

{
    SetCaretPos ((iCurChar - iFirstWindowChar) * cxAveChar + iLCWidth,
                 (iCurLine - iFirstWindowLine) * cyAveChar);
} // End MoveCaret


//***************************************************************************
//  DisplayChar
//
//  Display a single character by inserting it into the current line
//  and redrawing that line.
//***************************************************************************

void DisplayChar
    (WCHAR chCharCode)

{
    int i;

// Line Continuation ToDo
// Handle in the middle of a line (split in two)
// Handle beyond the end of the line (append trailing blanks
//   and create a second line)



    // Split case depending upon whether we're
    //   before or beyond the end of the line
    if (iCurChar < iCurLineLen)
    {
        // Handle Insert vs. Replace
        if (vkState.Ins)
        {
            // Ensure the line doesn't get too long
            if ((iCurLineLen + 1) > iLastBufferChar)
            {
                // ***FIXME*** -- LINE TOO LONG
                DbgMsg ("DisplayChar:  LINE TOO LONG");

                return;
            } // End IF

            // Make room at iCurChar for a new character
            memmove (&lpwszCurLine[iCurChar + 1],
                     &lpwszCurLine[iCurChar],
                     (iCurLineLen - iCurChar) * sizeof (WCHAR));
            iCurLineLen++;
        } // End IF
    } else
    if (iCurChar > iCurLineLen)
    {
        // Insert blanks from iCurLineLen to iCurChar
        for (i = iCurLineLen; i < iCurChar; i++)
            lpwszCurLine[i] = L' ';
        iCurLineLen = iCurChar;
    } // End IF/ELSE/IF

    // Insert the character
    lpwszCurLine[iCurChar] = chCharCode;
    bCurLineChanged = TRUE;

    // Set new caret position
    iCurChar++;

    // As we changed iCurChar, move the caret
    MoveCaret ();

    // Set new line length
    iCurLineLen = max (iCurLineLen, iCurChar);

    // Ensure properly terminated
    lpwszCurLine[iCurLineLen] = L'\0';

    // Scroll the window as necessary
    if (iCurChar > iLastWindowChar)
        ScrollHorz (SCROLL_LAST, iCurChar - iLastWindowChar);
    else
        // Invalidate the line so it gets repainted
        InvalidateLine (iCurLine);
} // End DisplayChar


//***************************************************************************
//  DeleteChar
//
//  Delete the character at iCurChar
//***************************************************************************

void DeleteChar
    (void)
{
    LPGLBHIST lpGlbHist;
    HGLOBAL   hGlb;
    LPWCHAR   wszLine;
    int iLen;

    // In case the user moved the cursor beyond
    //   the end of the line (i.e. lstrlen (lpwszCurLine)),
    //   we need to ensure we don't fill in a
    //   non-existant blank.
    if (iCurChar < iCurLineLen)
    {
        // Erase the char in that position
        memmove (&lpwszCurLine[iCurChar],
                 &lpwszCurLine[iCurChar + 1],
                 (iCurLineLen - iCurChar - 1) * sizeof (WCHAR));
        iCurLineLen--;
        lpwszCurLine[iCurLineLen] = L'\0';
        bCurLineChanged = TRUE;

        // Invalidate the line so it gets repainted
        InvalidateLine (iCurLine);
    } else
    if (iCurChar EQ iCurLineLen
     && iCurLine NE (iLastValidLine + 1))
    {
        // We're at the end of a line
        //   with one following line:
        //   merge it into this current line.

        // Get the global handle to the history buffer
        lpGlbHist = (LPGLBHIST) MyGlobalLock (hGlbHist);

        // Get a ptr to the contents of the current line
        hGlb = lpGlbHist[iCurLine + 1].hGlb;
        if (hGlb)
        {
            wszLine = (LPWCHAR) MyGlobalLock (hGlb);
        } else
            wszLine = wszEmpty;

        // Ensure the resulting line isn't too long
        iLen = lstrlenW (wszLine);
        if ((iCurLineLen + iLen) > iLastBufferChar)
        {
            // ***FIXME*** -- LINE TOO LONG
            DbgMsg ("DeleteChar:  LINE TOO LONG");
        } else
        {
            // Set new current line length
            iCurLineLen += iLen;

            // Merge iCurLine and iCurLine+1
            lstrcatW (lpwszCurLine, wszLine);
            ReplaceLine (iCurLine);
            bCurLineChanged = TRUE;

            // Unlock the memory before deleting it
            if (hGlb)
            {
                MyGlobalUnlock (hGlb); wszLine = NULL;
            } // End IF

            // Delete iCurLine + 1
            DeleteLine (iCurLine + 1); hGlb = NULL;

            // Redraw this line and the ones below it
            InvalidateRange (iCurLine, iLastValidLine + 1);
        } // End IF/ELSE

        // Unlock the handles
        if (hGlb)
        {
            MyGlobalUnlock (hGlb); wszLine = NULL;
        } // End IF
        MyGlobalUnlock (hGlbHist); lpGlbHist = NULL;
    } // End IF/ELSE/IF
} // End DeleteChar


//***************************************************************************
//  MyCreateCaret
//
//  Create a caret
//***************************************************************************

void MyCreateCaret
    (void)
{
    // Create a default sized system caret for display
    CreateCaret (hWndSM,
                 NULL,
                 vkState.Ins ? DEF_CURWID_INS : DEF_CURWID_REP,
                 cyAveChar);
    // Position it
    MoveCaret ();

    // Show it
    ShowCaret (hWndSM);
} // End MyCreateCaret


//***************************************************************************
//  InitCurLine
//
//  Initialize the current line
//***************************************************************************

void InitCurLine
    (BOOL bLineCont)
{
    int i;

    for (i = 0; (!bLineCont) && i < DEF_INDENT; i++)
        lpwszCurLine[i] = L' ';
    lpwszCurLine[i] = L'\0';
    iCurChar = iCurLineLen = i;

    bCurLineChanged = FALSE;
} // End InitCurLine


//***************************************************************************
//  SMWndProc
//
//  Message processing routine for the Session Manager window
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SMWndProc"
#else
#define APPEND_NAME
#endif

LRESULT APIENTRY SMWndProc
    (HWND hWnd,     // Window handle
     UINT message,  // Type of message
     UINT wParam,   // Additional information
     LONG lParam)   // ...

{
    LPGLBHIST lpGlbHist;
    HGLOBAL   hGlb;
    LPWCHAR   wszLine;
    RECT      rc;

////ODSAPI ("SM: ", hWnd, message, wParam, lParam);
    switch (message)
    {
#define chCharCode ((char) wParam)
        case WM_CHAR:               // chCharCode = (TCHAR) wParam; // character code
                                    // lKeyData = lParam;           // Key data
        {
            // Handle Unshifted and Shifted chars
            //  e.g., 'a' = 97, 'z' = 122
            //        'A' = 65, 'Z' =  90

            // Process the character code
            switch (chCharCode)
            {
                case VK_BACK:           // Backspace
                    if (iCurChar > iFirstBufferChar)
                    {
                        // We're past the first char in the buffer

                        // Delete the preceding char

                        // Set new caret position
                        iCurChar--;

                        // As we changed iCurChar, move the caret
                        MoveCaret ();   // Move the caret

                        // Delete the char at iCurChar
                        DeleteChar ();

                        // Scroll the window as necessary
                        if (iCurChar < iFirstWindowChar)
                            ScrollHorz (SCROLL_FIRST, iCurChar - iFirstWindowChar);
                    } else
                    {
                        // We're at the first char in the buffer.

                        // If there's a preceding line, merge with it
                        if (iCurLine > iFirstBufferLine)
                        {
                            int iLen;

                            // If the current line has changed,
                            //   save it into the history buffer
                            if (bCurLineChanged)
                                ReplaceLine (iCurLine);

                            // Copy the preceding line from the
                            //   history buffer into lpwszCurLine;
                            CopyLine (iCurLine - 1);

                            // Get the global handle to the history buffer
                            lpGlbHist = (LPGLBHIST) MyGlobalLock (hGlbHist);

                            // Get a ptr to the contents of the current line
                            hGlb = lpGlbHist[iCurLine].hGlb;
                            if (hGlb)
                            {
                                wszLine = (LPWCHAR) MyGlobalLock (hGlb);
                            } else
                                wszLine = wszEmpty;

                            // Ensure the resulting line isn't too long
                            iLen = lstrlenW (wszLine);
                            if ((iCurLineLen + iLen) > iLastBufferChar)
                            {
                                // ***FIXME*** -- LINE TOO LONG
                                DbgMsg ("VK_BACK:  LINE TOO LONG");

                                // Copy the current line from the
                                //   history buffer into lpwszCurLine;
                                CopyLine (iCurLine);
                            } else
                            {
                                // Set new current line
                                iCurLine--;

                                // Set new caret position
                                iCurChar = iCurLineLen;

                                // Set new current line length
                                iCurLineLen += iLen;

                                // As we changed iCurLine & iCurChar, move the caret
                                MoveCaret ();

                                // Scroll the window as necessary
                                if (iCurLine < iFirstWindowLine)
                                    ScrollVert (SCROLL_FIRST, iCurLine - iFirstWindowLine);

                                // Merge iCurLine and iCurLine+1
                                lstrcatW (lpwszCurLine, wszLine);
                                bCurLineChanged = TRUE;

                                // Unlock the memory before deleting it
                                if (hGlb)
                                {
                                    MyGlobalUnlock (hGlb); wszLine = NULL;
                                } // End IF

                                // Delete iCurLine + 1
                                DeleteLine (iCurLine + 1); hGlb = NULL;

                                // Redraw the this line and the ones below it
                                InvalidateRange (iCurLine, iLastValidLine + 1);
                            } // End IF/ELSE

                            // Unlock the handles
                            if (hGlb)
                            {
                                MyGlobalUnlock (hGlb); wszLine = NULL;
                            } // End IF
                            MyGlobalUnlock (hGlbHist); lpGlbHist = NULL;
                        } // End IF
                    } // End IF/ELSE

                    return FALSE;

                case VK_TAB:            // Tab
                {
                    int iNewCurChar, i;

                    iNewCurChar = DEF_TABS * (1 + iCurChar / DEF_TABS);

                    // Ensure line not too long
                    if ((iCurLineLen + iNewCurChar - iCurChar) > iLastBufferChar)
                    {
                        // ***FIXME*** -- LINE TOO LONG
                        DbgMsg ("DisplayChar:  LINE TOO LONG");




                    } // End IF

                    // If we're within the current line,
                    //   insert some spaces
                    if (iCurChar < iCurLineLen)
                    {
                        memmove (&lpwszCurLine[iNewCurChar],
                                 &lpwszCurLine[iCurChar],
                                 (iCurLineLen - iCurChar) * sizeof (WCHAR));
                        // Calculate the new line length
                        iCurLineLen += (iNewCurChar - iCurChar);

                        // Fill with blanks
                        for (i = iCurChar; i < iNewCurChar; i++)
                            lpwszCurLine[i] = L' ';
                    } // End IF

                    // Position the caret
                    iCurChar = iNewCurChar;

                    // As we changed iCurChar, move the caret
                    MoveCaret ();

                    // Ensure properly terminated
                    lpwszCurLine[iCurLineLen] = L'\0';

                    // Invalidate the line so it gets repainted
                    InvalidateLine (iCurLine);

                    return FALSE;
                } // End VK_TAB

                case '\n':              // Line continuation char
                    DisplayChar ('\n');

                    // Restore the original value of lpGlbHist[iCurLine]
                    InvalidateLine (iCurLine);

                    // Append lpwszCurLine to the end of the history
                    AppendLine (lpwszCurLine, TRUE);  // iLastValidLine changes

                    // Get the global handle to the history buffer
                    lpGlbHist = (LPGLBHIST) MyGlobalLock (hGlbHist);

                    // Mark the lines as continuations of each other
                    lpGlbHist[iLastValidLine    ].ContNext =
                    lpGlbHist[iLastValidLine + 1].ContPrev = TRUE;

                    // Unlock the handle
                    MyGlobalUnlock (hGlbHist); lpGlbHist = NULL;

                    return FALSE;

                case VK_RETURN:         // Enter
                {
                    EXECSTATE esState;
                    int       iExecLine;

                    // Restore the original value of lpGlbHist[iCurLine]
                    InvalidateLine (iCurLine);

                    // Append lpwszCurLine to the end of the history
                    AppendLine (lpwszCurLine, FALSE);  // iLastValidLine changes

                    // Get the global handle to the history buffer
                    lpGlbHist = (LPGLBHIST) MyGlobalLock (hGlbHist);

                    // Find the first line in the continuation sequence
                    for (iExecLine = iLastValidLine;
                         lpGlbHist[iExecLine].ContPrev;
                         iExecLine--)
                    {};

                    // Unlock the handle
                    MyGlobalUnlock (hGlbHist); lpGlbHist = NULL;

                    // Mark as immediate execution
                    esState.exType = EX_IMMEX;
                    ExecuteLine (iExecLine, &esState);

                    return FALSE;
                } // End VK_RETURN

                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F':
                case 'G':
                case 'H':
                case 'I':
                case 'J':
                case 'K':
                case 'L':
                case 'M':
                case 'N':
                case 'O':
                case 'P':
                case 'Q':
                case 'R':
                case 'S':
                case 'T':
                case 'U':
                case 'V':
                case 'W':
                case 'X':
                case 'Y':
                case 'Z':

                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                case 'f':
                case 'g':
                case 'h':
                case 'i':
                case 'j':
                case 'k':
                case 'l':
                case 'm':
                case 'n':
                case 'o':
                case 'p':
                case 'q':
                case 'r':
                case 's':
                case 't':
                case 'u':
                case 'v':
                case 'w':
                case 'x':
                case 'y':
                case 'z':

                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':

                case '`':
                case '~':
                case '!':
                case '@':
                case '#':
                case '$':
                case '%':
                case '^':
                case '&':
                case '*':
                case '(':
                case ')':
                case '{':
                case '}':
                case '[':
                case ']':
                case '-':
                case '_':
                case '=':
                case '+':
                case '|':
                case '\\':
                case ':':
                case ';':
                case '"':
                case '\'':
                case ',':
                case '<':
                case '.':
                case '>':
                case '/':
                case '?':
                case VK_SPACE:          // Space
                    DisplayChar (chCharCode);

                    return FALSE;
                default:
#ifdef DEBUG
                    wsprintf (lpszTemp,
                              "CHAR:  chCharCode = %d, %c",
                              chCharCode,
                              chCharCode);
                    DbgMsg (lpszTemp);
#endif
                    return FALSE;
            } // End SWITCH

            break;
        } // End WM_CHAR
#undef  chCharCode

#define chCharCode ((char) wParam)
        case WM_SYSCHAR:            // chCharCode = (TCHAR) wParam; // character code
                                    // lKeyData = lParam;           // Key data
        {
            int iChar;
            WCHAR wch;
            // Handle Shifted & unshifted Alt chars
            //  e.g., 'a' = 97, 'z' = 122

            iChar = chCharCode - ' ';
            if (iChar < (sizeof (aCharCode) / sizeof (CHARCODE)))
            {
                // Get the Alt- char code
                wch = aCharCode[iChar].alt;

                // If it's valid, display it
                if (wch)
                    DisplayChar (wch);
                else
                // Otherwise, DbgMsg it
                {
#ifdef DEBUG
                    wsprintf (lpszTemp,
                              "SYSCHAR:  chCharCode = %d, %c",
                              chCharCode,
                              chCharCode);
                    DbgMsg (lpszTemp);
#endif
                } // End IF/ELSE
            } else
            {
#ifdef DEBUG
                wsprintf (lpszTemp,
                          "SYSCHAR:  chCharCode = %d, %c",
                          chCharCode,
                          chCharCode);
                DbgMsg (lpszTemp);
#endif
            } // End IF/ELSE

            return FALSE;
        } // End WM_SYSCHAR
#undef  chCharCode

#ifdef DEBUG
#define chCharCode ((char) wParam)
        case WM_DEADCHAR:
            wsprintf (lpszTemp,
                      "DEADCHAR:  chCharCode = %d, %c",
                      chCharCode,
                      chCharCode);
            DbgMsg (lpszTemp);

            return FALSE;
#undef  chCharCode
#endif
#ifdef DEBUG
#define chCharCode ((char) wParam)
        case WM_SYSDEADCHAR:
            wsprintf (lpszTemp,
                      "SYSDEADCHAR:  chCharCode = %d, %c",
                      chCharCode,
                      chCharCode);
            DbgMsg (lpszTemp);

            return FALSE;
#undef  chCharCode
#endif

#define nVirtKey ((int) wParam)
        case WM_KEYDOWN:            // nVirtKey = (int) wParam;     // virtual-key code
                                    // lKeyData = lParam;           // Key data
        {
            // Process the virtual key
            switch (nVirtKey)
            {
                case VK_HOME:           // Home
                    // Set new caret position
                    iCurChar = 0;

                    // As we changed iCurChar, move the caret
                    MoveCaret ();       // Move the caret

                    // Scroll the window as necessary
                    if (iCurChar < iFirstWindowChar)
                        ScrollHorz (SCROLL_FIRST, iCurChar - iLastWindowChar);

                    return FALSE;

                case VK_END:            // End
                    // Set new caret position
                    iCurChar = iCurLineLen;

                    // As we changed iCurChar, move the caret
                    MoveCaret ();       // Move the caret

                    // Scroll the window as necessary
                    if (iCurChar > iLastWindowChar)
                        ScrollHorz (SCROLL_LAST, iCurChar - iLastWindowChar);
                    else
                    if (iCurChar < iFirstWindowChar)
                        ScrollHorz (SCROLL_FIRST, iCurChar - iFirstWindowChar);

                    return FALSE;

                case VK_LEFT:           // Left arrow
                    // If the caret is not at the start of the buffer, ...
                    if (iCurChar > iFirstBufferChar)
                    {
                        // Set new caret position
                        iCurChar--;

                        // As we changed iCurChar, move the caret
                        MoveCaret ();   // Move the caret
                    } // End IF

                    // Scroll the window as necessary
                    if (iCurChar < iFirstWindowChar)
                        ScrollHorz (SCROLL_FIRST, iCurChar - iFirstWindowChar);

                    return FALSE;

                case VK_RIGHT:          // Right arrow
                    // If the caret is not at the end of the buffer, ...
                    if (iCurChar < iLastBufferChar)
                    {
                        // Set new caret position
                        iCurChar++;

                        // As we changed iCurChar, move the caret
                        MoveCaret ();       // Move the caret
                    } // End IF

                    // Scroll the window as necessary
                    if (iCurChar > iLastWindowChar)
                        ScrollHorz (SCROLL_LAST, iCurChar - iLastWindowChar);

                    return FALSE;

                case VK_UP:             // Up arrow
                    // Move the cursor up one line
                    //   unless we're at the start
                    if (iCurLine > iFirstBufferLine)
                    {
                        // If the current line has changed,
                        //   save it into the history buffer
                        if (bCurLineChanged)
                            ReplaceLine (iCurLine);

                        // Set new current line
                        iCurLine--;

                        // As we changed iCurLine, move the caret
                        MoveCaret ();

                        // Copy the current line from the
                        //   history buffer into lpwszCurLine;
                        CopyLine (iCurLine);

                        // Scroll the window as necessary
                        if (iCurLine < iFirstWindowLine)
                            ScrollVert (SCROLL_FIRST, iCurLine - iFirstWindowLine);
                    } // End IF

                    return FALSE;

                case VK_DOWN:           // Down arrow
                    // Move the cursor down one line
                    //   unless we're at the last valid line
                    if (iCurLine < (iLastValidLine + 1))
                    {
                        // If the current line has changed,
                        //   save it into the history buffer
                        if (bCurLineChanged)
                            ReplaceLine (iCurLine);

                        // Set new current line
                        iCurLine++;

                        // As we changed iCurLine, move the caret
                        MoveCaret ();

                        // Copy the current line from the
                        //   history buffer into lpwszCurLine;
                        CopyLine (iCurLine);

                        // Scroll the window as necessary
                        if (iCurLine > iLastWindowLine)
                            ScrollVert (SCROLL_LAST, iCurLine - iLastWindowLine);
                    } // End IF

                    return FALSE;

                case VK_INSERT:         // Insert
                    vkState.Ins = !vkState.Ins;

                    DestroyCaret ();        // 'cause we're changing the cursor width

                    // Create a default sized system caret for display
                    MyCreateCaret ();

                    return FALSE;

                case VK_DELETE:         // Delete
                    // Delete the char at iCurChar
                    DeleteChar ();

                    return FALSE;

                case VK_PRIOR:          // Page Up
                    // View the previous page
                    ScrollVert (SCROLL_FIRST, -nWindowLines);

                    return FALSE;

                case VK_NEXT:           // Page Down
                    // View the next page
                    ScrollVert (SCROLL_FIRST,  nWindowLines);

                    return FALSE;
#ifdef DEBUG
                case VK_F1:             // Display session manager history
                    DisplayHistory ();

                    return FALSE;
#endif
#ifdef DEBUG
                case VK_F2:             // Display hash table entries
                    DisplayHshTab ();

                    return FALSE;
#endif
#ifdef DEBUG
                case VK_F3:             // Display current token entries
                    DisplayTokens (ghGlbToken);

                    return FALSE;
#endif
#ifdef DEBUG
                case VK_F4:             // Display symbol table entries
                                        //   with non-zero reference counts
                    // If it's Shift-, then display all
                    if (GetKeyState (VK_SHIFT) & 0x80000000)
                        DisplaySymTab (TRUE);
                    else
                        DisplaySymTab (FALSE);

                    return FALSE;
#endif
#ifdef DEBUG
                case VK_F5:             // Display outstanding global memory objects
                    // If it's Shift-, then display all
                    if (GetKeyState (VK_SHIFT) & 0x80000000)
                        DisplayGlobals (TRUE);
                    else
                        DisplayGlobals (FALSE);

                    return FALSE;
#endif
#ifdef DEBUG
                case VK_F9:             // Resize Debugger and Session Manager windows
                {
                    RECT rc;
                    int  nWidthMF,  nHeightMF,
                         nHeightDB, nHeightSM;

                    GetClientRect (hWndMF, &rc);
                    nWidthMF  = rc.right  - rc.left;
                    nHeightMF = rc.bottom - rc.top;

                    nHeightSM = 350;
                    nHeightDB = nHeightMF - nHeightSM;

                    // Resize the Debugger window
                    //   to the top of the client area
                    SetWindowPos (hWndDB,           // Window handle to position
                                  0,                // SWP_NOZORDER
                                  0,                // X-position
                                  0,                // Y-...
                                  nWidthMF,         // Width
                                  nHeightDB,        // Height
                                  SWP_NOZORDER      // Flags
                                | SWP_SHOWWINDOW
                                 );
                    // Resize the Session Manager window
                    //   to the bottom of the client area
                    SetWindowPos (hWndSM,           // Window handle to position
                                  0,                // SWP_NOZORDER
                                  0,                // X-position
                                  nHeightDB,        // Y-...
                                  nWidthMF,         // Width
                                  nHeightSM,        // Height
                                  SWP_NOZORDER      // Flags
                                | SWP_SHOWWINDOW
                                 );
                    return FALSE;
                } // End VK_F9
#endif
#ifdef DEBUG
////////////////case VK_F10:            // Not generated
////////////////    return FALSE;
#endif
#ifdef DEBUG
                case VK_F11:            // DbgBrk ()
                    DbgBrk ();

                    return FALSE;
#endif
#ifdef DEBUG
                case VK_F12:            // Clear the debugging display
                    // Clear the debugger listbox
                    DbgClr ();

                    return FALSE;
#endif
#ifdef DEBUG
////////////////default:
////////////////    wsprintf (lpszTemp,
////////////////              "WM_KEYDOWN:  nVirtKey = %d",
////////////////              nVirtKey);
////////////////    DbgMsg (lpszTemp);
#endif
            } // End SWITCH

            // We need to pass this message on to the next handler
            //   so WM_CHAR & WM_SYSCHAR can process it.
            break;
        } // End WM_KEYDOWN
#undef  nVirtKey

#define nPos ((short int) HIWORD(wParam))
        case WM_HSCROLL:        // nScrollCode = (int) LOWORD(wParam);  // scroll bar value
                                // nPos = (short int) HIWORD(wParam);   // scroll box position
                                // hwndScrollBar = (HWND) lParam;       // handle of scroll bar
            // Split cases
            switch (LOWORD (wParam))
            {
////////////////case SB_BOTTOM:
////////////////    break;

////////////////case SB_ENDSCROLL:
////////////////    break;

////////////////case SB_TOP:
////////////////    break;

////////////////case SB_THUMBPOSITION:      // Look in nPos
////////////////    break;

                case SB_LINELEFT:
                    ScrollHorz (SCROLL_FIRST, -1);
                    break;

                case SB_LINERIGHT:
                    ScrollHorz (SCROLL_FIRST,  1);
                    break;

                case SB_PAGELEFT:
                    ScrollHorz (SCROLL_FIRST, -nWindowChars);
                    break;

                case SB_PAGERIGHT:
                    ScrollHorz (SCROLL_FIRST,  nWindowChars);
                    break;

                case SB_THUMBTRACK:         // Look in nPos
                    // nPos contains the new iFirstWindowChar
                    ScrollHorz (SCROLL_FIRST,  nPos - iFirstWindowChar);

                    break;
            } // End SWITCH

            return FALSE;           // We handled the msg
#undef  nPos

#define nPos ((short int) HIWORD(wParam))
        case WM_VSCROLL:        // nScrollCode = (int) LOWORD(wParam);  // scroll bar value
                                // nPos = (short int) HIWORD(wParam);   // scroll box position
                                // hwndScrollBar = (HWND) lParam;       // handle of scroll bar
            // Split cases
            switch (LOWORD (wParam))
            {
////////////////case SB_BOTTOM:
////////////////    break;

////////////////case SB_ENDSCROLL:
////////////////    break;

////////////////case SB_TOP:
////////////////    break;

////////////////case SB_THUMBPOSITION:      // Look in nPos
////////////////    break;

                case SB_LINEUP:
                    ScrollVert (SCROLL_FIRST, -1);
                    break;

                case SB_LINEDOWN:
                    ScrollVert (SCROLL_FIRST,  1);
                    break;

                case SB_PAGEDOWN:
                    ScrollVert (SCROLL_FIRST,  nWindowLines);
                    break;

                case SB_PAGEUP:
                    ScrollVert (SCROLL_FIRST, -nWindowLines);
                    break;

                case SB_THUMBTRACK:         // Look in nPos
                    // nPos contains the new iFirstWindowLine
                    ScrollVert (SCROLL_FIRST,  nPos - iFirstWindowLine);

                    break;
            } // End SWITCH

            return FALSE;           // We handled the msg
#undef  nPos

#define fwKeys  (LOWORD(wParam))
#define xPos    (LOWORD(lParam))
#define yPos    (HIWORD(lParam))
        case WM_MOUSEWHEEL:         // fwKeys = LOWORD(wParam); // key flags
                                    // iDist  = (int) HIWORD(wParam); // Scroll distance in units od WHEEL_DELTA
                                    // xPos = LOWORD(lParam);  // horizontal position of cursor
                                    // yPos = HIWORD(lParam);  // vertical position of cursor
            if (GET_WHEEL_DELTA_WPARAM (wParam) > 0)
                PostMessage (hWnd, WM_VSCROLL, SB_LINEUP, 0);
            else
                PostMessage (hWnd, WM_VSCROLL, SB_LINEDOWN, 0);
            return FALSE;           // We handled the msg
#undef  yPos
#undef  xPos
#undef  fwKeys

        case WM_NCCREATE:       // lpcs = (LPCREATESTRUCT) lParam
            hWndSM = hWnd;

            break;                  // Continue with next handler

        case WM_CREATE:
        {
            HDC hDC;
            int i;

            // Initialize variables
            vkState.Ins = 1;        // Initially inserting ***FIXME*** Make it an option
            cfAPL.hwndOwner = hWnd;

            // Initialize window-specific resources
            SM_Create (hWnd);

            // *************** Bitmaps *********************************
            hBitMapLineCont = LoadBitmap (_hInstance, MAKEINTRESOURCE (IDB_LINECONT));
            if (hBitMapLineCont)
            {
                GetObject (hBitMapLineCont, sizeof (BITMAP), (LPSTR) &bmLineCont);

                iLCWidth = 2 + bmLineCont.bmWidth + 2;  // Width of line continuation column
            } // End IF

            // *************** lpwszCurLine ****************************

            // Allocate memory for the current line
            // Note that this memory is allocated as fixed
            //   because we use so often
            lpwszCurLine = MyGlobalAlloc (GPTR, (DEF_MAXLINELEN + 1) * sizeof (WCHAR));
            if (!lpwszCurLine)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsg ("WM_CREATE:  MyGlobalAlloc for <lpwszCurLine> failed");

                return -1;          // Mark as failed
            } // End IF

            // Initialize the current line
            InitCurLine (FALSE);

////////////// *************** lptkStackBase ***************************
////////////
////////////// Allocate virtual memory for the token stack used in parsing
////////////lptkStackBase = VirtualAlloc (NULL,      // Any address
////////////                              DEF_TOKENSTACK_MAXSIZE,
////////////                              MEM_RESERVE,
////////////                              PAGE_READWRITE);
////////////if (!lptkStackBase)
////////////{
////////////    // ***FIXME*** -- WS FULL before we got started???
////////////    DbgMsg ("WM_CREATE:  VirtualAlloc for <lptkStackBase> failed");
////////////
////////////    return -1;          // Mark as failed
////////////} // End IF
////////////
////////////// Commit the intial size
////////////VirtualAlloc (lptkStackBase,
////////////              DEF_TOKENSTACK_INITSIZE,
////////////              MEM_COMMIT,
////////////              PAGE_READWRITE);

            // *************** lpszNumAlp ******************************

            // Allocate virtual memory for the Name & Number accumulator
            lpszNumAlp = VirtualAlloc (NULL,      // Any address
                                       DEF_NUMALP_MAXSIZE,
                                       MEM_RESERVE,
                                       PAGE_READWRITE);
            if (!lpszNumAlp)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsg ("WM_CREATE:  VirtualAlloc for <lpszNumAlp> failed");

                return -1;          // Mark as failed
            } // End IF

            // Commit the intial size
            VirtualAlloc (lpszNumAlp,
                          DEF_NUMALP_INITSIZE,
                          MEM_COMMIT,
                          PAGE_READWRITE);

            // *************** lpwszString *****************************

            // Allocate virtual memory for the wide string accumulator
            lpwszString = VirtualAlloc (NULL,       // Any address
                                        DEF_STRING_MAXSIZE,
                                        MEM_RESERVE,
                                        PAGE_READWRITE);
            if (!lpwszString)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsg ("WM_CREATE:  VirtualAlloc for <lpwszString> failed");

                return -1;          // Mark as failed
            } // End IF

            // Commit the intial size
            VirtualAlloc (lpwszString,
                          DEF_STRING_INITSIZE,
                          MEM_COMMIT,
                          PAGE_READWRITE);

            // *************** lpHshTab ********************************

            // Allocate virtual memory for the hash table
            lpHshTab = VirtualAlloc (NULL,      // Any address
                                     DEF_HSHTAB_MAXSIZE,
                                     MEM_RESERVE,
                                     PAGE_READWRITE);
            if (!lpHshTab)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsg ("WM_CREATE:  VirtualAlloc for <lpHshTab> failed");

                return -1;          // Mark as failed
            } // End IF

            // Commit the intial size
            VirtualAlloc (lpHshTab,
                          DEF_HSHTAB_INITSIZE,
                          MEM_COMMIT,
                          PAGE_READWRITE);

            // Initialize the principal hash entry (1st one in each block).
            // This entry is never overwritten with an entry with a
            //   different hash value.
            for (i = 0; i < DEF_HSHTAB_INITSIZE; i += DEF_HSHTAB_EPB)
                lpHshTab[i].htFlags.PrinHash = 1;

            // Initialize the next & prev same HTE values
            for (i = 0; i < DEF_HSHTAB_INITSIZE; i++)
            {
                lpHshTab[i].NextSameHash =
                lpHshTab[i].PrevSameHash = LPHSHENTRY_NONE;
            } // End FOR

            // Initialize next split entry
            lpHshTabSplitNext = lpHshTab;

            // *************** lpSymTab ********************************

            // Allocate virtual memory for the symbol table
            lpSymTab = VirtualAlloc (NULL,      // Any address
                                     DEF_SYMTAB_MAXSIZE,
                                     MEM_RESERVE,
                                     PAGE_READWRITE);
            if (!lpSymTab)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsg ("WM_CREATE:  VirtualAlloc for <lpSymTab> failed");

                return -1;          // Mark as failed
            } // End IF

            // Commit the intial size
            VirtualAlloc (lpSymTab,
                          DEF_SYMTAB_INITSIZE,
                          MEM_COMMIT,
                          PAGE_READWRITE);

            // Initialize next available entry
            lpSymTabNext = lpSymTab;

            // *************** History *********************************

            // Allocate memory for the array of ptrs to the
            //   Session Manager window lines
            // The "+1" is for converting limit to length.
            hGlbHist = MyGlobalAlloc (GHND, (iLastBufferLine + 1) * sizeof (GLBHIST));
            if (!hGlbHist)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsg ("WM_CREATE:  MyGlobalAlloc for <hGlbHist> failed");

                return -1;          // Mark as failed
            } // End IF

            // Mark the first entry as such
            lpGlbHist = (LPGLBHIST) MyGlobalLock (hGlbHist);
            lpGlbHist[0].First = 1;
            MyGlobalUnlock (hGlbHist); lpGlbHist = NULL;

            // *************** Fonts ***********************************

            // Find the Unicode font
            hDC = MyGetDC (hWnd);
            SetAttrs (hDC);
            EnumFontFamilies (hDC,
                              DEF_FONTNAME,
                              (FONTENUMPROC) FindAplFontEnumProc,
                              0);
            if (!bAPLfont)
            {
                // No Unicode font found -- we'll add it as a resource
                MB ("No APL Unicode font found:  " DEF_FONTNAME);

                // ***FIXME*** -- add the font as a resource and retry
                DbgMsg ("WM_CREATE:  No APL font");






            } // End IF

            iLogPixelsX = GetDeviceCaps (hDC, LOGPIXELSX);  // # horz pixels/inch
            iLogPixelsY = GetDeviceCaps (hDC, LOGPIXELSY);  // # vert ...

            // Release the DC
            MyReleaseDC (hWnd, hDC); hDC = NULL;

            // Initialize for a new font
            NewFont ();

            // *************** ScrollBars ******************************

            // Set scrollbar ranges
            SetVScrollRange ();
            SetHScrollRange ();

            PostMessage (hWnd, WM_USER, 0, 0);

            return FALSE;           // We handled the msg
        } // End WM_CREATE

        case WM_USER:
            // *************** System Names ****************************

            // Append all system names (functions and variables) as reserved
            if (!AppendSystemNames_EM ())
            {
                DbgMsg ("WM_CREATE:  AppendSystemNames_EM failed");

                return -1;          // Mark as failed
            } // End IF

            // *************** System Vars *****************************

            // Initialize all system vars
            if (!InitSystemVars ())
            {
                DbgMsg ("WM_CREATE:  InitSystemVars failed");

                return -1;          // Mark as failed
            } // End IF

            // *************** Primitive Fns ***************************

            InitPrimFns ();

            return FALSE;           // We handled the msg

#define fwSizeType wParam
        case WM_SIZE:               // fwSizeType = wParam;      // resizing flag
                                    // nWidth = LOWORD(lParam);  // width of client area
                                    // nHeight = HIWORD(lParam); // height of client area
            if (fwSizeType NE SIZE_MINIMIZED)
            {
                cxWindowPixels = LOWORD (lParam);   // Save for later use
                cyWindowPixels = HIWORD (lParam);   // ...

                // Recalculate the # characters up/down and across
                // The test is in case we get called before cxAveChar is calculated
                if (cxAveChar)
                {
                    nWindowChars  = cxWindowPixels / cxAveChar;
                    nWindowLines  = cyWindowPixels / cyAveChar;

                    // Set last window chars & lines
                    iLastWindowChar = iFirstWindowChar + nWindowChars;
                    iLastWindowLine = iFirstWindowLine + nWindowLines;
                } // End IF
            } // End IF

            break;                  // Continue with next handler ***MUST***
#undef  fwSizeType

        case WM_SETFOCUS:           // hwndLoseFocus = (HWND) wParam; // handle of window losing focus
            // Create a default sized system caret for display
            MyCreateCaret ();

            break;                  // Continue with next handler ***MUST***

        case WM_KILLFOCUS:          // hwndGetFocus = (HWND) wParam; // handle of window receiving focus
            DestroyCaret ();        // 'cause we just lost the focus

            break;                  // Continue with next handler

        case WM_SYSCOLORCHANGE:
        case WM_SETTINGCHANGE:
            // Uninitialize window-specific resources
            SM_Delete (hWnd);

            // Initialize window-specific resources
            SM_Create (hWnd);

            return FALSE;           // We handled the msg

#define fwKeys  wParam
#define xPos    (LOWORD(lParam))
#define yPos    (HIWORD(lParam))
        case WM_LBUTTONDOWN:        // fwKeys = wParam;        // key flags
                                    // xPos = LOWORD(lParam);  // horizontal position of cursor
                                    // yPos = HIWORD(lParam);  // vertical position of cursor
        {
            int xPosRel;

            // If the current line has changed,
            //   save it into the history buffer
            if (bCurLineChanged)
                ReplaceLine (iCurLine);

            // Ensure xPos is relative to the non-iLCWidth client area
            xPosRel = max (0, xPos - iLCWidth);

            // Identify the selected char
            iCurChar = iFirstWindowChar + (xPosRel / cxAveChar);
            iCurLine = iFirstWindowLine + (yPos    / cyAveChar);

            // Move the cursor to this line
            //   unless it's below the last valid line
            iCurLine = min (iCurLine, iLastValidLine + 1);

            // ***FIXME** -- Should we squeak if the cursor is too low??

            // Move the caret there
            MoveCaret ();

            // Copy the current line from the
            //   history buffer into lpwszCurLine;
            CopyLine (iCurLine);

            return FALSE;           // We handled the msg
        } // End WM_LBUTTONDOWN
#undef  yPos
#undef  xPos
#undef  fwKeys

//// #define fwKeys wParam
//// #define xPos (LOWORD(lParam))
//// #define yPos (HIWORD(lParam))
////         case WM_LBUTTONDOWN:        // fwKeys = wParam;        // key flags
////                                     // xPos = LOWORD(lParam);  // horizontal position of cursor
////                                     // yPos = HIWORD(lParam);  // vertical position of cursor
////
////             // ***FIXME***
////
////             // If we've selected some text, cancel that action
////             if (bSelText)
////             {
////                 // Cancel selected text
////                 bSelText = FALSE;
////                 InvalidateRect (hWnd, NULL, TRUE);
////             } // End IF
////
////             bSelText = TRUE;        // Start selection
////             xSelText = xPos;        // Save starting position
////             ySelText = yPos;        // ...
////             xSelChar = iFirstWindowChar + (xPos / cxAveChar);
////             ySelChar = iFirstWindowLine + (yPos / cyAveChar);
////
////             // Clear the rectangle
////             SetRectEmpty (&rcSelInit);
////             SetRectEmpty (&rcSelText);
////
////             // If the Ctrl key is down, select whole lines
////             if (fwKeys & MK_CONTROL)
////             {
////                 eSelType  = SEL_LINES;
////                 rcSelInit.left   = 0;
////                 rcSelInit.top    = yPos;
////                 rcSelInit.right  = 65535;
////                 rcSelInit.bottom = yPos + cyAveChar;
////             } else
////             // If the Shift key is down, select a rectangular block
////             if (fwKeys & MK_SHIFT)
////             {
////                 eSelType  = SEL_BLOCKS;
////                 rcSelInit.left   = xPos;
////                 rcSelInit.top    = yPos;
////                 rcSelInit.right  = xPos + cxAveChar;
////                 rcSelInit.bottom = yPos + cyAveChar;
////             } else
////                 // Otherwise, select chars
////                 eSelType = SEL_CHARS;
////
////             return FALSE;           // We handled the msg
//// #undef  yPos
//// #undef  xPos
//// #undef  fwKeys
////
//// #define fwKeys wParam
//// #define xPos (LOWORD(lParam))
//// #define yPos (HIWORD(lParam))
////         case WM_MOUSEMOVE:          // fwKeys = wParam;        // key flags
////                                     // xPos = LOWORD(lParam);  // horizontal position of cursor
////                                     // yPos = HIWORD(lParam);  // vertical position of cursor
////
////             // If we're selecting text
////             if (bSelText)
////             {
////                 RECT rc;
////
////                 // Expand or contract selection
////                 switch (eSelType)
////                 {
////                     case SEL_CHARS:
////
////
////                         break;
////
////                     case SEL_LINES:
////                         // Clear previous inversion
////
////
////                         // Invert the lines between
////                         //   (0, ySelText) and
////                         //   (0, yPos)
////                         rc.left   = 0;
////                         rc.top    = yPos;
////                         rc.right  = 65535;
////                         rc.bottom = yPos + cyAveChar;
////
////                         UnionRect (&rcSelText, &rcSelInit, &rc);
////                         InvalidateRect (hWnd, &rcSelText, TRUE);
////
////                         break;
////
////                     case SEL_BLOCKS:
////                         // Clear previous inversion
////
////
////                         // Invert the chars between
////                         //   (xSelText, ySelText) and
////                         //   (xPos, yPos)
////
////                         rc.left   = xPos;
////                         rc.top    = yPos;
////                         rc.right  = xPos + cxAveChar;
////                         rc.bottom = yPos + cyAveChar;
////
////                         UnionRect (&rcSelText, &rcSelInit, &rc);
////                         InvalidateRect (hWnd, &rcSelText, TRUE);
////
////                         break;
////                 } // End SWITCH
////             } // End IF
////
////             return FALSE;           // We handled the msg
//// #undef  yPos
//// #undef  xPos
//// #undef  fwKeys
////
//// #define fwKeys wParam
//// #define xPos (LOWORD(lParam))
//// #define yPos (HIWORD(lParam))
////         case WM_LBUTTONUP:          // fwKeys = wParam;        // key flags
////                                     // xPos = LOWORD(lParam);  // horizontal position of cursor
////                                     // yPos = HIWORD(lParam);  // vertical position of cursor
////
////             // ***FIXME***
////
////             // Stop or cancel selection
////
////
////
////
////
////
////             bSelText = TRUE;
////
////             return FALSE;           // We handled the msg
//// #undef  yPos
//// #undef  xPos
//// #undef  fwKeys
////
////         case WM_RBUTTONDOWN:        // Popup menu
////             // ***FIXME***
////
////             return FALSE;           // We handled the msg

        case WM_ERASEBKGND:
            // In order to reduce screen flicker, we handle erase background
            // in the WM_PAINT message.
            return TRUE;            // We erased the background

        case WM_PAINT:
            // Validate the update region
            if (GetUpdateRect (hWnd, &rc, FALSE))
            {
                PAINTSTRUCT ps;
                int         iTop, iBot;
                LPWCHAR     lpwszLine;
                HDC         hDC, hDCMem;
                HBITMAP     hBitmap, hBitmapOld;
                RECT        rcLineCont;

                // Tell W to lay out the dropcloths
                BeginPaint (hWnd, &ps);

                // Get the size of the client area
                // so we know how big a bitmap to define
                GetClientRect (hWnd, &rc);

                rcLineCont.left   = 0;
                rcLineCont.top    = 0;
                rcLineCont.right  = iLCWidth;
                rcLineCont.bottom = rc.bottom;

                // To avoid flicker, we draw in a memory DC
                hDCMem = MyCreateCompatibleDC (ps.hdc);
                hBitmap = MyCreateCompatibleBitmap (ps.hdc,
                                                    rc.right,
                                                    rc.bottom);
                hBitmapOld = SelectObject (hDCMem, hBitmap);

                // Handle WM_ERASEBKGND here by filling in the client area
                FillRect (hDCMem, &rc, (HBRUSH) GetClassLong (hWnd, GCL_HBRBACKGROUND));

                // Fill in the background for the line continuation column
                FillRect (hDCMem, &rcLineCont, GetStockObject (LTGRAY_BRUSH));

                // Set our DC attributes
                SetAttrs (hDCMem);

                // Get the global handle to the history buffer
                lpGlbHist = (LPGLBHIST) MyGlobalLock (hGlbHist);

                // Calculate the range of changed line(s)
////////////////iTop = iFirstWindowLine + (ps.rcPaint.top    / cyAveChar);
////////////////iBot = iFirstWindowLine + (ps.rcPaint.bottom / cyAveChar);
                iTop = iFirstWindowLine + (rc.top    / cyAveChar);
                iBot = iFirstWindowLine + (rc.bottom / cyAveChar);
#if (defined (DEBUG)) && 0
                { // ***DEBUG***
                    wsprintf (lpszTemp,
                              "WM_PAINT:  iTop = %d, iBot = %d",
                              iTop,
                              iBot);
                    DbgMsg (lpszTemp);
                } // ***DEBUG*** END
#endif
                // Redraw the changed line(s)
                for (; iTop < iBot; iTop++)
                if (iTop EQ iCurLine)
                {
                    // Ensure properly terminated
                    lpwszCurLine[iCurLineLen] = L'\0';
                    wszLine = lpwszCurLine;

                    // Ensure iFirstWindowChar not beyond string end
                    if (iFirstWindowChar < lstrlenW (wszLine))
                        lpwszLine = &wszLine[iFirstWindowChar];
                    else
                        lpwszLine = wszEmpty;
                    DrawLine (lpwszLine,
                              iTop - iFirstWindowLine,
                              hDCMem,
                              lpGlbHist[iTop].ContPrev);
                } else
                if (iTop <= iLastBufferLine)
                {
                    if (lpGlbHist[iTop].hGlb)
                    {
                        // Get ptr to the global memory
                        hGlb = lpGlbHist[iTop].hGlb;
                        wszLine = (LPWCHAR) MyGlobalLock (hGlb);

                        // Ensure iFirstWindowChar not beyond string end
                        if (iFirstWindowChar < lstrlenW (wszLine))
                            lpwszLine = &wszLine[iFirstWindowChar];
                        else
                            lpwszLine = wszEmpty;
                        DrawLine (lpwszLine,
                                  iTop - iFirstWindowLine,
                                  hDCMem,
                                  lpGlbHist[iTop].ContPrev);

                        // Unlock the handle
                        MyGlobalUnlock (hGlb); wszLine = NULL;
                    }
////////////////////else
////////////////////    DrawLine (wszEmpty,
////////////////////              iTop - iFirstWindowLine,
////////////////////              hDCMem,
////////////////////              lpGlbHist[iTop].ContPrev);
                } // End FOR/IF/ELSE/IF

                // Obtain a DC without a clipping region
                //   so we may draw anywhere we like
                //   (such as for line continuations)
                hDC = MyGetDC (hWnd);

                // Set our DC attributes
                SetAttrs (hDC);

                // Copy the memory DC to the screen DC
                BitBlt (hDC,
                        0,
                        0,
                        rc.right,
                        rc.bottom,
                        hDCMem,
                        0,
                        0,
                        SRCCOPY);
                // Free resources
                SelectObject (hDCMem, hBitmapOld);
                MyDeleteObject (hBitmap); hBitmap = NULL;
                MyDeleteDC (hDCMem); hDCMem = NULL;
                MyReleaseDC (hWnd, hDC); hDC = NULL;

                // Unlock the handle
                MyGlobalUnlock (hGlbHist); lpGlbHist = NULL;

                // Pass the turpentine
                EndPaint (hWnd, &ps);
            } // End IF

            return FALSE;           // We handled the msg

        case WM_CLOSE:
            // Because the SM window doesn't close unless the
            //   MF window is closing, we ignore this message.
            // We close when the MF window calls DestroyWindow.

            return FALSE;           // We handled the msg

        case WM_DESTROY:
        {
            // *************** ScrollBars ******************************
            // Nothing to undo

            // *************** Fonts ***********************************
            if (hFontAPL)
            {
                MyDeleteObject (hFontAPL); hFontAPL = NULL;
            } // End IF

            // *************** History *********************************
            if (hGlbHist)
            {
                int i;

                // Get the global handle to the history buffer
                lpGlbHist = (LPGLBHIST) MyGlobalLock (hGlbHist);

                // Free the lines
                for (i = 0; i <= iLastValidLine; i++)
                if (lpGlbHist[i].hGlb)
                {
                    DbgGlobalFree (lpGlbHist[i].hGlb); lpGlbHist[i].hGlb = NULL;
                } // End IF

                // Unlock and free the global handle
                MyGlobalUnlock (hGlbHist); lpGlbHist = NULL;
                DbgGlobalFree (hGlbHist); hGlbHist = NULL;
            } // End IF

            // *************** lpSymTab ********************************
            if (lpSymTab)
            {
                VirtualFree (lpSymTab, 0, MEM_RELEASE); lpSymTab = NULL;
            } // End IF

            // *************** lpwszString *****************************
            if (lpwszString)
            {
                VirtualFree (lpwszString, 0, MEM_RELEASE); lpwszString = NULL;
            } // End IF

            // *************** lpszNumAlp ******************************
            if (lpszNumAlp)
            {
                VirtualFree (lpszNumAlp, 0, MEM_RELEASE); lpszNumAlp = NULL;
            } // End IF

////////////// *************** lptkStackBase ***************************
////////////if (lptkStackBase)
////////////{
////////////    VirtualFree (lptkStackBase, 0, MEM_RELEASE); lptkStackBase = NULL;
////////////} // End IF

            // *************** lpwszCurLine ****************************
            if (lpwszCurLine)
            {
                DbgGlobalFree (lpwszCurLine); lpwszCurLine = NULL;
            } // End IF

            // *************** Bitmaps *********************************
            if (hBitMapLineCont)
            {
                DeleteObject (hBitMapLineCont); hBitMapLineCont = NULL;
            } // End IF

            // Uninitialize window-specific resources
            SM_Delete (hWnd);

            return FALSE;           // We handled the msg
        } // End WM_DESTROY
    } // End SWITCH

////ODSAPI ("SMZ:", hWnd, message, wParam, lParam);
    return DefMDIChildProc (hWnd, message, wParam, lParam);
} // End SMWndProc
#undef  APPEND_NAME


//***************************************************************************
//  W2A
//
//  Convert and copy a wide char string to a multibyte char string
//***************************************************************************

void W2A
    (LPSTR  lpDest,
     LPWSTR lpSrc,
     DWORD  dwSize)

{
    int iLen;

    iLen = 1 + lstrlenW (lpSrc);    // Get source length
    WideCharToMultiByte (CP_ACP,    // ANSI code page
                         0,         // No flags
                         lpSrc,     // Source
                         iLen,      // Source length
                         lpDest,    // Destin
                         dwSize,    // Size of buffer
                         NULL,      // Use system default char
                         NULL);     // Skip notification
} // End W2A


//***************************************************************************
//  A2W
//
//  Convert and copy a multibyte char string to a wide char string
//***************************************************************************

void A2W
    (LPWSTR lpDest,
     LPSTR  lpSrc)

{
    int iLen;

    iLen = 1 + lstrlenA (lpSrc);    // Get source length
    MultiByteToWideChar (CP_ACP,    // ANSI code page
                         0,         // No flags
                         lpSrc,     // Source
                         iLen,      // Source length
                         lpDest,    // Destin
                         iLen * sizeof (WCHAR)); // Size of buffer
} // End A2W


//***************************************************************************
//  End of File: sessman.c
//***************************************************************************
