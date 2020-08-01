#include <ArduinoJson.h>
#include <string>
#include <FirebaseCloudMessaging.h>
#include <Firebase.h>
#include <FirebaseHttpClient.h>
#include <FirebaseArduino.h>
#include <FirebaseError.h>
#include <FirebaseObject.h>

#include <ESP8266WiFiGratuitous.h>
#include <WiFiServerSecure.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <ESP8266WiFiType.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFiAP.h>
#include <WiFiClient.h>
#include <BearSSLHelpers.h>
#include <WiFiServer.h>
#include <ESP8266WiFiScan.h>
#include <WiFiServerSecureBearSSL.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiSTA.h>
#include <WiFiClientSecureAxTLS.h>
#include <WiFiServerSecureAxTLS.h>
#include <NTPClient.h>

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

#define FIREBASE_AUTH "fEH52lWMDBlzPTuam7Hma9NrV5WWVJ0joJkb9caz"
#define FIREBASE_HOST "freebowler-trial.firebaseio.com"

const char *ssid = "Tenda_275060";
const char *pass = "samruddhi";
const char* server = "api.thingspeak.com";

const long utcOffset = 19800;

#define sessionSwitch D8
#define sessionLED D7
#define sensorSwitch D2
#define sensorLED D1

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", utcOffset);

WiFiClient client;
byte session, sensor;
String times = "",t="";
int counter = 0;
bool sessionStart=false;
void setup() {
  // put your setup code here, to run once:
  //pinMode(sessionLED, OUTPUT);
  pinMode(sensorLED, OUTPUT);
  //pinMode(sensorSwitch, INPUT);
  pinMode(sessionSwitch, INPUT);
  Serial.begin(115200);
  Serial.println();
  delay(2000);
  Serial.print("Wifi Connecting to ");
  Serial.println( ssid );

  WiFi.begin(ssid, pass);

  Serial.println();
  Serial.print("Connecting");

  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("Wifi Connected Success!");
  Serial.print("NodeMcu IP Address : ");
  Serial.println(WiFi.localIP());

  timeClient.begin();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

}

void loop() {
  // put your main code here, to run repeatedly:
  //session = digitalRead(sessionSwitch);
  Serial.print("Before reading session - >");
  Serial.println(sessionStart);
  sessionStart = Firebase.getBool("sessionStart");
  Serial.print("After reading session - >");
  Serial.println(sessionStart);
  if (sessionStart == true) {
    digitalWrite(sessionLED, HIGH);
    sensor = digitalRead(sensorSwitch);
    if (sensor == HIGH) {
      digitalWrite(sensorLED, HIGH);
      timeClient.update();
      t ="";
      t+= String(timeClient.getHours())  + ":" + String(timeClient.getMinutes()) + ":" + String(timeClient.getSeconds());
      counter++;
      times+= t + ", ";
      Serial.print("Counter - ");
      Serial.println(counter);
      if (counter == 4) {
        Serial.print("Sending Data to Firebase - ");
        Serial.print(times);
        Firebase.setString("Release Time", times);
        if (Firebase.failed()) {
          Serial.print("setting /number failed:");
          Serial.println(Firebase.error());
          return;
        }
        counter = 0;
      }
      delay(5000);
    }
    else {
      digitalWrite(sensorLED, LOW);
    }
  }
  else {
    digitalWrite(sessionLED, LOW);
  }
}
