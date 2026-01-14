#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <stdio.h>
#include <math.h>

#define PI 3.14159265
const int Num_Points = 30;       //Number of Points our cyrcle willhave
float Angle = 2*PI / Num_Points; //Radians % Number of points
int Radius = 150; //Radios of "px" our cyrcle will have
int PosX = 400, PosY = 300;
//FIRST faketeroid
int kNpoints = 12;
int color = 0;
float asteroid[24]{
  -13,-40,
  -35,-22,
  -28,-2,
  -40,18,
  -24,40,
  -12,33,
  14,45,
  37,19,
  22,-4,
  41,-12,
  25,-34,
  5,-27,
};

void MovePoly(){
  for(int i = 0; i < kNpoints * 2; i++){
    asteroid[i] = asteroid[i] + 300;
  }
}

int ComprobarLado(float lado1x,float lado1y,float lado2x,float lado2y,float puntox,float puntoy){
  float vecx = lado1x - lado2x;
  float vecy = lado1y - lado2y;
  float distx = puntox - lado1x;
  float disty = puntoy - lado1y;
  if((distx * -vecy + disty * vecx) < 0){
    return 0;
  }else{
    return 1;
  }
}

void CalcVector(){
  if((ComprobarLado(asteroid[0],asteroid[1],asteroid[2],asteroid[3],esat::MousePositionX(),esat::MousePositionY())
   + ComprobarLado(asteroid[2],asteroid[3],asteroid[4],asteroid[5],esat::MousePositionX(),esat::MousePositionY())
   + ComprobarLado(asteroid[4],asteroid[5],asteroid[10],asteroid[11],esat::MousePositionX(),esat::MousePositionY())
   + ComprobarLado(asteroid[10],asteroid[11],asteroid[12],asteroid[13],esat::MousePositionX(),esat::MousePositionY())
   + ComprobarLado(asteroid[12],asteroid[13],asteroid[14],asteroid[15],esat::MousePositionX(),esat::MousePositionY())
   + ComprobarLado(asteroid[14],asteroid[15],asteroid[16],asteroid[17],esat::MousePositionX(),esat::MousePositionY())
   + ComprobarLado(asteroid[16],asteroid[17],asteroid[22],asteroid[23],esat::MousePositionX(),esat::MousePositionY())
   + ComprobarLado(asteroid[22],asteroid[23],asteroid[0],asteroid[1],esat::MousePositionX(),esat::MousePositionY())) == 8
   ||
   (ComprobarLado(asteroid[4],asteroid[5],asteroid[6],asteroid[7],esat::MousePositionX(),esat::MousePositionY())
   + ComprobarLado(asteroid[6],asteroid[7],asteroid[8],asteroid[9],esat::MousePositionX(),esat::MousePositionY())
   + ComprobarLado(asteroid[8],asteroid[9],asteroid[10],asteroid[11],esat::MousePositionX(),esat::MousePositionY())
   + ComprobarLado(asteroid[10],asteroid[11],asteroid[4],asteroid[5],esat::MousePositionX(),esat::MousePositionY())) == 4
   || 
   (ComprobarLado(asteroid[16],asteroid[17],asteroid[18],asteroid[19],esat::MousePositionX(),esat::MousePositionY())
   + ComprobarLado(asteroid[18],asteroid[19],asteroid[20],asteroid[21],esat::MousePositionX(),esat::MousePositionY())
   + ComprobarLado(asteroid[20],asteroid[21],asteroid[22],asteroid[23],esat::MousePositionX(),esat::MousePositionY())
   + ComprobarLado(asteroid[22],asteroid[23],asteroid[16],asteroid[17],esat::MousePositionX(),esat::MousePositionY())) == 4
  ){
    color = 255;
  }else{
    color = 0;
  }
}

int esat::main(int argc, char **argv) {
  esat::WindowInit(800,600);
  esat::WindowSetMouseVisibility(true);
  MovePoly();
  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
    CalcVector();
    esat::DrawBegin();
    esat::DrawClear(0,0,0);

    esat::DrawSetStrokeColor(255,255,255);
    esat::DrawSetFillColor(255,50,50,color);
    esat::DrawSolidPath(asteroid, 12);

    esat::DrawEnd();  	
    esat::WindowFrame();
  }
  esat::WindowDestroy();
  return 0;  
}
