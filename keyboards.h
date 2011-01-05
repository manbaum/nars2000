//***************************************************************************
//  NARS2000 -- Keyboard Variables
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2011 Sudley Place Software

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
//  Keyboards and Character Codes
//***************************************************************************

typedef struct tagKEYDATA
{
    UINT repeatCount:16,            // Repeat count
         scanCode:8,                // Scan code
         extendedKey:1,             // TRUE iff extended key (right-Alt, Right-Ctl)
         reserved:4,                // Reserved
         contextCode:1,             // TRUE iff Alt key is down while key is pressed
         previousState:1,           // TRUE iff key is down before msg sent
         transitionState:1;         // TRUE iff key is being released
} KEYDATA, *LPKEYDATA;


// Keyboard accelerators
//   Not used with Customize Dialog for keyboards but left in to
//     to see how it's done.
EXTERN
HANDLE hAccel
#ifdef DEFINE_VALUES
= NULL
#endif
;

typedef struct tagVKSTATE
{
    UINT  Shift:1,              // 00000001:  Left- or right-shift key up(0) or down(1)
          Alt:1,                // 00000002:  Left- or right-Alt key up(0) or down(1)
          Ctl:1,                // 00000004:  Left or -right Ctl key up(0) or down(1)
          Ins:1,                // 00000008:  Replace(0) or insert(1)
          :28;                  // FFFFFFF0:  Available bits
} VKSTATE, *LPVKSTATE;


// If you are looking for places on the keyboard to put a new symbol,
//   there are several free Alt-Shift- combinations:
//     Alt-'A'
//     Alt-'B'
//     Alt-'C'
//     Alt-'D'
//     Alt-'F'
//     Alt-'K'
//     Alt-'Q'
//     Alt-'R'
//     Alt-'S'
//     Alt-'U'
//     Alt-'V'
//     Alt-'W'
//     Alt-'X'
//     Alt-'Y'
//     Alt-'Z'
//     Alt-'<'
//     Alt-'>'
//     Alt-':'
//     Alt-'"'
//
//   as well as several duplicated symbols:
//
//     Symbol & Name              Keystroke
//     --------------------------------------------------
//     * Asterisk                 Alt-'p'
//     = Equal                    Alt-'5'
//     < Left Caret               Alt-'3'
//     ? Question Mark            Alt-'q'
//     > Right Caret              Alt-'7'
//     ! Shriek                   Alt-'_'
//     ' Single Quote             Alt-'k'
//     | Stile                    Alt-'m'
//     ~ Tilde                    Alt-'t'
//     ^ Up Caret (Circumflex)    Alt-'0'

typedef struct tagCHARCODE
{
    WCHAR wc[8];    // 000-111:  Shift- Ctrl- Alt-keys
////WCHAR u   ,     // 000:  No Shift- Ctrl- Alt-keys pressed
////      a   ,     // 001:  Alt-key pressed
////      c   ,     // 010:  Ctrl-key pressed
////      ac  ,     // 011:  Alt- Ctrl- keys pressed
////      s   ,     // 100:  Shift-key pressed
////      sa  ,     // 101:  Shift- Alt-keys pressed
////      sc  ,     // 110:  Shift- Ctrl-keys pressed
////      sca ;     // 111:  Shift- Ctrl- Alt-keys pressed
} CHARCODE, *LPCHARCODE;

