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








/** boxRowManager
 * 
 *  Object holding functions to create a new boxRow */
var boxRowManager  = {
  /** boxRowManager.virtualTemplate
   * 
   *  Will be a copy of the hidden div with id 'boxTemplate'.
   *  Will be loaded once boxRowManager.init() has been called (onload). */
  virtualTemplate: undefined,
  /** boxRowManager.boxesContainer
   * 
   *  Will be a copy of the div with id 'boxesContainer'.
   *  Will be loaded once boxRowManager.init() has been called (onload).*/
  boxesContainer:  undefined,

  /** boxRowManager.init()
   * 
   *  Loads copies of the divs with id 'boxesContainer' and 'boxTemplate'
   *  into boxesContainer and virtualTemplate.*/
  init:            function() {
    var _row                        = document.getElementById('boxTemplate');
    boxRowManager.virtualTemplate   = _row.cloneNode(true);
    boxRowManager.boxesContainer    = document.getElementById('boxesContainer');
    // _row.parentNode.removeChild(_row);
  },
  
  /** boxRowManager.template()
   * 
   *  Returns a copy of the boxRow virtualTemplate.*/
  template: function() {
    return (this.virtualTemplate.cloneNode(true));
  },

  /** boxRowManager.insertNewBoxInBoxesContainerInDOM
   * 
   *  Object containing function to insert the new box row in div#boxesContainer*/
  insertNewBoxInBoxesContainerInDOM: {
    /** boxRowManager.insertNewBoxInBoxesContainerInDOM.last(_newRow)
     * 
     *  Inserts the _newRow as last child in div#boxesContainer*/
    last:   function(_newRow){
      return (boxRowManager.boxesContainer.appendChild(_newRow));
    },
    /** boxRowManager.insertNewBoxInBoxesContainerInDOM.last(_newRow)
     * 
     *  Inserts the _newRow as first child in div#boxesContainer. Returns the _newRow to the user.*/
    first:  function(_newRow){
      if (boxRowManager.boxesContainer.hasChildNodes()) {
        return (boxRowManager.boxesContainer.insertBefore(_newRow, boxRowManager.boxesContainer.firstChild));
      } else {
        return (boxRowManager.insertNewBoxInBoxesContainerInDOM.last(_newRow));
      }
    }
  }
};





class controlerBox {
  constructor (props) {
    this.lb = parseInt(props.lb, 10);
    this.boxState = props.boxState;
    // this.boxDefstate = props.boxDefstate;
    this.virtualHtmlRowElt = boxRowManager.template();
    this._setHtmlProperties();
  }
  
  setHtmlProperties() {
    this.virtualHtmlRowElt.id = "boxRow" + this.lb;
    this.virtualHtmlRowElt.dataset.lb = this.lb;
    this.virtualHtmlRowElt.classList.remove('hidden');
    this.virtualHtmlRowElt.querySelector("span.box_num").textContent = this.lb + 200;
  }
  // setEventHandlers: setEventListenersOnBoxRowElements(_dupRow, _laserBoxIndexNumber);
}














// BOX ROWS MAPS
var boxMaps = {
  controlerBoxes: new Map(),
  boxesRows:      new Map(),
  ctrlerBxes:       new Map(),

  /** boxMaps._create(data)
   *  Create a new entry in the ctrlerBxes map
   */
  _create: function(data) {
    var _newCtrlBx = new controlerBox({props: data});
    boxMaps.ctrlerBxes.set(data.lb, _newCtrlBx);
  },

  /** boxMaps._add() <-- TO BE DELETED AND REPLACED BY boxMaps._create()
   *  adder to the boxesRows and controlerBoxes maps.
   */
  _add: function(data, _dupRow) {
    console.log("boxMaps._add starting.");
    boxMaps.controlerBoxes.set(data.lb, data.boxState);
    // Map.prototype.set(key, value)
    // {1:4;2:3;etc.}
    console.log("boxMaps._add: boxMaps.controlerBoxes map: set key [" + data.lb + "] with value [" + data.boxState +  "] in boxMaps.controlerBoxes map.");
    console.log(boxMaps.controlerBoxes);
    boxMaps.boxesRows.set(data.lb, _dupRow);
    console.log("boxMaps._add: boxMaps.boxesRows map: set key [" + data.lb + "] with value [" + _dupRow +  "] in boxesRows map.");
    console.log(boxMaps.boxesRows);
    console.log("boxMaps._add ending.");
  },
  
  _delete: function(boxNumber) {
    console.log("boxMaps._delete starting.");
    boxMaps.controlerBoxes.delete(boxNumber);
    boxMaps.boxesRows.delete(boxNumber);
    console.log("boxMaps._delete ending.");
  }     
};




















// WEB SOCKET
/**
 * connectionObj:
 * Object to hold all the connection and WS management related stuffs
 */
