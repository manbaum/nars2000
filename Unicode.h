//****************************************************************************
//  NARS2000 -- Unicode Constants
//****************************************************************************

// When initializing an array of functions (such as PrimFnsTab) use these
//   equates instead of the low-order byte of the corresponding UTF16_xxx
//   so as to avoid a conflict.
#define INDEX_JOTDOT             0x01           // Index for {jot}{dot}
#define INDEX_EPSILON            0x02           // ...       {epsilon}
#define INDEX_DOWNSHOE           0x03           // ...       {downshoe}
#define INDEX_CIRCLESTILE        0x04           // ...       {circlestile}
#define INDEX_SLASHBAR           0x05           // ...       {slashbar}
///////                          0x06           // ...       {delta}
///////                          0x07           // ...       {del}
///////                          0x08           // ...       {upstile}
#define INDEX_DIERESISJOT        0x09           // ...       {dieresisjot}
///////                          0x0A           // ...       {epsilon}/{downstile}
#define INDEX_DIERESISDOWNTACK   0x0B           // ...       {dieresisdowntack}
#define INDEX_DIERESISCIRCLE     0x0C           // ...       {dieresiscircle}
#define INDEX_OPSLASH            0x0D           // ...       {slash} as an operator
#define INDEX_OPSLASHBAR         0x0E           // ...       {slashbar} ...
#define INDEX_OPSLOPE            0x0F           // ...       {slope}    ...
#define INDEX_OPSLOPEBAR         0x10           // ...       {slopebar} ...
///////                          0x11-0x17      // (available)
///////                          0x18           // ...       {jot}

// N.B.:  Whenever changing the above enum (TOKEN_TYPES),
//   be sure to make a corresponding change to
//   <aSymbolNames> in <externs.h>.

// No keystroke equivalents for these as yet
#define UTF16_DIERESISDOT        0x2235         //
#define UTF16_NOTEQUALUNDERBAR   0x2262         // Not-match function
#define UTF16_IBEAM              0x2336         //
#define UTF16_QUADJOT            0x233B         //
#define UTF16_QUADSLOPE          0x2342         //
#define UTF16_QUADLEFTARROW      0x2347         //
#define UTF16_QUADRIGHTARROW     0x2348         //
#define UTF16_QUADUPARROW        0x2350         //
#define UTF16_QUADDOWNARROW      0x2357         //
#define UTF16_LEFTSINGLEQUOTE    0x2018         // Left single quotation mark
#define UTF16_RIGHTSINGLEQUOTE   0x2019         // Right ...
#define UTF16_DELTILDE           0x236B         // Del-tilde

// The alphabet, unshifted
#define UTF16_ALPHA              0x237A         // Alt-'a' - alpha
#define UTF16_UPTACK             0x22A5         // Alt-'b' - base
#define UTF16_UPSHOE             0x2229         // Alt-'c' - intersection
#define UTF16_DOWNSTILE          0x230A         // Alt-'d' - floor
#define UTF16_EPSILON            0x220A         // Alt-'e' - epsilon
#define UTF16_UNDERBAR           0x005F         // Alt-'f' - underbar
#define UTF16_DEL                0x2207         // Alt-'g' - del
#define UTF16_DELTA              0x2206         // Alt-'h' - delta
#define UTF16_IOTA               0x2373         // Alt-'i' - iota
#define UTF16_JOT                0x2218         // Alt-'j' - jot
#define UTF16_APOSTROPHE         0x0027         // Alt-'k' - single quote
#define UTF16_QUAD               0x25AF         // Alt-'l' - quad (9109??)
#define UTF16_STILE              0x2223         // Alt-'m' - modulus
#define UTF16_DOWNTACK           0x22A4         // Alt-'n' - representation
#define UTF16_CIRCLE             0x25CB         // Alt-'o' - circle
#define UTF16_STAR               0x002A         // Alt-'p' - power
#define UTF16_QUERY              0x003F         // Alt-'q' - question-mark
#define UTF16_RHO                0x2374         // Alt-'r' - rho
#define UTF16_UPSTILE            0x2308         // Alt-'s' - ceiling
#define UTF16_TILDE              0x007E         // Alt-'t' - tilde
#define UTF16_DOWNARROW          0x2193         // Alt-'u' - down arrow
#define UTF16_DOWNSHOE           0x222A         // Alt-'v' - union
#define UTF16_OMEGA              0x2375         // Alt-'w' - omega
#define UTF16_RIGHTSHOE          0x2283         // Alt-'x' - disclose
#define UTF16_UPARROW            0x2191         // Alt-'y' - up arrow
#define UTF16_LEFTSHOE           0x2282         // Alt-'z' - enclose

