#include <Wire.h>  
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define DHTPIN D5           // Pin for DHT sensor
#define DHTTYPE DHT22       // Use DHT22 if needed
DHT dht(DHTPIN, DHTTYPE);

#define LED_PIN D6          // LED pin
#define MQ135_PIN A0        // Analog pin for MQ135

unsigned long previousMillis = 0;
const long interval = 2000; // OLED update interval

void setup() {
  Serial.begin(115200);
  
  pinMode(LED_PIN, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED failed");
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Starting...");
  display.display();

  dht.begin();
}

void loop() {
  // Blink LED (500 ms)
  static unsigned long previousLedMillis = 0;
  static bool ledState = LOW;
  unsigned long currentMillis = millis();

  if (currentMillis - previousLedMillis >= 500) {
    previousLedMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
  }

  // OLED + Sensor update every 2 seconds
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    int mq135Value = analogRead(MQ135_PIN);

    if (isnan(temp) || isnan(hum)) {
      Serial.println("DHT read failed");
      display.clearDisplay();
      display.setCursor(0,0);
      display.println("DHT read failed");
      display.display();
      return;
    }

    // Interpret MQ135 value (basic)
    String airQuality;
    if (mq135Value < 300) {
      airQuality = "Good";
    } else if (mq135Value < 500) {
      airQuality = "Moderate";
    } else {
      airQuality = "Poor";
    }

    // Debug info
    Serial.print("Temp: ");
    Serial.print(temp);
    Serial.print(" C, Hum: ");
    Serial.print(hum);
    Serial.print(" %, AQ: ");
    Serial.print(mq135Value);
    Serial.print(" (");
    Serial.print(airQuality);
    Serial.println(")");

    // Display on OLED
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("Temp: ");
    display.print(temp);
    display.println(" C");

    display.print("Hum: ");
    display.print(hum);
    display.println(" %");

    display.print("Air: ");
    display.print(airQuality);
    display.display();
  }
}
