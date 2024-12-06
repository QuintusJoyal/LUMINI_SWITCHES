#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include "Secrets.hpp"

WebSocketsClient webSocket;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("WebSocket connected");
      break;
    case WStype_TEXT:
      Serial.printf("Received data: %s", length, payload);
      Serial.println();
      // Add your logic to handle incoming data
      break;
  }
}

const int sensorPin = A0;     // Analog input pin for ACS712 sensor
const float sensitivity = 0.185; // Sensitivity of ACS712 module for 20A version (mV per Amp)
const float voltage = 230.0;  // Voltage of the AC power supply (V)
const float timeInterval = 1; // Time interval for measurements (seconds)

float currentTotal = 0.0;     // Total consumed current (A)
unsigned long previousMillis = 0; // Previous time for interval calculation

void calculatekWh() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= timeInterval * 1000) {
    previousMillis = currentMillis;
    
    // Read current sensor value
    int sensorValue = analogRead(sensorPin);
    
    // Convert sensor value to current (in Amperes)
    float current = (sensorValue - 512) * sensitivity / 1000.0;
    
    // Calculate charge (in Coulombs)
    float charge = current * timeInterval;
    
    // Convert charge to kilowatt-hour (kWh)
    float kWh = (charge * voltage) / (3600000.0 * 1000.0); // Conversion from Coulombs to kWh
    
    // Accumulate total charge
    currentTotal += kWh;
    
    // Print result
    Serial.print("Current (A): ");
    Serial.print(current);
    Serial.print("\tTotal kWh: ");
    Serial.println(currentTotal, 4);
  }
}

void setup() {
  Serial.begin(115200);
  setupWiFi();
  webSocket.begin(webSocketServerAddress, webSocketServerPort, "/");
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
  
  // Send real-time data to the WebSocket server
  calculatekWh();
  String realTimeData = "kWh: " + String(currentTotal);
  webSocket.sendTXT(realTimeData);
  Serial.println(realTimeData);
//  delay(1000); // Adjust as needed
}

void setupWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
