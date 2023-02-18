#include <ESP8266WiFi.h>
#include<PubSubClient.h>

const char* ssid     = "OPPO_A31"; //"YOUR_SSID";
const char* password = "0813470881"; //"YOUR_PASSWORD";

const char *mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char *mqttuser = "";
const char *mqttpass = "";

const char *phone = "YOUR_ID_NUMBER";
const char *house = "YOUR_HOUSE_NUMBER";

WiFiClient espclient;
PubSubClient client(mqtt_server, mqtt_port, espclient);

void setup(){
   Serial.begin(115200);
   Serial.println("WiFi Connecting...");

   WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED){
      delay(250);
      Serial.print(".");
   }

   Serial.println("\nWiFi is connected");  
   Serial.print("IP address: ");
   Serial.println(WiFi.localIP());

   client.setServer(mqtt_server, mqtt_port);
   client.setCallback(callback);
   connect_mqtt();
}

void loop(){
    if(!client.connected()){
      connect_mqtt();
    }
    client.loop();
}

void callback(String topic,byte* payload,unsigned int length1){ 
  Serial.print("message arrived ==> [");
  Serial.print(topic);
  Serial.println("] ");
  
  String msg;  
  for(int i=0;i<length1;i++){
   Serial.print((char)payload[i]);
   msg += (char)payload[i];    
  }
  Serial.println(msg);  
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

       client.subscribe(sub_topic);
       Serial.println("Subscribe topic: "+String(sub_topic));
     } else {
       Serial.print("failed with state ");
       Serial.print(client.state());
       delay(3000);
     }
  }
}
