
// Global variables
var ws = null;
var controlerBoxes = new Map();
var boxesRows = new Map();
var boxRowTemplate = boxRowTemplateSelector();






// WEB SOCKET
function connect() {
  ws = new WebSocket('ws://192.168.43.84/ws');

  // onopen websocket, send a message to the server with the list of controlerBoxes
  // currently in the DOM and their states
  ws.onopen = function() {
    console.log("WS connection open. Sending the server the list of controlerBoxes I have in the DOM (and their current state)");
    ws.send(JSON.stringify({
      action: "handshake",
      boxesStatesInDOM: mapToObj(controlerBoxes)
    }));
    // {action:0, boxStateInDOM:{1:4;2:3}}
  };

  // on receive a message, decode its action type to dispatch it
  ws.onmessage = function(e) {
    var received_msg = e.data;
    console.log( "WS Received Message: " + received_msg);
    var _data = JSON.parse(e.data);
    if (_data.action === 3) {
      // console.log("WS JSON message: " + _data.ServerIP);
      updateStationIp(_data.serverIP);
      ws.send(JSON.stringify({
        action: "ReceivedIP"
      }));
      // {action:"ReceivedIP}
      return;
    }
    if (_data.action === "changeBox" && _data.key === "boxState") { // 4. User request to change boxState of a given box has been received and is being processed
                                                                    // 5. boxState of existing box has been updated
      // _data = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 1} // boxState // ancient 4
      // _data = {lb: 1; action: "changeBox"; key: "boxState"; val: 6; st: 2}
      updateStateButton(_data);
      return;
    }
    if (_data.action === "addBox") { // 6. a new box has connected to the mesh
      // _data = {lb:1; action: "addBox"; boxState: 3; masterbox: 4; boxDefstate: 6}
      addOrUpdateNewRowForNewBox(_data);
      return;
    }
    if (_data.action === "deleteBox") { // 7. an existing box has been disconnected from the mesh
      // or the DOM contained boxRows corresponding to boxes that have been disconnected
      // from the mesh
      if (_data.lb === 'a') {
        // _data = {action: "deleteBox"; lb: "a"}
        deleteAllBoxRows();
      } else {
        // _data = {lb:1; action:"deleteBox"}
        deleteBoxRow(_data);
      }
      return;
    }
    if (_data.action === "changeBox" && _data.key === "masterbox") { // 8. a box has changed master
      // _data = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 1} // masterbox // ancient 8
      // _data = {lb: 1; action: "changeBox"; key: "masterbox"; val: 9; st: 2}
      updateMasterBoxNumber(_data);
      return;
    }
    if (_data.action === "changeBox" && _data.key === "boxDefstate") { // 9. User request to change default boxState of a given box has been received and is being processed
                                                                                         // 10. the default state of a given box has changed
      // _data = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate // ancient 9
      // _data = {lb:1; action: "changeBox"; key: "boxDefstate"; val: 4; st: 2}
      updateStateButton(_data);
      return;
    }
  };

  // onclose, just inform the user that an attempt to reconnect will be made soon
  ws.onclose = function(e) {
    console.log('Socket is closed. Reconnect will be attempted in 4 to 10 seconds.', e.reason);
  };

  // onerror, inform the user that you are closing the socket
  ws.onerror = function(err) {
    console.error('Socket encountered error: ', err.message, 'Closing socket');
    ws.close();
  };
}


// Check if WS server is still available (and reconnect as necessary)
function check(){
  if(!ws || ws.readyState === WebSocket.CLOSED) {
    console.log("!!! -- WE HAVE BEEN DISCONNECTED -- !!!");
    console.log("!!! Trying to reconnect !!!");
    connect();
  }
}
// WEB SOCKET END





function findUpLaserBoxNumber(el) {
    while (el.parentNode) {
        el = el.parentNode;
        if (el.dataset.lb) {
          return parseInt(el.dataset.lb, 10);
        }
    }
    return null;
}



