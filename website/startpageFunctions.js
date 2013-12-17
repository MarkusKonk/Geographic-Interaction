		//var pointname= prompt("Please Enter Point Name: ","");
		//var pointname=" ";
		//window.open('pointinformation.html','mywindow',"width=400,height=500,left=200,top=100, resizable=no, titlebar=no, toolbar=no, menubar=no");

function Editing()
{
// to add

}
function confirmation(id)
{
var ID=id
var x;
var r=confirm("Are You Sure to Delete the Point ? ");
if (r==true)
  {
  deleting(ID);
  }
else
  {
// to not delete the function

  }
//document.getElementById("demo").innerHTML=x;
}

var lastIDPoint;
var lastIDLine;

function addPoint(coord,name,des){
		
	var latlng = coord.split(',');
		
		var coordinates = coord;
		
		var coords=coordinates.toString();
		lastIDPoint=(lastIDPoint+1);

// 2 options to call the edit function, option 1: using prompt window ,, option 2 : using new form window .... option 1 is disactivated,, option 2 acitivated
//container.html('Coordination of Point Name: <br> ('+ coord+') <br>'+"<a href='#' font-size=30 > Website</a>"+ '&#09' +"<a href='#' font-size=30  onClick='confirmation()'> Delete</a>"+ '&#09' +"<a href='#' onClick=Editing() font-size=30 >  Edit</a>" );

	var container = $('<div />');
	container.html("<br> Point Name: * <br> <input type='text' SIZE=30 name='id' required><br>");
	//marker.bindPopup(container[0]);
    savePoint(name,des,"{Comment1,Comment2}",coordinates);
};

var icon = L.icon({
    iconUrl: 'punkt.png',
    iconSize:     [20, 20], // size of the icon
});

function savePoint(name,des,com,coordinates){
	$.post(
		"database_insert.php?",
		{	
		Coordinates:coordinates
		},
		function(data){lastIDPoint=data;}	
		);		
}

function deleting(id){
$.post("database_delete.php?",
		{ID: id},
		function(){javascript:location.reload()}
		);
}

function callPoints(){
var xmlhttp;
if (window.XMLHttpRequest)
  {// code for IE7+, Firefox, Chrome, Opera, Safari
  xmlhttp=new XMLHttpRequest();
  }
else
  {// code for IE6, IE5
  xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
  }
xmlhttp.onreadystatechange=function()
  {
  if (xmlhttp.readyState==4 && xmlhttp.status==200)
    {
	var obj = jQuery.parseJSON(xmlhttp.responseText);
    for(var i in obj)
	{
		var coordinates = obj[i].Coord.split(',');
	
		var marker = L.marker(new L.LatLng(parseFloat(coordinates[0]), parseFloat(coordinates[1])),options={"id":obj[i].ID});		
		marker
		
			.setIcon(icon)				
			.addTo(map)
		var container = $('<div />');	
	  		container.html(obj[i].Name+'<br>'+' Description: <br> '+obj[i].Description+' <br>' +"<button type='button' onclick='confirmation("+obj[i].ID+")' style='align:left;'>Delete</button>"+ '&#09');
			marker.bindPopup(container[0]);
			lastIDPoint=parseInt(obj[i].ID);
			map.almostOver.addLayer(marker);
		
	}
    }
  }
  //If data needs to be processed, "Post"
	xmlhttp.open("GET","database_select.php",true);
	xmlhttp.send();
}


function addLine(coord){

		//var pointname= prompt("Please Enter Point Name: ","");
		var linename=" ";
		//window.open('pointinformation.html','mywindow',"width=400,height=500,left=200,top=100, resizable=no, titlebar=no, toolbar=no, menubar=no");

		//var container = $('<div />');
		var coordinates = coord;
		//var coords=coordinates.toString();
		lastIDLine=(lastIDLine+1);
	
		// 2 options to call the edit function, option 1: using prompt window ,, option 2 : using new form window .... option 1 is disactivated,, option 2 acitivated
	//container.html('Coordination of Point Name: <br> ('+ coord+') <br>'+"<a href='#' font-size=30 > Website</a>"+ '&#09' +"<a href='#' font-size=30  onClick='confirmation()'> Delete</a>"+ '&#09' +"<a href='#' onClick=Editing() font-size=30 >  Edit</a>" );
	//container.html('Coordination of '+linename +' is: <br> ('+ coord+') <br>'+"<a href='#' font-size=30 > Website</a>"+ '&#09' +"<button type='button' onclick='deleting("+lastID+''+")' style='align:left;'>Delete</button>"+ '&#09' +"<a href='#' onClick=window.open('pointinformation.html','mywindow','width=400,height=500,left=200,top=100') font-size=30 >  Edit</a>" );
	
  // marker.bindPopup(container[0]);
	var lineCoordinates = "(";
	for (var i=0; i<coordinates.length; i=i+1){
		lineCoordinates = lineCoordinates +coordinates[i]+",";
	}
	lineCoordinates = lineCoordinates.substr(0,lineCoordinates.length-1);
	lineCoordinates = lineCoordinates+")";
   saveLine("test","Description","{Comment1,Comment2}",true,lineCoordinates);
			
};

function saveLine(name,des,com,roadtype,coordinates){
	$.post(
		"database_insertLines.php?",
		{	
		Coordinates:coordinates
		},
		function(data){lastIDLine=data;}	
		);
}

