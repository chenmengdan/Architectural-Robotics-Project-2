#include <Wire.h>//lib for I2C.
#include "rgb_lcd.h"//Lib for the LCD screen.
rgb_lcd lcd;
#include "IRremote.h" //lib for the controller
#define TouchPin 8 //                           [8]
#define ledPin 13 //                            [13]
#define vibrator 9  //                          [9]
int receiver = 11; //signal Pin of IR receiver  [11]
const int lightPIN=2; //                        [2]

IRrecv irrecv(receiver);
decode_results results;

int pressedNumber = 0;
int count = 0;

bool room1 = false;
bool room2 = false;
bool room3 = false;


void setup() {
  Serial.begin(9600);
  pinMode(lightPIN,OUTPUT);
  pinMode(vibrator,OUTPUT);
  pinMode(ledPin,OUTPUT);
  pinMode(TouchPin, INPUT);
  
  Serial.println("IR Receiver Button Decode");
  irrecv.enableIRIn();//Start the receiver
}

void loop() {
  if (irrecv.decode(&results))// have we received an IR signal?
  {
    translateIR();
    irrecv.resume();//receive the next value
  }
  int touchSensorValue = digitalRead(TouchPin);
  Serial.println(pressedNumber);
    if(pressedNumber == 1)
    {
//      while(touchSensorValue == 1){ //check for RFID flag
//        vibrateActivate();
//        if(touchSensorValue != 1){
//          break;
//        }
//      }
      
        if(touchSensorValue == 1)
        {
          vibrateActivate();
        }
        else
        {
          lightAlertActivate();
        }
    }
}

/*_____Functions_________*/
void translateIR() // takes action based on IR code received
{
//  Serial.println(results.value);
    switch(results.value)
    {
         case 16753245: 
        Serial.println("1");
        pressedNumber = 1;
        digitalWrite(lightPIN,HIGH);break;
        break;
      case 16736925: 
        Serial.println("2");
        pressedNumber = 2; 
        digitalWrite(lightPIN,LOW);break;
        break;
      case 16769565: 
        Serial.println("3");
        pressedNumber = 3;
        lcd.begin(16, 2);   
        lcd.setCursor(0, 0);// Print a message to the lcd.
        lcd.print("Child is Calling! ");
        lcd.setRGB(254,0,0);      
        break;
      case 16720605: 
        Serial.println("4");
        pressedNumber = 4; 
        lcd.begin(16, 2);   
        lcd.setCursor(0, 0);// Print a message to the lcd.
        lcd.print("Open the Light ");
        lcd.setRGB(254,150,0);   
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
//      case 16718055: 
//        Serial.println(" FORWARD"); break;
//      case 16716015: 
//        Serial.println(" LEFT");    break;
//      case 16726215: 
//        Serial.println(" -OK-");    break;
//      case 16734885: 
//        Serial.println(" RIGHT");   break;
//      case 16730805: 
//        Serial.println(" REVERSE"); break;
//        Serial.println(" *");    break;
//      case 16756815: 
//        Serial.println(" #");    break;
//      case 0xFFFFFFFF: 
//        Serial.println(" REPEAT"); break;
 
    }// End Case

  delay(500); // Do not get immediate repeat

}

void vibrateActivate(){
   for(count = 0; count < 2; count+=1 ){
    digitalWrite(vibrator,HIGH);
    delay(1000);
    digitalWrite(vibrator,LOW);
    delay(1000);
   }    

}

void lightAlertActivate(){
   for(count = 0; count < 2; count+=1 ){
    digitalWrite(ledPin,HIGH);
    delay(500);
    digitalWrite(ledPin,LOW);
    delay(500); 
   }
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