// EVENTS HANDLER
function onclickRebootBoxButton(e) {
  console.log("onclickRebootBoxButton starting");
  var _laserBoxNumber = findUpLaserBoxNumber(this.parentNode);

  ws.send(JSON.stringify({
    action: "changeBox",
    key: "reboot",
    save: 0, // reboot without saving
    lb: _laserBoxNumber
  }));
  // {action:"changeBox", key:"reboot", save: 0, lb:1}
  console.log("onclickRebootBoxButton: ending");
};



function onclickRebootAndSaveBoxButton(e) {
  console.log("onclickRebootAndSaveBoxButton starting");
  var _laserBoxNumber = findUpLaserBoxNumber(this.parentNode);

  ws.send(JSON.stringify({
    action: "changeBox",
    key: "reboot",
    save: 1, // save and reboot
    lb: _laserBoxNumber
  }));
  // {action:"changeBox", key:"reboot", save: 1, lb:1}
  console.log("onclickRebootAndSaveBoxButton: ending");
};




function onclickSavePrefsBoxButton(e) {
  console.log("onclickSavePrefsBoxButton starting");
  var _laserBoxNumber = findUpLaserBoxNumber(this.parentNode);

  ws.send(JSON.stringify({
    action: "changeBox",
    key: "save",
    val: "all", // save all the values for the box and the net
    lb: _laserBoxNumber
  }));
  // {action:"changeBox", key:"save", val: "all", lb:1}
  console.log("onclickSavePrefsBoxButton: ending");
};




function onclickRebootLBsButton(e) {
  console.log("onclickRebootLBsButton starting");

  ws.send(JSON.stringify({
    action: "changeNet",
    key: "reboot",
    save: 0, // reboot without saving
    lb: "LBs"
  }));
  // {action: "changeNet", key: "reboot", save: 0, lb: "LBs"}
  console.log("onclickRebootLBsButton: ending");
};



function onclickRebootIFButton(e) {
  console.log("onclickRebootIFButton starting");

  ws.send(JSON.stringify({
    action: "changeBox",
    key: "reboot",
    save: 0,
    lb: 0
  }));
  // {action:"changeBox", key:"reboot", save: 0, lb:0}
  console.log("onclickRebootIFButton: ending");
};



function onclickRebootAllButton(e) {
  console.log("onclickRebootAllButton starting");

  ws.send(JSON.stringify({
    action: "changeNet",
    key: "reboot",
    save: 0,
    lb: "all"
  }));
  // {action: "changeNet", key: "reboot", save: 0, lb: "all"}
  console.log("onclickRebootAllButton: ending");
};




function onclickSaveLBsButton(e) {
  console.log("onclickSaveLBsButton starting");

  ws.send(JSON.stringify({
    action: "changeNet",
    key: "save",
    val: "all",
    lb: "LBs"
  }));
  // {action: "changeNet", key: "save", val: "all", lb: "LBs"}
  console.log("onclickSaveLBsButton: ending");
};





function onclickSaveIFButton(e) {
  console.log("onclickSaveIFButton starting");

  ws.send(JSON.stringify({
    action: "changeBox",
    key: "save",
    val: "all",
    lb: 0
  }));
  // {action: "changeBox", key: "save", val: "all", lb: 0}
  console.log("onclickSaveIFButton: ending");
};





function onclickSaveAllButton(e) {
  console.log("onclickSaveAllButton starting");

  ws.send(JSON.stringify({
    action: "changeNet",
    key: "save",
    val: "all",
    lb: "all"
  }));
  // {action: "changeNet", key: "save", val: "all", lb: "all"}
  console.log("onclickSaveAllButton: ending");
};



function onclickButton(e) {
  console.log("onclickButton starting");
  _onclickButtonWrapper(this, "button[data-boxstate]", this.dataset.boxstate, "boxState");
  console.log("onclickButton: ending");
};


function onclickDefStateButton(e) {
  console.log("onclickDefStateButton starting");
  _onclickButtonWrapper(this, "button[data-boxDefstate]", this.dataset.boxdefstate, "boxDefstate");
  console.log("onclickDefStateButton: ending");
};

