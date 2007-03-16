//****************************************************************************
//  NARS2000 -- Unicode Constants
//
//  Technically, this is UCS-2 which is a subset of UTF-16.
//****************************************************************************

#define UCS2_JOTDOT             0x0001      // Pseudo-symbol for {jot}{dot} digraph

// No keystroke equivalents for these as yet
#define UCS2_DIERESISDOT        0x2235      //                      ()
#define UCS2_NOTEQUALUNDERBAR   0x2262      // Not-match function
#define UCS2_IBEAM              0x2336      //
#define UCS2_QUADJOT            0x233B      //
#define UCS2_QUADSLOPE          0x2342      //
#define UCS2_QUADLEFTARROW      0x2347      //
#define UCS2_QUADRIGHTARROW     0x2348      //
#define UCS2_QUADUPARROW        0x2350      //
#define UCS2_QUADDOWNARROW      0x2357      //
#define UCS2_DIERESISDOWNTACK   0x2361      // Convolution operator (snout)
#define UCS2_DIERESISDEL        0x2362      // Dual operator        (frog)
#define UCS2_DIERESISSTAR       0x2363      // Power operator       (sourpuss)
#define UCS2_DIERESISJOT        0x2364      // Rank operator        (hoot)
#define UCS2_DIERESISCIRCLE     0x2365      //                      (holler)
#define UCS2_DIERESISTILDE      0x2368      // Commute operator     (frown)
#define UCS2_STILETILDE         0x236D      // Partition operator   (dagger)

// The alphabet, unshifted
#define UCS2_ALPHA              0x237A      // Alt-'a' - alpha
#define UCS2_UPTACK             0x22A5      // Alt-'b' - base
#define UCS2_UPSHOE             0x2229      // Alt-'c' - intersection
#define UCS2_DOWNSTILE          0x230A      // Alt-'d' - floor
#define UCS2_EPSILON            0x220A      // Alt-'e' - epsilon
#define UCS2_UNDERBAR           0x005F      // Alt-'f' - underbar
#define UCS2_DEL                0x2207      // Alt-'g' - del
#define UCS2_DELTA              0x2206      // Alt-'h' - delta
#define UCS2_IOTA               0x2373      // Alt-'i' - iota
#define UCS2_JOT                0x2218      // Alt-'j' - jot
#define UCS2_APOSTROPHE         0x0027      // Alt-'k' - single-quote
#define UCS2_QUAD               0x25AF      // Alt-'l' - quad (9109??)
#define UCS2_STILE              0x2223      // Alt-'m' - modulus
#define UCS2_DOWNTACK           0x22A4      // Alt-'n' - representation
#define UCS2_CIRCLE             0x25CB      // Alt-'o' - circle
#define UCS2_STAR               0x002A      // Alt-'p' - power
#define UCS2_QUERY              0x003F      // Alt-'q' - question-mark
#define UCS2_RHO                0x2374      // Alt-'r' - rho
#define UCS2_UPSTILE            0x2308      // Alt-'s' - ceiling
#define UCS2_TILDE              0x007E      // Alt-'t' - tilde
#define UCS2_DNARROW            0x2193      // Alt-'u' - down arrow
#define UCS2_DOWNSHOE           0x222A      // Alt-'v' - union
#define UCS2_OMEGA              0x2375      // Alt-'w' - omega
#define UCS2_RIGHTSHOE          0x2283      // Alt-'x' - disclose
#define UCS2_UPARROW            0x2191      // Alt-'y' - up arrow
#define UCS2_LEFTSHOE           0x2282      // Alt-'z' - enclose

