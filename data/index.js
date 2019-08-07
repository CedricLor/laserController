/* TODO URGENTLY
- OTA update for the laser boxes
- indicate the name of the master on loading a new box
- feedback on save and reboot individual boxes
- force reboot button
- when a box disconnects and it is on a second layer, the intereface does not detect it
  -> check what is happening in the callbacks
- I guess the problem is the same when a box connects (if it connect to a box other
  than the IF, the IF does not detect it).
*/

// Global variables
var ws = null;
var controlerBoxes = new Map();
var boxesRows = new Map();
var boxRowTemplate = boxRowTemplateSelector();








// WEB SOCKET
function connect() {
  console.log("connect() starting.");
  ws = new WebSocket('ws://'+ self.location.host +'/');


  // onopen websocket,
  // send a message to the server with the list of controlerBoxes
  // currently in the DOM and their states
  ws.onopen = function() {
    console.log("connect(): WS connection open.");
    checkConnect.retryCount = 0;
    checkConnect.deleteNotConnectedMsg();
    // console.log("connect(): checkConnect.retryCount = " + checkConnect.retryCount + ".");
    console.log("Sending the server the list of controlerBoxes I have in the DOM (and their current state)");
    ws.send(JSON.stringify({
      action: "handshake",
      boxesStatesInDOM: mapToObj(controlerBoxes)
    }));
    // {action:0, boxStateInDOM:{1:4;2:3}}
  };


  // on receive a message,
  // decode its action type to dispatch it
  ws.onmessage = function(e) {
    ping.check = 0;
    // if ping pong message
    if (ping.onPingMsg(e)) {return;}
    // if other messages, parse JSON
    console.log( "WS Received Message: " + e.data);
    var _data = JSON.parse(e.data);
    onMsgActionSwitch(_data);
  };


  // onclose,
  //  inform the user that an attempt to reconnect
  // will be made soon and delete all the boxRows
  ws.onclose = function(e) {
    if (checkConnect.retryCount != 10) {
      console.log('Socket is closed. Reconnect will be attempted in 4 to 10 seconds. Reason: ', e.reason);
    }
    console.log('Socket is closed. Reason: ', e.reason);
    deleteAllBoxRows();
  };


  // onerror,
  // inform the user that you are closing the socket
  ws.onerror = function(err) {
    console.error('Socket encountered error: ', err.target.readyState, 'Closing socket');
    ws.close();
  };
}
// WEB SOCKET END







// ON MESSAGES EVENT HANDLERS
function sendReceivedIP() {
  ws.send(JSON.stringify({
    action: "ReceivedIP"
  }));
  // {action:"ReceivedIP}
}



//////////////////////////////////
// onRebootLBs Object
//////////////////////////////////
var onRebootLBs = {
  active: false,
  rebootBtnId: 'rebootLBs',

  waitingLBs: new Map(),
  rebootingLBs: new Map(),
  rebootedLBs: new Map(),


  onDeleteBox: function(_data) {
    if (this.active === true) {
      // store the number in a form accessible for the maps
      let _laserBoxIndexNumber = parseInt(_data.lb, 10);

      // if this is the first box to disconnect
      if (this.rebootingLBs.size === 0) {
        // change the button color to signal that it has started
        _onRebootCommon.turnBtnRed('rebootLBs');
        // create the info text and box
        _onRebootCommon.onFirstBox(_laserBoxIndexNumber, "divRebootingLBs", 'Laser boxes currently rebooting: ', "spanRebootingLBs", onRebootLBs.waitingLBs, this.rebootingLBs);
      } // this is not the first box to be deleted
      else {
        // add its number to the info box
        _onRebootCommon.onAdditionalBoxes(_laserBoxIndexNumber, '#spanRebootingLBs', this.rebootingLBs, onRebootLBs.waitingLBs);
      }

      // delete the box from the waitingLBs map
      onRebootLBs.waitingLBs.delete(_laserBoxIndexNumber);

      if (boxesRows.size === 0) {
        // remove the waiting LBs div
        removeDOMelement('#divLBsWaitingToReboot');
      }
    }
  },


  startConfirm: function() {
    // save the fact that we are in rebooting
    this.active = true;

    // change the button color
    let _rebootBtn = document.getElementById('rebootLBs');
    _rebootBtn.className += ' button_change_received';

    _onRebootCommon.startConfirm("divLBsWaitingToReboot", 'Boxes waiting to reboot: ', "spanLBsWaitingToReboot", onRebootLBs.waitingLBs);

    console.log("--------------- end LBs reboot switch -----------------");
  }
};





