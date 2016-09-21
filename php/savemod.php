<?php	
	include 'session.php';		
		
	$email = $_POST ["email"];
	$email = strtolower ($email);	
	$email = trim ($email);
	include 'serverinfo.php';
	
	$mod = $_POST ["mod"];
	$nick = $_POST ["nickname"];
	$pw = $_POST ["pw"];
	$pw1 = $_POST ["pw1"];
	$pw2 = $_POST ["pw2"];	
	$interval = $_POST ["interval"];

	$nick = trim ($nick);
	$interval = trim ($interval);
	$mod = trim ($mod);	
	$pw = trim ($pw);
	$pw1 = trim ($pw1);
	$pw2 = trim ($pw2);
	
	if (CheckSession ($email, 0) == 0)
	{
		RedirectToPage ("login.php?email=$email&wpw=savemodule");
		exit;
	}

	$mi = GetModuleInfo ($mod);

	/*******************************************************
		get specific variables to each module
	********************************************************/
	
	$pos = strrpos ($mod, "-");
	$modtype = substr ($mod, 0, $pos);	
	switch ($modtype)
	{
		case "esp":			
			break;
		case "espino":			
			break;
	}
	/********************************************************/

	if ($pw1 != "")
	{
		if ($pw1 != $pw2)
			goto badpw12;		
		if ($pw != $mi ['pw'])
			goto badpw;
		$pw = $pw1;
	}
	else $pw = $mi ['pw'];

	$fn = $IotPath.$mod.".txt";	
	$f = fopen($fn, "w");
	if ($f)
	{
		$pw = trim ($pw);
		fwrite ($f, "&email=");
		fwrite ($f, $email); 
		fwrite ($f, "\r\n");
		fwrite ($f, "&pw=");
		fwrite ($f, $pw);
		fwrite ($f, "\r\n");
		fwrite ($f, "&nick=");
		fwrite ($f, $nick);
		fwrite ($f, "&interval=");
		fwrite ($f, $interval);
	/*******************************************************
		do specific writing with variables to each module
	********************************************************/
		switch ($modtype)
		{
			case "esp":			
				break;
			case "espino":			
				break;
		}
	/********************************************************/
		
		fclose ($f);	
	}
	
end:	
	RedirectToPage ("main.php?email=$email");
	exit;
	
badpw12:
	RedirectToPage ("editmod.php?email=$email&pw=12");
	exit;
	
badpw:
	RedirectToPage ("editmod.php?email=$email&pw=main");
	exit;
	
?>
