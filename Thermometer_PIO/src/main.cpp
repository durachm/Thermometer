/***************************************************
  This is our GFX example for the Adafruit ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/


#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <BME280I2C.h>

const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

tmElements_t tm;
BME280I2C bme;

// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_CS 10

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

#define TEM_COLOR ILI9341_RED
#define HUM_COLOR ILI9341_BLUE

unsigned long  draw_cooardinates() {
  tft.fillScreen(ILI9341_BLACK);
  unsigned long start = micros();
  tft.setCursor(7, 50);
  tft.setTextColor(TEM_COLOR);
  tft.setTextSize(1);
  tft.println("C");
  tft.drawCircle(4, 50, 1,TEM_COLOR);
  tft.setTextColor(HUM_COLOR);
  tft.setCursor(310, 50);
  tft.println("%");
  // X-Axis is 280p long 
  tft.drawLine(20, 225, 300, 225, ILI9341_WHITE);

// Y-Axis is 175p long 
  tft.drawLine(20, 225, 20, 50, TEM_COLOR);
  tft.drawLine(17, 57, 20, 50, TEM_COLOR);
  tft.drawLine(23, 57, 20, 50, TEM_COLOR);

  tft.drawLine(300, 225, 300, 50, HUM_COLOR);
  tft.drawLine(303, 57, 300, 50, HUM_COLOR);
  tft.drawLine(297, 57, 300, 50, HUM_COLOR);

  for (size_t i = 0; i < 6; i++)
  {
    tft.setCursor(3,220 - i*30);
    tft.setTextColor(TEM_COLOR);
    tft.setTextSize(1);
    tft.println(i*5 + 10);
    tft.drawFastHLine(20,220 - i*30+5,5,TEM_COLOR);
  }

  for (size_t i = 0; i < 6; i++)
  {
    tft.setCursor(305,220 - i*30);
    tft.setTextColor(HUM_COLOR);
    tft.setTextSize(1);
    tft.println(i*10 + 20);
    tft.drawFastHLine(295,220 - i*30+5,5,HUM_COLOR);
  }

    for (size_t i = 0; i < 25; i = i+2)
  {
    tft.setCursor(23+i*11,230);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(1);
    tft.println(i,10);
  }

/*
    tft.setCursor(10,10);
    tft.setTextColor(TEM_COLOR);
    tft.setTextSize(3);
    tft.print(27.5,1);
    tft.print(" C");
    tft.drawCircle(93, 11, 3,TEM_COLOR);

    
    tft.setCursor(255,10);
    tft.setTextColor(HUM_COLOR);
    tft.setTextSize(3);
    tft.print(40);
    tft.print("%");
*/
}

bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}
float temp(NAN), hum(NAN), pres(NAN);
BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
BME280::PresUnit presUnit(BME280::PresUnit_Pa);

void printFuncttion(void){

  unsigned int min = tm.Hour * 60 + tm.Minute;

  if(min < 8)
    draw_cooardinates();

  unsigned int x = map(min, 0, 1440, 20, 300);

  bme.read(pres, temp, hum, tempUnit, presUnit);

  int y_tem = map(temp, 10, 40, 225, 40);
  //tft.drawPixel(x, 120, TEM_COLOR);
  tft.fillCircle(x, y_tem,1, TEM_COLOR);

  int y_hum = map(hum, 20, 80, 225, 40);
  //tft.drawPixel(x, 120, TEM_COLOR);
  tft.fillCircle(x, y_hum,1, HUM_COLOR);
  
}
void printBME280Data(void){
    bme.read(pres, temp, hum, tempUnit, presUnit);

    tft.fillRect(0,0,320,50,ILI9341_BLACK);
    tft.setCursor(10,10);
    tft.setTextColor(TEM_COLOR);
    tft.setTextSize(3);
    tft.print(temp,1);
    tft.print(" C");
    tft.drawCircle(93, 11, 3,TEM_COLOR);

    tft.setCursor(255,10);
    tft.setTextColor(HUM_COLOR);
    tft.setTextSize(3);
    tft.print(hum,0);
    tft.print("%");

}

void setup() {
  Serial.begin(9600);
  bool parse=false;
  bool config=false;
  bme.begin();

  // get the date and time the compiler was run
  if (getDate(__DATE__) && getTime(__TIME__)) {
    parse = true;
    // and configure the RTC with this info
    if (RTC.write(tm)) {
      config = true;
    }
  }
  tft.begin();
  tft.setRotation(1);
  draw_cooardinates();

  printBME280Data();
  printFuncttion();
}

void loop(void) {
RTC.read(tm);

  if(tm.Second %10 == 1){
   printBME280Data();
   delay(1000);
  }

  if(tm.Second %308 == 1){  // was 308
    printFuncttion();
   delay(1000);
  }

}


