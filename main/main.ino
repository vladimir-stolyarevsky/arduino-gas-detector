const int BUZZER_PIN = 9; 
const int MQ9_DO = 4;  
const int MQ9_A0 = 0;    
const int BUTTON_PIN = 7;
  
const int DISP_STATE_CCS811 = 1;
//const int DISP_STATE_MQ9 = 2;
const int DISP_STATE_AM2320 = 2;
const int DISP_STATE_ALERT = 0;
const int DISP_STATE_ERROR = -1;

bool GetCCS811SensorData(uint16_t* eCO2,uint16_t* tvoc,uint16_t* baseLine);

#include "Adafruit_AM2320.h"
Adafruit_AM2320 am2320 = Adafruit_AM2320();

void setup(void)
{    
    Serial.begin(115200);  
    delay(500);
    Serial.println("Starting..."); 
    SetupOLED();
    displayMessage("Initializing","Please wait");
    SetupCCS811(false);
    //SetupMQ9(3);

    //PIN MODES              
    pinMode(MQ9_DO, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);
    //Temp/Humid sensor
    am2320.begin();
    Serial.println("Setup done");  
}

 
unsigned long deltaTime = 0,
              uptimeSec = 0,
              prevLoopMillis = 0,
              thisLoopMillis = 0;
int alert = 0;
int displayState = DISP_STATE_CCS811;

uint16_t eCO2 = 0,
         tvoc = 0, 
         baseLine = 0;
         
float sensor_volt = 0,
      RS_gas = 0,
      ratio = 0,
      co_ppm = 0,
      r0 = 0;

float temp = 0,
      hum = 0;

int prevButtonState = HIGH;

void loop() {  

    Serial.println(millis()); 
    thisLoopMillis = millis();
    uptimeSec = thisLoopMillis/1000;
    deltaTime += thisLoopMillis - prevLoopMillis;
    unsigned long deltaTimeSec = deltaTime/1000;

    
    //getMQ9Data(&sensor_volt,&RS_gas,&ratio,&co_ppm,&r0);
   
    if(!getCCS811SensorData(&eCO2,&tvoc,&baseLine))
      displayState = DISP_STATE_ERROR;
    getAM2320Data(&hum,&temp);

  
    checkAlerts(); 
    toggleBuzz(alert);
    
    Serial.println(String(deltaTimeSec) + " sec"); 
    Serial.println(String(alert,BIN) + " alert");  
    Serial.println("Display state: " + String(displayState));
    int buttonState = digitalRead(BUTTON_PIN);
    Serial.println("btn state:");
    Serial.println(buttonState);
    if(prevButtonState == LOW && buttonState == HIGH){      
      if(displayState >= 3 || displayState <= 0)
        displayState = 1;
      else{
        displayState++; 
      }  
    }     
    else if(alert){
      displayState = DISP_STATE_ALERT;
    }
    else{
      displayState = DISP_STATE_CCS811;
      }      
    /*else if(deltaTimeSec < 10)
      displayState = DISP_STATE_CCS811;
    else if(deltaTimeSec<20)
      displayState = DISP_STATE_AM2320;      
    else if(deltaTimeSec<30)      
      displayState = DISP_STATE_MQ9;
    else if(deltaTimeSec>=30){
      deltaTime = 0;       
      displayState = DISP_STATE_CCS811;
    }*/

    displayData();
    //CCS811.writeBaseLine(0x93bf);
    //delay cannot be less than measurement cycle
    delay(1000);
    prevLoopMillis = thisLoopMillis;    
    prevButtonState = buttonState;
}
 
void displayData(){
  switch(displayState){
   case(DISP_STATE_CCS811):
      displayCCS811Data(eCO2,tvoc,baseLine);
      break;
//   case(DISP_STATE_MQ9):
//      displayMQ9Data(sensor_volt,RS_gas,ratio,co_ppm,r0);
//      break;
   case(DISP_STATE_AM2320):
      displayAM2320Data(hum,temp);
      break;
   case(DISP_STATE_ALERT):
      displayAlert(getAlertMessage());
      break;
   case(DISP_STATE_ERROR):
      displayConnectionError();
      break;
  }
}

void getAM2320Data(float* humidity, float* temperature){
  *temperature = am2320.readTemperature();
  *humidity = am2320.readHumidity();
}
   
String getAlertMessage(){
  String message = "";
  if((alert & (1<<0)) > 0)
    message += "|CO|";
  if((alert & (1<<2)) > 0)
    message += "|CO2|";
  if((alert & (1<<1)) > 0)
    message += "|TVOC|";
   
  return message;
}



void checkAlerts(){
  /*if(checkForCOAlert())
    alert |= 1 << 0;
    */
    
  if(tvoc > 1000){
    alert |= 1<<1;
    Serial.println("tvoc alert");
    Serial.println(tvoc);
  }
    
  if(eCO2>3000){
    alert |= 1<<2;
    Serial.println("eCO2 alert");
    Serial.println(eCO2);
  } 
    
  if(alert){ 
    /*if((alert & (1<<0)) > 0 && !checkForCOAlert())
      alert ^= 1<<0;
      */
    if((alert & (1 << 2)) > 0)
      if(eCO2<3000)
        alert ^=1<<2; 
      if(tvoc < 1000)
        alert ^=1<<1;
  } 
}
