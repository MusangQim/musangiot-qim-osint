#include <ArduinoJson.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);

const char* ssid = "whynot_2.4GHz";
const char* password = "this7465";
const char* serverURL = "http://192.168.x.xx:5000/lookup"; //"https://httpbin.org/post";

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
  String job_id = "";
  bool searchDone = false;
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
      job_id = doc["job_id"].as<String>();
      if (error)
      {
        display.setCursor(10, 32);
        display.println("JSON Parse Error");
        display.display();
        delay(2000);
        return;
      }
      display.clearDisplay();
      Serial.println("Searching.....");
      display.setCursor(10, 20);
      display.println(("Searching....."));
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
    while (searchDone == false)
    {
      delay(5000);
      String resultURL = "http://192.168.x.xx:5000/result/" + job_id;
      HTTPClient http2;
      http2.begin(resultURL);
      int httpCode2 = http2.GET();
      if (httpCode2 == 200)
      {
        String response2 = http2.getString();
        DynamicJsonDocument doc2(1024);
        DeserializationError error2 = deserializeJson(doc2, response2);
        String status = doc2["status"];
        if (status == "done")
        {
          JsonArray sites = doc2["sites_found"];
          int count = doc2["count"];
          Serial.println("Success! Response: " + response2);
          display.setCursor(10, 10);
          display.println("Found: " + String(count) + " sites");
          display.setCursor(10, 20);
          display.println("Found: " + String(sites));
          display.display();
          searchDone = true;
        }
      }
      http2.end();
    }
  }
  delay(8000);
}
