#include<Wire.h>
#include<LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include "SoftwareSerial.h"
#include "IRremote.h"
#include <TimerOne.h>  



volatile int i=0;
volatile boolean zero_cross=0;
int AC_pin = 3;
int dim2 = 0;
int dim = 128;
int pas = 10;
int freqStep = 75;

int buton1 = 12;           
int buton2 = 11;
int buton3 = 10;                
int buton4 = 9;  
int buton5 = 8; 

int aprn_l1 = A0;
int aprn_l2 = A1;
int aprn_so = 4;


SoftwareSerial esp(6,7);
//LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

boolean first = true;
boolean first_ok = true;
boolean first_only = true;
boolean button_prss = false;
//String jsonq = "{\"TIME\":\"10:26 PM\",\"LIGHT1\":\"1\",\"LIGHT2\":\"2\",\"FAN\":\"9\",\"SOCKET\":\"2\"}";

//Store for Update
const char* time_j;
int temp_state = 21;
int light1_state = 1;
int light2_state;
int fan_state;
int socket_state;

//Active line variable
int temp_state_act;
int light1_state_act;
int light2_state_act;
int fan_state_act;
int socket_state_act;

String update_value;
boolean on=false;
int buttonState;

//IR Section
int receiver = 13;
IRrecv irrecv(receiver);
decode_results results;

void setup() {
  pinMode(buton1,INPUT);
  pinMode(buton2,INPUT);
  pinMode(buton3,INPUT);
  pinMode(buton4,INPUT);
  pinMode(buton5,INPUT);
  
  pinMode(aprn_l1,OUTPUT);
  digitalWrite(aprn_l1,LOW);
  pinMode(aprn_l2,OUTPUT);
  pinMode(aprn_so,OUTPUT);
  pinMode(13,INPUT);
  pinMode(AC_pin, OUTPUT); 
  irrecv.enableIRIn();
  esp.begin(9600);
  Serial.begin(9600);
  
  attachInterrupt(0, zero_cross_detect, RISING);
  Timer1.initialize(freqStep);
  Timer1.attachInterrupt(dim_check, freqStep);
   
  Serial.println("Setup Ready");
 

}
void zero_cross_detect() {    
  zero_cross = true;               // set the boolean to true to tell our dimming function that a zero cross has occured
  i=0;
  digitalWrite(AC_pin, LOW);
}

void dim_check() {                   
  if(zero_cross == true) {              
    if(i>=dim) {                     
      digitalWrite(AC_pin, HIGH);  // turn on light       
      i=0;  // reset time step counter                         
      zero_cross=false;    // reset zero cross detection
    } 
    else {
      i++;  // increment time step counter                     
    }                                
  }    
}                                      


void loop() {



if(first){
  

   get_data();
  

 
  

}

digitalWrite(buton1,HIGH); 
buttonState = digitalRead(buton1);
  if (buttonState == LOW) {
  if (socket_state_act == 2) { 
       
    socket_state = 1;
    digitalWrite(aprn_so,HIGH);
   
  }
  else {
    socket_state = 2;
   digitalWrite(aprn_so,LOW);
  }
  update_value = "YES"; 
  button_prss = true;
}
digitalWrite(buton2,HIGH); 
buttonState = digitalRead(buton2);
  if (buttonState == LOW) {
  if (light1_state_act == 2) { 
       
    light1_state = 1;
    digitalWrite(aprn_l1,HIGH);
   
  }
  else {
    light1_state = 2;
   digitalWrite(aprn_l1,LOW);
  }
  update_value = "YES"; 
  button_prss = true;
}
digitalWrite(buton3,HIGH); 
buttonState = digitalRead(buton3);
  if (buttonState == LOW) {
  if (light2_state_act == 2) { 
       
    light2_state = 1;
    digitalWrite(aprn_l2,HIGH);
   
  }
  else {
    light2_state = 2;
   digitalWrite(aprn_l2,LOW);
  }
  update_value = "YES"; 
  button_prss = true;
}
digitalWrite(buton4,HIGH); 
buttonState = digitalRead(buton4);
  if (buttonState == LOW) {
  if (dim<127)  
  {
    dim = dim + pas;
   
    if (dim>127) 
    {
      dim=128;
     // in vechiul sketch era 127
    }
  }
  fan_state = dim;
  update_value = "YES"; 
  button_prss = true;
}
digitalWrite(buton5,HIGH); 
buttonState = digitalRead(buton5);
  if (buttonState == LOW) {
  if (dim>5)  
  {
     dim = dim - pas;
     
  if (dim<0) 
    {
      dim=0;
      // in vechiul sketch era 1
    }
   }
  fan_state = dim;
  update_value = "YES"; 
  button_prss = true;
}



if(first_ok == false){
  write_data();  
}

 if (irrecv.decode(&results)) // have we received an IR signal?
  {
    translateIR(); 
    irrecv.resume(); // receive the next value
  }  



}