// The alphabet, shifted
//efine UTF16_                                  // Alt-'A' - (none)
//efine UTF16_                                  // Alt-'B' - (none)
//efine UTF16_                                  // Alt-'C' - (none)
//efine UTF16_                                  // Alt-'D' - (none)
#define UTF16_EPSILONUNDERBAR    0x2377         // Alt-'E' - epsilon-underbar
//efine UTF16_                                  // Alt-'F' - (none)
#define UTF16_DIERESISDEL        0x2362         // Alt-'G' - Dual operator        (frog)
#define UTF16_DELTAUNDERBAR      0x2359         // Alt-'H' - delta-underbar
#define UTF16_IOTAUNDERBAR       0x2378         // Alt-'I' - iota-underbar
#define UTF16_DIERESISJOT        0x2364         // Alt-'J' - Rank operator        (hoot)
//efine UTF16_                                  // Alt-'K' - (none)
#define UTF16_SQUAD              0x2337         // Alt-'L' - squad
#define UTF16_STILETILDE         0x236D         // Alt-'M' - Partition operator   (dagger)
#define UTF16_DIERESISDOWNTACK   0x2361         // Alt-'N' - Convolution operator (snout)
#define UTF16_DIERESISCIRCLE     0x2365         // Alt-'O' -                      (holler)
#define UTF16_DIERESISSTAR       0x2363         // Alt-'P' - Power operator       (sourpuss)
//efine UTF16_                                  // Alt-'Q' - (none)
//efine UTF16_                                  // Alt-'R' - (none)
//efine UTF16_                                  // Alt-'S' - (none)
#define UTF16_DIERESISTILDE      0x2368         // Alt-'T' - Commute operator     (frown)
//efine UTF16_                                  // Alt-'U' - (none)
//efine UTF16_                                  // Alt-'V' - (none)
//efine UTF16_                                  // Alt-'W' - (none)
//efine UTF16_                                  // Alt-'X' - (none)
//efine UTF16_                                  // Alt-'Y' - (none)
//efine UTF16_                                  // Alt-'Z' - (none)

// Top row, unshifted
#define UTF16_DIAMOND            0x22C4         // Alt-'`' - diamond (9674??)
#define UTF16_DIERESIS           0x00A8         // Alt-'1' - dieresis
#define UTF16_OVERBAR            0x00AF         // Alt-'2' - high minus
#define UTF16_LEFTCARET          0x003C         // Alt-'3' - less
#define UTF16_LEFTCARETUNDERBAR  0x2264         // Alt-'4' - not more
#define UTF16_EQUAL              0x003D         // Alt-'5' - equal
#define UTF16_RIGHTCARETUNDERBAR 0x2265         // Alt-'6' - not less
#define UTF16_RIGHTCARET         0x003E         // Alt-'7' - more
#define UTF16_NOTEQUAL           0x2260         // Alt-'8' - not equal
#define UTF16_DOWNCARET          0x2228         // Alt-'9' - or
#define UTF16_UPCARET            0x2227         // Alt-'0' - and (94??)
#define UTF16_TIMES              0x00D7         // Alt-'-' - times
#define UTF16_COLONBAR           0x00F7         // Alt-'=' - divide

