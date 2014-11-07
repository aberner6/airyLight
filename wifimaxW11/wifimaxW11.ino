
#include <WiFiClient.h>
#include <WiFiServer.h>

/*
  AirNow Web Scraper
 Context: Arduino
 */
#include <SPI.h>
#include <Ethernet.h>
#include <TextFinder.h>
#include <WiFi.h>

#define DIR_PIN 2
#define STEP_PIN 3

const int numReadings = 10;
int readings[numReadings];      // the readings from the analog input

int index = 0;                  // the index of the current reading
//THE BELOW MAPS FROM MOST BAD TO LEAST ?
const int AQIMax = 100;              // maximum level for air quality
const int requestInterval = 50000;//10000;  //longer// delay between updates to the server

char ssid[] = "TG1672G02";      //  your network SSID (name) 
char pass[] = "TG1672G7F1E02";    // your network password

boolean requested;                   // whether you've made a request since connecting
long lastAttemptTime = 0;            // last time you connected to the server, in milliseconds
int airQuality = 0;                  // AQI value
int todaysHigh = 0;                  // AQI Forecasted HIGH
boolean meterIsSet = false;          // whether the meter is set from the latest reading

int status = WL_IDLE_STATUS; // status of the wifi connection
WiFiClient client;
char server[] = "appfog.com";     // name address for twitter API

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
//boolean two = false;
//boolean three = true;
int meterSetting;
int thisWasSet;

int sensorMin = 100;
int newIndex = 0;
int sensorMax = 0;
boolean beenHere = false;

void setup() {
  WiFi.begin(ssid, pass);
  // // start the serial library:
  Serial.begin(9600);
  while ( status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(1000);
  } 
  // you're connected now, so print out the status:
  printWifiStatus();
  connectToServer();
  pinMode(DIR_PIN, OUTPUT); 
  pinMode(STEP_PIN, OUTPUT); 
}

void loop()
{
  if (client.connected()) {
    if (!requested) {
      requested = makeRequest(); 
    } 
    else {
      // make an instance of TextFinder to search the response:
      TextFinder response(client);
      // see if the response from the server contains the AQI value:
      if(response.find("Air Quality:"))  {
        // convert the remaining part into an integer:
        airQuality = response.getValue();
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

  for (int i = 1; i<numReadings; i++){
  }
 lookAt = index - 1;
  if(index == 0){
    lookAt = numReadings - 1;
  }
  if (readings[index] != readings[lookAt] && readings[index]!=-1){     
    airDiff = readings[index]-readings[lookAt];
    setMeter(airDiff, airQuality); 
  } 
   else {
   beNewMeter(airQuality, sensorMin, sensorMax);
  }
  index++;
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
//  if (index==0){
//      setMeter(airDiff, airQuality); 
//  }
      }
    }
  }
      else if (millis() - lastAttemptTime > requestInterval) {
    // if you're not connected, and two minutes have passed since
    // your last connection, then attempt to connect again:
    client.stop();
    connectToServer();
  }
}
void connectToServer() {
  // clear the state of the meter:
  meterIsSet = false;
//  meterIsSetTwo  = false;
  // attempt to connect, and wait a millisecond:
  Serial.println("connecting...");
  if (client.connect(server, 80)) {
    requested = false; 
  }
  // note the time of this connect attempt:
  lastAttemptTime = millis();
}
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
boolean makeRequest() {
  Serial.println("requesting...");
  // make HTTP GET request and fill in the path to
  // the PHP script on your server:
  // Make a HTTP request:
  client.println("GET /aqscraper.php HTTP/1.1");  
  // fill in your server's name:
    client.println("HOST: airylight.aws.af.cm");  
  client.println();
  return true;
}

void setMeter(int airDiff, int airQuality){
//  Serial.println("setting meter at index of:");
//  Serial.println(index);
//       Serial.println("airDiff");
//   Serial.println(airDiff);
//        Serial.println("readings at index");
//   Serial.println(readings[index]);
//        Serial.println("readings at lookAt");
//   Serial.println(readings[lookAt]);
//           Serial.println("the index before");
//      Serial.println(lookAt);
      
      
//  if (index==0){ //DON'T COMPARE TO THE PREVIOUS GUY, DON'T DO ANYTHING
//    meterSetting=0;
//  }
  
  if (goBig==false){ //if you ARE the first ever, and indexis has not been set to true go far
    meterSetting = map(airQuality, 0, 100, 0, rotateUp);
    thisWasSet = airQuality;
    Serial.println("thisWasSet");
    Serial.println(thisWasSet);
    goBig=true;
  }   
  
  if (index>=1 && readings[index]!=thisWasSet && airQuality!=-1){
   Serial.println("inside airDiff");
    // map the result to a range the meter can use:
    if(airDiff>0){  
      meterSetting = map(airDiff, 0, 100, 0, rotateUp);
    }
    else if (airDiff<0){
      meterSetting = map(airDiff, -100, 0, rotateDown, 0); 
    }
  } 
  else if (index>=1 && readings[lookAt]==thisWasSet){
   meterSetting=0; 
  }
  if (index>2 && readings[index]==thisWasSet&&airQuality!=-1){
    Serial.println("inside same as first ever setting");
    if(airDiff>0){  
      meterSetting = map(airDiff, 0, 100, 0, rotateUp);
    }
    else if (airDiff<0){
      meterSetting = map(airDiff, -100, 0, rotateDown, 0); 
    }
  }
  Serial.println("setting rotation with:"); 
  Serial.println(meterSetting);
  rotateDeg(meterSetting, .1); 

}
void beNewMeter(int airQuality, int sensorMin, int sensorMax){
    Serial.println("beNewMeter");
//if (index%5==2 && goBig==true && airQuality!=-1){
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
// rotateDeg(goToMin, .1);
// delay(1000);
//rotateDeg(-goToMin, .1);
//delay(1000);
rotateDeg(goToMax, .1);
delay(6000);
rotateDeg(-goToMax, .1);
delay(6000);
//beenHere=true;
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






