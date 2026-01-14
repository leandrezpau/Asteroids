#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>

#include <iostream>

#include <stdlib.h>
#include <math.h>
#include <time.h>

const int kWindowHeight = 896;
const int kWindowWidth = 1024;
unsigned char fps = 5000; //Control de frames por segundo
int frameCounter = 0;
double deltaTime = 0.0, current_time, last_time;



int esat::main(int argc, char **argv) {
  esat::WindowInit(kWindowWidth,kWindowHeight);
  esat::WindowSetMouseVisibility(true);

  esat::DrawSetTextSize(50);
	esat::DrawSetTextFont("fonts/limon.otf");
  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
    //Control time & fps
    do{
      current_time = esat::Time();
      deltaTime = current_time - last_time;
    } while((deltaTime) <= 1000.0 / fps);
    frameCounter = (frameCounter>=5000)?0:frameCounter +1;
    last_time = esat::Time();   


    esat::DrawBegin();
    esat::DrawClear(0,0,0); 

    char frametime[10];
    static double avg_delta = 16.0;
    avg_delta += (deltaTime - avg_delta) / 100.0;
    sprintf(frametime , "%.02f", 1000.0/avg_delta);
    
    esat::DrawSetFillColor(255,50,50);
    esat::DrawText(200,50 ,frametime);

    esat::DrawEnd();
    esat::WindowFrame();
    if(esat::IsSpecialKeyDown(esat::kSpecialKey_Space)){
      *kWindowHeight += 50;
    }
    
  }
  esat::WindowDestroy();
  return 0;
}