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
bool isBlynkConnected = false;

void updateStatusLED() {
  digitalWrite(statusLedPin, (WiFi.status() == WL_CONNECTED && Blynk.connected()) ? HIGH : LOW);
}

BLYNK_WRITE(V1) {
  digitalWrite(relayPin, param.asInt() ? LOW : HIGH);
}

void checkWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, pass);
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);

  pinMode(statusLedPin, OUTPUT);
  digitalWrite(statusLedPin, LOW);

  Blynk.config(auth);
  
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
    if (MDNS.begin("esp-01")) {
      Serial.println("mDNS responder started");
    }
  }

  ArduinoOTA.setHostname("esp-01");
  ArduinoOTA.setPassword("12345678");
  ArduinoOTA.begin();
}

void loop() {
  ArduinoOTA.handle();
  Blynk.run();
  
  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 500) {
    lastCheck = millis();
    checkWiFi();
    updateStatusLED();
  }
}

  Blynk.run();
  timer.run();
}
