/*
 * CronAlarmsExample.ino
 *
 * This example calls alarm functions at 8:30 am and at 5:45 pm (17:45)
 * and simulates turning lights on at night and off in the morning
 * A weekly timer is set for Saturdays at 8:30:30
 *
 * A timer is called every 15 seconds
 * Another timer is called once only after 10 seconds
 *
 * At startup the time is set to Jan 1 2011  8:29 am
 */
#include <NTPClient.h>
#include <time.h>                       // time() ctime()
#include "CronAlarms.h"
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

const char *ssid     = "Montecillo1";
const char *password = "abcdef0123456789abcdef";

const int relayluz = 14;

WiFiUDP ntpUDP;
CronId id;


// You can specify the time server pool and the offset, (in seconds)
// additionaly you can specify the update interval (in milliseconds).
 NTPClient timeClient(ntpUDP, "pool.ntp.org", -18000, 3600000);

void setup() {
  Serial.begin(9600);
//pin relay setup
  pinMode(relayluz, OUTPUT);
  digitalWrite(relayluz, HIGH);
  
  Serial.println("Starting setup...");
   WiFi.begin(ssid, password);
  timeClient.begin();


  // create the alarms, to trigger at specific times
  id=Cron.create("0 30 0 * * *", MorningAlarm, false);  // 0:30am every day
  Cron.create("*/15 * * * * *", MorningAlarm, false);      // timer for every 2 seconds

  Serial.println("Ending setup...");
}

void loop() {
 timeClient.update();

  Serial.println(timeClient.getFormattedTime());

  Cron.delay(1000);// if the loop has nothing else to do, delay in ms
               // should be provided as argument to ensure immediate
               // trigger when the time is reached
  delay(1000);// do other things... like waiting one second between clock display
}

// functions to be called when an alarm triggers:
void MorningAlarm() {
  Serial.println("Alarm: - turn lights off");
   digitalWrite(relayluz, LOW);
   Cron.delay(5000);
   Serial.println("Alarm: - turn lights on");
    digitalWrite(relayluz, HIGH);
}
