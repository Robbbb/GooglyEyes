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

