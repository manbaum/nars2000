<?php

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2010 Sudley Place Software

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

// Script to strip out all but prototype text from a .c file
// Date:  15 December 2008
// Author:  Bob Smith


// Parse the argument line
if ($_SERVER['argc'] != 3)
{
    // Display a help message
    echo "Help!  Need two arguments:  typically, foo.c and RET\\ND32\\foo.pro\n";
    exit (1);
} // End IF


// Define <fnmatch> function in case it doesn't exist
if (!function_exists('fnmatch'))
{
    function fnmatch($pattern, $string)
    {
        for ($op = 0, $npattern = '', $n = 0, $l = strlen($pattern); $n < $l; $n++)
        {
            switch ($c = $pattern[$n])
            {
                case '\\':
                    $npattern .= '\\' . @$pattern[++$n];

                    break;

                case '.':
                case '+':
                case '^':
                case '$':
                case '(':
                case ')':
                case '{':
                case '}':
                case '=':
                case '!':
                case '<':
                case '>':
                case '|':
                    $npattern .= '\\' . $c;

                    break;

                case '?':
                case '*':
                    $npattern .= '.' . $c;

                    break;

                case '[':
                case ']':
                default:
                    $npattern .= $c;
                    if ($c == '[')
                    {
                        $op++;
                    } else
                    if ($c == ']')
                    {
                        if ($op == 0)
                            return false;
                        $op--;
                    } // End IF/ELSE

                    break;
            } // End SWITCH
        } // End FOR

        if ($op != 0) return false;
            return preg_match('/^' . $npattern . '/i', $string);
    } // End fnmatch
} // End IF



//****************************************************//
// Get the command line arguments

$FileWild = $_SERVER['argv'][1];    // Get the input file (possibly with wildcards)
$OutFile  = $_SERVER['argv'][2];    // Get the output file

// Split apart the file directory name and base
$FileDir  = dirname  ($FileWild);
$FileBase = basename ($FileWild);

$hDir = opendir ($FileDir);

// If the file dir doesn't end with a backslash, do so now
if (substr ($FileDir, -1, 1) != "\\")
    $FileDir .= "\\";

// Loop through the files in the dir
while (($FileName = readdir ($hDir)) !== false)
{
    if ($FileName != '.'
     && $FileName != '..'
     && $FileName != 'makever.c')
    {
        if (fnmatch ($FileBase, $FileName)
         && ($FileName == 'bjhash.c'
          || $FileName == 'dtoa.c'
          || $FileName == 'cs_parse.c'
          || $FileName == 'fh_parse.c'
          || $FileName == 'pl_parse.c'
          || $FileName == 'pn_parse.c'))
        {
            // Copy the .proto file from the source dir to the target dir
            $InpFile = basename ($FileName, ".c") . ".proto";

            // Copy the file
            copy ($InpFile, $OutFile);
        } else
        if (fnmatch ($FileBase, $FileName))
        {
            $path_parts = pathinfo ($FileName);
            if (strtolower ($path_parts['extension']) == 'c')
                ProcessFile ($FileDir . $FileName, $OutFile);
        } // End IF/ELSE/...
    } // End IF
} // End WHILE

closedir ($hDir);

exit (0);

