<?php
include 'session.php';	
include 'serverinfo.php';

$servername = $_SERVER['HTTP_HOST'];

$array = file_get_contents("php://input");

list ($mailtoe, $subjecte, $bodye, $fn, $buf) = split ('!#', $array);


$mailto = base64_decode($mailtoe);
$mailfrom = base64_decode($mailfrome);
$mailname = base64_decode($mailnamee);
$subject = base64_decode($subjecte);
$body = base64_decode($bodye);

$email = $mailto;
include 'serverinfo.php';

$ui = GetUserInfo ($email);	
//var_dump ($ui);
$skype = $ui ['skype'];
$phone = $ui ['phone'];	

$post = "\nYou can control your module by login on the following link:\n";
$post .= "http://$servername/login.php?email=$mailto\n\n";
$post .= "Thank you!\n";

$ret = SendEmail ($email, $subject, $body.$post);

if ($ret)
	echo "Mai1~S3nt"; 
   //       |  | is number 3
   //       | is number 1
else echo "error: $ErrorInfo";


?>
