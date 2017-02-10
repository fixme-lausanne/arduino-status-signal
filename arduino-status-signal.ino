#include <ESP8266WebServer.h>
#include <WiFiUdp.h>

ESP8266WebServer server(80);

int RED = D5;
int YELLOW = D6;
int GREEN = D7;

void setup() {
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);
  
  Serial.begin(115200);
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

void loop() {
  server.handleClient();
}

