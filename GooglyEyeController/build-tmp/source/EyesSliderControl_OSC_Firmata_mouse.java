import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import controlP5.*; 
import processing.serial.*; 
import cc.arduino.*; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class EyesSliderControl_OSC_Firmata_mouse extends PApplet {


// import oscP5.*;
// import netP5.*;



Arduino arduino;
// OscP5 oscP5;
// NetAddress myRemoteLocation;

int pixelsPerInch=10;
int interOccular = 500;
int eyeRad = 465/2;//46.5inch diam
int eyeMinorRad = 235/2;//23.5 inches diam
int rotationHeading = 0;
int rotationAltitude = 0;
// int pupilR =0;
// int pupilThetaDegrees = 0;
int pupilGutter= 145 -eyeMinorRad; //gutter carriage to edge is 15 and 14.5  floor to 14.5
float zoom = 2.05f;

ControlP5 cp5;
boolean mouse = true;
boolean bounce= false;
boolean OSC;
boolean eyesConected = false; //checked later on

public void setup() {
  size( displayWidth/2, displayWidth/2, P3D);
  // initFirmata();
  initControlPanel();
  frameRate(60);
  smooth();
  // ortho();
}

public void draw() {
  background(60);

// PVector eyePosition = 
   camera(-width*zoom, -height*zoom, height*zoom , width, height, 0, 0, 1, 0);
  
  noStroke();
  int xCen = width/2;
  int yCen= height/2;
  pushMatrix();
  // translate(width/2, height/2, zoom);//centers the rotation and the draw surface in the upper middle of the screen
  rotateY(radians(rotationHeading));
  rotateX(radians(rotationAltitude));
   drawGumball();
  //  eyeModel(0, 0, eyeRad, eyeMinorRad, pupilR, pupilThetaDegrees, pupilGutter);
  PVector origin = new PVector(0,0,0);
  PVector gap = new PVector(interOccular/2,0,0);
  // println("gap: "+gap);
  PVector origin1, origin2;
  origin1 = origin.get();
  origin2 = origin.get();
  origin1.add(gap);
  origin2.sub(gap);

findGazeDrawEye(origin1,planarMouse());
findGazeDrawEye(origin2,planarMouse());

planarMouse();
  // eyeModel( origin.add(gap),  pupilR, pupilThetaDegrees, pupilGutter);
  popMatrix();
    camera();
  cp5.draw();

// //  cp5.draw();//draw the sliders
//   if (mouse) {
//     simpleMouseXYControl();
//   }
//   else if (bounce) {
//     sinuasoidalBounce();
//   }
//   else if (OSC) {
//     //osc
//   }
}

public void initFirmata() {
  //ToDo: Dif between left and right eye
  int arduinoIndex = 5;
  println(Arduino.list());
  if(Arduino.list().length > 4){
  eyesConected = true;
  arduino = new Arduino(this, Arduino.list()[arduinoIndex], 57600);
}
}
public void initControlPanel() {
  cp5 = new ControlP5(this);
  bounce = true;
  cp5.setAutoDraw(false);
  cp5.addSlider("eyeRad", 1, height/2).linebreak();
  cp5.addSlider("eyeMinorRad", 10, 150).linebreak();
  cp5.addSlider("interOccular", 1, width).linebreak();
  cp5.addSlider("rotationHeading", -45, 45 ).linebreak();
  cp5.addSlider("rotationAltitude", -45, 45 ).linebreak();
  cp5.addSlider("zoom", 0.01f, 3.5f ).linebreak();
  //  cp5.addSlider("pupilR", -255, 255 ).linebreak();
  //  cp5.addSlider("pupilThetaDegrees", 0, 360 ).linebreak();
  cp5.addSlider("pupilGutter", 0, 100 ).linebreak();
  cp5.addToggle("mouse").linebreak().setState(false);
  cp5.addToggle("bounce").linebreak().setState(true);
  cp5.addToggle("OSC").linebreak().setState(false);
}


// void simpleMouseXYControl() {
//   float mouseXUnitized = map(mouseX, 0, width, -1, 1);
//   float mouseYUnitized = map(mouseY, 0, height, 1, -1);
//   float pupilR01 = sqrt((mouseXUnitized*mouseXUnitized)+(mouseYUnitized*mouseYUnitized));
//   // pupilR = int(map(pupilR01, 0, 1, 0, 255));
//   pupilThetaDegrees = int(degrees(atan2(mouseYUnitized, mouseXUnitized)));
//   pupilThetaDegrees = (pupilThetaDegrees+360)%360;
//   //  pupilR= int(map(touchOSCxParam, 0, 1, -255, 255)) ;
//   //  pupilThetaDegrees= int(map(touchOSCyParam, 0, 1, 0, 360)) ;

// //  if (mouseX < width/2) {
// //    pupilR=-pupilR;
// //    pupilThetaDegrees=(pupilThetaDegrees+180)%360;
// //  }
//   moveEye(11, 11);
// }

public void sinuasoidalBounce() {
  float sinuasoid = sin(radians(frameCount%360));
//  int discreteRotaryGoal  = int(map(sinuasoid, -1, 1, 0, 16));
  // constrain(sinuasoid,-1,1);
  int pupilR= PApplet.parseInt(map(sinuasoid, -1, 1, -255, 255));
  //  pupilThetaDegrees = int(map(sinuasoid, -1, 1, 0, 360));
  moveEye(11, 11);
}


public void moveEye(int thetaGoal, int pupilRGoal) {
  boolean debug = false;
  //Moves the physical eye to the coord specified.
  //TODO consider feedback upon goal reach
  //TODO figure out a left eye right eye solution or move them in tandem
  //Rotation: 0 to 360 CCW with zero at 3:00. 
  //linear: 0 to 360 with 180 being home

  int pupilRArduino = PApplet.parseInt(map(pupilRGoal, -255, 255, 0, 360));
     pupilRArduino = 360;


  //  int discreteRotaryGoal = (int(map(mouseY,0,height,0,16))+6)%16;
   int discreteRotaryGoal = (PApplet.parseInt(map(thetaGoal,0,360,0,16)));
  discreteRotaryGoal = (discreteRotaryGoal+6)%16;
if (debug) {
  
  print("pulilR: ", pupilRGoal);
  print("\tthetaGoal: ", thetaGoal);
  println("\tdiscreteRotaryGoal: ", discreteRotaryGoal);
}
  if (eyesConected) {
    arduino.analogWrite(2, pupilRArduino);
    arduino.analogWrite(3, discreteRotaryGoal);
  }
}


public void drawGumball() {
  int lineLength = 100;
  //draws a trident of colored lines to help with axis ID
  strokeWeight(1);
  strokeCap(ROUND);
  stroke(255, 0, 0); //x is red
  line(0, 0, lineLength, 0);
  stroke(0, 255, 0);//y is green
  line(0, 0, 0, lineLength);
  stroke(0, 0, 255);//z is blue
  line(0, 0, 0, 0, 0, lineLength);
}



public void findGazeDrawEye(PVector origin, PVector target){
  boolean debug = false;
  strokeWeight(1);
  strokeCap(ROUND);
  stroke(0, 255, 0); //x is red
  PVector eyeNormal = new PVector(0,0,1);
if(debug)line(origin.x,origin.y,origin.z,origin.x,origin.y,origin.z+400);
stroke(255, 0, 0); //x is red
if(debug)line(origin.x,origin.y,origin.z,target.x,target.y,target.z);
PVector gazeVector = target.get();
gazeVector.sub(origin);
float gazeTheta = PVector.angleBetween(eyeNormal,gazeVector);
if (gazeVector.x < 0){
  gazeTheta = -gazeTheta;
}
// println("gazeTheta: "+degrees(gazeTheta));
int pupilR = PApplet.parseInt(tan(gazeTheta)*255);
// print("\tpupilR:\t"+pupilR);
int thetaGoal = 0;
  // float gazeX;
  // float gazeY;
  // float gazeZ;
  // PVector gaze = new PVector(
  eyeModel(origin, pupilR, thetaGoal); 
  // return gaze;
}

public PVector planarMouse(){
  //returns a target that is on the screen
  noCursor();
  noStroke();
  fill(255,44);
  // controllerOrigin
  // rect(width, width-100, 0, 400);
  pushMatrix();
  rotateX(radians(90));
  int planeWidth = interOccular*2;
  int planeDepth = planeWidth*2;
  translate(-planeWidth/2, eyeRad*3, 0);
  // rect(0, 0, planeWidth, planeDepth);
  stroke(33);
  strokeWeight(1);
  int stripePitch = 99;
  for (int xStripe = 0; xStripe <= planeWidth/stripePitch; ++xStripe) {
    line(xStripe*stripePitch,0,xStripe*stripePitch,planeDepth);
    
  }
    for (int yStripe = 0; yStripe <= planeDepth/stripePitch; ++yStripe) {
    line(0,yStripe*stripePitch,planeWidth,yStripe*stripePitch);
    
  }
  popMatrix();
  stroke(255,0,255);//white
  strokeWeight(40);
  strokeCap(ROUND);
  PVector target = new PVector(map(mouseX,0,width,-planeWidth,planeWidth),0,map(mouseY,0,height,0,planeDepth*2));
  point(target.x, target.y-20,target.z);//whte dot behind eye   
  noStroke();
  return target;
}

public void eyeModel(PVector origin, int pupilR, int pupilTheta) {
// Displays a visual model of the eye with the specified params

int xCen = PApplet.parseInt(origin.x);
 int yCen = PApplet.parseInt(origin.y);
 int zCen = PApplet.parseInt(origin.z);
  int bigRad =eyeRad ;//46.5inch diam
  int smallRad = eyeMinorRad;//23.5 inches diam ////delete
  int trueMaximum = bigRad-smallRad-pupilGutter;
  float gutterLossCoeff = eyeRad/trueMaximum;
    int max255 = PApplet.parseInt(gutterLossCoeff*255);
  pupilR = constrain(pupilR, -max255, max255); //can travel backwards, meaning not truly polar coords
  pupilTheta = constrain(pupilTheta, 0, 360);//Angular position of the pupil, CCW degrees from 3 o'clock
  
  int pupilRPixels = PApplet.parseInt(map(pupilR, -255, 255, -eyeRad, eyeRad));
  pupilRPixels = constrain(pupilRPixels, -trueMaximum, trueMaximum);
 //whte dot behind eye 
  stroke(255);//white
  strokeWeight(10);
  strokeCap(ROUND);
  point(xCen, yCen, zCen);//whte dot behind eye   
  noStroke();
  //EYEBALL
  pushMatrix();
  translate(0, 0, bigRad);
  float pupilThetaRadians=radians(pupilTheta);
  fill(211);//White of entire eye
  ellipse(xCen, yCen, 2*bigRad, 2*bigRad);//White of eye
  fill(222);//White of traversable eye area
  translate(0, 0, 1);//shift 1 px forward to avoid self intercection
  ellipse(xCen, yCen, 2*(bigRad-pupilGutter), 2*(bigRad-pupilGutter));//White of eye, travelable zone
  fill(45);//Pupil Black
  translate(0, 0, 1);//shift 1 px forward to avoid self intercection
  ellipse(xCen + pupilRPixels*cos(pupilThetaRadians), yCen-pupilRPixels*sin(pupilThetaRadians), 2*smallRad, 2*smallRad);
  translate(0, 0, 1);//shift 1 px forward to avoid self intercection
  noFill();
  stroke(255);//HILIGHT WHITE
  strokeWeight(bigRad/10);
  strokeCap(SQUARE);//For the hilight
  float hightlightRad = 1.4f;
  arc(xCen, yCen, hightlightRad*bigRad, hightlightRad*bigRad, radians(207), radians(223));
  arc(xCen, yCen, hightlightRad*bigRad, hightlightRad*bigRad, radians(228), radians(239));
  noStroke();
  popMatrix();
}

// void OSCSetup(){
//   oscP5 = new OscP5(this, 12000);
//   myRemoteLocation = new NetAddress("10.140.70.36", 11111);
// }




// void oscEvent(OscMessage theOscMessage) {
//   //Is called whenever an OSC packet is recieved
//   float touchOSCxParam = 0.0;
//   float touchOSCyParam =0.0;
//   /* check if theOscMessage has the address pattern we are looking for. */
//   if (theOscMessage.checkAddrPattern("/3/xy")==true) {
//     touchOSCxParam = theOscMessage.get(0).floatValue();  
//     touchOSCyParam = theOscMessage.get(1).floatValue();  
//     //    println(" X: "+touchOSCxParam+" y: "+touchOSCyParam);
//   }

//   println("### received an osc message. with address pattern "+theOscMessage.addrPattern());
//   float newX = map(touchOSCxParam, 0, 1, -1, 1);
//   float newY = map(touchOSCyParam, 0, 1, 1, -1);//center on center
//   // println(" newX: "+newX+" newY: "+newY);

//   float pupilR01 = constrain(sqrt((newX*newX)+(newY*newY)), 0, 1);
//   pupilR = int(map(pupilR01, 0, 1, 0, 360));
 
//   pupilThetaDegrees = -int(degrees(atan2(newY, newX)));
//   //  pupilR= int(map(touchOSCxParam, 0, 1, -255, 255)) ;
//   //  pupilThetaDegrees= int(map(touchOSCyParam, 0, 1, 0, 360)) ;
//   moveEye(pupilThetaDegrees,pupilR);

// }

//
//public void controlEvent(ControlEvent theEvent) {
//  //  OscMessage thetaCommand = new OscMessage("/3/xy");
//  OscMessage thetaCommand = new OscMessage("/1/fader");
//  thetaCommand.add(1);
//  //  thetaCommand.add(",");
//  //  thetaCommand.add(0.0);
//  oscP5.send(thetaCommand, myRemoteLocation);
//}


  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "EyesSliderControl_OSC_Firmata_mouse" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
