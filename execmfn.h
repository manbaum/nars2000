//***************************************************************************
//  NARS2000 -- Magic Function Execution Header
//***************************************************************************

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

// Magic function variables
#define $F              L"F"
#define $L              L"L"
#define $L1             L"L1"
#define $LO             L"LO"
#define $O              L"O"
#define $R              L"R"
#define $RO             L"RO"
#define $X              L"X"
#define $Z              L"Z"

#define $DECODE         WS_UTF16_UPTACK
#define $DIAMOND        WS_UTF16_DIAMOND
#define $DROP           WS_UTF16_DOWNARROW
#define $DISCLOSE       WS_UTF16_RIGHTSHOE
#define $EACH           WS_UTF16_DIERESIS
#define $ENCLOSE        WS_UTF16_LEFTSHOE
#define $ENCODE         WS_UTF16_DOWNTACK
#define $EPSILON        WS_UTF16_EPSILON
#define $FIRST          WS_UTF16_UPARROW
#define $GOTO           WS_UTF16_RIGHTARROW
#define $IOTA           WS_UTF16_IOTA
#define $IS             WS_UTF16_LEFTARROW
#define $JOT            WS_UTF16_JOT
#define $MATCH          WS_UTF16_EQUALUNDERBAR
#define $MAX            WS_UTF16_UPSTILE
#define $MIN            WS_UTF16_DOWNSTILE
#define $NEG            WS_UTF16_OVERBAR
#define $NOTEQUAL       WS_UTF16_NOTEQUAL
#define $QUAD_PROTOTYPE WS_UTF16_QUAD L"PROTOTYPE"
#define $QUAD_ERROR     WS_UTF16_QUAD L"ERROR"
#define $QUAD_NC        WS_UTF16_QUAD L"NC"
#define $QUAD_IO        WS_UTF16_QUAD L"IO"
#define $RANK           WS_UTF16_DIERESISJOT
#define $RANKAXIS       WS_UTF16_DIERESISCIRCLE
#define $REVERSE        WS_UTF16_CIRCLESTILE
#define $RHO            WS_UTF16_RHO
#define $TAKE           WS_UTF16_UPARROW
#define $TILDE          WS_UTF16_TILDE


typedef struct tagMAGIC_FUNCTION
{
    LPAPLCHAR Header;           // Ptr to function/operator header
    LPAPLCHAR *Body;            // Ptr to array of function lines
    UINT      numFcnLines;      // # function lines in the body
} MAGIC_FUNCTION, *LPMAGIC_FUNCTION;


//***************************************************************************
//  End of File: execmfn.h
//***************************************************************************