EXTERN
CHARCODE aCharCodesNARS_US_EN_ALT[0x4E + 1]
#ifdef DEFINE_VALUES
=                                                                               // ScanCode
{{  0 },                                                                        // 00:  DO NOT USE
 {  0 },                                                                        // 01:  ESC
//  U    A                        C  AC   S   SA                     SC  SCA
 {L'1', UTF16_DIERESIS          , 0, 0, L'!', UTF16_EQUALUNDERBAR   , 0, 0},    // 02:  '1'
 {L'2', UTF16_OVERBAR           , 0, 0, L'@', UTF16_NOTEQUALUNDERBAR, 0, 0},    // 03:  '2'
 {L'3', L'<'                    , 0, 0, L'#', UTF16_DELSTILE        , 0, 0},    // 04:  '3'
 {L'4', UTF16_LEFTCARETUNDERBAR , 0, 0, L'$', UTF16_DELTASTILE      , 0, 0},    // 05:  '4'
 {L'5', L'='                    , 0, 0, L'%', UTF16_CIRCLESTILE     , 0, 0},    // 06:  '5'
 {L'6', UTF16_RIGHTCARETUNDERBAR, 0, 0, L'^', UTF16_CIRCLESLOPE     , 0, 0},    // 07:  '6'
 {L'7', L'>'                    , 0, 0, L'&', UTF16_CIRCLEBAR       , 0, 0},    // 08:  '7'
 {L'8', UTF16_NOTEQUAL          , 0, 0, L'*', UTF16_CIRCLESTAR      , 0, 0},    // 09:  '8'
 {L'9', UTF16_DOWNCARET         , 0, 0, L'(', UTF16_DOWNCARETTILDE  , 0, 0},    // 0A:  '9'
 {L'0', UTF16_UPCARET           , 0, 0, L')', UTF16_UPCARETTILDE    , 0, 0},    // 0B:  '0'
 {L'-', UTF16_TIMES             , 0, 0, L'_', UTF16_QUOTEDOT        , 0, 0},    // 0C:  '-'
 {L'=', UTF16_COLONBAR          , 0, 0, L'+', UTF16_DOMINO          , 0, 0},    // 0D:  '='
 {  0 },                                                                        // 0E:  BS
 {  0 },                                                                        // 0F:  HT
//  U    A                        C  AC   S   SA                     SC  SCA
 {L'q', UTF16_QUERY             , 0, 0, L'Q', 0                     , 0, 0},    // 10:  'q'
 {L'w', UTF16_OMEGA             , 0, 0, L'W', 0                     , 0, 0},    // 11:  'w'
 {L'e', UTF16_EPSILON           , 0, 0, L'E', UTF16_EPSILONUNDERBAR , 0, 0},    // 12:  'e'
 {L'r', UTF16_RHO               , 0, 0, L'R', 0                     , 0, 0},    // 13:  'r'
 {L't', UTF16_TILDE             , 0, 0, L'T', UTF16_DIERESISTILDE   , 0, 0},    // 14:  't'
 {L'y', UTF16_UPARROW           , 0, 0, L'Y', 0                     , 0, 0},    // 15:  'y'
 {L'u', UTF16_DOWNARROW         , 0, 0, L'U', 0                     , 0, 0},    // 16:  'u'
 {L'i', UTF16_IOTA              , 0, 0, L'I', UTF16_IOTAUNDERBAR    , 0, 0},    // 17:  'i'
 {L'o', UTF16_CIRCLE            , 0, 0, L'O', UTF16_DIERESISCIRCLE  , 0, 0},    // 18:  'o'
 {L'p', UTF16_STAR              , 0, 0, L'P', UTF16_DIERESISSTAR    , 0, 0},    // 19:  'p'
 {L'[', UTF16_LEFTARROW         , 0, 0, L'{', UTF16_QUOTEQUAD       , 0, 0},    // 1A:  '['
 {L']', UTF16_RIGHTARROW        , 0, 0, L'}', UTF16_ZILDE           , 0, 0},    // 1B:  ']'
 {  0 },                                                                        // 1C:  CR
 {  0 },                                                                        // 1D:  LCTL & RCTL
//  U    A                        C  AC   S   SA                     SC  SCA
 {L'a', UTF16_ALPHA             , 0, 0, L'A', 0                     , 0, 0},    // 1E:  'a'
 {L's', UTF16_UPSTILE           , 0, 0, L'S', 0                     , 0, 0},    // 1F:  's'
 {L'd', UTF16_DOWNSTILE         , 0, 0, L'D', 0                     , 0, 0},    // 20:  'd'
 {L'f', UTF16_INFINITY          , 0, 0, L'F', 0                     , 0, 0},    // 21:  'f'
 {L'g', UTF16_DEL               , 0, 0, L'G', UTF16_DIERESISDEL     , 0, 0},    // 22:  'g'
 {L'h', UTF16_DELTA             , 0, 0, L'H', UTF16_DELTAUNDERBAR   , 0, 0},    // 23:  'h'
 {L'j', UTF16_JOT               , 0, 0, L'J', UTF16_DIERESISJOT     , 0, 0},    // 24:  'j'
 {L'k', UTF16_APOSTROPHE        , 0, 0, L'K', 0                     , 0, 0},    // 25:  'k'
 {L'l', UTF16_QUAD              , 0, 0, L'L', UTF16_SQUAD           , 0, 0},    // 26:  'l'
 {L';', UTF16_UPTACKJOT         , 0, 0, L':', 0                     , 0, 0},    // 27:  ';'
 {L'\'', UTF16_DOWNTACKJOT      , 0, 0, L'"', 0                     , 0, 0},    // 28:  '''
 {L'`', UTF16_DIAMOND           , 0, 0, L'~', UTF16_COMMABAR        , 0, 0},    // 29:  '`'
 {  0 },                                                                        // 2A:  LSHIFT
//  U    A                        C  AC   S   SA                     SC  SCA
 {L'\\', UTF16_RIGHTTACK        , 0, 0, L'|', UTF16_LEFTTACK        , 0, 0},    // 2B:  '\'
 {L'z', UTF16_LEFTSHOE          , 0, 0, L'Z', 0                     , 0, 0},    // 2C:  'z'
 {L'x', UTF16_RIGHTSHOE         , 0, 0, L'X', 0                     , 0, 0},    // 2D:  'x'
 {L'c', UTF16_UPSHOE            , 0, 0, L'C', 0                     , 0, 0},    // 2E:  'c'
 {L'v', UTF16_DOWNSHOE          , 0, 0, L'V', 0                     , 0, 0},    // 2F:  'v'
 {L'b', UTF16_UPTACK            , 0, 0, L'B', 0                     , 0, 0},    // 30:  'b'
 {L'n', UTF16_DOWNTACK          , 0, 0, L'N', UTF16_DIERESISDOWNTACK, 0, 0},    // 31:  'n'
 {L'm', UTF16_STILE             , 0, 0, L'M', UTF16_STILETILDE      , 0, 0},    // 32:  'm'
 {L',', UTF16_LAMP              , 0, 0, L'<', 0                     , 0, 0},    // 33:  ','
 {L'.', UTF16_SLOPEBAR          , 0, 0, L'>', 0                     , 0, 0},    // 34:  '.'
 {L'/', UTF16_SLASHBAR          , 0, 0, L'?', UTF16_CIRCLEMIDDLEDOT , 0, 0},    // 35:  '/' & PadSlash
 {  0 , },                                                                      // 36:  RSHIFT
 {L'*', UTF16_CIRCLESTAR        , 0, 0, L'*', UTF16_CIRCLESTAR      , 0, 0},    // 37:  PadStar & PrintScreen
 {  0 , },                                                                      // 38:  LALT & RALT
//  U    A                        C  AC   S   SA                     SC  SCA
 {L' ', L' '                    , L' ', L' ', L' ', L' ', L' ', L' '      },    // 39:  Space
 {  0 },                                                                        // 3A:  CapsLock
 {  0 },                                                                        // 3B:  F1
 {  0 },                                                                        // 3C:  F2
 {  0 },                                                                        // 3D:  F3
 {  0 },                                                                        // 3E:  F4
 {  0 },                                                                        // 3F:  F5
 {  0 },                                                                        // 40:  F6
 {  0 },                                                                        // 41:  F7
 {  0 },                                                                        // 42:  F8
 {  0 },                                                                        // 43:  F9
 {  0 },                                                                        // 44:  F10
 {  0 },                                                                        // 45:  NumLock & Pause
 {  0 },                                                                        // 46:  ScrollLock
 {  0 },                                                                        // 47:  Home
 {  0 },                                                                        // 48:  Up
 {  0 },                                                                        // 49:  PageUp
 {L'-', UTF16_TIMES             , 0, 0, L'-', UTF16_TIMES           , 0, 0},    // 4A:  PadMinus
 {  0 },                                                                        // 4B:  Left
 {  0 },                                                                        // 4C:  Center ???
 {  0 },                                                                        // 4D:  Right
 {L'+', UTF16_DOMINO            , 0, 0, L'+', UTF16_DOMINO          , 0, 0},    // 4E:  PadPlus

//  U    A                        C  AC   S   SA                     SC  SCA
/***************** Control And Other Non-character Keys ***********
 {  0 },                                                                        // 4F:  End
 {  0 },                                                                        // 50:  Down
 {  0 },                                                                        // 51:  PageDown
 {  0 },                                                                        // 52:  Insert
 {  0 },                                                                        // 53:  Delete
 {  0 },                                                                        // 54:  SysReq
 {  0 },                                                                        // 55:
 {  0 },                                                                        // 56:
 {  0 },                                                                        // 57:  F11
 {  0 },                                                                        // 58:  F12
 {  0 },                                                                        // 59:
 {  0 },                                                                        // 5A:
 {  0 },                                                                        // 5B:  Left Windows Logo
 {  0 },                                                                        // 5C:  Right Windows Logo
 {  0 },                                                                        // 5D:  Application
 {  0 },                                                                        // 5E:  Power Event
 {  0 },                                                                        // 5F:  Sleep Event
 {  0 },                                                                        // 60:  DO NOT USE
 {  0 },                                                                        // 61:  DO NOT USE
 {  0 },                                                                        // 62:
 {  0 },                                                                        // 63:  Wake Event
*******************************************************************/
}
#endif
,
         aCharCodesNARS_US_EN_CTL[0x4E + 1]
