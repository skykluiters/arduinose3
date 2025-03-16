#include <DHT.h>

#define DHTPIN 2      // DHT22 is verbonden met pin 2
#define DHTTYPE DHT22 // Type sensor
DHT dht(DHTPIN, DHTTYPE);

int soundPin = 13; // Geluidssensor
int ldrPin = A0;   // Lichtsensor
int luchtPin = A1; // Luchtsensor

int ldrVal = 0;    // Lichtsensor waarde op 0 zetten
int co2Zero = 55;  // Gekalibreerde CO2 0-niveau

unsigned long previousDHTMillis = 0; // Tijdstip van laatste DHT22 meting
const long dhtInterval = 2000; // Interval van 2 seconden voor DHT22
unsigned long previousCO2Millis = 0; // Tijdstip van laatste CO2 meting
const long co2Interval = 2000; // Interval van 2 seconden voor CO2-meting
unsigned long previousSensorMillis = 0; // Tijdstip van laatste licht- en geluidsmeting
const long sensorInterval = 200; // Interval van 200ms voor licht en geluid

void setup() {
  Serial.begin(9600);
  pinMode(soundPin, INPUT);
  pinMode(luchtPin, INPUT);
  dht.begin();
}

void loop() {
  unsigned long currentMillis = millis();

  // Lichtsensor en Geluidsensor Code (elke 200ms)
  if (currentMillis - previousSensorMillis >= sensorInterval) {
    previousSensorMillis = currentMillis;
    
    ldrVal = analogRead(ldrPin);
    Serial.println(ldrVal);
    if (ldrVal > 930) {
      Serial.println("DARK"); // Stuur "DARK" als het donker is
    } else {
      Serial.println("LIGHT"); // Stuur "LIGHT" als het licht is
    }

    int soundVolume = digitalRead(soundPin);
    if (soundVolume == HIGH) {
      Serial.println("LUID"); // Stuur "LUID" als er geluid is
    } else {
      Serial.println("STIL"); // Stuur "STIL" als er geen geluid is
    }
  }

  // MQ-135 luchtsensor Code (elke 2s)
  if (currentMillis - previousCO2Millis >= co2Interval) {
    previousCO2Millis = currentMillis;
    int co2now[10];
    int co2raw = 0;
    int co2ppm = 0;
    int zzz = 0;

    for (int x = 0; x < 10; x++) { // Sample CO2 10x
      co2now[x] = analogRead(luchtPin);
    }

    for (int x = 0; x < 10; x++) { // Samples optellen
      zzz = zzz + co2now[x];
    }

    co2raw = zzz / 10; // Gemiddelde berekenen
    co2ppm = co2raw - co2Zero; // CO2 PPM berekenen

    Serial.print("AirQuality=");
    Serial.print(co2ppm);
    Serial.println(" PPM");
  }

  // DHT22 Code (elke 2s)
  if (currentMillis - previousDHTMillis >= dhtInterval) {
    previousDHTMillis = currentMillis;
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    
    if (!isnan(temperature) && !isnan(humidity)) {
      Serial.print("Temperatuur: ");
      Serial.print(temperature);
      Serial.print(" °C, Luchtvochtigheid: ");
      Serial.print(humidity);
      Serial.println(" %");
    } else {
      Serial.println("Fout bij uitlezen DHT22!");
    }
  }
}
