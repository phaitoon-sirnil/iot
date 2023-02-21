//Reading dht & sending to mqtt broker
// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include "DHT.h"
#include<PubSubClient.h>

const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

const char *mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char *mqttuser = "";
const char *mqttpass = "";

const char *phone = "YOUR_PHONE";
const char *house = "YOUR_HOUSE_ID";

WiFiClient espclient;
PubSubClient client(mqtt_server, mqtt_port, espclient);

// Uncomment one of the lines below for whatever DHT sensor type you're using!
#define DHTTYPE DHT11   // DHT 11
#define DHTPin D4
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);


// only runs once on boot
void setup() {
  // Initializing serial port for debugging purposes
  Serial.begin(115200);
  delay(10);

  dht.begin();  

  Serial.println("WiFi is connecting...");
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

// runs over and over again
void loop() {
    if(!client.connected()){
      connect_mqtt();
    }
    client.loop();
  
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

    delay(2000);  
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

       client.subscribe(sub_topic); //configuring subscribe topic
       Serial.println("Subscribe topic: "+String(sub_topic));
     } else {
       Serial.print("failed with state ");
       Serial.print(client.state());
       delay(3000);
     }
  }
}
