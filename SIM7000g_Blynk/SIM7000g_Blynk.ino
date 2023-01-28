/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  Attention! Please check out TinyGSM guide:
    https://tiny.cc/tinygsm-readme

  Change GPRS apm, user, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!

 *************************************************************/

/* Fill-in your Template ID (only if using Blynk.Cloud) */
#define BLYNK_TEMPLATE_ID ""
#define BLYNK_DEVICE_NAME ""
#define BLYNK_AUTH_TOKEN ""

// Select your modem:
#define TINY_GSM_MODEM_SIM7000

// Default heartbeat interval for GSM is 60
// If you want override this value, uncomment and set this option:
//#define BLYNK_HEARTBEAT 30

#include <TinyGsmClient.h>
#include <BlynkSimpleTinyGSM.h>

#include <Arduino.h>
#include <Wire.h>


BlynkTimer timer;

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = BLYNK_AUTH_TOKEN;

// Your GPRS credentials
// Leave empty, if missing user or pass
char apn[]  = "www";
char user[] = "";
char pass[] = "";

#define SerialAT Serial1
#define UART_BAUD   9600
#define PIN_DTR     25
#define PIN_TX      27
#define PIN_RX      26
#define PWR_PIN     4
#define LED_PIN     12   
#define BAT_ADC     35
bool reply = false;

TinyGsm modem(SerialAT);

BLYNK_WRITE(V3) {
  if(param.asInt()==1)
  {

   digitalWrite(LED_PIN, LOW);
   Blynk.logEvent("led_off");//Sending Events
  }
  else{
    digitalWrite(LED_PIN, HIGH);
     Blynk.logEvent("led_on");//Sending Events
  }
}

//Syncing the output state with the app at startup
BLYNK_CONNECTED()
{
Blynk.syncVirtual(V3);  // will cause BLYNK_WRITE(V3) to be executed
}


float readBattery(uint8_t pin)
{
    int vref = 1100;
    uint16_t volt = analogRead(pin);
    float battery_voltage = ((float)volt / 4095.0) * 2.0 * 3.3 * (vref);
    return battery_voltage;
}

 // This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void sendSensor()
{
  float h = 32.2;
  float t = 55.3;
  float mv = readBattery(BAT_ADC);
  Serial.print("mv :");  Serial.println(mv);
 Serial.print("Pressure :");  Serial.println(h);
 Serial.print("Temperature :");  Serial.println(t);

  // You can send any value at any time.
  // Please don't send more that 10 values per second.
    
  Blynk.virtualWrite(V0, t); 
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V2, ((mv/4200)*100));
}




void setup() {
  Serial.begin(115200); // Set console baud rate
  delay(100);

  // Set LED OFF
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, HIGH);
  delay(300);
  digitalWrite(PWR_PIN, LOW);

  Serial.println("\nWait...");

  delay(1000);

  SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);


   // Restart takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println("Initializing modem...");
  if (!modem.restart()) {
    Serial.println("Failed to restart modem, attempting to continue without restarting");
  }

  String name = modem.getModemName();
  delay(500);
  Serial.println("Modem Name: " + name);


  
   Blynk.begin(auth, modem, apn, user, pass);
   // Setup a function to be called every second
  timer.setInterval(2000L, sendSensor);
}

void loop() {
 
  Blynk.run();
  timer.run();

}
