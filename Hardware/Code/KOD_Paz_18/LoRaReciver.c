#include <SPI.h>
#include <LoRa.h>

void setup() {
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
  
  mesage = receive_data();
  send_data_ESP(mesage);
  delay(100);
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

int send_data_ESP(String mesage){
  int sendStatus = 0;
  int messageLength = mesage.length();

  int bytesSent = Serial.write(mesage);
  
  if(bytesSent == messageLength){
    sendStatus = 0;
  }
  else{
    sendStatus = 1;
  }
return sendStatus;
}
