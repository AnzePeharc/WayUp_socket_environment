#include <SPI.h>
#include <WiFi.h>
#include <FastLED.h>

// FASTLED Settings
#define LED_PIN     6
#define NUM_LEDS    50
CRGB leds[NUM_LEDS];

// change ssid and pass according to your WiFi network
char ssid[] = "PeharcUniverse";      // your network SSID (name)
char pass[] = "druzinapeharc";   // your network password
IPAddress ip(192, 168, 0, 150);  // static ip address
int keyIndex = 0;                 // your network key Index number (needed only for WEP)
String hold_ids; 
String hold_tags;
int status = WL_IDLE_STATUS;
boolean first_problem_displayed = false;

WiFiServer server(8080);

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  FastLED.addLeds<WS2811, LED_PIN, RGB>(leds, NUM_LEDS);
  while (!Serial) {
 // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  while (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    delay(100);
  }

  String fv = WiFi.firmwareVersion();
  Serial.println("Wifi firmwareVersion: " + fv);
  if (fv != "1.1.0") {
    Serial.println("Please upgrade the firmware");
  }

  WiFi.config(ip); // set static ip address for the Arduino Uno WiFi shield
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();
  // you're connected now, so print out the status:
  printWifiStatus();
}


void loop() {
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {
    Serial.println("new client");
    while (client.connected()) {
      if (client.available()) {
        String c = client.readString();
        Serial.println(c);
        // String that tells Arduino to turn off the lights 
        if (c == "reset_lights") {
          first_problem_displayed = false; // reset the variable for checking if the problem is shown on the wall
          // turn off LEDs with the next two commands
          FastLED.clear();
          FastLED.show();
        }
        else{
          // check if there is already a problem displayed and if the used wants to show the second one
          if(first_problem_displayed && getValue(c, ';', 2) == "second"){
          // call the function for turning on the correct light
          String build_status = buildArray(getValue(c, ';', 0), getValue(c, ';', 1), getValue(c, ';', 2));
          Serial.println(build_status);
            
          }

          // show the next problem by erasing the previous one and showing the next one
          else{
            
          FastLED.clear(); // turn all lights off
          // call the function for turning on the correct lights
          String build_status = buildArray(getValue(c, ';', 0), getValue(c, ';', 1), getValue(c, ';', 2));
          Serial.println(build_status);
          first_problem_displayed = true;
            
          }
          
        }


        /*
        if (c == "show_all") {
          fill_solid( leds, NUM_LEDS, CRGB(255,255,255));
          FastLED.show();
        }
        else if (c == "hide_all") { 
          FastLED.clear();
          FastLED.show();
        }
        else { 
            leds[c.toInt()] = CRGB(255, 0, 0);
            FastLED.show();
        }

        delay(1000);
        */
      }
      delay(10);
    }

    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
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

// function for retrieving seperate values from the received string message
String getValue(String data, char separator, int index)
{

    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

String buildArray(String ids, String colors, String sequence_identifier){
  // sequence_identifier tells us which sequence has been sent (first / second)
  Serial.println("The sent sequence is: " + sequence_identifier);
  Serial.println("Ids: " + ids);
  Serial.println("Colors: " + colors);
  // establish the correct primary colors for LEDs
  CRGB start = CRGB(0, 255, 0);
  CRGB intermediate = CRGB(0, 0, 255);
  CRGB top = CRGB(255,0, 0);
  // if there already is a problem on the wall and you want to display the second one, establish secondary colors
  if(sequence_identifier == "second"){
    start = CRGB(255, 255, 0);
    intermediate = CRGB(0, 255, 255);
    top = CRGB(255,0, 255);
  }

  // initalize id array
  char temp_ids[ids.length() + 1];
  ids.toCharArray(temp_ids, ids.length()+1);
  char *led_ids[20]; // assign default array size to 20, because there are usually no more than 20 holds in one sequence
  // initalize color array
  char temp_colors[colors.length() + 1];
  colors.toCharArray(temp_colors, colors.length()+1);
  char *led_colors[50]; // assign default array size to 100 for colors

  // variables for LED IDs
  char *ptr1 = NULL;
  byte index1 = 0; // counter1
  ptr1 = strtok(temp_ids, ",");
 
  // fill the array with LED IDs
  while (ptr1 != NULL){
    led_ids[index1] = ptr1;
    index1 ++;
    ptr1 = strtok(NULL, ",");
  }

  // variables for LED Colors
  char *ptr2 = NULL;
  byte index2 = 0; // counter2
  ptr2 = strtok(temp_colors, ",");
  
  // fill the array with LED Colors
  while (ptr2 != NULL){
    led_colors[index2] = ptr2;
    index2 ++;
    ptr2 = strtok(NULL, ",");
  }

  
  // FOR loop, which traverses over led_ids and lights the correct LED with the correct color
    for(int n = 0; n < index1; n++)
   {

    if (atoi(led_ids[n]) < 50){ // TODO: implement a system for all the lights, not just 50!!!!
      if (strcmp(led_colors[n], "green") == 0){ // comparison of char to string
        /* check if LED is already lit up, 
         *  if you are displaying two problems at once
         */
        if(leds[atoi(led_ids[n])] && sequence_identifier == "second"){
          leds[atoi(led_ids[n])] = CRGB(255, 255, 255); // lighting the correct LED
          FastLED.show();
        }
        else{
          leds[atoi(led_ids[n])] = start; // lighting the correct LED
          FastLED.show();
        }
      }
      else if (strcmp(led_colors[n], "blue") == 0){ // comparison of char to string
        /* check if LED is already lit up, 
         *  if you are displaying two problems at once
         */
        if(leds[atoi(led_ids[n])] && sequence_identifier == "second"){
          leds[atoi(led_ids[n])] = CRGB(255, 255, 255); // lighting the correct LED
          FastLED.show();
        }
        else{
          leds[atoi(led_ids[n])] = intermediate; // lighting the correct LED
          FastLED.show();
        }

      }
      else if (strcmp(led_colors[n], "red") == 0){ // comparison of char to string
        /* check if LED is already lit up, 
         *  if you are displaying two problems at once
         */
        if(leds[atoi(led_ids[n])] && sequence_identifier == "second"){
          leds[atoi(led_ids[n])] = CRGB(255, 255, 255); // lighting the correct LED
          FastLED.show();
        }
        else{
          leds[atoi(led_ids[n])] = top; // lighting the correct LED
          FastLED.show();          
        }

      }

      }

   }


  return "Done";
}
