<?php
$dbconn = pg_connect("host=giv-geointeraction.uni-muenster.de port=5432 dbname=featuredb user=postgres password=IGIpostgres")
    or die('Verbindungsaufbau fehlgeschlagen: ' . pg_last_error());
	
	$field_value = $_POST['isMainRoad'];
	
	$col_valueList = array();

	$query=pg_query($dbconn,'SELECT "ID","Line" FROM "Lines" WHERE IsMainRoad='. $field_value);
	
	while ($line = pg_fetch_array($query, null, PGSQL_ASSOC)) {
		foreach ($line as $col_value) {		
			array_push($col_valueList,$col_value);
		}
    
	}
	$JSON = "["; 
	for ($i=0;$i<count($col_valueList);$i+=4){
		$JSON=$JSON.'{"ID":"'.$col_valueList[$i].'",'.'"Line":"'.$col_valueList[$i+1].'"},'; 		
	}
	$JSON = substr($JSON,0,-1);
	$JSON = $JSON.']';
	
	echo $JSON;

	
?>
