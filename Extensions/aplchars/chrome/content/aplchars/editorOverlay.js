// Javascript routines for APL Chars

    window.addEventListener ("load"  , doAPLOnceWindowLoaded  , false);
    window.addEventListener ("unload", doAPLOnceWindowUnloaded, false);

// The new, nice, simple way of getting notified when a new editor has been created
var gMsgAPLEditorCreationObserver =
{
    observe: function (aSubject, aTopic, aData)
    {
////    myDump ('Topic = ' + aTopic + ', Data = ' + aData);
    var editor = GetCurrentEditor ();                       // nsIEditor

        // Split cases based upon the command
        switch (aTopic)
        {
            case 'obs_documentCreated':
                if (editor && GetCurrentCommandManager () == aSubject)
                {
                    // Because of conflicts with Alt-r and Alt-s keystrokes, we need to disable
                    //   the shortcut keystrokes for F&rom and &Subject.
                    // BTW, setting the .accessKey attribute to a different key and/or
                    //      setting the .disabled attribute to 'true' has no effect;
                    //      only setting the .control attribute to null seems to work.

                    // The label just before the F&rom menulist
                var el = document.getElementById ('msgIdentity').previousSibling;
                    el.control   = null;        // Zap the action so nothing happens
                    el.accessKey = null;        // Zap the accesskey so the UI doesn't show an underlined letter

                    // The label just before the &Subject textbox
                    el = document.getElementById ('msgSubject' ).previousSibling;
                    el.control   = null;        // Zap the action so nothing happens
                    el.accessKey = null;        // Zap the accesskey so the UI doesn't show an underlined letter

////                editor.addDocumentStateListener (gAPLSourceTextListener);
                    GetCurrentEditorElement ().addEventListener ('keypress', onInputAPLKeyPress, false);
////            var cmdMgr = GetCurrentCommandManager ();
////                cmdMgr.removeCommandObserver (gMsgAPLEditorCreationObserver, 'obs_documentCreated', false);
                } // End IF

                break;
        } // End SWITCH
    } // End observe
} // End gMsgAPLEditorCreationObserver


const gAPLSourceTextListener =
{
    NotifyDocumentCreated: function NotifyDocumentCreated ()
    {
////    myDump ('NotifyDocumentCreated');
    var editorEl = GetCurrentEditorElement ();                  // XULElement
        editorEl.addEventListener    ('keypress', onInputAPLKeyPress, false);
    },
    NotifyDocumentWillBeDestroyed: function NotifyDocumentWillBeDestroyed ()
    {
////    myDump ('NotifyDocumentWillBeDestroyed');
    var editorEl = GetCurrentEditorElement ();                  // XULElement
        editorEl.removeEventListener ('keypress', onInputAPLKeyPress, false);
    },
    NotifyDocumentStateChanged: function NotifyDocumentStateChanged (isChanged)
    {
////    myDump ('NotifyDocumentStateChanged ' + isChanged);

        if (isChanged)
        {
            this.NotifyDocumentCreated ();
        } else
        {
            this.NotifyDocumentWillBeDestroyed ();
            GetCurrentEditor ().removeDocumentStateListener (gAPLSourceTextListener);
        } // End IF
    }
};


// Handler for window event listener
function doAPLOnceWindowLoaded ()
{
////myDump ('doAPLOnceWindowLoaded');
var cmdMgr = GetCurrentCommandManager ();
    cmdMgr.addCommandObserver    (gMsgAPLEditorCreationObserver, 'obs_documentCreated', false);
    cmdMgr.addCommandObserver    (gMsgAPLEditorCreationObserver, 'obs_documentWillBeDestroyed', false);
} // End doAPLOnceWindowLoaded


// Handler for window event listener
function doAPLOnceWindowUnloaded ()
{
////myDump ('doAPLOnceWindowUnloaded');
var cmdMgr = GetCurrentCommandManager ();
    cmdMgr.removeCommandObserver (gMsgAPLEditorCreationObserver, 'obs_documentWillBeDestroyed', false);
    cmdMgr.removeCommandObserver (gMsgAPLEditorCreationObserver, 'obs_documentCreated', false);
} // End doAPLOnceWindowUnloaded