var connectionObj = {
  // ws will hold the WebSocket connection
  ws:               null,

  start:            function() {
    connectionObj.checkConnect.intervalCode = setInterval(connectionObj.check, (getRandomArbitrary(10, 4) * 1000));
  },

  wsonopen:         function() {
      console.log("connect(): WS connection open.");
      connectionObj.checkConnect.retryCount = 0;
      // console.log("connect(): checkConnect.retryCount = " + checkConnect.retryCount + ".");
      connectionObj.checkConnect.deleteNotConnectedMsg();
      console.log("Sending the server the list of boxMaps.controlerBoxes I have in the DOM (and their current state)");
      // Send a message to the server with the list of boxMaps.controlerBoxes I have in the DOM
      connectionObj.ws.send(JSON.stringify({
        action:           "handshake",
        boxesStatesInDOM: mapToObj(boxMaps.controlerBoxes)
      }));
      // {action:0, boxStateInDOM:{1:4;2:3}}
  },

  wsonmessage:     function(e) {
      connectionObj.ping.check = 0;
      // if ping pong message
      if (connectionObj.ping.onPingMsg(e)) {return;}
      // if other messages, parse JSON
      console.log( "WS Received Message: " + e.data);
      var _data = JSON.parse(e.data);
      onMsgActionSwitch(_data);
  },

  wsonclose:        function(e) {
      if (connectionObj.checkConnect.retryCount != 10) {
        console.log('Socket is closed. Reconnect will be attempted in 4 to 10 seconds. Reason: ', e);
      }
      console.log('Socket is closed. Reason: ', e);
      deleteAllBoxRows();
  },

  wsonerror:        function(err){
      console.error('Socket encountered error: ', err.target.readyState, 'Closing socket');
      connectionObj.ws.close();
  },
  
  // connect makes the connection. It is called by the connectionObj.check() function.
  connect:          function() {
    console.log("connect() starting.");
    connectionObj.ws =            new WebSocket('ws://'+ self.location.host +'/');
  
    /** ws: onopen
     *  send a message to the server with the list of 
     * controlerBoxes currently in the DOM and their states. */
    connectionObj.ws.onopen =     connectionObj.wsonopen;

    /** ws: on receive a message
     *  decode its action type to dispatch it */
    connectionObj.ws.onmessage =  connectionObj.wsonmessage;
  
    /** ws: onclose
     * inform the user that an attempt to reconnect
     * will be made soon and delete all the boxRows. */
    connectionObj.ws.onclose =     connectionObj.wsonclose;         
  
    /** ws: onerror,
     * inform the user that we will be closing the socket */
    connectionObj.ws.onerror =     connectionObj.wsonerror;
  },

  /**
   * check() is called at regular interval, within a setInterval.
   * It checks if the WS is still open. If not, it will instruct
   * connectionObj.connect() to try and open a new connection.
   * It is also the function that opens the initial connection.
   */
  check:                      function (){
    // if checkConnect.retryCount is equal to 10, just stop trying
    // console.log("check(): --- in check");
    // console.log("check(): checkConnect.retryCount === " + checkConnect.retryCount + ".");
    if (connectionObj.checkConnect.retryCount === 10) {
      console.log("check(): Tried to reconnect 10 times. Stopping. Please reload or check the server.");
      clearInterval(connectionObj.checkConnect.intervalCode);
      // TO DO: inform the user that he should try to reload
      return;
    }
    // if the connection is inexistant or closed
    if(!connectionObj.ws || connectionObj.ws.readyState === WebSocket.CLOSED) {
      console.log("check(): NO CONNECTION. TRYING TO RECONNECT");
      console.log("check(): !!! Trying to reconnect on !ws or ws.CLOSED !!!");
      // increment the checkConnect.retryCount
      connectionObj.checkConnect.retryCount++;
      // try to connect
      connectionObj.connect();
      return;
    }
    // if the connection is open,
    // check that the server is still effectively here
    if(connectionObj.ws.readyState === WebSocket.OPEN) {
      connectionObj.ping.checkPingStatus();
    }
  },

  // check connection and reconnect variables
  checkConnect: {
    intervalCode:           -1,
    retryCount:             0,
    notConnectedSpan:       false,

    addNotConnectedMsg:     function() {
      infoBox.addMsg("No connection.", "infoNotConnected", "span");
      this.notConnectedSpan = true;
    },

    deleteNotConnectedMsg:  function() {
      infoBox.deleteMsg('#infoNotConnected', this.notConnectedSpan);
    },

    closedVerb:             function() {
      if (!connectionObj.ws || connectionObj.ws.readyState === WebSocket.CLOSED) {
        this.addNotConnectedMsg();
        return true;
      }
      return false;
    }
  },

  // ping and reconnect
  ping: {
    count:            1,
    sentMark:         1,
    receivedMark:     1,
    check:            0,

    onPingMsg:        function(e) {
      connectionObj.ping.check = 0;
      if ((e.data > 0) && (e.data < 10)) {
        connectionObj.ping.receivedMark = e.data;
        return true;
      }
      return false;
    },

    checkPingStatus: function(){
      // console.log("check(): ping.sentMark === " + ping.sentMark);
      // console.log("check(): ping.receivedMark === " + ping.receivedMark);
      if (connectionObj.ping.sentMark != connectionObj.ping.receivedMark) {
        console.log("check(): not receiving server pongs");
        console.log("check(): about to close connection");
        connectionObj.ws.close();
        connectionObj.ping.count        = 1;
        connectionObj.ping.receivedMark = 1;
        connectionObj.ping.sentMark     = 1;
        return;
      }

      connectionObj.ping.check++;
      if (connectionObj.ping.check === 3) {
        connectionObj.ping.check        = 0;

        // try sending a numbered ping to the server
        connectionObj.ping.count++;
        if (connectionObj.ping.count === 9) {
          connectionObj.ping.count      = 1;
        }
        connectionObj.ws.send(connectionObj.ping.count);
        connectionObj.ping.sentMark = connectionObj.ping.count;
      }
    }
  },

  sendReceivedIP: function() {
    connectionObj.ws.send(JSON.stringify({
      action: "ReceivedIP"
    }));
    // {action:"ReceivedIP}
  }  
};























