<?php	
	include 'session.php';	
	include 'buttons.php';	
	
	$email = $_GET ["email"];	
	$email = strtolower ($email);			
	include 'serverinfo.php';	
	
	$mod = $_GET ["module"];	
	
	if (CheckSession ($email, 0) == 0)
	{
		RedirectToPage ("login.php?email=$email&wpw=profile");
		exit;
	}
	
	$ui = GetUserInfo ($email);	
	$skype = $ui ['skype'];
	$phone = $ui ['phone'];	
	$mi = GetModuleInfo ($mod);
	
	if ($phone == "")
		$phone = "No phone number!";
	if ($skype == "")
		$skype = "No username!";
	$nick = $mi ['nick'];	
	$interval = $mi ['interval'];
	if ($interval == "" || $interval < 30)
		$interval = 60;
	
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
			$option1 = $mi ['option1']; // for example, use $option1 anywhere			
			break;
	}
	/*******************************************************/		
	
?>

<!DOCTYPE html><html lang="en">
<head><meta name="viewport" content="width=device-width, initial-scale=1">
<title>IOT Module: <?php echo $mod; ?></title>
<link href="style.css" rel="stylesheet" type="text/css">   
</head>
<html>
    <body>
	
<?php	DisplayLogoutButton ($email); ?><br><br>

<?php 
	if ($pwtest == "12")
		echo "New Password mismatch Confirm Password!<br>";
	else if ($pwtest == "main")
		echo "Wrong Current Password!<br>";
?>

 <form name="module" action="savemod.php" method="post">		
 
             Email:<br><span class="mytext"><?php echo $email; ?></span>				 
				<br>Phone number (to be used to received SMS):<br><span class="mytext"><?php echo $phone; ?></span>
				<br>Skype UserName (to be used to received message):<br><span class="mytext"><?php echo $skype; ?></span>
				<br><br>
				<fieldset> <legend><?php echo $mod; ?></legend>
				<br>NickName:<br><input type="text" maxlength="32" size="32" name="nickname" 	value="<?php echo $nick; ?>"><br>
				<br>Current Password:<br><input type="password" maxlength="16" size="16" name="pw" 	value=""><br>
            <br>New Password:<br><input type="password" 		maxlength="16" size="16" name="pw1" 	value=""><br>
				<br>Confirm Password:<br><input type="password" maxlength="16" size="16" name="pw2" 	value=""><br>
				<br>Update Interval (in seconds):<br><input type="number" name="interval" 	value="<?php echo $interval; ?>"><br>
								
<?php
	/*******************************************************
		do specific field form to each module
	********************************************************/

	switch ($modtype)
	{
		case "esp":			
			break;
		case "espino":			
			break;
	}
	
	/********************************************************/
?>
				
				<input type="hidden" name="mod" value="<?php echo $mod; ?>">
				<input type="hidden" name="email" value="<?php echo $email; ?>">
            <br><input type="submit" name="save" class="myButton" value="Save">
				<?php DisplayButton ("Go Back", "main", "email=$email"); ?>
				</fieldset>
        </form>
    </body>
</html>
