/*
visit  www.facebook.com/theplantdoctor2014/
for wiring and more info; www.instructables.com/id/pot2/

nodemcu v1.0 via arduino ide

dht22 data line on 12   	//D6
light sensor on 13      	//D7
water sensor on A0      	//analog input

blynk setup
download blynk app on device
start new project, name it, choose board: esp8266, email yourself AUTH TOKEN and insert it to line 53
add widget > value display m > name: water, input V4 0-1023  > read freq: 11sec
add widget > value display m > name: light, input V3 0-1   > read freq: 11sec
add widget > value display m > name: temp c, input V1 0-45   > read freq: push  //not sure why this is push, just testing
add widget > value display m > name: humid %, input V2 0-100 > read freq: push  //not sure why this is push, just testing
add widget > superchart > data to show: V1, V2, V3, V4

change line 89 with your locations wifi name & password

*/

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <DHT.h>
#define DHTPIN 5  //D1
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
//float humidity, temp_f; 

int sensor_light = 13;  //D7
int value_light;

int sensor_water = A0;  //A0 analog input
int value_water;

char auth[] = "TIMAvpnFQ9xqGoJZTx_IADiQK8_9LwCs"; //enter blynk auth token

//#include <SimpleTimer.h> 
//SimpleTimer timer;

void temp_humid(){
  byte h = dht.readHumidity();
  byte t = dht.readTemperature();
  Blynk.virtualWrite(0, t);  			 //V1
  Blynk.virtualWrite(1, h);  			 //V2
}

void water_light(){
  value_light = digitalRead(sensor_light);
  Blynk.virtualWrite(3, value_light);    //V3
  
  value_water = analogRead(sensor_water);
  Blynk.virtualWrite(4, value_water);    //V4
//  if (value_water < 300) {
//  Blynk.email("xxxx@gmail.com", "ALARM", "Plant Thirsty");   //enter your email address
//   for(int i=0;i<2;i++){
//  strip.setPixelColor(0, 0, 0, 250);  //turn LEDs blue
//  strip.setPixelColor(1, 0, 0, 250);  
//  strip.show();
//   }
//  }else{
//   for(int i=0;i<2;i++){
//  strip.setPixelColor(0, 0, 250, 0);  //turn LEDs green
//  strip.setPixelColor(1, 0, 250, 0);  
//  strip.show();
//    }        
//  }
}

void setup() {
  //Serial.begin(9600);  			  //delete serial comments when troubleshoot is necessary
  Blynk.begin(auth, "Montecillo1", "abcdef0123456789abcdef");  //enter pot2 location wifi name & password
  dht.begin();
  pinMode(sensor_light, INPUT);

//  strip.begin();
//timer.setInterval(5000, water_light); //what is the best way of uploading this data.? the device does not need to be ON
//timer.setInterval(1000, temp_humid);  //for more than 3times/day, each time about 60seconds. wake up just to get readings from all sensors
}                   					//3 times, 20seconds apart. go to deep sleep, wake up8hrs later, do the same for 60seconds...

void loop() {
  Blynk.run();
  water_light();
  temp_humid();
  delay(4700);  
}
