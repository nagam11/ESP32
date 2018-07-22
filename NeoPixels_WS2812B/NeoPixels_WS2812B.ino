#include <WiFi.h>
#include <NeoPixelBus.h>

// TODO: Change your Wifi credentials
const char* ssid     = "WIFI-NAME";
const char* password = "PASSWORD";
WiFiServer server(80);

// PIN for NEOPIXEL strip
#define PIN 22
#define colorSaturation 255
// TODO: Change 60 to the number of pixels you are using.
#define COUNT 60
NeoPixelBus<NeoGrbFeature, NeoEsp32BitBangWs2813Method> strip(COUNT, PIN);
RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor black(0);

void setup() {
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

  strip.Begin();
  // There is an interrupts conflict with the NeoPixels on the ESP32 when the Wifi is on. Use delay.
  delay(1);
  strip.Show();
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
  if (currentLine.endsWith("GET /WIPE")) {
    Serial.print("Wipe request was sent.");
    wipeNeoPixels();
  }
  else if (currentLine.endsWith("GET /COLOR")) {
    Serial.print("Set color qequest was sent.");
    colorNeoPixels();
  } else {
    Serial.print("Invalid request..");
    return;
  }
}

void wipeNeoPixels() {
  colorWipe(red, 50);
  colorWipe(green, 50);
  colorWipe(black, 50);
  turnOff();
}

void colorNeoPixels() {
  colorPixels(blue, 0, COUNT / 4);
  delay(1000);
  colorPixels(red, COUNT / 4 , COUNT / 2);
  delay(1000);
  colorPixels(green, COUNT / 2, COUNT);
  turnOff();
}
// Fill the dots one after the other with a color
void colorWipe(RgbColor c, uint8_t wait) {
  for (uint16_t i = 0; i < COUNT; i++) {
    strip.SetPixelColor(i, c);
    delay(1);
    strip.Show();
    delay(wait);
  }
}
// Turn off all Pixels in the strip
void turnOff() {
  for (uint32_t i = 0; i < COUNT; i++) {
    strip.SetPixelColor(i, black);
  }
  delay(1);
  strip.Show();
}
// Fill specific dots at the same time with a color
void colorPixels(RgbColor c, uint32_t from, uint32_t to) {
  for (uint32_t i = from; i < to; i++) {
    strip.SetPixelColor(i, c);
  }
  delay(1);
  strip.Show();
}