var onRebootAll = {
  active: false,
  rebootBtnId: 'rebootAll',

  rebootingLBs: new Map(),
  rebootedLBs: new Map(),


  startConfirm: function() {
    // save the fact that we are in rebooting
    this.active = true;

    _onRebootCommon.turnBtnRed('rebootAll');

    _onRebootCommon.startConfirm("divRebootingLBs", 'Rebooting boxes: ', "spanRebootingLBs", onRebootAll.rebootingLBs);

    // specific to rebootAll
    // close the connection
    ws.close();
    // delete all the boxes
    deleteAllBoxRows();

    // shut down any eventual onRebootLBs active status
    onRebootLBs.active = false;
    // clear all the eventual remaining values in the onRebootLBs maps
    onRebootLBs.waitingLBs.clear();
    onRebootLBs.rebootingLBs.clear();
    onRebootLBs.rebootedLBs.clear();

    console.log("--------------- end reboot all switch -----------------");
  }
};




var _onRebootCommon = {

  noConnectedBoxesSpan: false,


  deleteNoConnectedBoxesSpan: function() {
    infoBox.deleteMsg('#infoNoBoxesConnected', this.noConnectedBoxesSpan);
  },


  addNoConnectedBoxesSpan: function() {
    if (!this.noConnectedBoxesSpan) {
      infoBox.addMsg("There are no laser boxes currently connected.", "infoNoBoxesConnected", "span");
    }
    this.noConnectedBoxesSpan = true;
  },


  onFirstBox: function( _laserBoxIndexNumber,
                        _msgDivId /*"divRebootedLBs"*/,
                        _msgIntro /*'Laser boxes currently rebooted: '*/,
                        _msgSpanId /*"spanRebootedLBs"*/,
                        _originMapSet /*this.rebootingLBs*/,
                        _destinationMapSet /*rebootedLBs*/) {

    // check whether there was a message saying there was no connected boxes
    // and delete it if necessary
    this.deleteNoConnectedBoxesSpan();

    // create a div to hold the infos
    let _divNewStateLBs = this.createBoxListContainerDiv(_msgDivId, _msgIntro);

    // create a span to hold the numbers of the rebooted LBs
    let _spanNewStateLBsList = infoBox.createContainer("span", _msgSpanId);
    console.log("onFirstBox: _msgSpanId = " + _msgSpanId);
    console.log("onFirstBox: _spanNewStateLBsList = ");
    console.log(_spanNewStateLBsList);

    // add the box number to the span
    _spanNewStateLBsList = this.mapToMapTransferAndInsertBoxList(_spanNewStateLBsList, _laserBoxIndexNumber, _msgSpanId, _destinationMapSet, _originMapSet);

    // append the span and the div to the DOM
    infoBox.renderInDom(_divNewStateLBs, _spanNewStateLBsList);
  },


  onAdditionalBoxes: function(_laserBoxIndexNumber, _destinationSpanId /*'#spanRebootingLBs'*/, _destinationMapSet, _originMapSet) {
    // select the infoBox
    let _spanNewStateLBsList = document.querySelector(_destinationSpanId);
    console.log("onFirstBox: _msgSpanId = " + _destinationSpanId);
    console.log("onFirstBox: _spanNewStateLBsList = ");
    console.log(_spanNewStateLBsList);
    this.mapToMapTransferAndInsertBoxList(_spanNewStateLBsList, _laserBoxIndexNumber, _destinationSpanId, _destinationMapSet, _originMapSet);
  },


  onAddBox: function(_data){

    var _onRebootTypeObj;

    if (onRebootAll.active) {
      _onRebootTypeObj = onRebootAll;
    } else if (onRebootLBs.active) {
      _onRebootTypeObj = onRebootLBs;
    } else {
      return;
    }

    if (_onRebootTypeObj.active) {
      // store the number in a form accessible for the maps
      let _laserBoxIndexNumber = parseInt(_data.lb, 10);

      if (_onRebootTypeObj.rebootedLBs.size === 0) {
        _onRebootCommon.onFirstBox(_laserBoxIndexNumber, "divRebootedLBs", 'Laser boxes currently rebooted: ', "spanRebootedLBs", _onRebootTypeObj.rebootingLBs, _onRebootTypeObj.rebootedLBs);
      } else {
        _onRebootCommon.onAdditionalBoxes(_laserBoxIndexNumber, '#spanRebootedLBs', _onRebootTypeObj.rebootedLBs, _onRebootTypeObj.rebootingLBs);
      }

      _onRebootTypeObj.rebootingLBs.delete(_laserBoxIndexNumber);

      if (_onRebootTypeObj.rebootingLBs.size === 0) {
        // change the button color
        let _rebootLbsBtn = document.getElementById(_onRebootTypeObj.rebootBtnId);
        _rebootLbsBtn.classList.remove('button_active_state');

        // delete the rebootingLBs div
        removeDOMelement('#divRebootingLBs');
        // delete the rebootedLBs div
        removeDOMelement('#divRebootedLBs');

        // create a span to hold the textnode informing that all the boxes have rebooted
        let _spanLBsHaveRebooted = infoBox.createContainer("span", "LBsHaveRebooted");
        // create a text node for the introduction text
        var _infoText = document.createTextNode('All the laser boxes have rebooted.');

        // render in dom
        infoBox.renderInDom(_spanLBsHaveRebooted, _infoText);

        // set a timeout to delete the info box after 15 seconds
        window.setTimeout(function() {
          removeDOMelement('#LBsHaveRebooted');
        }, 15000);

        // empty the rebooted boxes maps
        _onRebootTypeObj.rebootedLBs.clear();

        // get out of the reboot process
        _onRebootTypeObj.active = false;
      }
    }
  },

  startConfirm: function(_divRebootingBoxListId, _introMsg, _spanRebootingBoxListId, _mapRebootingBoxes) {

    // check whether there was a message saying there was no connected boxes
    // and delete it if necessary
    this.deleteNoConnectedBoxesSpan();

    let _divRebootingBoxList = this.createBoxListContainerDiv(_divRebootingBoxListId, _introMsg);

    // create a span to hold the numbers of the LBs
    let _spanRebootingBoxList = infoBox.createContainer("span", _spanRebootingBoxListId);

    _spanRebootingBoxList = this.createBoxTextNodesFromBoxRows(_spanRebootingBoxList, _mapRebootingBoxes);

    infoBox.renderInDom(_divRebootingBoxList, _spanRebootingBoxList);
  },


  createBoxTextNodesFromBoxRows: function(_spanLBsRebooting, _mapRebootingBoxes) {
    // iterate over the boxesRows map, create textNodes for the span and
    // store them in a map
    boxesRows.forEach(function(val, key) {
      let _text = (parseInt(key) + 200) + ". ";
      // create a textNode to hold the box number
      let _boxNumbNode = document.createTextNode(_text);
      // store it into a map
      _mapRebootingBoxes.set(key, _boxNumbNode);
      // add the new textNode to the span
      // _spanLBsRebooting.appendChild(onRebootLBs.waitingLBs.get(key));
      _spanLBsRebooting.appendChild(_mapRebootingBoxes.get(key));
    });
    return _spanLBsRebooting;
  },


  createBoxListContainerDiv: function(_containerId, _infoText) {
    // create a div to hold the infos
    let _containerDiv = infoBox.createContainer("div", _containerId);
    // create a text node for the introduction text
    let _infoTextNode = document.createTextNode(_infoText);
    // append it to the div
    _containerDiv.appendChild(_infoTextNode);
    return _containerDiv;
  },


  turnBtnRed: function(_btnId) {
    // change the button color to signal that it has started
    let _rebootBtn = document.getElementById(_btnId);
    _rebootBtn.classList.add('button_active_state');
    _rebootBtn.classList.remove('button_clicked', 'button_change_received');
  },

  mapToMapTransferAndInsertBoxList: function(_spanNewStateLBsList, _laserBoxIndexNumber, _destinationSpanId, _destinationMapSet, _originMapSet){
    // transfer the box from the waitingLBs map to the this.rebootingLBs map
    _destinationMapSet.set(_laserBoxIndexNumber, _originMapSet.get(_laserBoxIndexNumber));
    // add the text node to the span
    _spanNewStateLBsList.appendChild(_destinationMapSet.get(_laserBoxIndexNumber));
    return _spanNewStateLBsList;
  }
};




