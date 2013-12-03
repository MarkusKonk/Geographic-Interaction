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
	echo $LatLng[0];

  //$query=pg_query($dbconn,"Insert into \"Feature\" values (42,'test','bla','{eins,zwei}','51.1234,7.12345');");
	$query=pg_query($dbconn,"Insert into \"Lines\" values '{".$LatLng."}';");
	
	
?>
