#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

ESP8266WebServer server(80);

WiFiClient client;
char* STATUS_URL = "http://fixme.ch/status.json";

int RED = D5;
int YELLOW = D6;
int GREEN = D7;

bool isOpen = false;

void setup() {
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);
  
  Serial.begin(9600);
  Serial.println();

  Serial.print("Setting soft-AP ... ");
  WiFi.begin("FIXME-NAT", "hs1337_FIXME");

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected!");
  Serial.println(WiFi.localIP());

  launchServer();
}

void launchServer() {
  Serial.println("Lauching web server...");
  
  // Routes
  server.on("/", sendOK);
  server.on("/open", setOpen);
  server.on("/unknown", setUnknown);
  server.on("/close", setClose);

  server.begin();
  
  Serial.println("Web server ready!");
}

void getRequest(char* url) {
  HTTPClient http;
  http.begin(url);
  
  int httpCode = http.GET();
  
  // httpCode will be negative on error
  if(httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("Response code: %d\n", httpCode);

      // file found at server
      if(httpCode == HTTP_CODE_OK) {
          String payloadStr = http.getString();
          // DEBUG: Serial.println(payloadStr);

          int len = payloadStr.length() + 1;
          char payload[len];
          payloadStr.toCharArray(payload, len);

          // JSON Decode
          DynamicJsonBuffer jsonBuffer(2000);
          // StaticJsonBuffer<2000> jsonBuffer;
          JsonObject& status = jsonBuffer.parseObject(payload);
          
          if (!status.success()) {
            Serial.println("parseObject() failed");
            return;
          }
          
          isOpen = status["state"]["open"];
          Serial.print("isOpen: ");
          Serial.println((isOpen) ? "true" : "false");
      }
  } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

void setOpen() {
  Serial.println("Set status: open");
  
  digitalWrite(RED, HIGH);
  digitalWrite(YELLOW, HIGH);
  digitalWrite(GREEN, LOW);

  sendOK();
}

void setUnknown() {
  Serial.println("Set status: unknown");
  
  digitalWrite(RED, HIGH);
  digitalWrite(YELLOW, LOW);
  digitalWrite(GREEN, HIGH);

  sendOK();
}

void setClose() {
  Serial.println("Set status: close");
  
  digitalWrite(RED, LOW);
  digitalWrite(YELLOW, HIGH);
  digitalWrite(GREEN, HIGH);

  sendOK();
}

void sendOK() {
  server.send(200, "text/plain", "OK");
}

void updateStatus() {
  getRequest(STATUS_URL);
  if (isOpen) {
    setOpen();
  } else {
    setClose();
  }
}

void loop() {
  delay(5000);
  Serial.println("--------");
  
  server.handleClient();
  updateStatus();
}

