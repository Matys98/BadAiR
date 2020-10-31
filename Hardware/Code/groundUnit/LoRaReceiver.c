#include <SPI.h>
#include <LoRa.h>
#include <SoftwareSerial.h>

const int rx = 0;
const int tx = 1;

SoftwareSerial LoRaESP(rx, tx);

void setup(){
  LoRaESP.begin(9600);
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Receiver");

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  String mesage;
  String Oldmesage = " ";
  
  
  mesage = receive_data();
  if(mesage != Oldmesage){
    int len=mesage.length()+1;
    char buf[len];
    mesage.toCharArray(buf, len) ;
    LoRaESP.write(buf);
    Serial.print(buf);
    Oldmesage = mesage;
  }
  delay(500);
}

String receive_data(){
  // try to parse packet
  String LoRa_mesage;
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      LoRa_mesage = LoRa.readString();
      Serial.print(LoRa_mesage);
    }
    
    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
  return LoRa_mesage;
}