#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);           // select the pins used on the LCD panel

typedef enum {
  IdleT,
  CountH,
  CountD
}TIME_STATE;

typedef enum {
  IdleC,
  IncreaseC,
  DecreaseC
}CLICK_STATE;

typedef enum {
  IdleW,
  ChoiceW
}WEATHER_STATE;

TIME_STATE TimeCurrentState;
CLICK_STATE ClickCurrentState;
WEATHER_STATE WeatherCurrentState;

// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
int Click;
int StartTime;
int Heure;
int timer;

#define btnRIGHT  0
#define btnUP_Increase     1
#define btnDOWN_Decrease   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

int read_LCD_buttons(){               // read the buttons
    adc_key_in = analogRead(0);       // read the value from the sensor 


    if (adc_key_in > 1000) return btnNONE; 

    // For V1.1 us this threshold
    if (adc_key_in < 50)   return btnRIGHT;  
    if (adc_key_in < 250)  return btnUP_Increase; 
    if (adc_key_in < 450)  return btnDOWN_Decrease; 
    if (adc_key_in < 650)  return btnLEFT; 
    if (adc_key_in < 850)  return btnSELECT;  


    return btnNONE;                // when all others fail, return this.
}

void ClickInit(){
  Click = 1;
  ClickCurrentState = IdleC;
}

void ClickUpdate(){
  switch (ClickCurrentState){
    case IdleC:
      if(btnUP_Increase){
        StartTime = millis();
        if(Click < 3){
          Click ++;
        }
        ClickCurrentState = IncreaseC;
      }
      if(btnDOWN_Decrease){
        StartTime = millis();
        if(Click > 1){
          Click --;
        }
        ClickCurrentState = DecreaseC;
      }
      break;
    case IncreaseC:
      if(btnNONE && ((millis() - StartTime) > 100)){
        ClickCurrentState = IdleC;
      }
      break;
    case DecreaseC:
      if((btnNONE) && ((millis() - StartTime) > 100)){
        ClickCurrentState = IdleC;
      }
      break;
  }
}

void TimeInit(){
  Heure = 0;
  timer = millis() + (30000 / Click);
  TimeCurrentState = IdleT;
}

void setup(){
  Serial.begin(9600);
   lcd.begin(16, 2);               // start the library
   lcd.setCursor(0,0);             // set the LCD cursor   position 
   lcd.print("Choice weather ?");  // print a simple message on the LCD
}
 
void loop(){
   //lcd.setCursor(12,1);             // move cursor to second line "1" and 9 spaces over
   //lcd.print(millis()/1000);       // display seconds elapsed since power-up

   lcd.setCursor(0,1);             // move to the begining of the second line
   lcd_key = read_LCD_buttons();   // read the buttons

   switch (lcd_key){               // depending on which button was pushed, we perform an action

       case btnRIGHT:{             //  push button "RIGHT" and show the word on the screen
            lcd.print("CLOUDY       ");
            break;
       }
       case btnLEFT:{
             lcd.print("SUNNY        "); //  push button "LEFT" and show the word on the screen
             break;
       }    
       case btnUP_Increase:{
             lcd.print("RAINY        ");  //  push button "UP" and show the word on the screen
             break;
       }
       case btnDOWN_Decrease:{
             lcd.print("HEATWAVE     ");  //  push button "DOWN" and show the word on the screen
             break;
       }
       case btnSELECT:{
             lcd.print("Communication");  //  push button "SELECT" and show the word on the screen
             Serial.println("163");
             break;
       }
   }
}
