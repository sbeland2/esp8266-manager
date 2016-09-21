<?php	
	include 'session.php';	
	include 'buttons.php';	
	
	$email = $_GET ["email"];	
	$pwtest = $_GET ["pw"];	
	$email = strtolower ($email);			
	include 'serverinfo.php';	
	
	if (CheckSession ($email, 0) == 0)
	{
		RedirectToPage ("login.php?email=$email&wpw=profile");
		exit;
	}
	
	$ui = GetUserInfo ($email);	
	$pw = $ui ['pw'];	
	$skype = $ui ['skype'];
	$phone = $ui ['phone'];	
?>

<!DOCTYPE html><html lang="en">
<head><meta name="viewport" content="width=device-width, initial-scale=1">
<title>IOT Profile</title>
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

 <form name="profile" action="saveprofile.php" method="post">		
 
 
            <br>Email:<br><input type="email" 					name="email" 		value="<?php echo $email; ?>"><br>
				<br>Current Password:<br><input type="password" name="pw" 	value=""><br>
            <br>New Password:<br><input type="password" 		name="pw1" 	value=""><br>
				<br>Confirm Password:<br><input type="password" name="pw2" 	value=""><br>
				<br>Phone number (to be used to received SMS):<br>
				<input type="text" 										name="phone" 		value="<?php echo $phone; ?>"><br>
				<br>Skype UserName (to be used to received message):<br>
				<input type="text" 										name="skype" 		value="<?php echo $skype; ?>"><br>
				
				
            <br><input type="submit" name="save" class="myButton" value="Save">
				<?php DisplayButton ("Go Back", "main", "email=$email"); ?>
        </form>
    </body>
</html>
