// CRT 399 ESP32 Krannert Door Study
// Caleb & Braedon
// Update: FALL 2021

// WiFi Settings (MUST BE MODIFIED)
#include <WiFi.h>
const char* ssid     = "EZConnect";                   // Name of Wireless Network
const char* password = "Tyt462yeZ3";                  // Password for Wireless Network
const char* host = "10.40.6.2";                     // Host Name (IP Address or Domain, e.g. 10.40.4.1 or google.com
String url = "/399/groups/discovery/dataCollect.php?";       // URL Name (Path to PHP File, e.g. /399/zane/index.php?

// Sets Thresholds
int highVal = 22;
int midVal = 11;
int lowVal = midVal;

// Touch Reading
int touchVal;
int prevTouch;
String side;

// Other Variables (DO NOT CHANGE)
boolean wifiConnected = false;                        // Is WiFi Connected?
int wifiAttemptsCount = 0;                            // Number of Connection Attempts
int wifiAttemptsTotal = 20;                           // Max Number of Connection Attempts
int led = 5;                                          // Onboard LED to Show Upload

//MILLIS
unsigned long startTime;

//Array To Store Values For Sketch WiFi
int storeArray[1000];
long timeArray[1000];
int count = 0;

void setup() {
  Serial.begin(115200); delay(100); Serial.println("Starting Up..");                // Initalize Serial Connection
  pinMode(led, OUTPUT);                                                            // Set LED as output  
}

void loop() {
  touchVal = touchRead(4);              //check the reading from the touch sensor

  if (touchVal < highVal) {             //check the threshold and store the value if it is under the threshold
    if (abs(prevTouch - touchVal) < 3) {
      long thisTime = millis();
      storeArray[count] = touchVal;
      timeArray[count] = thisTime;
      count++;
      takeSample();
    }
  }
  prevTouch = touchVal;
}

void takeSample() {     //this function sends the reading from the touch sensor to the server

  // Connect to WiFI
  initWiFi();
  Serial.println("########### TOUCH VALUE #########");
  Serial.println(touchVal);  // get value of Touch 0 pin
  Serial.println("########### TOUCH VALUE #########");

  if (wifiConnected) {
    for (int i = count; i > 0; i--) {

      // Create variables to store readingLabel, readingValue, timeLabel, and timeValue
      String readingLabel = "reading=";
      int readingVal = storeArray[i-1];
      String timeLabel = "&millis=";
      long timeVal = timeArray[i-1];

      // Send to Database
      String msg = readingLabel + readingVal + timeLabel + timeVal;
      connectServer(msg);
      Serial.println(msg);
    }
    count = 0;
  }
  WiFi.disconnect();
}

  // WIFI FUNCTIONS
  // Connects to the WiFi
  void initWiFi() {
    wifiAttemptsCount = 0;                            // Initialize the number of attempted connections
    wifiConnected = true;                             // Assume WiFi will connect
    Serial.println("Connecting to network...");

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(20);
      Serial.print(".");
      wifiAttemptsCount++;
      if (wifiAttemptsCount > wifiAttemptsTotal) {
        wifiConnected = false;
        break;
      }
    }

    if (wifiConnected) {
      Serial.println("WiFi Connected!");
    }
    else {
      for (int i = 0; i < 10; i++) {
        digitalWrite(led, LOW);  // Flash to signal no connection
        delay(10);
        digitalWrite(led, HIGH);
        delay(10);
      }
      Serial.println("WiFi Not Connected! Will retry later.");
    }
  }

  // Connects to the Server and Sends a Query
  void connectServer(String msg) {
    // Connect to Server
    String finalURL = url + msg;
    WiFiClient client; const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
      Serial.println(host);
      Serial.println("Connection Failed");
      return;
    }

    Serial.println(finalURL);

    // Send Request to Server
    client.print(String("GET ") + finalURL + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

    // Get Response from Server
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 1000) {
        Serial.println("Connection Timeout!");
        client.stop();
        return;
      }
    }
    while (client.available()) {
      String pageData = client.readStringUntil('\r');
    }
  }
