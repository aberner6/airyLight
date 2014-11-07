/*
  AirNow Web Scraper
 Context: Arduino
 */
#include <SPI.h>

#define DIR_PIN 2
#define STEP_PIN 3

const int numReadings = 10;
int readings[numReadings];      // the readings from the analog input

int index = 0;                  // the index of the current reading
//THE BELOW MAPS FROM MOST BAD TO LEAST ?
const int AQIMax = 100;              // maximum level for air quality
const int requestInterval = 20000;//10000;  //longer// delay between updates to the server

boolean requested;                   // whether you've made a request since connecting
long lastAttemptTime = 0;            // last time you connected to the server, in milliseconds
int airQuality = 0;                  // AQI value
int todaysHigh = 0;                  // AQI Forecasted HIGH
boolean meterIsSet = false;          // whether the meter is set from the latest reading

//int rotateUp = -1200; //changed this wednes
//int rotateDown = 1200;
int rotateUp = 180; //changed this wednes
int rotateDown = -180;
int airDiff = 0;

int rotateBy = 0;
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int lookAt;
boolean one = false;
boolean indexis = false;
boolean goBig = false;

int meterSetting;
int thisWasSet;

int sensorMin = 100;
int newIndex = 0;
int sensorMax = 0;
boolean beenHere = false;

void setup() {
  // // start the serial library:
  Serial.begin(9600);
  pinMode(DIR_PIN, OUTPUT); 
  pinMode(STEP_PIN, OUTPUT); 
}

void loop()
{
        airQuality = 70;//response.getValue();
        // set the meter:
        while ((millis()<5000) && airQuality!=-1 && airQuality!=0){
         if (airQuality>sensorMax){
          sensorMax=airQuality; 
         }
         if (airQuality<sensorMin){
         sensorMin = airQuality;
         }
        }
        Serial.println(airQuality);
        readings[index] = airQuality; 
 lookAt = index - 1;
  if(index == 0){
    lookAt = numReadings - 1;
  }
   else {
     
   beNewMeter(airQuality, sensorMin, sensorMax);
  }
  index++;
  Serial.println(index);
  Serial.println("index");
  if (goBig==false){
     setMeter(airDiff, airQuality); 
  }
  // if we're at the end of the array...
  if (index >= numReadings){              
    // ...wrap around to the beginning: 
    index = 0;
    //AFTER THE FIRST RESET, NO LONGER USE THE INDEX AS 0 AS THE METHOD TO SAY "BE THE FIRST READING AND ROTATE ALL THE WAY TO THE CURRENT POSITION)
    indexis = true;    
  }
}

void setMeter(int airDiff, int airQuality){

//  Serial.println(index+"index");
  if (goBig==false){ //if you ARE the first ever, and indexis has not been set to true go far
    meterSetting = map(airQuality, 0, 100, 0, rotateUp);
    thisWasSet = airQuality;
    Serial.println("thisWasSet");
    Serial.println(thisWasSet);
    goBig=true;
  }   
  if (index>=1){
      meterSetting = map(airQuality, 0, 100, 0, rotateUp);
  }
  rotateDeg(meterSetting, .1); 

}
void beNewMeter(int airQuality, int sensorMin, int sensorMax){
    Serial.println("beNewMeter");
   Serial.println(index);
  Serial.println("index");
if (index%5==4){

  Serial.println("insideFirst");
    int newSetting = map(airQuality, 0, 100, 0, rotateUp);
    Serial.println(newSetting); //this is the air quality rotation
        int goToMin = map(0, 0, 100, 0, rotateUp);
        int goToMax = map(100, 0, 100, 0, rotateUp);
        Serial.println("thisismin");
Serial.println(goToMin);
Serial.println("thisismax");
Serial.println(goToMax);
      rotateDeg(-newSetting, .1);
 delay(6000);

rotateDeg(goToMax, .1);
delay(6000);
rotateDeg(-goToMax, .1);
delay(6000);
goBig=false; //AH WILL THIS WORK???
}
}

void rotateDeg(float deg, float speed){ 
  //rotate a specific number of degrees (negitive for reverse movement)
  //speed is any number from .01 -> 1 with 1 being fastest - Slower is stronger
  int dir = (deg > 0)? HIGH:LOW;
  digitalWrite(DIR_PIN,dir); 

  int steps = abs(deg)*(1/0.225);
  float usDelay = (1/speed) * 70;

  for(int i=0; i < steps; i++){ 
    digitalWrite(STEP_PIN, HIGH); 
    delayMicroseconds(usDelay); 

    digitalWrite(STEP_PIN, LOW); 
    delayMicroseconds(usDelay); 
  } 
}






