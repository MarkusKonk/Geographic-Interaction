<?php
$dbconn = pg_connect("host=giv-geointeraction.uni-muenster.de port=5432 dbname=featuredb user=postgres password=IGIpostgres")
    or die('Verbindungsaufbau fehlgeschlagen: ' . pg_last_error());
	
	
	//$Name = $_POST['Name'];
	$Name = "NAME";
	//$Description = $_POST['Description'];
	$Description = "DESCRIPTION";
	//$Comments = $_POST['Comments'];
	$LatLng = $_POST['Coordinates'];
	$splitLatLng = explode(',',$LatLng,2);
	$LatLng = $splitLatLng[1].','.$splitLatLng[0];
	

  //$query=pg_query($dbconn,"Insert into \"Feature\" values (42,'test','bla','{eins,zwei}','51.1234,7.12345');");
	$query=pg_query($dbconn,"Insert into \"Feature\" values (DEFAULT,'".$Name."','".$Description."','{}','".$LatLng."');");
	
	$query2=pg_query($dbconn,'select max("ID") FROM "Feature";');
	
	$lastid;
	
	while ($line = pg_fetch_array($query2, null, PGSQL_ASSOC)) {
		foreach ($line as $col_value) {
			$lastid = $col_value;
		
			}
	}
	echo $lastid;
?>
