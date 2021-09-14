#include <Joystick.h>

Joystick_ Joystick(0x03, 0x04, 0, 0, false, false, false, false, false, false, true, false, true, true, true);

#define rudderPin A2
#define leftBrakePin A1
#define rightBrakePin A0
#define bufferlength 100 
#define compareStep 5
#define checkerDivider 10

void setup() {
  Joystick.begin(false);  // the Joystick.sendState method must be called to update the game controller state.
  Joystick.setRudderRange(0, 1023);
  Joystick.setAcceleratorRange(1000, 750);
  Joystick.setBrakeRange(950, 700);

  Serial.begin(9600);
}

int rudder;
int abuffer[bufferlength];
int bufferIndex;
boolean active;
int checker = 0;

void loop() {

  delay(100);
  
//  Joystick.setRudder(analogRead(rudderPin));
  rudder = analogRead(rudderPin);
  Joystick.setRudder(rudder);
  Joystick.setBrake(analogRead(leftBrakePin));
  Joystick.setAccelerator(analogRead(rightBrakePin));

  Serial.print("Rudder value: ");
  Serial.println(rudder);


  bufferIndex = (bufferIndex + 1) % bufferlength;
  if(checker==0){
    abuffer[bufferIndex] = rudder;
    active = activity();
  }
  checker = (checker + 1) % checkerDivider;

  Serial.print("Checker value: ");
  Serial.println(checker);


  digitalWrite(LED_BUILTIN, active);
  if(active){
    Joystick.sendState();
  }
}

boolean activity(){
  int value = abuffer[0];
  boolean r = true;
  for(int i=compareStep; i<bufferlength; i+=compareStep){
    if(abuffer[i]!=value){
      r = false;  
    }
  }

  Serial.print("Checking for activity... ");
  Serial.println(r);
  
  return r;
}
