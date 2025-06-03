#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Atng"
#define WIFI_PASSWORD "12345678"

// Insert Firebase project API Key
#define API_KEY "AIzaSyDMYFqv9xCI9LwDgY8BP5qkxzDtm0Eq-AE"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://plantapp-9af90-default-rtdb.firebaseio.com/" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int intValue, L, M;
float floatValue;
bool signupOK = false;
int r1, r2;
int Turn1, Turn2;
int Hour1, Minute1, HourEnd1, MinuteEnd1;
int Hour2, Minute2, HourEnd2, MinuteEnd2;
//---------Cam Bien------------------------//
#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int moisture, sensor_analog;
const int sensor_pin = A0;

#define AO_PIN 39
#define DO_PIN 13
#define RELAY 18
#define RELAY2 19
//---------------------Thời gian-----------------//
#include <RTClib.h>

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
int hour;
int minute;
//----------------------------------------------//
void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

//-----------------Cam Bien--------------------//
  Serial.println(F("Kiem tra nhiet do va do am: "));
  dht.begin();

pinMode(DO_PIN, INPUT);
    pinMode(RELAY, OUTPUT);
    pinMode(RELAY2, OUTPUT);
    digitalWrite(RELAY, LOW);
    digitalWrite(RELAY2, LOW);

//-----------------Thời gian----------------//
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