#ifdef DEFINE_VALUES
=                                                                               // ScanCode
{{  0 },                                                                        // 00:  DO NOT USE
 {  0 },                                                                        // 01:  ESC
//  U   A  C                        AC    S  SA  SC                     SCA
 {L'1', 0, UTF16_DIERESIS          , 0, L'!', 0, UTF16_EQUALUNDERBAR   , 0},    // 02:  '1'
 {L'2', 0, UTF16_OVERBAR           , 0, L'@', 0, UTF16_NOTEQUALUNDERBAR, 0},    // 03:  '2'
 {L'3', 0, L'<'                    , 0, L'#', 0, UTF16_DELSTILE        , 0},    // 04:  '3'
 {L'4', 0, UTF16_LEFTCARETUNDERBAR , 0, L'$', 0, UTF16_DELTASTILE      , 0},    // 05:  '4'
 {L'5', 0, L'='                    , 0, L'%', 0, UTF16_CIRCLESTILE     , 0},    // 06:  '5'
 {L'6', 0, UTF16_RIGHTCARETUNDERBAR, 0, L'^', 0, UTF16_CIRCLESLOPE     , 0},    // 07:  '6'
 {L'7', 0, L'>'                    , 0, L'&', 0, UTF16_CIRCLEBAR       , 0},    // 08:  '7'
 {L'8', 0, UTF16_NOTEQUAL          , 0, L'*', 0, UTF16_CIRCLESTAR      , 0},    // 09:  '8'
 {L'9', 0, UTF16_DOWNCARET         , 0, L'(', 0, UTF16_DOWNCARETTILDE  , 0},    // 0A:  '9'
 {L'0', 0, UTF16_UPCARET           , 0, L')', 0, UTF16_UPCARETTILDE    , 0},    // 0B:  '0'
 {L'-', 0, UTF16_TIMES             , 0, L'_', 0, UTF16_QUOTEDOT        , 0},    // 0C:  '-'
 {L'=', 0, UTF16_COLONBAR          , 0, L'+', 0, UTF16_DOMINO          , 0},    // 0D:  '='
 {  0 },                                                                        // 0E:  BS
 {  0 },                                                                        // 0F:  HT
//  U   A  C                        AC    S  SA  SC                     SCA
 {L'q', 0, UTF16_QUERY             , 0, L'Q', 0, 0                     , 0},    // 10:  'q'
 {L'w', 0, UTF16_OMEGA             , 0, L'W', 0, 0                     , 0},    // 11:  'w'
 {L'e', 0, UTF16_EPSILON           , 0, L'E', 0, UTF16_EPSILONUNDERBAR , 0},    // 12:  'e'
 {L'r', 0, UTF16_RHO               , 0, L'R', 0, 0                     , 0},    // 13:  'r'
 {L't', 0, UTF16_TILDE             , 0, L'T', 0, UTF16_DIERESISTILDE   , 0},    // 14:  't'
 {L'y', 0, UTF16_UPARROW           , 0, L'Y', 0, 0                     , 0},    // 15:  'y'
 {L'u', 0, UTF16_DOWNARROW         , 0, L'U', 0, 0                     , 0},    // 16:  'u'
 {L'i', 0, UTF16_IOTA              , 0, L'I', 0, UTF16_IOTAUNDERBAR    , 0},    // 17:  'i'
 {L'o', 0, UTF16_CIRCLE            , 0, L'O', 0, UTF16_DIERESISCIRCLE  , 0},    // 18:  'o'
 {L'p', 0, UTF16_STAR              , 0, L'P', 0, UTF16_DIERESISSTAR    , 0},    // 19:  'p'
 {L'[', 0, UTF16_LEFTARROW         , 0, L'{', 0, UTF16_QUOTEQUAD       , 0},    // 1A:  '['
 {L']', 0, UTF16_RIGHTARROW        , 0, L'}', 0, UTF16_ZILDE           , 0},    // 1B:  ']'
 {  0 },                                                                        // 1C:  CR
 {  0 },                                                                        // 1D:  LCTL & RCTL
//  U   A  C                        AC    S  SA  SC                     SCA
 {L'a', 0, UTF16_ALPHA             , 0, L'A', 0, 0                     , 0},    // 1E:  'a'
 {L's', 0, UTF16_UPSTILE           , 0, L'S', 0, 0                     , 0},    // 1F:  's'
 {L'd', 0, UTF16_DOWNSTILE         , 0, L'D', 0, 0                     , 0},    // 20:  'd'
 {L'f', 0, UTF16_INFINITY          , 0, L'F', 0, 0                     , 0},    // 21:  'f'
 {L'g', 0, UTF16_DEL               , 0, L'G', 0, UTF16_DIERESISDEL     , 0},    // 22:  'g'
 {L'h', 0, UTF16_DELTA             , 0, L'H', 0, UTF16_DELTAUNDERBAR   , 0},    // 23:  'h'
 {L'j', 0, UTF16_JOT               , 0, L'J', 0, UTF16_DIERESISJOT     , 0},    // 24:  'j'
 {L'k', 0, UTF16_APOSTROPHE        , 0, L'K', 0, 0                     , 0},    // 25:  'k'
 {L'l', 0, UTF16_QUAD              , 0, L'L', 0, UTF16_SQUAD           , 0},    // 26:  'l'
 {L';', 0, UTF16_UPTACKJOT         , 0, L':', 0, 0                     , 0},    // 27:  ';'
 {L'\'',0,  UTF16_DOWNTACKJOT      , 0, L'"', 0, 0                     , 0},    // 28:  '''
 {L'`', 0, UTF16_DIAMOND           , 0, L'~', 0, UTF16_COMMABAR        , 0},    // 29:  '`'
 {  0 },                                                                        // 2A:  LSHIFT
//  U   A  C                        AC    S  SA  SC                     SCA
 {L'\\',0,  UTF16_RIGHTTACK        , 0, L'|', 0, UTF16_LEFTTACK        , 0},    // 2B:  '\'
 {L'z', 0, UTF16_LEFTSHOE          , 0, L'Z', 0, 0                     , 0},    // 2C:  'z'
 {L'x', 0, UTF16_RIGHTSHOE         , 0, L'X', 0, 0                     , 0},    // 2D:  'x'
 {L'c', 0, UTF16_UPSHOE            , 0, L'C', 0, 0                     , 0},    // 2E:  'c'
 {L'v', 0, UTF16_DOWNSHOE          , 0, L'V', 0, 0                     , 0},    // 2F:  'v'
 {L'b', 0, UTF16_UPTACK            , 0, L'B', 0, 0                     , 0},    // 30:  'b'
 {L'n', 0, UTF16_DOWNTACK          , 0, L'N', 0, UTF16_DIERESISDOWNTACK, 0},    // 31:  'n'
 {L'm', 0, UTF16_STILE             , 0, L'M', 0, UTF16_STILETILDE      , 0},    // 32:  'm'
 {L',', 0, UTF16_LAMP              , 0, L'<', 0, 0                     , 0},    // 33:  ','
 {L'.', 0, UTF16_SLOPEBAR          , 0, L'>', 0, 0                     , 0},    // 34:  '.'
 {L'/', 0, UTF16_SLASHBAR          , 0, L'?', 0, UTF16_CIRCLEMIDDLEDOT , 0},    // 35:  '/' & PadSlash
 {  0 ,    },                                                                   // 36:  RSHIFT
 {L'*', 0, UTF16_CIRCLESTAR        , 0, L'*', 0, UTF16_CIRCLESTAR      , 0},    // 37:  PadStar & PrintScreen
 {  0 ,    },                                                                   // 38:  LALT & RALT
 {L' ', L' ', L' '                 , L' ', L' ', L' ', L' ', L' '         },    // 39:  Space
 {  0 },                                                                        // 3A:  CapsLock
 {  0 },                                                                        // 3B:  F1
 {  0 },                                                                        // 3C:  F2
 {  0 },                                                                        // 3D:  F3
 {  0 },                                                                        // 3E:  F4
 {  0 },                                                                        // 3F:  F5
 {  0 },                                                                        // 40:  F6
 {  0 },                                                                        // 41:  F7
 {  0 },                                                                        // 42:  F8
 {  0 },                                                                        // 43:  F9
 {  0 },                                                                        // 44:  F10
 {  0 },                                                                        // 45:  NumLock & Pause
 {  0 },                                                                        // 46:  ScrollLock
 {  0 },                                                                        // 47:  Home
 {  0 },                                                                        // 48:  Up
 {  0 },                                                                        // 49:  PageUp
 {L'-', 0, UTF16_TIMES             , 0, L'-', 0, UTF16_TIMES           , 0},    // 4A:  PadMinus
 {  0 },                                                                        // 4B:  Left
 {  0 },                                                                        // 4C:  Center ???
 {  0 },                                                                        // 4D:  Right
 {L'+', 0, UTF16_DOMINO            , 0, L'+', 0, UTF16_DOMINO          , 0},    // 4E:  PadPlus

//  U   A  C                        AC    S   SA                     SC SCA
/***************** Control And Other Non-character Keys ***********
 {  0 },                                                                        // 4F:  End
 {  0 },                                                                        // 50:  Down
 {  0 },                                                                        // 51:  PageDown
 {  0 },                                                                        // 52:  Insert
 {  0 },                                                                        // 53:  Delete
 {  0 },                                                                        // 54:  SysReq
 {  0 },                                                                        // 55:
 {  0 },                                                                        // 56:
 {  0 },                                                                        // 57:  F11
 {  0 },                                                                        // 58:  F12
 {  0 },                                                                        // 59:
 {  0 },                                                                        // 5A:
 {  0 },                                                                        // 5B:  Left Windows Logo
 {  0 },                                                                        // 5C:  Right Windows Logo
 {  0 },                                                                        // 5D:  Application
 {  0 },                                                                        // 5E:  Power Event
 {  0 },                                                                        // 5F:  Sleep Event
 {  0 },                                                                        // 60:  DO NOT USE
 {  0 },                                                                        // 61:  DO NOT USE
 {  0 },                                                                        // 62:
 {  0 },                                                                        // 63:  Wake Event
*******************************************************************/
}
#endif
;