//   // _obj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3} // boxState // ancient 4
//   // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4} // masterbox // ancient 8
//   // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3} // boxDefstate // ancient 9

function _onclickButtonWrapper(clickedTarget, buttonSelector, _datasetValue, _clef) {
  var _laserBoxNumber = findUpLaserBoxNumber(clickedTarget.parentNode);
  _onclickButtonClassSetter(clickedTarget, buttonSelector, _laserBoxNumber);
  _onclickButtonWSSender(_laserBoxNumber, _datasetValue, _clef);
}

function _onclickButtonClassSetter(clickedTarget, buttonSelector, _laserBoxNumber) {
  var _boxRow = boxesRows.get(_laserBoxNumber);
  var _buttonList = boxRowEltsGroupSelector(_boxRow, buttonSelector);
  // remove red on other buttons
  for (var i = 0; i < _buttonList.length; i++) {
    _buttonList[i].classList.remove('button_clicked');
  }
  // turn this button red
  clickedTarget.classList.add('button_clicked');

  return _laserBoxNumber;
}

function _onclickButtonWSSender(_laserBoxNumber, _datasetValue, _clef) {
  var __toBeStringified = {};
  __toBeStringified["action"] = "changeBox";
  __toBeStringified["key"] = _clef;
  __toBeStringified["lb"] = _laserBoxNumber;
  __toBeStringified["val"] = parseInt(_datasetValue, 10);

  var _json = JSON.stringify(__toBeStringified);
  // {action: 4; lb: 1; "boxState": 3}
  // {action: 9; lb: 1; "boxDefstate": 3}
  //   // _obj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3} // boxState // ancient 4
  //   // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4} // masterbox // ancient 8
  //   // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3} // boxDefstate // ancient 9
  console.log("_onclickButtonWSSender: about to send JSON via WS: " + _json);
  ws.send(_json);
  console.log("_onclickButtonWSSender: JSON sent.");
}

function oninputMasterSelect(e) {
  console.log("oninputMasterSelect: starting");
  var _laserBoxNumber = findUpLaserBoxNumber(this.parentNode);
  if ((_laserBoxNumber !== null )) {
    console.log("oninputMasterSelect: slave box: " + (_laserBoxNumber + 200));
    console.log("oninputMasterSelect: master box " + this.options[this.selectedIndex].value);
    var _json = JSON.stringify({
      action: "changeBox",
      key: "masterbox",
      lb: _laserBoxNumber,
      val: parseInt(this.options[this.selectedIndex].value, 10)
     });
     //   // _obj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3} // boxState // ancient 4
     //   // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4} // masterbox // ancient 8
     //   // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3} // boxDefstate // ancient 9
    console.log("oninputMasterSelect: about to send json via WS: " + _json);
    ws.send(_json);
    console.log("oninputMasterSelect: json sent.");
  }
  console.log("oninputMasterSelect: ending");
}
// END EVENT HANDLERS




// DOM MANIPULATION
function updateStationIp(_stationIp) {
  document.getElementById('stationIp').innerHTML = _stationIp;
}




function updateStateButton(_data) {
  // _data = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 1} // boxState // ancient 4
  // _data = {lb: 1; action: "changeBox"; key: "boxState"; val: 6; st: 2}
  // _data = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate // ancient 9
  // _data = {lb:1; action: "changeBox"; key: "boxDefstate"; val: 4; st: 2}
  console.log("updateStateButton: laser box [" + _data.lb + "]; stateButton: " + _data.key + ".");
  // select the correct row in the map
  var _boxRow = boxesRows.get(_data.lb);
  var _requestStatus = parseInt(_data.st, 10);

  if (_requestStatus === 1) {
    updateClickedStateButton(_boxRow, _data.key, _data.val);
    return;
  }
  if (_requestStatus === 2) {
    // 1. remove classes on all the others stateButtons of the same type of this boxRow
    // 2. add button_active_state class to the relevant stateButton
    // _boxRow = updateCurrentStateButton(_boxRow, datasetKey, datasetValue);
    // _boxRow = updateCurrentStateButton(_boxRow, "boxDefstate", data.val);
    updateCurrentStateButton(_boxRow, _data.key, _data.val);
    return;
  }

  console.log("updateStateButton: ending after updating laser box [" + _data.lb + "]");
}