// The alphabet, shifted
//efine UCS2_                               // Alt-'A' - (none)
//efine UCS2_                               // Alt-'B' - (none)
//efine UCS2_                               // Alt-'C' - (none)
//efine UCS2_                               // Alt-'D' - (none)
#define UCS2_EPSILONUNDERBAR    0x2377      // Alt-'E' - epsilon-underbar
//efine UCS2_                               // Alt-'F' - (none)
//efine UCS2_                               // Alt-'G' - (none)
#define UCS2_DELTAUNDERBAR      0x2359      // Alt-'H' - delta-underbar
#define UCS2_IOTAUNDERBAR       0x2378      // Alt-'I' - iota-underbar
//efine UCS2_                               // Alt-'J' - (none)
//efine UCS2_                               // Alt-'K' - (none)
#define UCS2_SQUAD              0x2337      // Alt-'L' - squad
//efine UCS2_                               // Alt-'M' - (none)
#define UCS2_STILETILDE         0x236D      // Alt-'M' - Partition operator   (dagger)
//efine UCS2_                               // Alt-'N' - (none)
//efine UCS2_                               // Alt-'O' - (none)
//efine UCS2_                               // Alt-'P' - (none)
//efine UCS2_                               // Alt-'Q' - (none)
//efine UCS2_                               // Alt-'R' - (none)
//efine UCS2_                               // Alt-'S' - (none)
//efine UCS2_                               // Alt-'T' - (none)
//efine UCS2_                               // Alt-'U' - (none)
//efine UCS2_                               // Alt-'V' - (none)
//efine UCS2_                               // Alt-'W' - (none)
//efine UCS2_                               // Alt-'X' - (none)
//efine UCS2_                               // Alt-'Y' - (none)
//efine UCS2_                               // Alt-'Z' - (none)

// Top row, unshifted
#define UCS2_DIAMOND            0x22C4      // Alt-'`' - diamond (9674??)
#define UCS2_DIERESIS           0x00A8      // Alt-'1' - dieresis
#define UCS2_OVERBAR            0x00AF      // Alt-'2' - high minus
#define UCS2_LEFTCARET          0x003C      // Alt-'3' - less
#define UCS2_NOTMORE            0x2264      // Alt-'4' - not more
#define UCS2_EQUAL              0x003D      // Alt-'5' - equal
#define UCS2_NOTLESS            0x2265      // Alt-'6' - not less
#define UCS2_RIGHTCARET         0x003E      // Alt-'7' - more
#define UCS2_NOTEQUAL           0x2260      // Alt-'8' - not equal
#define UCS2_DOWNCARET          0x2228      // Alt-'9' - or
#define UCS2_UPCARET            0x2227      // Alt-'0' - and (94??)
#define UCS2_TIMES              0x00D7      // Alt-'-' - times
#define UCS2_DIVIDE             0x00F7      // Alt-'=' - divide

// Top row, shifted
#define UCS2_COMMABAR           0x236A      // Alt-'~' - comma-bar
#define UCS2_EQUALUNDERBAR      0x2261      // Alt-'!' - match
#define UCS2_DELTILDE           0x236B      // Alt-'@' - del-tilde
#define UCS2_DELSTILE           0x2352      // Alt-'#' - grade-down
#define UCS2_DELTASTILE         0x234B      // Alt-'$' - grade-up
#define UCS2_CIRCLESTILE        0x233D      // Alt-'%' - rotate
#define UCS2_CIRCLESLOPE        0x2349      // Alt-'^' - transpose
#define UCS2_CIRCLEBAR          0x2296      // Alt-'&' - circle-bar
#define UCS2_CIRCLESTAR         0x235F      // Alt-'*' - log
#define UCS2_DOWNCARETTILDE     0x2371      // Alt-'(' - nor
#define UCS2_UPCARETTILDE       0x2372      // Alt-')' - nand
#define UCS2_QUOTEDOT           0x0021      // Alt-'_' - quote-dot
#define UCS2_DOMINO             0x2339      // Alt-'+' - domino

// Second row, unshifted
#define UCS2_LEFTARROW          0x2190      // Alt-'[' - left arrow
#define UCS2_RIGHTARROW         0x2192      // Alt-']' - right arrow
#define UCS2_LEFTTACK           0x22A3      // Alt-'\\'- left tack

// Second row, shifted
#define UCS2_QUOTEQUAD          0x235E      // Alt-'{' - quote-quad
#define UCS2_ZILDE              0x236C      // Alt-'}' - zilde
#define UCS2_RIGHTTACK          0x22A2      // Alt-'|' - right tack

// Third row, unshifted
#define UCS2_HYDRANT            0x234E      // Alt-';' - execute
#define UCS2_THORN              0x2355      // Alt-'\''- format

// Third row, shifted
//efine UCS2_                               // Alt-':' - (none)
//efine UCS2_                               // Alt-'"' - (none)

