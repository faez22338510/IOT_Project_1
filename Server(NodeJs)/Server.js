var express = require('express');
 var http = require('http');
 var path = require('path');

 var app = express();

 // all environments
 app.set('port', process.env.PORT || 3000);
 app.set('views', __dirname + '/views');
 app.set('view engine', 'jade');
 app.use(express.favicon());
 app.use(express.logger('dev'));
 app.use(express.bodyParser());
 app.use(express.methodOverride());
 app.use(app.router);
 app.use(express.static(path.join(__dirname, 'public')));

 var net = require('net');

 var sockets = [];
 var mainsocket;

 var humidity = null;
 var soil_moisture= null;
 var temprture = null;


 function receiveData(data) {
     // console.log(String.fromCharCode(data[0]));
     // console.log(data.length);
     // console.log(data);
     try {
         var sens = "";
         for (var i = 0; i < data.length; i++) {
             sens += String.fromCharCode(data[i]);
         }
         console.log(sens);
         sens = sens.split(" ");
         var stringArray = new Array();
         for (var i = 0; i < sens.length; i++) {
             stringArray.push(sens[i]);
             if (i != sens.length - 1) {
                 stringArray.push(" ");
             }
         }
         temprture = sens[0];
         soil_moisture = sens[1];
         humidity = sens[2];
         mainsocket.write("OK");
     }
     catch (ex){
         console.log(ex);
     }
     // for(var i = 0; i<sockets.length; i++) {
     //     sockets[i].write(data);
     // }
 }
//-------------------------------------------------------
 function closeSocket(socket) {
     var i = sockets.indexOf(socket);
     if (i != -1) {
         sockets.splice(i, 1);
     }
 }
//---------------------------------------------------------
 app.get('/on', function(req, res){
     mainsocket.write("on");
     res.json({Motor:"ON"});
 });

 app.get('/off', function(req, res){
     mainsocket.write("off");
     res.json({Motor:"OFF"});
 });

 app.get('/a', function(req, res){
     mainsocket.write("a");
     res.json({name:"hamid"});
 });

 app.get('/b', function(req, res){
     mainsocket.write("send to client\n");
     res.json({
         Temperature:temprture,
         Soil_moisture:soil_moisture,
         Humidity:humidity
     });
 });
 // app.put('/flight/:number/arrived', routes.arrived);
 //-------------------------------------------------------

 function newSocket(socket) {
     mainsocket = socket;
     sockets.push(socket);
     socket.write('Welcome to the Telnet server!\n');
     socket.on('data', function(data) {
         receiveData(data);
     })
     socket.on('end', function() {
         closeSocket(socket);
     })
     socket.on('error', function(err) {
         console.log(err);
         console.log("\n Error \n");
     });
 }

 // Create a new server and provide a callback for when a connection occurs
 var server = net.createServer(newSocket);

 // Listen on port 2000
 server.listen(2000,function () {
     console.log('Tellnet server listening on port 2000');
 });

 http.createServer(app).listen(app.get('port'), function(){
     console.log('Express server listening on port ' + app.get('port'));
 });