<?php	
	include 'session.php';	
	include 'buttons.php';
	
	$method = $_SERVER['REQUEST_METHOD'];	

	if ($method == "POST")
	{
		$email = $_POST ["email"];
		$email = strtolower ($email);				
		$pw = $_POST ["pw"];
	
		include 'serverinfo.php';	
		
		$ui = GetUserInfo ($email);			

		if (strcmp ($ui ['pw'], $pw) != 0)
		{					
			RedirectToPage ("login.php?email=$email&wpw=main1");
			exit;
		}
		
		if (!SaveSessionInfo ($email))
		{
			RedirectToPage ("login.php?email=$email&wpw=main2");
			exit;			
		}	
	}
	else if ($method == "GET")
	{
		$email = $_GET ["email"];
		$email = strtolower ($email);							
		include 'serverinfo.php';	
		
		if (CheckSession ($email, 0) == 0)
		{
			RedirectToPage ("login.php?email=$email&wpw=main3");
			exit;
		}		
		$ui = GetUserInfo ($email);		
	}	
	$modnames = GetModuleNames ($ui);
?>

<!DOCTYPE html><html lang="en">
<head><meta name="viewport" content="width=device-width, initial-scale=1">
<title>IOT Control Panel</title>
<link href="style.css" rel="stylesheet" type="text/css">   
</head>
<html>
    <body>
	
<?php			
	DisplayLogoutButton ($email);
	echo "<br><br>";
	DisplayButton ("Profile", "editprofile", "email=$email");			
	echo " <span class=\"mytext\">$email</span>";
	echo "<br>";
	echo "<div class=\"divider\"/>";

	foreach ($modnames as $key => $value)					
	{				
		$mi = GetModuleInfo ($value);
		if ($mi ['nick'] != "" && $mi ['email'] != "")
		{
			if ($mi['interval'] == 0)
				$mi ['interval'] = 60;
			$tf = gmdate ("H:i:s", $mi ['interval']);
			echo "<a class=\"myButton moduleButton\" href=\"editmod.php?email=".$email;
			echo "&module=".$value."\">".$value." (".$tf.")<br>";						
			echo $mi['nick']."</a><br>";				
			echo "<div class=\"divider\"/>";
		}
	}
?>	

    </body>
</html>
