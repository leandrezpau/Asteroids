#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


#define PI 3.14159265


//PLAYER
struct Player{
  esat::Vec2 center;
  esat::Vec2 speed;
  float mainpoints[12];  //All player points that conform the figure
  float fire[6];        //Player points of the fire booster
  float angle;        //Player global angle
  int stage;          //Stage: 1 -> Normal, Alive | 2 -> Dead, Respawning
  int lives;          //Number of lives player has
  int counter;        //Timer to various functions    
  float mass;
};
Player* player;

const int kWindowHeight = 1000;
const int kWindowWidth = 1000;
unsigned char fps = 60; //Control de frames por segundo
int frameCounter = 0;
double deltaTime = 0.0, current_time = 0.0, last_time = 0.0;
double bullet_time = 0.0, current_bullet_time = 0.0;

int tra = 1;
esat::Vec2* trayec = (esat::Vec2*) malloc(sizeof(esat::Vec2) * tra);
bool start = false;


struct BlackHole{
  esat::Vec2 center;
  esat::Vec3* base;
  esat::Vec2* points;
  int stage;
  float mass;
  int counter;
};
BlackHole* blackhole;
int kNHoles = 1;

void DrawPlayer();
void InputPlayer();
void UpdatePlayer();
void BlackHoleUpdate();
void DrawBlackHole();
void InitBlackHole();
esat::Vec2 GetGravity(esat::Vec2 center, esat::Vec2 hole);

int esat::main(int argc, char **argv) {
  player = (Player*) malloc(sizeof(Player) * 1);
  (player)->center.x = 500;
  (player)->center.y = 500;
  (player)->speed.x = 0;
  (player)->speed.y = 0;
  (player)->angle = -90 * PI / 180;
  (player)->lives = 3;
  (player)->stage = 1;
  (player)->counter = 0;

	esat::WindowInit(kWindowWidth,kWindowHeight);
	esat::WindowSetMouseVisibility(true);
  InitBlackHole();
  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
    //Control time & fps
    for(int i = 0; i < 1; i++){
      BlackHoleUpdate();
      InputPlayer();

      esat::DrawBegin();
      esat::DrawClear(0,0,0);
      DrawBlackHole();
      DrawPlayer();
      esat::DrawEnd();

      esat::WindowFrame();
    }
  }
  esat::WindowDestroy();
  return 0;
}


void InitBlackHole(){
  blackhole =  (BlackHole*) malloc(sizeof(BlackHole) * kNHoles);
  for(int e = 0; e < kNHoles; e++){
    float angle = 2 * PI / 5;
    (blackhole + e)->base = (esat::Vec3*) malloc(sizeof(esat::Vec3) * 5);
    (blackhole + e)->points = (esat::Vec2*) malloc(sizeof(esat::Vec2) * 5);
    (blackhole + e)->stage = 0;

    (blackhole + e)->center.x = kWindowHeight / 2;
    (blackhole + e)->center.y = kWindowHeight / 2;

    for(int i = 0; i < 5; i++){
      ((blackhole + e)->base + i)->x = cosf(i * angle) * 50;
      ((blackhole + e)->base + i)->y = sinf(i * angle) * 50;
      ((blackhole + e)->base + i)->z = 1.0f;
    }

    (blackhole + e)->mass = e;
  }
  
}

void DrawBlackHole(){
  for(int j = 0; j < kNHoles; j++){
    if((blackhole + j)->stage > 0){
      for(int i = 4; i >= 0; i--){
        esat::Mat3 m = esat::Mat3Identity();
        //float size = (blackhole + j)->mass * i * 0.25 + 1;
        float size = i * 0.25 + 1;
        m = esat::Mat3Multiply(esat::Mat3Translate(0.0f, 0.0f),m);
        m = esat::Mat3Multiply(esat::Mat3Scale(size, size), m);
        m = esat::Mat3Multiply(esat::Mat3Rotate((i * 33 * PI / 180 + esat::Time() * 0.001) * (((i % 2) * 2) - 1)), m);
        m = esat::Mat3Multiply(esat::Mat3Translate((blackhole + j)->center.x	, (blackhole + j)->center.y), m);
    
        for (int e = 0; e < 5; e++) {
          esat::Vec3 tmp = esat::Mat3TransformVec3(m, *(blackhole->base + e));
          *((blackhole + j)->points + e) = { tmp.x, tmp.y };
        }
        esat::DrawSetStrokeColor(5 + i * 50, 5 + i * 50, 5 + i * 50, 255);
        esat::DrawSetFillColor(i * 10, i * 10, i * 10, 255);
        esat::DrawSolidPath(&(blackhole + j)->points->x, 5, true);
      }
    }
    if(start){
      trayec = (esat::Vec2*) realloc( trayec, tra * sizeof(esat::Vec2));
      (trayec + tra - 1)->x = player->center.x;
      (trayec + tra - 1)->y = player->center.y;
      esat::DrawSetStrokeColor(255, 255, 255, 255);
      esat::DrawSetFillColor(0, 0, 0, 255);
      esat::DrawPath(&trayec->x, tra);
      tra++;
    }
  } 
}

