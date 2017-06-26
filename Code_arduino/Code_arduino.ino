#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);           // select the pins used on the LCD panel

typedef enum {
  IdleT,
  CountT
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
TIME_STATE TimeNextState;
CLICK_STATE ClickNextState;
WEATHER_STATE WeatherNextState;

// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
int Click;
int StartTime;
int Heure;
int Current;
int Forecast;
unsigned long Timer;
int CurrentWeather;
int ForecastWeather;
int PlusMoins;
int boucle;



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
  lcd_key = read_LCD_buttons();
  switch (ClickCurrentState){
    case IdleC:
      if(lcd_key == btnUP_Increase){
        StartTime = millis();
        if(Click < 5){
          Click ++;
          lcd.setCursor(5, 0);
          lcd.print(Click);
      }
        ClickCurrentState = IncreaseC;
      }
      if(lcd_key == btnDOWN_Decrease){
        StartTime = millis();
        if(Click > 1){
          Click --;
          lcd.setCursor(5, 0);
          lcd.print(Click);
        }
        ClickCurrentState = DecreaseC;
      }
      break;
    case IncreaseC:
      if((lcd_key == btnNONE) && ((millis() - StartTime) > 100)){
        ClickCurrentState = IdleC;
      }
      break;
    case DecreaseC:
      if((lcd_key == btnNONE) && ((millis() - StartTime) > 100)){
        ClickCurrentState = IdleC;
      }
      break;
  }
}

void TimeInit(){
  Heure = 0;
  Timer = millis() + (30000 / Click);
  TimeCurrentState = IdleT;
}

void TimeUpdate(){
  switch (TimeCurrentState){
    case IdleT:
      if(Timer <= millis()){
        Heure ++;
        boucle = 0;
        TimeCurrentState = CountT;
      }
      break;
    case CountT:
      Timer = millis() + (30000 / Click);
      lcd.setCursor(0, 1);
      lcd.print(Heure);
      Serial.print("@");
      Serial.print(Heure);
      Serial.print(",");
      switch(CurrentWeather){
        case (0):
          Serial.print("rainy");
          break;
        case(1):
          Serial.print("cloudy");
          break;
        case(2):
          Serial.print("sunny");
          break;
        case(3):
          Serial.print("heatwave");
          break;
        case(4):
          Serial.print("thunderstorm");
          break;
      }
      Serial.print(",");
      switch(ForecastWeather){
        case (0):
          Serial.print("rainy");
          break;
        case(1):
          Serial.print("cloudy");
          break;
        case(2):
          Serial.print("sunny");
          break;
        case(3):
          Serial.print("heatwave");
          break;
        case(4):
          Serial.print("thunderstorm");
          break;
      }
      Serial.println("#");
      TimeCurrentState = IdleT;
  }
}

void WeatherInit(){
  boucle = 1;
  WeatherCurrentState = IdleW;
}

void WeatherUpdate(){
  switch(WeatherCurrentState){
    case IdleW:
      if (Heure%24 == 0 && boucle == 0){
        Current = random(1,101);
        Forecast = random(1,101);
        Serial.print(Current);
        Serial.print(",");
        Serial.println(Forecast);
        CurrentWeather = ForecastWeather;
        WeatherNextState = ChoiceW;
        boucle = 1;
      }
      break;
    case ChoiceW:
      WeatherNextState = IdleW;
  }
  WeatherCurrentState = WeatherNextState;
}

void WeatherOutput(){
  if (WeatherCurrentState == ChoiceW){
    if (Current >= 89 && Current <= 98){
    PlusMoins = random(1,3);
    if (PlusMoins == 1){
      CurrentWeather ++;
      if (CurrentWeather == 5){
        CurrentWeather = 0;
      }
    }else {
      CurrentWeather --;
      if (CurrentWeather == -1){
        CurrentWeather = 4;
      }
    }
    }
    if (Current == 99 || Current == 100){
      PlusMoins = random(1,3);
      if (PlusMoins == 1){
        CurrentWeather += 2;
        if (CurrentWeather == 5){
          CurrentWeather = 0;
        }
        if (CurrentWeather == 6){
          CurrentWeather = 1;
        }
      }else {
        CurrentWeather -= 2;
        if (CurrentWeather == -1){
          CurrentWeather = 4;
        }
        if (CurrentWeather == -2){
          CurrentWeather = 3;
        }
      }
    }
    if (Forecast >= 1 && Forecast <= 15){
      ForecastWeather = 0;
    }
    if (Forecast >= 16 && Forecast <= 35){
      ForecastWeather = 1;
    }
    if (Forecast >= 36 && Forecast <= 75){
      ForecastWeather = 2;
    }
    if (Forecast >= 76 && Forecast <= 95){
      ForecastWeather = 3;
    }
    if (Forecast >= 96 && Forecast <= 100){
      ForecastWeather = 4;
    }
  }
  
}

void setup(){
  Serial.begin(9600);
   lcd.begin(16, 2);
   ClickInit();
   TimeInit();
   WeatherInit();
   randomSeed(analogRead(0));
}
 
void loop(){
   //lcd.setCursor(12,1);             // move cursor to second line "1" and 9 spaces over
   //lcd.print(millis()/1000);       // display seconds elapsed since power-up

   /*lcd.setCursor(0,1);             // move to the begining of the second line
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
   }*/
   lcd.setCursor(0, 0);
   lcd.print(Heure/24);
   lcd.setCursor(10, 1);
   lcd.print(millis()/((30000 / Click) / 60));
   TimeUpdate();
   WeatherUpdate();
   ClickUpdate();
   WeatherOutput();
}
