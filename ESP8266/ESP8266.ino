#include <FirebaseCloudMessaging.h>
#include <Firebase.h>
#include <FirebaseHttpClient.h>
#include <FirebaseArduino.h>
#include <FirebaseError.h>
#include <FirebaseObject.h>
#include <Arduino.h>
#include "HX711.h"

#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>     
#include <Math.h>            
//#include <FirebaseArduino.h>     


const int LOADCELL_DOUT_PIN = 12;
const int LOADCELL_SCK_PIN = 13;
HX711 scale;


const byte RX = D2;
const byte TX = D8;
SoftwareSerial mySerial = SoftwareSerial(RX, TX);
long lastUART = 0;
void Read_Uart();    // UART STM
String LED1 = "OFF", LED2 = "OFF";
String st = "";
String type = "";
String reset = "";
String refill = "";
float weight = 0;
String done1 = "0";
String done2 = "0";
String done3 = "0";

#define FIREBASE_HOST "reallab-c55b6-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "mYHxjJkVTkqSquHEhcDYPquiW46kW0iOghXyS3rs"   
#define WIFI_SSID "Kamol_2.4"                                  
#define WIFI_PASSWORD "0812569363"    

void setup()
{
  Serial.begin(115200);
  mySerial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                  
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.print("Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());                               //prints local IP address

  //lastUART = millis();


  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);                 // connect to the firebase

  Serial.begin(115200);
  Serial.println("HX711 Demo");
  Serial.println("Initializing the scale");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());      // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));   // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight (not set yet)

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);  // print the average of 5 readings from the ADC minus tare weight (not set) divided
            // by the SCALE parameter (not set yet)
            
  scale.set_scale(-513);
  //scale.set_scale(-471.497);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();               // reset the scale to 0

  Serial.println("After setting up the scale:");

  Serial.print("read: \t\t");
  Serial.println(scale.read());                 // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));       // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight, set with tare()

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided
            // by the SCALE parameter set with set_scale

  Serial.println("Readings:");



}
void loop()
{
  Read_Uart();
//  if (millis() - lastUART > 1000)
//{
//    lastUART = millis();
//  }
  reset = Firebase.getString("/val/reset/reset1");
  refill = Firebase.getString("/val/state/state1");
  //weight = Float.parseFloat(Firebase.getString("/val/remaining/count"));  
  if(type=="a"){
    Firebase.setString("/val/alcCount/count",st);
  }
  if(type=="p"){
    Firebase.setString("/val/ppCount/count",st);
  }
  Firebase.setString("/val/remaining/count",String(weight));
  //Serial.println(reset);
  if(type=="s"){
    done1="0";
    Firebase.setString("/val/reset/reset1",st);
  }
  if(type=="l"){
    done2="0";
    Firebase.setString("/val/state/state1",st);
  }
  //mySerial.print("1on2off3on4");
  if(reset=="1" && done1=="0"){
    mySerial.print("s");
    done1="1";
  }
  if(refill=="1" && done2=="0"){
    mySerial.print("l");
    done2="1"; 
  }
  if(weight<10 && done3=="0"){
    mySerial.print("t");
    done3 = "1";
  }
  if(weight>10){
    done3 = "0";  
  }

  
   

  if (Firebase.failed()) {
     Serial.print("setting /number failed:");
     Serial.println(Firebase.error());
     return;
 }

  weight = scale.get_units(10)*100/550;
  weight = floor(weight*100)/100;
//  Serial.println(weight);
//  
//  scale.power_down();             // put the ADC in sleep mode
//  delay(5000);
//  scale.power_up();
}
void Read_Uart()
{
  st="";
  type="";
  while (mySerial.available())
  {
    char inChar = (char)mySerial.read();
    if(inChar!='a' and inChar!='p' and inChar!='s' and inChar!='l'){
      st +=  inChar;
    }
    if(inChar == 'a' or inChar == 'p' or inChar == 's' or inChar == 'l'){
      type=inChar;
      Serial.println(type);
      Serial.println(st);
      break;
    }
  }
}
