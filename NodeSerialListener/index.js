const SerialPort = require('serialport');
const parser = new SerialPort.parsers.Readline();

let appCalls = 0;
let keyboardCalls = 0;

var port = new SerialPort('COM3', { baudRate: 9600});

port.pipe(parser);

parser.on('data', (data) => { 
  console.log(data);
  
});