function onMsgActionSwitch(_data) {
  // Received IP and other global data (wifi settings)
  if (_data.action === 3) {
    // console.log("WS JSON message: " + _data.ServerIP);
    // Fill in the data in the DOM and add some eventHandlers
    updateGlobalInformation(_data);
    sendReceivedIP();
    return;
  }


  // 4. User request to change boxState of a given box has been received
  // and is being processed
  // 5. boxState of existing box has been updated
  if (_data.action === "changeBox" && _data.key === "boxState") {
    // _data = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 1} // boxState // ancient 4
    // _data = {lb: 1; action: "changeBox"; key: "boxState"; val: 6; st: 2}
    updateStateButton(_data);
    return;
  }


  // 6. a new box has connected to the mesh
  if (_data.action === "addBox") {
    console.log("---------------- addBox switch starting -----------------");
    // _data = {lb:1; action: "addBox"; boxState: 3; masterbox: 4; boxDefstate: 6}
    addOrUpdateNewRowForNewBox(_data);
    return;
  }


  // 7. an existing box has been disconnected from the mesh
  // or the DOM contained boxRows corresponding to boxes that
  // have been disconnected from the mesh
  if (_data.action === "deleteBox") {
    console.log("---------------- delete switch starting -----------------");
    // delete all the boxes
    if (_data.lb === 'a') {
      // _data = {action: "deleteBox"; lb: "a"}
      deleteAllBoxRows();
      return;
    }
    // if delete one box
    // _data = {lb:1; action:"deleteBox"}
    deleteBoxRow(_data);
    return;
  }


  // 8. a box has changed master
  if (_data.action === "changeBox" && _data.key === "masterbox") {
    // _data = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 1} // masterbox // ancient 8
    // _data = {lb: 1; action: "changeBox"; key: "masterbox"; val: 9; st: 2}
    updateMasterBoxNumber(_data);
    return;
  }


  // 9. User request to change default boxState of a given box has been received
  // and is being processed
  // 10. the default state of a given box has changed
  if (_data.action === "changeBox" && _data.key === "boxDefstate") {
    // _data = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate // ancient 9
    // _data = {lb:1; action: "changeBox"; key: "boxDefstate"; val: 4; st: 2}
    updateStateButton(_data);
    return;
  }



  if (_data.action === "changeBox" && _data.key === "reboot" && _data.lb === (0 || "all")) { // 9. User request to reboot the interface or all the boxes has been received and is being processed
    // _data = {lb:1; action: "changeBox"; key: "reboot"; val: 0; lb: 0 st: 1}
    onRebootAll.startConfirm();
    return;
  }



  if (_data.action === "changeBox" && _data.key === "reboot" && _data.lb === "LBs") { // User request to reboot the LBs has been received and is being processed
    console.log("---------------- rebootStart switch starting -----------------");
    onRebootLBs.startConfirm();
    return;
  }
}

