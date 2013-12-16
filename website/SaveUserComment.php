<html> 
    <body> 
        <?php 
		echo "<script type='text/javascript'> alert('{$text}') </script>"; 
        $db = pg_connect("host=giv-geointeraction.uni-muenster.de port=5432 dbname=featuredb user=postgres password=IGIpostgres")
			or die('Verbindungsaufbau fehlgeschlagen: ' . pg_last_error());

	$featureid = $_POST['FID'];
	$usercomment = $_POST['UComment'];
	$featuretype = $_POST['FType']; 
	

        $query = "INSERT INTO userscomments(feature_id, usercomment,featuretype) VALUES('" . $featureid . "', '" . $usercomment . "', '" . $featuretype . "')";
        $result = pg_query($query); 
        if (!$result) { 
            $errormessage = pg_last_error(); 
            echo "Error with query: " . $errormessage; 
            exit(); 
        } 
        echo ("Thanks you , we have received your comment."); 
        pg_close(); 
        ?> 
    </body> 
</html> 