// --- Esp8266 Websocket --- //

#include <ESP8266WiFi.h>
#include <WebSocketClient.h>
#include <SoftwareSerial.h>

// --- identifier of this device --- //
char path[] = "/"; 

boolean handshakeFailed = 0;
String dataRecivedFromWeb = "";
String dataToSend = "";

const char *ssid = "BadAiR";
const char *password = "0123456789";
char *host = "169.254.101.121"; 

// --- ESP port for node.js --- //
const int espport = 3000;

WebSocketClient webSocketClient;
unsigned long previousMillis = 0;
unsigned long currentMillis;
unsigned long intervalMesage = 1000; //interval for sending data to the websocket server in ms

// --- WiFiClient class to create TCP connections --- //
WiFiClient client;

// --- ESP --- //
const int  rxESPpin = 3;
const int txESPpin = 2;
SoftwareSerial NodeMCU(rxESPpin, txESPpin); //(rxPin, txPin);
String dataReceivedFromUno;

String receivedString;
String receivedFromUno;
String serialMessage = "";
bool stringComplete = false;

// --- LoRa --- //            
#include <SPI.h>
#include <LoRa.h>

const int  rxLoRapin = 3;
const int txLoRapin = 2;

SoftwareSerial serial_LoRa(rxLoRapin, txLoRapin);

//Method to connect to the websocket server
void web_socket_connect(){

    if (client.connect(host, espport)){
        Serial.println("Connected");
    }
    else{
        Serial.println("Connection failed.");
        delay(1000);

        if (handshakeFailed){
            handshakeFailed = 0;
            ESP.restart();
        }
        handshakeFailed = 1;
    }

    // --- Handshake with the server --- //
    webSocketClient.path = path;
    webSocketClient.host = host;
    if (webSocketClient.handshake(client)){
        Serial.println("Handshake successful");
    }
    else{
        Serial.println("Handshake failed.");
        delay(4000);

        if (handshakeFailed){
            handshakeFailed = 0;
            ESP.restart();
        }
        handshakeFailed = 1;
    }
}

void setup()
{
    // --- WiFi config --- //
    Serial.begin(9600); 
    //pinMode(esp_led, INPUT);

    delay(10);

    //Connecting to a WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(1000);

    web_socket_connect();
    
    // --- ESP config --- //
    pinMode(rxESPpin,INPUT);
    pinMode(txESPpin,OUTPUT);
    NodeMCU.begin(9600);

    // --- LoRa config --- //
    Serial_LoRa.begin(9600);
    while (!Serial_LoRa);

    Serial_LoRa.println("LoRa Receiver");

    if (!LoRa.begin(915E6)) {
        Serial_LoRa.println("Starting LoRa failed!");
        while (1);
    }
}


void loop()
{
    // Możliwe ze do wywalenia
    webSocketClient.getData(dataRecivedFromWeb);
        
    if (dataRecivedFromWeb.length() > 0){
        Serial.print("Data to send to arduino: ");
        Serial.println(dataRecivedFromWeb);
        //Serial.print("Sending data to arduino...\n" );
        dataRecivedFromWeb = dataToSend;
        dataRecivedFromWeb = "";
        NodeMCU.println(dataToSend);
        dataToSend = "";
        //webSocketClient.sendData("data was send to arduino");
    }
       

    //When there is something in serial buffer
    while (NodeMCU.available()) {
        //Serial.println("NodeMCU available !@");
        //char serialChar = NodeMCU.read();  //gets one byte from serial buffer
        char serialChar = receive_data();
        
        if (serialChar == '<'){ //if < is the beginning of the received char reset the buffer
            serialMessage = "";
            serialMessage += serialChar;
        }

        else if (serialChar == '>'){
            stringComplete = true;
            serialMessage += serialChar;
        }
                        
        else{
            //Append the message within specified ascii section 
            if ((serialChar >= 48 && serialChar <= 122) || serialChar == ',') 
                serialMessage += serialChar;
        }               
    }
    
    webSocketClient.sendData("<lat:"+String(random(0,5000))+";lon:"+String(random(0,5000))+";adt:"+String(random(0,100))+";tem:"+String(random(0,34))+";hum:"+String(random(0,100))+";pre:"+String(random(1000,1040))+";mq:"+String(random(0,500))+";mq_max:"+String(random(0,600))+";pm1_0:"+String(random(0,50))+";pm2_5:"+String(random(0,75))+";pm10:"+String(random(0,100))+";>");

    if(stringComplete == true){
        Serial.println("Received from arduino: " );  
        delay(2); 
        stringComplete = false;
        Serial.println(serialMessage);
                          
        webSocketClient.sendData(serialMessage); //send received data to websocket server
        serialMessage = "";
    }
       
    //delay(100);
    
}

/*

char* receive_data(){
    
    char LoRa_mesage;
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        char mesage[packetSize + 10];
        // received a packet
        Serial_LoRa.print("Received packet ");
        // read packet
        while (LoRa.available()) {
            mesage = (char)LoRa.read();
            Serial_LoRa.print(mesage);
        }
        // print RSSI of packet
        Serial_LoRa.print("' with RSSI ");
        Serial_LoRa.println(LoRa.packetRssi());
        LoRa_mesage = mesage;
    }
return LoRa_mesage;
}

*/