// ON MESSAGE EVENT HANDLERS END

// ping and reconnect
var ping = {
  count: 1,
  sentMark: 1,
  receivedMark: 1,
  check: 0,

  onPingMsg: function(e) {
    ping.check = 0;
    if ((e.data > 0) && (e.data < 10)) {
      ping.receivedMark = e.data;
      return true;
    }
    return false;
  },

  checkPingStatus: function(){
    // console.log("check(): ping.sentMark === " + ping.sentMark);
    // console.log("check(): ping.receivedMark === " + ping.receivedMark);
    if (ping.sentMark != ping.receivedMark) {
      console.log("check(): not receiving server pongs");
      console.log("check(): about to close connection");
      ws.close();
      ping.count = 1;
      ping.receivedMark = 1;
      ping.sentMark = 1;
      return;
    }

    ping.check++;
    if (ping.check === 3) {
      ping.check = 0;

      // try sending a numbered ping to the server
      ping.count++;
      if (ping.count === 9) {
        ping.count = 1;
      }
      ws.send(ping.count);
      ping.sentMark = ping.count;
    }
  }
};

// check connection and reconnect variables
var checkConnect = {
  intervalCode: -1,
  retryCount: 0,
  notConnectedSpan: false,

  addNotConnectedMsg: function() {
    infoBox.addMsg("No connection.", "infoNotConnected", "span");
    this.notConnectedSpan = true;
  },

  deleteNotConnectedMsg: function() {
    infoBox.deleteMsg('#infoNotConnected', this.notConnectedSpan);
  },

  closedVerb: function() {
    if (!ws || ws.readyState === WebSocket.CLOSED) {
      this.addNotConnectedMsg();
      return true;
    }
    return false;
  }
};


// Check if WS server is still available (and reconnect as necessary)
function check(){
  // if checkConnect.retryCount is equal to 10, just stop trying
  // console.log("check(): --- in check");
  // console.log("check(): checkConnect.retryCount === " + checkConnect.retryCount + ".");
  if (checkConnect.retryCount === 10) {
    console.log("check(): Tried to reconnect 10 times. Stopping. Please reload or check the server.");
    clearInterval(checkConnect.intervalCode);
    // TO DO: inform the user that he should try to reload
    return;
  }

  // if the connection is inexistant or closed
  if(!ws || ws.readyState === WebSocket.CLOSED) {
    console.log("check(): NO CONNECTION. TRYING TO RECONNECT");
    console.log("check(): !!! Trying to reconnect on !ws or ws.CLOSED !!!");
    // increment the checkConnect.retryCount
    checkConnect.retryCount++;
    // try to connect
    connect();
    return;
  }

  // if the connection is open,
  // check that the server is still effectively here
  if(ws.readyState === WebSocket.OPEN) {
    ping.checkPingStatus();
  }
}












// EVENT HANDLER HELPER FUNCTIONS
var _onClickHelpers = {
  findUpLaserBoxNumber: function(el) {
      while (el.parentNode) {
          el = el.parentNode;
          if (el.dataset.lb) {
            return parseInt(el.dataset.lb, 10);
          }
      }
      return null;
  },

  btnSend: function (_obj) {
    ws.send(JSON.stringify(_obj));
  },

  updateClickButtons: function(e, _selector, _element) {
    _element.querySelectorAll(_selector).forEach(
      function(_button){
        _button.classList.remove('button_clicked');
      }
    );
    e.target.className += ' button_clicked';
  }
};



