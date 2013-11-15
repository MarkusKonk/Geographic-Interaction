<?php
$dbconn = pg_connect("host=localhost dbname=test user=postgres password=***")
    or die('Verbindungsaufbau fehlgeschlagen: ' . pg_last_error());
	
	$query=pg_query($dbconn,"Insert into \"Feature\" values (30,'test','bla','{eins,zwei}','(51.1234,7.12345)');");

	
	while ($line = pg_fetch_array($query, null, PGSQL_ASSOC)) {
    foreach ($line as $col_value) {
		echo $col_value;
    }
}
	
?>