/**
 * onReboot: Object holding all the stack to handle the process of informing the user
 * on rebooting the all the LBs or all the mesh nodes
 */
var onReboot = {

  /** 
   * I. Object handling the onReboot LBs stack
   * */
  LBs: {
    // id of the button "reboots LBs"
    rebootBtnId:    'rebootLBs',
    // stores the numbers of the LBs which have been sent the order to reboot
    waitingLBs:     new Map(),
  
    /** onDeleteBox
     *  Gets called from deleteBoxRow(), itself upon receiving a message 
     *  from the interface that a box has disconnected from the mesh.
     *  Is going to keep track of the boxes which are effectively rebooting. */
    onDeleteBox:    function(_data) {
      // First check that we are in the reBoot.LBs mode
      if (onReboot.common.rebootType === 1) {
  
        // if this is the first box to disconnect
        if (onReboot.LBs.rebootingLBs.size === 0) {
          // change the button color to signal that it has started
          onReboot.common.turnBtnRed(this.rebootBtnId);
          // create the info text and box
          onReboot.common.onFirstBox(parseInt(_data.lb, 10), 
                                     "divRebootingLBs", 
                                     'Laser boxes currently rebooting: ', 
                                     "spanRebootingLBs", 
                                     onReboot.LBs.waitingLBs, 
                                     onReboot.common.rebootingLBs);
        } 
        // this is not the first box to be deleted
        else {
          // add its number to the info box
          onReboot.common.onAdditionalBoxes(parseInt(_data.lb, 10), 
                                            '#spanRebootingLBs', 
                                            onReboot.common.rebootingLBs, 
                                            onReboot.LBs.waitingLBs);
        }
  
        // delete the box from the waitingLBs map
        onReboot.LBs.waitingLBs.delete(parseInt(_data.lb, 10));
  
        if (boxMaps.boxesRows.size === 0) {
          // remove the waiting LBs div
          removeDOMelement('#divLBsWaitingToReboot');
        }
      }
    },
  
    /** onReboot.LBs.startConfirm
     *  gets called by the action switch, upon receiving confirmation that the 
     *  reboot LBs has started. */
    startConfirm:     function() {
      // 1. change the button color
      let _rebootBtn = document.getElementById('rebootLBs');
      _rebootBtn.className += ' button_change_received';
  
      // 2. call the common startConfirm proc      
      onReboot.common.startConfirm(1, "divLBsWaitingToReboot", 
                                   'Boxes waiting to reboot: ', 
                                   "spanLBsWaitingToReboot", 
                                   onReboot.LBs.waitingLBs);
  
      console.log("--------------- end LBs reboot switch -----------------");
    }
  },


  /** 
   * II. Object handling the process when clicking on the Reboot all button
   * */
  all: {
    // id of the button "reboots LBs"
    rebootBtnId:    'rebootAll',
    
    /** onReboot.all.startConfirm
     *  gets called by the action switch, upon receiving confirmation that the 
     *  reboot All has started. */
    startConfirm:   function() {
  
      // 1. turn the rebooting all button red
      onReboot.common.turnBtnRed(this.rebootBtnId);
  
      // 2. call the common start config proc      
      onReboot.common.startConfirm(2, "divRebootingLBs", 
                                   'Rebooting boxes: ', 
                                   "spanRebootingLBs", 
                                   onReboot.common.rebootingLBs);
  
      // 3. close the connection
      connectionObj.ws.close();
      // 5. delete all the boxes
      deleteAllBoxRows();
  
      console.log("--------------- end reboot all switch -----------------");
    }
  },


  /** 
   * III. Object wrapping various functions common to the objects onReboot.all and onReboot.LBs
   * */
  common: {
    // rebootType: keeps track of the reboot type (0: none; 1: LBs; 2: All)
    rebootType:                 0,
    // noConnectedBoxesSpan: keeps track whether the "no boxes currently connected" span is displayed or not
    noConnectedBoxesSpan:       false,

    // stores the numbers of the LBs which are currently disconnected
    rebootingLBs:   new Map(),
    // stores the numbers of the LBs which have reconnected
    rebootedLBs:    new Map(),

    /** reBoot.common.deleteNoConnectedBoxesSpan:
     *  Informs the user that no nodes are connected to the mesh, if he clicks on reboot
     *  and no boxes are connected. */
    deleteNoConnectedBoxesSpan: function() {
      infoBox.deleteMsg('#infoNoBoxesConnected', onReboot.common.noConnectedBoxesSpan);
    },
  
    /** reBoot.common.addNoConnectedBoxesSpan:
     *  Informs the user that no nodes are connected to the mesh, if he clicks on reboot
     *  and no boxes are connected. */
    addNoConnectedBoxesSpan:    function() {
      if (!onReboot.common.noConnectedBoxesSpan) {
        infoBox.addMsg("There are no laser boxes currently connected.", "infoNoBoxesConnected", "span");
      }
      onReboot.common.noConnectedBoxesSpan = true;
    },
  
    /** reBoot.common.onFirstBox:
     *  when the first rebooting box connects back: 
     *  1. creates a div and a span to inform the user of the number of the incoming box
     *  2. add the box number to the span and
     *  3. transfer the box number from one map to the other. */
    onFirstBox:                 function( _laserBoxIndexNumber,
                                          _msgDivId /*"divRebootedLBs"*/,
                                          _msgIntro /*'Laser boxes currently rebooted: '*/,
                                          _msgSpanId /*"spanRebootedLBs"*/,
                                          _rebootingLBs /* [all || LBs].rebootingLBs*/,
                                          _rebootedLBs /* [all || LBs].rebootedLBs*/) {
  
      // 1. check whether there was a message saying there was no connected boxes
      // and delete it if necessary
      onReboot.common.deleteNoConnectedBoxesSpan();
  
      // 2. create a div to hold the infos
      let _divNewStateLBs        = onReboot.common.createBoxListContainerDiv(_msgDivId, _msgIntro);
  
      // 3. create a span to hold the numbers of the rebooted LBs
      let _spanNewStateLBsList   = infoBox.createContainer("span", _msgSpanId);
      console.log("onFirstBox: _msgSpanId = " + _msgSpanId);
      console.log("onFirstBox: _spanNewStateLBsList = ");
      console.log(_spanNewStateLBsList);
  
      // 4. transfer the box number from one span to the other 
      //    and add the box number to the span
      _spanNewStateLBsList       = onReboot.common.mapToMapTransferAndInsertBoxList(_spanNewStateLBsList, 
                                                                                    _laserBoxIndexNumber, 
                                                                                    _msgSpanId, 
                                                                                    _rebootedLBs, 
                                                                                    _rebootingLBs);
  
      // 5. append the span and the div to the DOM
      infoBox.renderInDom(_divNewStateLBs, _spanNewStateLBsList);
    },
  
    /** reBoot.common.onAdditionalBoxes:
     *  When additional boxes connect back, inform the user of the 
     *  number of the new incoming box. */
    onAdditionalBoxes:          function(_laserBoxIndexNumber, 
                                         _destinationSpanId /*'#spanRebootingLBs'*/, 
                                         _rebootedLBs, 
                                         _rebootingLBs) {
      // select the span in which the numbers of the newly connecting boxes
      // is displayed.
      let _spanNewStateLBsList = document.querySelector(_destinationSpanId);
      console.log("onFirstBox: _msgSpanId = " + _destinationSpanId);
      console.log("onFirstBox: _spanNewStateLBsList = ");console.log(_spanNewStateLBsList);
      // 
      this.mapToMapTransferAndInsertBoxList(_spanNewStateLBsList,
                                            _laserBoxIndexNumber, 
                                            _destinationSpanId, 
                                            _rebootedLBs, 
                                            _rebootingLBs);
    },
  
    /** reBoot.common.onAddBox:
     * This gets called by addOrUpdateNewRowForNewBox(), which itself gets called from
     * the action switch, when a new box comes in. */
    onAddBox: function(_data){

      // 1. Identify which type of reboot cycle we are in (rebootAll or rebootLBs)
      //    Select the corresponding object to handle the case.
      var _onRebootTypeObj;
      if (onReboot.common.rebootType === 2) {
        // select the onReboot.all object
        _onRebootTypeObj = onReboot.all;
      } else if (onReboot.common.rebootType === 1) {
        // select the onReboot.LBs object
        _onRebootTypeObj = onReboot.LBs;
      } else {
        // if none of them, just return
        return;
      }
  
      /** 2. if the size of the rebootedLBs is 0, it means that no
       *  box has yet rejoined => this is the first box to reboot. */
      if (onReboot.common.rebootedLBs.size === 0) {
        onReboot.common.onFirstBox(parseInt(_data.lb, 10), 
                                   "divRebootedLBs", 
                                   'Laser boxes currently rebooted: ', 
                                   "spanRebootedLBs", 
                                   onReboot.common.rebootingLBs, 
                                   onReboot.common.rebootedLBs);
      } 
      /** 3. If the size of the rebootedLBs is > 0, it means that 
       *  it is not the first box to reconnect => it shall only
       *  be a question of updating the displayed list. */
      else {
        onReboot.common.onAdditionalBoxes(parseInt(_data.lb, 10), 
                                          '#spanRebootedLBs', 
                                          onReboot.common.rebootedLBs, 
                                          onReboot.common.rebootingLBs);
      }

      /** 4. In all the cases of incoming box:
       *  delete the incoming box from the rebooting LBs map */
      onReboot.common.rebootingLBs.delete(parseInt(_data.lb, 10));

      /** 5. At the end of the reboot process:
       *  the size of the rebootingLBs shall be 0.
       *  All the boxes should be reconnected.*/
      if (onReboot.common.rebootingLBs.size === 0) {
        // a. change the color of the button
        let _rebootLbsBtn = document.getElementById(_onRebootTypeObj.rebootBtnId);
        _rebootLbsBtn.classList.remove('button_active_state');

        // b. delete the rebootingLBs div
        removeDOMelement('#divRebootingLBs');
        // c. delete the rebootedLBs div
        removeDOMelement('#divRebootedLBs');

        // d. create a span to hold the textnode informing that all the boxes have rebooted
        let _spanLBsHaveRebooted = infoBox.createContainer("span", "LBsHaveRebooted");
        // e. create a text node for the introduction text
        var _infoText = document.createTextNode('All the laser boxes have rebooted.');

        // f. render in dom
        infoBox.renderInDom(_spanLBsHaveRebooted, _infoText);

        // g. set a timeout to delete the info box after 15 seconds
        window.setTimeout(function() {
          removeDOMelement('#LBsHaveRebooted');
        }, 15000);

        // h. empty the rebooted boxes maps
        onReboot.common.rebootedLBs.clear();

        // i. get out of the reboot process
        onReboot.common.rebootType = 0;
      }
    },
  
    /** reBoot.common.startConfirm:
     *  Called at the beginning of a reboot process
     * */
    startConfirm: function(_rebootType, _divRebootingBoxListId, _introMsg, 
                           _spanRebootingBoxListId, _mapRebootingBoxes) {
      // 1. save the fact that we are in one of the rebooting modes
      onReboot.common.rebootType = _rebootType;
  
      // 2. clear all the eventual remaining values in the onReboot.LBs maps
      onReboot.LBs.waitingLBs.clear();
      onReboot.common.rebootingLBs.clear();
      onReboot.common.rebootedLBs.clear();                            

      // 3. check whether there was a message saying there was no connected boxes
      // and delete it if necessary
      this.deleteNoConnectedBoxesSpan();
  
      // 4. create a div and a span to hold the number of the rebooting boxes
      // and info text
      let _divRebootingBoxList = this.createBoxListContainerDiv(_divRebootingBoxListId, _introMsg);
      let _spanRebootingBoxList = infoBox.createContainer("span", _spanRebootingBoxListId);
  
      // 5. Load the span with the values from the boxRows map
      _spanRebootingBoxList = this.createBoxTextNodesFromBoxRows(_spanRebootingBoxList, _mapRebootingBoxes);
  
      // 6. render the span in the infoBox
      infoBox.renderInDom(_divRebootingBoxList, _spanRebootingBoxList);
    },
  
    /** reBoot.common.createBoxTextNodesFromBoxRows:
     *  Called at the beginning of a reboot process
     * */
    createBoxTextNodesFromBoxRows: function(_spanLBsRebooting) {
      // iterate over the boxMaps.boxesRows map, create textNodes for the span and
      // store them in a map
      boxMaps.boxesRows.forEach(function(val, key) {
        let _text = (parseInt(key) + 200) + ". ";
        // create a textNode to hold the box number
        let _boxNumbNode = document.createTextNode(_text);
        // store it into a map
        onReboot.common.rebootingLBs.set(key, _boxNumbNode);
        // add the new textNode to the span
        // _spanLBsRebooting.appendChild(onRebootLBs.waitingLBs.get(key));
        _spanLBsRebooting.appendChild(onReboot.common.rebootingLBs.get(key));
      });
      return _spanLBsRebooting;
    },
  
    /** reBoot.common.createBoxListContainerDiv:
     *  Creates a div to receive the box numbers in process of 
     *  rebooting.
     * */
    createBoxListContainerDiv: function(_containerId, _infoText) {
      // create a div to hold the infos
      let _containerDiv = infoBox.createContainer("div", _containerId);
      // create a text node for the introduction text
      let _infoTextNode = document.createTextNode(_infoText);
      // append it to the div
      _containerDiv.appendChild(_infoTextNode);
      return _containerDiv;
    },
  
    /** reBoot.common.turnBtnRed:
     *  Called at the beginning of a reboot process, upon receiving
     *  confirmation from the server that the process has started.
     * */
    turnBtnRed: function(_btnId) {
      // change the button color to signal that it has started
      let _rebootBtn = document.getElementById(_btnId);
      _rebootBtn.classList.add('button_active_state');
      _rebootBtn.classList.remove('button_clicked', 'button_change_received');
    },
  
    /** reBoot.common.mapToMapTransferAndInsertBoxList:
     *  Called at various steps in the process:
     *  1. to transfer the box numbers from on map to the other (how ridiculous !!!);
     *  2. to append a the box number to the span */
    mapToMapTransferAndInsertBoxList: function(_spanNewStateLBsList, 
                                               _laserBoxIndexNumber, 
                                               _destinationSpanId, 
                                               _destinationMapSet, 
                                               _originMapSet){
      // transfer the box from the waitingLBs map to the this.rebootingLBs map
      _destinationMapSet.set(_laserBoxIndexNumber, _originMapSet.get(_laserBoxIndexNumber));
      // add the text node to the span
      _spanNewStateLBsList.appendChild(_destinationMapSet.get(_laserBoxIndexNumber));
      return _spanNewStateLBsList;
    }
  }
};






 