// EVENTS HANDLER
// _onClickBoxConfig Helper Object
var _onClickBoxConfig = {
  wrapper: function(e, _obj) {
    // update the buttons
    _onClickHelpers.updateClickButtons(e, 'button', e.target.parentNode); // parent node is <div class='setters_group command_gp'>
    // if the connection is closed, inform the user
    if (checkConnect.closedVerb()) { return; }
    // else, complete the message
    _obj.lb = _onClickHelpers.findUpLaserBoxNumber(e.target.parentNode);
    _obj.action = 'changeBox';
    // and send the message
    _onClickHelpers.btnSend(_obj);
    // {action:"changeBox", key:"reboot", save: 0, lb:1}
    // {action:"changeBox", key:"reboot", save: 1, lb:1}
    // {action: "changeBox", key: "save", val: "gi8RequestedOTAReboots", lb: 1, reboots: 2}
    // {action:"changeBox", key:"save", val: "all", lb:1}
  },
};


function onclickRebootBoxButton(e) {
  console.log("onclickRebootBoxButton starting");

  _onClickBoxConfig.wrapper(e, {
    key: "reboot",
    save: 0, // reboot without saving
  });
  // {action:"changeBox", key:"reboot", save: 0, lb:1}
  console.log("onclickRebootBoxButton: ending");
}


function onclickRebootAndSaveBoxButton(e) {
  console.log("onclickRebootAndSaveBoxButton starting");

  _onClickBoxConfig.wrapper(e, {
    key: "reboot",
    save: 1, // save and reboot
  });
  // {action:"changeBox", key:"reboot", save: 1, lb:1}
  console.log("onclickRebootAndSaveBoxButton: ending");
}


function onclickgOTARebootsBoxBtn(e) {
  console.log("onclickgOTARebootsBoxBtn starting");

  _onClickBoxConfig.wrapper(e, {
    key: "save",
    val: "gi8RequestedOTAReboots",
    reboots: parseInt(this.dataset.reboots, 10),
  });
  // {action: "changeBox", key: "save", val: "gi8RequestedOTAReboots", lb: 1, reboots: 2}
  console.log("onclickgi8RequestedOTAReboots ending");
}


function onclickSavePrefsBoxButton(e) {
  console.log("onclickSavePrefsBoxButton starting");

  _onClickBoxConfig.wrapper(e, {
    key: "save",
    save: "all", // save and reboot
  });
  // {action:"changeBox", key:"save", val: "all", lb:1}
  console.log("onclickSavePrefsBoxButton: ending");
}





// _onClickGroupReboot Helper Object
var _onClickGroupReboot = {
  saveObj: {
    key: "save",
    val: "all",
  },

  rebootObj: {
    key: "reboot",
    save: 0,
  },

  wrapper: function(e, _obj)/*_lbs, _save)*/ {
    // if the connection is closed, inform the user
    if (checkConnect.closedVerb()) { return; }
    // if there are boxes in the boxes map, we are probably connected, so reboot
    if (boxesRows.size) {
      _onClickHelpers.updateClickButtons(e, '.net_command_gp > button', document);
      // else, complete the message and send it
      _obj.action = 'changeNet';
      _onClickHelpers.btnSend(_obj);
      // {action: "changeNet", key: "reboot", save: 0, lb: "LBs"}
      return;
    }
    // if there are no boxes in the boxes map, inform the user that there are no boxes
    _onRebootCommon.addNoConnectedBoxesSpan();
  }
};


function onclickRebootLBsButton(e) {
  console.log("onclickRebootLBsButton starting");
  _onClickGroupReboot.wrapper(e, Object.assign(_onClickGroupReboot.rebootObj, {lb: "LBs"}));
  // {action: "changeNet", key: "reboot", save: 0, lb: "LBs"}
  console.log("onclickRebootLBsButton: ending");
}


function onclickRebootAllButton(e) {
  console.log("onclickRebootAllButton starting");
  _onClickGroupReboot.wrapper(e, Object.assign(_onClickGroupReboot.rebootObj, {lb: "all"}));
  // {action: "changeNet", key: "reboot", save: 0, lb: "all"}
  console.log("onclickRebootAllButton: ending");
}


function onclickSaveLBsButton(e) {
  console.log("onclickSaveLBsButton starting");
  _onClickGroupReboot.wrapper(e, Object.assign(_onClickGroupReboot.saveObj, {lb: "LBs"}));
  // {action: "changeNet", key: "save", val: "all", lb: "LBs"}
  console.log("onclickSaveLBsButton: ending");
}