void BlackHoleUpdate(){
  for(int j = 0; j < kNHoles; j++){
    if(esat::MouseButtonDown(0)){
      if((blackhole + j)->stage == 1){
        (blackhole + j)->stage = 0;
      }else{
        (blackhole + j)->stage = 1;
        (blackhole + j)->center.x = esat::MousePositionX();
        (blackhole + j)->center.y = esat::MousePositionY();
      }
      
    }
    if(esat::MouseButtonDown(1)){
      if(start == true){
        start = false;
      }else{
        start = true;
      }
      
    }
    if((blackhole + j)->stage > 0){
      float kGravity = 15;
      float kForce = 1;
  
      esat::Vec2 gravity = GetGravity(player->center, (blackhole + j)->center);
      if(gravity.y > 0.1 || gravity.x > 0.1 || gravity.y < -0.1 || gravity.x < -0.1){
        player->lives--;
      }
      player->speed.x += gravity.x * kGravity;
      player->speed.y += gravity.y * kGravity;
  
      (blackhole + j)->mass = 1.0f;
      /*
      player->speed.x += gravity.x * kGravity * (blackhole + j)->mass;
      player->speed.y += gravity.y * kGravity * (blackhole + j)->mass;

      (blackhole + j)->counter++;
      if((blackhole + j)->counter < 350){
        (blackhole + j)->mass += 0.01 / 3;
      }

      if((blackhole + j)->counter > 1500){
        (blackhole + j)->mass -= 0.01 / 5;
      }
      if((blackhole + j)->counter > 2000){
        (blackhole + j)->stage = 0;
      }
      */
      printf("\n\n%d",(blackhole + j)->counter);
    }
  }
  
  if(esat::IsKeyDown('J')){
    float cosa = player->center.x - blackhole->center.x;
    float cosa2 = player->center.y - blackhole->center.y;
    player->angle = tanf(sqrtf(cosa * cosa + cosa2 * cosa2));
  }
}

esat::Vec2 GetGravity(esat::Vec2 center, esat::Vec2 hole){
  esat::Vec2 gravity = {hole.x - center.x, hole.y - center.y};
  float norm = sqrtf(gravity.x * gravity.x + gravity.y * gravity.y);
  if(norm < 0.01){norm = 0.01;}
  norm = norm * norm;

  gravity.x = gravity.x / norm;
  gravity.y = gravity.y / norm;

  return gravity;
}






void DrawPlayer(){
  if(player->stage == 1){
    player->center.x += player->speed.x;
    player->center.y += player->speed.y;

    //HEAD
    *(player->mainpoints + 0) = cosf(((0 * PI) / 180) + player->angle) * 18 + player->center.x;
    *(player->mainpoints + 1) = sinf(((0 * PI) / 180) + player->angle) * 18 + player->center.y;

    //SIDES
    *(player->mainpoints + 2) = cosf((( 153 * PI) / 180) +  player->angle) * 25 + player->center.x;
    *(player->mainpoints + 3) = sinf((( 153 * PI) / 180) + player->angle) * 25 + player->center.y;
    *(player->mainpoints + 8) = cosf(((-153 * PI) / 180) + player->angle) * 25 + player->center.x;
    *(player->mainpoints + 9) = sinf(((-153 * PI) / 180) + player->angle) * 25 + player->center.y;

    //BACK
    *(player->mainpoints + 4) = cosf((( 160 * PI) / 180) + player->angle) * 18 + player->center.x;
    *(player->mainpoints + 5) = sinf((( 160 * PI) / 180) + player->angle) * 18 + player->center.y;
    *(player->mainpoints + 6) = cosf(((-160 * PI) / 180) + player->angle) * 18 + player->center.x;
    *(player->mainpoints + 7) = sinf(((-160 * PI) / 180) + player->angle) * 18 + player->center.y;

    *(player->mainpoints + 10) = player->center.x;
    *(player->mainpoints + 11) = player->center.y;

    if(esat::IsKeyPressed('W')){
      //FIRE
      *(player->fire + 0) = cosf((( 164 * PI) / 180) + player->angle) * 18 + player->center.x;
      *(player->fire + 1) = sinf((( 164 * PI) / 180) + player->angle) * 18 + player->center.y;
      *(player->fire + 2) = cosf((( 180 * PI) / 180) + player->angle) * 25 + player->center.x;
      *(player->fire + 3) = sinf((( 180 * PI) / 180) + player->angle) * 25 + player->center.y;
      *(player->fire + 4) = cosf(((-164 * PI) / 180) + player->angle) * 18 + player->center.x;
      *(player->fire + 5) = sinf(((-164 * PI) / 180) + player->angle) * 18 + player->center.y;
    }
  }
  

  esat::DrawSetStrokeColor(255,255,255);
  esat::DrawSetFillColor(255,255,255,0);
  esat::DrawSolidPath(player->mainpoints, 5);
  
  //DRAW FIRE
  if(esat::IsKeyPressed('W') && (frameCounter % 2 == 0)){
    esat::DrawSolidPath(player->fire, 3);
  }

  esat::DrawEnd();
}

void InputPlayer(){
  if(player->stage == 1){
    if(player->speed.x != 0 && !esat::IsKeyPressed('W')){
      //player->speed.x *= 0.965;
    }
    if(player->speed.y != 0 && !esat::IsKeyPressed('W')){
      //player->speed.y *= 0.965;
    }

    if(esat::IsKeyPressed('A')){
      player->angle -= 0.065;
    }
    if(esat::IsKeyPressed('D')){
      player->angle += 0.065;
    }

    if(esat::IsKeyPressed('W')){
      if(player->speed.x < 15 && player->speed.x > -15){
        player->speed.x += cosf(player->angle) * 0.1;
      }
      if(player->speed.y < 15 && player->speed.y > -15){
        player->speed.y += sinf(player->angle) * 0.1;
      }
    }
  }
  if(player->center.x < 0){
    player->center.x = 1000;
  }
  if(player->center.x > 1000){
    player->center.x = 0;
  }
  if(player->center.y < 0){
    player->center.y = 1000;
  }
  if(player->center.y > 1000){
    player->center.y = 0;
  }
}

