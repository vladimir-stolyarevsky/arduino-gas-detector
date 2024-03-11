#include <OLED_I2C.h>                           
OLED  myOLED(SDA, SCL);                      
extern uint8_t SmallFont[]; 
extern uint8_t TinyFont[]; 



void SetupOLED(){ 
    myOLED.begin(SSD1306_128X32);
    delay(500);   
 }

 void displayCCS811Data(uint16_t eCO2, uint16_t tvoc, uint16_t baseLine){        
        myOLED.clrScr(); 
        myOLED.setFont(SmallFont);
        myOLED.print("CO2: ", LEFT, 0);
        myOLED.print(String(eCO2)+" ppm", RIGHT, 0); 
        myOLED.print("TVOC: ", LEFT, 9);
        myOLED.print(String(tvoc) + " ppb", RIGHT, 9);  
        myOLED.print("Uptime: " + String(uptimeSec) + " sec",LEFT,25);
        myOLED.setFont(TinyFont);
        myOLED.print("base line: ", LEFT, 18);
        myOLED.print(String(baseLine,HEX), RIGHT, 18);
        myOLED.update();
  } 

void displayAM2320Data(float hum, float temp){ 
        myOLED.clrScr(); 
        myOLED.setFont(SmallFont);
        myOLED.print("Humid.: ", LEFT, 0);
        myOLED.print(String(hum)+" %", RIGHT, 0); 
        myOLED.print("Temp.: ", LEFT, 9);
        myOLED.print(String(temp) + " C", RIGHT, 9);  
        myOLED.print("Uptime: " + String(uptimeSec) + " sec",LEFT,25); 
        myOLED.update();
  } 
void displayMQ9Data(float sensor_volt,float RS_gas, float ratio, float co_ppm, float r0){ 
        myOLED.clrScr(); 
        myOLED.setFont(SmallFont);
        myOLED.print("CO",LEFT,0);
        if(ratio > 2)
          myOLED.print("<100 aprx(" + String((int)co_ppm) + ") ppm",RIGHT,0);  
        else
          myOLED.print(String((int)co_ppm) + " ppm",RIGHT,0);  
        myOLED.print("ratio: ", LEFT, 9);
        myOLED.print(String(ratio), RIGHT, 9);
        myOLED.print("Uptime: " + String(uptimeSec) + " sec",LEFT,25);
        myOLED.setFont(TinyFont);
        myOLED.print("R0: ", LEFT, 18);
        myOLED.print(String(r0), RIGHT, 18);
        
        /*myOLED.print("Sensor: ", LEFT, 0);
        myOLED.print(String(sensor_volt)+" V", RIGHT, 0); 
        myOLED.print("RS_gas: ", LEFT, 9);
        myOLED.print(String(RS_gas), RIGHT, 9);  
        myOLED.print("Uptime: " + String(uptimeSec) + " sec",LEFT,25);
        myOLED.setFont(TinyFont);
        */
        myOLED.update();
}

void displayConnectionError(){
        myOLED.clrScr(); 
        myOLED.print("failed to init chip",CENTER,15); 
        myOLED.update();
}

void displayAlert(String message){
        myOLED.clrScr(); 
        myOLED.setFont(SmallFont);
        myOLED.print("!!ALERT!!",CENTER,9); 
        myOLED.print(message,CENTER,15); 
        myOLED.update();
}

void displayMessage(String message,String message2){
        myOLED.clrScr(); 
        myOLED.setFont(SmallFont);
        myOLED.print(message,CENTER,9);  
        myOLED.print(message2,CENTER,15); 
        myOLED.update();
}