// Translate table from charCode to Unicode char
//   for the NARS2000 keyboard layout
var KeyboardNARS2000 =
[                       // charCode
        0,              // 32 =
        0,              // 33 =
        0,              // 34 =
        0,              // 35 =
        0,              // 36 =
        0,              // 37 =
        0,              // 38 =
['\u2355',        0],   // 39 = Alt-'\'' Alt-'"'    down-tack-jot
        0,              // 40 =
        0,              // 41 =
        0,              // 42 =
        0,              // 43 =
['\u235D',        0],   // 44 = Alt-','  Alt-'<'    lamp
['\u00D7', '\u0021'],   // 45 = Alt-'-'  Alt-'_'    times                   quote-dot
['\u2340',        0],   // 46 = Alt-'.'  Alt-'>'    slope-bar
['\u233F',        0],   // 47 = Alt-'/'  Alt-'?'    slash-bar
['\u2227', '\u2372'],   // 48 = Alt-'0'  Alt-')'    up-caret                up-caret-tilde
['\u00A8', '\u2261'],   // 49 = Alt-'1'  Alt-'!'    dieresis                equal-underbar
['\u00AF', '\u2299'],   // 50 = Alt-'2'  Alt-'@'    overbar                 circle-middle-dot
['\u003C', '\u2352'],   // 51 = Alt-'3'  Alt-'#'    left-caret              del-stile
['\u2264', '\u234B'],   // 52 = Alt-'4'  Alt-'$'    left-caret-underbar     delta-stile
['\u003D', '\u233D'],   // 53 = Alt-'5'  Alt-'%'    equal                   circle-stile
['\u2265', '\u2349'],   // 54 = Alt-'6'  Alt-'^'    right-caret-underbar    circle-slope
['\u003E', '\u2296'],   // 55 = Alt-'7'  Alt-'&'    right-caret             circle-bar
['\u2260', '\u235F'],   // 56 = Alt-'8'  Alt-'*'    notequal                circle-star
['\u2228', '\u2371'],   // 57 = Alt-'9'  Alt-'('    down-caret              down-caret-tilde
        0,              // 58 =
['\u234E',        0],   // 59 = Alt-';'  Alt-':'    up-tack-jot
        0,              // 60 =
['\u00F7', '\u2339'],   // 61 = Alt-'='  Alt-'+'    colon-bar               quad-divide
        0,              // 62 =
        0,              // 63 =
        0,              // 64 =
        0,              // 65 = Alt-'A'
        0,              // 66 = Alt-'B'
        0,              // 67 = Alt-'C'
        0,              // 68 = Alt-'D'
 '\u2377',              // 69 = Alt-'E'             epsilon-underbar
        0,              // 70 = Alt-'F'
 '\u2362',              // 71 = Alt-'G'             del-dieresis
 '\u2359',              // 72 = Alt-'H'             delta-underbar
 '\u2378',              // 73 = Alt-'I'             iota-underbar
 '\u2364',              // 74 = Alt-'J'             dieresis-jot
        0,              // 75 = Alt-'K'
 '\u2337',              // 76 = Alt-'L'             squad
 '\u236D',              // 77 = Alt-'M'             stile-tilde
 '\u2361',              // 78 = Alt-'N'             dieresis-down-tack
 '\u2365',              // 79 = Alt-'O'             dieresis-circle
 '\u2363',              // 80 = Alt-'P'             dieresis-star
        0,              // 81 = Alt-'Q'
        0,              // 82 = Alt-'R'
        0,              // 83 = Alt-'S'
 '\u2368',              // 84 = Alt-'T'             dieresis-tilde
        0,              // 85 = Alt-'U'
        0,              // 86 = Alt-'V'
        0,              // 87 = Alt-'W'
        0,              // 88 = Alt-'X'
        0,              // 89 = Alt-'Y'
        0,              // 90 = Alt-'Z'
['\u2190', '\u235E'],   // 91 = Alt-'['  Alt-'{'    left-arrow              quote-quad
['\u22A3', '\u22A2'],   // 92 = Alt-'\\' Alt-'|'    left-tack               right-tack
['\u2192', '\u236C'],   // 93 = Alt-']'  Alt-'}'    right-arrow             zilde
        0,              // 94 =
        0,              // 95 =
['\u22C4', '\u236A'],   // 96 = Alt-'`'  Alt-'~'    diamond                 commabar
 '\u237A',              // 97 = Alt-'a'
 '\u22A5',              // 98 = Alt-'b'
 '\u2229',              // 99 = Alt-'c'
 '\u230A',              //100 = Alt-'d'
 '\u220A',              //101 = Alt-'e'
 '\u221E',              //102 = Alt-'f'
 '\u2207',              //103 = Alt-'g'
 '\u2206',              //104 = Alt-'h'
 '\u2373',              //105 = Alt-'i'
 '\u2218',              //106 = Alt-'j'
 '\u0027',              //107 = Alt-'k'
 '\u2395',              //108 = Alt-'l'
 '\u2223',              //109 = Alt-'m'
 '\u22A4',              //110 = Alt-'n'
 '\u25CB',              //111 = Alt-'o'
 '\u002A',              //112 = Alt-'p'
 '\u003F',              //113 = Alt-'q'
 '\u2374',              //114 = Alt-'r'
 '\u2308',              //115 = Alt-'s'
 '\u007E',              //116 = Alt-'t'
 '\u2193',              //117 = Alt-'u'
 '\u222A',              //118 = Alt-'v'
 '\u2375',              //119 = Alt-'w'
 '\u2283',              //120 = Alt-'x'
 '\u2191',              //121 = Alt-'y'
 '\u2282'               //122 = Alt-'z'
];

