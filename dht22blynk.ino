#include "DHT.h"                              // DHT Sensor
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "TIMAvpnFQ9xqGoJZTx_IADiQK8_9LwCs";
char ssid[] = "Montecillo1";
char pass[] = "abcdef0123456789abcdef";

DHT dhtA(5, DHT22);                           // DHT instance named dhtA, D1 and sensor type

void setup() {
  Blynk.begin(auth, ssid, pass);              // Connecting to blynk
  dhtA.begin();                               // Starting the DHT-22
}

void loop() {
  Blynk.run();
  climateRoutine();                           // Climate routine
  delay(4700);                                // 4.7 sec between routines
}

void climateRoutine() {
    byte h1 = dhtA.readHumidity();            // f1 and h1 are celsius and humidity readings
    // byte t1 = dhtA.readTemperature(true);  // for temperature in farenheits
    byte t1 = dhtA.readTemperature();         // from DHT/A
    Blynk.virtualWrite(V0, t1);               //  Set Virtual Pin 0 frequency to PUSH in Blynk app
    Blynk.virtualWrite(V1, h1);               //  Set Virtual Pin 1 frequency to PUSH in Blynk app
}
