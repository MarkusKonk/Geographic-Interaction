<?php
$dbconn = pg_connect("host=giv-geointeraction.uni-muenster.de port=5432 dbname=featuredb user=postgres password=IGIpostgres")
    or die('Verbindungsaufbau fehlgeschlagen: ' . pg_last_error());
	
	$col_valueList = array();
//	$query=pg_query($dbconn,'select "ID","Line","Name","Description" FROM "Feature";');
	$query=pg_query($dbconn,'select "ID","Line","IsMainRoad","Name","Description" FROM "Lines" ORDER BY "ID" ASC;');
	
	while ($line = pg_fetch_array($query, null, PGSQL_ASSOC)) {
    foreach ($line as $col_value) {
		//$col_value=substr($col_value,1,$col_value.length-1);
		array_push($col_valueList,$col_value);
		}
    
}
	$JSON = "["; 
	for ($i=0;$i<count($col_valueList);$i+=5){
		$JSON=$JSON.'{"ID":"'.$col_valueList[$i].'",'.'"Coord":"'.substr($col_valueList[$i+1],1,-1).'","MainRoad":"'.$col_valueList[$i+2].'","Name":"'.$col_valueList[$i+3].'","Description":"'.$col_valueList[$i+4].'"},'; 
	}
	$JSON = substr($JSON,0,-1);
	$JSON = $JSON.']';
	
	echo $JSON;

	
?>
