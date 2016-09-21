<?php 

$email = $_GET ["email"]; 
$wrongpw = $_GET ["wpw"]; 

?>

<!DOCTYPE html><html lang="en">
<head><meta name="viewport" content="width=device-width, initial-scale=1">
<title>IOT Login</title>
<link href="style.css" rel="stylesheet" type="text/css">   
</head>
<html>
    <body>
	 
        <form name="login" action="main.php" method="post">									
				<?php if ($wrongpw != "" && $wrongpw != "0") echo "You typed a wrong password!<br>Error: $wrongpw<br>Try again!<br>"; ?>
            <br>Email:<br><input type="email" name="email" value="<?php echo $email; ?>"><br>
            <br>Password:<br><input type="password" name="pw" value=""><br>
				
            <br><input type="submit" name="signin" class="myButton" value="Sign In"><br>
				
        </form>
	 
    </body>
</html>



