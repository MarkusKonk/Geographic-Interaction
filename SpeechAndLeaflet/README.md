## IWGI - Interaction with Geographic Information ##
### Speech And Leaflet ###
----------
![Speech][1]        ![enter image description here][2]   ![enter image description here][3]

To test it, you need to run the .html site within a web server like apache or tomcat. For an easy installation you could use packages like XAMPP. 

 1. Clone the repository 
 2. Copy all files to the htcdocs of your installed webserver. 
 3. Browse the site with Google Chrome. Other Browser are not supported, because the application uses heavily Googles WebSpeech API, which only works with Google Chrome. We're currently investigating other options using PocketSphinx or other libs.
 4. To start interacting with the map by speech, first click the green button, allow microphone access from browser. There you have the following options:

 - Basic Map Navigation
     - **"Zoom out"**, 
     - **"Zoom in"**, 
     - **"Up"**, 
     - **"Down"**,
     - **"Left"**, 
     - **"Right"** 
 - Place fixed-scale geometry objects on the map
     - **"Place Marker"**, 
     - **"Place Large/medium/small circle"**,
     - **"Place Large/medium/small rectangle"**
 - Control the drawing control plugin (enable drawing functionalities)
     - **"Place a point project"**,
     - **"Place a street project"**,
     - **"Start circle drawing"**, 
     - **"Start polygon drawing"**, 
     - **"Start rectangle drawing"** 
 - Locate yourself using the Geolocate Tool
     - **"Enable location function"**,
     - **"Disable location function"**  
 - To control the Reveal OSM Plugin, which provides additional information via OSM features say:
     - **"Enable additional information"**,
     - **"Disable additional information"** 
 - To switch back to the home extend (Münster): 
     - **"Go back to home"** 
 - To Enable or Disable the measurement tool, say
     - **"Enable measurement"**,
     - **"Disable measurement"**
 - To control the minimap.
     - **"Disable minimap"**,
     - **"Enable minimap"**
 - To enable a temperature layer. More layers will follow
     - **"Enable temperature map"**,
     - **"Disable temperature map"**


----------
To-DOs

 - Issue list: https://github.com/ChristopherStephan/IWGI-Speech/issues
 - Mainly: 
     - Sphinx Integration, 
     - Timeout for WebSpeech API
     - Gesture Integration
     - Backend Stuff


  [1]: http://megaicons.net/static/img/icons_sizes/8/60/96/basic-speech-bubble-icon.png
  [2]: http://www.ipart.nsw.gov.au/files/1/209/plus-sign.jpg
  [3]: http://leafletjs.com/docs/images/logo.png
