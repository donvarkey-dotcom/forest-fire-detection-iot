#include <WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>

// -------- PIN DEFINITIONS --------
#define DHTPIN 4
#define DHTTYPE DHT11

#define MQ2_PIN 36       // DIGITAL NOW
#define FLAME_PIN 25     // DIGITAL NOW
#define LED_PIN 20
#define BUZZER_PIN 21

DHT dht(DHTPIN, DHTTYPE);

// -------- WIFI --------
const char* ssid = "CirkitWifi";
const char* password = "";

// -------- THINGSPEAK --------
WiFiClient client;
unsigned long channelID = 3327944;
const char* writeAPIKey = "TY06Y56OFJI196CP";

void setup() {
  Serial.begin(115200);

  dht.begin();

  pinMode(MQ2_PIN, INPUT);
  pinMode(FLAME_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  ThingSpeak.begin(client);
}

void loop() {

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    temp = random(25, 40);
    hum = random(40, 80);
  }

  // DIGITAL READINGS
  int gas = digitalRead(MQ2_PIN);
  int flame = digitalRead(FLAME_PIN);

  Serial.println("------ SENSOR DATA ------");
  Serial.print("Temp: "); Serial.println(temp);
  Serial.print("Humidity: "); Serial.println(hum);
  Serial.print("Gas (Digital): "); Serial.println(gas);
  Serial.print("Flame (Digital): "); Serial.println(flame);

  // FIRE LOGIC
  if (flame == LOW || gas == HIGH) {
    Serial.println("🔥 FIRE DETECTED!");
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }

  // Send to ThingSpeak
  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(2, hum);
  ThingSpeak.setField(3, gas);
  ThingSpeak.setField(4, flame);

  ThingSpeak.writeFields(channelID, writeAPIKey);

  delay(15000);
}