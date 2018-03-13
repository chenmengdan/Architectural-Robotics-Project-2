#include <Adafruit_RGBLCDShield.h>
#include <Wire.h>//lib for I2C.
#include "rgb_lcd.h"//Lib for the LCD screen.
#include <SPI.h>
#include <MFRC522.h>
#include "IRremote.h" //lib for the controller
#include <Stepper.h>

/* ****************** WINDOW SHADE PART ******************************** */
Stepper stepper(STEPS,15,16,17,18);  
const int thresholdvalue = 120;         //The treshold for which the LED should turn on. Setting it lower will make it go on at more light, higher for more darkness

#define LIGHT_SENSOR A0                 //Grove - Light Sensor is connected to A0 of Arduino
#define STEPS 2038
/* ********************************************************************** */

#define TouchPin 8 //                           [8]
#define ledPin 13 //                            [13]
#define vibrator 9  //                          [9]
#define buzzer 5
#define RST_PIN         3          // Configurable, see typical pin layout above
#define SS_PIN          53         // Configurable, see typical pin layout above
#define RST_PIN_2         7          // Configurable, see typical pin layout above
#define SS_PIN_2          49         // Configurable, see typical pin layout above
rgb_lcd lcd;

int receiver = 11; //signal Pin of IR receiver  [11]
const int lightPIN=2; //                        [2]

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
MFRC522 mfrc522_2(SS_PIN_2, RST_PIN_2);  // Create MFRC522 instance

IRrecv irrecv(receiver);
decode_results results;


int pressedNumber = 0;
int count = 0;



bool IS_CHILD_IN_KITCHEN = false;
bool IS_CHILD_IN_BATHROOM = false;

bool IS_PARENT_IN_KITCHEN = false;
bool IS_PARENT_IN_BATHROOM = false;


/* ****************** WINDOW SHADE PART ******************************** */
int numberOfStep = 0;
volatile int mode = 0; // mode 0 -> light sensor // mode 1 -> manual button
volatile int windowState = 1; // window state 0 -> closed // window state 1 -> open
volatile bool toggle = true; // true -> to close // false -> to open 
/* ********************************************************************** */

void setup() {
  Serial.begin(9600);
  pinMode(lightPIN,OUTPUT);
  pinMode(vibrator,OUTPUT);
  pinMode(ledPin,OUTPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(TouchPin, INPUT);

/* ****************** WINDOW SHADE PART ******************************** */
//  pinMode(LED_PIN,OUTPUT);            //Set the LED on Digital 12 as an OUTPUT
//  pinMode(MANUAL_PIN, INPUT);  // initialize the pushbutton pin as an input
//  pinMode(AUTO_MODE_PIN, INPUT); // initialize the pushbutton pin as an input
//  attachInterrupt(digitalPinToInterrupt(receiver), manualButtonController, RISING);
//  attachInterrupt(digitalPinToInterrupt(receiver), lightSensorMode, RISING);
  attachInterrupt(0,translateIR,CHANGE);
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
            buzzerLEDActivate();
        }
        else if(IS_PARENT_IN_BATHROOM == true)
        {
            buzzerLEDActivate();
            lcd.setRGB(255,255,255);
        }
        else // PARENT IN THE BED ROOM
        {
            if(digitalRead(TouchPin) == 1) 
            {
              Serial.println("In Bed -> Vibrate, buzzer, LED");
              Serial.println();
              vibrateBuzzerLEDActivate(); 
            }
        } 
        pressedNumber = 0;
    }



      
      
    }
    

  
