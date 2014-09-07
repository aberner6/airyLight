
#include <WiFiClient.h>
#include <WiFiServer.h>

/*
  AirNow Web Scraper
 Context: Arduino
 */
#include <SPI.h>
//#include <Ethernet.h>
#include <TextFinder.h>
#include <WiFi.h>

#define DIR_PIN 2
#define STEP_PIN 3

/////////////////addition to show connection
const int connectedLED = 2;          // indicates when there's a TCP connection
const int successLED = 3;            // indicates if the meter was set
const int resetLED = 4;              // indicates reset of Arduino
const int disconnectedLED = 5;       // indicates connection to server
/////////////////addition to show connection


const int numReadings = 10;
int readings[numReadings];      // the readings from the analog input

int index = 0;                  // the index of the current reading
//THE BELOW MAPS FROM MOST BAD TO LEAST ?
const int AQIMax = 100;              // maximum level for air quality
const int requestInterval = 10000;  // delay between updates to the server

//char ssid[] = "itpsandbox";      //  your network SSID (name) 
//char pass[] = "NYU+s0a!+P?";    // your network password

char ssid[] = "NewINC 2.4GHz";      //  your network SSID (name) 
char pass[] = "newinc231";    // your network password

boolean requested;                   // whether you've made a request since connecting
long lastAttemptTime = 0;            // last time you connected to the server, in milliseconds
int airQuality = 0;                  // AQI value
int todaysHigh = 0;                  // AQI Forecasted HIGH
boolean meterIsSet = false;          // whether the meter is set from the latest reading

int status = WL_IDLE_STATUS; // status of the wifi connection
WiFiClient client;
char server[] = "airylight.aws.af.cm";     // name address for twitter API

int rotateUp = -1200; //changed this wednes
int rotateDown = 1200;

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
  WiFi.begin(ssid, pass);
  // // start the serial library:
  Serial.begin(9600);

  /////////////////addition to show connection
  // set all status LED pins:
  pinMode(connectedLED, OUTPUT);
  pinMode(successLED, OUTPUT);
  pinMode(resetLED, OUTPUT);
  pinMode(disconnectedLED, OUTPUT);
  pinMode(meterPin, OUTPUT);
  /////////////////addition to show connection

  while ( status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(1000);
  } 

  /////////////////addition to show connection
  // blink the reset LED:
  blink(resetLED, 3);
  /////////////////addition to show connection

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
      //turn LED Green?
    } 
    else {
      // make an instance of TextFinder to search the response:
      TextFinder response(client);
      // see if the response from the server contains the AQI value:
      if(response.find("Air Quality:"))  {
        meterIsSet = true;
        // convert the remaining part into an integer:
        airQuality = response.getValue();
        // set the meter:
        //        todayMax = 
        while ((millis()<5000) && airQuality!=-1 && airQuality!=0){
          if (airQuality>sensorMax){
            sensorMax=airQuality; 
            Serial.println(sensorMax);
          }
          if (airQuality<sensorMin){
            sensorMin = airQuality;
            Serial.println(sensorMin);
          }
        }
        Serial.println(airQuality);
        readings[index] = airQuality; //storing all readings so far

        //       Serial.println("readings");
        for (int i = 1; i<numReadings; i++){
          //       Serial.println(readings[i]);
        }
        lookAt = index - 1;
        if(index == 0){
          lookAt = numReadings - 1;
        }
        if (readings[index] != readings[lookAt] && readings[index]!=-1){ //if this reading is not equal to the reading before
          airDiff = readings[index]-readings[lookAt]; //this is the difference between the reading now and the one before
          setMeter(airDiff, airQuality); //send it to the motor along with the current airquality
        } 
        else {
          //should there be another if statement in here checking about whether go big is true? is this even ever triggered? yes i think it was. 
          beNewMeter(airQuality, sensorMin, sensorMax); 
          //this happens if, every __x__ seconds, the new reading is not "interesting" (not different from the reading before it), 
          //do a big swirl from the minimum read to the maximum read so far today (?)
        }
        index++;
        if (goBig==false){ 
          //this happens if it is the first reading ever. 
          //go big is just whether to do differences or not... like, go small until you get a new read and then maybe you will have to go big
          setMeter(airDiff, airQuality); 
        }
        // if we're at the end of the array...
        if (index >= numReadings){              
          // ...wrap around to the beginning: 
          index = 0;
          //AFTER THE FIRST RESET, NO LONGER USE THE INDEX AS 0 AS THE METHOD TO SAY "BE THE FIRST READING AND ROTATE ALL THE WAY TO THE CURRENT POSITION)
          //this has to do with the fact that we are doing a "look at the one before" comparison once we get a second reading
          indexis = true;    
        }
      }
    }
  }
  else if (millis() - lastAttemptTime > requestInterval) {
    // if you're not connected, and two minutes have passed since
    // your last connection, then attempt to connect again:
    client.stop();
    connectToServer();
  }
  /////////////////addition to show connection
  // set the status LEDs:
  //will this timing be right? I think so.
  setLeds();
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
  client.println("GET /aqscraper.php HTTP/1.1");
  // fill in your server's name:
  client.println("HOST: appfog");
  client.println();
  return true;
}