//get data function.........
void get_data(){

  if(esp.available() > 0){
    StaticJsonDocument<400> doc;
       String json = esp.readStringUntil("\n");
       Serial.println(json);   
       DeserializationError error = deserializeJson(doc, json);
        
  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  
  JsonObject root = doc.as<JsonObject>();

  if(first_only == true){
  
  light1_state = root["LIGHT1"];
  light2_state = root["LIGHT2"];
  fan_state = root["FAN"];
  socket_state = root["SOCKET"];
  temp_state = root["TEMP"];
  first_only = false;
 }
  
  time_j = root["TIME"];
  light1_state_act = root["LIGHT1"];
  light2_state_act = root["LIGHT2"];
  fan_state_act = root["FAN"];
  socket_state_act = root["SOCKET"];
  temp_state_act = root["TEMP"];

 
  // Print values.
  Serial.println("Arduino Recive: ");
  Serial.println(time_j);
  Serial.println(light1_state);
  Serial.println(light2_state);
  Serial.println(fan_state);
  Serial.println(socket_state);
  Serial.println(temp_state);
  first_ok = false;
  first = false;
  
  
  if(button_prss == false){
  if(socket_state_act == 1){
    digitalWrite(aprn_so,HIGH);
  }
  if(socket_state_act == 2){
    digitalWrite(aprn_so,LOW);
  }

  
  if(light1_state_act == 1){
    digitalWrite(aprn_l1,HIGH);
  }
  if(light1_state_act == 2){
    digitalWrite(aprn_l1,LOW);
  }


  if(light2_state_act == 1){
    digitalWrite(aprn_l2,HIGH);
  }
  if(light2_state_act == 2){
    digitalWrite(aprn_l2,LOW);
  }
  if(fan_state_act == 0){
      dim = fan_state_act;
    
  }
  if(fan_state_act != NULL){
    dim = fan_state_act;
  }
  
  }
  button_prss = false;
  delay(1000);

  //run_apperance();
 // write_data();
    
  }
}

///write function..................
void write_data(){
  
  first_ok = true;
  
  String time_state = String(time_j);
  String li1 = String(light1_state);
  String li2 = String(light2_state);
  String fna = String(fan_state);
  String soc = String(socket_state);
  String tmp = String(temp_state);

  String write_data = "{\"LIGHT1\":\""+li1+"\",\"LIGHT2\":\""+li2+"\",\"SOCKET\":\""+soc+"\",\"FAN\":\""+fna+"\",\"TEMP\":\""+tmp+"\",\"UDATE\":\""+update_value+"\"}\r";
  Serial.println(write_data);
  update_value = "";
  
  esp.println(write_data);
  Serial.println("Sent .. ..");
  first = true;
  return;
  
}


void translateIR() 

{
  switch(results.value)
  {
   
case 1168:  
    
    {
   
    light1_state = 1;
    update_value = "YES";
    button_prss = true;
    digitalWrite(aprn_l1,HIGH);
   
    }
    break;   

case 3216:  
  
    {
   
    light1_state = 2;
    update_value = "YES";
    button_prss = true;
    digitalWrite(aprn_l1,LOW);
    }
    break;

case 144:  
    
    {
   
    light2_state = 1;
    update_value = "YES";
    button_prss = true;
    digitalWrite(aprn_l2,HIGH);
   
    }
    break;   
case 2192:  
    
    {
   
    light2_state = 2;
    update_value = "YES";
    button_prss = true;
    digitalWrite(aprn_l2,LOW);
   
    }
    break;   
    
case 752:  
    
    {
   
    socket_state = 1;
    update_value = "YES";
    button_prss = true;
    digitalWrite(aprn_so,HIGH);
   
    }
    break;   
    
case 2800:  
    
    {
   
    socket_state = 2;
    update_value = "YES";
    button_prss = true;
    digitalWrite(aprn_so,LOW);
   
    }
    break;   
    
case 16:  
    dim=120;
    {
   
     fan_state = dim;
     update_value = "YES";
     button_prss = true;
   
    }
    break;

case 2064:  
    dim=105;
    {
   
     fan_state = dim;
     update_value = "YES";
     button_prss = true;
   
    }
    break;
case 1040:  
    dim=90;
    {
   
     fan_state = dim;
     update_value = "YES";
     button_prss = true;
   
    }
    break;
case 3088:  
    dim=75;
    {
   
     fan_state = dim;
     update_value = "YES";
     button_prss = true;
   
    }
    break;
case 528:  
    dim=60;
    {
   
     fan_state = dim;
     update_value = "YES";
     button_prss = true;
   
    }
    break;
case 2576:  
    dim=45;
    {
   
     fan_state = dim;
     update_value = "YES";
     button_prss = true;
   
    }
    break;
case 1552:  
    dim=30;
    {
   
     fan_state = dim;
     update_value = "YES";
     button_prss = true;
   
    }
    break;
case 3600:  
    dim=15;
    {
   
     fan_state = dim;
     update_value = "YES";
     button_prss = true;
   
    }
    break;
case 272:  
    dim=00;
    {
   
     fan_state = dim;
     update_value = "YES";
     button_prss = true;
   
    }
    break;
case 2320:  
    dim=128;
    {
   
     fan_state = dim;
     update_value = "YES";
     button_prss = true;
   
    }
    break;
case 720:  
    
    {
   
    if (dim<127)  
   {
    dim = dim + pas;
    if (dim>127) 
    {
     dim=128;
    }
    }
    fan_state = dim;
    update_value = "YES";
    button_prss = true;
    }
    break;
case 3280:  
    {
       if (dim>5)  
       {
           dim = dim - pas;
      if (dim<0) 
       {
         dim=0;  // in vechiul sketch era 1
      }
     }
     fan_state = dim;
     update_value = "YES";
     button_prss = true;
   
    }
    break;
    
   
    default: 
    //Serial.println(results.value);
    ;

}
}
