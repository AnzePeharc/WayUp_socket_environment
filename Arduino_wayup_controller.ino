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
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiServer server(8080);

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  FastLED.addLeds<WS2811, LED_PIN, RGB>(leds, NUM_LEDS);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
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
