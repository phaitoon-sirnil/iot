#include <DHT.h>  //Adafruit DHT22 1.3.4 + Adafuit Unified Sensor 1.0.3
#include <ESP8266WiFi.h>
#include<PubSubClient.h> //by Nick O'Leary Version 2.8.0
#include "Simpletimer.h" // by Natan Lisowski version 2.1.7

const char *phone = "YOUR_ID_NUMBER";
const char *house = "YOUR_HOUSE_NUMBER";

const char *ssid = "YOUR_SSID";//“********”; // cannot be longer than 32 characters!
const char *password = "YOUR_PASSWORD";//“*********”;

const char *mqtt_server = "broker.hivemq.com";//“m14.cloudmqtt.com”;
const int mqtt_port = 1883;//19405;
const char *mqttuser = "";//“******”;
const char *mqttpass = "";//“*********”;

#define FAN D0  // ขา D0 ของบอร์ด NodeMCU เป็นขาที่ต่อกับ LED 
#define VALVE1 D1 //D2  // D0 controls solinoid VALVE
#define VALVE2 D2 // D3 controls light
#define SPRAY D3 //D1
#define DHTTYPE DHT22          //Define sensor type
#define DHTPIN D4              // Define sensor pin
#define SOIL_SENSOR A0

char str[32];
String data="";

DHT dht(DHTPIN, DHTTYPE, 15);   //Initialize DHT sensor 

float humid = 0;
float temp = 0;

WiFiClient espclient;
PubSubClient client(mqtt_server, mqtt_port, espclient);
Simpletimer timer1,timer2;

void callback(String topic,byte* payload,unsigned int length1){ 
  Serial.print("message arrived ==> [");
  Serial.print(topic);
  Serial.print("] ");
  String msgg;
  
  for(int i=0;i<length1;i++){
   Serial.print((char)payload[i]);
   msgg += (char)payload[i]; 
   
  }
  Serial.print("\n");
  char pub_topic[32];
  
  if(topic == String("/")+String(phone)+String("/")+String(house)+String("/control/spray")){
    if(msgg == "ON_SPRAY"){
      data = String("/")+String(phone)+String("/")+String(house)+String("/state/spray");
      data.toCharArray(pub_topic,data.length()+1);
       digitalWrite(SPRAY,HIGH); 
       client.publish(pub_topic,"ON");  
       Serial.print("publish(");Serial.print(pub_topic);Serial.println(",ON)");    
    }else if(msgg == "OFF_SPRAY"){
      data = String("/")+String(phone)+String("/")+String(house)+String("/state/spray");
      data.toCharArray(pub_topic,data.length()+1);
       digitalWrite(SPRAY,LOW); 
       client.publish(pub_topic,"OFF");   
       Serial.print("publish(");Serial.print(pub_topic);Serial.println(",OFF)"); 
    }
  }else if(topic==String("/")+String(phone)+String("/")+String(house)+String("/control/fan")){
    if(msgg=="ON_FAN"){
      data = String("/")+String(phone)+String("/")+String(house)+String("/state/fan");
      data.toCharArray(pub_topic,data.length()+1);
      digitalWrite(FAN,HIGH);
      client.publish(pub_topic,"ON");
      Serial.print("publish(");Serial.print(pub_topic);Serial.println(",ON)"); 
    }else if(msgg="OFF_FAN"){
      data = String("/")+String(phone)+String("/")+String(house)+String("/state/fan");
      data.toCharArray(pub_topic,data.length()+1);
      digitalWrite(FAN,LOW);
      client.publish(pub_topic,"OFF");
      Serial.print("publish(");Serial.print(pub_topic);Serial.println(",OFF)"); 
    }
  }else if(topic==String("/")+String(phone)+String("/")+String(house)+String("/control/valve1")){
    if(msgg=="ON_VALVE1"){
      data = String("/")+String(phone)+String("/")+String(house)+String("/state/valve1");
      data.toCharArray(pub_topic,data.length()+1);
      digitalWrite(VALVE1,HIGH);      
      client.publish(pub_topic,"ON");
      Serial.print("publish(");Serial.print(pub_topic);Serial.println(",ON)"); 
      
    }else if(msgg="OFF_VALVE1"){
      data = String("/")+String(phone)+String("/")+String(house)+String("/state/valve1");
      data.toCharArray(pub_topic,data.length()+1);
      digitalWrite(VALVE1,LOW);
      client.publish(pub_topic,"OFF");
      Serial.print("publish(");Serial.print(pub_topic);Serial.println(",OFF)"); 
    }
  }else if(topic==String("/")+String(phone)+String("/")+String(house)+String("/control/valve2")){
    if(msgg=="ON_VALVE2"){
      data = String("/")+String(phone)+String("/")+String(house)+String("/state/valve2");
      data.toCharArray(pub_topic,data.length()+1);
      digitalWrite(VALVE2,HIGH);
      client.publish(pub_topic,"ON");
      Serial.print("publish(");Serial.print(pub_topic);Serial.println(",ON)"); 
    }else if(msgg="OFF_VALVE2"){
      data = String("/")+String(phone)+String("/")+String(house)+String("/state/valve2");
      data.toCharArray(pub_topic,data.length()+1);
      digitalWrite(VALVE2,LOW);
      client.publish(pub_topic,"OFF");
      Serial.print("publish(");Serial.print(pub_topic);Serial.println(",OFF)"); 
    }
  } 
}