var projects = new Array();
function callLines(){
var xmlhttp;
if (window.XMLHttpRequest)
  {// code for IE7+, Firefox, Chrome, Opera, Safari
  xmlhttp=new XMLHttpRequest();
  }
else
  {// code for IE6, IE5
  xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
  }
xmlhttp.onreadystatechange=function()
  {
  if (xmlhttp.readyState==4 && xmlhttp.status==200)
    {
	//document.getElementById("myDiv").innerHTML=xmlhttp.responseText;
	var obj = jQuery.parseJSON(xmlhttp.responseText);
    
	for(var i in obj)
	{
		coordinates = obj[i].Coord.split(',');

		linePointsTemp = new Array();
		for (var j in coordinates){
			tempPoint=coordinates[j].replace("(","");
			tempPoint=tempPoint.replace(")","");
			linePointsTemp.push(tempPoint);
		}
	
		linePoints = new Array();
		for (var k = 0; k<linePointsTemp.length;k=k+2){
			linePoints.push(new L.LatLng(linePointsTemp[k+1], linePointsTemp[k]));
		}
	
	if (obj[i].MainRoad=="t"){
		var polyline = L.polyline(linePoints, {color: 'green'},options={"id":obj[i].ID}).addTo(map);
		var roadType="Planned";
		projects.push(obj[i]);
		
		}
	else{	
		var polyline = L.polyline(linePoints, {color: 'red'},options={"id":obj[i].ID}).addTo(map);
		var roadType="Alternative";
		
		}
		
		var container = $('<div />');	
		container.html('Name: '+obj[i].Name+' Description: <br> '+obj[i].Description+' <br>'+"Roadtype: " + roadType + ' <br>' +"<a href='#' font-size=30 ></a>"+ '&#09' +"<button type='button' onclick='deleteLine("+obj[i].ID+")' style='align:left;'>Delete</button>"+ '&#09' +"<a href='#' onclick='AddComment("+obj[i].ID+")' font-size=30 >  Add Comment </a>");
		//container.html('Name: '+obj[i].Name+' Description: <br> '+obj[i].Description+' <br>'+"Roadtype: " + roadType + ' <br>' +"<a href='#' font-size=30 ></a>"+ '&#09' +"<button type='button' onclick='deleteLine("+obj[i].ID+")' style='align:left;'>Delete</button>");
		//	  		container.html('Coordination of '+obj[i].Name+' is: <br> '+obj[i].Coord+' <br>'+' Description: <br> '+obj[i].Description+' <br>'+"<a href='#' font-size=30 > Website</a>"+ '&#09' +"<button type='button' onclick='confirmation("+obj[i].ID+")' style='align:left;'>Delete</button>"+ '&#09' +"<a href='#' onClick=window.open('editform.html','mywindow','width=400,height=250,left=200,top=100') font-size=30 >  Edit</a>");
		polyline.bindPopup(container[0]);
		lastIDLine=parseInt(obj[i].ID);
		map.almostOver.addLayer(polyline);

		
	}
    }
	}
  
  //If data needs to be processed, "Post"
	xmlhttp.open("GET","database_callLines.php",true);
	xmlhttp.send();
}

function deleteLine(id){
$.post("database_deleteLine.php?",
		{ID: id},
			function(){javascript:location.reload()}
		);
}
function AddComment(id){
document.getElementById('featureid').value = id;
sidebarComments.show();
}

function addPointAttributes(name,des){
	$.post(
		"database_addPointAttributes.php?",
		{	
		Name:name,
		Description:des,		
		ID:lastIDPoint
		},
		function(data){alert(name);javascript:location.reload()}	
		);		
}

function addLineAttributes(name,des,type){
	$.post(
		"database_addLineAttributes.php?",
		{	
		Name:name,
		Description:des,
		Type:type,
		ID:lastIDLine
		},
		function(data){javascript:location.reload()}	
		);		
}

function addUserComments(FID,FType,UComment){

	$.post(
		"SaveUserComment.php?",
		{	
		FID:FID,
		FType:FType,
		UComment:UComment
		}
		);		
		alert("Thank you , We have Received your comments !!") ;
		sidebarComments.hide();
}

function up(){
var latln=marker.getLatLng();
var hoeher=latln.lat+0.001;
marker.setLatLng(new L.LatLng(hoeher,latln.lng));
}
function down(){
var latln=marker.getLatLng();
var runter=latln.lat-0.001;
marker.setLatLng(new L.LatLng(runter,latln.lng));
}
function right(){
var latln=marker.getLatLng();
var rechts=latln.lng+0.001;
marker.setLatLng(new L.LatLng(latln.lat,rechts));
}
function left(){
var latln=marker.getLatLng();
var links=latln.lng-0.001;
marker.setLatLng(new L.LatLng(latln.lat,links));
}
function mapup(){
map.panBy([0, -50]);
}
function mapdown(){
map.panBy([0, +50]);
}
function mapleft(){
map.panBy([-50, 0]);
}
function mapright(){
//map.panTo([51.95,7.6197]);
map.panBy([50, 0]);
}

function showProjects(){
for (var i=projects.length-1;i>projects.length-2;i--){
var coordinate = projects[i].Coord.split(',');
var longitude = coordinate[0].substr(1,coordinate[0].length-1);
var latitude = coordinate[1].substr(0,coordinate[1].length-1);
document.getElementById("button1").onclick=function(){map.setView([latitude,longitude],16);};
document.getElementById("name1").innerHTML=projects[i].Name;
document.getElementById("des1").innerHTML=projects[i].Description;
}
sidebarProjects.show();
}
