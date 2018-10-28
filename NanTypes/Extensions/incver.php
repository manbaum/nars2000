<?php
// Script to increment version # in an <install.rdf> file
//   and copy that value to the appropriate line in <about.xul>
//   and <update.rdf>.
// Date:  20 June 2006
// Author:  Bob Smith



// Parse the argument line
if ($_SERVER['argc'] != 4)
{
    // Display a help message
    echo "Help!  Need three arguments:  typically, install.rdf about.xul update.rdf\n";
    exit (1);
} // End IF

//****************************************************//
// First command line argument (e.g., install.rdf).

$FileName = $_SERVER['argv'][1];

// Read in the file contents
$File = file_get_contents ($FileName);
if ($File == false)
{
    echo "Unable to read the filename <$FileName>\n";
    exit (2);
} // End IF

// Find the version #
preg_match ('/em:version=\"(\d+)\.(\d+)\.(\d+)\"/', $File, $matches);

// Increment and save the version #
$VerNum = $matches[1] . '.' . $matches[2] . '.' . ++$matches[3];

// Replace the old version # with the new one
$File = preg_replace ('/em:version=\"\d+\.\d+\.\d+\"/',
                      "em:version=\"$VerNum\"",
                      $File);

// Write out the file contents
file_put_contents ($FileName, $File);


//****************************************************//
// Second command line argument (e.g., about.xul).

$FileName = $_SERVER['argv'][2];

// Read in the file contents
$File = file_get_contents ($FileName);
if ($File == false)
{
    echo "Unable to read the filename <$FileName>\n";
    exit (3);
} // End IF

// Replace the old version # with the new one
$File = preg_replace ('/(\"&version;)(.*?)\"/',
                      "\${1}$VerNum\"",
                      $File);

// Write out the file contents
file_put_contents ($FileName, $File);


//****************************************************//
// Third command line argument (e.g., update.rdf).

$FileName = $_SERVER['argv'][3];

// Read in the file contents
$File = file_get_contents ($FileName);
if ($File == false)
{
    echo "Unable to read the filename <$FileName>\n";
    exit (3);
} // End IF

// Replace the old version # with the new one
$File = preg_replace ('/(<em:version>)(.*?)(<\/em:version>)/',
                      "\${1}$VerNum\${3}",
                      $File);
// Write out the file contents
file_put_contents ($FileName, $File);


echo "Version incremented to $VerNum\n";
exit (0);
?>
