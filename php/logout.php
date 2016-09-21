<?php

	include 'session.php';
	
	$email = $_GET ["email"];	
	CheckSession ($email, 1);
	RedirectToPage ("login.php?wpw=0");	
?>
