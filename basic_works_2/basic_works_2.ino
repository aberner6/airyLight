#include <SPI.h>
#include <WiFi.h>
#include <TextFinder.h>

char ssid[] = "NewINC 2.4GHz";      //  your network SSID (name) 
char pass[] = "newinc231";    // your network password
int airQuality = 0;
int status = WL_IDLE_STATUS;
const int buttonPin = 7;     // the number of the pushbutton pin
int buttonState = 0;         // variable for reading the pushbutton status

const int connectedLED = 2;          // indicates when there's a TCP connection
const int successLED = 3;            // indicates if the meter was set
const int disconnectedLED = 5;       // indicates connection to server
boolean requested;                   // whether you've made a request since connecting
long lastAttemptTime = 0;  
const int requestInterval = 100000000; //should be 10,000  // delay between updates to the server
boolean meterIsSet = false;          // whether the meter is set from the latest reading

char server[] = "appfog.com";     // name address for twitter API

// Initialize the client library
WiFiClient client;


const int numReadings = 10;
int readings[numReadings];      // the readings from the analog input
int index = 0;                  // the index of the current reading
//THE BELOW MAPS FROM MOST BAD TO LEAST ?
const int AQIMax = 100;              // maximum level for air quality
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
//USELESS?
int sensorMin = 100;
int newIndex = 0;
int sensorMax = 0;
boolean beenHere = false;

void setup() {
  Serial.begin(9600);
  // set all status LED pins:
  pinMode(connectedLED, OUTPUT);
  pinMode(successLED, OUTPUT);
  pinMode(disconnectedLED, OUTPUT);
  pinMode(buttonPin, INPUT);     

  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);
  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) { 
    Serial.println("Couldn't get a wifi connection");
    // don't do anything else:
    while(true);
  } 
  else {
    Serial.println("Connected to wifi");
    Serial.println("\nStarting connection...");
    // if you get a connection, report back via serial:
    connectToServer();
    //    if (client.connect(server, 80)) {
    //      Serial.println("connected");
    //      // Make a HTTP request:
    //      client.println("GET /aqscraper.php HTTP/1.1");
    //      client.println("HOST: airylight.aws.af.cm");
    //      client.println();
    //    }
  }
}

void loop() {
    // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
      Serial.println(buttonState);

  if (client.connected()) {
    if (!requested) {
      requested = makeRequest(); 
      //turn LED Green?
    } 
    else {
      TextFinder response(client);
      // see if the response from the server contains the AQI value:
      if(response.find("Air Quality:"))  {
        // convert the remaining part into an integer:
        airQuality = response.getValue();
        Serial.println(airQuality);


  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == HIGH) { 
    Serial.println("read high");
    Serial.println(airQuality);
//   beNewMeter(airQuality, sensorMin, sensorMax);
    //    Serial.println("HI");  
    // go big function triggered  
  }

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




        if(airQuality!=0 && airQuality!=-1){
          meterIsSet = true;
          Serial.println(meterIsSet);
          digitalWrite(successLED, HIGH);
        } 
        else {
          digitalWrite(successLED, LOW);
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

 
//  else {
//    Serial.println("LO");  
    //SET boolean for "go big" to off
//  }  
  /////////////////addition to show connection
  // set the status LEDs:
  //will this timing be right? I think so.
}

void connectToServer() {
  // clear the state of the meter:
  meterIsSet = false;
  // attempt to connect, and wait a millisecond:
  Serial.println("connecting...");

  if (client.connect(server, 80)) {
    digitalWrite(connectedLED, client.connected());
    requested = false; 
  }

  //        digitalWrite(connectedLED, client.connected());
  // note the time of this connect attempt:
  lastAttemptTime = millis();
}
boolean makeRequest() {
  Serial.println("requesting...");

  digitalWrite(successLED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);               // wait for a second
  digitalWrite(successLED, LOW);    // turn the LED off by making the voltage LOW
  delay(100);
  digitalWrite(successLED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);               // wait for a second
  digitalWrite(successLED, LOW);    // turn the LED off by making the voltage LOW
  //  delay(100);  
  // Make a HTTP request:
  client.println("GET /aqscraper.php HTTP/1.1");
  client.println("HOST: airylight.aws.af.cm");  
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
  //  rotateDeg(meterSetting, .1); 
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
    goBig=false; //AH WILL THIS WORK??? -->> what would make it not work...? 

  }
}