// Fourth row, unshifted
#define UCS2_LAMP               0x235D      // Alt-',' - comment
#define UCS2_SLOPEBAR           0x2340      // Alt-'.' - slope-bar
#define UCS2_SLASHBAR           0x233F      // Alt-'/' - slash-bar

// Fourth row, shifted
//efine UCS2_                               // Alt-'<' - (none)
//efine UCS2_                               // Alt-'>' - (none)
//efine UCS2_                               // Alt-'?' - (none)

// Non-Alt key equivalents
#define UCS2_DOUBLEQUOTE        0x0022      // Double quote
#define UCS2_POUND              0x0023      // Pound
#define UCS2_DOLLAR             0x0024      // Dollar
#define UCS2_PERCENT            0x0025      // Percent
#define UCS2_AMPERSAND          0x0026      // Ampersand
#define UCS2_SINGLEQUOTE        0x0027      // Single quote (a.k.a. Apostrophe)
#define UCS2_LPAREN             0x0028      // Left paren
#define UCS2_RPAREN             0x0029      // Right paren
#define UCS2_PLUS               0x002B      // Plus
#define UCS2_COMMA              0x002C      // Comma
#define UCS2_BAR                0x002D      // Bar
#define UCS2_DOT                0x002E      // Dot
#define UCS2_SLASH              0x002F      // Slash
#define UCS2_COLON              0x003A      // Colon
#define UCS2_SEMICOLON          0x003B      // Semicolon
#define UCS2_ATSIGN             0x0040      // At sign
#define UCS2_LBRACKET           0x005B      // Left bracket
#define UCS2_SLOPE              0x005C      // Slope
#define UCS2_RBRACKET           0x005D      // Right bracket
#define UCS2_CIRCUMFLEX         0x005E      // Circumflex
#define UCS2_BACKTICK           0x0060      // Backtick
#define UCS2_LBRACE             0x007B      // Left brace
#define UCS2_STILE2             0x007C      // Stile (a.k.a. 0x2223)
#define UCS2_RBRACE             0x007D      // Right brace


// The corresponding wide string constants

// The alphabet, unshifted
#define WS_UCS2_ALPHA           L"\x237A"   // Alt-'a' - alpha
#define WS_UCS2_UPTACK          L"\x22A5"   // Alt-'b' - base
#define WS_UCS2_UPSHOE          L"\x2229"   // Alt-'c' - intersection
#define WS_UCS2_DOWNSTILE       L"\x230A"   // Alt-'d' - floor
#define WS_UCS2_EPSILON         L"\x220A"   // Alt-'e' - epsilon
#define WS_UCS2_UNDERBAR        L"\x005F"   // Alt-'f' - underbar
#define WS_UCS2_DEL             L"\x2207"   // Alt-'g' - del
#define WS_UCS2_DELTA           L"\x2206"   // Alt-'h' - delta
#define WS_UCS2_IOTA            L"\x2373"   // Alt-'i' - iota
#define WS_UCS2_JOT             L"\x2218"   // Alt-'j' - jot
#define WS_UCS2_APOSTROPHE      L"\x0027"   // Alt-'k' - single-quote
#define WS_UCS2_QUAD            L"\x25AF"   // Alt-'l' - quad (9109??)
#define WS_UCS2_STILE           L"\x2223"   // Alt-'m' - modulus
#define WS_UCS2_DOWNTACK        L"\x22A4"   // Alt-'n' - representation
#define WS_UCS2_CIRCLE          L"\x25CB"   // Alt-'o' - circle
#define WS_UCS2_STAR            L"\x002A"   // Alt-'p' - power
#define WS_UCS2_QUERY           L"\x003F"   // Alt-'q' - question-mark
#define WS_UCS2_RHO             L"\x2374"   // Alt-'r' - rho
#define WS_UCS2_UPSTILE         L"\x2308"   // Alt-'s' - ceiling
#define WS_UCS2_TILDE           L"\x007E"   // Alt-'t' - tilde
#define WS_UCS2_DNARROW         L"\x2193"   // Alt-'u' - down arrow
#define WS_UCS2_DOWNSHOE        L"\x222A"   // Alt-'v' - union
#define WS_UCS2_OMEGA           L"\x2375"   // Alt-'w' - omega
#define WS_UCS2_RIGHTSHOE       L"\x2283"   // Alt-'x' - disclose
#define WS_UCS2_UPARROW         L"\x2191"   // Alt-'y' - up arrow
#define WS_UCS2_LEFTSHOE        L"\x2282"   // Alt-'z' - enclose

