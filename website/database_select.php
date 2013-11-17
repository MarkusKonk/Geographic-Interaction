<?php
$dbconn = pg_connect("host=localhost dbname=test user=postgres password=ehrenmann87")
    or die('Verbindungsaufbau fehlgeschlagen: ' . pg_last_error());
	
	$query=pg_query($dbconn,'select "Point" from "Feature" where "ID"=31;');

	
	while ($line = pg_fetch_array($query, null, PGSQL_ASSOC)) {
    foreach ($line as $col_value) {
		echo '{ "Name": "test"}';
    }
}
	
?>