#!/usr/bin/env node

var log = require("./factory");

var readline = require('readline');


var f = new log.Factory();

var rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout,
  terminal: false
});

rl.on('line', function(line){
	f.parseLine( line );
});

rl.on('close', function()
		{
			console.log("generating Report");
			f.dump();
		});



