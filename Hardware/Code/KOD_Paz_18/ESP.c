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
    String LoRa_mesage;

    //When there is something in serial buffer
    while (NodeMCU.available()) {
        char LoRaChar = NodeMCU.read();
        
        if (LoRaChar == '<'){ //if < is the beginning of the received char reset the buffer
            LoRa_mesage = "";
            LoRa_mesage += LoRaChar;
        }

        else if (LoRaChar == '>'){
            stringComplete = true;
            LoRa_mesage += LoRaChar;
        }
                        
        else{
            //Append the message within specified ascii section 
            if ((LoRaChar >= 48 && LoRaChar <= 122) || LoRaChar == ',') 
                LoRa_mesage += LoRaChar;
        }               
    }
    
    //webSocketClient.sendData();

    if(stringComplete == true){
        Serial.println("Received from LoRa: " );  
        delay(2); 
        stringComplete = false;
        Serial.println(LoRa_mesage);
                          
        webSocketClient.sendData(LoRa_mesage); //send received data to websocket server
        LoRa_mesage = "";
    }
       
    //delay(100);
    
}

