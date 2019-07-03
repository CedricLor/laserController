
// Global variables
var ws = null;



// WEB SOCKET
function connect() {
  ws = new WebSocket('ws://192.168.43.84/ws');
  ws.onopen = function() {
    console.log("WS connection open ...");
    ws.send(JSON.stringify({
      type: 0,
      message: "Hello WebSockets! This is Cedric."
    }));
  };

  ws.onmessage = function(e) {
    var received_msg = e.data;
    console.log( "WS Received Message: " + received_msg);
    var _data = JSON.parse(e.data);
    // console.log("WS JSON message: " + _data.message);
    if (_data.type === 3) {
      updateStationIp(_data.message);
      ws.send(JSON.stringify({
        type: 3,
        message: "received Station IP"
      }));
    }
    if (_data.type === 4) {
      updateButton(_data.message);
    }
    if (_data.type === 5) {
      setActiveStateButton(_data.message);
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
 * On click button callback function
 */
function onclickButton(e) {
  console.log("onclickButton starting");
  var _laserBoxNumber = this.parentNode.parentNode.dataset.lb;
  var _buttonList = StateButtonsDOMSelector(_laserBoxNumber);

  // remove red on other buttons
  for (var i = 0; i < _buttonList.length; i++) {
    _buttonList[i].classList.remove('button_clicked');
  }
  // turn this button red
  this.classList.add('button_clicked');
  // add sending a request in WS to the server
  var _json = JSON.stringify({
    type: 4,
    lb: this.parentNode.parentNode.getAttribute('data-lb'),
    boxState: this.getAttribute('data-boxstate') })
  ws.send(_json);
  console.log("onclickButton stoping");
};

function updateButton(data) {
  console.log("updateButton starting.");
  var _elt = StateButtonDOMSelector(data);
  console.log(_elt);
  _elt.classList.add('button_change_received');
  // var _elt = StateButtonsDOMSelector(data);
  // _elt_arr.forEach(
  //   function(currentValue, currentIndex, listObj) {
  //     _elt_arr.classList.add('button_change_received');
  //   }
  // );
  console.log("updateButton ending.");
}

function setActiveStateButton(data) {
  console.log("setActiveStateButton starting.");
  var _elt = StateButtonDOMSelector(data);
  console.log(_elt);
  _elt.classList.add('button_active_state');
  _elt.classList.remove('button_clicked');
  _elt.classList.remove('button_change_received');
  // var _elt = StateButtonsDOMSelector(data);
  // _elt_arr.forEach(
  //   function(currentValue, currentIndex, listObj) {
  //     currentValue.classList.add('button_active_state');
  //   }
  // );
  console.log("setActiveStateButton ending.");
}

function StateButtonDOMSelector(data) {
  console.log("buttonStateDOMSelector starting.");
  // div[data-lb='1'] button[data-boxstate='1'
  // "div[data-lb='1'] > div > button[data-boxstate='1'"
  var _selector = "div[data-lb='" + data.lb + "'] > div > button[data-boxstate='" + data.boxState + "']";
  console.log(_selector);
  var _elts = document.querySelectorAll(_selector);
  console.log(_elts);
  console.log("buttonStateDOMSelector ending.");
  return _elts[0];
}

function updateStationIp(_stationIp) {
  console.log("updateStationIp starting.");
  console.log(_stationIp);
  document.getElementById('stationIp').innerHTML = _stationIp;
  console.log("updateStationIp ending.");
}
// END DOM MANIPULATION


// WINDOW LOAD
window.onload = function(e){
    console.log("window.onload");
    // Interval at which to check if WS server is still available (and reconnect as necessary)
    setInterval(check, 5000);
    // setInterval(check, (getRandomArbitrary(10, 4) * 1000));
}


/**
 * Returns a random number between min (inclusive) and max (exclusive)
 */
function getRandomArbitrary(min, max) {
    return Math.random() * (max - min) + min;
}
