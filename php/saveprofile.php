<?php	
	include 'session.php';		
		
	$email = $_POST ["email"];
	$email = strtolower ($email);		
	include 'serverinfo.php';
	
	$newemail = $_POST ["email"];
	$pw = $_POST ["pw"];
	$pw1 = $_POST ["pw1"];
	$pw2 = $_POST ["pw2"];
	$phone = $_POST ["phone"];
	$skype = $_POST ["skype"];
	
	$pw = trim ($pw);
	$pw1 = trim ($pw1);
	$pw2 = trim ($pw2);
	
	if (CheckSession ($email, 0) == 0)
	{
		RedirectToPage ("login.php?email=$email&wpw=saveprofile");
		exit;
	}
	
	$ui = GetUserInfo ($email);			

	if ($pw1 != "")
	{
		if ($pw1 != $pw2)
			goto badpw12;		
		if ($pw != $ui ['pw'])
			goto badpw;
		$pw = $pw1;
	}
	else $pw = $ui ['pw'];

	$f = CreateFile ($IotEmailPath);
	if ($f)
	{
		$pw = trim ($pw);
		$phone = trim ($phone);
		$skype = trim ($skype);
		fwrite ($f, "&pw=".$pw."\r\n");
		fwrite ($f, "&phone=".$phone."\r\n");
		fwrite ($f, "&skype=".$skype."\r\n");
		foreach ($ui as $key => $value )
			if (strstr ($key, "mod"))
			{				
			//	echo $key."=".$value."<br>";
				$key = trim ($key);
				$value = trim ($value);
				fwrite ($f, "&".$key."=".$value."\r\n");
			}
		fclose ($f);	
	}
	
end:	
	RedirectToPage ("main.php?email=$email");
	exit;
	
badpw12:
	RedirectToPage ("editprofile.php?email=$email&pw=12");
	exit;
	
badpw:
	RedirectToPage ("editprofile.php?email=$email&pw=main");
	exit;
	
?>
