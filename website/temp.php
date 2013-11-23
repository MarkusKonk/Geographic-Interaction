<?php
$dbconn = pg_connect("host=giv-geointeraction.uni-muenster.de port=5432 dbname=featuredb user=postgres password=IGIpostgres")
    or die('Verbindungsaufbau fehlgeschlagen: ' . pg_last_error());

	$query=pg_query($dbconn,'ALTER TABLE "Feature" ADD Number INTEGER;');

?>