void setMeter(int airDiff, int airQuality){

  if (goBig==false){ //if you ARE the first ever, and indexis has not been set to true go far
    meterSetting = map(airQuality, 0, 100, 0, rotateUp);
    thisWasSet = airQuality;
    Serial.println("thisWasSet first ever");
    Serial.println(thisWasSet);
    goBig=true;
  }   

  if (index>=1 && readings[index]!=thisWasSet && airQuality!=-1){ //this is a double check to make sure that we have a new reading
    Serial.println("inside airDiff");
    // map the result to a range the meter can use:
    if(airDiff>0){  
      meterSetting = map(airDiff, 0, 100, 0, rotateUp);
    }
    else if (airDiff<0){
      meterSetting = map(airDiff, -100, 0, rotateDown, 0); 
    }
  } 
  else if (index>=1 && readings[lookAt]==thisWasSet){ //if the reading is not new, don't do anything
    meterSetting=0; 
  }

  ////////////////////////
  //is this a duplicate use case?
  //whatever. it's basically saying, if current reading is the same as the first reading ever, go do that difference
  //i think it's a fail safe.
  if (index>2 && readings[index]==thisWasSet&&airQuality!=-1){ 
    //if we are bigger than 1 and the reading is the same as the first ever reading
    Serial.println("inside same as first ever setting");
    if(airDiff>0){  
      meterSetting = map(airDiff, 0, 100, 0, rotateUp);
    }
    else if (airDiff<0){
      meterSetting = map(airDiff, -100, 0, rotateDown, 0); 
    }
  }
  ////////////////////////

  Serial.println("setting rotation with:"); 
  Serial.println(meterSetting);
  rotateDeg(meterSetting, .1); 
}

void beNewMeter(int airQuality, int sensorMin, int sensorMax){
  Serial.println("beNewMeter");
  if (index%5==4){ //every once in a while?

    Serial.println("insideFirst");
    int newSetting = map(airQuality, 0, 100, 0, rotateUp);
    Serial.println(newSetting); //this is the FULL air quality rotation (not a difference between prior value)
    int goToMin = map(0, 0, 100, 0, rotateUp);
    int goToMax = map(100, 0, 100, 0, rotateUp);
    Serial.println("thisismin");
    Serial.println(goToMin);
    Serial.println("thisismax");
    Serial.println(goToMax);
    rotateDeg(-newSetting, .1); //get to zero first no matter what
    delay(500); //wait
    rotateDeg(goToMax, .1); //go to the maximum
    delay(100); //wait a tiny bit
    rotateDeg(-goToMax, .1); //go to the minimum
    delay(100); //wait a tiny bit
    goBig=false; //AH WILL THIS WORK??? -->> what would make it not work...? 
    //if go big is already false from above - 
  }
}

void rotateDeg(float deg, float speed){ 
  //rotate a specific number of degrees (negative for reverse movement)
  //speed is any number from .01 -> 1 with 1 being fastest - Slower is stronger, smoother
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

//now let me try to write some psuedo code for turning leds on and off to show network connection
void setLeds() {
  // connected LED and disconnected LED can just use
  // the client's connected() status:
  digitalWrite(connectedLED, client.connected());
  digitalWrite(disconnectedLED, !client.connected());
  // success LED depends on reading being successful:
  digitalWrite(successLED, meterIsSet);
}

void blink(int thisPin, int howManyTimes) {
  //     Blink the reset LED:
  for (int blinks=0; blinks< howManyTimes; blinks++) {
    digitalWrite(thisPin, HIGH);
    delay(200);
    digitalWrite(thisPin, LOW);
    delay(200);  
  }
}
