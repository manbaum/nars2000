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
  <style type="text/css">
body
{
  background-color: rgb(180,193,205);
  background-image: url('/art/bg-grad.jpg');
  background-repeat: repeat-x;
  color: black;
  font-family: "trebuchet ms", arial, sans-serif;
  margin: 0;
}
img
{
  border: none;
}
h2
{
  background-color: rgb(204,213,221);
  padding: 2px 0.5em 2px 0.5em;
  margin: 0;
  margin-right:  -0.45em;
  border-bottom: dotted 1px rgb(180,193,205);
  font-size: 110%;
}
h3
{
  padding: 2px 0.5em 2px 0.5em;
  margin: 0;
  font-size: 100%;
  font-weight: bold;
}
h4
{
  padding: 2px 0.5em 2px 0.5em;
  margin: 0;
  font-size: 90%;
  font-weight: bold;
  background-color: inherit;
}
p
{
  padding: 0.25em 0 0.50em 0.5em;
  margin: 0;
}
ul, ol
{
  padding: 0 0 0 0.5em;
  margin: 0 0 0 0.2in;
}
.title
{
  background-image: url(/art/NARS-TitleExt.jpg);
  background-repeat:  repeat-x;
}
.pagetable tr
{
  vertical-align: top;
}
.pagetable
{
  width: 100%;
}
.pagecolumn1
{
  width: 175px;
  padding: 1em 0 0 0;
}
.pagecolumn1 a, .pagecolumn1 a:visited
{
  color: rgb(0,0,164);
  background-color:  inherit;
  text-decoration: none;
}
.pagecolumn1 a:hover
{
  text-decoration: underline;
}
.pagecolumn2
{
  padding: 1em 2em;
}
.section
{
  color:  inherit;
  background-color: white;
  border-color: rgb(24,24,24);
  border-width: 1px;
  border-style: solid;
  padding-bottom: 0.5em;
  padding-right: 0.5em;
  margin-bottom: 1.5em;
}
.reltable
{
  margin-left: 1em;
  border-collapse: collapse;
}
.reltable th, .reltable tr, .reltable td
{
  color: rgb(0,0,164);
  padding: 0 1em 0 1em;
}
.reltable td
{
  border: solid blue 1px;
  font-weight: bold;
  color: seagreen;
}
.relbutton
{
  background-color: orangered;
}
  </style>
</head>

<body>

<div class="title">
<img src="/art/NARS-Title.jpg"
     alt="NARS2000: An Experimental APL Interpreter"
     title="Last Changed <?php echo date ('d F Y H:i:s', filemtime (__FILE__));?>" />
</div>

<table class="pagetable" summary="">
<tr>
  <td class="pagecolumn2">
    <div class="section">
      <h2>Latest File Releases</h2>

      <table class="reltable" border="0" cellspacing="0" summary="">
      <tr>
        <th>File Name</th>
        <th>Release</th>
        <th>Date</th>
        <th>Size</th>
        <th>Type</th>
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
           && strcmp ($File, "linestat.txt") != 0)
          {
              $Files[] = $File;
          } // End IF
      } // End WHILE

      // Sort files descendingly
      rsort ($Files);

      foreach ($Files as $File)
      {
          $Pos  = strpos ($File, '-');
          $Name = substr ($File, 0, $Pos);
          $Rel  = substr ($File, $Pos + 1);
          $ExtPos = strpos (strrev ($File), '.');
          $Ext  = substr ($File, -$ExtPos);    // Extract the extension
          $Rel  = substr ($Rel , 0, -$ExtPos-1); // Remove trailing extension
          $Date = date ("Y F d H:i:s", filemtime ($DirName . $File));
          $Size = number_format (filesize ($DirName . $File));

          echo   "      <tr>\n"
             .   "        <td>$Name</td>\n"
             .   "        <td>$Rel</td>\n"
             .   "        <td>$Date</td>\n"
             .   "        <td>$Size</td>\n"
             .   "        <td>$Ext</td>\n"
             .   "        <td class=\"relbutton\"><a class=\"linkleft\" href=\"binaries/$File\">Download</a></td>\n"
             .   "      </tr>\n";
      } // End FOREACH
      closedir ($dh);
      ?>
      </table>
    </div>
  </td>
</tr>

<tr>
  <td class="pagecolumn2">
    <div class="section">
      <h2>Statistics On The Most Current File Release</h2>
      <pre style="margin-left: 1em;"><?php readfile ($DirName . "linestat.txt");?></pre>
    </div>
  </td>
</tr>
</table>

<?php include "/www/htdocs/ValidXHTML-CSS.html" ?>
<?php include "../footer.html" ?>
</body></html>