// Top row, shifted
#define UTF16_COMMABAR           0x236A         // Alt-'~' - comma-bar
#define UTF16_EQUALUNDERBAR      0x2261         // Alt-'!' - match
#define UTF16_CIRCLEMIDDLEDOT    0x2299         // Alt-'@' - circle-middle-dot
#define UTF16_DELSTILE           0x2352         // Alt-'#' - grade-down
#define UTF16_DELTASTILE         0x234B         // Alt-'$' - grade-up
#define UTF16_CIRCLESTILE        0x233D         // Alt-'%' - rotate
#define UTF16_CIRCLESLOPE        0x2349         // Alt-'^' - transpose
#define UTF16_CIRCLEBAR          0x2296         // Alt-'&' - circle-bar
#define UTF16_CIRCLESTAR         0x235F         // Alt-'*' - log
#define UTF16_DOWNCARETTILDE     0x2371         // Alt-'(' - nor
#define UTF16_UPCARETTILDE       0x2372         // Alt-')' - nand
#define UTF16_QUOTEDOT           0x0021         // Alt-'_' - quote-dot
#define UTF16_DOMINO             0x2339         // Alt-'+' - domino

// Second row, unshifted
#define UTF16_LEFTARROW          0x2190         // Alt-'[' - left arrow
#define UTF16_RIGHTARROW         0x2192         // Alt-']' - right arrow
#define UTF16_LEFTTACK           0x22A3         // Alt-'\' - left tack

// Second row, shifted
#define UTF16_QUOTEQUAD          0x235E         // Alt-'{' - quote-quad
#define UTF16_ZILDE              0x236C         // Alt-'}' - zilde
#define UTF16_RIGHTTACK          0x22A2         // Alt-'|' - right tack

// Third row, unshifted
#define UTF16_UPTACKJOT          0x234E         // Alt-';' - execute
#define UTF16_DOWNTACKJOT        0x2355         // Alt-'\''- format

// Third row, shifted
//efine UTF16_                                  // Alt-':' - (none)
//efine UTF16_                                  // Alt-'"' - (none)

// Fourth row, unshifted
#define UTF16_LAMP               0x235D         // Alt-',' - comment
#define UTF16_SLOPEBAR           0x2340         // Alt-'.' - slope-bar
#define UTF16_SLASHBAR           0x233F         // Alt-'/' - slash-bar

// Fourth row, shifted
//efine UTF16_                                  // Alt-'<' - (none)
//efine UTF16_                                  // Alt-'>' - (none)
//efine UTF16_                                  // Alt-'?' - (none)

// Non-Alt key equivalents
#define UTF16_JOTDOT             0x0001         // Pseudo-symbol for {jot}{dot} digraph
#define UTF16_DOUBLEQUOTE        0x0022         // Double quote
#define UTF16_POUND              0x0023         // Pound
#define UTF16_DOLLAR             0x0024         // Dollar
#define UTF16_PERCENT            0x0025         // Percent
#define UTF16_AMPERSAND          0x0026         // Ampersand
#define UTF16_SINGLEQUOTE        0x0027         // Single quote (a.k.a. Apostrophe)
#define UTF16_LPAREN             0x0028         // Left paren
#define UTF16_RPAREN             0x0029         // Right paren
#define UTF16_PLUS               0x002B         // Plus
#define UTF16_COMMA              0x002C         // Comma
#define UTF16_BAR                0x002D         // Bar
#define UTF16_DOT                0x002E         // Dot
#define UTF16_SLASH              0x002F         // Slash
#define UTF16_COLON              0x003A         // Colon
#define UTF16_SEMICOLON          0x003B         // Semicolon
#define UTF16_ATSIGN             0x0040         // At sign
#define UTF16_LBRACKET           0x005B         // Left bracket
#define UTF16_SLOPE              0x005C         // Slope
#define UTF16_RBRACKET           0x005D         // Right bracket
#define UTF16_CIRCUMFLEX         0x005E         // Circumflex
#define UTF16_BACKTICK           0x0060         // Backtick
#define UTF16_LBRACE             0x007B         // Left brace
#define UTF16_STILE2             0x007C         // Stile (a.k.a. 0x2223)
#define UTF16_RBRACE             0x007D         // Right brace


