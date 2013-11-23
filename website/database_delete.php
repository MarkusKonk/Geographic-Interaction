<?php
$dbconn = pg_connect("host=giv-geointeraction.uni-muenster.de port=5432 dbname=featuredb user=postgres password=IGIpostgres")
    or die('Verbindungsaufbau fehlgeschlagen: ' . pg_last_error());
	
	
	$ID = $_POST['ID'];

	

  //$query=pg_query($dbconn,"Insert into \"Feature\" values (42,'test','bla','{eins,zwei}','51.1234,7.12345');");
	$query=pg_query($dbconn,'Delete from "Feature" where "ID"='.$ID.';');

	

?>