/*
  This function adds a red border (by applying class button_change_received)
  to any button that has been touched and for which the browser has received
  by WS a response from the server saying that the corresponding request is
  currently being processed.
*/
function updateClickedStateButton(_boxRow, _stateTypeSelector, _stateNumberSelector) {
  // updateClickedStateButton(_boxRow, _data.key:"boxState", _data.val:3);
  console.log("updateClickedStateButton starting. _stateTypeSelector = " + _stateTypeSelector + "; _stateNumberSelector = " + _stateNumberSelector + "; _boxRow: ");console.log(_boxRow);
  // --> updateClickedStateButton starting. _stateTypeSelector = boxState; _stateNumberSelector = 3.;
  var _elt = _boxRow.querySelector("button[data-" + _stateTypeSelector + "='" + _stateNumberSelector + "']");
  console.log(_elt);
  if (_elt) {
    _elt.classList.add('button_change_received');
  }
  console.log("updateClickedStateButton ending.");
}




// function updateDefaultStateButton(data) {
//   console.log("updateDefaultStateButton: default state for laser box [" + data.lb + "] has changed.");
//   // select the correct row in the map
//   var _boxRow = boxesRows.get(data.lb);
//
//   // 1. remove classes on all the others default stateButtons of this boxRow
//   // 2. add button_active_state class to the relevant default stateButton
//   // _boxRow = updateCurrentStateButton(_boxRow, datasetKey, datasetValue);
//   // _boxRow = updateCurrentStateButton(_boxRow, "boxDefstate", data.val);
//   _boxRow = updateCurrentStateButton(_boxRow, data.key, data.val);
//
//   console.log("updateDefaultStateButton: ending after updating laser box [" + data.lb + "]");
// }




function updateBoxRow(_data) {
  // _data = {lb:1; action: "addBox"; boxState: 3; masterbox: 4; boxDefstate: 6}
  console.log("updateBoxRow: a boxRow for laser box [" + _data.lb + "] already exists in DOM.");
  // select the correct row in the map
  var _boxRow = boxesRows.get(_data.lb);

  // update the current active and default states
  updateCurrentStateButtons(_data, _boxRow);

  // update the master select
  _indicateMasterBoxNumber(_data.masterbox, _boxRow);

  console.log("updateBoxRow: ending after updating laser box [" + _data.lb + "]");
}





function updateCurrentStateButtons(_data, _boxRow) {
  // _data = {lb:1; action: "addBox"; boxState: 3; masterbox: 4; boxDefstate: 6}
  console.log("updateCurrentStateButtons starting.");
  console.log("updateCurrentStateButtons: About to set the activeState and defaultState buttons.");

  // 1. remove classes on all the others stateButtons of this boxRow
  // 2. add button_active_state class to the relevant stateButton
  // _boxRow = updateCurrentStateButton(_boxRow, datasetKey, datasetValue);
  _boxRow = updateCurrentStateButton(_boxRow, "boxstate", _data.boxState);

  // 1. remove classes on all the others default stateButtons of this boxRow
  // 2. add button_active_state class to the relevant default stateButton
  // _boxRow = updateCurrentStateButton(_boxRow, datasetKey, datasetValue);
  _boxRow = updateCurrentStateButton(_boxRow, "boxDefstate", _data.boxDefstate);

  console.log("updateCurrentStateButtons: ending after updating laser box [" + _data.lb + "]");
}





