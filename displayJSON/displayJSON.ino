#include <ArduinoJson.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);

// Global Wifi Setup
const char* ssid = "whynot_2.4GHz";
const char* password = "this7465";
const char* serverURL = "http://192.168.0.xx:5000/lookup"; //"https://httpbin.org/post";
const char* resultBaseURL = "http://192.168.0.xx:5000/result/";

//Global touch sensor + menu
String usernames[] = {"musangqim", "musang_qim", "musangQim"};
int totalUser = 3;
int selectedIndex = 0;
enum State {SELECTING, MODE_SELECTED, SEARCHING, RESULT};
State currentState = SELECTING;
const char* menuItems[] = {"RANDOM FIND", "github", "twitter", "instagram", "threads"};
const int MENU_COUNT = 5;
int modeIndex = 0;
String selectedMode = "";
String selectedSite = "";
const int PIN_UP = 18;
const int PIN_SELECT = 5;
const int PIN_DOWN = 4;

//Global LED and Buzzer
const int LED_R = 19;
const int LED_G = 23;
const int BUZZ = 2;
unsigned long lastBlinkTime = 0;
unsigned long lastPollTime = 0;
unsigned long lastBeepTime = 0;
bool ledRState = false;
bool donePlay = false;

String job_id = "";
bool searchDone = false;
int totalSitesFound = 0;
String sitesList[20];
int scrollIndex = 0;

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
  pinMode(PIN_UP, INPUT);
  pinMode(PIN_DOWN, INPUT);
  pinMode(PIN_SELECT, INPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(BUZZ, OUTPUT);
}

void loop() 
{
  switch (currentState)
  {
    case SELECTING:
    {
      // choose username
      display.clearDisplay();
      display.setCursor(10, 10);
      display.println("Select Username:");
      display.setCursor(10, 20);
      display.println("> " + usernames[selectedIndex]);
      display.display();
      digitalWrite(LED_G, LOW);
      digitalWrite(LED_R, LOW);
      donePlay = false;
      if (digitalRead(PIN_UP) == HIGH)
      {
        selectedIndex = (selectedIndex - 1 + totalUser) % totalUser;
        delay(300);
      }
      if (digitalRead(PIN_DOWN) == HIGH)
      {
        selectedIndex = (selectedIndex + 1 + totalUser) % totalUser;
        delay(300);
      }
      if (digitalRead(PIN_SELECT) == HIGH)
      {
        currentState = MODE_SELECTED;
        modeIndex = 0;
        searchDone = false;
        delay(300);
      }
      break;
    }
    case MODE_SELECTED
    {
      display.clearDisplay();
      display.setCursor(10, 10);
      display.println("Select Mode:");
      display.setCursor(10, 20);
      display.println("> " + String(menuItems[modeIndex]));
      display.display();
      if (digitalRead(PIN_UP) == HIGH)
      {
        modeIndex = (modeIndex - 1 + MENU_COUNT) % MENU_COUNT;
        delay(300);
      }
      if (digitalRead(PIN_DOWN) == HIGH)
      {
        modeIndex = (modeIndex + 1) % MENU_COUNT;
        delay(300); 
      }
      if (digitalRead(PIN_SELECT) == HIGH)
      {
        if(modeIndex == 0)
        {
          selectedMode = "random";
          selectedSite = "";
        }
        else
        {
          selectedMode = "specific";
          selectedSite = String(menuItems[modeIndex]);
        }
        currentState = SEARCHING;
        searchDone = false;
        delay(300);
      }
      break;
    }
    case SEARCHING:
    {
      if (WiFi.status() != WL_CONNECTED)
      {
        display.clearDisplay();
        display.setCursor(10, 20);
        display.println("WiFi not Connected");
        display.display();
        delay(2000);
        currentState = SELECTING;
        break;
      }
      display.clearDisplay();
      Serial.println("Searching.....");
      display.setCursor(10, 20);
      display.println(("Searching....."));
      display.display();

      // PART POST /lookup
      HTTPClient http;
      http.begin(serverURL);
      String jsonPayload = "{\"username\": \"" + usernames[selectedIndex] + "\"}";
      http.addHeader("Content-Type", "application/json");
      int httpCode = http.POST(jsonPayload);

      if (httpCode != 200)
      {
        Serial.println("POST Failed, code: " + String(httpCode));
        http.end();
        display.clearDisplay();
        display.setCursor(10, 20);
        display.println("POST Error: " + String(httpCode));
        display.display();
        delay(200);
        currentState = SELECTING;
        break;
      }

      String response = http.getString();
      http.end();

      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, response);
      if (error)
      {
        display.setCursor(10, 32);
        display.println("JSON Parse Error");
        display.display();
        delay(2000);
        currentState = SELECTING;
        break;
      }
      job_id = doc["job_id"].as<String>();
      digitalWrite(LED_G, LOW);
      
      // PART POLLING LOOP /result/job_id
      while (searchDone == false)
      {
        if (millis() - lastBeepTime >= 1000)
        {
          tone(BUZZ, 1000, 100);
          lastBeepTime = millis();
        }
        if (millis() - lastBlinkTime >= 500)
        {
          ledRState = !ledRState;
          digitalWrite(LED_R, ledRState);
          lastBlinkTime = millis();
        }
        if (millis() - lastPollTime >= 5000)
        {
          lastPollTime = millis();
          String resultURL = String(resultBaseURL) + job_id;
          HTTPClient http2;
          http2.begin(resultURL);
          int httpCode2 = http2.GET();
          if (httpCode2 == 200)
          {
            String response2 = http2.getString();
            DynamicJsonDocument doc2(2048);
            DeserializationError error2 = deserializeJson(doc2, response2);
            if (!error2)
            {
              String status = doc2["status"];
              if (status == "done")
              {
                JsonArray sites = doc2["sites_found"];
                totalSitesFound = doc2["count"];
                Serial.println("Success! Response: " + response2);
                for (int i = 0; i < totalSitesFound && i < 20; i++)
                  sitesList[i] = sites[i].as<String>();
                searchDone = true;
                scrollIndex = 0;
                currentState = RESULT;
              }
            }
          }
          http2.end();
        }
      }
      break;
    }
    case RESULT:
    {
      digitalWrite(LED_R, LOW);
      digitalWrite(LED_G, HIGH);
      if (donePlay == false)
      {
        tone(BUZZ, 1000, 200);
        delay(200);
        tone(BUZZ, 1500, 200);
        delay(200);
        tone(BUZZ, 2000, 300);
        delay(300);
        donePlay = true;
      }
      display.clearDisplay();
      display.setCursor(10, 10);
      display.println("Found: " + String(totalSitesFound) + " sites");
      for (int i = 0; i < 3; i++)
      {
        int siteNum = scrollIndex + i;
        if (siteNum < totalSitesFound)
        {
          display.setCursor(10, 20 + (i * 12));
          display.println("> " + sitesList[siteNum]);
        }
      }
      display.display();
      if (digitalRead(PIN_UP) == HIGH)
      {
        if (scrollIndex > 0)
          scrollIndex = scrollIndex - 1;
        delay(300);
      }
      if (digitalRead(PIN_DOWN) == HIGH)
      {
        if (scrollIndex < totalSitesFound - 3)
        scrollIndex = scrollIndex + 1;
        delay(300);
      }
      if (digitalRead(PIN_SELECT) == HIGH)
      {
        currentState = SELECTING;
        delay(300);
      }
      break;
    }
  }
}