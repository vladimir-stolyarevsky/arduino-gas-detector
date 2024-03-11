#include <EEPROM.h>
#include "DFRobot_CCS811.h"
//DFRobot_CCS811 CCS811(&Wire, /*IIC_ADDRESS=*/0x5A);
DFRobot_CCS811 CCS811;

uint16_t recordedBaseLine = 0;
uint16_t currentBaseLine = 0;
bool useRecordedBaseLine;


void SetupCCS811(bool _useRecordedBaseLine){
    Serial.println("Starting CCS811..."); 
    useRecordedBaseLine = _useRecordedBaseLine;
      /*Wait for the chip to be initialized completely, and then exit*/ 

    if(useRecordedBaseLine){
      byte low,high;
      EEPROM.get(0, low);
      EEPROM.get(1, high);
      uint16_t loadedBaseLine = word(high,low);
      if(loadedBaseLine>0)
        recordedBaseLine = loadedBaseLine;
      else
        useRecordedBaseLine = false;
    }
      
    int code = CCS811.begin();
    while(code != 0){ 
        Serial.println(code);
        delay(1000);
        code = CCS811.begin();
    }
}
   
bool getCCS811SensorData(uint16_t* eCO2,uint16_t* tvoc,uint16_t* baseLine){ 
  if(CCS811.checkDataReady() == true){
      *eCO2 = CCS811.getCO2PPM();
      *tvoc = CCS811.getTVOCPPB();
      *baseLine = currentBaseLine = CCS811.readBaseLine();
      if(!useRecordedBaseLine&&uptimeSec>3000 && !recordedBaseLine){
        recordedBaseLine = currentBaseLine;
        saveBaseLine();
      }
          
  } 
  else {
      *eCO2 = 0;
      *tvoc = 0;
      *baseLine = -1; 
      return false;
  }
  if(useRecordedBaseLine && recordedBaseLine)
    CCS811.writeBaseLine(recordedBaseLine);
  return true;
}

void saveBaseLine(){
  EEPROM.write(0, lowByte(currentBaseLine)); 
  EEPROM.write(1, highByte(currentBaseLine));  
}
 
