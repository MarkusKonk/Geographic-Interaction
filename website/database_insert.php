<?php
$dbconn = pg_connect("host=localhost dbname=test user=postgres password=ehrenmann87")
    or die('Verbindungsaufbau fehlgeschlagen: ' . pg_last_error());
	
	$ID = $_POST["ID"];
	$Name = $_POST['Name'];
	$Description = $_POST['Description'];
	//$Comments = $_POST['Comments'];
	$LatLng = $_POST['Coordinates'];
	

  //$query=pg_query($dbconn,"Insert into \"Feature\" values (42,'test','bla','{eins,zwei}','51.1234,7.12345');");
	$query=pg_query($dbconn,"Insert into \"Feature\" values (".$ID.",'".$Name."','".$Description."','{}','".$LatLng[1].",".$LatLng[0]."');");


	
?>