#include <Adafruit_RGBLCDShield.h>
#include <Wire.h>//lib for I2C.
#include "rgb_lcd.h"//Lib for the LCD screen.
#include <SPI.h>
#include <MFRC522.h>
#include "IRremote.h" //lib for the controller
#include <Stepper.h>

#define TouchPin 8 //                           [8]

#define ledPinBed 30
#define ledPinLiv 33 
#define ledPinKit 32 
#define ledPinBath 31 

#define vibrator 9
#define buzzer 5
#define RST_PIN         3          // Configurable, see typical pin layout above
#define SS_PIN          53         // Configurable, see typical pin layout above
#define RST_PIN_2       7          // Configurable, see typical pin layout above
#define SS_PIN_2        49         // Configurable, see typical pin layout above

/* ****************** WINDOW SHADE PART ******************************** */
#define LIGHT_SENSOR A0                 //Grove - Light Sensor is connected to A0 of Arduino
#define STEPS 2038
Stepper stepper(STEPS,38,40,39,41);  
const int thresholdvalue = 170;         //The treshold for which the LED should turn on. Setting it lower will make it go on at more light, higher for more darkness
/* ********************************************************************** */

rgb_lcd lcd;

int receiver = 11; //signal Pin of IR receiver  [11]
const int lightPIN=2; //                        [2]

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
MFRC522 mfrc522_2(SS_PIN_2, RST_PIN_2);  // Create MFRC522 instance

IRrecv irrecv(receiver);
decode_results results;

int pressedNumber = 0;
int count = 0;
bool lightPinIsOn = false; 
bool IS_CHILD_IN_KITCHEN = false;
bool IS_CHILD_IN_BATHROOM = false;
bool IS_PARENT_IN_KITCHEN = false;
bool IS_PARENT_IN_BATHROOM = false;

/* ****************** WINDOW SHADE PART ******************************** */
int numberOfStep = 0;
int mode = 0; // mode 0 -> light sensor // mode 1 -> manual button
int windowState = 1; // window state 0 -> closed // window state 1 -> open
bool toggle = true; // true -> to close // false -> to open 
/* ********************************************************************** */


