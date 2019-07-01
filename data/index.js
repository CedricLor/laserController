
// Global variables
var ws = null;
var source = new EventSource('/events');

// Web events
source.addEventListener('open', function(e) {
  console.log('Web Events Connected');
}, false);

source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log('Web Events Disconnected');
  }
}, false);

source.addEventListener('message', function(e) {
  console.log('Web Events message', e.data);
}, false);

source.addEventListener('STATIONIP', function(e) {
  console.log("received IP");
  console.log('Web Events stationIp', e.data);
}, false);



// Web socket
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
    console.log('Socket is closed. Reconnect will be attempted some time.', e.reason);
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

function check(){
  if(!ws || ws.readyState === WebSocket.CLOSED) connect();
  // if(!ws || ws.readyState == 3) connect();
}

connect();

setInterval(check, Math.min(10000,timeout+=timeout));

var buttonClass = document.getElementsByClassName("button");

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

window.onload = function(e){
    console.log("window.onload");
    for (var i = 0; i < buttonClass.length; i++) {
      buttonClass[i].addEventListener('click', onclickButton, false);
    }
}
