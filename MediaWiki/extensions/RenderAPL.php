<?php

// MediaWiki extension to apply a consistent style to APL expressions
//   by defining a new tag name <apl>...</apl>.

$wgExtensionFunctions[] = "wfAPLExtension";

$FontName = "APL385 Unicode";

function wfAPLExtension ()
{
    global $wgParser;

    // Register the extension with the WikiText parser
    // the first parameter is the name of the new tag.
    // In this case it defines the tag <apl> ... </apl>
    // the second parameter is the callback function for
    // processing the text between the tags
    $wgParser->setHook( "apl", "renderAPL" );
} // End wfAPLExtension


// The callback function for converting the input text to HTML output
function renderAPL ($input, $argv, &$parser)
{
    global $FontName;

    // $argv is an array containing any arguments passed to the
    // extension like <example argument="foo" bar>..
    // Put this on the sandbox page:  (works in MediaWiki 1.5.5)
    //   <example argument="foo" argument2="bar">Testing text **example** in between the new tags</example>
    //    $output = "Text passed into example extension: <br/>$input";
    //    $output .= " <br/> and the value for the arg 'argument' is " . $argv["argument"];
    //    $output .= " <br/> and the value for the arg 'argument2' is: " . $argv["argument2"];
    //    return $output;

    $style = '';

    if ($argv['size'])
        $style .= 'font-size: '   . $argv['size']   . '; ';

    if ($argv['font'])
        $style .= 'font-family: ' . $argv['font']   . '; ';
    else
        $style .= 'font-family: ' . $FontName       . '; ';

    if ($argv['weight'])
        $style .= 'font-weight: ' . $argv['weight'] . '; ';

    return '<span style="' . $style . '">' . $input . '</span>';
} // End renderAPL

?>
