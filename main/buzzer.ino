void toggleBuzz(int buzz){
  
  if(buzz){
      tone(BUZZER_PIN, 2000); 
    }else{
      noTone(BUZZER_PIN);
    }
}