// Function to process a single file
function ProcessFile ($FileName, $OutFile)
{
    // Read in the file contents
    $FileNew = file_get_contents ($FileName);
    if ($FileNew == false)
    {
        echo "Unable to read the filename <$FileName>\n";
        exit (2);
    } // End IF

    // In case this is a .c file from a .y file, replace single \n with \r\n
    $count = 1;
    while ($count > 0)
        $FileNew = preg_replace ('/([^\r])\n/', "$1\r\n", $FileNew, -1, $count);

    // Strip out single line comments
    $FileNew = preg_replace ('/\/\/.*?\r\n/s', "\r\n", $FileNew);

    // Strip out multiple line comments
    $FileNew = preg_replace ('/\/\*.*?\*\//s', '', $FileNew);

    // Strip out character strings
    $FileNew = preg_replace ('/".*?"/', '', $FileNew);
    $FileNew = preg_replace ('/\'.*?\'/', '', $FileNew);

    // Strip out txt between braces
    $count = 1;
    while ($count > 0)
        $FileNew = preg_replace ('/{[^{]*?}/s', '', $FileNew, -1, $count);

    // Merge continuation lines
    $count = 1;
    while ($count > 0)
        $FileNew = preg_replace ('/\x5c\r\n/m', '', $FileNew, -1, $count);

    // Strip out #include lines
    $FileNew = preg_replace ('/#\s*include.*?\r\n/', '', $FileNew);

    // Strip out #pragma lines
    $FileNew = preg_replace ('/#\s*pragma.*?\r\n/', '', $FileNew);

    // Strip out #define .* lines
    $FileNew = preg_replace ('/#\s*define .*?\r\n/', '', $FileNew);

    // Strip out typedef {.*}; lines
    $FileNew = preg_replace ('/typedef .*?{.*?};\r\n/s', '', $FileNew);

    // Strip out typedef .*; lines
    $FileNew = preg_replace ('/typedef .*?;\r\n/s', '', $FileNew);

    // Strip out #undef .* lines
    $FileNew = preg_replace ('/#\s*undef .*?\r\n/', '', $FileNew);

    // Strip out static .*?; lines
    $FileNew = preg_replace ('/static.*?;\r\n/', '', $FileNew);

    // Strip out static (.*) lines
    $FileNew = preg_replace ('/static.*?\(.*?\)\r\n/ms', '', $FileNew);

    // Strip out blank lines
    $FileNew = StripBlankLines ($FileNew);

    // Strip out empty #if... #endif statements
    $FileNew = StripEmptyDecl ($FileNew);

    // Strip out trailing blanks on a line
    $FileNew = preg_replace ('/\s+\r\n/', "\r\n", $FileNew);

    // Merge lines that do not begin with a '#' and end with a comma or equal sign
    //   with the next line if it does not begin with a '#'
    $count = 1;
    while ($count > 0)
        $FileNew = preg_replace ('/^([^#].*?)([,=])\r\n([^#])/m', '$1$2$3', $FileNew, -1, $count);

    // Replace ')\r\n' with ');\r\n'
    $FileNew = preg_replace ('/\)\r\n/', ");\r\n", $FileNew);

    // Replace '\r\n\s+\(' with "\r\n("
    $FileNew = preg_replace ('/\r\n\s+\(/', "\r\n(", $FileNew);

    // Strip out spurious blanks
    $count = 1;
    while ($count > 0)
        $FileNew = preg_replace ('/  /', ' ', $FileNew, -1, $count);

    // Merge lines that do not begin with '#' or '(' with a next line
    //   that does begin with '('
    $FileNew = preg_replace ('/^([^#(].*?)\r\n\(/m', '$1 (', $FileNew);

    // Strip out lines that do not begin with '#' and have no parens
    $FileNew = preg_replace ('/^[^#()]+;/m', '', $FileNew);

    // Strip out empty #if... #endif statements
    $FileNew = StripEmptyDecl ($FileNew);

    // Strip out blank lines
    $FileNew = StripBlankLines ($FileNew);

    // Strip out '#ifdef .*?\r\n#endif\r\n'
    $FileNew = preg_replace ('/^#ifdef .*?\r\n#endif\r\n/m', '', $FileNew);

    // Strip out '#ifndef PROTO.*?#endif\r\n'
    $FileNew = preg_replace ('/^#ifndef PROTO.*?^#endif\r\n/ms', '', $FileNew);

    // Strip out '#ifdef __cplusplus.*?#endif\r\n'
    $FileNew = preg_replace ('/^#ifdef __cplusplus.*?#endif\r\n/ms', '', $FileNew);

    // Strip out blank lines
    $FileNew = StripBlankLines ($FileNew);

    // Replace "\r\n;" with ";"
    $FileNew = preg_replace ('/\r\n;/', ';', $FileNew);

    // In lines that begin with a '#' and end with ");", replace the ");" with ")"
    $FileNew = preg_replace ('/(#.*?\));\r\n/', "$1\r\n", $FileNew);

    // Strip out lines that do not begin with '#' and have no parens
    $FileNew = preg_replace ('/^[^#()]+;/m', '', $FileNew);

    // Strip out leading \r\n
    while (substr ($FileNew, 0, 2) == "\r\n")
        $FileNew = substr ($FileNew, 2);

    // Get the resulting length
    $FileLen = strlen ($FileNew);

    if (file_exists ($OutFile))
        // Read in the existing file contents
        $FileOld = file_get_contents ($OutFile);
    else
        $FileOld = false;

    // Compare against the existing file:
    //   if the same, don't write out (and don't change the timestamp)
    if ($FileOld != false
    && $FileLen == strlen ($FileOld)
    && strncmp ($FileNew, $FileOld, $FileLen) == 0)
        return;

    // Write out the file contents
    file_put_contents ($OutFile, $FileNew);

////echo "Output file written to <$OutFile>.\n";
} // End ProcessFile


// Strip out empty declarations
function StripEmptyDecl ($File)
{
    $count1 = $count2 = 1;
    while ($count1 > 0 || $count2 > 0)
    {
        // Strip out '#if.*?(\r\n)+#endif\r\n'
        $File = preg_replace ('/^#\s*if.*?(\r\n\s*)+#\s*endif(\r\n\s*)+/m', '', $File, -1, $count1);

        // Strip out #if...(#elif...)#else...#endif
        $File = preg_replace ('/^#\s*if.*?(\r\n\s*)+(#\s*elif.*?(\r\n\s*)+)*#\s*else(\r\n\s*)+#\s*endif(\r\n\s*)+/m', '', $File, -1, $count2);
    } // End WHILE

    return $File;
} // End StripEmptyDecl


// Strip out blank lines
function StripBlankLines ($File)
{
    $count = 1;
    while ($count > 0)
        $File = preg_replace ('/\r\n\r\n/', "\r\n", $File, -1, $count);

    return $File;
} // End StripBlankLines

?>
