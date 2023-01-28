#include <M5StickC.h>
// #include <SPI.h>
#include <HTTPClient.h>
#include <WiFi.h>

const char *SSID = "";
const char *WiFiPassword = "";

uint32_t targetTime = 0;
String price;
float previousPrice;

// Reset screen between functions.
void clearScreen() { M5.Lcd.fillScreen(TFT_BLACK); };

void doTheThing() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String serverPath = "https://api.coingecko.com/api/v3/simple/"
                        "price?ids=bitcoin&vs_currencies=usd&include_market_"
                        "cap=false&include_24hr_vol=false&include_24hr_change="
                        "false&include_last_updated_at=false";
    http.begin(serverPath.c_str());
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {

      Serial.println(previousPrice, 5);

      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);

      // Adjust the substring parameters as needed.
      price = payload.substring(18, 23);
      float currentPrice = price.toFloat();
      Serial.println(currentPrice, 5);

      clearScreen();
      M5.Lcd.setTextColor(TFT_ORANGE);
      // Change the last # to make font smaller/larger.
      M5.Lcd.drawString("Bitcoin", 4, 4, 4);

      // Change the hue according to -/+ change.
      if (currentPrice <= previousPrice) {
        M5.Lcd.setTextColor(TFT_RED);
      } else {
        M5.Lcd.setTextColor(TFT_GREEN);
      }

      // Change the last # to make font smaller/larger.
      M5.Lcd.drawString(price, 4, 40, 6);
      previousPrice = currentPrice;
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }
}

void ConnectToWiFi() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, WiFiPassword);
  Serial.print("Connecting to ");
  Serial.println(SSID);
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);

    if ((++i % 16) == 0) {
      Serial.println(F(" still trying to connect"));
    }
  }

  Serial.print(F("Connected. My IP address is: "));
  Serial.println(WiFi.localIP());
}

void setup(void) {
  M5.begin();
  M5.Lcd.setRotation(1);
  M5.Lcd.setTextColor(TFT_YELLOW);
  targetTime = millis() + 1000;
  ConnectToWiFi();
  doTheThing();
}

unsigned long lastTime = 0;
unsigned long timerDelay = 120000; // 2 minutes.

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    doTheThing();
    lastTime = millis();
  }
}
