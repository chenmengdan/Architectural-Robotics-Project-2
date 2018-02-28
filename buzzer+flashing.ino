

#include "IRremote.h" //lib for the controller


//#define SENSOR_ROOM2 A0
//#define SENSOR_ROOM3 A0
//
//#define FLASH_LIGHT1 A0
//#define FLASH_LIGHT2 A0
//#define FLASH_LIGHT3 A0
//
//#define BUZZER1 A0
//#define BUZZER2 A0
//#define BUZZER3 A0

#define TouchPin 8
#define ledPin 13
#define vibrator 9
//signal Pin of IR receiver
int receiver = 11;
IRrecv irrecv(receiver);
decode_results results;
int pressedNumber = 0;

bool room1 = false;
bool room2 = false;
bool room3 = false;



void setup() {
  Serial.begin(9600); 
  pinMode(vibrator,OUTPUT);
  pinMode(ledPin,OUTPUT);
  pinMode(TouchPin, INPUT);
}

void loop() {
    if (irrecv.decode(&results))// have we received an IR signal?
    {
      translateIR();
      irrecv.resume();//receive the next value
    }
    int touchSensorValue = digitalRead(TouchPin);
    if(touchSensorValue == 1)
    {
        if(pressedNumber == 1){
            vibrateActivate();
        }
    }
}
void vibrateActivate(){
    digitalWrite(vibrator,HIGH);
    delay(1000);
    digitalWrite(vibrator,LOW);
    delay(1000);
}
void lightAlertActivate(){
    digitalWrite(ledPin,HIGH);
    delay(500);
    digitalWrite(ledPin,LOW);
    delay(500); 
}
void translateIR() // takes action based on IR code received
{
//  Serial.println(results.value);
    switch(results.value)
    {
      case 16753245: 
        Serial.println("1");
        pressedNumber = 1;
        break;
      case 16736925: 
        Serial.println("2");
        pressedNumber = 2; 
        break;
      case 16769565: 
        Serial.println("3");
        pressedNumber = 3;    
        break;
      case 16720605: 
        Serial.println("4");
        pressedNumber = 4;    
        break;
      case 16712445: 
        Serial.println("5");
        pressedNumber = 5;    
        break;
      case 16761405: 
        Serial.println("6");
        pressedNumber = 6;    
        break;
      case 16769055: 
        Serial.println("7");
        pressedNumber = 7;    
        break;
      case 16754775: 
        Serial.println("8");
        pressedNumber = 8;    
        break;
      case 16748655: 
        Serial.println("9");
        pressedNumber = 9;    
        break;
      case 16750695: 
        Serial.println("0");   
        break;
       default:
        Serial.println("  other button   ");
    }// End Case

  delay(500); // Do not get immediate repeat

}