/**
 * onMsgActionSwitch:
 * a kind of controller, dispatching messages to various functions
 * depending on their types
 */
function onMsgActionSwitch(_data) {
  // Received IP and other global data (wifi settings)
  if (_data.action === 3) {
    // console.log("WS JSON message: " + _data.ServerIP);
    // Fill in the data in the DOM and add some eventHandlers
    updateGlobalInformation(_data);
    connectionObj.sendReceivedIP();
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
  // (or the DOM contained boxRows corresponding to boxes that
  // have been disconnected from the mesh)
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
    onReboot.all.startConfirm();
    return;
  }


  if (_data.action === "changeBox" && _data.key === "reboot" && _data.lb === "LBs") { // User request to reboot the LBs has been received and is being processed
    console.log("---------------- rebootStart switch starting -----------------");
    onReboot.LBs.startConfirm();
    return;
  }
}










/**
 * _onClickHelpers:
 * Library of helpers, to remove classes, add classes, send messages, identify the laser
 * box number, etc.
 */
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
    connectionObj.ws.send(JSON.stringify(_obj));
  },

  /** updateClickButtons
   *  Called by onClick event handlers on buttons.
   *  Iterates over the group of buttons to which the clicked buttons pertains.
   *  Removes any "button_clicked", "button_active_state" or
   *  "button_change_received" class that they may retain.
   *  Add a "button_clicked" class to the clicked button. */
  updateClickButtons: (e, _selector, _buttonsParentElement) => {
    _buttonsParentElement.querySelectorAll(_selector).forEach(
      (_button) => {
        _onClickHelpers.removeClassesOnNonClickedButton(_button);
      }
    );
    e.target.className += ' button_clicked';
  },

  /** removeClassesOnNonClickedButton
   *   Removes any "button_clicked", "button_active_state" or
   *  "button_change_received" class that a button may retain.  */
  removeClassesOnNonClickedButton: (_button) => {
    _button.classList.remove('button_clicked');
    _button.classList.remove('button_active_state');
    _button.classList.remove('button_change_received');
  } 
};







