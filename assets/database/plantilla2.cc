#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>




#include <stdio.h>



unsigned char fps=25; //Control de frames por segundo
double current_time,last_time;


int esat::main(int argc, char **argv) {


	esat::WindowInit(800,600);
	WindowSetMouseVisibility(true);
  
    while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {

    	last_time = esat::Time(); 
    	esat::DrawBegin();
    	esat::DrawClear(0,0,0);


    	esat::DrawEnd();
    	


    	//Control fps 
    	do{
    		current_time = esat::Time();
    	}while((current_time-last_time)<=1000.0/fps);
    	esat::WindowFrame();
  }
  
  
  esat::WindowDestroy();

  
  return 0;
    
}
