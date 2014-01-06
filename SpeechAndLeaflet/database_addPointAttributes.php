<?php
$dbconn = pg_connect("host=giv-geointeraction.uni-muenster.de port=5432 dbname=featuredb user=postgres password=IGIpostgres")
    or die('Verbindungsaufbau fehlgeschlagen: ' . pg_last_error());
	
	
	$Name = $_POST['Name'];
	$Description = $_POST['Description'];
	//$Comments = $_POST['Comments'];
	$ID = $_POST['ID'];
	
	
	

  //$query=pg_query($dbconn,"Insert into \"Feature\" values (42,'test','bla','{eins,zwei}','51.1234,7.12345');");
	$query=pg_query($dbconn,"UPDATE \"Feature\" SET \"Name\"='".$Name."' where \"ID\"=".$ID.";");
	$query=pg_query($dbconn,"UPDATE \"Feature\" SET \"Description\"='".$Description."' where \"ID\"=".$ID.";");
	
?>
