#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);           // select the pins used on the LCD panel

typedef enum {
  IdleT,
  CountT
}TIME_STATE;

typedef enum {
  IdleC,
  IncreaseC,
  DecreaseC,
  StopC,
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
#define btnSTOP   4
#define btnNONE   5

int read_LCD_buttons(){
    adc_key_in = analogRead(0);
    
    if (adc_key_in > 1000) return btnNONE; 

    if (adc_key_in < 50)   return btnRIGHT;  
    if (adc_key_in < 250)  return btnUP_Increase; 
    if (adc_key_in < 450)  return btnDOWN_Decrease; 
    if (adc_key_in < 650)  return btnLEFT; 
    if (adc_key_in < 850)  return btnSTOP;  

    return btnNONE;                
}

void ClickInit(){
  Click = 1;
  lcd.setCursor(5, 0);
  lcd.print(Click / 10);
  ClickCurrentState = IdleC;
}

void ClickUpdate(){
  lcd_key = read_LCD_buttons();
  switch (ClickCurrentState){
    case IdleC:
      if(lcd_key == btnUP_Increase){
        StartTime = millis();
        if(Click < 10){
          Click += 9;
          lcd.setCursor(5, 0);
          lcd.print(Click / 10);
        } else if (Click < 50){
          Click += 10;
          lcd.setCursor(5, 0);
          lcd.print(Click / 10);
        }
        ClickCurrentState = IncreaseC;
      }
      if(lcd_key == btnDOWN_Decrease){
        StartTime = millis();
        if(Click > 10){
          Click -= 10;
          lcd.setCursor(5, 0);
          lcd.print(Click / 10);
        }else if(Click > 1){
          Click -= 9;
          lcd.setCursor(5, 0);
          lcd.print(Click / 10);
        }
        ClickCurrentState = DecreaseC;
      }
      if(lcd_key == btnSTOP){
        StartTime = millis();
        Serial.println("!");
        ClickCurrentState = StopC;
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
    case StopC:
      if((lcd_key == btnNONE) && ((millis() - StartTime) > 100)){
        ClickCurrentState = IdleC;
      }
  }
}

void TimeInit(){
  Heure = 0;
  Timer = millis() + (30000 / (1 * Click));
  lcd.setCursor(0, 1);
  lcd.print(Heure);
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
      Timer = millis() + (30000 / (1 * Click));
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
  Current = random(1,101);
  Forecast = random(1,101);
  WeatherCurrentState = IdleW;
}

void WeatherUpdate(){
  switch(WeatherCurrentState){
    case IdleW:
      if (Heure%24 == 0 && boucle == 0){
        Current = random(1,101);
        Forecast = random(1,101);
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
   lcd.setCursor(0, 0);
   lcd.print(Heure/24);
   lcd.setCursor(10, 1);
   TimeUpdate();
   WeatherUpdate();
   ClickUpdate();
   WeatherOutput();
}
