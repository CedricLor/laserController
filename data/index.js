
// Global variables
var ws = null;

// WEB SOCKET
function connect() {
  ws = new WebSocket('ws://192.168.43.84/ws');
  ws.onopen = function() {
    console.log("WS connection open ...");
    ws.send("Hello WebSockets! This is Cedric.");
    ws.send(JSON.stringify({
      type: 0,
      message: "Hello WebSockets! This is Cedric."
    }));
  };

  ws.onmessage = function(e) {
    var received_msg = e.data;
    console.log( "WS Received Message: " + received_msg);
    var _data = JSON.parse(e.data);
    console.log("WS message: " + _data.message);
    if (_data.type === 3) {
      updateStationIp(_data.message);
    }
    if (_data.type === 4) {
      updateButton(_data.message);
    }
  };

  ws.onclose = function(e) {
    console.log('Socket is closed. Reconnect will be attempted in 5 to 10 seconds.', e.reason);
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
  var _json = JSON.stringify({
    type: 4,
    lb: this.getAttribute('data-lb'),
    boxState: this.getAttribute('data-boxstate') })
  ws.send(_json);
};

function updateStationIp(_stationIp) {
  console.log("updateStationIp starting.");
  console.log(_stationIp);
  document.getElementById('stationIp').innerHTML = _stationIp;
  console.log("updateStationIp ending.");
}

function updateButton(data) {
  console.log("updateButton starting.");
  var _lb_selector = "[data-lb='" + data.lb + "']";
  console.log(_lb_selector);
  var _elts = document.querySelectorAll(_lb_selector);
  console.log(_elts);
  var _boxstate_selector = "[data-boxstate='" + data.boxState + "']";
  console.log(_boxstate_selector);
  var _elt_arr = document.querySelectorAll(_boxstate_selector);
  _elt_arr.forEach(
    function(currentValue, currentIndex, listObj) {
      currentValue.classList.add('button_change_received');
    }
  );
  console.log("updateButton ending.");
}
// END DOM MANIPULATION


// WINDOW LOAD
window.onload = function(e){
    console.log("window.onload");
    // Interval at which to check if WS server is still available (and reconnect as necessary)
    setInterval(check, 5000);
    // setInterval(check, (getRandomArbitrary(10, 4) * 1000));

    // select the whole button class
    buttonClass = document.getElementsByClassName("button");
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
