// Get references to UI elements
let connectButton = document.getElementById('connect');
let disconnectButton = document.getElementById('disconnect');
let terminalContainer = document.getElementById('terminal');
let sendForm = document.getElementById('send-form');
let inputField = document.getElementById('input');

// Connect to the device on Connect button click
connectButton.addEventListener('click', function() {
  connect();
});

// Disconnect from the device on Disconnect button click
disconnectButton.addEventListener('click', function() {
  disconnect();
});

// Handle form submit event
sendForm.addEventListener('submit', function(event) {
  event.preventDefault(); // Prevent form sending
  send(inputField.value); // Send text field contents
  inputField.value = '';  // Zero text field
  inputField.focus();     // Focus on text field
});

window.turn_led=(n)=>send(n);
window.move=(n)=>send(n);
window.activateFollow=()=>{
  send('U');
  toggleSelectorVis("","none",'inline');
};
window.activateControl=()=>{
  send('C');
  document.getElementById("controller-page").style.display='grid';
  document.getElementById("big-container").style.display='none';
  document.body.style.backgroundImage="url('Imgs/TopBkg.png')";
  document.body.style.backgroundPosition="center";
};
window.controllerBack=(e)=>{
  e = e || window.event;
  e.preventDefault();
  send('S');
  document.getElementById("controller-page").style.display='none';
  document.getElementById("big-container").style.display='grid';
  document.body.style.backgroundImage="url('Imgs/BigBkg2.png')";
  document.body.style.backgroundPosition="center";
 
};

window.toggleSelectorVis=(e, type1, type2, button=false)=>{
  if(button){
    e = e || window.event;
    e.preventDefault();
    send('S');
  }
  
  document.getElementsByClassName("selectors")[0].style.display=type1;
  document.getElementsByClassName("selectors")[1].style.display=type1;
  document.getElementsByClassName("selectors")[2].style.display=type1;
  document.getElementById("ultrasonicAd").style.display=type2;
  
  
}
window.activateLed=(type1, type2)=>{
  document.getElementsByClassName("selectors")[0].style.display=type1;
  document.getElementsByClassName("selectors")[1].style.display=type1;
  document.getElementsByClassName("selectors")[2].style.display=type1;
  document.getElementById("led-buttons").style.display=type2;
};

// Selected device object cache
let deviceCache = null;

// Launch Bluetooth device chooser and connect to the selected
function connect() {
  return (deviceCache ? Promise.resolve(deviceCache) :
      requestBluetoothDevice()).
      then(device => connectDeviceAndCacheCharacteristic(device)).
      then(characteristic => startNotifications(characteristic)).
      catch(error => log(error));
}

function requestBluetoothDevice() {
  log('Requesting bluetooth device...');

  return navigator.bluetooth.requestDevice({
    filters: [{services: [0xFFE0]}],
  }).
      then(device => {
        log('"' + device.name + '" bluetooth device selected');
        deviceCache = device;

        return deviceCache;
      });
}

// Connect to the device specified, get service and characteristic
// Characteristic object cache
let characteristicCache = null;

// Connect to the device specified, get service and characteristic
function connectDeviceAndCacheCharacteristic(device) {
  if (device.gatt.connected && characteristicCache) {
    return Promise.resolve(characteristicCache);
  }

  log('Connecting to GATT server...');

  return device.gatt.connect().
      then(server => {
        log('GATT server connected, getting service...');

        return server.getPrimaryService(0xFFE0);
      }).
      then(service => {
        log('Service found, getting characteristic...');

        return service.getCharacteristic(0xFFE1);
      }).
      then(characteristic => {
        log('Characteristic found');
        characteristicCache = characteristic;

        return characteristicCache;
      });
}

// Enable the characteristic changes notification
function startNotifications(characteristic) {
  log('Starting notifications...');

  return characteristic.startNotifications().
      then(() => {
        log('CONNECTED TO BLUETOOTH');
        terminalContainer.style.color="#00FF00";
      });
}

// Output to terminal
function log(data, type = '') {
  terminalContainer.innerHTML=
      '<div' + (type ? ' class="' + type + '"' : '') + '>' + data + '</div>';
      // terminalContainer.insertAdjacentHTML('beforeend',
      // '<div' + (type ? ' class="' + type + '"' : '') + '>' + data + '</div>');
}
function requestBluetoothDevice() {
  log('Requesting bluetooth device...');

  return navigator.bluetooth.requestDevice({
    filters: [{services: [0xFFE0]}],
  }).
      then(device => {
        log('"' + device.name + '" bluetooth device selected');
        deviceCache = device;

        // Added line
        deviceCache.addEventListener('gattserverdisconnected',
            handleDisconnection);

        return deviceCache;
      });
}

function handleDisconnection(event) {
  terminalContainer.style.color="#FF0000"
  let device = event.target;

  log('"' + device.name +
      '" bluetooth device disconnected, trying to reconnect...');

  connectDeviceAndCacheCharacteristic(device).
      then(characteristic => startNotifications(characteristic)).
      catch(error => log(error));
}
// Disconnect from the connected device
// Enable the characteristic changes notification
function startNotifications(characteristic) {
  log('Starting notifications...');

  return characteristic.startNotifications().
      then(() => {
        log('CONNECTED TO BLUETOOTH');
        terminalContainer.style.color="#00FF00";
        // Added line
        characteristic.addEventListener('characteristicvaluechanged',
            handleCharacteristicValueChanged);
      });
}

function disconnect() {
  terminalContainer.style.color="#FF0000"
  if (deviceCache) {
    log('Disconnecting from "' + deviceCache.name + '" bluetooth device...');
    deviceCache.removeEventListener('gattserverdisconnected',
        handleDisconnection);

    if (deviceCache.gatt.connected) {
      deviceCache.gatt.disconnect();
      log('"' + deviceCache.name + '" bluetooth device disconnected');
    }
    else {
      log('"' + deviceCache.name +
          '" bluetooth device is already disconnected');
    }
  }

  // Added condition
  if (characteristicCache) {
    characteristicCache.removeEventListener('characteristicvaluechanged',
        handleCharacteristicValueChanged);
    characteristicCache = null;
  }

  deviceCache = null;
}

// Intermediate buffer for incoming data
let readBuffer = '';

// Data receiving
function handleCharacteristicValueChanged(event) {
  let value = new TextDecoder().decode(event.target.value);

  for (let c of value) {
    if (c === '\n') {
      let data = readBuffer.trim();
      readBuffer = '';

      if (data) {
        receive(data);
      }
    }
    else {
      readBuffer += c;
    }
  }
}

// Received data handling
function receive(data) {
  // log(data, 'in');
}

function send(data) {
  data = String(data);

  if (!data || !characteristicCache) {
    return;
  }

  data += '\n';

  if (data.length > 20) {
    let chunks = data.match(/(.|[\r\n]){1,20}/g);

    writeToCharacteristic(characteristicCache, chunks[0]);

    for (let i = 1; i < chunks.length; i++) {
      setTimeout(() => {
        writeToCharacteristic(characteristicCache, chunks[i]);
      }, i * 100);
    }
  }
  else {
    writeToCharacteristic(characteristicCache, data);
  }

  // log(data, 'out');
}

function writeToCharacteristic(characteristic, data) {
  characteristic.writeValue(new TextEncoder().encode(data));
}