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
                    
                    if(document.getElementById(document.activeElement.id).value == "" || document.getElementById(document.activeElement.id).value == null)
                    	document.getElementById(document.activeElement.id).value = final_transcript;
                    else
                    	document.getElementById(document.activeElement.id).value += final_transcript;
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

            if ((final_transcript.indexOf("left") >= 0) ||(final_transcript.indexOf("list") >= 0)
				||(final_transcript.indexOf("net") >= 0) ||(final_transcript.indexOf("love") >= 0)
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

            if ((final_transcript.indexOf("up") >= 0) || (final_transcript.indexOf("app") >= 0)
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
			
			recognition.onend = function() {
				recognizing = false;
				// automatically restart the speech recognition in case it stops
				recognition.start();
				
				/*
				//used for manual restart via click on icon
				$("#speech").click(function() {
					recognition.start();
				});
				*/
			}
			
	//////////////////////////////////////////////////////////
	/////Start using leaflet plugins//////////////////////////
	//////////////////////////////////////////////////////////
			
			
			
           if (final_transcript.indexOf("where am i") >= 0) {
                locater.locate();
                console.log("where am i");
                final_transcript = '';
            }
            
            
            if (final_transcript.indexOf("don't show where i am") >= 0) {
                locater.stopLocate();
                console.log("don't show where i am");
                final_transcript = '';
            }

           if (final_transcript.indexOf("go home") >= 0) {
                map.setView(new L.LatLng(51.95442, 7.62709), 13);
                console.log("go home");
                final_transcript = '';
            }
            
            
            if (final_transcript.indexOf("start measurement") >= 0) {
                map.measureControl.toggle();
                console.log("start measurement");
                final_transcript = '';
            }
            
            
            if (final_transcript.indexOf("stop measurement") >= 0) {
                map.measureControl.toggle();
                console.log("stop measurement");
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
	/////Draw lines or points/////////////////////////////////
	//////////////////////////////////////////////////////////
	
			if (final_transcript.indexOf("point") >= 0) {
                new L.Draw.Marker(map).enable();
                console.log("point");
                final_transcript = '';
            }
			
			if ((final_transcript.indexOf("road") >= 0 || final_transcript.indexOf("world") >= 0)
				&& (sidebarOpen != 'line')){
                new L.Draw.Polyline(map).enable();
                console.log("road");
				console.log(sidebarOpen);
                final_transcript = '';
            }

			if (final_transcript.indexOf("main road") >= 0) {
                document.getElementById('alternative').checked = false;
				document.getElementById('main').checked = true;
                console.log("main checked");
                final_transcript = '';
            }
			
			if (final_transcript.indexOf("suggested road") >= 0) {
                document.getElementById('alternative').checked = true;
				document.getElementById('main').checked = false;
                console.log("alternative checked");
                final_transcript = '';
            }
			
			if (final_transcript.indexOf("name") >= 0) {
				if (sidebarOpen == 'line'){
					document.getElementById('linename').focus();
				}else if (sidebarOpen == 'point'){
					document.getElementById('pointname').focus();
				}
				else if (sidebarOpen == 'comment'){
					document.getElementById('featuretype').focus();
				}
                console.log("name");
                final_transcript = '';
            }
			
			if (final_transcript.indexOf("description") >= 0) {
				if (sidebarOpen == 'line'){
					document.getElementById('linedes').focus();
				}else if (sidebarOpen == 'point'){
					document.getElementById('pointdes').focus();
				}
                console.log("description");
                final_transcript = '';
            }
			
			if ((final_transcript.indexOf("user comment") >= 0 || final_transcript.indexOf("user comments") >= 0 ||
				final_transcript.indexOf("user comet") >= 0 || final_transcript.indexOf("user command") >= 0 ||
				final_transcript.indexOf("user common") >= 0 || final_transcript.indexOf("user command") >= 0)
				&& sidebarOpen == 'comment'){
					document.getElementById('usercomment').focus();
				}
			
			if (final_transcript.indexOf("submit" || final_transcript.indexOf("summit") >= 0) {
				if (sidebarOpen == 'line'){
					al();
				}else if (sidebarOpen == 'point'){
					ap();
				}else if (sidebarOpen == 'comment'){
					sendUserComments();
				}
                console.log("submit");
                final_transcript = '';
            }
			
			if (final_transcript.indexOf("clear") >= 0 || final_transcript.indexOf("clip") >= 0) {
				clearForm();
                console.log("clear");
                final_transcript = '';
            }
			
			
			
            
			if ((final_transcript.indexOf("close") >= 0)
				||(final_transcript.indexOf("clothes") >= 0)){
				closeSidebar();
				console.log("close");
				final_transcript = '';
			}
			
			if (final_transcript.indexOf("openstreetmap") >= 0)	{
				baselayerOSM();
				console.log("openstreetmap");
				final_transcript = '';
			}
			
			if (final_transcript.indexOf("aerial view") >= 0)	{
				baselayerAerialView();
				console.log("aerial view");
				final_transcript = '';
			}
						
			if ((final_transcript.indexOf("projects") >= 0)
				||(final_transcript.indexOf("project") >= 0)){
				showProjects();
				console.log("projects");
				final_transcript = '';
			}
			
			/* unfinished functions: object ID of the selected object could not be accessed
			if (final_transcript.indexOf("delete") >= 0)	{
				deleting(selectedObjectId);
				console.log("delete");
				console.log(selectedObjectId);
				final_transcript = '';
			}
			
			if (final_transcript.indexOf("comments") >= 0)	{
				AddComment(selectedObjectId);
				console.log("comments");
				final_transcript = '';
			}
			
			**/
	}
	

	
			
