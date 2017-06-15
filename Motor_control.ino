#include <LWiFi.h>
#include <WiFiClient.h>
#include <Servo.h>
#include "MCS.h"

// Assign AP ssid / password here
#define _SSID "Xperia XZ_9fcf"
#define _KEY  "77777777"

// Assign device id / key of your test device
MCSDevice mcs("DAIiPdZ0", "c1acrs2ohoiNouvW");

// Assign channel id 
// The test device should have 2 channel
// the first channel should be "Controller" - "On/Off"
// the secord channel should be "Display" - "On/Off"
MCSControllerOnOff led("LED_Control");
MCSDisplayOnOff remote1("LED_Display");
MCSControllerAnalog motor("motor123");
MCSDisplayInteger    remote("Motor_show");

#define MOTOR_PIN 9
#define LED_PIN 7
Servo myservo;

void setup() {
  // setup Serial output at 9600
  Serial.begin(9600);
  myservo.attach(MOTOR_PIN);
  myservo.write(90);

  pinMode(LED_PIN,OUTPUT);
  

  // setup Wifi connection
  while(WL_CONNECTED != WiFi.status())
  {
    Serial.print("WiFi.begin(");
    Serial.print(_SSID);
    Serial.print(",");
    Serial.print(_KEY);
    Serial.println(")...");
    WiFi.begin(_SSID, _KEY);
  }
  Serial.println("WiFi connected !!");

  // setup MCS connection
  mcs.addChannel(motor);
  mcs.addChannel(remote);
  mcs.addChannel(led);
  mcs.addChannel(remote1);
  while(!mcs.connected())
  {
    Serial.println("MCS.connect()...");
    mcs.connect();
  }
  Serial.println("MCS connected !!");

  // read MOTOR value from MCS server
  while(!motor.valid())
  {
    Serial.println("read MOTOR value from MCS...");
    motor.value();
  }
  Serial.print("done, MOTOR value = ");
  Serial.println(motor.value());
//  digitalWrite(MOTOR_PIN, led.value() ? 0 : 180);
  myservo.write(motor.value());

    // read MOTOR value from MCS server
  while(!led.valid())
  {
    Serial.println("read led value from MCS...");
    led.value();
  }
  Serial.print("done, LED value = ");
  Serial.println(led.value());
   digitalWrite(LED_PIN, led.value() ? HIGH : LOW);
}

void loop() {
  // call process() to allow background processing, add timeout to avoid high cpu usage
  Serial.print("process(");
  Serial.print(millis());
  Serial.println(")");
  mcs.process(500);
  
  // updated flag will be cleared in process(), user must check it after process() call.
  if(motor.updated())
  {
    Serial.print("MOTOR updated, new value = ");
    Serial.println(motor.value());
//    digitalWrite(MOTOR_PIN, motor.value() ? HIGH : LOW);
    myservo.write(motor.value());
    if(!remote.set(motor.value()))
    {
      Serial.print("Failed to update remote");
      Serial.println(remote.value());
    }
  }

  if(led.updated())
  {
    Serial.print("LED updated, new value = ");
    Serial.println(led.value());
    digitalWrite(LED_PIN, led.value() ? HIGH : LOW);
    if(!remote1.set(led.value()))
    {
      Serial.print("Failed to update remote");
      Serial.println(remote1.value());
    }
  }
  
  // check if need to re-connect
  while(!mcs.connected())
  {
    Serial.println("re-connect to MCS...");
    mcs.connect();
    if(mcs.connected())
      Serial.println("MCS connected !!");
  }
}
