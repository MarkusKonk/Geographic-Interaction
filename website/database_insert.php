<?php
$dbconn = pg_connect("host=localhost dbname=test user=postgres password=***")
    or die('Verbindungsaufbau fehlgeschlagen: ' . pg_last_error());
	
	$query=pg_query($dbconn,"Insert into \"Feature\" values (31,'test','bla','{eins,zwei}','(51.1234,7.12345)');");


	
?>