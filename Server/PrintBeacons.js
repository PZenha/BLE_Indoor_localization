
function PrintPositions(x,y){
	//x->0  pixel-> 0	left		y->0  pixel->0 top
	// x-> 15  pixel-> 750			y->10   pixel->500 top
	console.log("print PrintPositions");
	var xPos = (parseInt(x)*750)/15;
	var yPos = (parseInt(y)*128)/10;

	canvas(xPos,yPos);
}

function canvas(x,y){

	window.onload = function(){
		var canvas = document.getElementById("myCanvas");
		var ctx = canvas.getContext("2d");

		var img = document.getElementById("image");
		ctx.drawImage(img,0,0,750,500);

		ctx.beginPath();
		ctx.arc(x,y,10,0,2*Math.PI);
		ctx.fillStyle="blue";
		ctx.fill();
		ctx.stroke();
	};

}