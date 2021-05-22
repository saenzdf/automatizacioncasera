#include <DHT.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include <NTPClient.h>
#include <time.h>                       // time() ctime()
#include <CronAlarms.h>
#include <WiFiUdp.h>

#define WIFI_SSID "wifi provider"
#define WIFI_PASSWORD "your password"

// Raspberri Pi Mosquitto MQTT Broker
#define MQTT_HOST IPAddress(34, 122, 20, 41)
// For a cloud MQTT broker, type the domain name
//#define MQTT_HOST "example.com"
#define MQTT_PORT 1883

// Temperature MQTT Topics
#define MQTT_PUB_TEMP "esp/temperature"
#define MQTT_PUB_HUM "esp/humidity"
#define MQTT_PUB_LIGHT "esp/light"
#define MQTT_PUB_WATER "esp/water"

// Digital pin connected to the DHT sensor
#define DHTPIN 5  //D1 on the board
#define DHTTYPE DHT22 

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// Variables to hold sensor readings
float temp;
int hum;
int light;
int water;

//digital pin connected to the light sensor
int sensor_light = 13;  //D7 on the board

//analog pin conneced to the humidity capacitor sensor
int sensor_water = A0;  //A0 analog input on the board

// digital pins connected to the relay module
const int relaylight = 14;
const int relaywater = 12;

//to store hour value
int h;

WiFiUDP ntpUDP;
CronId id;

// You can specify the time server pool and the offset, (in seconds)
// additionaly you can specify the update interval (in milliseconds).
NTPClient timeClient(ntpUDP, "pool.ntp.org", -18000, 3600000);

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

unsigned long previousMillis = 0;   // Stores last time temperature was published
const long interval = 10000;        // Interval at which to publish sensor readings

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Connected to Wi-Fi.");
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(2, connectToWifi);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

/*void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}*/

void onMqttPublish(uint16_t packetId) {
  Serial.print("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

// Alarm to be done daily
void dlight() {
   Serial.println("Prende luces");
    digitalWrite(relaylight, LOW);
} 
void nlight() {
   Serial.println("Apaga luces");
    digitalWrite(relaylight, HIGH);
}

// For watering the plants
void waterplants(){
  if (water > 650) {
      Serial.println("Bomba de agua prendida");   
      digitalWrite (relaywater, LOW); // relay on
  }else{
      Serial.println("Bomba de agua apagada"); 
      digitalWrite (relaywater, HIGH); // relay off
  }  
}


void setup() {
  Serial.begin(9600);
  Serial.println();

  //It initialize relay module's pins and dht's pins
  pinMode(relaylight, OUTPUT);
  digitalWrite(relaylight, HIGH);
  pinMode(relaywater, OUTPUT);
  digitalWrite(relaywater, HIGH);
  dht.begin();
  pinMode(sensor_light, INPUT);
  
  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
  connectToWifi();
   
  mqttClient.onConnect(onMqttConnect);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
   // If your broker requires authentication (username and password), set them below
  //mqttClient.setCredentials("sotara", "masqueel0r0");
  //mqttClient.onSubscribe(onMqttSubscribe);
  //mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.onDisconnect(onMqttDisconnect);

  timeClient.begin();//Inilize time
  // create the alarms, to trigger at specific times
 Cron.create("0 30 18 * * *", dlight, false);  // 6:30pm every day
  id=Cron.create("0 30 0 * * *", nlight, false);  // 0:30am every day
 
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Every X number of seconds (interval = 10 seconds) 
  // it publishes a new MQTT message
  
  if (currentMillis - previousMillis >= interval) {
    // Save the last time a new reading was published
    previousMillis = currentMillis;
    // New DHT sensor readings
    hum = dht.readHumidity();
    // Read temperature as Celsius (the default)
    temp = dht.readTemperature();
    //light sensor new read
    light = digitalRead(sensor_light);
    //humidity sensor analog read
    water = analogRead(sensor_water);
      
    // Publish an MQTT message on topic esp/dht/temperature
    uint16_t packetIdPub1 = mqttClient.publish(MQTT_PUB_TEMP, 1, true, String(temp).c_str());                            
    Serial.printf("Publishing on topic %s at QoS 1, packetId %i: ", MQTT_PUB_TEMP, packetIdPub1);
    Serial.print("Temperatura: ");
    Serial.print(temp);
    Serial.println("c°");

    // Publish an MQTT message on topic esp/dht/humidity
    uint16_t packetIdPub2 = mqttClient.publish(MQTT_PUB_HUM, 1, true, String(hum).c_str());                            
    Serial.printf("Publishing on topic %s at QoS 1, packetId %i: ", MQTT_PUB_HUM, packetIdPub2);
    Serial.print("Humedad: ");
    Serial.print(hum);
    Serial.println("%");
    
    // Publish an MQTT message on topic esp/light
    uint16_t packetIdPub3 = mqttClient.publish(MQTT_PUB_LIGHT, 1, true, String(light).c_str());                            
    Serial.printf("Publishing on topic %s at QoS 1, packetId %i: ", MQTT_PUB_LIGHT, packetIdPub3);
    Serial.print("Luz: ");
    Serial.println(light);
    
    // Publish an MQTT message on topic esp/water
    uint16_t packetIdPub4 = mqttClient.publish(MQTT_PUB_WATER, 1, true, String(water).c_str());                            
    Serial.printf("Publishing on topic %s at QoS 1, packetId %i: ", MQTT_PUB_WATER, packetIdPub4);
    Serial.print("Sensor riego: ");
    Serial.println(water);

     waterplants();
     timeClient.update();
     h= timeClient.getHours();
     Serial.println(timeClient.getFormattedTime()); // print time
  }

  // for turning on the lights if the esp gets connected after the cron hour
  
 if ((h >= 18 ) && (h <= 23))  
     dlight();
 else nlight();
     
    
  
    Cron.delay(500);// if the loop has nothing else to do, delay in ms
               // should be provided as argument to ensure immediate
               // trigger when the time is reached
    delay(500);
}