void reconnect(){
   char alias[30];
   strcpy(alias,phone);
   strcat(alias,house);

   while (!client.connected()) {
     Serial.println("Connecting to MQTT…\n");
     if (client.connect(alias, mqttuser, mqttpass )) {      
       char sub_topic[32];
       Serial.println("connected"); 
       data = String("/")+String(phone)+String("/")+String(house)+String("/control/#");
       data.toCharArray(sub_topic,data.length()+1);

       client.subscribe(sub_topic);
       Serial.print("subscribe: ");Serial.println(sub_topic);
     } else {
       Serial.print("failed with state ");
       Serial.print(client.state());
       delay(3000);
     }
  }
}

void readDHT(){
  char pub_topic[32];
  humid = dht.readHumidity(); //อ่านค่าความชื้นจาก DHT
  temp = dht.readTemperature(); //อ่านค่าอุณหภูมิจาก DHT

  if (isnan(humid) || isnan(temp)) { //ตรวจสอบค่าที่อ่านได้ว่าเป็น NaN หรือไม่
     data = String("/")+String(phone)+String("/")+String(phone)+String("/state/error");
     data.toCharArray(pub_topic,data.length()+1);
     Serial.println("Failed to read from DHT sensor!");
     client.publish(pub_topic,"Failed to read from DHT!!!");
     delay(500);
     //return;
  }else{ 
     String data = String(humid)+","+String(temp);
     data.toCharArray(str,data.length()+1);
     data = String("/")+String(phone)+String("/")+String(house)+String("/state/dht");
     data.toCharArray(pub_topic,data.length()+1);
     client.publish(pub_topic,str);
     Serial.print("publish(");Serial.print(pub_topic);Serial.print(",");Serial.print(str);Serial.println(")");
          
     data = String(temp);
     data.toCharArray(str,data.length()+1);
//   client.publish("/buu/iot/diy/state/dht",str); //ส่งข้อมูล ความชื้น และ อุณหภูมิ ไปยัง topic ชื่อ /dht
     data = String("/")+String(phone)+String("/")+String(house)+String("/state/temp");
     data.toCharArray(pub_topic,data.length()+1);
     client.publish(pub_topic,str);
     Serial.print("publish(");Serial.print(pub_topic);Serial.print(",");Serial.print(str);Serial.println(")");
          
     data = String(humid);
     data.toCharArray(str,data.length()+1);
     data = String("/")+String(phone)+String("/")+String(house)+String("/state/humid");
     data.toCharArray(pub_topic,data.length()+1);
     client.publish(pub_topic,str);
     Serial.print("publish(");Serial.print(pub_topic);Serial.print(",");Serial.print(str);Serial.println(")");
  }
}

void readMoisture(){
   char pub_topic[32];
   float moisture_percentage,moisture;
   moisture = analogRead(SOIL_SENSOR);
   //moisture_percentage = (100.00 - (moisture * 100.00));
   moisture_percentage = map(moisture,0,1023,0,100);
   if (isnan(moisture_percentage)) { //ตรวจสอบค่าที่อ่านได้ว่าเป็น NaN หรือไม่
//    Serial.println("Failed to read from Moisture sensor!");
      data = String("/")+String(phone)+String("/")+String(house)+String("/state/error");
      data.toCharArray(pub_topic,data.length()+1);
      client.publish(pub_topic,"Failed to read from SOIL!!!");
      Serial.print("publish(");Serial.print(pub_topic);Serial.print(",");Serial.print("Failed to read from SOIL!!!)");
      delay(500);
   }else{
      Serial.print("Moisture value:= ");Serial.println(100-moisture_percentage);
      String data = String(100-moisture_percentage);
      data.toCharArray(str,data.length()+1);
      data = String("/")+String(phone)+String("/")+String(house)+String("/state/moisture");
      data.toCharArray(pub_topic,data.length()+1);
      client.publish(pub_topic,str);
//    client.publish("/buu/iot/diy/state/soil",str); //ส่งข้อมูล ความชื้น และ อุณหภูมิ ไปยัง topic ชื่อ /dht
      Serial.print("publish(");Serial.print(pub_topic);Serial.print(",");Serial.print(str);Serial.println(")");
    }
}
void setup(){
    dht.begin();
    
    Serial.begin(115200);
    Serial.println("Starting...");
    pinMode(SPRAY, OUTPUT);
    pinMode(FAN, OUTPUT);
    pinMode(VALVE1, OUTPUT);
    pinMode(VALVE2,OUTPUT);
   
    if (WiFi.begin(ssid, password)) {
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
    }
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

 
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
    delay(1000);
    reconnect();

    timer1.register_callback(readDHT);
    timer2.register_callback(readMoisture);

 }

void loop(){
    char pub_topic[32];
    if(!client.connected()){
      reconnect();
    }
    client.loop();

    timer1.run(10000);
    timer2.run(12000);

}
