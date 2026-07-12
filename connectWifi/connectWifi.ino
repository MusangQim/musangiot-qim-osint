#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "whynot_2.4GHz";
const char* password = "this7465";
const char* serverURL = "https://httpbin.org/post";//"http://192.168.1.50:5000/lookup";

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
    if (httpCode == 200)
    {
      String response = http.getString();
      Serial.println("Success! Response: " + response);
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