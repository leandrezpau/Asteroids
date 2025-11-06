#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>




#include <stdio.h>



int esat::main(int argc, char **argv) {

	
	esat::WindowInit(800,600);
	WindowSetMouseVisibility(true);
  
    while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {

    	esat::DrawBegin();
    	esat::DrawClear(0,0,0);

    	
    	esat::DrawEnd();  	
    	esat::WindowFrame();

    }
    esat::WindowDestroy();

    return 0;  
}
