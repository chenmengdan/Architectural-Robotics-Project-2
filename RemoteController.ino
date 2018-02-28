#include <Wire.h>//lib for I2C.
#include "rgb_lcd.h"//Lib for the LCD screen.
rgb_lcd lcd;
#include "IRremote.h" //lib for the controller
int receiver = 11; //signal Pin of IR receiver
const int lightPIN=2;

IRrecv irrecv(receiver);
decode_results results;

void setup() {
//*****************SET UP OF THE REMOTE CONTROLLER MODULE**************//  
  pinMode(lightPIN,OUTPUT);
  Serial.begin(9600);
  Serial.println("IR Receiver Button Decode");
  irrecv.enableIRIn();//Start the receiver
}

void loop() {
  if (irrecv.decode(&results))// have we received an IR signal?
  {
    translateIR();
    irrecv.resume();//receive the next value
  }
}

/*_____Functions_________*/
void translateIR() // takes action based on IR code received
{
//  Serial.println(results.value);
    switch(results.value)
    {
      case 16718055: 
        Serial.println(" FORWARD"); break;
      case 16716015: 
        Serial.println(" LEFT");    break;
      case 16726215: 
        Serial.println(" -OK-");    break;
      case 16734885: 
        Serial.println(" RIGHT");   break;
      case 16730805: 
        Serial.println(" REVERSE"); break;
      case 16753245: 
        Serial.println(" 1");
        digitalWrite(lightPIN,HIGH);break;
      case 16736925: 
        Serial.println(" 2");
        digitalWrite(lightPIN,LOW);break;
      case 16769565: 
        Serial.println(" 3");
        //******************SET UP OF THE LCD SCREEN****************************//
        lcd.begin(16, 2);   
        lcd.setCursor(0, 0);// Print a message to the lcd.
        lcd.print("Child is Calling! ");
        lcd.setRGB(254,0,0);       
        break;
      case 16720605: 
        Serial.println(" 4");
        lcd.begin(16, 2);   
        lcd.setCursor(0, 0);// Print a message to the lcd.
        lcd.print("Open the Light ");
        lcd.setRGB(254,150,0);
        break;
      case 16712445: 
        Serial.println(" 5");    break;
      case 16761405: 
        Serial.println(" 6");    break;
      case 16769055: 
        Serial.println(" 7");    break;
      case 16754775: 
        Serial.println(" 8");    break;
      case 16748655: 
        Serial.println(" 9");    break;
      case 16738455: 
        Serial.println(" *");    break;
      case 16750695: 
        Serial.println(" 0");    break;
      case 16756815: 
        Serial.println(" #");    break;
      case 0xFFFFFFFF: 
        Serial.println(" REPEAT"); break;
      default:
        Serial.println(" other button   ");
 
    }// End Case

  delay(500); // Do not get immediate repeat

}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/