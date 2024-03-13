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

void setup() {
  Serial.begin(115200);
  setupWiFi();
  webSocket.begin(webSocketServerAddress, webSocketServerPort, "/");
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
  
  // Send real-time data to the WebSocket server
  String realTimeData = "SensorValue: " + String(random(0, 1024)); // Replace with your actual sensor data
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
