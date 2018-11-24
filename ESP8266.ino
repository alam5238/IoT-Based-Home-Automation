#include <ESP8266HTTPClient.h>
#include<ESP8266WiFi.h>
#include <ArduinoJson.h>

const char* ssid = "ESPHOST";
const char* password = "87654321";

String server = "http://artisenitbd.ml/home_automation/";
String homeID = "dhaka";
String roomID = "bedroom";
String read_data = "read.php?homeID="+homeID+"&roomID="+roomID;
String read_url = String(server)+String(read_data);

char a;
String recive_data;
boolean priod;
void setup () {
  pinMode(2, OUTPUT); 
 
  Serial.begin(9600);
  
  WiFi.begin(ssid, password);
  delay(8000);
  while (WiFi.status() != WL_CONNECTED) {
 
    delay(1000);
    digitalWrite(2, HIGH);
    
  }

  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
 
    HTTPClient http;  //Declare an object of class HTTPClient
 
    http.begin(read_url);  //Specify request destination
    int httpCode = http.GET();  //Send the request
 
    if (httpCode > 0) { //Check the returning code
 
      String payload = http.getString();   //Get the request response payload
      recive_data = payload;                     //Print the response payload
 
    }
 
    http.end();   //Close connection
 
  }
  priod = false;
  delay(5000); 
}
 
void loop() {


  if(priod == true){
    if(Serial.available() > 0){
        
       // Serial.println(Serial.readString()+"BACK DATA");
        
        StaticJsonDocument<400> doc;
        
        String json = Serial.readString();
        //Serial.println(json);
        
        DeserializationError error = deserializeJson(doc, json);
      
        // Test if parsing succeeds.
        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.c_str());
          return;
        }
      
        JsonObject root = doc.as<JsonObject>();
      
        
        int light1_state = root["LIGHT1"];
        int light2_state = root["LIGHT2"];
        int fan_state = root["FAN"];
        int socket_state = root["SOCKET"];
        int temp_state = root["TEMP"];
        const char* update_p = root["UDATE"];
        String update_state = String(update_p);
        delay(1000);

        if(update_state == "YES"){

                    
String write_url = "http://www.artisenitbd.ml/home_automation/write_read.php?homeID="+String(homeID)+"&roomID="+String(roomID)+"&light1="+String(light1_state)+"&light2="+String(light2_state)+"&fan="+String(fan_state)+"&socket="+String(socket_state)+"&temp="+String(temp_state);

       
        if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
 
          HTTPClient http;  //Declare an object of class HTTPClient
       
          http.begin(write_url);  //Specify request destination
          int httpCode = http.GET(); //Send the request
       
          if (httpCode > 0) { //Check the returning code
       
            String payload = http.getString();   //Get the request response payload
            recive_data = payload; 
            Serial.println(recive_data+"BACK UPDATE");      //Print the response payload
       
          }
       
          http.end();   //Close connection
       
        }else{
          
                WiFi.begin(ssid, password);
                Serial.println(json+"BACK UF");
        
        
        }
        delay(300);
        }else{
        
            if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
 
              HTTPClient http;  //Declare an object of class HTTPClient
           
              http.begin(read_url);  //Specify request destination
              int httpCode = http.GET(); //Send the request
           
              if (httpCode > 0) { //Check the returning code
           
                String payload = http.getString();   //Get the request response payload
                recive_data = payload;                     //Print the response payload
                 Serial.println(recive_data+"BACK DATA");
              }
           
              http.end();   //Close connection
           
            }else{
            
                    WiFi.begin(ssid, password);
                    Serial.println(json+"BACK DF");          
            }
           delay(3000);
        }
        
 
    }//end serial..
  
  }

  
    if(priod == false){
    if(recive_data != NULL){
    Serial.println(recive_data);
    digitalWrite(2, LOW);
    priod = true;
    }
    }
  
 
}




void light(){

          a = Serial.read();
        if(a == '1'){

           digitalWrite(2, HIGH);
           Serial.println(a);
        
        }
        if(a == '2'){

           digitalWrite(2, LOW);
           Serial.println(a);
        }
  
  }















//------------------------------------------------------------------------------------------------------------------