function onclickSaveAllButton(e) {
  console.log("onclickSaveAllButton starting");
  _onClickGroupReboot.wrapper(e, Object.assign(_onClickGroupReboot.saveObj, {lb: "all"}));
  // {action: "changeNet", key: "save", val: "all", lb: "all"}
  console.log("onclickSaveAllButton: ending");
}





// Infobox Object
var infoBox = {
  createContainer: function(_type="span", _id="", _class="") {
    let _container = document.createElement(_type);
    _container.setAttribute("id", _id);
    _container.setAttribute("class", _class);
    return _container;
  },

  addMsg: function(_message, _id, _containerType="span") {
    infoBox.renderInDom(
      infoBox.createContainer(_containerType, _id),
      document.createTextNode(_message));
  },

  deleteMsg: function(_id, _stateToTest=true) {
    if (_stateToTest) {
      _stateToTest = false;
      removeDOMelement(_id);
    }
  },

  renderInDom: function(_div, _span) {
    // select the infoBox
    var _infoBox = document.querySelector('#infoBox');
    // append the span to the div
    _div.appendChild(_span);
    // append the div to the DOM
    _infoBox.appendChild(_div);
  }
};







function onclickRebootIFButton(_e) {
  console.log("onclickRebootIFButton starting");

  ws.send(JSON.stringify({
    action: "changeBox",
    key: "reboot",
    save: 0,
    lb: 0
  }));
  // {action:"changeBox", key:"reboot", save: 0, lb:0}
  console.log("onclickRebootIFButton: ending");
}






function onclickSaveIFButton(_e) {
  console.log("onclickSaveIFButton starting");

  ws.send(JSON.stringify({
    action: "changeBox",
    key: "save",
    val: "all",
    lb: 0
  }));
  // {action: "changeBox", key: "save", val: "all", lb: 0}
  console.log("onclickSaveIFButton: ending");
}





var _onClickSaveWifi = {
  obj: {
    key: "save",
    val: "wifi",
    dataset: {
      ssid: document.getElementById('ssid').value,
      pass: document.getElementById('pass').value,
      gatewayIP: document.getElementById('gatewayIP').value,
      ui16GatewayPort: parseInt(document.getElementById('ui16GatewayPort').value, 10),
      ui8WifiChannel: parseInt(document.getElementById('ui8WifiChannel').value, 10),
    }
  },
  buildObj: function(_passedObj) {
    return Object.assign(this.obj, _passedObj);
  },
  wrapper: function(e, _obj) {
    // update the buttons
    _onClickHelpers.updateClickButtons(e, 'button', e.target.parentNode); // parent node is <div class='setters_group command_gp'>
    // if the connection is closed, inform the user
    if (checkConnect.closedVerb()) { return; }
    // else, complete the message
    _obj = this.buildObj(_obj);
    // and send the message
    _onClickHelpers.btnSend(_obj);
    // {action: "changeBox", key: "save", val: "wifi", lb: 0, dataset: {ssid: "blabla", pass: "blabla", gatewayIP: "192.168.25.1", ui16GatewayPort: 0, ui8WifiChannel: 6}}
  },
};


function onclickSaveWifiSettingsIF(e) {
  console.log("onclickSaveWifiSettingsIF starting");

  _onClickSaveWifi.wrapper(e, {
      action: "changeBox",
      lb: 0,
    });
  // {action: "changeBox", key: "save", val: "wifi", lb: 0, dataset: {ssid: "blabla", pass: "blabla", gatewayIP: "192.168.25.1", ui16GatewayPort: 0, ui8WifiChannel: 6}}

  console.log("onclickSaveWifiSettingsIF ending");
}


function onclickSaveWifiSettingsAll(e) {
  console.log("onclickSaveWifiSettingsAll starting");

  _onClickSaveWifi.wrapper(e, {
      action: "changeNet",
      lb: "all",
    });
  // {action: "changeNet", key: "save", val: "wifi", lb: "all", dataset: {ssid: "blabla", pass: "blabla", gatewayIP: "192.168.25.1", ui16GatewayPort: 0, ui8WifiChannel: 6}}

  console.log("onclickSaveWifiSettingsAll ending");
}





function onclickgi8RequestedOTAReboots(_e) {
  console.log("onclickgi8RequestedOTAReboots starting");

  ws.send(JSON.stringify({
    action: "changeBox",
    key: "save",
    val: "gi8RequestedOTAReboots",
    lb: 0,
    reboots: parseInt(this.dataset.reboots, 10)
  }));
  // {action: "changeBox", key: "save", val: "gi8RequestedOTAReboots", lb: 0, reboots: 2}
  console.log("onclickgi8RequestedOTAReboots ending");
}







