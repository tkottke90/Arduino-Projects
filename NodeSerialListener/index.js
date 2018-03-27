'use strict'

const SerialPort = require('serialport');
const parsers = SerialPort.parsers;

const parser = new parsers.Readline({
  delimiter: '\r\n'
});

var port = new SerialPort('COM3', { baudRate: 9600});

port.pipe(parser);

port.on('open', () => {
  console.log(`Ardiuno Port open on: COM3`);
});

// port.on('data', (data) => {
//   console.log(data);
// })

parser.on('data', console.log);