// The alphabet, shifted
//efine WS_UCS2_                            // Alt-'A' - (none)
//efine WS_UCS2_                            // Alt-'B' - (none)
//efine WS_UCS2_                            // Alt-'C' - (none)
//efine WS_UCS2_                            // Alt-'D' - (none)
#define WS_UCS2_EPSILONUNDERBAR L"\x2377"   // Alt-'E' - epsilon-underbar
//efine WS_UCS2_                            // Alt-'F' - (none)
//efine WS_UCS2_                            // Alt-'G' - (none)
#define WS_UCS2_DELTAUNDERBAR   L"\x2359"   // Alt-'H' - delta-underbar
#define WS_UCS2_IOTAUNDERBAR    L"\x2378"   // Alt-'I' - iota-underbar
//efine WS_UCS2_                            // Alt-'J' - (none)
//efine WS_UCS2_                            // Alt-'K' - (none)
#define WS_UCS2_SQUAD           L"\x2337"   // Alt-'L' - squad
//efine WS_UCS2_                            // Alt-'M' - (none)
//efine WS_UCS2_                            // Alt-'N' - (none)
//efine WS_UCS2_                            // Alt-'O' - (none)
//efine WS_UCS2_                            // Alt-'P' - (none)
//efine WS_UCS2_                            // Alt-'Q' - (none)
//efine WS_UCS2_                            // Alt-'R' - (none)
//efine WS_UCS2_                            // Alt-'S' - (none)
//efine WS_UCS2_                            // Alt-'T' - (none)
//efine WS_UCS2_                            // Alt-'U' - (none)
//efine WS_UCS2_                            // Alt-'V' - (none)
//efine WS_UCS2_                            // Alt-'W' - (none)
//efine WS_UCS2_                            // Alt-'X' - (none)
//efine WS_UCS2_                            // Alt-'Y' - (none)
//efine WS_UCS2_                            // Alt-'Z' - (none)

// Top row, unshifted
#define WS_UCS2_DIAMOND         L"\x22C4"   // Alt-'`' - diamond (9674??)
#define WS_UCS2_DIERESIS        L"\x00A8"   // Alt-'1' - dieresis
#define WS_UCS2_OVERBAR         L"\x00AF"   // Alt-'2' - high minus
#define WS_UCS2_LEFTCARET       L"\x003C"   // Alt-'3' - less
#define WS_UCS2_NOTMORE         L"\x2264"   // Alt-'4' - not more
#define WS_UCS2_EQUAL           L"\x003D"   // Alt-'5' - equal
#define WS_UCS2_NOTLESS         L"\x2265"   // Alt-'6' - not less
#define WS_UCS2_RIGHTCARET      L"\x003E"   // Alt-'7' - more
#define WS_UCS2_NOTEQUAL        L"\x2260"   // Alt-'8' - not equal
#define WS_UCS2_DOWNCARET       L"\x2228"   // Alt-'9' - or
#define WS_UCS2_UPCARET         L"\x2227"   // Alt-'0' - and (94??)
#define WS_UCS2_TIMES           L"\x00D7"   // Alt-'-' - times
#define WS_UCS2_DIVIDE          L"\x00F7"   // Alt-'=' - divide

// Top row, shifted
#define WS_UCS2_COMMABAR        L"\x236A"   // Alt-'~' - comma-bar
#define WS_UCS2_EQUALUNDERBAR   L"\x2261"   // Alt-'!' - match
#define WS_UCS2_DELTILDE        L"\x236B"   // Alt-'@' - del-tilde
#define WS_UCS2_DELSTILE        L"\x2352"   // Alt-'#' - grade-down
#define WS_UCS2_DELTASTILE      L"\x234B"   // Alt-'$' - grade-up
#define WS_UCS2_CIRCLESTILE     L"\x233D"   // Alt-'%' - rotate
#define WS_UCS2_CIRCLESLOPE     L"\x2349"   // Alt-'^' - transpose
#define WS_UCS2_CIRCLEBAR       L"\x2296"   // Alt-'&' - circle-bar
#define WS_UCS2_CIRCLESTAR      L"\x235F"   // Alt-'*' - log
#define WS_UCS2_DOWNCARETTILDE  L"\x2371"   // Alt-'(' - nor
#define WS_UCS2_UPCARETTILDE    L"\x2372"   // Alt-')' - nand
#define WS_UCS2_QUOTEDOT        L"\x0021"   // Alt-'_' - quote-dot
#define WS_UCS2_DOMINO          L"\x2339"   // Alt-'+' - domino

