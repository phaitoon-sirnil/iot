//Uplink_Downlink.ino
//Reading dht & sending to mqtt broker

#include <ESP8266WiFi.h>
#include "DHT.h"
#include<PubSubClient.h>

const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

const char *mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char *mqttuser = "";
const char *mqttpass = "";

const char *phone = "YOUR_ID_NUMBER";
const char *house = "YOUR_HOUSE_NUMBER";

WiFiClient espclient;
PubSubClient client(mqtt_server, mqtt_port, espclient);

#define DHTTYPE DHT11   // DHT 11
#define DHTPin D4
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);
unsigned long t_dht;

#define LIGHT D0


// only runs once on boot
void setup() {
  // Initializing serial port for debugging purposes
  Serial.begin(115200);
  pinMode(LIGHT,OUTPUT);
  delay(10);
  

  dht.begin();  

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  connect_mqtt();
  
  t_dht = millis();   
}

// runs over and over again
void loop() {
    if(!client.connected()){
      connect_mqtt();
    }
    client.loop();

    if(millis()-t_dht >= 1000){
      // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
      float humid = dht.readHumidity();
      // Read temperature as Celsius (the default)
      float temp = dht.readTemperature();
    
      // Check if any reads failed and exit early (to try again).
      if (isnan(humid) || isnan(temp)) {
        Serial.println("Failed to read from DHT sensor!");  
      }
      else {     
        Serial.print("Humidity: ");
        Serial.print(humid);
        Serial.print(" %\t Temperature: ");
        Serial.print(temp);
        Serial.println(" *C ");    
  
        String data;
        char str[32];
        char pub_topic[32];
  
        data = String(temp);
        data.toCharArray(str,data.length()+1);
        data = String("/")+String(phone)+String("/")+String(house)+String("/status/temp");
        data.toCharArray(pub_topic,data.length()+1);
        client.publish(pub_topic,str);
            
        data = String(humid);
        data.toCharArray(str,data.length()+1);
        data = String("/")+String(phone)+String("/")+String(house)+String("/status/humid");
        data.toCharArray(pub_topic,data.length()+1);
        client.publish(pub_topic,str);
      }

      t_dht = millis();  
    }
    
    //delay(2000);  
}

void callback(String topic,byte* payload,unsigned int length1){ 
  Serial.println("\nMessage is comming...");
  Serial.println(String("Topic: ")+String(topic));
  
  String msg,data;  
  char pub_topic[32];
  
  for(int i=0;i<length1;i++){
   msg += (char)payload[i]; 
  }
  Serial.println(String("Message: ")+String(msg));  

  if(topic == String("/")+String(phone)+String("/")+String(house)+String("/control/sw/ch1")){
    data = String("/")+String(phone)+String("/")+String(house)+String("/status/sw/ch1");
    data.toCharArray(pub_topic,data.length()+1);
    if(msg == "TURN_ON"){
       digitalWrite(LIGHT,HIGH);       
       client.publish(pub_topic,"SW1_ON");  
       Serial.println(String("publish('")+String(pub_topic)+String("','SW1_ON'")+String(")"));    
    }else if(msg == "TURN_OFF"){
      digitalWrite(LIGHT,LOW);  
       client.publish(pub_topic,"SW1_OFF");  
       Serial.println(String("publish('")+String(pub_topic)+String("','SW1_OFF'")+String(")")); 
    }
  }else if(topic == String("/")+String(phone)+String("/")+String(house)+String("/control/sw/ch2")){
    data = String("/")+String(phone)+String("/")+String(house)+String("/status/sw/ch2");
    data.toCharArray(pub_topic,data.length()+1);
    if(msg == "TURN_ON"){       
       client.publish(pub_topic,"SW2_ON");  
       Serial.println(String("publish('")+String(pub_topic)+String("','SW2_ON'")+String(")"));    
    }else if(msg == "TURN_OFF"){
       client.publish(pub_topic,"SW2_OFF");  
       Serial.println(String("publish('")+String(pub_topic)+String("','SW2_OFF'")+String(")")); 
    }
  }
}


void connect_mqtt(){
   char alias[30];
   strcpy(alias,phone);
   strcat(alias,house);
   
// Generate client name based on MAC address and last 8 bits of microsecond counter
   String clientName;
   clientName += "esp8266-";
   uint8_t mac[6];
   WiFi.macAddress(mac);
     for (int i = 0; i < 6; ++i) {
     clientName += String(mac[i], 16);
     if (i < 5)
       clientName += ':';
   } 
   
   while (!client.connected()) {
     Serial.println("\nMQTT Connecting");
     if (client.connect(clientName.c_str(), mqttuser, mqttpass )) {      
       char sub_topic[32];
       
       Serial.println("MQTT is connected"); 
       Serial.println("Client name: "+String(clientName));
       String data = String("/")+String(phone)+String("/")+String(house)+String("/control/#");
       data.toCharArray(sub_topic,data.length()+1);

       client.subscribe(sub_topic); //configuring subscribe topic
       Serial.println("Subscribe topic: "+String(sub_topic));
     } else {
       Serial.print("failed with state ");
       Serial.print(client.state());
       delay(3000);
     }
  }
}
