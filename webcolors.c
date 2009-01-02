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
#include "headers.h"


/*************

Web Color Names in Decimal
--------------------------

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

Web Color Names in Hex
-----------------------

AliceBlue   #F0F8FF
AntiqueWhite    #FAEBD7
Aqua    #00FFFF
Aquamarine      #7FFFD4
Azure   #F0FFFF
Beige   #F5F5DC
Bisque      #FFE4C4
Black   #000000
BlanchedAlmond      #FFEBCD
Blue    #0000FF
BlueViolet      #8A2BE2
Brown   #A52A2A
BurlyWood   #DEB887
CadetBlue   #5F9EA0
Chartreuse      #7FFF00
Chocolate   #D2691E
Coral   #FF7F50
CornflowerBlue      #6495ED
Cornsilk    #FFF8DC
Crimson     #DC143C
Cyan    #00FFFF
DarkBlue    #00008B
DarkCyan    #008B8B
DarkGoldenRod   #B8860B
DarkGray    #A9A9A9
DarkGrey    #A9A9A9
DarkGreen   #006400
DarkKhaki   #BDB76B
DarkMagenta     #8B008B
DarkOliveGreen      #556B2F
Darkorange      #FF8C00
DarkOrchid      #9932CC
DarkRed     #8B0000
DarkSalmon      #E9967A
DarkSeaGreen    #8FBC8F
DarkSlateBlue   #483D8B
DarkSlateGray   #2F4F4F
DarkSlateGrey   #2F4F4F
DarkTurquoise   #00CED1
DarkViolet      #9400D3
DeepPink    #FF1493
DeepSkyBlue     #00BFFF
DimGray     #696969
DimGrey     #696969
DodgerBlue      #1E90FF
FireBrick   #B22222
FloralWhite     #FFFAF0
ForestGreen     #228B22
Fuchsia     #FF00FF
Gainsboro   #DCDCDC
GhostWhite      #F8F8FF
Gold    #FFD700
GoldenRod   #DAA520
Gray    #808080
Grey    #808080
Green   #008000
GreenYellow     #ADFF2F
HoneyDew    #F0FFF0
HotPink     #FF69B4
IndianRed       #CD5C5C
Indigo      #4B0082
Ivory   #FFFFF0
Khaki   #F0E68C
Lavender    #E6E6FA
LavenderBlush   #FFF0F5
LawnGreen   #7CFC00
LemonChiffon    #FFFACD
LightBlue   #ADD8E6
LightCoral      #F08080
LightCyan   #E0FFFF
LightGoldenRodYellow    #FAFAD2
LightGray   #D3D3D3
LightGrey   #D3D3D3
LightGreen      #90EE90
LightPink   #FFB6C1
LightSalmon     #FFA07A
LightSeaGreen   #20B2AA
LightSkyBlue    #87CEFA
LightSlateGray      #778899
LightSlateGrey      #778899
LightSteelBlue      #B0C4DE
LightYellow     #FFFFE0
Lime    #00FF00
LimeGreen   #32CD32
Linen   #FAF0E6
Magenta     #FF00FF
Maroon      #800000
MediumAquaMarine    #66CDAA
MediumBlue      #0000CD
MediumOrchid    #BA55D3
MediumPurple    #9370D8
MediumSeaGreen      #3CB371
MediumSlateBlue     #7B68EE
MediumSpringGreen   #00FA9A
MediumTurquoise     #48D1CC
MediumVioletRed     #C71585
MidnightBlue    #191970
MintCream   #F5FFFA
MistyRose   #FFE4E1
Moccasin    #FFE4B5
NavajoWhite     #FFDEAD
Navy    #000080
OldLace     #FDF5E6
Olive   #808000
OliveDrab   #6B8E23
Orange      #FFA500
OrangeRed   #FF4500
Orchid      #DA70D6
PaleGoldenRod   #EEE8AA
PaleGreen   #98FB98
PaleTurquoise   #AFEEEE
PaleVioletRed   #D87093
PapayaWhip      #FFEFD5
PeachPuff   #FFDAB9
Peru    #CD853F
Pink    #FFC0CB
Plum    #DDA0DD
PowderBlue      #B0E0E6
Purple      #800080
Red     #FF0000
RosyBrown   #BC8F8F
RoyalBlue   #4169E1
SaddleBrown     #8B4513
Salmon      #FA8072
SandyBrown      #F4A460
SeaGreen    #2E8B57
SeaShell    #FFF5EE
Sienna      #A0522D
Silver      #C0C0C0
SkyBlue     #87CEEB
SlateBlue   #6A5ACD
SlateGray   #708090
SlateGrey   #708090
Snow    #FFFAFA
SpringGreen     #00FF7F
SteelBlue   #4682B4
Tan     #D2B48C
Teal    #008080
Thistle     #D8BFD8
Tomato      #FF6347
Turquoise   #40E0D0
Violet      #EE82EE
Wheat   #F5DEB3
White   #FFFFFF
WhiteSmoke      #F5F5F5
Yellow      #FFFF00
YellowGreen     #9ACD32

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
