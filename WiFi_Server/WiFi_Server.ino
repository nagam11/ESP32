#include <WiFi.h>

#define PIN 5

const char* ssid     = "WIFI-NAME";
const char* password = "PASSWORD";

WiFiServer server(80);

void setup()
{
  Serial.begin(115200);
  pinMode(PIN, OUTPUT);

  delay(10);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

int value = 0;

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println(); 
            client.print("<h1> Hello from the ESP32! </h1>");
            client.print("Click <a href=\"/H\">here</a> to turn the LED on pin 5 on.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn the LED on pin 5 off.<br>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
        handleRequests(currentLine);
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

void handleRequests(String currentLine) {
  //  Check which request was send
  if (currentLine.endsWith("GET /H")) {
    Serial.print("H Request was sent. PIN on High.");
    digitalWrite(PIN, HIGH);
  }
  else if (currentLine.endsWith("GET /L")) {
    Serial.print("L Request was sent. PIN on Low.");
    digitalWrite(PIN, LOW);
  } else {
    Serial.print("Invalid request..");    
    return;
  }
}

