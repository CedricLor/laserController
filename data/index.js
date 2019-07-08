
// Global variables
var ws = null;
var controlerBoxes = new Map();
var boxesRows = new Map();
var boxRowTemplate = boxRowTemplateSelector();

// WEB SOCKET
function connect() {
  ws = new WebSocket('ws://192.168.43.84/ws');
  ws.onopen = function() {
    console.log("WS connection open ...");
    ws.send(JSON.stringify({
      type: 0,
      message: mapToObj(controlerBoxes)
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
    if (_data.type === 4) { // User request to change boxState has been received and is being processed
      updateStateButton(_data.message);
    }
    if (_data.type === 5) { // boxState of existing box has been updated
      addNewRowForNewBox(_data.message);
    }
    if (_data.type === 6) { // a new box has connected to the mesh
      addNewRowForNewBox(_data.message);
    }
    if (_data.type === 7) { // an existing box has been disconnected from the mesh
      deleteBoxRow(_data.message);
    }
    if (_data.type === 8) { // a box has changed master
      updateMasterBoxNumber(_data.message);
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



function onclickButton(e) {
  console.log("onclickButton starting");

  var _laserBoxNumber = parseInt(this.parentNode.parentNode.parentNode.dataset.lb); // div.box_wrapper[data-lb=X] > div.box_state_setter > div.setters_group > button
  var _boxRow = boxesRows.get(_laserBoxNumber);
  var _buttonList = stateButtonsDOMSelector(_boxRow); // modified

  // remove red on other buttons
  for (var i = 0; i < _buttonList.length; i++) {
    _buttonList[i].classList.remove('button_clicked');
  }
  // turn this button red
  this.classList.add('button_clicked');

  // send a WSrequest to the server to change the boxState to the corresponding boxstate
  var _json = JSON.stringify({
    type: 4,
    lb: _laserBoxNumber,
    boxState: this.dataset.boxstate });
  console.log("onclickButton: about to send json via WS: " + _json);
  ws.send(_json);
  console.log("onclickButton: json sent.");

  console.log("onclickButton: ending");
};

function oninputMasterSelect(e) {
  console.log("oninputMasterSelect: starting");
  if ((this.parentNode.parentNode.dataset.lb !== "undefined" )) { // selector to update in case of change in the DOM structure
    console.log("oninputMasterSelect: slave box: " + (this.parentNode.parentNode.dataset.lb + 200));
    console.log("oninputMasterSelect: master box " + this.selectedIndex);
    var _json = JSON.stringify({
      type: 8,
      lb: this.parentNode.parentNode.dataset.lb,
      masterbox: parseInt(this.options[this.selectedIndex].value, 10)
      // masterbox: this.selectedIndex + 1
     });
    console.log("oninputMasterSelect: about to send json via WS: " + _json);
    ws.send(_json);
    console.log("onclickButton: json sent.");
  }
  console.log("oninputMasterSelect: ending");
}
// END EVENT HANDLERS

// DOM MANIPULATION
function updateStationIp(_stationIp) {
  console.log("updateStationIp starting.");
  console.log(_stationIp);
  document.getElementById('stationIp').innerHTML = _stationIp;
  console.log("updateStationIp ending.");
}

function updateStateButton(data) {
  console.log("updateStateButton starting.");
  var _boxRow = boxesRows.get(data.lb);
  var _elt = stateButtonDOMSelector(_boxRow, data.boxState);
  console.log(_elt);
  if (_elt) {
    _elt.classList.add('button_change_received');
  }
  console.log("updateStateButton ending.");
}

function updateActiveStateButton(data) {
  console.log("updateActiveStateButton starting.");
  // remove formerly added classes on all stateButtons of the boxRow
  var _boxRow = boxesRows.get(data.lb);

  // remove classes on all the others stateButtons of this boxRow
  _removeClassesOnAllRowStateButtons(_boxRow);

  // add button_active_state class to the relevant stateButton
  _setActiveStateButton(data, _boxRow);
  console.log("updateActiveStateButton ending.");
}




function _removeClassesOnAllRowStateButtons(_boxRow) {
  console.log("_removeClassesOnAllRowStateButtons starting.");
  var _elt_arr = stateButtonsDOMSelector(_boxRow);
  console.log("_removeClassesOnAllRowStateButtons: array of all the buttons related to this boxRow available = ");console.log(_elt_arr);
  if (_elt_arr && _elt_arr.length) {
    _elt_arr.forEach(
      function(currentValue, currentIndex, listObj) {
        currentValue.classList.remove('button_active_state');
        currentValue.classList.remove('button_change_received');
        currentValue.classList.remove('button_clicked');
      }
    );
  }
  console.log("_removeClassesOnAllRowStateButtons ending.");
}




function _setActiveStateButton(data, memRow) {
  console.log("_setActiveStateButton: preparing a selector to select the state buttons included in _dupRow.");
  var _selectorActiveBoxState = "button[data-boxstate='" + data.boxState + "']";
  console.log("_setActiveStateButton: selector created: '" + _selectorActiveBoxState + "'");
  var _activeStateButtonList = memRow.querySelectorAll(_selectorActiveBoxState);
  console.log("_setActiveStateButton: button list selected: ");console.log(_activeStateButtonList);
  if (_activeStateButtonList) {
    console.log("_setActiveStateButton: about to add the active class to select button");
    _activeStateButtonList[0].classList.add('button_active_state');
    _activeStateButtonList[0].classList.remove('button_change_received');
    _activeStateButtonList[0].classList.remove('button_clicked');
  }
  console.log("_setActiveStateButton: ending on returning row");
  return memRow;
}




function _newBoxRowSetProperties(data, _dupRow) {
  console.log("_newBoxRowSetProperties: _dupRow: setting the id of the new wrapper div to: " + data.lb);
  _dupRow.id = "boxRow" + data.lb;     // update data-lb attribute
  console.log("_newBoxRowSetProperties: _dupRow: setting the data-lb property of the new wrapper div to: " + data.lb);
  _dupRow.dataset.lb = data.lb;     // update data-lb attribute
  console.log("_newBoxRowSetProperties: _dupRow: removing the class hidden from the classes of the new wrapper div");
  _dupRow.classList.remove('hidden');
  console.log("_newBoxRowSetProperties: _dupRow: setting the laser box number: " + (data.lb + 200));
  _dupRow.children[0].children[0].children[0].textContent = data.lb + 200;
  return _dupRow;
}




function _setEVentListenersStateButtons(_dupRow) {
  console.log("_setEVentListenersStateButtons: about to set event listeners on buttons");
  var _stateButtonListSelector = "button[data-boxstate]";
  console.log("_setEVentListenersStateButtons: _stateButtonListSelector = " + _stateButtonListSelector);
  var _buttonList = _dupRow.querySelectorAll(_stateButtonListSelector);
  console.log("_setEVentListenersStateButtons: buttonList selected");
  console.log(_buttonList);
  console.log("_setEVentListenersStateButtons: about to call setStateButtonEvents");
  setStateButtonEvents(_buttonList);
  return _dupRow;
}




function _indicateMasterBoxNumber(data, _dupRow) {
  console.log("_indicateMasterBoxNumber: starting. About to write masterbox number");

  // Write box number in box number span
  var _masterBoxNumberSelector = "span.master_box_number";
  console.log("_indicateMasterBoxNumber: _masterBoxNumberSelector = " + _masterBoxNumberSelector);
  var _masterBoxNumbSpan = _dupRow.querySelector(_masterBoxNumberSelector);
  console.log("_indicateMasterBoxNumber: masterbox span selected. Writing box number:" + data.ms);
  _masterBoxNumbSpan.textContent = data.ms + 200;
  console.log("_indicateMasterBoxNumber: Writed box number in masterBox span.");

  // Select corresponding option in masterBoxSelect
  var _select = _selectMasterSelectInRow(_dupRow);
  console.log("_indicateMasterBoxNumber: starting. About to select correct option in master select");
  _select.value=parseInt(data.ms);
  console.log("_indicateMasterBoxNumber: ending. About to return _dupRow.");

  return _dupRow;
}




function _selectMasterSelectInRow(_dupRow) {
  console.log("_selectMasterSelectInRow: starting");
  var _masterSelectSelector = "select.master_select";
  console.log("_selectMasterSelectInRow: _masterSelectSelector = " + _masterSelectSelector);
  var _select = _dupRow.querySelector(_masterSelectSelector);
  console.log("_selectMasterSelectInRow: master select selected: ");
  console.log(_select);
  console.log("_selectMasterSelectInRow: ending returning _select:");
  return _select;
}




function _renderInDom(_dupRow) {
  console.log("_renderInDom: about to insert the new box in the DOM");
  _dupRow = document.getElementById('boxTemplate').parentNode.insertBefore(_dupRow, document.getElementById('boxTemplate'));
  console.log("_renderInDom: inserted the new box in the in DOM:");
  console.log(_dupRow);
  return _dupRow;
}




function _addToMaps(data, _dupRow) {
  console.log("_addToMaps starting.");
  controlerBoxes.set(data.lb, data.boxState);
  console.log("_addToMaps: controlerBoxes map: set key [" + data.lb + "] with value [" + data.boxState +  "] in controlerBoxes map.");
  boxesRows.set(data.lb, _dupRow);
  console.log("_addToMaps: boxesRows map: set key [" + data.lb + "] with value [" + _dupRow +  "] in boxesRows map.");
  console.log(controlerBoxes);
  console.log("_addToMaps ending.");
}




function _deleteFromMaps(boxNumber) {
  console.log("_deleteFromMaps starting.");
  controlerBoxes.delete(boxNumber);
  boxesRows.delete(boxNumber);
  console.log("_deleteFromMaps ending.");
}




function addNewRowForNewBox(data) {
  console.log("addNewRowForNewBox starting.");

  // Check whether the boxRow has not already been created
  var _controlerBoxEntry = controlerBoxes.get(data.lb);
  console.log("addNewRowForNewBox: looking if an entry exists in the map for this box");
  console.log("addNewRowForNewBox _controlerBoxEntry (if undefined, the entry does not exist): " + _controlerBoxEntry);
  console.log("addNewRowForNewBox: testing if (!(_controlerBoxEntry === undefined)): " + (!(_controlerBoxEntry === undefined)));

  // Case where it the box does not exist
  if (_controlerBoxEntry === undefined) {
    // _controlerBoxEntry is equal to undefined: the boxRow does not already exists
    // let's create it
    console.log("addNewRowForNewBox: the boxRow does not already exist. I am about to create it.");
    if (boxRowTemplate) {
      // clone the template
      console.log("addNewRowForNewBox: Hidden boxRow selected. About to clone it.");
      var _dupRow = boxRowTemplate.cloneNode(true);  // duplicate the box
      console.log("addNewRowForNewBox: Clone _dupRow created");//console.log(_dupRow);

      // set properties
      _dupRow = _newBoxRowSetProperties(data, _dupRow);

      // set the activeState button
      _dupRow = _setActiveStateButton(data, _dupRow);

      // set event listener on buttons
      _dupRow = _setEVentListenersStateButtons(_dupRow);

      // indicate masterbox number
      _dupRow = _indicateMasterBoxNumber(data, _dupRow);

      // set event listener on slave select
      var _select = _selectMasterSelectInRow(_dupRow);
      setSelectEvents(_select);

      // render in DOM
      _dupRow = _renderInDom(_dupRow);

      // add a key/entry pair to the controlerBoxes map and to the rowsMap map
      _addToMaps(data, _dupRow);
    }
    console.log("addNewRowForNewBox ending after adding laser box [" + data.lb + "]");
  }

  // Case where the box exists
  else {
    // _controlerBoxEntry is not equal to undefined, the boxRow already exists
    // let's update it instead
    console.log("addNewRowForNewBox: a boxRow for laser box [" + data.lb + "] already exists in DOM.");
    console.log("addNewRowForNewBox: About to set the activeState button.");
    updateActiveStateButton(data);
    console.log("addNewRowForNewBox ending after updating laser box [" + data.lb + "]");
  }
}




function deleteBoxRow(data) {
  console.log("deleteBoxRow starting.");
  var _boxRowToDelete = boxRowDOMSelector(data.lb);
  if (_boxRowToDelete === undefined) {
    console.log("deleteBoxRow: There was no laser box [" + data.lb + "] in controlerBoxes map.");
  } else {
    console.log("deleteBoxRow: About to delete row corresponding to laser box [" + data.lb + "] in DOM and maps.");
    _boxRowToDelete.parentNode.removeChild(_boxRowToDelete);
    _deleteFromMaps(data.lb); // updating the controlesBoxes map
    console.log("deleteBoxRow: deleted key [" + data.lb + "] in controlerBoxes map.");
    console.log(controlerBoxes);
  }
  console.log("deleteBoxRow ending.");
}




function updateMasterBoxNumber(data) {
  console.log("updateMasterBoxNumber starting.");
  // select the relevant row
  var _row = boxRowDOMSelector(data.lb);
  console.log("updateMasterBoxNumber: about to write masterbox number");
  _masterBoxNumberSelector = "span.master_box_number";
  console.log("updateMasterBoxNumber: _masterBoxNumberSelector = " + _masterBoxNumberSelector);
  var _select = _row.querySelector(_masterBoxNumberSelector);
  console.log("addNewRowForNewBox: masterbox span selected");
  _row.children[1].children[0].textContent = data.ms + 200;
  console.log("addNewRowForNewBox: masterbox span updated: " + (data.ms + 200));
  console.log("addNewRowForNewBox: data.st === 1: " + (data.st === 1))
  if (data.st === 1) {
    _row.children[1].children[0].classList.add("change_ms_received");
    console.log("addNewRowForNewBox: added class change_ms_received to masterbox span");
  } else if (data.st === 2) {
    console.log("addNewRowForNewBox: data.st === 2: " + (data.st === 2))
    _row.children[1].children[0].classList.remove("change_ms_received");
    console.log("addNewRowForNewBox: removed class change_ms_received to masterbox span");
    _row.children[1].children[0].classList.add("change_ms_executed");
    console.log("addNewRowForNewBox: added class change_ms_executed to masterbox span");
  }

  console.log("updateMasterBoxNumber ending.");
}




function boxRowDOMSelector(laserBoxIndexNumber) {
  console.log("boxRowDOMSelector starting.");
  // div[data-lb='1'] button[data-boxstate='1'
  // var _selector = "div[data-lb='"+ laserBoxIndexNumber + "']"; // selector for the whole div

  // div.box_wrapper[data-lb=X] > div.box_state_setter > div.setters_group > button
  var _selector = "div.box_wrapper[data-lb='" + laserBoxIndexNumber + "']";
  console.log(_selector);
  var _rows = document.querySelectorAll(_selector); // should be a list composed of one single element
  console.log("boxRowDOMSelector ending.");
  return _rows[0]; // return the first (and unique) element of the list
}




function boxRowTemplateSelector() {
  console.log("boxRowTemplateSelector starting.");
  var _row = document.getElementById('boxTemplate'); // should be a list composed of one single element
  var _templateDup = _row.cloneNode(true);
  console.log("boxRowTemplateSelector ending.");
  return _templateDup; // return the first (and unique) element of the list
}


// function stateButtonsDOMSelector(laserBoxIndexNumber) {
//   console.log("stateButtonsDOMSelector starting.");
//   var _boxRow = boxesRows.get(laserBoxIndexNumber);
//   var _buttonsSelector = "div.box_state_setter > div.setters_group > button";
//   var _elts = _boxRow.querySelectorAll(_buttonsSelector);
//
//   console.log(_elts);
//   console.log("stateButtonsDOMSelector ending.");
//   return _elts;
// }
//


function stateButtonsDOMSelector(_boxRow) {
  console.log("stateButtonsDOMSelector starting.");
  var _buttonsSelector = "div.box_state_setter > div.setters_group > button";
  var _elts = _boxRow.querySelectorAll(_buttonsSelector);

  console.log(_elts);
  console.log("stateButtonsDOMSelector ending.");
  return _elts;
}


function stateButtonDOMSelector(laserBoxIndexNumber, boxActiveState) {
  console.log("stateButtonDOMSelector starting.");

  // div.box_wrapper[data-lb=X] > div.box_state_setter > div.setters_group > button
  var _boxRow = boxesRows.get(laserBoxIndexNumber);
  var _buttonSelector = "div.box_state_setter > div.setters_group > button[data-boxstate='" + boxActiveState + "']";
  var _elts = _boxRow.querySelectorAll(_buttonSelector);

  console.log(_elts);
  console.log("stateButtonDOMSelector ending.");
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

function setSelectEvents(selectElt) {
  selectElt.addEventListener('input', oninputMasterSelect, false);
}
// END EVENT LISTENERS





// WINDOW LOAD
window.onload = function(e){
    console.log("window.onload");
    // Interval at which to check if WS server is still available (and reconnect as necessary)
    setInterval(check, 5000);
    // setInterval(check, (getRandomArbitrary(10, 4) * 1000));
}
// END WINDOW LOAD




// HELPERS
// Returns a random number between min (inclusive) and max (exclusive)
function getRandomArbitrary(min, max) {
    return Math.random() * (max - min) + min;
}

// Converts a Map to an Object
function mapToObj(map) {
    let obj = Object.create(null);
    for (let [k,v] of map) {
        obj[k] = v;
    }
    return obj;
}