/**
 *  _onClickBoxConfig:
 *  Holder of all the onClick events of the box level configuration
 *  buttons (reboot, save, reboot and save, OTA reboot) buttons. */
var _onClickBoxConfig = {
  wrapper:        function(e, _obj) {
      // update the buttons
      _onClickHelpers.updateClickButtons(e, 'button', e.target.parentNode); // parent node is <div class='setters_group command_gp'>
      // if the connection is closed, inform the user
      if (connectionObj.checkConnect.closedVerb()) { return; }
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

  reboot:         function(e) {
      console.log("_onClickBoxConfig.reboot starting");
      _onClickBoxConfig.wrapper(e, {
        key:  "reboot",
        save: 0, // reboot without saving
      });
      // {action:"changeBox", key:"reboot", save: 0, lb:1}
      console.log("_onClickBoxConfig.reboot ending");  
  },

  rebootAndSave: function(e) {
      console.log("_onClickBoxConfig.rebootAndSave starting");
      _onClickBoxConfig.wrapper(e, {
        key:  "reboot",
        save: 1, // save and reboot
      });
      // {action:"changeBox", key:"reboot", save: 1, lb:1}
      console.log("_onClickBoxConfig.rebootAndSave ending");
  },

  savePrefs:     function(e) {
      console.log("_onClickBoxConfig.savePrefs starting");
      _onClickBoxConfig.wrapper(e, {
        key:  "save",
        save: "all", // save and reboot
      });
      // {action:"changeBox", key:"save", val: "all", lb:1}
      console.log("_onClickBoxConfig.savePrefs ending");  
  },

  OTAReboots:   function(e) {
    console.log("_onClickBoxConfig.OTAReboots starting");
    _onClickBoxConfig.wrapper(e, {
      key:      "save",
      val:      "gi8RequestedOTAReboots",
      reboots:  parseInt(this.dataset.reboots, 10),
    });
    // {action: "changeBox", key: "save", val: "gi8RequestedOTAReboots", lb: 1, reboots: 2}
    console.log("_onClickBoxConfig.OTAReboots ending");
  }
};

function onclickgOTARebootsBoxBtn(e) {
  console.log("onclickgOTARebootsBoxBtn starting");
  _onClickBoxConfig.wrapper(e, {
    key:     "save",
    val:     "gi8RequestedOTAReboots",
    reboots: parseInt(this.dataset.reboots, 10),
  });
  // {action: "changeBox", key: "save", val: "gi8RequestedOTAReboots", lb: 1, reboots: 2}
  console.log("onclickgi8RequestedOTAReboots ending");
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
    if (connectionObj.checkConnect.closedVerb()) { return; }
    // if there are boxes in the boxes map, we are probably connected, so reboot
    if (boxMaps.boxesRows.size) {
      // 
      _onClickHelpers.updateClickButtons(e, '.net_command_gp > button', document);
      // else, complete the message and send it
      _obj.action = 'changeNet';
      _onClickHelpers.btnSend(_obj);
      // {action: "changeNet", key: "reboot", save: 0, lb: "LBs"}
      return;
    }
    // if there are no boxes in the boxes map, inform the user that there are no boxes
    onReboot.common.addNoConnectedBoxesSpan();
  },

  onclickRebootLBsButton: function(e) {
    console.log("onclickRebootLBsButton starting");
    _onClickGroupReboot.wrapper(e, Object.assign(_onClickGroupReboot.rebootObj, {lb: "LBs"}));
    // {action: "changeNet", key: "reboot", save: 0, lb: "LBs"}
    console.log("onclickRebootLBsButton: ending");
  },
  
  onclickRebootAllButton: function(e) {
    console.log("onclickRebootAllButton starting");
    _onClickGroupReboot.wrapper(e, Object.assign(_onClickGroupReboot.rebootObj, {lb: "all"}));
    // {action: "changeNet", key: "reboot", save: 0, lb: "all"}
    console.log("onclickRebootAllButton: ending");  
  },

  onclickSaveLBsButton: function (e) {
    console.log("onclickSaveLBsButton starting");
    _onClickGroupReboot.wrapper(e, Object.assign(_onClickGroupReboot.saveObj, {lb: "LBs"}));
    // {action: "changeNet", key: "save", val: "all", lb: "LBs"}
    console.log("onclickSaveLBsButton: ending");
  },
  
  onclickSaveAllButton: function(e) {
    console.log("onclickSaveAllButton starting");
    _onClickGroupReboot.wrapper(e, Object.assign(_onClickGroupReboot.saveObj, {lb: "all"}));
    // {action: "changeNet", key: "save", val: "all", lb: "all"}
    console.log("onclickSaveAllButton: ending");
  }  
};













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