#define KBLEN                   128     // Length of a Keyboard Layout name

typedef struct tagKEYBLAYOUTS
{
    CHARCODE   aCharCodes[0x80];        // 0000:  aCharCodes table (0x80 x 8 x 2)
    UINT       uCharCodesLen;           // 0800:  # actual rows used in the table aCharCodes (<= 0x80)
    WCHAR      wszLayoutName[KBLEN];    // 0804:  Keyboard Layout Name
    UINT       uScanCode2B_RowNum;      // 0884:  Row # (Origin-0) of ScanCode 2B (Backslash & Stile):  1 or 2
    UBOOL      bExtraKeyRow3:1,         // 0888:  00000001:  TRUE iff there is an extra key on the left of row #3
               bUseCXV:1,               //        00000002:  TRUE iff this keyb layout uses Ctrl-C, -X, -V for Copy, Cut, Paste
               bUseZY:1,                //        00000004:  TRUE iff ...                   Ctrl-Z, -Y     for Undo, Redo
               bUseSEQ:1,               //        00000008:  TRUE iff ...                   Ctrl-S, -E, -Q for function editing commands
               bReadOnly:1,             //        00000010:  TRUE iff the layout is read-only (i.e. built-in)
               :27;                     //        FFFFFFE0:  Available bits
                                        // 088C:  Length
} KEYBLAYOUTS, *LPKEYBLAYOUTS;

