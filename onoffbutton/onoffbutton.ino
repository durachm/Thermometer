//This example implements a simple sliding On/Off button. The example
// demonstrates drawing and touch operations.
//
//Thanks to Adafruit forums member Asteroid for the original sketch!
//
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

#define STMPE_CS 8
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);
#define TFT_CS 10
#define TFT_DC 9
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

void drawBtns()
{
    for (int x = 0; x < 3; x++) {
      for (int y = 0; y < 4; y++) {
        if(y == 0)
          tft.drawRect(x*100+10*x, y*50+5, 100, 50, ILI9341_DARKGREY);
        else
          tft.drawRect(x*100+10*x, y*50+10*y+5, 100, 50, ILI9341_DARKGREY);
      }
    }
  
}

void writeText(int x, int y, const char text[])
{ 
  tft.setCursor(5 + x*110, 10 + y*+60);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.println(text);
}

int readTouchScreen(void){
  if (!ts.bufferEmpty())
  {   
    // Retrieve a point  
    TS_Point p = ts.getPoint(); 
    // Scale using the calibration #'s
    // and rotate coordinate system
    p.x = map(p.x, TS_MINY, TS_MAXY, 0, tft.height());
    p.y = map(p.y, TS_MINX, TS_MAXX, 0, tft.width());
    int y = tft.height() - p.x;
    int x = p.y;

    for (int xt = 0; xt < 3; xt++) {
      for (int yt = 0; yt < 4; yt++) {
        if(yt == 0){
          if((x > xt*100+10*xt) && (x < (xt*100+10*xt + 100))) {
            if ((y > yt*50+5) && (y <= (yt*50+5 + 50))){
              return(xt*10 + yt);
            }
          }
        }
        else{
          if((x > xt*100+10*xt) && (x < (xt*100+10*xt + 100))) {
            if ((y > yt*50+10*yt+5) && (y <= (yt*50+10*yt+5 + 50))){
              return(xt*10 + yt);
            }
          }
        }
      }
    }
  }
  return(-1);
}

void setup(void)
{
  tft.begin();
  ts.begin();
  

  tft.fillScreen(ILI9341_BLACK);
  // origin = left,top landscape (USB left upper)
  tft.setRotation(1); 

  drawBtns();
  writeText(2,3, "hallo");

}

void loop()
{

  int test =  readTouchScreen();

  if(test != -1){
    tft.setCursor(50,50);
    tft.setTextColor(ILI9341_RED);
    tft.setTextSize(1);
    tft.println(test);
  }

 
}
