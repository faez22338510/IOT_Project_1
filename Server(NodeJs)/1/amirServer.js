var app = require('express')()
var bodyParser = require('body-parser')
var server = require('http').createServer(app)
var io = require('socket.io')(server)
var myClient;
app.use(bodyParser.json())

app.get('/', function(req, res) {
    console.log("connet");
    res.send('Hello world!')
})

app.post('/message', function(req, res) {
    console.log(req.body.message)
    res.send('I got the message')
    myClient.emit('message', req.body.message)
})

io.on('connection', function(client) {
    myClient = client
    console.log('Client connected...')
    client.on('join', function(data) {
        console.log(data)
        client.emit('message', client.id.toString())
    })
})


server.listen(3000)
