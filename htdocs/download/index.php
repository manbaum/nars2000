<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
		 "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">

<head>
  <title>NARS2000 Binaries</title>
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








  </style>
</head>

<body><!--#config timefmt="%d %B %Y %H:%M:%S" -->

<div class="title">
<img src="/art/NARS-Title.jpg"
	 alt="NARS2000: An Experimental APL Interpreter"
	 title="Last Changed <!--#echo var='LAST_MODIFIED' -->" />
</div>

<p>The following binaries are available for download:</p>

<table border = "0" style="border: 1px solid black;" summary="">

<?php

$DirName = '/v/nars2000.org/htdocs/binaries/';

$dh = opendir ($DirName);

// This is the correct way to loop over the directory.
while (false !== ($file = readdir ($dh)))
{
	echo '<tr>'
	   .   '<td>' . $file . '</td>'
	   .   '<td>' . (number_format (filesize ($file))) . '</td>'
	   . '</tr>';


} // End WHILE

closedir ($dh);

?>

<!--#include virtual="/ValidXHTML-CSS.html" -->
<!--#include virtual="/footer.html" -->
</body></html>