typedef struct tagKEYBLAYOUTS_BI
{
    LPCHARCODE lpCharCodes;             // 0000:  Ptr to aCharCodesxxx for built-in layouts
    UINT       uCharCodesLen;           // 0004:  # actual rows used in the table pointed to by lpCharCodes
    WCHAR      wszLayoutName[KBLEN];    // 0008:  Keyboard Layout Name
    UINT       uScanCode2B_RowNum;      // 0088:  Row # (Origin-0) of ScanCode 2B (Backslash & Stile):  1 or 2
    UBOOL      bExtraKeyRow3:1,         // 008C:  00000001:  TRUE iff there is an extra key on the left of row #3
               bUseCXV:1,               //        00000002:  TRUE iff this keyb layout uses Ctrl-C, -X, -V for Copy, Cut, Paste
               bUseZY:1,                //        00000004:  TRUE iff ...              uses Ctrl-Z, -Y     for Undo, Redo
               bUseSEQ:1,               //        00000008:  TRUE iff ...                   Ctrl-S, -E, -Q for function editing commands
               :28;                     //        FFFFFFF0:  Available bits
                                        // 0090:  Length
} KEYBLAYOUTS_BI, *LPKEYBLAYOUTS_BI;

#define KEYBLAYOUT_PRE_UK       L"NARS2000-UK Extended"
#define KEYBLAYOUT_PRE_US       L"NARS2000-US English"
#define KEYBLAYOUT_PRE_INTL     L"NARS2000-US International"
#define KEYBLAYOUT_UK_ALT       KEYBLAYOUT_PRE_UK   L" (Alt)"
#define KEYBLAYOUT_US_ALT       KEYBLAYOUT_PRE_US   L" (Alt)"
#define KEYBLAYOUT_INTL_ALT     KEYBLAYOUT_PRE_INTL L" (Alt)"
#define KEYBLAYOUT_UK_CTL       KEYBLAYOUT_PRE_UK   L" (Ctl)"
#define KEYBLAYOUT_US_CTL       KEYBLAYOUT_PRE_US   L" (Ctl)"
#define KEYBLAYOUT_INTL_CTL     KEYBLAYOUT_PRE_INTL L" (Ctl)"