function updateCurrentStateButton(_boxRow, datasetKey, datasetValue) {
  // 1. remove classes on all the others stateButtons of this boxRow
  // 2. add button_active_state class to the relevant stateButton
  console.log("updateCurrentStateButton starting.");
  console.log("updateCurrentStateButton: datasetKey = " + datasetKey + "; datasetValue = " + datasetValue + "; _boxRow = ");console.log(_boxRow);

  // remove classes on all the others stateButtons/defaultStateButtons of this boxRow
  // _removeClassesOnButtonsGroupForRow(_boxRow, _buttonsSelector)
  _removeClassesOnButtonsGroupForRow(_boxRow, "button[data-" + datasetKey + "]");

  // add button_active_state class to the relevant active / default stateButton
  // _setCurrentStateButton(memRow, datasetKey, datasetValue)
  _boxRow = _setCurrentStateButton(_boxRow, datasetKey, datasetValue);

  console.log("updateCurrentStateButton: ending.");

  return _boxRow;
}





function _removeClassesOnButtonsGroupForRow(_boxRow, _buttonsSelector) {
  console.log("_removeClassesOnButtonsGroupForRow starting: _buttonsSelector: " + _buttonsSelector + "; _boxRow: ");console.log(_boxRow);
  var _buttonList = boxRowEltsGroupSelector(_boxRow, _buttonsSelector);
  console.log("_removeClassesOnButtonsGroupForRow: array of all the buttons related to this boxRow available = ");console.log(_buttonList);
  if (_buttonList && _buttonList.length) {
    _buttonList.forEach(
      function(currentValue, currentIndex, listObj) {
        currentValue.classList.remove('button_active_state');
        currentValue.classList.remove('button_change_received');
        currentValue.classList.remove('button_clicked');
      }
    );
  }
  console.log("_removeClassesOnButtonsGroupForRow ending.");
}





function _setCurrentStateButton(memRow, datasetKey, datasetValue) {
  // console.log("_setCurrentStateButton: preparing a selector to select the state buttons included in _dupRow.");
  var _buttonSelector = "button[data-" + datasetKey + "='" + datasetValue + "']";
  console.log("_setCurrentStateButton: selector created: '" + _buttonSelector + "'");
  memRow = _setStateButtonAsActive(_buttonSelector, memRow);
  return memRow;
}




function _setStateButtonAsActive(_selector, memRow) {
  var _targetButton = memRow.querySelector(_selector);
  console.log("_setStateButtonAsActive: button selected: ");console.log(_targetButton);
  if (_targetButton) {
    console.log("_setStateButtonAsActive: about to add the active class to selected button");
    _targetButton.classList.add('button_active_state');
    _targetButton.classList.remove('button_change_received');
    _targetButton.classList.remove('button_clicked');
  }
  console.log("_setStateButtonAsActive: ending on returning row");
  return memRow;
}





function _newBoxRowSetProperties(_laserBoxIndexNumber, _dupRow) {
  console.log("_newBoxRowSetProperties: _dupRow: setting the id of the new wrapper div to: " + _laserBoxIndexNumber);
  _dupRow.id = "boxRow" + _laserBoxIndexNumber;     // set a unique id
  console.log("_newBoxRowSetProperties: _dupRow: setting the data-lb property of the new wrapper div to: " + _laserBoxIndexNumber);
  _dupRow.dataset.lb = _laserBoxIndexNumber;     // update data-lb attribute
  console.log("_newBoxRowSetProperties: _dupRow: removing the class hidden from the classes of the new wrapper div");
  _dupRow.classList.remove('hidden');
  console.log("_newBoxRowSetProperties: _dupRow: setting the laser box number: " + (_laserBoxIndexNumber + 200));
  _dupRow.querySelector("span.box_num").textContent = _laserBoxIndexNumber + 200;

  console.log("_newBoxRowSetProperties: _dupRow: setting the reboot and save buttons for this box");
  _dupRow.querySelector("#rebootBox").addEventListener('click', onclickRebootBoxButton, false);
  _dupRow.querySelector("#rebootBox").id = "rebootBox" + _laserBoxIndexNumber;     // set a unique id
  _dupRow.querySelector("#rebootAndSaveBox").addEventListener('click', onclickRebootAndSaveBoxButton, false);
  _dupRow.querySelector("#rebootAndSaveBox").id = "rebootBox" + _laserBoxIndexNumber;     // set a unique id
  _dupRow.querySelector("#savePrefsBox").addEventListener('click', onclickSavePrefsBoxButton, false);
  _dupRow.querySelector("#savePrefsBox").id = "rebootBox" + _laserBoxIndexNumber;     // set a unique id
  return _dupRow;
}