// The corresponding wide string constants

// No keystroke equivalents for these as yet
#define WS_UTF16_DIERESISDOT        L"\x2235"   //
#define WS_UTF16_NOTEQUALUNDERBAR   L"\x2262"   // Not-match function
#define WS_UTF16_IBEAM              L"\x2336"   //
#define WS_UTF16_QUADJOT            L"\x233B"   //
#define WS_UTF16_QUADSLOPE          L"\x2342"   //
#define WS_UTF16_QUADLEFTARROW      L"\x2347"   //
#define WS_UTF16_QUADRIGHTARROW     L"\x2348"   //
#define WS_UTF16_QUADUPARROW        L"\x2350"   //
#define WS_UTF16_QUADDOWNARROW      L"\x2357"   //
#define WS_UTF16_DELTILDE           L"\x236B"   // Del-tilde

// The alphabet, unshifted
#define WS_UTF16_ALPHA              L"\x237A"   // Alt-'a' - alpha
#define WS_UTF16_UPTACK             L"\x22A5"   // Alt-'b' - base
#define WS_UTF16_UPSHOE             L"\x2229"   // Alt-'c' - intersection
#define WS_UTF16_DOWNSTILE          L"\x230A"   // Alt-'d' - floor
#define WS_UTF16_EPSILON            L"\x220A"   // Alt-'e' - epsilon
#define WS_UTF16_UNDERBAR           L"\x005F"   // Alt-'f' - underbar
#define WS_UTF16_DEL                L"\x2207"   // Alt-'g' - del
#define WS_UTF16_DELTA              L"\x2206"   // Alt-'h' - delta
#define WS_UTF16_IOTA               L"\x2373"   // Alt-'i' - iota
#define WS_UTF16_JOT                L"\x2218"   // Alt-'j' - jot
#define WS_UTF16_APOSTROPHE         L"\x0027"   // Alt-'k' - single quote
#define WS_UTF16_QUAD               L"\x25AF"   // Alt-'l' - quad (9109??)
#define WS_UTF16_STILE              L"\x2223"   // Alt-'m' - modulus
#define WS_UTF16_DOWNTACK           L"\x22A4"   // Alt-'n' - representation
#define WS_UTF16_CIRCLE             L"\x25CB"   // Alt-'o' - circle
#define WS_UTF16_STAR               L"\x002A"   // Alt-'p' - power
#define WS_UTF16_QUERY              L"\x003F"   // Alt-'q' - question-mark
#define WS_UTF16_RHO                L"\x2374"   // Alt-'r' - rho
#define WS_UTF16_UPSTILE            L"\x2308"   // Alt-'s' - ceiling
#define WS_UTF16_TILDE              L"\x007E"   // Alt-'t' - tilde
#define WS_UTF16_DOWNARROW          L"\x2193"   // Alt-'u' - down arrow
#define WS_UTF16_DOWNSHOE           L"\x222A"   // Alt-'v' - union
#define WS_UTF16_OMEGA              L"\x2375"   // Alt-'w' - omega
#define WS_UTF16_RIGHTSHOE          L"\x2283"   // Alt-'x' - disclose
#define WS_UTF16_UPARROW            L"\x2191"   // Alt-'y' - up arrow
#define WS_UTF16_LEFTSHOE           L"\x2282"   // Alt-'z' - enclose