// Make a copy of the NARS2000 keyboard layout so
//   it's not the same variable.
var KeyboardDyalogUS = KeyboardNARS2000.slice (0);

// Make the handful of changes from the NARS2000 keyboard layout
KeyboardDyalogUS[39-32][1] = '\u236A';  // Alt-'"' => commabar
KeyboardDyalogUS[44-32][1] = '\u2377';  // Alt-'<' => epsilon-underbar
KeyboardDyalogUS[46-32][1] = '\u2359';  // Alt-'>' => delta-underbar
KeyboardDyalogUS[47-32][1] = '\u2368';  // Alt-'?' => tilde-dieresis
KeyboardDyalogUS[50-32][1] = '\u236B';  // Alt-'2' => del-tilde
KeyboardDyalogUS[59-32][1] = '\u2261';  // Alt-':' => equal-underbar
KeyboardDyalogUS[71-32]    =        0;  // Alt-'G' => <nothing>
KeyboardDyalogUS[72-32]    =        0;  // Alt-'H' => <nothing>
KeyboardDyalogUS[73-32]    =        0;  // Alt-'I' => <nothing>
KeyboardDyalogUS[74-32]    =        0;  // Alt-'J' => <nothing>
KeyboardDyalogUS[77-32]    =        0;  // Alt-'M' => <nothing>
KeyboardDyalogUS[78-32]    =        0;  // Alt-'N' => <nothing>
KeyboardDyalogUS[79-32]    =        0;  // Alt-'O' => <nothing>
KeyboardDyalogUS[84-32]    =        0;  // Alt-'T' => <nothing>


// The table of keyboard layouts
var CodeToAPL = new Object ();
CodeToAPL["NARS2000"] = KeyboardNARS2000;
CodeToAPL["DyalogUS"] = KeyboardDyalogUS;


// Our keypress listener
function onInputAPLKeyPress (aEvent)
{
    // If this is an alt key, ...
    if (aEvent.altKey)
    {
        // Get a ptr to our local preference branch
    var lclPrefs = Components.classes["@mozilla.org/preferences-service;1"]
                             .getService (Components.interfaces.nsIPrefService)
                             .getBranch (gAplCharsRoot);
        // Get the current enabled state
    var charsEnabled = lclPrefs.getBoolPref (gAplCharsCharsEnabled);

        // If we're disabled, just return
        if (!charsEnabled)
            return;

        // Get the current keyboard layout
    var keyboardlayout = lclPrefs.getCharPref (gAplCharsKeyboardLayout);

        // Get the corresponding Unicode char (or pair of Unicode chars
        //   if Unshift/Shift share the same charCode)
    var aplchar = CodeToAPL[keyboardlayout][aEvent.charCode - 32];

////    myDump ('aEvent.charCode = ' + aEvent.charCode + ', typeof = ' + (typeof (aplchar)) + ', CodeToAPL = ' + CodeToAPL[aEvent.charCode - 32]);
        if (typeof (aplchar) == "object")
        {
            if (aEvent.shiftKey)
                aplchar = aplchar[1];
            else
                aplchar = aplchar[0];
        } // End IF

        if (typeof (aplchar) == "string")
        {
        var editor  = GetCurrentEditor ();                  // nsIEditor
            editor.insertText (aplchar);

            // Tell the DOM not to take any default action
            aEvent.preventDefault ();

            // Tell the DOM not to propagate this event farther along
            aEvent.stopPropagation ();
        } // End IF
    } // End IF
} // End onInputAPLKeyPress