function _setEVentListenersOnGroupOfButtons(_dupRow, _eventHandler, _buttonGroupSelector) {
  console.log("_setEVentListenersOnGroupOfButtons: about to set event listeners on buttons");
  console.log("_setEVentListenersOnGroupOfButtons: _buttonGroupSelector = " + _buttonGroupSelector);
  var _buttonList = boxRowEltsGroupSelector(_dupRow, _buttonGroupSelector);
  console.log("_setEVentListenersOnGroupOfButtons: _buttonList selected");
  console.log(_buttonList);
  console.log("_setEVentListenersOnGroupOfButtons: about to call setButtonsGroupEvents");
  setButtonsGroupEvents(_buttonList, _eventHandler);
  return _dupRow;
}





function _indicateMasterBoxNumber(_masterBoxIndexNumber, _dupRow) {
  console.log("_indicateMasterBoxNumber: starting. About to write masterbox number");

  // Write box number in box number span
  var _span = selectMasterBoxNumberSpan(_dupRow);
  writeMasterBoxNumberInBoxNumberSpan(_span, _masterBoxIndexNumber);

  // Select corresponding option in masterBoxSelect
  var _select = _selectMasterSelectInRow(_dupRow);
  console.log("_indicateMasterBoxNumber: About to select correct option in master select");
  _select.value = parseInt(_masterBoxIndexNumber, 10);
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
  // Map.prototype.set(key, value)
  // {1:4;2:3;etc.}
  console.log("_addToMaps: controlerBoxes map: set key [" + data.lb + "] with value [" + data.boxState +  "] in controlerBoxes map.");
  console.log(controlerBoxes);
  boxesRows.set(data.lb, _dupRow);
  console.log("_addToMaps: boxesRows map: set key [" + data.lb + "] with value [" + _dupRow +  "] in boxesRows map.");
  console.log(boxesRows);
  console.log("_addToMaps ending.");
}




function _deleteFromMaps(boxNumber) {
  console.log("_deleteFromMaps starting.");
  controlerBoxes.delete(boxNumber);
  boxesRows.delete(boxNumber);
  console.log("_deleteFromMaps ending.");
}





function addNewRowForNewBox(data) {
  // _data = {lb:1; action: "addBox"; boxState: 3; masterbox: 4; boxDefstate: 6}
  console.log("addNewRowForNewBox: Starting: the boxRow does not already exist. I am about to create it.");
  if (boxRowTemplate) {
    // clone the template
    console.log("addNewRowForNewBox: Hidden boxRow selected. About to clone it.");
    var _dupRow = boxRowTemplate.cloneNode(true);  // duplicate the box
    console.log("addNewRowForNewBox: Clone _dupRow created");//console.log(_dupRow);

    // set properties
    _dupRow = _newBoxRowSetProperties(data.lb, _dupRow);

    // set the activeState button
    // _setCurrentStateButton(memRow, datasetKey, datasetValue)
    _dupRow = _setCurrentStateButton(_dupRow, "boxstate", data.boxState);

    // set event listener on current state buttons
    // _setEVentListenersOnGroupOfButtons(_dupRow, _eventHandler, _buttonGroupSelector);
    _dupRow = _setEVentListenersOnGroupOfButtons(_dupRow, onclickButton, "button[data-boxstate]");

    // indicate masterbox number
    _dupRow = _indicateMasterBoxNumber(data.masterbox, _dupRow);

    // set event listener on master select
    var _select = _selectMasterSelectInRow(_dupRow);
    setSelectEvents(_select);

    // set boxDefaultState button
    // _setCurrentStateButton(memRow, datasetKey, datasetValue)
    _dupRow = _setCurrentStateButton(_dupRow, "boxDefstate", data.boxDefstate);

    // set event listener on default state buttons
    // _setEVentListenersOnGroupOfButtons(_dupRow, _eventHandler, _buttonGroupSelector);
    _dupRow = _setEVentListenersOnGroupOfButtons(_dupRow, onclickDefStateButton, "button[data-boxDefstate]");

    // render in DOM
    _dupRow = _renderInDom(_dupRow);

    // add a key/entry pair to the controlerBoxes map and to the rowsMap map
    _addToMaps(data, _dupRow);
  }
  console.log("addNewRowForNewBox: ending after adding laser box [" + data.lb + "]");
}






