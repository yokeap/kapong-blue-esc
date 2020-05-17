#include <Servo.h>

const byte servoPins[] = {6,7,8,9};
const byte _lightPin = 11;
     
Servo thruster[4];

String inString = "";    // string to hold input
int n = 0;
bool blFlag = 0;
int value[4], lastVal[4];
unsigned long timeCount,last_time;

int zeroThrottle = 90; //90 Needs to be between 66 and 101.
int maxThrottle = 120; //120 seems to be where the maximum is. Above this doesn't give any extra speed.
int minThrottle = 60;  //60 This represents the "reverse" speed.

void setup() {
  pinMode(_lightPin, OUTPUT); 
  for(int n = 0; n < 4; n++){
    thruster[n].attach(servoPins[n]);
    //zero Throttle
    thruster[n].writeMicroseconds(1500); // send "stop" signal to ESC.
  }
  Serial.begin(115200);
  Serial1.begin(115200);
  delay(5000);       // This delay allows the ESC to be connected and powered on. The motor will beep once on
                    // power up and once when it recognises the zero position.


  for(int n = 0; n < 4; n++){
      //max Throttle
      thruster[n].writeMicroseconds(2000); // send "stop" signal to ESC.
    }
   delay(2000);

  for(int n = 0; n < 4; n++){
      //max Throttle
      thruster[n].writeMicroseconds(1500); // send "stop" signal to ESC.
    }
   delay(2000);

   for(int n = 0; n < 4; n++){
      //max Throttle
      thruster[n].writeMicroseconds(1000); // send "stop" signal to ESC.
    }
   delay(20);

   for(int n = 0; n < 4; n++){
      //max Throttle
      thruster[n].writeMicroseconds(1500); // send "stop" signal to ESC.
    }
   delay(20);
                    
  delay(5000); // delay to allow the ESC to recognize the stopped signal
  Serial.println("\n\nTest:");
}

void loop() {
  timeCount = millis();

  if((timeCount - last_time) > 2000) {
    last_time = timeCount;
    for(int i = 0; i < 4 ; i++){
     value[i] = 0;
    }
    blFlag = true;
    Serial.println("Reset");
  }
  if (Serial1.available() > 0) {
    //Serial.print((char)Serial1.read());
    String inStringRAW = Serial1.readStringUntil('\n');
    inStringRAW += '\n'; 
    Serial.println(inStringRAW);
    Serial.println();
    int index = 0;
    n = 0;
    int count = 0;
     while(inStringRAW.charAt(index) != '\n'){
      if ((isDigit(inStringRAW.charAt(index))) || (inStringRAW.charAt(index)  == '-')) {
        inString += inStringRAW.charAt(index);
        count = 0;
      }
      if (inStringRAW.charAt(index)  == ',') {
        Serial.print("\nValue:");
        Serial.println(inString.toInt());
        Serial.print("String: ");
        Serial.println(inString);
        Serial.print("POS: ");
        Serial.println(n);
        value[n] = inString.toInt();
        
        // clear the string for new input:
        inString = "";

        //increase Thruster Position
        n++;
        count = 0;
      }
      index++;
      blFlag = true;
      count++;
      if(count > 100) {
        // clear the string for new input:
        inString = "";

        //increase Thruster Position
        n++;
        blFlag = false;
        break;
      }
    }
    last_time = timeCount;
  }
  if(blFlag){
    blFlag = false;
    H_Control();
  }
}

void serialEvent1() {
  while (Serial1.available()) {
    char inChar = (char)Serial1.read();
    rs485InputString += inChar;
    if (inChar == '\r') {
      rs485StringComplete = true;
    }
  }
}

bool process(){

  
  return 1;
}

bool H_Control()
{
  int i = 0;
  for(i = 0; i < 4 ; i++){
    if(value[i] < -100) value[i] = -100;
    if(value[i] > 100) value[i] = 100;

    //check crossing forward or backward and send stop signal before move
    if((value[i] > 0) && (lastVal[i] < 0)) {
      thruster[i].writeMicroseconds( map(50, -100, 100, 1000, 2000)); // Send stop signal signal to ESC.
    }
    
    else if((value[i] < 0) && (lastVal[i] > 0)) {
      thruster[i].writeMicroseconds( map(50, -100, 100, 1000, 2000)); // Send stop signal signal to ESC.
    }
    else {
    thruster[i].writeMicroseconds( map(value[i], -100, 100, 1000, 2000)); // Send signal to ESC.
    }
    lastVal[i] = value[i];
    return 1;
  }
}
