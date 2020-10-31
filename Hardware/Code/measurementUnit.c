
// --- Arduino Bad Chimney --- //
// --- Copyright 2020/2021 --- //
// --- Michał Matysiak --- //
// --- All rights reserved ---//

// --- Global variable --- //
    double d_pms[3] = {0.0};
    double d_gps[3] = {0.0};
    double d_air_press[2] = {0.0};
    double d_tem_hum[2] = {0.0};
    int i_mq7[2] = {0.0};

#include <SoftwareSerial.h> 

// --- PMS 70003 --- //
#include "PMS.h"

const int rxPMSpin = 5;
const int txPMSpin = 6;

SoftwareSerial Serial_pms(rxPMSpin, txPMSpin); // RX, TX
 
PMS pms(Serial_pms);
PMS::DATA data;

// --- GPS --- //
#include <TinyGPS++.h>

TinyGPSPlus gps;

// --- MQ 7 --- //
const int AOUTpin = 0;
const int DOUTpin = 8;

// --- Tem/Hum --- //
#include <DHT.h>
#define dhtPin 4
DHT dht;

// --- BMP 180 --- //
#include <Wire.h>
#include <SFE_BMP180.h>

SFE_BMP180 bmp;

// --- LoRa --- //
#include <SPI.h>
#include <LoRa.h>


// --- Setup --- //
void setup() {
    Serial.begin(9600);
    while (!Serial);

    Serial.println("LoRa Sender");

//    if (!LoRa.begin(433E6)) {
//      Serial.println("Starting LoRa failed!");
//      while (1);
//    }
    
    if (bmp.begin())
      Serial.println("BMP180 init success");
      
    Serial.println("DHT setup"); 
    dht.setup(dhtPin);
    
    Serial.println("PMS init"); 
    Serial_pms.begin(9600);  
}

// --- Main program --- //
void loop(){    
    pms_7003();
    if(d_pms[0] != 0.0 || d_pms[1] != 0.0 || d_pms[2] != 0.0){
        gps_position();
        //if(d_gps[0] != 0.0 || d_gps[1] != 0.0 || d_gps[2] != 0.0){
          mq_7();
          temperature_humidity();
          air_pressure(d_gps[2]);
          sendDataToESP();
        //}
    }
    delay(500);
}

// --- PMS --- //
double * pms_7003(){
  
    double pm[3] = {0.0};

    if (pms.read(data)){
        pm[0] = data.PM_AE_UG_1_0;
        pm[1] = data.PM_AE_UG_2_5;
        pm[2] = data.PM_AE_UG_10_0;
        
//        Serial.println(String(pm[0])+ "(ug/m3) " + String(pm[1])+ "(ug/m3) " + String(pm[2])+ "(ug/m3) ");
        
        d_pms[0] = pm[0];
        d_pms[1] = pm[1];
        d_pms[2] = pm[2];
    }
return pm;
}

// --- GPS --- //
double * gps_position(){
    double gps_data[3] = {0.0};// 0 = latitude, 1 = longitude, 2 = adtitude
    
    while(Serial.available() > 0){
      gps.encode(Serial.read());
      if(gps.location.isUpdated()){        
        gps_data[0] = (double) gps.location.lat();
        gps_data[1] = (double) gps.location.lng();
        gps_data[2] = (double) gps.altitude.meters();

//        Serial.println(String(gps_data[0])+ " " + String(gps_data[1]) + " " + String(gps_data[2]));
        
        d_gps[0] = gps_data[0];
        d_gps[1] = gps_data[1];
        d_gps[2] = gps_data[2];
      }
    }
return gps_data; 
} 

// --- MQ7 --- //
int * mq_7(){
    int mq[2] = {0};

    mq[0] = analogRead(AOUTpin);
    mq[1] = digitalRead(DOUTpin);
    
//    Serial.println(String(mq[0])+ "ppm " + " If gas is detect: " +String(mq[1]));
    
    i_mq7[0] = mq[0];
    i_mq7[1] = mq[1];
       
return mq;
}

// --- DTH 22 --- //
double * temperature_humidity(){
    double tem_hum[2] = {0.0}; // 0 = temperature, 1 = humidity

//    int check = dht.getMinimumSamplingPeriod();
    tem_hum[0] = (double)dht.getTemperature();
    tem_hum[1] = (double)dht.getHumidity();
    
//    Serial.println(String(tem_hum[0])+ "*C " + String(tem_hum[1]) + "%");

    d_tem_hum[0] = tem_hum[0];
    d_tem_hum[1] = tem_hum[1];

return tem_hum;
}

// --- BMP 180 --- //
double * air_pressure(double ALTITUDE){
    char status;
    double tem, pre, pre0, alt_bar;
    double pressure[3] = {0.0};   
    
    status = bmp.startTemperature();
    if (status != 0){
      delay(status);
      status = bmp.getTemperature(tem);
      if (status != 0){
        delay(status);
        status = bmp.startPressure(3);
        if (status != 0){
          delay(status);        }
          status = bmp.getPressure(pre, tem);
          if (status != 0){
            delay(status);
            pre0 = bmp.sealevel(pre, ALTITUDE);
            delay(status);
            alt_bar = bmp.altitude(pre, pre0);
          }
      }
    }    
    
    pressure[0] = pre; //hPa
    pressure[1] = pre * 0.014503774; //Bar

    d_air_press[0] = pre0;
    d_air_press[1] = pressure[0];

    Serial.println(String(tem)+ "*C "  + String(pressure[0])+ "hPa " + String(pre0)+ "hPa " + String(alt_bar));

return pressure;
}

// --- LoRa message sender --- //
void sendDataToESP(){
    String message = "";
    
    message = "<";
    
    message += ("lat:" + String(d_gps[0]) + ";");
    message += ("lng:" + String(d_gps[1]) + ";");
    message += ("adt:" + String(d_gps[2]) + ";");

    message += ("tem:" + String(d_tem_hum[0]) + ";");
    message += ("hum:" + String(d_tem_hum[1]) + ";");
    message += ("pre:" + String(d_air_press[0]) + ";");
    
    message += ("mq:" + String(i_mq7[0]) + ";");
    message += ("mq_max:" + String(i_mq7[1]) + ";");
    
    message += ("pm1_0:" + String(d_pms[0]) + ";");
    message += ("pm2_5:" + String(d_pms[1]) + ";");
    message += ("pm10:" + String(d_pms[2]) + ";");

    message += ">";

    Serial.println(message);
    
//    LoRa.beginPacket();
//    LoRa.print(message);
//    LoRa.endPacket();
}