

#define SENSOR_ROOM1 A0
#define SENSOR_ROOM2 A0
#define SENSOR_ROOM3 A0

#define FLASH_LIGHT1 A0
#define FLASH_LIGHT2 A0
#define FLASH_LIGHT3 A0

#define BUZZER1 A0
#define BUZZER2 A0
#define BUZZER3 A0

bool room1 = false
bool room2 = false
bool room2 = false

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 


  pinMode(FLASH_LIGHT1,OUTPUT);
  pinMode(FLASH_LIGHT2,OUTPUT);
  pinMode(FLASH_LIGHT3,OUTPUT);
  
  pinMode(BUZZER1,OUTPUT);
  pinMode(BUZZER2,OUTPUT);
  pinMode(BUZZER3,OUTPUT);
  
  pinMode(SENSOR_ROOM1,INPUT);
  pinMode(SENSOR_ROOM2,INPUT);
  pinMode(SENSOR_ROOM3,INPUT);
}

void loop() {
    if(){
      digitalWrite()
      digitalWrite()
      digitalWrite()
    }
    else if(){
      digitalWrite()
      digitalWrite()
      digitalWrite()
    }else{
      digitalWrite()
      digitalWrite()
      digitalWrite()
    }
}
