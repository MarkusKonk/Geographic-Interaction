<html> 
    <body margin= "0"> 

<font color="#0080C0"><label>Previous comments</label></font>
<table width="100%" border="2" cellpadding="2" cellspacing="2" style="background-color: #ffffff;">
<tr valign="top">
<td width="100" style="border-width : 0px; background-color: #0080C0;"><p style=" text-align: left; text-indent: 0px; padding: 0px 0px 0px 0px; margin: 0px 0px 0px 0px;"><span style=" font-size: 10pt; font-family: 'Arial', 'Helvetica', sans-serif; font-style: normal; font-weight: bold; color: #ffffff; background-color: transparent; text-decoration: none;">Citizien Name</span></p>
</td>
<td style="border-width : 0px; background-color: #0080C0;"><p style=" text-align: left; text-indent: 0px; padding: 0px 0px 0px 0px; margin: 0px 0px 0px 0px;"><span style=" font-size: 10pt; font-family: 'Arial', 'Helvetica', sans-serif; font-style: normal; font-weight: bold; color: #ffffff; background-color: transparent; text-decoration: none;">Comment</span></p>
</td>
</tr>
      <?php 
	  session_start();
	  // open the connection with the database 
	        $db = pg_connect("host=giv-geointeraction.uni-muenster.de port=5432 dbname=featuredb user=postgres password=IGIpostgres")
			or die('Verbindungsaufbau fehlgeschlagen: ' . pg_last_error());
			
		  if('POST' == $_SERVER['REQUEST_METHOD']) {
			//save code will be in here 
			 $featureid = 2; 
			    $feature_Type = "Point"; 
			}else 
			{
			$featureid = $_GET['fid'];; 
			//$var_value = $_SESSION['fid'];
		    //$feature_Type = $_GET['ftype'];; 
						//$featureid = $_POST['FID'];
 //$feature_Type = "Point"; 
  //$featureid = 4; 

		
			//echo ($var_value);
		

			}
			
		 
  
       //$sql="select feature_id,usercomment,featuretype,comment_id,feature_type FROM userscomments WHERE feature_id=$featureid and featuretype ='$feature_Type' ";
		$sql="select feature_id,usercomment,featuretype,comment_id,feature_type FROM userscomments WHERE feature_id=$featureid";
		
		$result =pg_query($db,$sql);
     
        if (!$result) { 
            echo "Problem with query " . $query . "<br/>"; 
            echo pg_last_error(); 
            exit(); 
        } 

        while($myrow = pg_fetch_assoc($result)) { 
            printf ("<tr><td>%s</td><td>%s</td></tr>", htmlspecialchars($myrow['featuretype']),htmlspecialchars($myrow['usercomment']));
        } 
        ?> 
        </table> 
    </body> 
</html>