void setup() {
  Serial.begin(9600);
  pinMode(lightPIN,OUTPUT);
  pinMode(vibrator,OUTPUT);
  pinMode(ledPinBed,OUTPUT);
  pinMode(ledPinLiv,OUTPUT);
  pinMode(ledPinKit,OUTPUT);
  pinMode(ledPinBath,OUTPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(TouchPin, INPUT);

/* ****************** WINDOW SHADE PART ******************************** */
  //attachInterrupt(0,translateIR,CHANGE);
  stepper.setSpeed(15);
/* ********************************************************************** */

  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  mfrc522_2.PCD_Init();
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  Serial.println("IR Receiver Button Decode");
  Serial.println();
  irrecv.enableIRIn();//Start the receiver
}


void loop() {
    lcd.begin(16, 2);  
    lcd.setCursor(0, 0);// Print a message to the lcd.
    lcd.print("CHILD IS IN ");
    lcd.setCursor(0, 1);// Print a message to the lcd.
    lcd.print("THE BEDROOM");
    
    if (irrecv.decode(&results)) {// have we received an IR signal?
      translateIR();
      irrecv.resume();//receive the next value
    }
    checkLocation();

 /* ************************* CALLING PARENTS ********************************* */
    if( pressedNumber == 1 )
    {
        if(IS_PARENT_IN_KITCHEN == true)
        {
            KitchenBuzzerLEDActivate();
            Serial.println("Parent is in the kitchen");
        }
        else if(IS_PARENT_IN_BATHROOM == true)
        {
            BathroomBuzzerLEDActivate();
            lcd.setRGB(255,255,255);
            Serial.println("Parent is in the bathroom");
        }
        else // PARENT IN THE BED ROOM
        {
            if(digitalRead(TouchPin) == 1) 
            {
              Serial.println("In Bed -> Vibrate, buzzer, LED");
              Serial.println();
              vibrateBuzzerLEDActivate(); 
            }
            LivingBuzzerLEDActivate();
            Serial.println("Parent is in the bedroom or living room");
        } 
        pressedNumber = 0;
    }
    
/* ************************** WINDOW SHADE PART  ****************************** */
    else if(pressedNumber == 3 || pressedNumber == 4)
    {
       if( mode == 1)
       { // mode = 1 -> using pushbutton
          if(mode == 1 && toggle == false && windowState == 0)
          { // push to open window
            Serial.println("open!!!!!!!!!!!!!!!!!!");
            open_shade();
          }
          if(mode == 1 && toggle == true && windowState == 1)
          { // push to close window
            Serial.println("close-----------------");
            close_shade();
          }
        }
       else{ // mode = 0 -> using light sensor
          Serial.println(analogRead(LIGHT_SENSOR));
          if(mode == 0 && analogRead(LIGHT_SENSOR) < thresholdvalue && windowState == 0 )// no light -> open window
          { 
             Serial.println("open!!!!!!!!!!!!!!!!!!");
             open_shade();
          }
          if(mode == 0 && analogRead(LIGHT_SENSOR) >= thresholdvalue && windowState == 1)// light -> close window
          { 
             Serial.println("close-----------------");
             close_shade();
          }
       }
    }
/* ****************************************************************************** */



}
/*_____Functions_________*/
void translateIR() {// takes action based on IR code received

  switch(results.value)
  { 
    case 16753245:
      pressedNumber = 1;
      Serial.println("Button_1 - Child Calling!!");
      break;
    case 16736925:
        Serial.println("Button_2 - Light ON/OFF");
        pressedNumber = 2;
        lightPinIsOn = !lightPinIsOn;
        if(lightPinIsOn == true){
          digitalWrite(lightPIN,HIGH);
        }else{
          digitalWrite(lightPIN,LOW);
        }
        break;
    case 16726215:
      Serial.println("Button_OK - Off the calling symbol");
      lcd.clear();
      lcd.setRGB(0,0,0);   
      break;
      case 16769565:
        Serial.println("3");
        pressedNumber = 3;
        mode = 1; // change to manual mode
        toggle = !toggle;
        break;
      case 16720605:
        Serial.println("4");
        pressedNumber = 4;
        mode = 0;
        break;

    }// End Case
  delay(50); // Do not get immediate repeat
}



void checkLocation(){

   /* ******************************CODE FOR  RFID AT KITCHEN STARTS***************************************** */ 
      if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()){
         String content= "";
          Serial.println("This is Kitchen");
          for (byte i = 0; i < mfrc522.uid.size; i++)
        {
           Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
           Serial.print(mfrc522.uid.uidByte[i], HEX);
           content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
           content.concat(String(mfrc522.uid.uidByte[i], HEX));
        }
        Serial.println();
        Serial.print("Message : ");
        content.toUpperCase();
        
          if (content.substring(1) == "65 AF A6 43") //Child
          { 
              Serial.println("Child");
              IS_CHILD_IN_KITCHEN = !IS_CHILD_IN_KITCHEN; // true when a child walk in the kitchen
             
              if (IS_CHILD_IN_KITCHEN == true)
              {
                Serial.println("CHILD IS IN THE KITCHEN NOW");
                lcd.begin(16, 2);  
                lcd.setCursor(0, 0);// Print a message to the lcd.
                lcd.print("CHILD IS IN ");
                lcd.setCursor(0, 1);// Print a message to the lcd.
                lcd.print("THE KITCHEN");
                lcd.setRGB(254,0,0);
              }
              else if (IS_CHILD_IN_KITCHEN == false)
              {
                Serial.println("CHILD IS NOT IN THE KITCHEN NOW");
                lcd.begin(16, 2);  
                lcd.setCursor(0, 0);// Print a message to the lcd.
                lcd.print("CHILD IS NOT IN ");
                lcd.setCursor(0, 1);// Print a message to the lcd.
                lcd.print("THE KITCHEN");
                lcd.setRGB(254,0,0);
              } 
              delay(1000);
          }
       
          else if (content.substring(1) == "D1 96 AF 85") // Parent 
          {
            Serial.println("Parent"); 
            IS_PARENT_IN_KITCHEN = !IS_PARENT_IN_KITCHEN;
            
            if (IS_PARENT_IN_KITCHEN == true)
              {
                Serial.println("PARENT IS IN THE KITCHEN NOW");
                lcd.begin(16, 2);  
                lcd.setCursor(0, 0);// Print a message to the lcd.
                lcd.print("PARENT IS IN THE");
                 lcd.setCursor(0, 1);// Print a message to the lcd.
                lcd.print("KITCHEN NOW");
                lcd.setRGB(254,0,0);
              }
              else if (IS_PARENT_IN_KITCHEN == false)
              {
                Serial.println("PARENT IS NOT IN THE KITCHEN NOW");
                lcd.begin(16, 2);  
                lcd.setCursor(0, 0);// Print a message to the lcd.
                lcd.print("PARENT IS NOT");
                 lcd.setCursor(0, 1);// Print a message to the lcd.
                lcd.print("IN KITCHEN");
                lcd.setRGB(254,0,0);
              } 
            delay(1000);
          }
      }
    /********************************CODE FOR RFID KITCHEN ENDS *******************/ 

        
        
    
   /* ****************************** CODE FOR RFID2 AT BATHROOM BEGINS******************************** */
       if (mfrc522_2.PICC_IsNewCardPresent() && mfrc522_2.PICC_ReadCardSerial()){
        String content2= "";
          Serial.println("This is Bathroom");
          for (byte i = 0; i < mfrc522_2.uid.size; i++)
        {
           Serial.print(mfrc522_2.uid.uidByte[i] < 0x10 ? " 0" : " ");
           Serial.print(mfrc522_2.uid.uidByte[i], HEX);
           content2.concat(String(mfrc522_2.uid.uidByte[i] < 0x10 ? " 0" : " "));
           content2.concat(String(mfrc522_2.uid.uidByte[i], HEX));
        }
        Serial.println();
        Serial.print("Message : ");
        content2.toUpperCase();
        
          if (content2.substring(1) == "65 AF A6 43") //Child
          {
            Serial.println("Child");
            IS_CHILD_IN_BATHROOM = !IS_CHILD_IN_BATHROOM;
            
            if (IS_CHILD_IN_BATHROOM == true)
            {
              Serial.println("CHILD IS IN THE BATHROOM NOW");
              lcd.begin(16, 2);  
              lcd.setCursor(0, 0);// Print a message to the lcd.
              lcd.print("CHILD IS IN THE");
              lcd.setCursor(0, 1);// Print a message to the lcd.
              lcd.print("BATHROOM");
              lcd.setRGB(254,0,0);
            }
            else if (IS_CHILD_IN_BATHROOM == false){
              Serial.println("CHILD IS NOT IN THE BATHROOM NOW");
              lcd.begin(16, 2);  
              lcd.setCursor(0, 0);// Print a message to the lcd.
              lcd.print("CHILD IS NOT IN");
              lcd.setCursor(0, 1);// Print a message to the lcd.
              lcd.print("THE BATHROOM");
              lcd.setRGB(254,0,0);
            }
            delay(1000);
          }
          
          else if (content2.substring(1) == "D1 96 AF 85") //Parent
          {
              Serial.println("Parent");
              IS_PARENT_IN_BATHROOM = !IS_PARENT_IN_BATHROOM;
              if (IS_PARENT_IN_BATHROOM == true)
              {
                Serial.println("PARENT IS IN THE BATHROOM NOW");
                lcd.begin(16, 2);  
                lcd.setCursor(0, 0);// Print a message to the lcd.
                lcd.print("PARENT IS IN THE");
                lcd.setCursor(0, 1);// Print a message to the lcd.
                lcd.print("BATHROOM");
                lcd.setRGB(254,0,0);
              }
              else if (IS_PARENT_IN_BATHROOM == false){
                Serial.println("PARENT IS NOT IN THE BATHROOM NOW");
                lcd.begin(16, 2);  
                lcd.setCursor(0, 0);// Print a message to the lcd.
                lcd.print("PARENT IS NOT IN");
                lcd.setCursor(0, 1);// Print a message to the lcd.
                lcd.print("THE BATHROOM");
                lcd.setRGB(254,0,0);
              }
              delay(1000);
           }    
        }

      /*********************** CODE FOR RFID2 AT BATHROOM ENDS*******************/  
}