// Second row, unshifted
#define WS_UCS2_LEFTARROW       L"\x2190"   // Alt-'[' - left arrow
#define WS_UCS2_RIGHTARROW      L"\x2192"   // Alt-']' - right arrow
#define WS_UCS2_LEFTTACK        L"\x22A3"   // Alt-'\\'- left tack

// Second row, shifted
#define WS_UCS2_QUOTEQUAD       L"\x235E"   // Alt-'{' - quote-quad
#define WS_UCS2_ZILDE           L"\x236C"   // Alt-'}' - zilde
#define WS_UCS2_RIGHTTACK       L"\x22A2"   // Alt-'|' - right tack

// Third row, unshifted
#define WS_UCS2_HYDRANT         L"\x234E"   // Alt-';' - execute
#define WS_UCS2_THORN           L"\x2355"   // Alt-'\''- format

// Third row, shifted
//efine WS_UCS2_                            // Alt-':' - (none)
//efine WS_UCS2_                            // Alt-'"' - (none)

// Fourth row, unshifted
#define WS_UCS2_LAMP            L"\x235D"   // Alt-',' - comment
#define WS_UCS2_SLOPEBAR        L"\x2340"   // Alt-'.' - slope-bar
#define WS_UCS2_SLASHBAR        L"\x233F"   // Alt-'/' - slash-bar

// Fourth row, shifted
//efine WS_UCS2_                            // Alt-'<' - (none)
//efine WS_UCS2_                            // Alt-'>' - (none)
//efine WS_UCS2_                            // Alt-'?' - (none)

// Non-Alt key equivalents
#define WS_UCS2_DOUBLEQUOTE     L"\x0022"   // Double quote
#define WS_UCS2_POUND           L"\x0023"   // Pound
#define WS_UCS2_DOLLAR          L"\x0024"   // Dollar
#define WS_UCS2_PERCENT         L"\x0025"   // Percent
#define WS_UCS2_AMPERSAND       L"\x0026"   // Ampersand
#define WS_UCS2_SINGLEQUOTE     L"\x0027"   // Single quote (a.k.a. Apostrophe)
#define WS_UCS2_LPAREN          L"\x0028"   // Left paren
#define WS_UCS2_RPAREN          L"\x0029"   // Right paren
#define WS_UCS2_PLUS            L"\x002B"   // Plus
#define WS_UCS2_COMMA           L"\x002C"   // Comma
#define WS_UCS2_BAR             L"\x002D"   // Bar
#define WS_UCS2_DOT             L"\x002E"   // Dot
#define WS_UCS2_SLASH           L"\x002F"   // Slash
#define WS_UCS2_COLON           L"\x003A"   // Colon
#define WS_UCS2_SEMICOLON       L"\x003B"   // Semicolon
#define WS_UCS2_ATSIGN          L"\x0040"   // At sign
#define WS_UCS2_LBRACKET        L"\x005B"   // Left bracket
#define WS_UCS2_SLOPE           L"\x005C"   // Slope
#define WS_UCS2_RBRACKET        L"\x005D"   // Right bracket
#define WS_UCS2_CIRCUMFLEX      L"\x005E"   // Circumflex
#define WS_UCS2_BACKTICK        L"\x0060"   // Backtick
#define WS_UCS2_LBRACE          L"\x007B"   // Left brace
#define WS_UCS2_STILE2          L"\x007C"   // Stile (a.k.a. 0x2223)
#define WS_UCS2_RBRACE          L"\x007D"   // Right brace

//***************************************************************************
//  End of File: Unicode.h
//***************************************************************************
