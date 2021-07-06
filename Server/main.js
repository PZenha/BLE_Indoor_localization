var express = require('express');
var bodyParser = require('body-parser');
var app = express();
var fs = require('fs');
var dateFormat = require('dateformat');
var ip = require("ip");
var io = require('socket.io')(http);
var http = require('http').Server(app);
var moment = require('moment')
var beaconList = require('./beacon.js');



console.log(ip.address());


app.set('views', __dirname + '/views');
app.set('view engine','ejs');

app.use(express.static(__dirname + '/views'));

app.get('/', (req,res) => {
	
	var epoch = Math.round((new Date).getTime() / 1000)

	var BeaconPosition = beaconList.ShowBeaconsPositions(epoch);
	console.log(JSON.stringify(BeaconPosition));
	

	if(BeaconPosition[0] === undefined){
		BeaconPosition[0] ={
			posx : "-1",
			posy : "-1"
		}
	}
	res.render('index',{data: BeaconPosition[0]});

	beaconList.eraseBuffer();
});


app.use(bodyParser.urlencoded({ extended: false }));


app.post('/', (req,res) =>{
	var now = new Date();
	var epoch = Math.round((new Date).getTime() / 1000)
	var body = req.body;
	var s = "Major:" + body.major + " Minor:" + body.minor + " RSSI;" + body.rssi + "; " + "MAC:" + body.MAC + " ->" + dateFormat(now)+"\r\n";
	console.log(s);
	res.end("ok");
	
	fs.appendFile("MyiBeacons.txt", s, function(err) {
    	if(err) throw err ;
	});

	var vv = beaconList.addBeaconMain(body.major,body.minor,body.rssi,body.MAC,epoch);
	console.log("returned beacon");
	console.log(JSON.stringify(vv));
})



http.listen(8080,() => console.log('Listening on port 8080'));
