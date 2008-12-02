//***************************************************************************
//  NARS2000 -- Web Color Names Window
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

#define STRICT
#include <windows.h>
#include <windowsx.h>

#include "main.h"
#include "resdebug.h"
#include "resource.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

// Web Color Names
/*************

GIMP Palette
Name: Web Named Colors
#
# ColorZilla Web Named Colors palette
#
0 0 0   black
105 105 105 dimgray
105 105 105 dimgrey
128 128 128 grey
128 128 128 gray
169 169 169 darkgray
169 169 169 darkgrey
192 192 192 silver
211 211 211 lightgray
211 211 211 lightgrey
220 220 220 gainsboro
245 245 245 whitesmoke
255 255 255 white
255 250 250 snow
188 143 143 rosybrown
240 128 128 lightcoral
205 92 92   indianred
165 42 42   brown
178 34 34   firebrick
128 0 0 maroon
139 0 0 darkred
255 0 0 red
255 228 225 mistyrose
250 128 114 salmon
255 99 71   tomato
233 150 122 darksalmon
255 127 80  coral
255 160 122 lightsalmon
255 69 0    orangered
160 82 45   sienna
255 245 238 seashell
210 105 30  chocolate
139 69 19   saddlebrown
255 218 185 peachpuff
244 164 96  sandybrown
250 240 230 linen
205 133 63  peru
255 228 196 bisque
255 140 0   darkorange
250 235 215 antiquewhite
210 180 140 tan
222 184 135 burlywood
255 222 173 navajowhite
255 239 213 papayawhip
255 235 205 blanchedalmond
255 228 181 moccasin
255 250 240 floralwhite
253 245 230 oldlace
245 222 179 wheat
255 165 0   orange
218 165 32  goldenrod
184 134 11  darkgoldenrod
255 248 220 cornsilk
255 215 0   gold
255 250 205 lemonchiffon
240 230 140 khaki
238 232 170 palegoldenrod
189 183 107 darkkhaki
255 255 240 ivory
245 245 220 beige
255 255 224 lightyellow
250 250 210 lightgoldenrodyellow
128 128 0   olive
255 255 0   yellow
107 142 35  olivedrab
154 205 50  yellowgreen
85 107 47   darkolivegreen
173 255 47  greenyellow
124 252 0   lawngreen
127 255 0   chartreuse
240 255 240 honeydew
143 188 143 darkseagreen
144 238 144 lightgreen
152 251 152 palegreen
34 139 34   forestgreen
50 205 50   limegreen
0 100 0 darkgreen
0 128 0 green
0 255 0 lime
60 179 113  mediumseagreen
46 139 87   seagreen
245 255 250 mintcream
0 255 127   springgreen
0 250 154   mediumspringgreen
102 205 170 mediumaquamarine
127 255 212 aquamarine
64 224 208  turquoise
32 178 170  lightseagreen
72 209 204  mediumturquoise
240 255 255 azure
224 255 255 lightcyan
175 238 238 paleturquoise
47 79 79    darkslategrey
47 79 79    darkslategray
0 128 128   teal
0 139 139   darkcyan
0 206 209   darkturquoise
0 255 255   cyan
0 255 255   aqua
95 158 160  cadetblue
176 224 230 powderblue
173 216 230 lightblue
0 191 255   deepskyblue
135 206 235 skyblue
135 206 250 lightskyblue
240 248 255 aliceblue
70 130 180  steelblue
30 144 255  dodgerblue
112 128 144 slategrey
112 128 144 slategray
119 136 153 lightslategrey
119 136 153 lightslategray
176 196 222 lightsteelblue
100 149 237 cornflowerblue
65 105 225  royalblue
248 248 255 ghostwhite
230 230 250 lavender
25 25 112   midnightblue
0 0 128 navy
0 0 139 darkblue
0 0 205 mediumblue
0 0 255 blue
72 61 139   darkslateblue
106 90 205  slateblue
123 104 238 mediumslateblue
147 112 219 mediumpurple
138 43 226  blueviolet
75 0 130    indigo
153 50 204  darkorchid
148 0 211   darkviolet
186 85 211  mediumorchid
216 191 216 thistle
221 160 221 plum
238 130 238 violet
128 0 128   purple
139 0 139   darkmagenta
255 0 255   fuchsia
255 0 255   magenta
218 112 214 orchid
199 21 133  mediumvioletred
255 20 147  deeppink
255 105 180 hotpink
255 240 245 lavenderblush
219 112 147 palevioletred
220 20 60   crimson
255 192 203 pink
255 182 193 lightpink

*******************************/



//***************************************************************************
//  $WCWndProc
//
//  Message processing routine for the Web Color Names window
//***************************************************************************

LRESULT APIENTRY WCWndProc
    (HWND hWnd,         // Window handle
     UINT message,      // Type of message
     UINT wParam,       // Additional information
     LONG lParam)       // ...

{
////LCLODSAPI ("WC:", hWnd, message, wParam, lParam);
    switch (message)
    {
        case WM_CREATE:
            DbgBrk ();

            // Create




            break;                  // Continue with next handler

        case WM_CLOSE:
            // This also tells the child windows to close
            DestroyWindow (hWnd);

            break;                  // Continue with next handler

        case WM_DESTROY:
            break;                  // Continue with next handler

    } // End SWITCH

////LCLODSAPI ("WCZ:", hWnd, message, wParam, lParam);
    return DefWindowProcW (hWnd, message, wParam, lParam);
} // End WCWndProc


//***************************************************************************
//  End of File: webcolornames.c
//***************************************************************************
