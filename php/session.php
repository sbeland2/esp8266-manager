<?php

function GetUserInfo ($email)
{
	include 'serverinfo.php';
	$tab = array();
	
	if (!file_exists ($IotEmailPath))
		return NULL;
	$lines = explode ("\n", file_get_contents($IotEmailPath));						
	$n = 0;
	$m = count ($lines);
	for ($i = 0; $i < $m; $i++)
	{
		$keyval = explode ("=", $lines [$i]);	
		$keyval [0] = trim ($keyval [0]);
		$keyval [1] = trim ($keyval [1]);
		//echo $lines [$i]." => ".$keyval [0]."<br>";
		switch ($keyval [0])
		{
			case "&pw":
				$tab ['pw'] = $keyval [1];				
				//echo "<br>case$j: ".$keyval[1]."<br>";
				break;
			case "&phone":
				$tab ['phone'] = $keyval [1];
				break;
			case "&skype":
				$tab ['skype'] = $keyval [1];
				break;
			default:
				if (!strstr ($keyval [0], "&mod"))
					break;
				$modname = 'mod'.$n;
				$tab [$modname] = $keyval [1];
				$n++;
				break;
		}
	}	
	
//	var_dump ($tab);
	return $tab;
}

function GetModuleNames ($uitab)
{	
	$tab = array();
			
	$n = 0;
	foreach ($uitab as $key => $value)	
		if (strstr ($key, "mod"))
		{			
			$value = trim ($value);
			$tab [$n] = $value;
			$n++;
		}

	/*	echo "mydump: ";
	var_dump ($tab);	
	echo "<br>end";
	*/
	return $tab;
}

function GetModuleInfo ($mod)
{	
	include 'serverinfo.php';
	$tab = array();
	
	$fn = $IotPath.$mod.".txt";			
	if (!file_exists ($fn))
		return NULL;
	
	$lines = explode ("&", file_get_contents($fn));													
//var_dump ($lines);
	$m = count ($lines);
	for ($i = 0; $i < $m; $i++)
	{
		$keyval = explode ("=", $lines [$i]);	
		$keyval [0] = trim ($keyval [0]);
		$keyval [1] = trim ($keyval [1]);		
		$tab [$keyval[0]] = $keyval [1];				
	}
	return $tab;
}

function GetClientInfo ()
{
	$client = $_SERVER['HTTP_USER_AGENT'];
	$ip = $_SERVER['REMOTE_ADDR'];
	return $client.$ip;
}

function GetMailServerNumber ()
{
	include 'serverinfo.php';
		
	$fn = $IotPath."mailservernumber.txt";	
		
	if (!file_exists ($fn))
		return 0;
	$f = fopen($fn, "r");
	if ($f)
	{		
		$buf = fread ($f, filesize ($fn));
		$nb = (int) $buf;
		fclose ($f);
		if ($nb >= $MaxNbOfEmailServer)
			return 0;
		return ((int) $buf);
	}
	return 0;
}

function PutMailServerNumber ($buf)
{
	include 'serverinfo.php';
		
	$fn = $IotPath."mailservernumber.txt";	
		
	$f = fopen($fn, "w");
	if ($f)
	{		
		if (flock($f, LOCK_EX)) 
		{          
			fwrite ($f, (string) $buf);
			flock($f, LOCK_UN);
			fclose ($f);				
		}
	}
}

function SaveSessionInfo ($email)
{	
	include 'serverinfo.php';
		
	$f = fopen($IotSessionPath, "w");
	if ($f)
	{
		$clientinfo = GetClientInfo ();
		fwrite ($f, $clientinfo);
		fclose ($f);
		return 1;
	}
	return 0;
}
	
function GetSessionInfo ($email)	
{
	include 'serverinfo.php';

	if (!file_exists ($IotSessionPath))
		return NULL;

	$f = fopen ($IotSessionPath, "r");
	$content = fread ($f, filesize ($IotSessionPath));
	fclose ($f);	
	return $content;
}

function DeleteSession ($email)
{	
	include 'serverinfo.php';

	if (!file_exists ($IotSessionPath))
		return;

	$f = fopen($IotSessionPath, "w");
	fclose ($f);
	unlink ($IotSessionPath);
}

function CheckSession ($email, $delsession)
{
	$session = GetSessionInfo ($email);
	$id = GetClientInfo ();	
	if (strcmp ($id, $session) == 0)
	{
		if ($delsession)
			DeleteSession ($email);			
		return 1;
	}
	return 0;
}

function RedirectToPage ($page)
{
	header ("Location: " . "http://" . $_SERVER['HTTP_HOST'] . "/$page");
}

function SendEmail ($mailto, $subject, $body, $debug=0)
{
	include ('class.phpmailer.php');
	include('class.smtp.php');	
	include 'serverinfo.php';
	
	$mail = new PHPMailer();
	$mail->IsSMTP(); 
	$mail->SMTPDebug  = $debug;                                                                                                          
	$mail->SMTPAuth   = true;                  
	$mail->SMTPSecure = "ssl";                 
	$mail->Host       = "smtp.gmail.com";      
	$mail->Port       = 465;     

	$sn = GetMailServerNumber ();
	$mail->Username   = $ServerEmail [$sn]; 
	$mail->From = $ServerEmail [$sn];
	PutMailServerNumber ($sn + 1);
	$mail->Password   = $ServerEmailPw;            

	$mail->AddAddress ($mailto, "iot user");
	$mail->FromName = $ServerFromName;
	$mail->Subject = $subject;
	$mail->Body = $body;
/* in the future if needed, can add a file as attachment
if (strlen ($fn) > 0)
	$mail->AddStringAttachment ($buf, $fn, 'base64', strstr ($fn, '.png') ? 'image/png' : 'application/octet-stream');
*/

	return ($mail->Send ());
}

?>