var _onClickIF = {
    reboot: function(_e) {
      console.log("_onClickIF.reboot starting");
      connectionObj.ws.send(JSON.stringify({
        action: "changeBox",
        key: "reboot",
        save: 0,
        lb: 0
      }));
      // {action:"changeBox", key:"reboot", save: 0, lb:0}
      console.log("_onClickIF.reboot: ending");
    },

    save: function (_e) {
      console.log("_onClickIF.save starting");
      connectionObj.ws.send(JSON.stringify({
        action: "changeBox",
        key: "save",
        val: "all",
        lb: 0
      }));
      // {action: "changeBox", key: "save", val: "all", lb: 0}
      console.log("_onClickIF.save ending");
    }
};










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
    if (connectionObj.checkConnect.closedVerb()) { return; }
    // else, complete the message
    _obj = this.buildObj(_obj);
    // and send the message
    _onClickHelpers.btnSend(_obj);
    // {action: "changeBox", key: "save", val: "wifi", lb: 0, dataset: {ssid: "blabla", pass: "blabla", gatewayIP: "192.168.25.1", ui16GatewayPort: 0, ui8WifiChannel: 6}}
  },

  onIF: function (e) {
    console.log("_onClickSaveWifi.onIF starting");
    _onClickSaveWifi.wrapper(e, {
        action: "changeBox",
        lb: 0,
      });
    // {action: "changeBox", key: "save", val: "wifi", lb: 0, dataset: {ssid: "blabla", pass: "blabla", gatewayIP: "192.168.25.1", ui16GatewayPort: 0, ui8WifiChannel: 6}}
    console.log("_onClickSaveWifi.onIF ending");
  },

  onAll: function (e) {
    console.log("_onClickSaveWifi.onAll starting");
  
    _onClickSaveWifi.wrapper(e, {
        action: "changeNet",
        lb: "all",
      });
    // {action: "changeNet", key: "save", val: "wifi", lb: "all", dataset: {ssid: "blabla", pass: "blabla", gatewayIP: "192.168.25.1", ui16GatewayPort: 0, ui8WifiChannel: 6}}
  
    console.log("_onClickSaveWifi.onAll ending");
  }
};










