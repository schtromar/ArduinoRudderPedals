#include <Joystick.h>

//Check the library github to make sense of this: https://github.com/MHeironimus/ArduinoJoystickLibrary
Joystick_ Joystick(0x03, 0x04, 0, 0, false, false, false, false, false, false, true, false, true, true, true);

//Analog pins for pots
#define rudderPin A2
#define leftBrakePin A1
#define rightBrakePin A0

//Time limits for standby and sleep modes.
//Standby mode stops sending HID events to pc, but still looks for a change on the rudder input.
//A change over standbyLimit puts the system back into active mode.
//Sleep mode does not do that, the pedals can only be reactivated by pulling enablepin down to GND.
#define standbyLimit 524280
#define sleepLimit 8388607
#define standbyLimit 10
#define enablepin 4
#define disablepin 9

//Set debug to true to enable sending raw data to serial at 9600 baud
#define debug false

void setup() {
  Joystick.begin(false);
  //Change the ranges so they make sense. Enable debug mode to see raw values of pots.
  Joystick.setRudderRange(0, 1023);
  Joystick.setAcceleratorRange(900, 710);
  Joystick.setBrakeRange(920, 660);

  pinMode(enablepin, INPUT);
  pinMode(disablepin, INPUT);

  digitalWrite(enablepin, 1);
  digitalWrite(disablepin, 1);

#if debug
  Serial.begin(9600);
#endif
  
}

int rudder;
int left;
int right;
int lastRudder;
long counter = 0;
boolean active = true;
boolean sleeping = false;

void loop() {

  delay(1);
  
//  Joystick.setRudder(analogRead(rudderPin));
  rudder = analogRead(rudderPin);
  Joystick.setRudder(rudder);
  left = analogRead(leftBrakePin);
  Joystick.setBrake(left);
  right = analogRead(rightBrakePin);
  Joystick.setAccelerator(right);

  
#if debug
  Serial.print("Rudder: ");
  Serial.print(rudder);

  Serial.print(" Left: ");
  Serial.print(left);

  Serial.print(" Right: ");
  Serial.print(right);
  
  Serial.print(" A: ");
  Serial.print(active);

  Serial.print(" S: ");
  Serial.print(sleeping);

  Serial.print(" C: ");
  Serial.println(counter);
#endif

  //Black magic I can't explain, I was in the zone(tm) while coding this.
  if((!active) && (counter >= sleepLimit)){
      sleeping = true;  
  }else if(active && (counter >= standbyLimit)){
    lastRudder = rudder;
    counter = 0;
    active = false;
  }else if(difference(rudder, lastRudder)){
    counter++;
  }else{
    counter = 0;
    lastRudder = rudder;
    active = true;
  }

  if(digitalRead(enablepin)==0){
    active = true;
    sleeping = false;
  }
  if(digitalRead(disablepin)==0){
    active = false;
  }

  //Uncomment if you want, but you'll see activity on TX anyway.
  //digitalWrite(LED_BUILTIN, active);
  if(active){
    Joystick.sendState();
  }
}

//Such compare much wow
boolean difference(int a, int b){
    return abs(a-b)<=standbyLimit;
}
