#define BLYNK_TEMPLATE_ID "TMPL4g3FuYme8"
#define BLYNK_TEMPLATE_NAME "Relay"
#define BLYNK_AUTH_TOKEN "your_api"  //write your api token blynk
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "your_ssid";  //write your ssid
char pass[] = "your_pass";  //write your wifi password

const int relayPin = 0;
const int statusLedPin = LED_BUILTIN; 

BlynkTimer timer;
int wifiReconnectAttempts = 0;
int blynkReconnectAttempts = 0;

bool ledBlinkState = false;

void checkConnections() {
  if (WiFi.status() != WL_CONNECTED) {
    wifiReconnectAttempts++;
    Serial.print("Wifi connection lost, attempt: ");
    Serial.println(wifiReconnectAttempts);
    WiFi.disconnect();
    WiFi.begin(ssid, pass);
    if (wifiReconnectAttempts > 5) {
      Serial.println("too many attempts to connect to wifi, reset...");
      ESP.reset();
    }
  } else {
    wifiReconnectAttempts = 0;
  }

  if (!Blynk.connected()) {
    blynkReconnectAttempts++;
    Serial.print("Blynk connection lost, attempt: ");
    Serial.println(blynkReconnectAttempts);
    Blynk.connect();
    if (blynkReconnectAttempts > 7) {
      Serial.println("too many attempts to connect blynk, reset...");
      ESP.reset();
    }
  } else {
    blynkReconnectAttempts = 0;
  }
}

void updateStatusLED() {
  //If there is no connection, turn on the blue LED
  if (WiFi.status() != WL_CONNECTED || !Blynk.connected()){
    ledBlinkState = !ledBlinkState;
    digitalWrite(statusLedPin, ledBlinkState ? LOW : HIGH);
  } else {
    digitalWrite(statusLedPin, HIGH);
  }
}

BLYNK_CONNECTED() {
  Serial.println("Blynk: Connected");
}

BLYNK_DISCONNECTED() {
  Serial.println("Blynk: Disconnected");
}

// When receiving a command from V1, if the value is 1, the relay closes for 1200 ms
BLYNK_WRITE(V1) {
  int pinValue = param.asInt();
  if (pinValue == 1) {
    digitalWrite(relayPin, LOW); 
    timer.setTimeout(1200, [](){
      digitalWrite(relayPin, HIGH);  
    });
  } else {
    digitalWrite(relayPin, HIGH); 
  }
}


void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); 
  
  pinMode(statusLedPin, OUTPUT);
  digitalWrite(statusLedPin, HIGH); 
  Blynk.begin(auth, ssid, pass);

  if (MDNS.begin("esp-01")) {
    Serial.println("mDNS responder start, esp-01.local");
  } else {
    Serial.println("mDNS responder start error");
  }

  // OTA setup
  ArduinoOTA.setHostname("esp-01");
  ArduinoOTA.setPassword("12345678"); // set password

  ArduinoOTA.onStart([]() {
    Serial.println("OTA start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA end");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA прогресс: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error OTA[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Authentication error");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Start error");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connection error");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Reception error");
    else if (error == OTA_END_ERROR) Serial.println("Completion error");
  });
  ArduinoOTA.begin();

  timer.setInterval(100000L, checkConnections);
  timer.setInterval(500L, updateStatusLED);
}

void loop() {
  ArduinoOTA.handle();
  Blynk.run();
  timer.run();
}
