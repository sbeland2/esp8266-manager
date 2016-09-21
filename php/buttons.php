<?php

function DisplayLogoutButton ($email)
{
	echo "<a href=\"logout.php?email=$email\" class=\"myButton myButtonLogout\">Logout</a>";	
}


function DisplayButton ($butName, $script, $parameters)
{
	echo "<a href=\"$script.php?$parameters\" class=\"myButton\">$butName</a>";
}

?>
