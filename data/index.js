
// Global variables
var ws = null;

// WEB SOCKET
function connect() {
  ws = new WebSocket('ws://192.168.43.84/ws');
  ws.onopen = function() {
    console.log("WS connection open ...");
    ws.send("Hello WebSockets! This is Cedric.");
    // ws.send(JSON.stringify({
    //     //.... some message the I must send when I connect ....
    // }));
  };

  ws.onmessage = function(e) {
    var received_msg = e.data;
    console.log( "WS Received Message: " + received_msg);
    // var _data = JSON.parse(evt.data);
    // console.log( "Received Message: " + _data);
    // if (_data.stationIp) {
    //   updateStationIp(_data.stationIp);
    // }
  };

  ws.onclose = function(e) {
    console.log('Socket is closed. Reconnect will be attempted in 5 second.', e.reason);
    // setTimeout (connect, Math.min(10000,timeout+=timeout))
    // setTimeout(function() {
    //   connect();
    // }, 1000);
    check();
  };

  ws.onerror = function(err) {
    console.error('Socket encountered error: ', err.message, 'Closing socket');
    ws.close();
  };
}

// Check if WS server is still available (and reconnect as necessary)
function check(){
  if(!ws || ws.readyState === WebSocket.CLOSED) connect();
  // if(!ws || ws.readyState == 3) connect();
}
// WEB SOCKET END



// DOM MANIPULATION
/**
 * Create a var that will in the future select all the buttons in the DOM by class "button"
 */
var buttonClass;

/**
 * On click button callback function
 */
function onclickButton(e) {
  // remove red on other buttons
  for (var i = 0; i < buttonClass.length; i++) {
    buttonClass[i].classList.remove('button_clicked');
  }
  // turn this button red
  this.classList.add('button_clicked');
  // add sending a request in WS to the server
  ws.send(this.getAttribute('data-href'));
};

/**
 * On window load
 */
window.onload = function(e){
    console.log("window.onload");
    // iterate over each buttons and add an eventListener on click
    for (var i = 0; i < buttonClass.length; i++) {
      buttonClass[i].addEventListener('click', onclickButton, false);
    }
}


/**
 * Returns a random number between min (inclusive) and max (exclusive)
 */
function getRandomArbitrary(min, max) {
    return Math.random() * (max - min) + min;
}