// The alphabet, shifted
//efine WS_UTF16_                               // Alt-'A' - (none)
//efine WS_UTF16_                               // Alt-'B' - (none)
//efine WS_UTF16_                               // Alt-'C' - (none)
//efine WS_UTF16_                               // Alt-'D' - (none)
#define WS_UTF16_EPSILONUNDERBAR    L"\x2377"   // Alt-'E' - epsilon-underbar
//efine WS_UTF16_                               // Alt-'F' - (none)
#define WS_UTF16_DIERESISDEL        L"\x2362"   // Alt-'G' - Dual operator        (frog)
#define WS_UTF16_DELTAUNDERBAR      L"\x2359"   // Alt-'H' - delta-underbar
#define WS_UTF16_IOTAUNDERBAR       L"\x2378"   // Alt-'I' - iota-underbar
#define WS_UTF16_DIERESISJOT        L"\x2364"   // Alt-'J' - Rank operator        (hoot)
//efine WS_UTF16_                               // Alt-'K' - (none)
#define WS_UTF16_SQUAD              L"\x2337"   // Alt-'L' - squad
#define WS_UTF16_STILETILDE         L"\x236D"   // Alt-'M' - Partition operator   (dagger)
#define WS_UTF16_DIERESISDOWNTACK   L"\x2361"   // Alt-'N' - Convolution operator (snout)
#define WS_UTF16_DIERESISCIRCLE     L"\x2365"   // Alt-'O' -                      (holler)
#define WS_UTF16_DIERESISSTAR       L"\x2363"   // Alt-'P' - Power operator       (sourpuss)
//efine WS_UTF16_                               // Alt-'Q' - (none)
//efine WS_UTF16_                               // Alt-'R' - (none)
//efine WS_UTF16_                               // Alt-'S' - (none)
#define WS_UTF16_DIERESISTILDE      L"\x2368"   // Alt-'T' - Commute operator     (frown)
//efine WS_UTF16_                               // Alt-'U' - (none)
//efine WS_UTF16_                               // Alt-'V' - (none)
//efine WS_UTF16_                               // Alt-'W' - (none)
//efine WS_UTF16_                               // Alt-'X' - (none)
//efine WS_UTF16_                               // Alt-'Y' - (none)
//efine WS_UTF16_                               // Alt-'Z' - (none)

// Top row, unshifted
#define WS_UTF16_DIAMOND            L"\x22C4"   // Alt-'`' - diamond (9674??)
#define WS_UTF16_DIERESIS           L"\x00A8"   // Alt-'1' - dieresis
#define WS_UTF16_OVERBAR            L"\x00AF"   // Alt-'2' - high minus
#define WS_UTF16_LEFTCARET          L"\x003C"   // Alt-'3' - less
#define WS_UTF16_LEFTCARETUNDERBAR  L"\x2264"   // Alt-'4' - not more
#define WS_UTF16_EQUAL              L"\x003D"   // Alt-'5' - equal
#define WS_UTF16_RIGHTCARETUNDERBAR L"\x2265"   // Alt-'6' - not less
#define WS_UTF16_RIGHTCARET         L"\x003E"   // Alt-'7' - more
#define WS_UTF16_NOTEQUAL           L"\x2260"   // Alt-'8' - not equal
#define WS_UTF16_DOWNCARET          L"\x2228"   // Alt-'9' - or
#define WS_UTF16_UPCARET            L"\x2227"   // Alt-'0' - and (94??)
#define WS_UTF16_TIMES              L"\x00D7"   // Alt-'-' - times
#define WS_UTF16_COLONBAR           L"\x00F7"   // Alt-'=' - divide

// Top row, shifted
#define WS_UTF16_COMMABAR           L"\x236A"   // Alt-'~' - comma-bar
#define WS_UTF16_EQUALUNDERBAR      L"\x2261"   // Alt-'!' - match
#define WS_UTF16_CIRCLEMIDDLEDOT    L"\x2299"   // Alt-'@' - circle-middle-dot
#define WS_UTF16_DELSTILE           L"\x2352"   // Alt-'#' - grade-down
#define WS_UTF16_DELTASTILE         L"\x234B"   // Alt-'$' - grade-up
#define WS_UTF16_CIRCLESTILE        L"\x233D"   // Alt-'%' - rotate
#define WS_UTF16_CIRCLESLOPE        L"\x2349"   // Alt-'^' - transpose
#define WS_UTF16_CIRCLEBAR          L"\x2296"   // Alt-'&' - circle-bar
#define WS_UTF16_CIRCLESTAR         L"\x235F"   // Alt-'*' - log
#define WS_UTF16_DOWNCARETTILDE     L"\x2371"   // Alt-'(' - nor
#define WS_UTF16_UPCARETTILDE       L"\x2372"   // Alt-')' - nand
#define WS_UTF16_QUOTEDOT           L"\x0021"   // Alt-'_' - quote-dot
#define WS_UTF16_DOMINO             L"\x2339"   // Alt-'+' - domino