function addOrUpdateNewRowForNewBox(data) {
  // _data = {lb:1; action: "addBox"; boxState: 3; masterbox: 4; boxDefstate: 6}
  console.log("addOrUpdateNewRowForNewBox starting.");

  // Check whether the boxRow has not already been created
  var _controlerBoxEntry = controlerBoxes.get(data.lb);
  console.log("addOrUpdateNewRowForNewBox: looking if an entry exists in the map for this box");
  console.log("addOrUpdateNewRowForNewBox _controlerBoxEntry (if undefined, the entry does not exist): " + _controlerBoxEntry);
  console.log("addOrUpdateNewRowForNewBox: testing if (!(_controlerBoxEntry === undefined)): " + (!(_controlerBoxEntry === undefined)));

  // Case where the box does not exist
  if (_controlerBoxEntry === undefined) {
    // _controlerBoxEntry is equal to undefined: the boxRow does not already exists
    // let's create it
    addNewRowForNewBox(data);
    // _data = {lb:1; action: "addBox"; boxState: 3; masterbox: 4; boxDefstate: 6}
  }

  // Case where the box exists
  else {
    // _controlerBoxEntry is not equal to undefined, the boxRow already exists
    // let's update it instead
    updateBoxRow(data);
  }
}




function deleteAllBoxRows() {
  // delete from maps represenations
  if (controlerBoxes.size) {
    controlerBoxes.clear();
  }
  if (boxesRows.size) {
    boxesRows.clear();
  }
  // delete from DOM
  var _boxRowsContainer = document.querySelector(".boxes_state_setter");
  while (_boxRowsContainer.children[1]) {
    _boxRowsContainer.removeChild(_boxRowsContainer.firstChild);
  }
}




function deleteBoxRow(data) {
  console.log("deleteBoxRow starting.");
  var _boxRowToDelete = boxesRows.get(data.lb);
  if (_boxRowToDelete === undefined) {
    console.log("deleteBoxRow: There was no laser box [" + data.lb + "] in controlerBoxes map.");
  } else {
    console.log("deleteBoxRow: About to delete row corresponding to laser box [" + data.lb + "] in DOM and maps.");
    _boxRowToDelete.parentNode.removeChild(_boxRowToDelete);
    _deleteFromMaps(data.lb); // updating the controlesBoxes map
    console.log("deleteBoxRow: deleted key [" + data.lb + "] in controlerBoxes and boxesRows maps.");
    console.log(controlerBoxes);
  }
  console.log("deleteBoxRow ending.");
}




function selectMasterBoxNumberSpan(_dupRow) {
  console.log("selectMasterBoxNumberSpan: starting.");
  var _masterBoxNumberSelector = "span.master_box_number";
  console.log("selectMasterBoxNumberSpan: _masterBoxNumberSelector = " + _masterBoxNumberSelector);
  console.log("selectMasterBoxNumberSpan: ending.");
  return _dupRow.querySelector(_masterBoxNumberSelector);
}




function writeMasterBoxNumberInBoxNumberSpan(_span, _masterBoxIndexNumber) {
  console.log("writeMasterBoxNumberInBoxNumberSpan: starting.");
  _span.textContent = _masterBoxIndexNumber + 200;
  // _row.children[1].children[0].textContent = data.ms + 200;
  console.log("writeMasterBoxNumberInBoxNumberSpan: masterbox span updated: " + (_masterBoxIndexNumber + 200));
  console.log("writeMasterBoxNumberInBoxNumberSpan: ending.");
}




