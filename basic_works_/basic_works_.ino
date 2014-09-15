#include <SPI.h>
#include <WiFi.h>
#include <TextFinder.h>

char ssid[] = "NewINC 2.4GHz";      //  your network SSID (name) 
char pass[] = "newinc231";    // your network password
int airQuality = 0;
int status = WL_IDLE_STATUS;

const int connectedLED = 2;          // indicates when there's a TCP connection
const int successLED = 3;            // indicates if the meter was set
const int disconnectedLED = 5;       // indicates connection to server
boolean requested;                   // whether you've made a request since connecting
long lastAttemptTime = 0;  
const int requestInterval = 10000; //should be 10,000  // delay between updates to the server
boolean meterIsSet = false;          // whether the meter is set from the latest reading

char server[] = "appfog.com";     // name address for twitter API

// Initialize the client library
WiFiClient client;

void setup() {
  Serial.begin(9600);
  // set all status LED pins:
  pinMode(connectedLED, OUTPUT);
  pinMode(successLED, OUTPUT);
  pinMode(disconnectedLED, OUTPUT);

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
        if(airQuality!=0 && airQuality!=-1){
          meterIsSet = true;
          Serial.println(meterIsSet);
  digitalWrite(successLED, HIGH);
        } else {
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
  /////////////////addition to show connection
  // set the status LEDs:
  //will this timing be right? I think so.
  setLeds();
}

void connectToServer() {
  // clear the state of the meter:
  meterIsSet = false;
  // attempt to connect, and wait a millisecond:
  Serial.println("connecting...");
  if (client.connect(server, 80)) {
    requested = false; 
  }
  // note the time of this connect attempt:
  lastAttemptTime = millis();
}
boolean makeRequest() {
  Serial.println("requesting...");
  // Make a HTTP request:
  client.println("GET /aqscraper.php HTTP/1.1");
  client.println("HOST: airylight.aws.af.cm");  
  client.println();
  return true;
}
//now let me try to write some psuedo code for turning leds on and off to show network connection
void setLeds() {
  Serial.println("setting leds");
  // connected LED and disconnected LED can just use
  // the client's connected() status:
  digitalWrite(connectedLED, client.connected());
  digitalWrite(disconnectedLED, !client.connected());
  // success LED depends on reading being successful:
  digitalWrite(successLED, meterIsSet);
}
