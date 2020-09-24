
// --- Arduino Bad Chimney --- //
// --- Copyright 2020/2021 --- //
// --- Michał Matysiak --- //
// --- All rights reserved ---//

#include <SoftwareSerial.h> 
#include <LiquidCrystal.h> 

// --- GPS --- //
#include <TinyGPS++.h> 

const int rxGPSpin = 2;
const int txGPSpin = 3;

TinyGPSPlus gps;
SoftwareSerial gpsSerial(rxGPSpin, txGPSpin);


// --- LoRa --- //
const int rxLoRapin = 4;
const int txLoRapin = 5;

#include <SPI.h>
#include <LoRa.h>

SoftwareSerial Serial_LoRa(rxLoRapin, txLoRapin);

// --- Tem/Hum --- //
#include <DHT.h>
const int DHTpin = 6;
#define DHTtype DHT11

DHT dht(DHTpin, DHTtype);

// --- MQ 7 --- //
const int AOUTpin = 0;
const int DOUTpin = 8;

// --- PMS 70003 --- //
#include "PMS.h"

const int rxPMSpin = 4;
const int txPMSpin = 5;

SoftwareSerial Serial_pms(rxPMSpin, txPMSpin); // RX, TX
 
PMS pms(Serial_pms);
PMS::DATA data;

// --- BMP 180 --- //
#include <Wire.h>
#include <SFE_BMP180.h>

const int BMPpin = 7;

SFE_BMP180 bmp;


void setup() {
    gpsSerial.begin(9600);

    // --- LoRa --- //
    Serial_LoRa.begin(9600);
    while (!Serial_LoRa);

    Serial_LoRa.println("LoRa Receiver");

    if (!LoRa.begin(915E6)) {
        Serial_LoRa.println("Starting LoRa failed!");
        while (1);
    }

}

void loop() {
    
    double* gps = gps_position();
    double* tem_hum = temperature_humidity();
    double* pressure = air_pressure(*(gps+2));
    int* mq = mq_7();
    double* pm = pms_7003();
    char* data = prepare_data(gps, tem_hum, pressure, pm, mq);
    
//    send_data(data);
}

double * gps_position(){
    double gps_data[] = {0.0, 0.0, 0.0}; // 0 = latitude, 1 = longitude, 2 = adtitude
    
    while (gpsSerial.available() > 0)
        if (gps.encode(gpsSerial.read())){
            gps_data[0] = (double) gps.location.lat();
            gps_data[1] = (double) gps.location.lng();
            gps_data[2] = (double) gps.altitude.meters();
        }

return gps_data; 
} 

double * temperature_humidity(){
    double tem_hum[2] = { 0.0, 0.0}; // 0 = temperature, 1 = humidity

    int check = dht.read(tem_hum);

    tem_hum[0] = (double)dht.readTemperature();
    tem_hum[1] = (double)dht.readHumidity();

return tem_hum;
}

double * air_pressure(double adtitude){
    char status;
    double tem, pre;
    double pressure[3] = {0}; 
        
    // --- Add future for presure by adtitude --- //    
    status = bmp.getTemperature(tem);
    if (status != 0){
        status = bmp.startPressure(3);
        if (status != 0){
            status = bmp.getPressure(pre, tem);
            if (status != 0){
                pressure[0] = pre;
                pressure[1] = pre * 100;
                pressure[2] = pre * 0.014503774;
            }
        }
    }

return pressure;
}

double * pms_7003(){
    double pm[3] = {0.0};

    // --- Using PM lib --- //
    if (pms.read(data)){
        pm[0] = data.PM_AE_UG_1_0;
        pm[1] = data.PM_AE_UG_2_5;
        pm[2] = data.PM_AE_UG_10_0;
    }

return pm;
}

int * mq_7(){
    int mq[2] = {0};
    int valueMQ = 0, limitMQ = 0;

    valueMQ = analogRead(AOUTpin);
    limitMQ = digitalRead(DOUTpin);

    mq[0] = valueMQ;
    mq[1] = limitMQ;

return mq;
}


char * prepare_data(double *gps, double *tem_hum, double *pressure, double *pm, int *mq){
    String gps_str[3], tem_str, hum_str, pre_str, mq_str[2], pm_str[3];
    
    gps_str[0] = String((double) gps[0]);
    gps_str[1] = String((double) gps[1]);
    gps_str[2] = String((double) gps[2]);

    tem_str = String((double) tem_hum[0]);
    hum_str = String((double) tem_hum[1]);

    pre_str = String((double) pressure[1]);

    pm_str[0] = String((double) pm[0]);
    pm_str[1] = String((double) pm[1]);
    pm_str[2] = String((double) pm[2]);

    mq_str[0] = String((int) mq[0]);
    mq_str[1] = String((int) mq[1]);

 
    String data = String("<") + "GPS_lat:" + gps_str[0] + ";lon:" + gps_str[1] + ";Adt:" + gps_str[2] + ";Tem:" + tem_str + ";Hum:" + hum_str + ";Pre:" + pre_str + "PM1.0:" + pm_str[0] + ";PM2.5:" + pm_str[1] ";PM10:" + pm_str[2] + ";Mq:" + mq_str[0] + ";MqLim:" + mq_str[1] + String(">");

    char* buf;
    int len = sizeof(data);
    data.toCharArray(buf, len);    

return  buf;
}

void send_data(char data[]){
    Serial_LoRa.print("Sending packet: ");

    // send packet 
    LoRa.beginPacket();
    LoRa.print(data);
    LoRa.endPacket();

    delay(1000);
}
/*

CREATE TABLE "Drone_readings" (
	`ID`	INTEGER PRIMARY KEY AUTOINCREMENT,
	`Gps_latitude`	INTEGER,
    `Gps_longitude`	INTEGER,
    `Adtitude`  INTEGER,
	`Air_Temperature`	INTEGER,
	`Air_Humidity`	INTEGER,
	`Presure`	INTEGER,
	`MQ7`	INTEGER,
    `MQ7_max`	INTEGER,
    `PM1_0`	INTEGER,
    `PM2_5`	INTEGER,
    `PM10`	INTEGER,
	`Date`	INTEGER UNIQUE
)

*/