#include <ESP8266WiFi.h>
const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

void setup(){
   Serial.begin(115200);
   Serial.println("WiFi Connecting...");

   WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED){
      delay(250);
      Serial.print(".");
   }

   Serial.println("WiFi connected");  
   Serial.println("IP address: ");
   Serial.println(WiFi.localIP());
}

void loop(){

}
