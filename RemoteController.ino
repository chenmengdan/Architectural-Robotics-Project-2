#include <Wire.h>//lib for I2C.
#include "rgb_lcd.h"//Lib for the LCD screen.
#include <SPI.h>
#include <MFRC522.h>
rgb_lcd lcd;
#include "IRremote.h" //lib for the controller
#define TouchPin 8 //                           [8]
#define ledPin 13 //                            [13]
#define vibrator 9  //                          [9]
#define buzzer 5

int receiver = 11; //signal Pin of IR receiver  [11]
const int lightPIN=2; //                        [2]

#define SS_PIN 10
#define RST_PIN 12
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

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
  pinMode(buzzer,OUTPUT);
  pinMode(TouchPin, INPUT);
  
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  
  Serial.println("IR Receiver Button Decode");
  irrecv.enableIRIn();//Start the receiver
}

void loop() {
//   // Look for new cards
//  if ( ! mfrc522.PICC_IsNewCardPresent())
//  {
//    return;
//  }
//  // Select one of the cards
//  if ( ! mfrc522.PICC_ReadCardSerial())
//  {
//    return;
//  }
//  //Show UID on serial monitor
//  Serial.print("UID tag :");
//  String content= "";
//  byte letter;
//  for (byte i = 0; i < mfrc522.uid.size; i++)
//  {
//     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
//     Serial.print(mfrc522.uid.uidByte[i], HEX);
//     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
//     content.concat(String(mfrc522.uid.uidByte[i], HEX));
//  }
//  Serial.println();
//  Serial.print("Message : ");
//  content.toUpperCase();
//  if (content.substring(1) == "06 D4 37 07") //Child
//  {
//    Serial.println("Child");
//    Serial.println();
//    delay(1000);
//  }
// 
//  else if (content.substring(1) == "D1 96 AF 85") {
//    Serial.println("Parent");
//    delay(1000);
//  }
// 
//  else   {
//  Serial.println("Someone else");
//  delay(1000);
//  }
//  
  //***********************************************//
  if (irrecv.decode(&results))// have we received an IR signal?
  {
    translateIR();
    irrecv.resume();//receive the next value
  }
    // Child calling for help
    if(pressedNumber == 1)
    {
      //Check if parents are in bed
      while(digitalRead(TouchPin)){
        digitalWrite(vibrator,HIGH);
      }
      digitalWrite(vibrator,LOW);

     
      /*
       * if parents are not in bed
       * 
      if( in kitchen )
      {
        digitalWrite(buzzer,HIGH);
        activate light 1
      }
      else if( in living room )
      {
        digitalWrite(buzzer,HIGH);
        activate light 2
      }
      else if( in bathroom )
      {
        digitalWrite(buzzer,HIGH);
        activate light 3 
      }
      else if( in bedroom )
      {
        digitalWrite(buzzer,HIGH);
        activate light 4
      }
      */
      //lightAlertActivate();
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
        lcd.begin(16, 2);   
        lcd.setCursor(0, 0);// Print a message to the lcd.
        lcd.print("Child is Calling! ");
        lcd.setRGB(254,0,0);   
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

  delay(50); // Do not get immediate repeat

}

void vibrateActivate(){
    digitalWrite(vibrator,HIGH);
    digitalWrite(ledPin,HIGH);
    delay(500);
    digitalWrite(vibrator,LOW);
    digitalWrite(ledPin,LOW);
    delay(500); 
}

void lightAlertActivate(){
    digitalWrite(ledPin,HIGH);
    delay(500);
    digitalWrite(ledPin,LOW);
    delay(500); 
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
