#include <SPI.h>
#include <LoRa.h>



void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Sender");

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  String Message = "<lat:"+String(random(0,5000))+";lon:"+String(random(0,5000))+";adt:"+String(random(0,100))+";tem:"+String(random(0,34))+";hum:"+String(random(0,100))+";pre:"+String(random(1000,1040))+";mq:"+String(random(0,500))+";mq_max:"+String(random(0,600))+";pm1_0:"+String(random(0,50))+";pm2_5:"+String(random(0,75))+";pm10:"+String(random(0,100))+";>";
  Serial.print("Sending message: ");
  Serial.print(Message);
  Serial.print('\n');
  // send packet
  LoRa.beginPacket();
  LoRa.print(Message);
  LoRa.endPacket();

  delay(5000);
}