/* ************************** WINDOW SHADE PART  ****************************** */
//    buttonValueOfMannaul = digitalRead(MANUAL_PIN);
//    buttonValueOfAuto = digitalRead(auto_button);
    Serial.print("Mode: ");
    Serial.print(mode);  
    Serial.println();
    Serial.println(analogRead(LIGHT_SENSOR));
    /* Mannaul Button is pressed, meaaning toggle between open and close */
   
    if(mode == 1){ // mode = 1 -> using pushbutton
      if(mode == 1 && toggle == false && windowState == 0){ // push to open window
      Serial.println("open!!!!!!!!!!!!!!!!!!");
      open_shade();
    }
      if(mode == 1 && toggle == true && windowState == 1){ // push to close window
        Serial.println("close-----------------");
        close_shade();
      }
    }
    else{ // mode = 0 -> using light sensor
      if(mode == 0 && analogRead(LIGHT_SENSOR) < thresholdvalue && windowState == 0 ){ // no light -> open window
      Serial.println("open!!!!!!!!!!!!!!!!!!");
      open_shade();
      }
      if(mode == 0 && analogRead(LIGHT_SENSOR) >= thresholdvalue && windowState == 1){ // light -> close window
      Serial.println("close-----------------");
      close_shade();
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
      lcd.begin(16, 2);  
      lcd.setCursor(0, 0);// Print a message to the lcd.
      lcd.print("Child is Calling! ");
      lcd.setRGB(254,0,0);
      break;
//    case 16736925:
//        Serial.println("Button_2 - Light ON/OFF");
//        pressedNumber = 2;
//        digitalWrite(lightPIN,LOW);
//        break;
    case 16726215:
      Serial.println("Button_OK - Off the calling symbol");
      lcd.clear();
      lcd.setRGB(0,0,0);   
      break;
      case 16769565:
        Serial.println("3");
        pressedNumber = 3;
        lcd.begin(16, 2);  
        lcd.setCursor(1, 0);// Print a message to the lcd.
        lcd.print("Manual Mode Window");
        lcd.setRGB(66,244,104);
        manualButtonController();
        break;
      case 16720605:
        Serial.println("4");
        pressedNumber = 4;
        lcd.begin(16, 2);  
        lcd.setCursor(0, 0);// Print a message to the lcd.
        lcd.print("Auto Mode Window");
        lcd.setRGB(176,66,244);
        lightSensorMode();
        break;
//      case 16712445:
//        Serial.println("5");
//        pressedNumber = 5;    
//        break;
//      case 16761405:
//        Serial.println("6");
//        pressedNumber = 6;   
//        break;
//      case 16769055:
//        Serial.println("7");
//        pressedNumber = 7;   
//        break;
//      case 16754775:
//        Serial.println("8");
//        pressedNumber = 8;   
//        break;
//      case 16748655:
//        Serial.println("9");
//        pressedNumber = 9;   
//        break;
//      case 16750695:
//        Serial.println("0");  
//        break;
//       default:
//        Serial.println("  other button   ");
//      case 16718055:
//        Serial.println(" FORWARD"); break;
//      case 16716015:
//        Serial.println(" LEFT");    break;

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

void checkLocation(){
   /* **************************RFID 1 ***************************** */
        Serial.print("UID tag :");
        String content= "";
        byte letter;
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

   /* ****************************** RFID AT KITCHEN ***************************************** */ 
      if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()){
    
          if (content.substring(1) == "06 D4 37 07") //Child
          { 
              Serial.println("Child");
              IS_CHILD_IN_KITCHEN = !IS_CHILD_IN_KITCHEN;
             
              if (IS_CHILD_IN_KITCHEN == true)
              {
                Serial.println("CHILD IS IN THE KITCHEN NOW");
                lcd.begin(16, 2);  
                lcd.setCursor(0, 0);// Print a message to the lcd.
                lcd.print("CHILD IS IN THE KITCHEN NOW!!");
                lcd.setRGB(254,0,0);
              }
              else if (IS_CHILD_IN_KITCHEN == false)
              {
                Serial.println("CHILD IS NOT IN THE KITCHEN NOW");
                lcd.begin(16, 2);  
                lcd.setCursor(0, 0);// Print a message to the lcd.
                lcd.print("CHILD IS NOT IN THE KITCHEN NOW!!");
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
                lcd.print("PARENT IS IN THE KITCHEN NOW!!");
                lcd.setRGB(254,0,0);
              }
              else if (IS_PARENT_IN_KITCHEN == false)
              {
                Serial.println("PARENT IS NOT IN THE KITCHEN NOW");
                lcd.begin(16, 2);  
                lcd.setCursor(0, 0);// Print a message to the lcd.
                lcd.print("PARENT IS NOT IN THE KITCHEN NOW!!");
                lcd.setRGB(254,0,0);
              } 
            delay(1000);
          }
      }
      
    /* ***************************** RFID 2 ************************************* */
        Serial.print("UID tag :");
        String content2= "";
        byte letter2;
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
    
   /* ****************************** RFID AT BATHROOM ******************************** */
       if (mfrc522_2.PICC_IsNewCardPresent() && mfrc522_2.PICC_ReadCardSerial()){
        
         
          if (content2.substring(1) == "06 D4 37 07") //Child
          {
            Serial.println("Child");
            IS_CHILD_IN_BATHROOM = !IS_CHILD_IN_BATHROOM;
            
            if (IS_CHILD_IN_BATHROOM == true)
            {
              Serial.println("CHILD IS IN THE BATHROOM NOW");
              lcd.begin(16, 2);  
              lcd.setCursor(0, 0);// Print a message to the lcd.
              lcd.print("CHILD IS IN THE BATHROOM NOW!!!");
              lcd.setRGB(254,0,0);
            }
            else if (IS_CHILD_IN_BATHROOM == false){
              Serial.println("CHILD IS NOT IN THE BATHROOM NOW");
              lcd.begin(16, 2);  
              lcd.setCursor(0, 0);// Print a message to the lcd.
              lcd.print("CHILD IS NOT IN THE BATHROOM NOW!!!");
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
                Serial.println("CHILD IS IN THE BATHROOM NOW");
                lcd.begin(16, 2);  
                lcd.setCursor(0, 0);// Print a message to the lcd.
                lcd.print("CHILD IS IN THE BATHROOM NOW!!!");
                lcd.setRGB(254,0,0);
              }
              else if (IS_PARENT_IN_BATHROOM == false){
                Serial.println("PARENT IS NOT IN THE BATHROOM NOW");
                lcd.begin(16, 2);  
                lcd.setCursor(0, 0);// Print a message to the lcd.
                lcd.print("PARENT IS NOT IN THE BATHROOM NOW!!!");
                lcd.setRGB(254,0,0);
              }
              delay(1000);
           }    
        }
}


void buzzerLEDActivate() {
  Serial.println("Not in Bed -> buzzer, LED");
  digitalWrite(buzzer,HIGH);
  digitalWrite(ledPin,HIGH);
  delay(1000);
  digitalWrite(buzzer,LOW);
  digitalWrite(ledPin,LOW);
  delay(1000);
}

void vibrateBuzzerLEDActivate() {
  digitalWrite(ledPin,HIGH);
  digitalWrite(vibrator,HIGH);
  digitalWrite(buzzer,HIGH);
  delay(1000);
  digitalWrite(ledPin,LOW);
  digitalWrite(vibrator,LOW);
  digitalWrite(buzzer,LOW);
  delay(1000);
}

/* ***************** WINDOW SHADE PART ************************************************* */
void lightSensorMode(){
    mode = 0;
}
void manualButtonController(){
    mode = 1; // change to manual mode
    toggle = !toggle;
}
void open_shade(){
    digitalWrite(LED_PIN,HIGH);
    while(numberOfStep < 3000){
        stepper.step(1);
        numberOfStep++;       
        if( (mode == 1 && toggle == true) || (mode == 0 && analogRead(LIGHT_SENSOR) > thresholdvalue) ){
           break;
        }
    }
    digitalWrite(LED_PIN,LOW);
    windowState = 1; // 
}

void close_shade(){
    digitalWrite(LED_PIN,HIGH);
    while(numberOfStep > 0){
        stepper.step(-1);
        numberOfStep--;
         if( (mode == 1 && toggle == false) || (mode == 0 && analogRead(LIGHT_SENSOR) < thresholdvalue)){
           break;
        }
    }
    digitalWrite(LED_PIN,LOW);
    windowState = 0;
}
/*********************************************************************************************************

  END FILE

*********************************************************************************************************/
