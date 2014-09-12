
// A HTML range slider
var sliderPositionLens;
var sliderPositionLight;
var heightOnWall = 300;

var wallHeight = 600;
var margin = 100;

var radius = 10;
var grow = +1;
var d = 1;

function setup() {
  createCanvas(720, wallHeight+margin);
  // hue, saturation, and brightness
  // colorMode(HSB, 255);
  // slider has a range between 0 and 255 with a starting value of 127
  sliderPositionLens = createSlider(0, wallHeight, wallHeight);

  sliderPositionLight = createSlider(0, wallHeight, wallHeight);

}

function draw() {
  background(255);
  strokeWeight(1);

  // Set the hue according to the slider
  stroke(0, sliderPositionLens.value(), 255);
  fill(0, sliderPositionLens.value(), 255, 127);
  ellipse(200, sliderPositionLens.value(), 20, 20);

  line(0, sliderPositionLight.value(), 200, sliderPositionLight.value());  

radius = map(sliderPositionLens.value(),0,wallHeight,-100,wallHeight/2.5);
// radius = sliderPositionLens.value()/5;
  for(var i = 0; i < 100;){
    stroke(0);
   strokeWeight(.1);
    i = i+50;
    ellipse(200-75+i,85,radius,radius);
    ellipse(200,10+i,radius,radius);
  }

}