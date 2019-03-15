#include <SoftwareSerial.h>
#include <serLCD.h>
#include "max6675.h"

//Max Setup
int thermoDO = 8;
int thermoCS = 9;
int thermoCLK = 10;
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
int vccPin = 11;
int gndPin = 12;
int temp = 0;

//Relay Stuff
int relayPin = 13;
float heatTime = 1;
int cycleCount = 0;
int cycleReset = 20;
int Setpoint = 100;


//Input pins
int selPin=A0;
int upPin=4;
int downPin=6;
int leftPin=5;
int rightPin=A1;
int debounce = 0;

//LCD setup
int LCDpin = 7;
serLCD lcd(LCDpin);

//timer
unsigned long startTime = millis();
unsigned long currentTime = 0;
int mins = 0;
int secs = 0;

//Menu
int mode = 0;
int menu = 0;

//reflowing
int stage = 1;
int sTime = 0;

void setup() {
  // Temp Pins
  pinMode(vccPin, OUTPUT); digitalWrite(vccPin, HIGH);
  pinMode(gndPin, OUTPUT); digitalWrite(gndPin, LOW);
  //Input Pins
  pinMode(selPin, INPUT);
  pinMode(upPin, INPUT);
  pinMode(downPin, INPUT);
  pinMode(leftPin, INPUT);
  pinMode(rightPin, INPUT);
  // wait for MAX chip to stabilize
  delay(500);
  Serial.begin(9600);
    
}

void loop() {
   temp = thermocouple.readCelsius();
   currentTime = millis();
   currentTime = currentTime - startTime;
   Serial.println(temp);
//Relay control
 //tune until performance is acceptable
   if (temp + 25 < Setpoint){
    heatTime = cycleReset;
   }else{
    heatTime = map(temp, Setpoint, 0, 1, cycleReset);
   }
   cycleCount = cycleCount + 1;
   if (cycleCount > cycleReset){
    cycleCount = 0;
   }
   if (heatTime >= cycleCount){
    digitalWrite(relayPin,HIGH);
    //Serial.print(" ON ");
   }else{
    digitalWrite(relayPin,LOW);
    //Serial.print(" OFF ");
   }
   
// Display temp and elapsed time on the top line 
   lcd.clear();
   lcd.selectLine(1);
   lcd.print(temp);
   lcd.print(" C");
   lcd.print("   ");
   if (temp < 100){
      lcd.print(" ");
   }
   if (mins < 10){
      lcd.print(" ");
   }
   //Display the time in Human formatting
   mins = currentTime/60000;
   secs = (currentTime/1000)-(mins*60);
   lcd.print(mins);
   lcd.print(":");
   if (secs < 10){
     lcd.print(0);
   }
   lcd.print(secs);

   lcd.selectLine(2);
   //modes
   if(mode==0){
    if(menu==0){
      lcd.print("Ready to Cook");
    }else if(menu==1){
      lcd.print("Set Temp");
    }else if(menu==2){
      lcd.print("Reflow");
    }else if(menu==3){
      menu=1;
    }
   }else if(mode==1){             //Set Temp
    lcd.print("Set Temp : ");
    lcd.print(Setpoint);
   }else if(mode==2){             //Reflow Mode
    lcd.print("Reflowing");
    if (stage == 1){
     Setpoint = 150;
     lcd.print("Ramping Up");
   }else if (stage == 2){
     lcd.print("Soaking");
    }else if (stage == 3){
     Setpoint = 225;
     lcd.print("Reflowing");
    }else if (stage == 4){
     Setpoint = 10;
     if (temp < 50){
      lcd.print("Complete!");
     }else{
     lcd.print("Cooling Off");
     }
    }
   }
   //Reflow stage progression
   if (stage == 3 && temp > 220){
     stage = 4;
   }else if (stage == 2 && currentTime > sTime ){
     stage = 3;
   }else if (stage == 1 && temp > 145){
     sTime = currentTime + 60000;
     stage = 2;
   }
   
//Inputs etc
   if(digitalRead(selPin) == LOW){
    //Serial.print("Select");
    if(debounce==0){
     mode=menu;
     menu=0;
     startTime = currentTime;
    }
    debounce = 2;
   }
   if(digitalRead(upPin) == LOW){
    //Serial.print("Up");
    Setpoint = Setpoint + 1;
    debounce = debounce + 2;
    if(debounce>5){
      debounce = 6;
      Setpoint = Setpoint + 1;
    }
   }
   if(digitalRead(downPin) == LOW){
   //Serial.print("Down");
    Setpoint = Setpoint - 1;
    debounce = debounce + 2;
    if(debounce>5){
      debounce = 6;
      Setpoint = Setpoint - 1;
    }
   }
    if(digitalRead(leftPin) == LOW){
    //Serial.print("Left");
    if(debounce==0){
     menu = menu-1;
     if(menu<0){
       menu=0;
     }
    }
    debounce = 2;    
   }
    if(digitalRead(rightPin) == LOW){
    //Serial.print("Right");
    if(debounce==0){
     menu = menu+1;
    }
    debounce = 2;
   }
   debounce = debounce-1;
   if(debounce<0){
    debounce=0;
   }
   delay(220);
}
