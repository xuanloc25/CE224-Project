#include <Arduino.h>
#include "DHT.h"
#include <RTClib.h>

#define DHTPIN 4 // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
// #define DHTTYPE DHT22 // DHT 22  (AM2302), AM2321/
// #define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is. 
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

RTC_DS3231 rtc;
char daysOfWeek[7][12] = {
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
};

int moisture, sensor_analog;
const int sensor_pin = A0;

#define AO_PIN 39
#define DO_PIN 13
#define RELAY 18
#define RELAY2 19

void setup()
{
    Serial.begin(9600);

    dht.begin();

    pinMode(DO_PIN, INPUT);
    pinMode(RELAY, OUTPUT);
    pinMode(RELAY2, OUTPUT);
    digitalWrite(RELAY, LOW);
    digitalWrite(RELAY2, LOW);

    // SETUP RTC MODULE
    if (! rtc.begin()) {
      Serial.println("RTC module is NOT found");
      Serial.flush();
      while (1);
    }
    // Check if the RTC lost power and if so, set the time:
    if (rtc.lostPower()) {
      Serial.println("RTC lost power, let's set the time!");
      // automatically sets the RTC to the date & time on PC this sketch was compiled
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

      // manually sets the RTC with an explicit date & time, for example to set
      // January 21, 2021 at 3am you would call:
      // rtc.adjust(DateTime(2021, 1, 21, 3, 0, 0));
    }
}

void loop()
{
    delay(2000);

    DateTime now = rtc.now();
    Serial.print("ESP32 RTC Date Time: ");
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.println(now.second(), DEC);

    int lightValue = analogRead(AO_PIN);
    int lightState = digitalRead(DO_PIN);

    Serial.println("The AO value: ");
    Serial.print(lightValue);
    if(lightState == HIGH)
    {
      //Serial.println("It is dark");
      digitalWrite(RELAY, HIGH);
    }
    else
    {
      //Serial.println("It is light");
      digitalWrite(RELAY, LOW);
    }
    //do am dat
    sensor_analog = analogRead(sensor_pin);
    moisture = (100 - ((sensor_analog/4095.00)*100));
    if(moisture<30){
      digitalWrite(RELAY2, HIGH);
    }
    else{
      digitalWrite(RELAY2, LOW);
    }

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f))
    {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
    }

    // Compute heat index in Fahrenheit (the default)
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);

    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.print(F("°C "));
    Serial.print(f);
    Serial.print(F("°F  Heat index: "));
    Serial.print(hic);
    Serial.print(F("°C "));
    Serial.print(hif);
    Serial.println(F("°F"));
    Serial.print("moisture = ");
    Serial.print(moisture);
    Serial.println("%");
}