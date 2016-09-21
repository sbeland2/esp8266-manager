<?php

include 'session.php';	
include 'serverinfo.php';


$servername = $_SERVER['HTTP_HOST'];

$create = $_GET ["create"];
$enciot = $_GET ["iot"];
$buf = base64_decode($enciot);
$iot = strtolower ($buf);
$iot = trim ($iot);

$encemail = $_GET ["email"];
$buf = base64_decode($encemail);
$email = strtolower ($buf);
$email = trim ($email);

$encmodpw = $_GET ["modpw"];
$buf = base64_decode($encmodpw);
$modpw = strtolower ($buf);
$modpw = trim ($modpw);

//echo "&$iot&$email";

$fn = $IotPath.$iot.".txt";
$fn2 = $IotPath.$iot.".pending.txt";

$error = 0;

if (!file_exists ($fn))
{	
	$f = 0;
	if ($create == "1" || !file_exists ($fn2))		
		$f = fopen ($fn2, "w");
	if ($f)
	{		
		fwrite ($f, "&email=");
		fwrite ($f, $email); 
		fwrite ($f, "\r\n");
		fwrite ($f, "&pw=");
		fwrite ($f, $modpw);
		fwrite ($f, "\r\n");
		fwrite ($f, "&nick=nickname");
		fclose ($f);		
		
		$subject = "Your $iot has been created on our server";		
		$body    = "Hello,\r\n\r\nYou need to activate it by clicking on the following link:\r\n\r\n"
								."http://$servername/activate.php?iot=$enciot&email=$encemail\r\n\r\n"
								."Please Activate your $iot in the following 15 minutes.\r\nThank you";

		if (SendEmail ($email, $subject, $body))
			echo "&Mai1~S3nt";	
   //              |  | is number 3
   //              | is number 1
		else echo "\nerror: $ErrorInfo\n";
	}	
	echo "&pending";
}
else
{	
	$f = fopen ($fn, "r");
	if ($f)
	{
		$content = fread ($f, filesize ($fn));
		$test = "&email=".$email;	
		if (strstr ($content, $test))
			echo $content;
		else $error = 1;
		fclose ($f);
	}	
}

if (!$error)
	echo "&Inf0";

?>