var _onClickStateBtns = {
  wrapper: function(e, buttonSelector, _datasetValue, _clef) {
    var _laserBoxNumber = _onClickHelpers.findUpLaserBoxNumber(e.target.parentNode);
    _onClickHelpers.updateClickButtons(e, buttonSelector, boxesRows.get(_laserBoxNumber));
    // if the connection is closed, inform the user
    if (checkConnect.closedVerb()) { return; }
    _onClickHelpers.btnSend({
      "action": "changeBox",
      "key": _clef,
      "lb": _laserBoxNumber,
      "val": parseInt(_datasetValue, 10)
    });
      // _obj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3} // boxState // ancient 4
      // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4} // masterbox // ancient 8
      // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3} // boxDefstate // ancient 9
  }
};


function onclickButton(e) {
  console.log("onclickButton starting");
  _onClickStateBtns.wrapper(e, "button[data-boxstate]", this.dataset.boxstate, "boxState");
  console.log("onclickButton: ending");
}


function onclickDefStateButton(e) {
  console.log("onclickDefStateButton starting");
  _onClickStateBtns.wrapper(e, "button[data-boxDefstate]", this.dataset.boxdefstate, "boxDefstate");
  console.log("onclickDefStateButton: ending");
}






function oninputMasterSelect(_e) {
  console.log("oninputMasterSelect: starting");
  var _laserBoxNumber = _onClickHelpers.findUpLaserBoxNumber(this.parentNode);
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
function updateGlobalInformation(_data) {
  console.log("updateGlobalInformation() starting");
  // {"action":3,"serverIP":"...","ssid":"...","pass":"...","gatewayIP":true,"...":0,"ui8WifiChannel":6}
  document.getElementById('serverIp').innerHTML = _data.serverIP;
  document.getElementById('ssid').value = _data.ssid;
  document.getElementById('pass').value = _data.pass;
  document.getElementById('gatewayIP').value = _data.gatewayIP;
  document.getElementById('ui16GatewayPort').value = _data.ui16GatewayPort;
  document.getElementById('ui8WifiChannel').value = _data.ui8WifiChannel;
  console.log("updateGlobalInformation() ending");
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
  updateCurrentStateButton(_boxRow, "boxDefstate", _data.boxDefstate);

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
      function(currentValue) {
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
  _dupRow = setEventListenersOnBoxRowElements(_dupRow, _laserBoxIndexNumber);
  console.log("_newBoxRowSetProperties: _dupRow: setting the reboot and save buttons for this box");
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
  console.log("_selectMasterSelectInRow: ending returning _select");
  return _select;
}




function _renderRowInDom(_dupRow) {
  console.log("_renderRowInDom: about to insert the new box in the DOM");
  _dupRow = document.getElementById('boxTemplate').parentNode.insertBefore(_dupRow, document.getElementById('boxTemplate'));
  console.log("_renderRowInDom: inserted the new box in the in DOM:");
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
    // setEVentListenersOnGroupOfButtons(_dupRow, _eventHandler, _buttonGroupSelector);
    _dupRow = setEVentListenersOnGroupOfButtons(_dupRow, onclickButton, "button[data-boxstate]");

    // indicate masterbox number
    _dupRow = _indicateMasterBoxNumber(data.masterbox, _dupRow);

    // set event listener on master select
    var _select = _selectMasterSelectInRow(_dupRow);
    setSelectEvents(_select);

    // set boxDefaultState button
    // _setCurrentStateButton(memRow, datasetKey, datasetValue)
    _dupRow = _setCurrentStateButton(_dupRow, "boxDefstate", data.boxDefstate);

    // set event listener on default state buttons
    // setEVentListenersOnGroupOfButtons(_dupRow, _eventHandler, _buttonGroupSelector);
    _dupRow = setEVentListenersOnGroupOfButtons(_dupRow, onclickDefStateButton, "button[data-boxDefstate]");

    // render in DOM
    _dupRow = _renderRowInDom(_dupRow);

    // add a key/entry pair to the controlerBoxes map and to the rowsMap map
    _addToMaps(data, _dupRow);
  }
  console.log("addNewRowForNewBox: ending after adding laser box [" + data.lb + "]");
}






function addOrUpdateNewRowForNewBox(_data) {
  // _data = {lb:1; action: "addBox"; boxState: 3; masterbox: 4; boxDefstate: 6}
  console.log("addOrUpdateNewRowForNewBox starting.");

  // Check whether the boxRow has not already been created
  var _controlerBoxEntry = controlerBoxes.get(_data.lb);
  console.log("addOrUpdateNewRowForNewBox: looking if an entry exists in the map for this box");
  console.log("addOrUpdateNewRowForNewBox _controlerBoxEntry (if undefined, the entry does not exist): " + _controlerBoxEntry);
  console.log("addOrUpdateNewRowForNewBox: testing if _controlerBoxEntry is undefined: -> " + (_controlerBoxEntry === undefined));

  // Case where the box does not exist
  if (_controlerBoxEntry === undefined) {
    // _controlerBoxEntry is equal to undefined: the boxRow does not already exists
    // let's create it
    addNewRowForNewBox(_data);
    // _data = {lb:1; action: "addBox"; boxState: 3; masterbox: 4; boxDefstate: 6}
  }

  // Case where the box exists
  else {
    // _controlerBoxEntry is not equal to undefined, the boxRow already exists
    // let's update it instead
    updateBoxRow(_data);
  }
  // handles the case where this is a reboot
  _onRebootCommon.onAddBox(_data);
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
  var _boxRowsContainer = document.querySelector("#boxesContainer");
  while (_boxRowsContainer.children[1]) {
    _boxRowsContainer.removeChild(_boxRowsContainer.firstChild);
  }
}




function deleteBoxRow(_data) {
  console.log("deleteBoxRow starting.");
  if (boxesRows.size) {
    var _boxRowToDelete = boxesRows.get(_data.lb);
    if (_boxRowToDelete === undefined) {
      console.log("deleteBoxRow: There was no laser box [" + _data.lb + "] in controlerBoxes map.");
      return;
    }
    _boxRowToDelete.parentNode.removeChild(_boxRowToDelete);
    _deleteFromMaps(_data.lb); // updating the controlesBoxes map
    onRebootLBs.onDeleteBox(_data);
    console.log("deleteBoxRow: deleted key [" + _data.lb + "] in controlerBoxes and boxesRows maps.");
    console.log("deleteBoxRow ending.");
    return true;
  } else {
    return false;
  }
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









function boxRowTemplateSelector() {
  var _row = document.getElementById('boxTemplate');
  var _templateDup = _row.cloneNode(true);

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
function setEventListenersOnBoxRowElements(_dupRow, _laserBoxIndexNumber) {
  _dupRow.querySelector("#rebootBox").addEventListener('click', onclickRebootBoxButton, false);
  _dupRow.querySelector("#rebootBox").id = "rebootBox" + _laserBoxIndexNumber;     // set a unique id
  _dupRow.querySelector("#rebootAndSaveBox").addEventListener('click', onclickRebootAndSaveBoxButton, false);
  _dupRow.querySelector("#rebootAndSaveBox").id = "rebootAndSaveBox" + _laserBoxIndexNumber;     // set a unique id
  _dupRow.querySelector("#savePrefsBox").addEventListener('click', onclickSavePrefsBoxButton, false);
  _dupRow.querySelector("#savePrefsBox").id = "savePrefsBox" + _laserBoxIndexNumber;     // set a unique id
  // _dupRow.querySelector("#OTA1reboot").addEventListener('click', onclickOTABoxButton, false);
  _dupRow.querySelector("#OTA1reboot").id = "OTA1reboot" + _laserBoxIndexNumber;     // set a unique id
  // _dupRow.querySelector("#OTA2reboots").addEventListener('click', onclickOTABoxButton, false);
  _dupRow.querySelector("#OTA2reboots").id = "OTA2reboots" + _laserBoxIndexNumber;     // set a unique id
  return _dupRow;
}

function setEVentListenersOnGroupOfButtons(_dupRow, _eventHandler, _buttonGroupSelector) {
  console.log("setEVentListenersOnGroupOfButtons: starting");
  setButtonsGroupEvents(boxRowEltsGroupSelector(_dupRow, _buttonGroupSelector), _eventHandler);
  return _dupRow;
}

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
  document.getElementById('saveWifiSettingsIF').addEventListener('click', onclickSaveWifiSettingsIF, false);
  document.getElementById('saveWifiSettingsAll').addEventListener('click', onclickSaveWifiSettingsAll, false);
  document.querySelectorAll('.gi8RequestedOTAReboots').forEach(
    function(_OTARebootButton){
      _OTARebootButton.addEventListener('click', onclickgi8RequestedOTAReboots, false);
    }
  );  
}

// END EVENT LISTENERS





// WINDOW LOAD
window.onload = function(_e){
    console.log("window.onload");
    // Interval at which to check if WS server is still available
    // (and reconnect as necessary) setInterval(check, 5000);
    checkConnect.intervalCode = setInterval(check, (getRandomArbitrary(10, 4) * 1000));
    setTimeout(setGroupEvents, 2000);
    // checkWSStarter(check);
};
// END WINDOW LOAD




// HELPERS
// Remove an element from the DOM
function removeDOMelement(_selector) {
  var _eltToRemove = document.querySelector(_selector);
  if (_eltToRemove !== null) {
    _eltToRemove.parentNode.removeChild(_eltToRemove);
    return true;
  }
  return false;
}

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
