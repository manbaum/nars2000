<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
         "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">

<head>
  <title>NARS2000 Downloads</title>
  <base target="_top" />
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
  <meta name="Braces2APL" content="font" />
  <link rel="stylesheet"
        href="http://www.sudleyplace.com/styles/common.css"
        type="text/css" />
  <link rel="stylesheet"
        href="http://www.nars2000.org/styles/common.css"
        type="text/css" />
  <style type="text/css">
#reltable
{
  margin-left: 1em;
  /* border-collapse: collapse; */
}
#reltable th, #reltable tr, #reltable td
{
  color: rgb(0, 0, 164);
  padding: 0 1em 0 1em;
}
#reltable td
{
  border: solid blue 1px;
  font-weight: bold;
  color: rgb(46, 139, 87); /* seagreen */
}
.dnlbutton
{
  background-color: rgb(255, 69, 0); /* orangered */
}
.notes
{
  display: none;
}
  </style>
</head>

<body><!--#config timefmt="%d %B %Y %H:%M:%S" -->

<h1 class="header"
    title="Last Changed <?php echo date ('d F Y H:i:s', filemtime (__FILE__));?>">
Downloads</h1>

<div class="section">
  <h2 id="Latest_File_Releases">Latest File Releases</h2>

  <p>There are several types of files available for downloading:
    <b>NARS2000.zip</b>, <b>NARS2000.map</b>, <b>Release Notes</b>, and
    <b>Special Files</b>.  The <b>zip</b> files contain everything you
    need to run the program and are the recommended downloads for end
    users.  The <b>map</b> files contain debugging information and are used
    by developers.  The <b>Release Notes</b> contain information about the
    corresponding release and are, generally, of most use to developers.  The
    <b>Special Files</b> such as <b>qdebug.zip</b> and <b>gsldir.zip</b>
    contain support files needed by developers.</p>

  <form id="DisplayForm" action="">&nbsp;
    <input type="button" name="displaytype" id="IDtoggle" value="Display all files" onclick="DisplayTab ();" />
  </form>

  <table border="0" cellspacing="0" summary="">
    <tr>
      <td>
        <table id="reltable" border="0" cellspacing="0" summary="">
        <tr>
          <th>File Name</th>
          <th>Version</th>
          <th>Date (GMT)</th>
          <th>Size</th>
          <th>Type</th>
          <th class="notes">Notes</th>
          <th></th>
        </tr>

        <?php
        ob_implicit_flush (true);
        echo "\n";

        $DirName = '/v/nars2000.org/htdocs/download/binaries/';

        $dh = opendir ($DirName);

        // This is the correct way to loop over the directory.
        while (false !== ($File = readdir ($dh)))
        {
            if (!is_dir ($File)
             && strcmp  ($File, "linestat.txt") != 0
             && strcmp  ($File, "nars2000.ver") != 0
             && strncmp ($File, "Notes-", 6) != 0)
            {
                $Files[] = $File;
            } // End IF
        } // End WHILE

        // Sort files descendingly by version #
        //   (which also sorts "gsldir.zip" & "qdebug.zip" to the front)
        natsort ($Files);
        $Files = array_reverse ($Files);

        foreach ($Files as $File)
        {
            // Ignore "robots.txt"
            if (strcmp ($File, "robots.txt") != 0)
            {
                if (strcmp ($File, "gsldir.zip") == 0
                 || strcmp ($File, "qdebug.zip") == 0)
                {
                    $Name   = substr ($File, 0, strpos ($File, '.'));
                    $Rel    = "";
                    $ExtPos = strpos (strrev ($File), '.');
                    $Ext    = substr ($File, -$ExtPos);    // Extract the extension
                    $Class  = "map";
                } else
                {
                    $Pos    = strpos ($File, '-');
                    $Name   = substr ($File, 0, $Pos);
                    $Rel    = substr ($File, $Pos + 1);
                    $ExtPos = strpos (strrev ($File), '.');
                    $Ext    = substr ($File, -$ExtPos);    // Extract the extension
                    $Rel    = substr ($Rel , 0, -$ExtPos-1); // Remove trailing extension
                    $Class  = $Ext;
                    $Notes  = "Notes-$Rel.txt";

                    // Get the corresponding release
                    $CurRel = file_get_contents ('binaries/' . $Notes, false, NULL, 0, strlen ("Build #nnn "));
                } // End IF/ELSE

                $Date   = gmdate ("Y F d H:i:s", filemtime ($DirName . $File));
                $Size   = number_format (filesize ($DirName . $File));

                echo   "      <tr class=\"$Class\">\n"
                   .   "        <td>$Name</td>\n"
                   .   "        <td>$Rel</td>\n"
                   .   "        <td>$Date</td>\n"
                   .   "        <td align=\"right\">$Size</td>\n"
                   .   "        <td>$Ext</td>\n"
                   . (($Class == 'zip')
                   ?   "        <td class=\"notes\"><a target=\"bodyFrame\" class=\"linkleft\" href=\"binaries/$Notes\">$CurRel</a></td>\n"
                   :   "        <td class=\"notes\"></td>\n")
                   .   "        <td class=\"dnlbutton\"><a class=\"linkleft\" href=\"binaries/$File\">Download</a></td>\n"
                   .   "      </tr>\n";
            } // End IF
        } // End FOREACH

        closedir ($dh);
        ?>
        </table>
      </td>
      <td class="recommended" valign="top"><br /><b>&nbsp;&larr;&nbsp;Recommended</b></td>
    </tr>
  </table>

  <script type="text/javascript">
  <!--
    function DisplayTab ()
    {
    var rows = document.getElementsByTagName ('tr');
    var i;
        for (i = 0; i < rows.length; i++)
        {
        var tr = rows[i];
////////var attr = tr.getAttribute ('class');   // Doesn't work in IE
        var attr = tr.attributes['class'];
            if (attr && attr.value == 'map')
                tr.style.display = gAllFiles ? gTableRow : 'none';
        } // End FOR

    var el = document.getElementById ('IDtoggle');

        // Toggle the button text
        el.value = gAllFiles ? 'Display end user files only'
                             : 'Display all files';
        // Toggle the display style of elements with class "notes"
        ToggleClassNotes (document.getElementsByTagName ('td'));
        ToggleClassNotes (document.getElementsByTagName ('th'));

        // Toggle the state
        gAllFiles = !gAllFiles;
    } // End DisplayTab

    function ToggleClassNotes (rows)
    {
    var i;
    var DispNotes = gAllFiles ? gTableCell : 'none';
    var DispRecom = gAllFiles ? 'none' : gTableCell;

        for (i = 0; i < rows.length; i++)
        {
        var td = rows[i];
        var attr = td.attributes['class'];
            if (attr)
            {
                if (attr.value == 'notes')
                    td.style.display = DispNotes;
                else
                if (attr.value == 'recommended')
                    td.style.display = DispRecom;
            } // End IF
        } // End FOR
    } // End ToggleClassNotes

    // We need to overcome various problems with IE
var gIE = (navigator.appName == "Microsoft Internet Explorer");
    if (!gIE)
    {
        // IE screws up border-collapse, so we enable
        //   it for non-IE browsers.  On IE, the inner
        //   lines will be a little thicker than necessary,
        //   but if we allow border-collapse on IE then the
        //   bottom line for zip-only display is missing.
    var el = document.getElementById ('reltable');
        el.style.borderCollapse = 'collapse';
    } // End IF

    // Overcome mis-interpretation/limitation/bug in IE
var gTableRow  = gIE ? 'block' : 'table-row';
var gTableCell = gIE ? 'block' : 'table-cell';

    // Start with just zip files displayed
var gAllFiles = false;
    document.getElementById ("IDtoggle").click ();
  -->
  </script>
</div>

<div class="section">
  <h2 id="Statistics">Statistics On The Most Current File Release</h2>
  <pre style="margin-left: 1em;"><?php readfile ($DirName . "linestat.txt");?></pre>
</div>

<?php include "/www/htdocs/ValidXHTML-CSS.html" ?>
<?php include "../footer.html" ?>
</body></html>
