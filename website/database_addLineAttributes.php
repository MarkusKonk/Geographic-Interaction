<?php
$dbconn = pg_connect("host=giv-geointeraction.uni-muenster.de port=5432 dbname=featuredb user=postgres password=IGIpostgres")
    or die('Verbindungsaufbau fehlgeschlagen: ' . pg_last_error());
	
	
	$Name = $_POST['Name'];
	$Description = $_POST['Description'];
	$ID = $_POST['ID']; 
	//$Comments = $_POST['Comments'];
	$type = $_POST['Type'];

	
	$query=pg_query($dbconn,"UPDATE \"Lines\" SET \"Name\"='".$Name."' where \"ID\"=".$ID.";");
	$query=pg_query($dbconn,"UPDATE \"Lines\" SET \"Description\"='".$Description."' where \"ID\"=".$ID.";");
	$query=pg_query($dbconn,"UPDATE \"Lines\" SET \"IsMainRoad\"='".$type."' where \"ID\"=".$ID.";");
	
	echo $type;
?>
