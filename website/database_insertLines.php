<?php
$dbconn = pg_connect("host=giv-geointeraction.uni-muenster.de port=5432 dbname=featuredb user=postgres password=IGIpostgres")
    or die('Verbindungsaufbau fehlgeschlagen: ' . pg_last_error());
	
	
	//$Name = $_POST['Name'];
	$Name = "NAME";
	//$Description = $_POST['Description'];
	$Description = "DESCRIPTION";
	//$Comments = $_POST['Comments'];
	$LatLng = $_POST['Coordinates'];
	$Roadtype = "false";
	

	$query=pg_query($dbconn,"Insert into \"Lines\" values (DEFAULT,'".$LatLng."','".$Roadtype."','".$Name."','".$Description."');");

	
?>
