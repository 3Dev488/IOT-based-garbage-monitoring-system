/*********************************************************************
  This is the code for Fetching your location from Google Geolocation API
  
*********************************************************************/
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "ESP8266WiFi.h"
//////////////////thigns board////////////////////////
#include <PubSubClient.h>
#define WIFI_AP "smarthelmet"
#define WIFI_PASSWORD "12345678"
#define TOKEN "cTtZeWp3Iz054sVmyELS"
char thingsboardServer[] = "demo.thingsboard.io"; 
WiFiClient wifiClient;
PubSubClient client(wifiClient);
int status = WL_IDLE_STATUS;
unsigned long lastSend;
static double latitude_1;
static double longitude_1;
////////////////////////////////////////////////////////////////////////////

char ssid[] = "Dustbin";         // your network SSID name
char pass[] = "12345678";          // your network password


//Credentials for Google GeoLocation API...
const char* Host = "www.googleapis.com";
String thisPage = "/geolocation/v1/geolocate?key=";
//String key = "AIzaSyCwDrYRTuFul-K00_4e6lTqBei7fMNpZGQ ";
String key = "AIzaSyChlwNUlJrT771rGyawGi8oVWtag2U3mGk";

//int status = WL_IDLE_STATUS;
String jsonString = "{\n";
double latitude    = 0.0;
double longitude   = 0.0;
double accuracy    = 0.0;
int more_text = 1;    // set to 1 for more debug output
////////////dustbin////////////////////////////////////
// defines pins numbers
const int trigPin = 2;  //D4
const int echoPin = 0;  //D3
const int dustbin_full = 16;  //D0

// defines variables
long duration;
int distance;
static String dustbin;
/////////////////////////////////////////////////////////////////
/////////////////////////////////thingsboard///////////////////////////
void getAndSendTemperatureAndHumidityData()
{
  Serial.println("Collecting temperature data.");

  // Just debug messages
  Serial.print( "Sending latitude and longitude : [" );
  Serial.print( latitude_1 ); Serial.print( "," );
  Serial.print( longitude_1 );Serial.print( "," );
  Serial.print( dustbin);
  Serial.print( "]   -> " );

  // Prepare a JSON payload string
  String payload = "{";
  payload += "\"latitude\":"; payload += latitude_1; payload += ",";
  payload += "\"DISTANCE\":"; payload += dustbin; payload += ",";
  payload += "\"longitude\":"; payload +=longitude_1 ;payload += "}";

  // Send payload
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  client.publish( "v1/devices/me/telemetry", attributes );
  Serial.println( attributes );

}

void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("ESP8266 Device", TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////


void setup()   {
  Serial.begin(9600);
  Serial.println("Start");
  ////////////dustbin////////////////
 pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT); // Sets the echoPin as an Input
pinMode(dustbin_full, OUTPUT); 
//////////////////////////////////

 // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("Setup done");
  
// We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("connected to AP");
  //////////thingsboard////////////////////////////////
  client.setServer( thingsboardServer, 1883 );
  lastSend = 0;
  /////////////////////////////////////////////////////
}
void loop() {
  char bssid[6];
  DynamicJsonBuffer jsonBuffer;
  Serial.println("scan start");
  
// WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found...");
    if (more_text) {
      Serial.println("\"wifiAccessPoints\": [");
      for (int i = 0; i < n; ++i)
      {
        Serial.println("{");
        Serial.print("\"macAddress\" : \"");
        Serial.print(WiFi.BSSIDstr(i));
        Serial.println("\",");
        Serial.print("\"signalStrength\": ");
        Serial.println(WiFi.RSSI(i));
        if (i < n - 1)
        {
          Serial.println("},");
        }
        else
        {
          Serial.println("}");
        }
      }
      Serial.println("]");
      Serial.println("}");
    }
    Serial.println(" ");
  }
// now build the jsonString...
  jsonString = "{\n";
  jsonString += "\"homeMobileCountryCode\": 234,\n"; // this is a real UK MCC
  jsonString += "\"homeMobileNetworkCode\": 27,\n";  // and a real UK MNC
  jsonString += "\"radioType\": \"gsm\",\n";         // for gsm
  jsonString += "\"carrier\": \"Vodafone\",\n";      // associated with Vodafone
  jsonString += "\"wifiAccessPoints\": [\n";
  for (int j = 0; j < n; ++j)
  {
    jsonString += "{\n";
    jsonString += "\"macAddress\" : \"";
    jsonString += (WiFi.BSSIDstr(j));
    jsonString += "\",\n";
    jsonString += "\"signalStrength\": ";
    jsonString += WiFi.RSSI(j);
    jsonString += "\n";
    if (j < n - 1)
    {
      jsonString += "},\n";
    }
    else
    {
      jsonString += "}\n";
    }
  }
  jsonString += ("]\n");
  jsonString += ("}\n");
  //-------------------------------------------------------------------- Serial.println("");
  //Connect to the client and make the api call
  WiFiClientSecure client;
  Serial.print("Requesting URL: ");
  Serial.println("https://" + (String)Host + thisPage + key);
  Serial.println(" ");
  if (client.connect(Host, 443)) {
    Serial.println("Connected");
    client.println("POST " + thisPage + key + " HTTP/1.1");
    client.println("Host: " + (String)Host);
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.println("User-Agent: Arduino/1.0");
    client.print("Content-Length: ");
    client.println(jsonString.length());
    client.println();
    client.print(jsonString);
    delay(500);
  }
  //Read and parse all the lines of the reply from server
  while (client.available()) {
    String line = client.readStringUntil('\r');
    if (more_text) {
      Serial.print(line);
    }
    JsonObject& root = jsonBuffer.parseObject(line);
    if (root.success()) {
      latitude    = root["location"]["lat"];
      longitude   = root["location"]["lng"];
      accuracy   = root["accuracy"];
    }
  }
  Serial.println("closing connection");
  Serial.println();
  client.stop();
  Serial.print("Latitude = ");
  Serial.println(latitude, 6);
  Serial.print("Longitude = ");
  Serial.println(longitude, 6);
  Serial.print("Accuracy = ");
  Serial.println(accuracy);
  /////////////////////////////dustbin//////////////////////////////////////
   //////////////////////////////////////dustbin/////////////////////////////
  // Clears the trigPin
digitalWrite(trigPin, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);

// Calculating the distance
distance= duration*0.034/2;
// Prints the distance on the Serial Monitor
//Serial.print("Distance: ");
//Serial.println(distance);
if(distance <=10)
{
  digitalWrite(dustbin_full,HIGH);
}
if(distance >=11)
{
  digitalWrite(dustbin_full,LOW);
}
delay(2000);

  dustbin = String(distance);
/////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////thingsboard/////////////////////
  latitude_1=latitude ;
  longitude_1=longitude;
  //latitude_1="12.3453";
  //longitude_1="45.3613";
  Serial.println('*');
  Serial.print(latitude, 6);
   Serial.print(',');
  Serial.print(longitude, 6);
  Serial.print(',');
  Serial.print(dustbin);
   Serial.println('#');
  
  if ( !client.connected() ) {
    reconnect();
  }

  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds
    getAndSendTemperatureAndHumidityData();
    lastSend = millis();
  }

  //client.loop();
}