function updateMasterBoxNumber(_data) {
  // _data = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 1} // masterbox // ancient 8
  // _data = {lb: 1; action: "changeBox"; key: "masterbox"; val: 9; st: 2}
  console.log("updateMasterBoxNumber starting.");

  // select the relevant row
  var _row = boxesRows.get(_data.lb);
  console.log("updateMasterBoxNumber: selected row: " + _data.lb);

  // write box number in box number span
  var _span = selectMasterBoxNumberSpan(_row);
  writeMasterBoxNumberInBoxNumberSpan(_span, _data.val);

  if (_data.st === 1) {
    _span.classList.add("change_ms_received");
    console.log("updateMasterBoxNumber: added class change_ms_received to masterbox span");
    return;
  }
  if (_data.st === 2) {
    // update the number mentionned in html
    _span.classList.remove("change_ms_received");
    console.log("updateMasterBoxNumber: removed class change_ms_received to masterbox span");
    _span.classList.add("change_ms_executed");
    console.log("updateMasterBoxNumber: added class change_ms_executed to masterbox span");

    // update the select by choosing the correct option
    var _select = _selectMasterSelectInRow(_row);
    console.log("updateMasterBoxNumber: About to select correct option in master select");
    _select.value = parseInt(_data.val, 10);
  }
}





function boxRowDOMSelector(laserBoxIndexNumber) {
  console.log("boxRowDOMSelector starting.");
  // div[data-lb='1'] button[data-boxstate='1'
  // var _selector = "div[data-lb='"+ laserBoxIndexNumber + "']"; // selector for the whole div

  // div.box_wrapper[data-lb=X] > div.box_state_setter > div.setters_group > button
  var _selector = "div.box_wrapper[data-lb='" + laserBoxIndexNumber + "']";
  console.log(_selector);
  var _row = document.querySelector(_selector); // should be a list composed of one single element
  console.log("boxRowDOMSelector ending.");
  return _row; // return the first (and unique) element of the list
}




function boxRowTemplateSelector() {
  console.log("boxRowTemplateSelector starting.");
  var _row = document.getElementById('boxTemplate');
  var _templateDup = _row.cloneNode(true);

  console.log("boxRowTemplateSelector ending.");
  return _templateDup; // return the first (and unique) element of the list
}




function boxRowEltsGroupSelector(_boxRow, _buttonsSelector) {
  console.log("boxRowEltsGroupSelector starting: _buttonsSelector = " + _buttonsSelector + "; _boxRow = ");console.log(_boxRow);
  var _elts = _boxRow.querySelectorAll(_buttonsSelector);
  console.log(_elts);
  console.log("boxRowEltsGroupSelector ending.");
  return _elts;
}
// END DOM MANIPULATION





// EVENT LISTENERS
function setButtonsGroupEvents(buttonList, eventHandler) {
  // iterate over each buttons and add an eventListener on click
  for (var i = 0; i < buttonList.length; i++) {
    buttonList[i].addEventListener('click', eventHandler, false);
  }
}

function setSelectEvents(selectElt) {
  selectElt.addEventListener('input', oninputMasterSelect, false);
}

function setGroupEvents() {
  document.getElementById("rebootLBs").addEventListener('click', onclickRebootLBsButton, false);
  document.getElementById("rebootIF").addEventListener('click', onclickRebootIFButton, false);
  document.getElementById("rebootAll").addEventListener('click', onclickRebootAllButton, false);
  document.getElementById("saveLBs").addEventListener('click', onclickSaveLBsButton, false);
  document.getElementById("saveIF").addEventListener('click', onclickSaveIFButton, false);
  document.getElementById("saveAll").addEventListener('click', onclickSaveAllButton, false);
}
// END EVENT LISTENERS





// WINDOW LOAD
window.onload = function(e){
    console.log("window.onload");
    // Interval at which to check if WS server is still available
    // (and reconnect as necessary) setInterval(check, 5000);
    setInterval(check, (getRandomArbitrary(10, 4) * 1000));
    setTimeout(setGroupEvents, 2000);
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
