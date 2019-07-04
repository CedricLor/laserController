
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
    if (_data.type === 6) {
      addNewRowForNewBox(_data.message);
    }
    if (_data.type === 7) {
      deleteBoxRow(_data.message);
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

function updateStationIp(_stationIp) {
  console.log("updateStationIp starting.");
  console.log(_stationIp);
  document.getElementById('stationIp').innerHTML = _stationIp;
  console.log("updateStationIp ending.");
}

function updateButton(data) {
  console.log("updateButton starting.");
  var _elt = StateButtonDOMSelector(data.lb, data.boxState);
  console.log(_elt);
  _elt.classList.add('button_change_received');
  console.log("updateButton ending.");
}

function setActiveStateButton(data) {
  console.log("setActiveStateButton starting.");
  // remove formerly added classes on all stateButtons of the boxRow
  var _elt_arr = StateButtonsDOMSelector(data.lb);
  // add button_active_state class to the relevant stateButton
  var _elt = StateButtonDOMSelector(data.lb, data.boxState);
  console.log(_elt);
  _elt_arr.forEach(
    function(currentValue, currentIndex, listObj) {
      currentValue.classList.remove('button_active_state');
      currentValue.classList.remove('button_change_received');
      currentValue.classList.remove('button_clicked');
    }
  );
  _elt.classList.add('button_active_state');
  _elt.classList.remove('button_clicked');
  _elt.classList.remove('button_change_received');
  console.log("setActiveStateButton ending.");
}

function addNewRowForNewBox(data) {
  console.log("addNewRowForNewBox starting.");
  var _boxRow = BoxRowDOMSelector(0);
  var _dupRow = _boxRow.cloneNode(true);  // duplicate the box

  _dupRow.dataset.lb = data.lb;     // update data-lb attribute
  _dupRow.classList.remove('hidden');
  _dupRow.children[0].textContent = data.lb + 200;

  var _selectorBoxState = "div > button[data-boxstate='" + data.boxState + "']";
  var _stateButtonList = _dupRow.querySelectorAll(_selectorBoxState);
  _stateButtonList[0].classList.add('button_active_state');

  _boxRow.parentNode.insertBefore(_dupRow, _boxRow);

  buttonList = document.querySelectorAll("div[data-lb='" + data.lb + "'] > div > button");
  console.log(buttonList);
  setStateButtonEvents(buttonList);

  console.log("addNewRowForNewBox ending.");
}

function deleteBoxRow(data) {
  console.log("deleteBoxRow starting.");

  console.log("deleteBoxRow ending.");
}

function BoxRowDOMSelector(laserBoxIndexNumber) {
  console.log("BoxRowDOMSelector starting.");
  // div[data-lb='1'] button[data-boxstate='1'
  var _selector = "div[data-lb='"+ laserBoxIndexNumber + "']"; // selector for the whole div
  console.log(_selector);
  var _rows = document.querySelectorAll(_selector); // should be a list composed of one single element
  console.log("BoxRowDOMSelector ending.");
  return _rows[0]; // return the first (and unique) element of the list
}

function StateButtonsDOMSelector(laserBoxIndexNumber) {
  console.log("StateButtonsDOMSelector starting.");
  // div[data-lb='1'] button[data-boxstate='1'
  // "div[data-lb='1'] > div > button[data-boxstate='1'"
  var _selector = "div[data-lb='" + laserBoxIndexNumber + "'] > div > button";
  console.log(_selector);
  var _elts = document.querySelectorAll(_selector);
  console.log(_elts);
  console.log("StateButtonsDOMSelector ending.");
  return _elts;
}

function StateButtonDOMSelector(laserBoxIndexNumber, boxActiveState) {
  console.log("buttonStateDOMSelector starting.");
  // div[data-lb='1'] button[data-boxstate='1'
  // "div[data-lb='1'] > div > button[data-boxstate='1'"
  var _selector = "div[data-lb='" + laserBoxIndexNumber + "'] > div > button[data-boxstate='" + boxActiveState + "']";
  console.log(_selector);
  var _elts = document.querySelectorAll(_selector);
  console.log(_elts);
  console.log("buttonStateDOMSelector ending.");
  return _elts[0];
}

// END DOM MANIPULATION



// EVENT LISTENERS
function setStateButtonEvents(buttonList) {
  // iterate over each buttons and add an eventListener on click
  for (var i = 0; i < buttonList.length; i++) {
    buttonList[i].addEventListener('click', onclickButton, false);
  }
}
// END EVENT LISTENERS




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
