// Programa a cargar en ESP32
#include "WiFi.h"
#include "WiFiClient.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// Conexión Wi-Fi y comunicación MQTT
const char* ssid = "sensoresurjc";
const char* password = "Goox0sie_WZCGGh25680000";

#define SERVER "193.147.53.2"
#define SERVERPORT 21883
bool END_LAP = false;

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, SERVER, SERVERPORT);
Adafruit_MQTT_Publish test = Adafruit_MQTT_Publish(&mqtt, "/SETR/2022/7/");

// Define specific pins for Serial2.
#define RXD2 33
#define TXD2 4

// Conectarse al Wi-Fi
void conectWandMQ(){

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  Serial.println("Connecting to WiFi ...");

  while(WiFi.status() != WL_CONNECTED){
    Serial.println('.');
    delay(1000);
  }

  Serial.println("CONNECTED");
  int8_t ret;
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;

  while ((ret = mqtt.connect()) != 0) {
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);
       retries--;
       if (retries == 0) {
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

// Conectar al Wi-Fi y al ESP32
void setup(){

  Serial.begin(9600);

  // Puerto serie que comunica con Arduino
  Serial2.begin(9600,SERIAL_8N1, RXD2, TXD2);

}

String sendBuff;
String sendBuff2;

// Mensajes a enviar
void loop() {

  if (Serial2.available()) {
    
    char c = Serial2.read();
    sendBuff += c;
    
    if (c == '}'){    

      if (WiFi.status() != WL_CONNECTED) {
        if (!mqtt.connected()) {
          conectWandMQ();
        }
      }        

      //Serial.println(sendBuff);
      
      // Mensaje de comeinzo de vuelta
      if(sendBuff == "{START_LAP}"){
        Serial.println("START");
        String payload = "{\"team_name\": \"ATLAS\", \"id\": \"7\", \"action\": \"START_LAP\"}";
        char str[payload.length()+1];
        payload.toCharArray(str, (payload.length()+1));

        bool publ = test.publish(str);
        while(!publ) {
          publ = test.publish(str);
        }

      // Mensaje de línea perdida
      } else if (sendBuff == "{LINE_LOST}"){
        Serial.println("LOST");
        String payload = "{\"team_name\": \"ATLAS\", \"id\": \"7\", \"action\": \"LINE_LOST\"}";
        char str[payload.length()+1];
        payload.toCharArray(str, (payload.length()+1));

        bool publ = test.publish(str);
        while(!publ) {
          publ = test.publish(str);
        }

      // Mensaje de obstáculo detectado
      } else if (sendBuff == "{OBSTACLE_DETECTED}"){
        Serial.println("OBSTACLE");
        String payload = "{\"team_name\": \"ATLAS\", \"id\": \"7\", \"action\": \"OBSTACLE_DETECTED\"}";
        char str[payload.length()+1];
        payload.toCharArray(str, (payload.length()+1));
        
        bool publ = test.publish(str);
        while(!publ) {
          publ = test.publish(str);
        }

      // Mensaje de PING (se envía cada 4 segundos)
      } else if(sendBuff == "{PING}"){
        char c2 = Serial2.read();
        while(c2 != '*') {
          sendBuff2 += c2;
          c2 = Serial2.read();
        }
        //Serial.println("PING");
        //Serial.println(sendBuff2);
        String payload = "{\"team_name\": \"ATLAS\", \"id\": \"7\", \"action\": \"PING\", \"time\": " + sendBuff2 + "}";
        char str[payload.length()+1];
        payload.toCharArray(str, (payload.length()+1));

        bool publ = test.publish(str);
        while(!publ) {
          publ = test.publish(str);
        }

      // Mensaje de final de vuelta
      } else if(sendBuff == "{END_LAP}"){
        char c2 = Serial2.read();
        while(c2 != '*') {
          sendBuff2 += c2;
          c2 = Serial2.read();
        }
        Serial.println("END");
        String payload = "{\"team_name\": \"ATLAS\", \"id\": \"7\", \"action\": \"END_LAP\", \"time\": " + sendBuff2 + "}";
        char str[payload.length()+1];
        payload.toCharArray(str, (payload.length()+1));
        bool publ = test.publish(str);
        while(!publ) {
          publ = test.publish(str);
        }
        mqtt.disconnect();
      } else if (sendBuff == "{CONNECT}"){
        
        conectWandMQ();

        if (mqtt.connected()) {
          Serial2.print("{CONNECTED}");
          Serial.println("{CONNECTED}");
        }

      }

      sendBuff = "";
      sendBuff2 = "";
    } 
  }

  

}