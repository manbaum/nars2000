<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
         "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">

<head>
  <title>NARS2000 Downloads</title>
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
  </style>
</head>

<body>

<div class="title">
<img src="/art/NARS-Title.jpg"
     alt="NARS2000: An Experimental APL Interpreter"
     title="Last Changed <?php echo date ('d F Y H:i:s', filemtime (__FILE__));?>" />

</div>
<div class="menu">&nbsp;&nbsp;
<a href="/">Home</a> |
<a href="/download/">Download</a> |
<a href="http://wiki.nars2000.org">Documentation</a> |
<a href="/Contributors.ahtml">Contributors</a> |
<a href="/LICENSE">License</a> |
<a href="http://wiki.nars2000.org/index.php/NARS2000:About">About</a>
</div>

<table class="pagetable" summary="">
<tr>
  <td class="pagecolumn1">
    <h4>On this page:</h4>

    <ul class="bookmarks">
      <li><a href="#Latest_File_Releases">Latest File Releases</a></li>
      <li><a href="#Statistics">Statistics</a></li>
    </ul>

    <br />
    <br />
    <br />

    <div style="margin-left:  1em;">
    <script type="text/javascript"
            src="http://www.qualitas.com/GoogleAds.js">
    </script>
    <script type="text/javascript"
            src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
    </script>
    </div>
  </td>

  <td class="pagecolumn2">
    <h1 class="header">Downloads</h1>

    <div class="section">
      <h2 id="Latest_File_Releases">Latest File Releases</h2>

      <p>There are two types of files available for downloading:
        <b>zip</b> and <b>map</b>.  The former contains everything you
        need to run the program and is the recommended download for end
        users.  The latter files contain debugging information and are used
        by developers.</p>

      <form id="DisplayForm" action="">&nbsp;
        <input type="button" name="displaytype" id="IDtoggle" value="Display all files" onclick="DisplayTab ();" />
      </form>

      <table border="0" cellspacing="0" summary="">
        <tr>
          <td>
            <table id="reltable" border="0" cellspacing="0" summary="">
            <tr>
              <th>File Name</th>
              <th>Release</th>
              <th>Date</th>
              <th>Size</th>
              <th>Type</th>
              <th>Notes</th>
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
                 && strncmp ($File, "Notes-", 6) != 0)
                {
                    $Files[] = $File;
                } // End IF
            } // End WHILE

            // Sort files descendingly by version #
            natsort ($Files);
            $Files = array_reverse ($Files);

            foreach ($Files as $File)
            {
                $Pos    = strpos ($File, '-');
                $Name   = substr ($File, 0, $Pos);
                $Rel    = substr ($File, $Pos + 1);
                $ExtPos = strpos (strrev ($File), '.');
                $Ext    = substr ($File, -$ExtPos);    // Extract the extension
                $Rel    = substr ($Rel , 0, -$ExtPos-1); // Remove trailing extension
                $Date   = date ("Y F d H:i:s", filemtime ($DirName . $File));
                $Size   = number_format (filesize ($DirName . $File));
                $Notes  = "Notes-$Rel.txt";

                echo   "      <tr class=\"$Ext\">\n"
                   .   "        <td>$Name</td>\n"
                   .   "        <td>$Rel</td>\n"
                   .   "        <td>$Date</td>\n"
                   .   "        <td>$Size</td>\n"
                   .   "        <td>$Ext</td>\n"
                   . (($Ext == "zip") ? "<td><a class=\"linkleft\" href=\"binaries/$Notes\">Release Notes</a></td>\n"
                                      : "<td></td>")
                   .   "        <td class=\"dnlbutton\"><a class=\"linkleft\" href=\"binaries/$File\">Download</a></td>\n"
                   .   "      </tr>\n";
            } // End FOREACH

            closedir ($dh);
            ?>
            </table>
          </td>
          <td><br/><b>&nbsp;&larr;&nbsp;Recommended</b></td>
        </tr>
      </table>

      <script type="text/javascript">
      <!--
        function DisplayTab ()
        {
            // Overcome mis-interpretation/limitation in IE
        var TableRow = gIE ? 'block' : 'table-row';
        var rows = document.getElementsByTagName ('tr');
        var i;
            for (i = 0; i < rows.length; i++)
            {
            var tr = rows[i];
////////////var attr = tr.getAttribute ('class');   // Doesn't work in IE
            var attr = tr.attributes['class'];
                if (attr && attr.value == 'map')
                    tr.style.display = gAllFiles ? TableRow : 'none';
            } // End FOR

        var el = document.getElementById ('IDtoggle');

            // Toggle the button text
            el.value = gAllFiles ? 'Display end user files only'
                                 : 'Display all files';
            // Toggle the state
            gAllFiles = !gAllFiles;
        } // End DisplayTab

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
  </td>
</tr>
</table>

<?php include "/www/htdocs/ValidXHTML-CSS.html" ?>
<?php include "../footer.html" ?>
</body></html>
