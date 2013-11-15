
/**
When the mouse enters the sidebar, the map control actions are disabled.
*/
function mouseEnterSidebar(){
map.dragging.disable();
}
function mouseLeaveSidebar(){
map.dragging.enable();
}
/**
Function to open and close the sidebar. First checks, if the sidebar is open or closed,
then if it is open changes the text, hides the elements in the sidebar and removes the border.
Otherwise the objects will be shown and a new border will be created.
*/
function onClickSidebarBorder(){
if($("#sideBar").width()!=parseInt(map.getSize().x/4)){
$("#sideBar").css("width",map.getSize().x/4);
$("#sidebarBorder").text(">>");
$("#sidebarBorder").css("border-right","solid");
$("#sidebarBorder").css("border-right-Width","2px");
$("#sidebarBorder").css("border-right-color","rgb(180,180,180)");
$("#sidebarTitle").show();
$("#sidebarContent").show();
}
else{
$("#sideBar").css("width",map.getSize().x/40);
$("#sidebarBorder").text("<<");
$("#sidebarBorder").css("border-right","none");
$("#sidebarTitle").hide();
$("#sidebarContent").hide();
}
};
/**
Function to test opening and closing the sidebar. Quite the same code like the paragraph before.
If one click on the map, the sidebar will be closed.
*/
function onClickMap(e){
if(e.containerPoint.x<(map.getSize().x-(sidebar.getContainer().offsetWidth))){
$("#sideBar").css("width",map.getSize().x/40);
$("#sidebarBorder").text("<<");
$("#sidebarBorder").css("border-right","none");
$("#sidebarTitle").text("");
$("#sidebarContent").hide();
}
};


function addPoint(){
	function onMapClick(e) {
		var marker = L.marker();
		marker.bindPopup("Some kind of information")
		marker
				.setLatLng(e.latlng,{draggable:'true'})
				.setIcon(icon)				
				.addTo(map)
		marker.dragging.enable();
			save();
			
			
	map.off('click', onMapClick);				
	}
	map.on('click', onMapClick);	
	
};

var icon = L.icon({
    iconUrl: 'punkt.png',
    iconSize:     [20, 20], // size of the icon
});


function save(){
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
    document.getElementById("myDiv").innerHTML=xmlhttp.responseText;
    }
  }
	xmlhttp.open("GET","http://localhost/Geographic-Interaction/website/database.php",true);
	xmlhttp.send();
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
map.panBy([0, 50]);
}
function mapdown(){
map.panBy([0, -5]);
}
function mapleft(){
map.panBy([-5, 0]);
}
function mapright(){
//map.panTo([51.95,7.6197]);
map.panBy([5, 0]);
}