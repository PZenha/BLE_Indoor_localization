

var beacons = [];
var scannerInfo = [];

module.exports = {


	addBeaconMain: function(major, minor, rssi, scannerId, time) {
		var beacon = this.addBeacon(major, minor);
		var scanner = this.addScannerToBeacon(beacon, rssi, scannerId, time);
		this.TimeOutControl(time);
		return beacons;
	},

	addBeacon: function(major, minor) {
		//verificar se um becon com este major e minor existe na lista
		var beacon = beacons.find(x => x.major === major && x.minor === minor);

		if(beacon === undefined){
			beacon = {
				major: major,
				minor: minor,
				posx: "0",
				posy: "0",
				scanners: []
			}
			beacons.push(beacon);
		}

		//se existir return beacon existente
		return beacon;
	},


	addScannerToBeacon: function(beacon, rssi, scannerId, time) {
		var scanner = beacon.scanners.find(x => x.id === scannerId);
		if(scanner === undefined){
			scanner = {
				rssi: rssi,
				id: scannerId,
				time: time
			}
			beacon.scanners.push(scanner);
		}else{
			var IdPos = beacon.scanners.findIndex(x => x.id === scannerId);  //Find the array pos of the ID
			console.log("IDPOS");
			console.log(IdPos);
			beacon.scanners.splice(IdPos,1);			//Remove the elements of IDpos
			
			scanner = {
				rssi: rssi,
				id: scannerId,
				time: time
			}
			beacon.scanners.push(scanner);  //Update the rssi for the same beacon
		}		
		return scanner;
	},

	//If current time is 15 seconds superior to a registered beacon at the list we remove it
	TimeOutControl: function(time){		
		for(var i = 0; i < beacons.length; i++){
			for(var j = 0; j < beacons[i].scanners.length; j++){
				if(time - beacons[i].scanners[j].time >= 40){
					beacons[i].scanners.splice(j,1);
				}
			}
		}
	},

	//If the beacon was saw by 3 scanners at least
	CheckForThreeScanners: function(i){		
		if(beacons[i].scanners.length >= 3){  
			return true;
		}
	},

	//Sort by descending order the RSSI value
	BubbleSortRSSI: function(pos){
		for(var i = 0; i < beacons[pos].scanners.length - 1; i++){
			for(var j = i +1; j < beacons[pos].scanners.length; j++){
				if(parseInt(beacons[pos].scanners[j].rssi) > parseInt(beacons[pos].scanners[i].rssi)){
					var temp = beacons[pos].scanners[i];
					beacons[pos].scanners[i] = beacons[pos].scanners[j];
					beacons[pos].scanners[j] = temp;
				}
			}
		}
		console.log("bubble");
		console.log(JSON.stringify(beacons));
	},

	//This function will convert RSSI to meters
	RssiToMeters:function(rssi){
    	var meters = (parseInt(rssi) + 51.41)/-5.22;  //y = -5,215x - 51,413      (y + 53,52)/ -4,24
    	//var meters = 10^((60 - (rssi))/(10*2));
    	return meters;
	},

	//This method will return the actual postion of the beacon inside the store
	MonteCarloMethod: function(scanner1, scanner2, scanner3) {

		var Position = {X:"0",Y:"0"};

		var scanner1PosX = parseInt(scanner1.PosX);
		var scanner1PosY = parseInt(scanner1.PosY);
		var scanner1R = parseInt(scanner1.r);
		var scanner2PosX = parseInt(scanner2.PosX);
		var scanner2PosY = parseInt(scanner2.PosY);
		var scanner2R = parseInt(scanner2.r);
		var scanner3PosX = parseInt(scanner3.PosX);
		var scanner3PosY = parseInt(scanner3.PosY);
		var scanner3R = parseInt(scanner3.r);


		var bound = this.boundingRectangle(scannerInfo);
		var boundS = "x:" + bound.x + " y:" + bound.y + " height:" + bound.height + " width:" + bound.width; 
		console.log(boundS);

		  
		/*// determine bounding rectangle
		var left   = Math.min(scanner1PosX - scanner1R, scanner2PosX - scanner2R, scanner3PosX - scanner3R);
		var right  = Math.max(scanner1PosX + scanner1R, scanner2PosX + scanner2R, scanner3PosX + scanner3R);
		var top    = Math.min(scanner1PosY - scanner1R, scanner2PosY - scanner2R, scanner3PosY - scanner3R);
		var bottom = Math.max(scanner1PosY + scanner1R, scanner2PosY + scanner2R, scanner3PosY + scanner3R);
		*/
		

		// area of bounding rectangle
		//var rectArea = (right - left) * (bottom - top);

		var iterations = 1000000;
		var pts = 0;
		for (var i=0; i<iterations; i++) {

			// random point coordinates
			Position.x =  bound.x + Math.random()* bound.width;
			Position.y =  bound.y + Math.random()* bound.height;


			if(this.containedInCircles(Position)){
				break;
			}

			/*Position.X =  Math.random()*(7.5 - 0.1) + 0.1;
			Position.Y =  Math.random()*(4 - 0.5) + 0.5; */
   
			/*// check if it is inside all the three circles (the intersecting area)
			if (Math.sqrt(Math.pow(Position.X - scanner1PosX, 2) + Math.pow(Position.Y - scanner1PosY, 2)) <= scanner1R &&
			    Math.sqrt(Math.pow(Position.X - scanner2PosX, 2) + Math.pow(Position.Y - scanner2PosY, 2)) <= scanner2R &&
			    Math.sqrt(Math.pow(Position.X - scanner3PosX, 2) + Math.pow(Position.Y - scanner3PosY, 2)) <= scanner3R){ 
				console.log("Cheguei até aqui");
				console.log(i);
			    break;
			} */
		}
			var boudS = "bound.X:" + bound.x + " bound.Y:" + bound.y + " bound.width:" + bound.width + " bound.height:" + bound.height;
			console.log(boudS);

			var MonteCString = "Monte Carlo Method X: " + Position.x + " Y:" + Position.y + "\r\n";
			console.log(MonteCString);
			console.log(i);
			// Returns the position of the point inside all three circles
			return Position;
	},

	circleCircleIntersec: function(p1,p2){
		var d = this.distance(p1,p2);

			var r1 = p1.r;
			var r2 = p2.r;

			if((d >= (r1+r2) || d <= Math.abs(r1 - r2))){
				return [];
			}

			var a = (r1 * r1 - r2 * r2 + d * d) / (2 * d);
	        var h = Math.sqrt(r1 * r1 - a * a);
	        var x0 = p1.PosX + a * (p2.PosX - p1.PosX) / d;
	        var y0 = p1.PosY + a * (p2.PosY - p1.PosY) / d;
	        var rx = -(p2.PosY - p1.PosY) * (h / d);
	        var ry = -(p2.PosX - p1.PosX) * (h / d);


	        return [{ x: x0 + rx, y : y0 - ry },
	                { x: x0 - rx, y : y0 + ry }];
    },
	

	//This function have all scanners positions
	distance: function(p1,p2){
		return Math.sqrt(Math.pow(p1.PosX - p2.PosX,2) + Math.pow(p1.PosY - p2.PosY,2));
	},

	distancePoint: function(p1,p2){
		return Math.sqrt(Math.pow(p1.x - p2.PosX,2) + Math.pow(p1.y - p2.PosY,2));
	},

	intersectPoints: function(scannerInfo){
		var ret = [];
		for(var i = 0; i < scannerInfo.length; i++){
			for(var j = i + 1; j < scannerInfo.length; j++){
				var intersect = this.circleCircleIntersec(scannerInfo[i],scannerInfo[j]);

				 for(var k = 0; k < intersect.length; k++){
				 	var p = intersect[k];
				 	p.parentIndex = [i,j];
				 	ret.push(p);
				 }

			}
		}
		return ret;
	},

	containedInCircles: function(p){
		for(var i = 0; i < scannerInfo.length; i++){
			if(this.distancePoint(p,scannerInfo[i]) > scannerInfo[i].r){
				return false;
			}
		}
		return true;
	},

	boundingRectangle: function(scannerInfo){
		var intersectP = this.intersectPoints(scannerInfo);


		for(var i = 0; i < intersectP.length; i++){
			if(Math.sqrt(Math.pow(intersectP[i].x - scannerInfo[i].PosX,2) + Math.pow(intersectP[i].y - scannerInfo[i].PosY,2) > scannerInfo[i].r )){
				intersectP.splice(i,1);
			}
		}

		var x1 = Math.min.apply(null, intersectP.map(function(p){ return p.x; }));
		var y1 = Math.min.apply(null, intersectP.map(function(p){ return p.y; }));
		var x2 = Math.max.apply(null, intersectP.map(function(p){ return p.x; }));
		var y2 = Math.max.apply(null, intersectP.map(function(p){ return p.y; }));

		var XXString = "x1:" + x1 + " y1:" + y1 + " x2:" + x2 + " y2:" + y2;
		console.log(XXString);

		for(var i = 0; i < 3; i++){
			var p = scannerInfo[i];
			if(((p.PosX - p.r) < x1) && (this.containedInCircles({x: p.PosX - p.r, y:p.PosY}))){
				x1 = p.PosX - p.r;
			}
			if(((p.PosX + p.r) > x2) && (this.containedInCircles({x: p.PosX + p.r, y:p.PosY}))){
				x2 = p.PosX + p.r;
			}
			if(((p.PosY - p.r) < y1) && (this.containedInCircles({y: p.PosY - p.r, x:p.PosX}))){
				y1 = p.PosY - p.r;
			}
			if(((p.PosY + p.r) < y1) && (this.containedInCircles({y: p.PosY + p.r, x:p.PosX}))){
				y2 = p.PosY + p.r;
			}
		}
		return {x: x1, y: y1, height: y2 - y1, width: x2 - x1};
	},

	ScannerPosition: function(MACaddr){
			var pos = {x:"0",y:"0"};
		if(MACaddr === "30:AE:A4:25:05:BC"){ //85
			pos.x = 7.5;
			pos.y = 3.5;
		}else
		if(MACaddr === "30:AE:A4:14:B3:C4"){ //consola
			pos.x = 0.1;
			pos.y = 2;
		}else
		if(MACaddr === "30:AE:A4:37:E0:34"){//34
			pos.x = 7.5;
			pos.y = 0.5;
		}else
		if(MACaddr === "44:44:44:44"){
			pos.x = 7;
			pos.y = 6;
		}else
		if(MACaddr === "55:55:55:55"){
			pos.x = 10;
			pos.y = 8;
		}
		if(MACaddr === "66:66:66:66"){
			pos.x = 15;
			pos.y = 8;
		}
		return pos;
	},

	//This function will save the three scanners info
	GetAllScannersInfo: function(i){
		
			for(var j = 0; j < 3; j++){
				var Position = this.ScannerPosition(beacons[i].scanners[j].id);
				var radius = this.RssiToMeters(beacons[i].scanners[j].rssi);
				scannerInfo.push({
					PosX : Position.x,
					PosY : Position.y,
					r : radius
				});
		}
	},

	UpdateBeaconsPositions: function(){
		
		for(var i=0; i < beacons.length; i++){
			var foundByThree = this.CheckForThreeScanners(i);  //Return true if beacon was found by three scanners
			if(foundByThree == true){
				this.BubbleSortRSSI(i);
				this.GetAllScannersInfo(i);
				var pos = this.MonteCarloMethod(scannerInfo[0],scannerInfo[1],scannerInfo[2]);
				beacons[i].posx = pos.x;
				beacons[i].posy = pos.y;
			}	
		}
	},

	//This function will be used on nodejs server side to calculate all positions of all beacons
	ShowBeaconsPositions: function(time){	
		this.TimeOutControl(time);
		var AllBeaconsPos = [];
		this.UpdateBeaconsPositions();
		for(var i = 0; i < beacons.length; i++){
			if(beacons[i].posx != 0){
				AllBeaconsPos.push({
				major: beacons[i].major,
				minor: beacons[i].minor,
				posx: beacons[i].posx,
				posy: beacons[i].posy
				});
			}
		}
		this.CleanPositionStruct();
		return AllBeaconsPos;
	},

	//Erase past position history
	CleanPositionStruct: function(){
		for(var i=0; i < beacons.length; i++){
			beacons[i].posx = 0;
			beacons[i].posy = 0;
		}
	},

	eraseBuffer: function(){
		scannerInfo.splice(0,scannerInfo.length);
	}
}