#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_SHT4x.h>

const char* WIFI_SSID = "wifi-name";
const char* WIFI_PASS = "password";

// Europe/Prague, automatic summer/winter time
const char* TZ_INFO = "CET-1CEST,M3.5.0,M10.5.0/3";

Adafruit_SSD1306 display(128, 64, &Wire, -1, 100000, 100000);
Adafruit_SHT4x sht4;
bool sensorOk = false;

void setup() {
  Wire.begin(3, 2);  // SDA, SCL
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  sensorOk = sht4.begin();

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Connecting WiFi...");
  display.display();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) delay(250);

  configTzTime(TZ_INFO, "pool.ntp.org", "time.google.com");
}

void loop() {
  sensors_event_t hum, temp;
  if (sensorOk) sht4.getEvent(&hum, &temp);

  struct tm t;
  char timeStr[9] = "--:--:--";
  char dateStr[11] = "no time";
  if (getLocalTime(&t, 100)) {
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &t);
    strftime(dateStr, sizeof(dateStr), "%d.%m.%Y", &t);
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print(timeStr);
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.print(dateStr);
  if (sensorOk) {
    display.setTextSize(2);
    display.setCursor(0, 34);
    display.printf("%.1f C", temp.temperature);
    display.setTextSize(1);
    display.setCursor(0, 54);
    display.printf("Humidity %.0f %%", hum.relative_humidity);
  } else {
    display.setTextSize(2);
    display.setCursor(0, 34);
    display.print("No sensor");
  }
  display.display();

  delay(1000);
}