#include <Servo.h>

const byte servoPins[] = {6,7,8,9,10};
const byte _lightPin = 11;
     
Servo thruster[5];

String inString = "";    // string to hold input
int n = 0;
bool blFlag = 0;
int value[8];
unsigned long timeCount,last_time;

void setup() {
  pinMode(_lightPin, OUTPUT); 
  for(int n = 0; n < 5; n++){
    thruster[n].attach(servoPins[n]);
    thruster[n].writeMicroseconds(1500); // send "stop" signal to ESC.
  }
  Serial.begin(115200);
  Serial1.begin(115200);
  delay(7000); // delay to allow the ESC to recognize the stopped signal
  Serial.println("\n\nTest:");
}

void loop() {
  timeCount = millis();

  if((timeCount - last_time) > 2000) {
    last_time = timeCount;
    for(int i = 0; i < 5 ; i++){
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
      if(count > 1000) {
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

void H_Control()
{
  int i = 0;
  for(i = 0; i < 5 ; i++){
    if((value[i] <= -100) || (value[i]) >= 100) break;
    thruster[i].writeMicroseconds( map(value[i], -100, 100, 1000, 2000)); // Send signal to ESC.
  }
    analogWrite(_lightPin, map(abs(value[5]), 0, 100, 0, 255));
}