void LivingBuzzerLEDActivate() {
  Serial.println("Not in Bed -> buzzer, LED");
  int count;
  digitalWrite(buzzer,HIGH);
  for( count = 0; count < 10; count++){
    digitalWrite(ledPinBed,HIGH);
    digitalWrite(ledPinLiv,HIGH);
    delay(1000);
    digitalWrite(ledPinBed,LOW);
    digitalWrite(ledPinLiv,LOW);
    delay(400);
  }
  digitalWrite(buzzer,LOW);
}

void KitchenBuzzerLEDActivate() {
  Serial.println("Not in Bed -> buzzer, LED");
  int count;
  digitalWrite(buzzer,HIGH);
  for( count = 0; count < 10; count++){
    digitalWrite(ledPinLiv,HIGH);
    digitalWrite(ledPinKit,HIGH);
    delay(1000);
    digitalWrite(ledPinLiv,LOW);
    digitalWrite(ledPinKit,LOW);
    delay(400);
  }
  digitalWrite(buzzer,LOW);
}
void BathroomBuzzerLEDActivate() {
  Serial.println("Not in Bed -> buzzer, LED");
  int count;
  digitalWrite(buzzer,HIGH);
  for( count = 0; count < 10; count++){
    digitalWrite(ledPinLiv,HIGH);
    digitalWrite(ledPinBath,HIGH);
    delay(1000);
    digitalWrite(ledPinLiv,LOW);
    digitalWrite(ledPinBath,LOW);
    delay(400);
  }
  digitalWrite(buzzer,LOW);
}

void vibrateBuzzerLEDActivate() {
  int count;
  digitalWrite(buzzer,HIGH);
  for( count = 0; count < 10; count++){
    digitalWrite(ledPinBed,HIGH);
    digitalWrite(vibrator,HIGH);
    delay(1000);
    digitalWrite(ledPinBed,LOW);
    digitalWrite(vibrator,LOW);
    delay(400);
  }
  digitalWrite(buzzer,LOW);
}

///* ***************** WINDOW SHADE PART ************************************************* */
void open_shade(){
    while(numberOfStep < 3000){
      stepper.step(1);
      numberOfStep++;       
      if( (mode == 1 && toggle == true) || (mode == 0 && analogRead(LIGHT_SENSOR) > thresholdvalue) ){
         break;
      }
    }
    windowState = 1; // 
}

void close_shade(){
    while(numberOfStep > 0){
      stepper.step(-1);
      numberOfStep--;
       if( (mode == 1 && toggle == false) || (mode == 0 && analogRead(LIGHT_SENSOR) < thresholdvalue)){
         break;
      }
    }
    windowState = 0;
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