// Second row, unshifted
#define WS_UTF16_LEFTARROW          L"\x2190"   // Alt-'[' - left arrow
#define WS_UTF16_RIGHTARROW         L"\x2192"   // Alt-']' - right arrow
#define WS_UTF16_LEFTTACK           L"\x22A3"   // Alt-'\' - left tack

// Second row, shifted
#define WS_UTF16_QUOTEQUAD          L"\x235E"   // Alt-'{' - quote-quad
#define WS_UTF16_ZILDE              L"\x236C"   // Alt-'}' - zilde
#define WS_UTF16_RIGHTTACK          L"\x22A2"   // Alt-'|' - right tack

// Third row, unshifted
#define WS_UTF16_UPTACKJOT          L"\x234E"   // Alt-';' - execute
#define WS_UTF16_DOWNTACKJOT        L"\x2355"   // Alt-'\''- format

// Third row, shifted
//efine WS_UTF16_                               // Alt-':' - (none)
//efine WS_UTF16_                               // Alt-'"' - (none)

// Fourth row, unshifted
#define WS_UTF16_LAMP               L"\x235D"   // Alt-',' - comment
#define WS_UTF16_SLOPEBAR           L"\x2340"   // Alt-'.' - slope-bar
#define WS_UTF16_SLASHBAR           L"\x233F"   // Alt-'/' - slash-bar

// Fourth row, shifted
//efine WS_UTF16_                               // Alt-'<' - (none)
//efine WS_UTF16_                               // Alt-'>' - (none)
//efine WS_UTF16_                               // Alt-'?' - (none)

// Non-Alt key equivalents
#define WS_UTF16_JOTDOT             L"\x0001"   // Pseudo-symbol for {jot}{dot} digraph
#define WS_UTF16_DOUBLEQUOTE        L"\x0022"   // Double quote
#define WS_UTF16_POUND              L"\x0023"   // Pound
#define WS_UTF16_DOLLAR             L"\x0024"   // Dollar
#define WS_UTF16_PERCENT            L"\x0025"   // Percent
#define WS_UTF16_AMPERSAND          L"\x0026"   // Ampersand
#define WS_UTF16_SINGLEQUOTE        L"\x0027"   // Single quote (a.k.a. Apostrophe)
#define WS_UTF16_LPAREN             L"\x0028"   // Left paren
#define WS_UTF16_RPAREN             L"\x0029"   // Right paren
#define WS_UTF16_PLUS               L"\x002B"   // Plus
#define WS_UTF16_COMMA              L"\x002C"   // Comma
#define WS_UTF16_BAR                L"\x002D"   // Bar
#define WS_UTF16_DOT                L"\x002E"   // Dot
#define WS_UTF16_SLASH              L"\x002F"   // Slash
#define WS_UTF16_COLON              L"\x003A"   // Colon
#define WS_UTF16_SEMICOLON          L"\x003B"   // Semicolon
#define WS_UTF16_ATSIGN             L"\x0040"   // At sign
#define WS_UTF16_LBRACKET           L"\x005B"   // Left bracket
#define WS_UTF16_SLOPE              L"\x005C"   // Slope
#define WS_UTF16_RBRACKET           L"\x005D"   // Right bracket
#define WS_UTF16_CIRCUMFLEX         L"\x005E"   // Circumflex
#define WS_UTF16_BACKTICK           L"\x0060"   // Backtick
#define WS_UTF16_LBRACE             L"\x007B"   // Left brace
#define WS_UTF16_STILE2             L"\x007C"   // Stile (a.k.a. 0x2223)
#define WS_UTF16_RBRACE             L"\x007D"   // Right brace


//***************************************************************************
//  End of File: Unicode.h
//***************************************************************************