function onclickgi8RequestedOTAReboots(_e) {
  console.log("onclickgi8RequestedOTAReboots starting");

  connectionObj.ws.send(JSON.stringify({
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
    _onClickHelpers.updateClickButtons(e, buttonSelector, boxMaps.boxesRows.get(_laserBoxNumber));
    // if the connection is closed, inform the user
    if (connectionObj.checkConnect.closedVerb()) { return; }
    _onClickHelpers.btnSend({
      "action": "changeBox",
      "key":    _clef,
      "lb":     _laserBoxNumber,
      "val":    parseInt(_datasetValue, 10)
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
      key:    "masterbox",
      lb:     _laserBoxNumber,
      val:    parseInt(this.options[this.selectedIndex].value, 10)
     });
     //   // _obj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3} // boxState // ancient 4
     //   // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4} // masterbox // ancient 8
     //   // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3} // boxDefstate // ancient 9
    console.log("oninputMasterSelect: about to send json via WS: " + _json);
    connectionObj.ws.send(_json);
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
  var _boxRow = boxMaps.boxesRows.get(_data.lb);
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
//   var _boxRow = boxMaps.boxesRows.get(data.lb);
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
  var _boxRow = boxMaps.boxesRows.get(_data.lb);

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
      function(_button) {
        _onClickHelpers.removeClassesOnNonClickedButton(_button);
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
    _onClickHelpers.removeClassesOnNonClickedButton(_targetButton);
    _targetButton.classList.add('button_active_state');
  }
  console.log("_setStateButtonAsActive: ending on returning row");
  return memRow;
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









function addNewRowForNewBox(data) {
  // _data = {lb:1; action: "addBox"; boxState: 3; masterbox: 4; boxDefstate: 6}
  console.log("addNewRowForNewBox: Starting: the boxRow does not already exist. I am about to create it.");
  var _ctrlerBx = boxMaps._create(data);

  // set properties of the boxRow
  _ctrlerBx.setHtmlProperties();

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
  _dupRow = boxRowManager.insertNewBoxInBoxesContainerInDOM.first(_dupRow);

  console.log("addNewRowForNewBox: ending after adding laser box [" + data.lb + "]");
}






function addOrUpdateNewRowForNewBox(_data) {
  // _data = {lb:1; action: "addBox"; boxState: 3; masterbox: 4; boxDefstate: 6}
  console.log("addOrUpdateNewRowForNewBox starting.");

  // Check whether the boxRow has not already been created
  var _controlerBoxEntry = boxMaps.controlerBoxes.get(_data.lb);
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
  onReboot.common.onAddBox(_data);
}




function deleteAllBoxRows() {
  // delete from maps represenations
  if (boxMaps.controlerBoxes.size) {
    boxMaps.controlerBoxes.clear();
  }
  if (boxMaps.boxesRows.size) {
    boxMaps.boxesRows.clear();
  }
  // delete from DOM
  var _boxRowsContainer = document.querySelector("#boxesContainer");
  while (_boxRowsContainer.children[1]) {
    _boxRowsContainer.removeChild(_boxRowsContainer.firstChild);
  }
}



/** deleteBoxRow
 *  deletes one boxRow from the DOM.
 *  gets called from the action switch upon receiving a delete message from
 *  the server. */
function deleteBoxRow(_data) {
  console.log("deleteBoxRow starting.");
  // first check whether the boxMaps contain any boxRows. If not, just return.
  if (boxMaps.boxesRows.size) {
    // if there are boxRows in the maps, try to find the corresponding box
    var _boxRowToDelete = boxMaps.boxesRows.get(_data.lb);
    // if the corresponding box is not in the map, just return
    if (_boxRowToDelete === undefined) {
      console.log("deleteBoxRow: There was no laser box [" + _data.lb + "] in boxMaps.controlerBoxes map.");
      return;
    }

    // else, the box exists: 
    // 1. delete the box row and 
    _boxRowToDelete.parentNode.removeChild(_boxRowToDelete);
    // 2. update the controlesBoxes maps
    boxMaps._delete(_data.lb);
    // 3. check whether the box is not disconnecting as
    //    a result of a reboot order and inform the user
    onReboot.LBs.onDeleteBox(_data);
    console.log("deleteBoxRow: deleted key [" + _data.lb + "] in controlerBoxes and boxesRows maps.");
    console.log("deleteBoxRow ending.");
    return true;
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
  var _row = boxMaps.boxesRows.get(_data.lb);
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
  _dupRow.querySelector("#rebootBox").addEventListener('click', _onClickBoxConfig.reboot, false);
  _dupRow.querySelector("#rebootBox").id = "rebootBox" + _laserBoxIndexNumber;     // set a unique id
  _dupRow.querySelector("#rebootAndSaveBox").addEventListener('click', _onClickBoxConfig.rebootAndSave, false);
  _dupRow.querySelector("#rebootAndSaveBox").id = "rebootAndSaveBox" + _laserBoxIndexNumber;     // set a unique id
  _dupRow.querySelector("#savePrefsBox").addEventListener('click', _onClickBoxConfig.savePrefs, false);
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
  document.getElementById("rebootLBs").addEventListener('click', _onClickGroupReboot.onclickRebootLBsButton, false);
  document.getElementById("rebootIF").addEventListener('click', _onClickIF.reboot, false);
  document.getElementById("rebootAll").addEventListener('click', _onClickGroupReboot.onclickRebootAllButton, false);
  document.getElementById("saveLBs").addEventListener('click', _onClickGroupReboot.onclickSaveLBsButton, false);
  document.getElementById("saveIF").addEventListener('click', _onClickIF.save, false);
  document.getElementById("saveAll").addEventListener('click', _onClickGroupReboot.onclickSaveAllButton, false);
  document.getElementById('saveWifiSettingsIF').addEventListener('click', _onClickSaveWifi.onIF, false);
  document.getElementById('saveWifiSettingsAll').addEventListener('click', _onClickSaveWifi.onAll, false);
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
    connectionObj.start();
    setTimeout(setGroupEvents, 2000);
    boxRowManager.init();
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
