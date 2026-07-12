#include <ArduinoJson.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);

const char* ssid = "whynot_2.4GHz";
const char* password = "this7465";
const char* serverURL = "https://httpbin.org/post";

void setup() 
{
  Serial.begin(115200);
  // WiFi setup
  delay(1000);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi.....");
  }
  Serial.println("WiFi Successfully Connected!!");
  //OLED Display setup
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED init failed..");
    while(1);
  }
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void loop() 
{
  display.clearDisplay();
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    http.begin(serverURL);
    String jsonPayload = "{\"username\": \"musangqim\"}";
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(jsonPayload);
    if (httpCode == 200)
    {
      String response = http.getString();
      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, response);
      if (error)
      {
        display.setCursor(10, 32);
        display.println("JSON Parse Error");
        display.display();
        delay(2000);
        return;
      }
      String username = doc["json"]["username"];
      Serial.println("Success! Response: " + response);
      display.setCursor(10, 10);
      display.println("User: " + username);
      display.display();
    }
    else if (httpCode > 0)
    {
      Serial.println("Server responded but with error code: " + String(httpCode));
    }
    else
    {
      Serial.println("Connection Failed, Error: " + String(httpCode));
    }
    http.end();
  }
  delay(10000);
}
