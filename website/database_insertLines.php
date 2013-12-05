<?php
$dbconn = pg_connect("host=giv-geointeraction.uni-muenster.de port=5432 dbname=featuredb user=postgres password=IGIpostgres")
    or die('Verbindungsaufbau fehlgeschlagen: ' . pg_last_error());
	
	
	//$Name = $_POST['Name'];
	//$Name='';
	//$Description='';
	//
	//$Description = $_POST['Description'];
	//$Comments = $_POST['Comments'];
	$LatLng = $_POST['Coordinates'];
	//$splitLatLng = explode(',',$LatLng,2);
	//$LatLng = $splitLatLng[1].','.$splitLatLng[0];

	$query=pg_query($dbconn,"Insert into \"Lines\" values (DEFAULT,'".$LatLng."');");

	
?>
