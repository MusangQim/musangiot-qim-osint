#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "whynot_2.4Ghz";
const char* password = "this7465";
const char* serverURL = "http://192.168.1.50:5000/lookup";

void setup() 
{
  Serial.begin(115200);
  delay(1000);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi.....");
  }
  Serial.println("WiFi Successfully Connected!!");
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    http.begin(serverURL);
    String jsonPayload = "{\"username\": \"musangqim\"}";
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(jsonPayload);
    if (httpCode > 0)
    {
      Serial.println(httpCode);
    }
    else
    {
      Serial.println("Error on HTTP request");
    }
    http.end();
  }
  delay(10000);
}