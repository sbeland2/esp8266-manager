<?php

$servername = $_SERVER['HTTP_HOST'];

$enc = $_GET ["iot"];
$buf = base64_decode($enc);
$iot = strtolower ($buf);
$iot = trim ($iot);

$enc = $_GET ["email"];
$buf = base64_decode($enc);
$email = strtolower ($buf);
$email = trim ($email);

//echo "$iot&$email";

include 'serverinfo.php';

$fn = $IotPath.$iot.".txt";		
$fn2 = $IotPath.$iot.".pending.txt";
$fn3 = $IotPath.$email.".txt";

if (file_exists ($fn2))
{
	rename ($fn2, $fn);
	echo "Your $iot has been activated on our server.<br>You may now login to our server to control your $iot.<br><br>";
	
	if (!file_exists ($fn3))
	{
		$f = fopen($fn3, "w");
		if ($f)
		{
			$chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
			$pw = substr (str_shuffle ($chars), 0, 8);					
			fwrite ($f, "&pw=".$pw);
			fwrite ($f, "\r\n");
			fwrite ($f, "&mod=".$iot);
			fwrite ($f, "\r\n");
			fclose ($f);
			echo "We have created an account for you using this email address:<br>$email<br><br>And added your $iot to it.";
			echo "<br><br>Your login credentials are:<br>";
			echo "<span class=\"importantText\">";
			echo "username: $email<br>password: $pw<br><br>";
			echo "</span>";
			echo "Note that you should change this auto generated password once logged";
		}
		else echo "Unable to create an account using this email address:<br>$email<br>Please contact us!<br>Reply to this email.";
	}
	else
	{
		$f = fopen($fn3, "r+");
		if ($f)
		{			
			$lines = explode ("\n", file_get_contents($fn3));	
			$m = count ($lines);
			$max = 0;
			for ($i = 0; $i < $m; $i++)
			{
				$keyval = explode ("=", $lines [$i]);
				$keyval [0] = trim ($keyval [0]);
				if (strstr ($keyval [0], "&mod"))
				{
					$s = substr ($keyval [0], 4);
					$num = (int) $s;
					if ($num > $max)
						$max = $num;
				}
			}
			$max++;
			fseek ($f, 0, SEEK_END);
			fwrite ($f, "&mod".$max."=".$iot);
			fwrite ($f, "\r\n");
			fclose ($f);
			echo "Your $iot has been added to your account using this email address:<br>$email";
		}
	}
	echo "<br><br>Login by clicking on the following link:<br><br>";
	echo "<a href=\"http://$servername/login.php?email=$email\">http://$servername/login.php?email=$email</a><br>";
	echo "<br>Thank you.";

	return;
}

echo "Invalid Link";


?>

