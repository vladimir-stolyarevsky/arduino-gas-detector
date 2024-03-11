
#include <EEPROM.h>
float _R0 = 0;
void SetupMQ9(int calibrateForSeconds){ 
  _R0 = readFloatFromEEPROM(3);
   Serial.println(_R0);
  if(calibrateForSeconds)
    getR0(calibrateForSeconds); 
    
}

void getMQ9Data(float* _sensor_volt,
      float* _RS_gas, 
      float* _ratio,
      float* _ppm,
      float* _r0){
      
      //-Replace the name "R0" with the value of R0 in the demo of First Test -/ 
      float R0 = _R0; 
      int sensorValue = analogRead(MQ9_A0); 
      float sensor_volt = 0,RS_gas=0, ratio = 0;

      
      *_sensor_volt= sensor_volt  = ((float)sensorValue / 1024.0) * 5.0; 
      *_RS_gas = RS_gas = (float)((5.0 - sensor_volt) / sensor_volt); // Depend on RL on yor module 
      *_ratio = ratio= RS_gas / R0; // ratio = RS/R0  

      /* 
        Power law
        y=a⋅x^b
      */ 
      float  
        a=624.096,
        b=-2.151;
      *_ppm = a*(float)pow(ratio,b); 
      *_r0 = R0;
} 

void getR0(int calibrateForSeconds){

 float prevR0 = _R0;
 float R0 = 0;
 unsigned long calibrationStartMs = millis();
 while(true){
   float sensor_volt;  
   float RS_air; //  Rs in clean air 
   R0 = 0;  // R0 in 1000 ppm LPG 
   float sensorValue = 0; 
  //Average   
  for(int x = 0 ; x < 100 ; x++) 
   { 
     sensorValue = sensorValue + analogRead(MQ9_A0); 
   } 
   sensorValue = sensorValue/100.0; 
  //-----------------------------------------------/ 
   sensor_volt = (sensorValue/1024)*5.0; 
   RS_air = (5.0-sensor_volt)/sensor_volt; // Depend on RL on yor module 
   R0 = RS_air/9.9; // According to MQ9 datasheet table 
   Serial.print("sensor_volt = "); 
   Serial.print(sensor_volt); 
   Serial.println("V");
   Serial.print("R0 = "); 
   Serial.println(R0);
   Serial.print("Abs diff = ");
   Serial.println(abs(prevR0-R0));
   unsigned long calibratingForMs = millis() - calibrationStartMs; 
   //displayMessage("Calibr. MQ9 " + String(calibrateForSeconds - calibratingForMs/1000),"dR0: " + String(abs(prevR0-R0)));   
   if((abs(prevR0-R0) < 0.001) && (calibratingForMs > (calibrateForSeconds*1000L)))
      break; 
   else
      prevR0 = R0; 
   delay(1000); 
 }

 Serial.print("write to eeprom = "); 
   Serial.println(R0);
   writeFloatToEEPROM(3,R0);
  _R0 = R0;
   Serial.print("mq9 done"); 
}

bool checkForCOAlert(){
  return !digitalRead(MQ9_DO);
}

void writeFloatToEEPROM(int startAddr,float value){
    byte* p = (byte*)(void*)&value;
    for (int i = 0; i < sizeof(value); i++)
        EEPROM.write(startAddr++, *p++);
}

float readFloatFromEEPROM(int startAddr){
    float value = 0.0;
    byte* p = (byte*)(void*)&value;
    for (int i = 0; i < sizeof(value); i++)
        *p++ = EEPROM.read(startAddr++);
    return value;
}
