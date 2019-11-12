
//onload function
function main()
{
	//get <canvas> elementFromPoint
	var canvas = document.getElementById("AMC");
	if(!canvas)
		console.log("obtaining Canvas Failed\n");
	else
		console.log("Obtaining Canvas Succeeded\n");
	
	//print canvas width and height on console
	console.log("Canvas Width : "+canvas.width+" And Canvas Height : "+canvas.height);
	
	//get 2D contextual
	var context = canvas.getContext("2d");
	if(!context)
		console.log("Obtaining 2D Context Failed\n");
	else
		console.log("Obtaining 2D Context Succeeded.\n");
	
	//fill canvas with black colorDepth
	context.fillStyle="black"; //"#000000"
	context.fillRect(0,0,canvas.width,canvas.height);
	
	//clear the text
	context.textAlign="center"; // center horizontal
	context.textBaselne="middle"; // Center vertically
	
	//text
	var str = "Hello World !!!";
	
	//text font
	context.font="48px sans-serif";
	
	//text color
	context.fillStyle="white"; //"#FFFFFF"
	
	//display the text in center
	context.fillText(str,canvas.width/2, canvas.height/2);
	
}