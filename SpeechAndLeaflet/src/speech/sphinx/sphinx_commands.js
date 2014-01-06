
		
		
		
		
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////Voice Recognition Part//////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		
			
		/*Adding Words:
		var recognizer = new Module.Recognizer();
		var words = new Module.VectorWords();
		words.push_back(["LEFT", "L EH F T"]);

		if (recognizer.addWords(words) != Module.ReturnType.SUCCESS)
			// Probably bad format used for pronunciation
			alert("Error while adding words");
			words.delete()
		*/
		
		
		
		
         // These will be initialized later
		var recognizer, recorder, callbackManager, audioContext, outputContainer;
		// Only when both recorder and recognizer do we have a ready application
		var recorderReady = recognizerReady = false;

		// A convenience function to post a message to the recognizer and associate
		// a callback to its response
		function postRecognizerJob(message, callback) {
		var msg = message || {};
			if (callbackManager) msg.callbackId = callbackManager.add(callback);
			if (recognizer) recognizer.postMessage(msg);
		};

      // This function initializes an instance of the recorder
      // it posts a message right away and calls onReady when it
      // is ready so that onmessage can be properly set
      function spawnWorker(workerURL, onReady) {
          recognizer = new Worker(workerURL);
          recognizer.onmessage = function(event) {
            onReady(recognizer);
          };
          recognizer.postMessage('');
      };

      // To display the hypothesis sent by the recognizer
      function updateHyp(hyp) {
	    //This displays the hyp
        if (outputContainer) outputContainer.innerHTML = hyp;
		
		// This checks the hyp and executes a function according the command
						
			
      };
	  

      // This updates the UI when the app might get ready
      // Only when both recorder and recognizer are ready do we enable the buttons
      function updateUI() {
        if (recorderReady && recognizerReady) startBtn.disabled = stopBtn.disabled = false;
		
		

      };

      // This is just a logging window where we display the status
      function updateStatus(newStatus) {
        document.getElementById('current-status').innerHTML += "<br/>" + newStatus;
      };

      // A not-so-great recording indicator
      function displayRecording(display) {
        if (display) document.getElementById('recording-indicator').innerHTML = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
        else document.getElementById('recording-indicator').innerHTML = "";
      };

      // Callback function once the user authorises access to the microphone
      // in it, we instanciate the recorder
      function startUserMedia(stream) {
        var input = audioContext.createMediaStreamSource(stream);
        var audioRecorderConfig = {errorCallback: function(x) {updateStatus("Error from recorder: " + x);}};
       // Can be configured:
	   // https://github.com/syl22-00/pocketsphinx.js/tree/master/doc/AudioRecorder
		recorder = new AudioRecorder(input, audioRecorderConfig);
        // If a recognizer is ready, we pass it to the recorder
        if (recognizer) recorder.consumers = [recognizer];
        recorderReady = true;
        updateUI();
        updateStatus("Audio recorder ready");
      };

      // This starts recording. We first need to get the id of the grammar to use
      var startRecording = function() {
        var id = document.getElementById('grammars').value;
        if (recorder && recorder.start(id)) displayRecording(true);
      };

      // Stops recording
      var stopRecording = function() {
        recorder && recorder.stop();
        displayRecording(false);
      };

      // Called once the recognizer is ready
      // We then add the grammars to the input select tag and update the UI
      var recognizerReady = function() {
           updateGrammars();
           recognizerReady = true;
           updateUI();
           updateStatus("Recognizer ready");
      };

      // We get the grammars defined below and fill in the input select tag
      var updateGrammars = function() {
        var selectTag = document.getElementById('grammars');
        for (var i = 0 ; i < grammarIds.length ; i++) {
            var newElt = document.createElement('option');
            newElt.value=grammarIds[i].id;
            newElt.innerHTML = grammarIds[i].title;
            selectTag.appendChild(newElt);
        }                          
      };

      // This adds a grammar from the grammars array
      // We add them one by one and call it again as
      // a callback.
      // Once we are done adding all grammars, we can call
      // recognizerReady()
      var feedGrammar = function(g, index, id) {
        if (id && (grammarIds.length > 0)) grammarIds[0].id = id.id;
        if (index < g.length) {
          grammarIds.unshift({title: g[index].title})
	  postRecognizerJob({command: 'addGrammar', data: g[index].g},
                             function(id) {feedGrammar(grammars, index + 1, {id:id});});
        } else {
          recognizerReady();
        }
      };

      // This adds words to the recognizer. When it calls back, we add grammars
      var feedWords = function(words) {
           postRecognizerJob({command: 'addWords', data: words},
                        function() {feedGrammar(grammars, 0);});
      };

      // This initializes the recognizer. When it calls back, we add words
      var initRecognizer = function() {
          // You can pass parameters to the recognizer, such as : {command: 'initialize', data: [["-hmm", "my_model"], ["-fwdflat", "no"]]}
          postRecognizerJob({command: 'initialize'},
                            function() {
                                        if (recorder) recorder.consumers = [recognizer];
                                        feedWords(wordList);});
      };

      // When the page is loaded, we spawn a new recognizer worker and call getUserMedia to
      // request access to the microphone
      window.onload = function() {
        outputContainer = document.getElementById("output");
        updateStatus("Initializing web audio and speech recognizer, waiting for approval to access the microphone");
        callbackManager = new CallbackManager();
        spawnWorker("src/speech/sphinx/recognizer.js", function(worker) {
            // This is the onmessage function, once the worker is fully loaded
            worker.onmessage = function(e) {
                // This is the case when we have a callback id to be called
                if (e.data.hasOwnProperty('id')) {
                  var clb = callbackManager.get(e.data['id']);
                  var data = {};
                  if ( e.data.hasOwnProperty('data')) data = e.data.data;
                  if(clb) clb(data);
                }
                // This is a case when the recognizer has a new hypothesis
                if (e.data.hasOwnProperty('hyp')) {
                  var newHyp = e.data.hyp;
                  if (e.data.hasOwnProperty('final') &&  e.data.final) newHyp = "Final: " + newHyp;
                  
				 
				  updateHyp(newHyp);
				  checkHyp(newHyp);
				  newHyp="";
				  newHyp="";
				  newHyp="";
				  newHyp="";
                }
				
				
                // This is the case when we have an error
                if (e.data.hasOwnProperty('status') && (e.data.status == "error")) {
                  updateStatus("Error in " + e.data.command + " with code " + e.data.code);
                }
            };
            // Once the worker is fully loaded, we can call the initialize function
            initRecognizer();
        });

        // The following is to initialize Web Audio
        try {
          window.AudioContext = window.AudioContext || window.webkitAudioContext;
          navigator.getUserMedia = navigator.getUserMedia || navigator.webkitGetUserMedia || navigator.mozGetUserMedia;
          window.URL = window.URL || window.webkitURL;
          audioContext = new AudioContext();
        } catch (e) {
          updateStatus("Error initializing Web Audio browser");
        }
        if (navigator.getUserMedia) navigator.getUserMedia({audio: true}, startUserMedia, function(e) {
                                        updateStatus("No live audio input in this browser");
                                    });
        else updateStatus("No web audio support in this browser");

      // Wiring JavaScript to the UI
      var startBtn = document.getElementById('startBtn');
      var stopBtn = document.getElementById('stopBtn');
      startBtn.disabled = true;
      stopBtn.disabled = true;
      startBtn.onclick = startRecording;
      stopBtn.onclick = stopRecording;
      };

	  

	  
	//////////////////////////////////////////////////////////
	/////Grammar and vocabulary //////////////////////////////
	//////////////////////////////////////////////////////////
	  
	  
	  
	  
	  
	  
       // This is the list of words that need to be added to the recognizer
       // This follows the CMU dictionary format
      var wordList = [["LEFT", "L EH F T"], ["RIGHT", "R AY T"], ["UP", "AH P"],["DOWN", "D AW N"],["ZOOM", "Z UW M"],["IN", "IH N"],["OUT", "AW T"],["LOCATE", "L OW K EY T"],["STOP", "S T AA P"],["SHOW", "SH OW"],["INFORMATION", "IH N F AO R M EY SH AH N"],["INFORMATION(2)", "IH N F ER M EY SH AH N"],["HIDE", "HH AY D"],["START", "S T AA R T"],
					  ["MEASUREMENT", "M EH ZH ER M AH N T"],["POINT", "P OY N T"],["CIRCLE", "S ER K AH L"],["RECTANGLE", "R EH K T AE NG G AH L"],["SMALL", "S M AO L"], ["MEDIUM", "M IY D IY AH M"], ["LARGE", "L AA R JH"]];
     
	 //different grammars
      var grammarPAN = {numStates: 1, start: 0, end: 0, transitions: [{from: 0, to: 0, word: "UP"}, {from: 0, to: 0, word: "DOWN"}, {from: 0, to: 0, word: "LEFT"}, {from: 0, to: 0, word: "RIGHT"}]};     
      
	  var grammarZOOM = {numStates: 7, start: 0, end: 6, transitions: [{from: 0, to: 1, word: "ZOOM"}, {from: 1, to: 2, word: "IN"},  {from: 1, to: 2, word: "OUT"}]};
	  var grammarTOOLS = {numStates: 7, start: 0, end: 2, transitions: [{from: 0, to: 1, word: "LOCATE"},{from: 0, to: 1, word: "SHOW"}, {from: 0, to: 1, word: "MEASUREMENT"}, {from: 0, to: 1, word: "HIDE"}, {from: 1, to: 2, word: "STOP"}, {from: 1, to: 2, word: "START"}, {from: 1, to: 2, word: "INFORMATION"}]};
	  var grammarDRAW = {numStates: 6, start: 0, end: 2, transitions: [{from: 0, to: 0, word: "POINT"},{from: 0, to: 1, word: "LARGE"}, {from: 0, to: 1, word: "MEDIUM"}, {from: 0, to: 1, word: "SMALL"}, {from: 1, to: 2, word: "CIRCLE"}, {from: 1, to: 2, word: "RECTANGLE"}]};
	  var grammars = [ {title: "ZOOM", g: grammarZOOM}, {title: "PAN", g: grammarPAN},{title: "TOOLS", g: grammarTOOLS},{title: "DRAW", g: grammarDRAW}];
      var grammarIds = [];
	  
	  
	//////////////////////////////////////////////////////////
	/////Check the Recognition hyp ///////////////////////////
	//////////////////////////////////////////////////////////  


    function checkHyp(hyp) {
	
	if(hyp=="LEFT"){
		 				 
	    stopRecording();
		startRecording();
		pan_left();			 
				}
	  
	if(hyp=="RIGHT"){
		 				 
	    stopRecording();
		startRecording();
		//document.getElementById("output").innerHTML = "";
		pan_right();
				}
	  
	if(hyp=="UP"){
		 				 
	    stopRecording();
		startRecording();
		pan_up();		 
				}
	  
	if(hyp=="DOWN"){
		 				 
	    stopRecording();
		startRecording();
		pan_down();
		stopRecording();
		startRecording();
				}  
	  
	if(hyp=="ZOOM IN"){
		 				 
	    stopRecording();
		startRecording();
		map.zoomIn();	
				} 
	   
	if(hyp=="ZOOM OUT"){
		 				 
	    stopRecording();
		startRecording();
		map.zoomOut();	
				} 
	if(hyp=="LOCATE"){
		 				 
	    stopRecording();
		startRecording();
		locater.locate();	
				} 
				
	if(hyp=="LOCATE STOP"){
		 				 
	    stopRecording();
		startRecording();
		locater.stopLocate();	
				} 
				
	if(hyp=="SHOW INFORMATION"){
		 				 
	    stopRecording();
		startRecording();
		map.revealOSMControl.activate();	
				} 
	
	if(hyp=="HIDE INFORMATION"){
		 				 
	    stopRecording();
		startRecording();
		map.revealOSMControl.deactivate();	
				} 
				
	if(hyp=="MEASUREMENT START"){
		 				 
	    stopRecording();
		startRecording();
		map.measureControl._enable();	
				} 
	
	if(hyp=="MEASUREMENT STOP"){
		 				 
	    stopRecording();
		startRecording();
		map.measureControl._disable();	
				} 
				
	if(hyp=="POINT"){
		 				 
	    stopRecording();
		startRecording();
		L.marker(map.getCenter()).addTo(map);	
				} 
	if(hyp=="MEASUREMENT STOP"){
		 				 
	    stopRecording();
		startRecording();
		map.measureControl._disable();	
				} 
	if(hyp=="LARGE CIRCLE"){
		 				 
	    stopRecording();
		startRecording();
		L.circle(map.getCenter(), 400).addTo(map);	
				} 
	if(hyp=="MEDIUM CIRCLE"){
		 				 
	    stopRecording();
		startRecording();
		L.circle(map.getCenter(), 200).addTo(map);	
				} 
				
	if(hyp=="SMALL CIRCLE"){
		 				 
	    stopRecording();
		startRecording();
		L.circle(map.getCenter(), 100).addTo(map);	
				} 
				
				
	if(hyp=="LARGE RECTANGLE"){
		 				 
	    stopRecording();
		startRecording();
		L.rectangle([[map.getCenter().lat - 0.01, map.getCenter().lng - 0.01],[map.getCenter().lat + 0.01, map.getCenter().lng + 0.01]]).addTo(map);
				} 
	if(hyp=="MEDIUM RECTANGLE"){
		 				 
	    stopRecording();
		startRecording();
		L.rectangle([[map.getCenter().lat - 0.005, map.getCenter().lng - 0.005],[map.getCenter().lat + 0.005, map.getCenter().lng + 0.005]]).addTo(map);	
				} 
				
	if(hyp=="SMALL RECTANGLE"){
		 				 
	    stopRecording();
		startRecording();
		L.rectangle([[map.getCenter().lat - 0.0025, map.getCenter().lng - 0.0025],[map.getCenter().lat + 0.0025, map.getCenter().lng + 0.0025]]).addTo(map);	
				} 
	
   };
	  




	  
	//////////////////////////////////////////////////////////
	/////Functions ///////////////////////////////////////////
	//////////////////////////////////////////////////////////  
	  
	//PAN
	  function pan_left() {
	    
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
				
				 //outputContainer.innerHTML = "";
				//if (outputContainer) outputContainer.innerHTML = "";
				//document.getElementById("output").innerHTML = "";
	  
	   };
	    function pan_right() {
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
	  };
	   function pan_up() {
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
	  };
	  
	   function pan_down() {
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
	  };  
	  