EXTERN
KEYBLAYOUTS_BI aKeybLayoutsBI[]
#ifdef DEFINE_VALUES
=
{
    {aCharCodesNARS_US_EN_ALT, countof (aCharCodesNARS_US_EN_ALT), KEYBLAYOUT_UK_ALT   , 2, TRUE , TRUE , TRUE , TRUE },
    {aCharCodesNARS_US_EN_ALT, countof (aCharCodesNARS_US_EN_ALT), KEYBLAYOUT_US_ALT   , 1, FALSE, TRUE , TRUE , TRUE },
    {aCharCodesNARS_US_EN_ALT, countof (aCharCodesNARS_US_EN_ALT), KEYBLAYOUT_INTL_ALT , 2, FALSE, TRUE , TRUE , TRUE },
    {aCharCodesNARS_US_EN_CTL, countof (aCharCodesNARS_US_EN_CTL), KEYBLAYOUT_UK_CTL   , 2, TRUE , FALSE, FALSE, FALSE},
    {aCharCodesNARS_US_EN_CTL, countof (aCharCodesNARS_US_EN_CTL), KEYBLAYOUT_US_CTL   , 1, FALSE, FALSE, FALSE, FALSE},
    {aCharCodesNARS_US_EN_CTL, countof (aCharCodesNARS_US_EN_CTL), KEYBLAYOUT_INTL_CTL , 2, FALSE, FALSE, FALSE, FALSE},
}
#endif
;