void loop() {
  //delay(2000);
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


    //do am dat
    sensor_analog = analogRead(sensor_pin);
    moisture = (100 - ((sensor_analog/4095.00)*100));

    //Thời gian
    DateTime now = rtc.now();
  hour = now.hour();
  minute = now.minute();

  // Serial.print("ESP32 RTC Date Time: ");
  // Serial.print(now.year(), DEC);
  // Serial.print('/');
  // Serial.print(now.month(), DEC);
  // Serial.print('/');
  // Serial.print(now.day(), DEC);
  // Serial.print(" (");
  // Serial.print(daysOfWeek[now.dayOfTheWeek()]);
  // Serial.print(") ");
  // Serial.print(now.hour(), DEC);
  // Serial.print(':');
  // Serial.print(now.minute(), DEC);
  // Serial.print(':');
  // Serial.println(now.second(), DEC);

  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    //int lightValue = analogRead(AO_PIN);
    int lightState = digitalRead(DO_PIN);
    //--------Lấy giá trị Firebase---------------//
    if (Firebase.RTDB.getInt(&fbdo, "/Manual")) {
      if (fbdo.dataType() == "int") {
        intValue = fbdo.intData();
        Serial.print("Manual: ");
        Serial.println(intValue);
      }
    }
    else {
      Serial.println(fbdo.errorReason());
    }
    if (Firebase.RTDB.getInt(&fbdo, "/State/Light")) {
      if (fbdo.dataType() == "int") {
         L = fbdo.intData();
      }
    }
    if (Firebase.RTDB.getInt(&fbdo, "/State/Motor")) {
      if (fbdo.dataType() == "int") {
         M = fbdo.intData();
      }
    }
  if(intValue == 1){
    if(L == 1) {digitalWrite(RELAY, HIGH); r1=1;}
    else {digitalWrite(RELAY, LOW); r1=0;}

    if(M == 1) {digitalWrite(RELAY2, HIGH); r2=1;}
    else {digitalWrite(RELAY2, LOW); r2=0;}

    //-------Thời gian--------//
if (Firebase.RTDB.getInt(&fbdo, "/Alarm/Light/TurnOn")) {
      if (fbdo.dataType() == "int") {
         Turn1 = fbdo.intData();
      }
    }
  

if (Firebase.RTDB.getInt(&fbdo, "/Alarm/Motor/TurnOn")) {
      if (fbdo.dataType() == "int") {
         Turn2 = fbdo.intData();
      }
    }
  

if(Turn1 == 1){
  if (Firebase.RTDB.getInt(&fbdo, "/Alarm/Light/Hour")) {
      if (fbdo.dataType() == "int") {
         Hour1 = fbdo.intData();
      }
    }
  
  if (Firebase.RTDB.getInt(&fbdo, "/Alarm/Light/Minute")) {
      if (fbdo.dataType() == "int") {
         Minute1 = fbdo.intData();
      }
    }
  
  if (Firebase.RTDB.getInt(&fbdo, "/Alarm/Light/HourEnd")) {
      if (fbdo.dataType() == "int") {
         HourEnd1 = fbdo.intData();
      }
    }
  
  if (Firebase.RTDB.getInt(&fbdo, "/Alarm/Light/MinuteEnd")) {
      if (fbdo.dataType() == "int") {
         MinuteEnd1 = fbdo.intData();
      }
    }
  
  if(Hour1 == hour && Minute1 == minute){
    digitalWrite(RELAY, HIGH); 
    r1=1;
  }
  if(HourEnd1 == hour && MinuteEnd1 == minute){
    digitalWrite(RELAY, LOW); 
    r1=0;
  }
}

if(Turn2 == 1){
   if (Firebase.RTDB.getInt(&fbdo, "/Alarm/Motor/Hour")) {
      if (fbdo.dataType() == "int") {
         Hour2 = fbdo.intData();
      }
    }
  
  if (Firebase.RTDB.getInt(&fbdo, "/Alarm/Motor/Minute")) {
      if (fbdo.dataType() == "int") {
         Minute2 = fbdo.intData();
      }
    }
  
  if (Firebase.RTDB.getInt(&fbdo, "/Alarm/Motor/HourEnd")) {
      if (fbdo.dataType() == "int") {
         HourEnd2 = fbdo.intData();
      }
    }
  
  if (Firebase.RTDB.getInt(&fbdo, "/Alarm/Motor/MinuteEnd")) {
      if (fbdo.dataType() == "int") {
         MinuteEnd2 = fbdo.intData();
      }
    }
  
  if(Hour2 == hour && Minute2 == minute){
    digitalWrite(RELAY2, HIGH); 
    r2=1;
  }
  if(HourEnd2 == hour && MinuteEnd2 == minute){
    digitalWrite(RELAY2, LOW); 
    r2=0;
  }
}
}
  else {
  if(lightState == HIGH)
    {
      //Serial.println("It is dark");
      digitalWrite(RELAY, HIGH);
      r1=1;
    }
    else
    {
      //Serial.println("It is light");
      digitalWrite(RELAY, LOW);
      r1=0;
    }

  if(moisture<30){
      r2=1;
      digitalWrite(RELAY2, HIGH);
    }
    else{
      r2=0;
      digitalWrite(RELAY2, LOW);
    }
  }

//-------------------------------------//
  // if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
  //   sendDataPrevMillis = millis();
    //--------------------------------------------//
    if (Firebase.RTDB.setInt(&fbdo, "State/Light", r1)) 
    {
      Serial.println(); Serial.print(r1);
      Serial.print("- successfully saved to: " + fbdo.dataPath());
      Serial.println(" (" + fbdo.dataType() + ")");
    }
    else {
      Serial.println("FAILED: " +fbdo.errorReason());
    }

    if (Firebase.RTDB.setInt(&fbdo, "State/Motor", r2)) {
      Serial.println(); Serial.print(r2);
      Serial.print("- successfully saved to: " + fbdo.dataPath());
      Serial.println(" (" + fbdo.dataType() + ")");
    }
    else {
      Serial.println("FAILED: " +fbdo.errorReason());
    }


    if (Firebase.RTDB.setFloat(&fbdo, "Sensor/Humidity", h)) {
      Serial.println(); Serial.print(h);
      Serial.print("- successfully saved to: " + fbdo.dataPath());
      Serial.println(" (" + fbdo.dataType() + ")");
    }
    else {
      Serial.println("FAILED: " +fbdo.errorReason());
    }
    
    if (Firebase.RTDB.setFloat(&fbdo, "Sensor/Temperature", t)) {
      Serial.println(); Serial.print(t);
      Serial.print("- successfully saved to: " + fbdo.dataPath());
      Serial.println(" (" + fbdo.dataType() + ")");
    }
    else {
      Serial.println("FAILED: " +fbdo.errorReason());
    }
    
    if (Firebase.RTDB.setFloat(&fbdo, "SoilSensor/SoilHumidity", moisture)) {
      Serial.println(); Serial.print(moisture);
      Serial.print("- successfully saved to: " + fbdo.dataPath());
      Serial.println(" (" + fbdo.dataType() + ")");
    }
    else {
      Serial.println("FAILED: " +fbdo.errorReason());
    }
    
//-----------------------------------------//
  }
}
