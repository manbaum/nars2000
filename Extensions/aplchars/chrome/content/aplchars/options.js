
window.addEventListener ("load", initPrefsAPL, false);

// Function to initialize the fields in the Preferences window
function initPrefsAPL ()
{
var keyboardlayout;
var enabledstate;
var prefs = Components.classes["@mozilla.org/preferences-service;1"]
                      .getService (Components.interfaces.nsIPrefBranch);
    // Get the current keyboard layout
    keyboardlayout = prefs.getCharPref (gAplCharsKeyboardLayout);

    // Mark as selected
    document.getElementById (keyboardlayout).checked = true;

    // Get the current enabled state
    enabledstate = prefs.getBoolPref (gAplCharsEnabledState);

    // Mark as enabled or not
    document.getElementById ("enabledstate").checked = enabledstate;
} // End initPrefsAPL

// Function to save the fields in the Preferences window
function savePrefsAPL ()
{
var prefs = Components.classes["@mozilla.org/preferences-service;1"]
                      .getService (Components.interfaces.nsIPrefBranch);

    // Get the current setting
var enabledstate   = document.getElementById ('enabledstate')  .value;
var keyboardlayout = document.getElementById ('keyboardlayout').value;

    // Save the current settings
    prefs.setBoolPref (gAplCharsEnabledState  , enabledstate  );
    prefs.setCharPref (gAplCharsKeyboardLayout, keyboardlayout);
} // End savePrefsAPL

// Display the About Box
function displayAboutAPL ()
{
    window.open ("chrome://aplchars/content/about.xul",
                 "&prefwindow.abouttitle;", "chrome,centerscreen");
} // End displayAboutAPL