EXTERN
KEYBLAYOUTS aKeybLayoutAct;     // Active keyboard layout

EXTERN
UINT uKeybLayoutNumAct,         // # of active keyboard layout
     uKeybLayoutNumVis,         // # of visible ...
     uKeybLayoutCount,          // Total # keyboard layouts (built-in + user-defined)
     uKeybLayoutBI              // # built-in keyboard layouts
#ifdef DEFINE_VALUES
= countof (aKeybLayoutsBI)      // Initialize to # built-in keyboard layouts
#endif
,    uKeybLayoutUser,           // # user-defined keyboard layouts
     uKeybUnibase,              // Keyboard Unicode base:  10 or 16
     uKeybTCNum,                // Initial keyboard TabCtrl index
     uKeybState;                // Initial keyboard state:  0 to 7 (3 bits, Shift(4), Ctrl(2), Alt(1))

EXTERN
HGLOBAL hGlbKeybLayouts;        // Keyboard layout global memory handle
                                // The contents of this handle is an array of KEYBLAYOUTS strucs
EXTERN
WCHAR wszKeybLayoutAct[KBLEN],  // Active keyboard layout name
      wszKeybLayoutVis[KBLEN];  // Visible .....

EXTERN
LOGFONTW lfKB                   // LOGFONTW for the Keyboard
#ifdef DEFINE_VALUES
 = {DEF_KBLOGFONT}
#endif
;


//***************************************************************************
//  End of File: keyboards.h
//***************************************************************************
