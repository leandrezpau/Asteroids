#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/math.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265
const int kNpoints = 5;       //Number of Points our cyrcle willhave
float puntos[kNpoints * 2]; //*2 To have the double of coords: x, y
float Angle = 2*PI / kNpoints; //Radians % Number of points
int Radius = 150; //Radios of "px" our cyrcle will have
int PosX = 400, PosY = 300;
int pepe = 0;
void InitCyrcle(){
    for(int e = 0; e < kNpoints; e++){
        puntos[e*2] = cosf(e * Angle) * Radius + PosX;
        puntos[e*2+1] = sinf(e * Angle) * Radius + PosY;
        
    }
}
int ComprobarLado(float lado1x,float lado1y,float lado2x,float lado2y,float puntox,float puntoy){
  if(((puntox - lado1x) * -(lado1y - lado2y) + (puntoy - lado1y) * (lado1x - lado2x)) < 0){
    return 1;
  }else{
    return 0;
  }
}
void CalcVector(){
  int dentro = 0;
  dentro += ComprobarLado(puntos[0],puntos[1],puntos[2],puntos[3],esat::MousePositionX(),esat::MousePositionY());
  dentro += ComprobarLado(puntos[2],puntos[3],puntos[4],puntos[5],esat::MousePositionX(),esat::MousePositionY());
  dentro += ComprobarLado(puntos[4],puntos[5],puntos[6],puntos[7],esat::MousePositionX(),esat::MousePositionY());
  dentro += ComprobarLado(puntos[6],puntos[7],puntos[8],puntos[9],esat::MousePositionX(),esat::MousePositionY());
  dentro += ComprobarLado(puntos[8],puntos[9],puntos[0],puntos[1],esat::MousePositionX(),esat::MousePositionY());
  if(dentro == kNpoints){
    pepe = 255;
  }else{
    pepe = 0;
  }
}

int esat::main(int argc, char **argv) {
	esat::WindowInit(800,600);
	esat::WindowSetMouseVisibility(true);
    
    InitCyrcle();

    while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
      CalcVector();
    	esat::DrawBegin();
    	esat::DrawClear(0,0,0);

        esat::DrawSetStrokeColor(255,50,50);
        esat::DrawSetFillColor(255,50,50,pepe);
        esat::DrawSolidPath(puntos, kNpoints);

    	esat::DrawEnd();  	
    	esat::WindowFrame();

    }
    esat::WindowDestroy();

    return 0;  
}