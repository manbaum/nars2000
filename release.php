<?php

// This is a script to upload a version of NARS2000
//	 to the NARS2000 website.

// It first zips together the executable, help, and other related files
//	 into one file which is named according to the version naming convention
//	 e.g., nars2000-0.0.4.889.zip
//	 where the version string comes from the file NARS2000.ver

// Set the base of the output filename
$FileBase = "NARS2000";

// Set the filename with the version string
$FileVerName = "NARS2000.ver";

// Get the version string
$FileVer = file_get_contents ($FileVerName);

/////$fs = fopen('php://stdout', 'w');
////fwrite ($fs, $FileVer);
////fclose ($fs);

$BND = 'binaries/nd/';
$BHT = 'htdocs/binaries/';

$SRC	 = $BND . $FileBase . '.zip';
$DSTName = $FileBase . '-' . $FileVer . '.zip';
$DST	 = $BHT . $DSTName;

// Copy the already zipped file, renaming it in the process
echo "Copying <$SRC>\n     to <$DST>...\n";
copy ($SRC, $DST);

// Upload the file to the website
$ftp_server = 'ftp.nars2000.org';
$UserName = "nars-bin";
$PassWord = "nars8087";

echo "Connecting to <$ftp_server>...\n";
$id = ftp_connect ($ftp_server) or die("Couldn't connect to $ftp_server");

echo "Logging into <$ftp_server> as <$UserName>...\n";
if (ftp_login ($id, $UserName, $PassWord))
{
	$Count = 0;
	$CountLim = 10;

	echo "Uploading <$DST>\n       to <$ftp_server>\n       as <$DSTName>...\n";
	$ret = ftp_nb_put ($id, $DSTName, $DST, FTP_BINARY);
	while ($ret == FTP_MOREDATA)
	{
		if (++$Count == $CountLim)
		{
			echo ('.');
			$Count = 0;
		} // End IF

		$ret = ftp_nb_continue ($id);
	} // End WHILE

	if ($ret == FTP_FINISHED)
	{
		echo "\nFile <$DST> successfully uploaded\n  to <$DSTName>.\n";
	} else
	{
		echo "\n*** File <$DST> NOT successfully uploaded\n";
	} // End IF/ELSE
} else
{
	echo "***Couldn't connect as <$UserName>\n";
} // End IF/ElSE

ftp_close ($id);

?>
