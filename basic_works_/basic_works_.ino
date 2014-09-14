#include <SPI.h>
#include <WiFi.h>
#include <TextFinder.h>

char ssid[] = "NewINC 2.4GHz";      //  your network SSID (name) 
char pass[] = "newinc231";    // your network password
int airQuality = 0;
int status = WL_IDLE_STATUS;
//IPAddress server(74,125,115,105);  // Google
//char server[] = "www.arduino.cc";
//IPAddress server(54,84,140,179);
char server[] = "appfog.com";     // name address for twitter API

// Initialize the client library
WiFiClient client;

void setup() {
  Serial.begin(9600);
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
    if (client.connect(server, 80)) {
      Serial.println("connected");
      // Make a HTTP request:
  client.println("GET /aqscraper.php HTTP/1.1");
//    client.println("GET /latest.txt HTTP/1.1");
  client.println("HOST: airylight.aws.af.cm");
//    client.println("Host: www.arduino.cc");
      client.println();
    }
  }
}

void loop() {
  while (client.available()) {
//    char c = client.read();
//    Serial.write(c);
          TextFinder response(client);
      // see if the response from the server contains the AQI value:
      if(response.find("Air Quality:"))  {
        // convert the remaining part into an integer:
        airQuality = response.getValue();
        Serial.println(airQuality);
//        Serial.println("air");
      }
  }  

}
