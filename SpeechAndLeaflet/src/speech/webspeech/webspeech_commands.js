/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////Voice Recognition Part//////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        var recognition = new webkitSpeechRecognition();
        var final_transcript = '';
        var interim_transcript = '';
        var language = 'en-GB'; // TODO: fetch language as option value from drop down box
         // en-GB

        recognition.continuous = true; // keep processing input until stopped
        recognition.interimResults = true; // show interim results
        recognition.lang = language; // specify the language

        recognition.onresult = function (event) {
            // Assemble the transcript from the array of results
            for (var i = event.resultIndex; i < event.results.length; ++i) {
                if (event.results[i].isFinal) {
                    final_transcript = event.results[i][0].transcript;
                } else {
                    interim_transcript = event.results[i][0].transcript;
                }
            }

            console.log("interim: " + interim_transcript);
            console.log("final: " + final_transcript);

            // update the web page
            if (final_transcript.length > 0) {
                $('#transcript').html(final_transcript);
            }

            if (interim_transcript.length > 0) {
                $('#interim').html(interim_transcript);
            }

            // handling commands

	//////////////////////////////////////////////////////////
	/////Start using navigation controls//////////////////////
	//////////////////////////////////////////////////////////
            if (final_transcript.indexOf("zoom in") >= 0) {
                $("#command_animation").html("zoom in").css({
                    'fontSize': '12pt'
                }).animate({
                        fontSize: '13em'
                    }, 1100,
                    function () {
                        $("#command_animation").html("");
                    });
                map.zoomIn(1);
                console.log("Zoomed in");
                final_transcript = '';
            }

            if (final_transcript.indexOf("zoom out") >= 0) {
                $("#command_animation").html("zoom out").css('fontSize', '13em').animate({
                        fontSize: '12pt'
                    }, 1100,
                    function () {
                        $("#command_animation").html("");
                    });
                map.zoomOut(1);
                console.log("Zoomed out");
                final_transcript = '';
            }

            if ((final_transcript.indexOf("left") >= 0) 
				||(final_transcript.indexOf("net") >= 0)
				||(final_transcript.indexOf("next") >= 0))				{
				centerPoint = map.getCenter();
				var m = (map.getBounds().getEast()- map.getBounds().getWest())/4;
                centerPoint.lng -= Math.abs(m);
                $("#command_animation").html("left ").css('fontSize', '13em').animate({
                        'left': '-500px'
                    }, 1100,
                    function () {
                        $("#command_animation").html("").css('left', '45%');
                    });
                map.panTo(centerPoint);
                console.log("Panned Left by " + Math.abs(m));
                final_transcript = '';
            }

            if  ((final_transcript.indexOf("right") >= 0) 
				|| (final_transcript.indexOf("white") >= 0)
				|| (final_transcript.indexOf("fight") >= 0)){
				centerPoint = map.getCenter();
				var m = (map.getBounds().getEast()- map.getBounds().getWest())/4;
                centerPoint.lng += Math.abs(m);
                $("#command_animation").html("right").css({
                    'fontSize': '13em'
                }).animate({
                        'left': '100%'
                    }, 1100,
                    function () {
                        $("#command_animation").html("").css('left', '45%');
                    });
                map.panTo(centerPoint);
                console.log("Panned right by " + Math.abs(m));
                final_transcript = '';
            }

            if ((final_transcript.indexOf("up") >= 0) 
				|| (final_transcript.indexOf("apple") >= 0)){
                centerPoint = map.getCenter();
				var m = (map.getBounds().getNorth() - map.getBounds().getSouth())/4;
                centerPoint.lat += m;
                $("#command_animation").html("up").css('fontSize', '13em').animate({
                        'top': '-250px'
                    }, 1100,
                    function () {
                        $("#command_animation").html("").css('top', '50%');
                    });
                map.panTo(centerPoint);
                console.log("Panned up by " + Math.abs(m));
                final_transcript = '';
            }

            if (final_transcript.indexOf("down") >= 0) {
				centerPoint = map.getCenter();
				var m = (map.getBounds().getNorth() - map.getBounds().getSouth())/4;
                centerPoint.lat -= m;
                $("#command_animation").html("down").css('fontSize', '13em').animate({
                        'top': '500px'
                    }, 1100,
                    function () {
                        $("#command_animation").html("").css('top', '50%');
                    });
                map.panTo(centerPoint);
                console.log("Panned down by "+ Math.abs(m));
                final_transcript = '';
            }
			
	//////////////////////////////////////////////////////////
	/////Start using leaflets integrated drawing tool/////////
	//////////////////////////////////////////////////////////
			
			
            if (final_transcript.indexOf("place marker") >= 0) {
                L.marker(map.getCenter()).addTo(map);
                console.log("place marker");
                final_transcript = '';
            }

            if (final_transcript.indexOf("place large circle") >= 0) {
                L.circle(map.getCenter(), 400).addTo(map);
                console.log("place large circle");
                final_transcript = '';
            }

            if (final_transcript.indexOf("place medium circle") >= 0) {
                L.circle(map.getCenter(), 200).addTo(map);
                console.log("place medium circle");
                final_transcript = '';
            }

            if (final_transcript.indexOf("place small circle") >= 0) {
                L.circle(map.getCenter(), 100).addTo(map);
                console.log("place small circle");
                final_transcript = '';
            }

            if (final_transcript.indexOf("place large rectangle") >= 0) {
                L.rectangle([[map.getCenter().lat - 0.01, map.getCenter().lng - 0.01],[map.getCenter().lat + 0.01, map.getCenter().lng + 0.01]]).addTo(map);
                console.log("place large rectangle");
                final_transcript = '';
            }


            if (final_transcript.indexOf("place medium rectangle") >= 0) {
                L.rectangle([[map.getCenter().lat - 0.005, map.getCenter().lng - 0.005],[map.getCenter().lat + 0.005, map.getCenter().lng + 0.005]]).addTo(map);
                console.log("place medium rectangle");
                final_transcript = '';
            }


            if (final_transcript.indexOf("place small rectangle") >= 0) {
                L.rectangle([[map.getCenter().lat - 0.0025, map.getCenter().lng - 0.0025],[map.getCenter().lat + 0.0025, map.getCenter().lng + 0.0025]]).addTo(map);
                console.log("place small rectangle");
                final_transcript = '';
            }


	//////////////////////////////////////////////////////////
	/////Start using leaflet plugins//////////////////////////
	//////////////////////////////////////////////////////////
			
			
			
           if (final_transcript.indexOf("enable location function") >= 0) {
                locater.locate();
                console.log("enable location function");
                final_transcript = '';
            }
            
            
            if (final_transcript.indexOf("disable location function") >= 0) {
                locater.stopLocate();
                console.log("disable location function");
                final_transcript = '';
            }

			
            if (final_transcript.indexOf("enable additional information") >= 0) {
                map.revealOSMControl.activate();
                console.log("enable additional information");
                final_transcript = '';
            }
            
            
            if (final_transcript.indexOf("disable additional information") >= 0) {
                map.revealOSMControl.deactivate();
                console.log("disable additional information");
                final_transcript = '';
            }
            
            
            if (final_transcript.indexOf("go back to home") >= 0) {
                map.setView(new L.LatLng(51.95442, 7.62709), 13);
                console.log("go back to home");
                final_transcript = '';
            }
            
            
            if (final_transcript.indexOf("enable measurement") >= 0) {
                map.measureControl.toggle();
                console.log("enable measurement");
                final_transcript = '';
            }
            
            
            if (final_transcript.indexOf("disable measurement") >= 0) {
                map.measureControl.toggle();
                console.log("disable measurement");
                final_transcript = '';
            }
            
			if (final_transcript.indexOf("enable minimap") >= 0) {
                miniMap._restore();
                console.log("enable mini map");
                final_transcript = '';
            }
            
            if (final_transcript.indexOf("disable minimap") >= 0) {
                miniMap._minimize();
                console.log("disable mini map");
                final_transcript = '';
            }
            
			
	//////////////////////////////////////////////////////////
	/////Start using drawing plugin tool//////////////////////
	//////////////////////////////////////////////////////////
			
	if (final_transcript.indexOf("place a point project") >= 0) {
                new L.Draw.Marker(map).enable();
				//or this way: L.Draw.Marker(map, drawControl.options.marker).enable();    https://github.com/Leaflet/Leaflet.draw/issues/179#issuecomment-26500042
                console.log("place a point project");
                final_transcript = '';
            }
            
            
            if (final_transcript.indexOf("place a street project") >= 0) {
                new L.Draw.Polyline(map).enable();
                console.log("place a street project");
                final_transcript = '';
            }
            
            
            if (final_transcript.indexOf("enable circle drawing") >= 0) {
                new L.Draw.Circle(map).enable();
                console.log("enable circle drawing");
                final_transcript = '';
            }
            
            
            if (final_transcript.indexOf("enable polygon drawing") >= 0) {
                new L.Draw.Polygon(map).enable();
                console.log("enable polygon drawing");
                final_transcript = '';
            }
            
            
            if (final_transcript.indexOf("enable rectangle drawing") >= 0) {
                new L.Draw.Rectangle(map).enable();
                console.log("enable rectangle drawing");
                final_transcript = '';
            }
        

	//////////////////////////////////////////////////////////
	/////Start using leaflet layer control////////////////////
	//////////////////////////////////////////////////////////
	
	
			if (final_transcript.indexOf("enable temperature map") >= 0) {
                overlayLayers.OpenWeatherMap_Precipitation.onAdd(map);
                console.log("enable temperature map");
                final_transcript = '';
            }
        	
		
		if (final_transcript.indexOf("disable temperature map") >= 0) {
                overlayLayers.OpenWeatherMap_Precipitation.onRemove(map);
                console.log("disable temperature map");
                final_transcript = '';
            }
        }
	
	
	
	
	
