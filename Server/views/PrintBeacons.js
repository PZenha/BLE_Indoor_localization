
function PrintPositions(x,y){
	//x->0  pixel-> 0	left		y->0  pixel->0 top
	// x-> 7.5  pixel-> 750			y->4   pixel->500 top
	var xPos = x*100;
	var yPos = (y*100)*-1;

	console.log("xPos and yPos");
	console.log("" + xPos + ":" + yPos);

	canvas(xPos,yPos);
}

function canvas(x,y){

	window.onload = function(){
		var canvas = document.getElementById("myCanvas");
		var ctx = canvas.getContext("2d");

		var img = document.getElementById("image");
		ctx.drawImage(img,0,0,750,400);

		ctx.beginPath();
		ctx.translate(0,400);

		ctx.arc(x,y,10,0,2*Math.PI);
		ctx.fillStyle="blue";
		ctx.fill();
		ctx.stroke();
	};

}

function PrintNoDots(){
	window.onload = function(){
		var canvas = document.getElementById("myCanvas");
		var ctx = canvas.getContext("2d");
		var img = document.getElementById("image");
		ctx.drawImage(img,0,0,750,500);
	}
}