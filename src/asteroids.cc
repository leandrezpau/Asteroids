//#define WIN32

//NORMAL LIBRARIES
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

//ESAT LIBRARY
#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <esat_extra/sqlite3.h> 
#include <esat/sprite.h>
//DEFINITIONS
#define PI 3.141591653589

//BULLETS -> PLAYER && UFO
struct Bullet{
  esat::Vec2 center;
  esat::Vec2 speed;
  int active;        //0 -> Dead | 1 -> Alive | 3 -> Willing to die
  int counter;        //Timer to be killed
};
//PLAYER
struct Player{
  esat::Vec2 center;
  esat::Vec2 speed;
  float* mainpoints;  //All player points that conform the figure
  float* fire;        //Player points of the fire booster
  float angle;        //Player global angle
  int stage;          //Stage: 1 -> Normal, Alive | 2 -> Dead, Respawning
  int lives;          //Number of lives player has
  int counter;        //Timer to various functions
  Bullet* bullet;     
};
struct Fakeplayer{    //Prop model for number of lives and player dieing
  esat::Vec2 center;  //Center of the prop
  float* points;      //Every point it has
};

//ASTEROIDS
struct Asteroid{
  esat::Vec2 center;
  esat::Vec2 speed;
  esat::Vec2* points; //Every point of that asteroid
  int kNpoints;       //Number of points that that type has
  int stage;          //0 -> Dead | 1 -> Big | 2 -> Mid | 3 -> Small || That numbs x10 means it is exploding
  int type;           //Type of asteroid it is
  int angle;          //Angle in which appears
  int counter;        //Timer to calc explosion
};
struct Faketeroid{    //Prop model to copy every asteroid
  esat::Vec3* points; //Points it has
  int kNpoints;       //Num of points it has
};
//UFO
struct UFO{
  esat::Vec2 center;  
  esat::Vec2 speed;
  esat::Vec3* basepoints; //Prop Model
  esat::Vec2* points;     //Points it has
  int stage;              //0 -> Dead | 1 -> Big | 2 -> Dead
  int counter;            
};
//GAME MANAGER
struct Strings{        //Helper to use texts in screen, each has its number variable and string
  int number;
  char* string;
};

struct BlackHole{     
  esat::Vec2 center;  //Where it spawns
  esat::Vec3* base;   //Base points
  esat::Vec2* points; //Where thouse points are painted
  int stage;          //0 -> Dead | 1 -> Alive
  float mass;         //The mass the blackhole gets
  int counter;        //Timer
};    

struct UsuarioInfo{
  char* username = NULL;
  char* user_password = NULL;
  char* maxscore = NULL;
  char* user_credits = NULL;
  char* score = NULL;
  char* round = NULL;

  int actual_lives;
  int kNextLive;

  Strings plscore;
  Strings plactualround;
  Strings plcredits;
};
    

enum GameState{       //States the game has each moment
  playermenu = 0,     //Select 1 or 2 players
  unlogged,           //Select to login or toregister
  login,              //Login screen
  registr,            //Register screen
  scoreboard,         //Scoreboard Screen
  mainmenu,           //Menu to select to play or scoreboard
  fullgame,           //Gameplay
  credits,            //When player dies
  error,              //Error screen
};


enum GameMode{
  normal = 0,
  black_hole,
};

Strings max_score;

//IMPORTANT userinfo-> is a pointer to the real credentials on (all_player 0/1)->info
//IMPORTANTSo every time we switch players in reality we switch userinfo-> to (all_player 0 or 1)
UsuarioInfo* userinfo;
UsuarioInfo* all_players;

GameState gamestate = GameState::playermenu;
GameMode gamemode = GameMode::normal;

const int kWindowHeight = 896;
const int kWindowWidth = 1024;
unsigned char fps = 60; //Control de frames por segundo
int frameCounter = 0;
double deltaTime = 0.0, current_time = 0.0, last_time = 0.0;
double bullet_time = 0.0, current_bullet_time = 0.0;

int kNAsteroids = 50;
int AstInGame = 0;
int kNBullets = 5;
int kNPlayers = 1;
int kNHoles = 3;
float kGravity = 6;

int gamecounter = 0;
int user_index = 0;

float* bullet_points = NULL;
char* auxstring = NULL;
char* auxstring2 = NULL;
char* auxstring3 = NULL;
char* sql = NULL;

bool gamesaved = false;
int topplayer = 0;

//GAME MANAGER
void InitGame(Asteroid** asteroid, Faketeroid** faketeroid,
   Fakeplayer** fakeplayer, UFO** ufo, Bullet** ufo_bullet, BlackHole** blackhole);
void InitPlayers(Player** player);
void BorderColide(Asteroid* asteroid, UFO* ufo, Player* player);
int CheckPerimeter(float lado1x,float lado1y,float lado2x,float lado2y,float puntox,float puntoy);
void UpdateDrawScore(Player* player);
void Gameplaygamestate(Player* player, Asteroid* asteroid, Faketeroid* faketeroid, Fakeplayer* fakeplayer);
void ResetString(char* string, int caracters);

//MEMORY
void FreeMemory(Player* player, Asteroid* asteroid, Faketeroid* faketeroid, Fakeplayer* fakeplayer, UFO* ufo, Bullet* ufo_bullet, BlackHole* blackhole);
void CheckPointer(void* punterito);
void FreePointer(void* punterito);
void* OpenPointer(int howmuch);
void* OpenString(int howmuch);

//PLAYER
void InputPlayer(Player* player);
void PlayerShoot(Player* player);
void UpdatePlayer(Player* player, float posx, float posy);
void DrawPlayer(Player* player,Fakeplayer* fakeplayer);
void PlayerRespawn(Player* player, Fakeplayer* fakeplayer, Asteroid* asteroid, UFO* ufo);

void PlayerColides(Player* player, Asteroid* asteroid, Faketeroid* faketeroid);
void PlayerDying(Player* player, Fakeplayer* fakeplayer);

//ARROW
void DefineBullet(Player* player, Bullet* bullet);
void UpdateBullet(Bullet* bullet);
void DrawBullet(Bullet* bullet);

//ASTEROIDS
void AsteroidGeneration(Asteroid* asteroid, Faketeroid* faketeroid);
void UpdateAsteroid(Asteroid* asteroid, Faketeroid* faketeroid);
void DrawAsteroid(Asteroid* asteroid);

void AsteroidColides(Asteroid* asteroid,Faketeroid* faketeroid, Bullet* ufo_bullet, Player* player);
bool AsteroidTriangulate(Asteroid* asteroid, int type, float pointx, float pointy);
void AsteroidOblivion(Asteroid* asteroid,Faketeroid* faketeroid, int father_stage, float centerx, float centery);
void AsteroidExplosion(Asteroid* asteroid,Faketeroid* faketeroid);

void KillAsteroids(Asteroid* asteroid);
//UFO
void UFOGeneration(UFO* ufo, Player* player);
void DrawUFO(UFO* ufo);
void UpdateUFO(UFO* ufo);
void UFOAttacks(UFO* ufo, Bullet* ufo_bullet, Player* player);
void UFOColides(UFO* ufo, Bullet* ufo_bullet, Player* player);

//Menu
void MenuAsteroids(Asteroid* asteroid, Faketeroid* faketeroid);
void DrawCalcMenu(Asteroid* asteroid, Faketeroid* faketeroid);
void EndMenu(Asteroid* asteroid);
void DieScoreBoard(Asteroid* asteroid, Faketeroid* faketeroid);
void DrawCalcPlayerMenu(Player** player, Asteroid* asteroid, Faketeroid* faketeroid);
void InitMenu();
void UnLoggedMenu();
void LoginMenu();
void RegistrterMenu();
void SaveScore();
void ReadScore();

void SwitchPlayers();
void SwitchPlayerstats(Asteroid* asteroid, UFO* ufo);
//DATABASE 
static int callback(void *NotUsed, int argc, char **argv, char **azColName);
static int callbackuser(void *data, int argc, char **argv, char **azColName);

void CreateTable();
void InsertVNN(bool* duplicado);
void InsertData(bool duplicado, int infocounter);
void GetUser(bool* duplicado);
void ShowScoreBoard();
void InsertScore();

void BlackHoleUpdate(BlackHole* blackhole,Player* player, Asteroid* asteroid);
void DrawBlackHole(BlackHole* blackhole);
esat::Vec3 GetGravity(esat::Vec2 center, esat::Vec2 hole);

int esat::main(int argc, char **argv) {
  Player* player = NULL;
  Bullet* bullet = NULL;
  Asteroid* asteroid = NULL;

  Faketeroid* faketeroid = NULL;
  Fakeplayer* fakeplayer = NULL;
  UFO* ufo = NULL;
  Bullet* ufo_bullet = NULL;

  BlackHole* blackhole;
  bullet_points = (float*) OpenPointer(sizeof(float) * 8);
  auxstring = (char*) OpenString(sizeof(char) * 30);
  auxstring2 = (char*) OpenString(sizeof(char) * 30);
  auxstring3 = (char*) OpenString(sizeof(char) * 30);

  InitGame(&asteroid, &faketeroid, &fakeplayer, &ufo, &ufo_bullet, &blackhole);
/*.___  ___.      ___       __  .__   __.     _______  __    __  .__   __.   ______ .___________. __    ______   .__   __. 
  |   \/   |     /   \     |  | |  \ |  |    |   ____||  |  |  | |  \ |  |  /      ||           ||  |  /  __  \  |  \ |  | 
  |  \  /  |    /  ^  \    |  | |   \|  |    |  |__   |  |  |  | |   \|  | |  ,----'`---|  |----`|  | |  |  |  | |   \|  | 
  |  |\/|  |   /  /_\  \   |  | |  . `  |    |   __|  |  |  |  | |  . `  | |  |         |  |     |  | |  |  |  | |  . `  | 
  |  |  |  |  /  _____  \  |  | |  |\   |    |  |     |  `--'  | |  |\   | |  `----.    |  |     |  | |  `--'  | |  |\   | 
  |__|  |__| /__/     \__\ |__| |__| \__|    |__|      \______/  |__| \__|  \______|    |__|     |__|  \______/  |__| \__| 
*/
  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
    //Control time & fps
    do{
      current_time = esat::Time();
      deltaTime = current_time - last_time;
    } while((deltaTime) <= 1000.0 / fps);
    frameCounter = (frameCounter>=fps)?0:frameCounter +1;
    last_time = esat::Time();

    switch(gamestate){
      case GameState::playermenu:{
        //Start the asteroids on that screen
        MenuAsteroids(asteroid,faketeroid);
        //Update and Draw Those asteroids && the buttons of the screen
        DrawCalcPlayerMenu(&player, asteroid,faketeroid);
        //Where asteroids teleport to the other side
        BorderColide(asteroid, ufo, player);
        break;
      }
      case GameState::mainmenu:{
        //Start the asteroids on that screen
        MenuAsteroids(asteroid,faketeroid);
        //Update and Draw Those asteroids && the buttons of the screen
        DrawCalcMenu(asteroid,faketeroid);
        //Where asteroids teleport to the other side
        BorderColide(asteroid, ufo, player);
        break;
      }
      case GameState::unlogged:{
        //Where player has to select to login or register
        UnLoggedMenu();
        break;
      }
      case GameState::login:{
        //Where player logins
        LoginMenu();
        break;
      }
      case GameState::registr:{
        //Where player registers
        RegistrterMenu();
        break;
      }
      case GameState::scoreboard:{
        //Screen to show the top 10 scores
        ShowScoreBoard();
        break;
      }
      case GameState::fullgame:{
        //COLIDES
        AsteroidColides(asteroid, faketeroid,ufo_bullet, player); //Bullet colides with asteroids
        PlayerColides(player, asteroid, faketeroid);              //Asteroid colides on player
        UFOColides(ufo, ufo_bullet, player);                      //Bullet colides on UFO
        AsteroidExplosion(asteroid, faketeroid);                  //Asteroid explosion animation
        //INPUT
        InputPlayer(player);                      //Where player recieves every input
        UFOGeneration(ufo, player);               //Where ufo generations are calculated
        UFOAttacks(ufo, ufo_bullet, player);      //Ufo Attacks calculations
        //UPDATES
        BlackHoleUpdate(blackhole, player, asteroid);
        UpdateAsteroid(asteroid, faketeroid);
        UpdateBullet(player->bullet);
        UpdateBullet(ufo_bullet);
        UpdatePlayer(player, player->center.x, player->center.y);
        UpdateUFO(ufo);

        //DRAW BEGIN
        esat::DrawBegin();
        esat::DrawClear(0,0,0);

        DrawBlackHole(blackhole);

        esat::DrawSetStrokeColor(255,255,255);
        esat::DrawSetFillColor(255,255,255,0);

        DrawAsteroid(asteroid);
        DrawBullet(player->bullet);
        DrawBullet(ufo_bullet); 
        DrawUFO(ufo);
        
        esat::DrawSetFillColor(0,0,0,255);
        DrawPlayer(player, fakeplayer);

        BorderColide(asteroid, ufo, player);  //Where border colides are calculated
        UpdateDrawScore(player);      //Where screen texts are calculates
        esat::DrawEnd();
        //DRAW END

        PlayerRespawn(player, fakeplayer, asteroid, ufo);           //To respawn player 
        Gameplaygamestate(player,asteroid, faketeroid, fakeplayer); //To change round and control some things
        break;
      }
      case GameState::credits:{
        //To init asteroids on that screen
        MenuAsteroids(asteroid,faketeroid);
        UpdateAsteroid(asteroid, faketeroid);
        //To show the scoreboard
        DieScoreBoard(asteroid, faketeroid);
        BorderColide(asteroid, ufo, player);
        break;
      }
      case GameState::error:{
        //TO FREE EVERY MALLOC AND POINTER
        FreeMemory(player, asteroid, faketeroid, fakeplayer, ufo, ufo_bullet, blackhole);
        esat::DrawBegin();
        esat::DrawClear(0,0,0);
        esat::DrawEnd();
        esat::WindowDestroy();
        break;
      }
    }
    esat::WindowFrame();
  }
  esat::WindowDestroy();
  //TO SAVE SCORE FROM PLAYER
  //TO FREE EVERY MALLOC AND POINTER
  if(gamesaved == false){
    SaveScore();
  }
  FreeMemory(player, asteroid, faketeroid, fakeplayer, ufo, ufo_bullet, blackhole);
  printf("\n\nFINALIZADO CORRECTAMENTE\n\n");
  return 0;
}
//FUNCTION THAT INITS MALLOC AND EVERY STATS FROM ASTEROIDS, UFO && BLACKHOLE
void InitGame(Asteroid** asteroid, Faketeroid** faketeroid, Fakeplayer** fakeplayer, UFO** ufo, Bullet** ufo_bullet, BlackHole** blackhole){
  //Init window settings
  esat::WindowInit(kWindowWidth,kWindowHeight);
  esat::WindowSetMouseVisibility(true);
  //Set text size
  esat::DrawSetTextSize(50);
	esat::DrawSetTextFont("../assets/fonts/hyperspace.otf");
  srand(time(NULL));
  //Sql string to make statemets on sqlite
  sql = (char*) OpenPointer(sizeof(char) * 200);
  CheckPointer(sql); //To check that malloc
  //Stings tha shows on the hud the mex score
  max_score.string = (char*) OpenPointer(sizeof(char) * 7);
  CheckPointer(max_score.string);

  //BLACKHOLE INIT
  *blackhole = (BlackHole*) OpenPointer(sizeof(BlackHole) * kNHoles);
  CheckPointer(*blackhole);
  //Initializing Every possible black hole
  for(int e = 0; e < kNHoles; e++){
    (*blackhole + e)->base = (esat::Vec3*) OpenPointer(sizeof(esat::Vec3) * 5);
    (*blackhole + e)->points = (esat::Vec2*) OpenPointer(sizeof(esat::Vec2) * 5);
    (*blackhole + e)->stage = 0;
    (*blackhole + e)->mass = 0;
    float angle = 2 * PI / 5;
    //Setting every pentagon point
    for(int i = 0; i < 5; i++){
      *((*blackhole + e)->base + i) = { cosf(i * angle) * 50,
                                        sinf(i * angle) * 50,
                                        1.0f};
    }
  }
  
  //ASTEROIDS Initializing
  *asteroid = (Asteroid*) OpenPointer(sizeof(Asteroid) * kNAsteroids);
  CheckPointer(*asteroid);
  for(int i = 0; i < kNAsteroids; i++){
    (*asteroid + i)->points = (esat::Vec2*) OpenPointer(sizeof(esat::Vec2) * 12); 
    CheckPointer((*asteroid + i)->points);
    (*asteroid + i)->stage = 0; // 0 -> Dead
    (*asteroid + i)->counter = 0;
  }
  
  //UFO Initializing
  *ufo = (UFO*) OpenPointer(sizeof(UFO) * 1);
  CheckPointer(*ufo);
  (*ufo)->center.x = 0;
  (*ufo)->center.y = 0;
  (*ufo)->stage = 0;
  (*ufo)->points = (esat::Vec2*) OpenPointer(sizeof(esat::Vec2) * 8);
  CheckPointer((*ufo)->points);
  (*ufo)->basepoints = (esat::Vec3*) OpenPointer(sizeof(esat::Vec3) * 8);
  CheckPointer((*ufo)->basepoints);
  //Setting every point of the ufo
  *((*ufo)->basepoints + 0) = {-15, - 3, 1.0f};
  *((*ufo)->basepoints + 1) = {- 6,   3, 1.0f};
  *((*ufo)->basepoints + 2) = {  7,   3, 1.0f};
  *((*ufo)->basepoints + 3) = { 16, - 3, 1.0f};
  *((*ufo)->basepoints + 4) = {  7, - 9, 1.0f};
  *((*ufo)->basepoints + 5) = {  4, -15, 1.0f};
  *((*ufo)->basepoints + 6) = {- 3, -15, 1.0f};
  *((*ufo)->basepoints + 7) = {- 6, - 9, 1.0f};

  //UFO BULLET Initializing
  *ufo_bullet = (Bullet*) OpenPointer(sizeof(Bullet) * kNBullets);
  CheckPointer(*ufo_bullet);
  for(int i = 0; i < kNBullets; i++){
    (*ufo_bullet + i)->active = false;
    (*ufo_bullet + i)->counter = 0;
  }


  //FAKE PLAYER THINGS
  //This is the player that is shown below score and the one that make player death animation
  *fakeplayer = (Fakeplayer*) OpenPointer(sizeof(Fakeplayer) * 6);
  CheckPointer(*fakeplayer);
  int posx = 50, posy = 80;
  for(int i = 0; i < 6; i++){
    (*fakeplayer + i)->points = (float*) OpenPointer(sizeof(float) * 12);
    CheckPointer((*fakeplayer + i)->points);
    //HEAD
    *((*fakeplayer + i)->points + 0) = cosf(((0 * PI) / 180) - 90 * PI / 180) * 18 + posx + i * 30;
    *((*fakeplayer + i)->points + 1) = sinf(((0 * PI) / 180) - 90 * PI / 180) * 18 + posy;
    //SIDES
    *((*fakeplayer + i)->points + 2) = cosf((( 153 * PI) / 180) - 90 * PI / 180) * 25 + posx + i * 30;
    *((*fakeplayer + i)->points + 3) = sinf((( 153 * PI) / 180) - 90 * PI / 180) * 25 + posy;
    *((*fakeplayer + i)->points + 8) = cosf(((-153 * PI) / 180) - 90 * PI / 180) * 25 + posx + i * 30;
    *((*fakeplayer + i)->points + 9) = sinf(((-153 * PI) / 180) - 90 * PI / 180) * 25 + posy;
    //BACK
    *((*fakeplayer + i)->points + 4) = cosf((( 160 * PI) / 180) - 90 * PI / 180) * 18 + posx + i * 30;
    *((*fakeplayer + i)->points + 5) = sinf((( 160 * PI) / 180) - 90 * PI / 180) * 18 + posy;
    *((*fakeplayer + i)->points + 6) = cosf(((-160 * PI) / 180) - 90 * PI / 180) * 18 + posx + i * 30;
    *((*fakeplayer + i)->points + 7) = sinf(((-160 * PI) / 180) - 90 * PI / 180) * 18 + posy;
  }

  //FAKETEROIDS Initializing
  *faketeroid = (Faketeroid*) OpenPointer(sizeof(Faketeroid) * 4);
  CheckPointer(*faketeroid);
  //We have to set every point from every asteroid
  (*faketeroid + 0)->kNpoints = 12;
  (*faketeroid + 1)->kNpoints = 10;
  (*faketeroid + 2)->kNpoints = 12;
  (*faketeroid + 3)->kNpoints = 11;

  //Malloc with their number of points
  for(int i = 0; i < 4; i++){
    (*faketeroid + i)->points = (esat::Vec3*) OpenPointer(sizeof(esat::Vec3) * (*faketeroid + i)->kNpoints);
    CheckPointer((*faketeroid + i)->points);
  }

  //FIRST faketeroid
  *((*faketeroid + 0)->points + 0) = {-13, -40, 1.0f};
  *((*faketeroid + 0)->points + 1) = {-35, -22, 1.0f};
  *((*faketeroid + 0)->points + 2) = {-28, -2, 1.0f};
  *((*faketeroid + 0)->points + 3) = {-40, 18, 1.0f};
  *((*faketeroid + 0)->points + 4) = {-24, 40, 1.0f};
  *((*faketeroid + 0)->points + 5) = {-12, 33, 1.0f};
  *((*faketeroid + 0)->points + 6) = {14, 45, 1.0f};
  *((*faketeroid + 0)->points + 7) = {37, 19, 1.0f};
  *((*faketeroid + 0)->points + 8) = {22, -4, 1.0f};
  *((*faketeroid + 0)->points + 9) = {41, -12, 1.0f};
  *((*faketeroid + 0)->points + 10) = {25, -34, 1.0f};
  *((*faketeroid + 0)->points + 11) = {5, -27, 1.0f};
  //SECOND faketeroid
  *((*faketeroid + 1)->points + 0) = {-6, -40, 1.0f};
  *((*faketeroid + 1)->points + 1) = {-28, -27, 1.0f};
  *((*faketeroid + 1)->points + 2) = {-40, 11, 1.0f};
  *((*faketeroid + 1)->points + 3) = {-28, 36, 1.0f};
  *((*faketeroid + 1)->points + 4) = {0, 46, 1.0f};
  *((*faketeroid + 1)->points + 5) = {33, 36, 1.0f};
  *((*faketeroid + 1)->points + 6) = {36, 13, 1.0f};
  *((*faketeroid + 1)->points + 7) = {44, -3, 1.0f};
  *((*faketeroid + 1)->points + 8) = {32, -28, 1.0f};
  *((*faketeroid + 1)->points + 9) = {9, -16, 1.0f};
  //THIRD faketeroid
  *((*faketeroid + 2)->points + 0) =  { -6, -40, 1.0f};
  *((*faketeroid + 2)->points + 1) =  {-29, -22, 1.0f};
  *((*faketeroid + 2)->points + 2) =  {-40,  11, 1.0f};
  *((*faketeroid + 2)->points + 3) =  {-17,  30, 1.0f};
  *((*faketeroid + 2)->points + 4) =  {-18,  44, 1.0f};
  *((*faketeroid + 2)->points + 5) =  {  8,  49, 1.0f};
  *((*faketeroid + 2)->points + 6) =  {  8,  13, 1.0f};
  *((*faketeroid + 2)->points + 7) =  { 31,  31, 1.0f};
  *((*faketeroid + 2)->points + 8) =  { 39,  24, 1.0f};
  *((*faketeroid + 2)->points + 9) =  { 39, -12, 1.0f};
  *((*faketeroid + 2)->points + 10) = { 21, -35, 1.0f};
  *((*faketeroid + 2)->points + 11) = { 11, -17, 1.0f};
  //FOURTH faketeroid
  *((*faketeroid + 3)->points + 0) = {-8, -40, 1.0f};
  *((*faketeroid + 3)->points + 1) = {-8, -6, 1.0f};
  *((*faketeroid + 3)->points + 2) = {-26, -28, 1.0f};
  *((*faketeroid + 3)->points + 3) = {-40, -18, 1.0f};
  *((*faketeroid + 3)->points + 4) = {-38, 0, 1.0f};
  *((*faketeroid + 3)->points + 5) = {-24, 36, 1.0f};
  *((*faketeroid + 3)->points + 6) = {-8, 48, 1.0f};
  *((*faketeroid + 3)->points + 7) = {28, 30, 1.0f};
  *((*faketeroid + 3)->points + 8) = {34, -14, 1.0f};
  *((*faketeroid + 3)->points + 9) = {16, -10, 1.0f};
  *((*faketeroid + 3)->points + 10) = {22, -30, 1.0f};

}
//This function inits player settings when player selects number of players, 1 or 2
void InitPlayers(Player** player){
  //PLAYER THINGS
  *player = (Player*) OpenPointer(sizeof(Player) * kNPlayers);
  CheckPointer(*player);
  for(int i = 0; i < kNPlayers; i++){
    ((*player) + i)->center.x = kWindowWidth / 2;
    ((*player) + i)->center.y = kWindowHeight / 2;
    ((*player) + i)->speed.x = 0;
    ((*player) + i)->speed.y = 0;
    ((*player) + i)->angle = -90 * PI / 180;
    ((*player) + i)->lives = 4;
    ((*player) + i)->stage = 1;
    ((*player) + i)->counter = 0;
    ((*player) + i)->mainpoints = (float*) OpenPointer(sizeof(float) * 12); //Adding Slots into array
    ((*player) + i)->fire = (float*) OpenPointer(sizeof(float) * 6); //Adding Slots into array
    CheckPointer(((*player) + i)->mainpoints);
    CheckPointer(((*player) + i)->fire);

    //PLAYER BULLET
    ((*player) + i)->bullet = (Bullet*) OpenPointer(sizeof(Bullet) * kNBullets);
    CheckPointer(((*player) + i)->bullet);
    for(int e = 0; e < kNBullets; e++){
      (((*player) + i)->bullet + e)->active = false;
      (((*player) + i)->bullet + e)->counter = 0;
    }
  }
  
  printf("\nPLAYER ALLOCADO\n");

  //Making allocations of the user info
  all_players = (UsuarioInfo*) OpenPointer(sizeof(UsuarioInfo) * 2);
  for(int i = 0; i < 2; i++){
    (all_players + i)->username = (char*) OpenString(12 * sizeof(char));
    CheckPointer((all_players + i)->username); //USERNAME
    (all_players + i)->user_password = (char*) OpenString(12 * sizeof(char));
    CheckPointer((all_players + i)->user_password); //PASSWORD
    (all_players + i)->user_credits = (char*) OpenString(12 * sizeof(char));
    CheckPointer((all_players + i)->user_credits); //CREDITS
    (all_players + i)->maxscore = (char*) OpenString(12 * sizeof(char));
    CheckPointer((all_players + i)->maxscore);   //MAXSCORE
    (all_players + i)->score = (char*) OpenString(12 * sizeof(char));
    CheckPointer((all_players + i)->score);      //SCORE
    (all_players + i)->round = (char*) OpenString(12 * sizeof(char));
    CheckPointer((all_players + i)->round);      //ROUND
    (all_players + i)->kNextLive = 10000;
    //STRINGS FROM SYSTEM
    //Score
    (all_players + i)->plscore.string = (char*) OpenPointer(sizeof(char) * 7);
    (all_players + i)->plscore.number = 0;
    CheckPointer((all_players + i)->plscore.string);
    //Actual round
    (all_players + i)->plactualround.string = (char*) OpenPointer(sizeof(char) * 3);
    (all_players + i)->plactualround.number = 1;
    CheckPointer((all_players + i)->plactualround.string);
    //Credits
    (all_players + i)->plcredits.string = (char*) OpenPointer(sizeof(char) * 3);
    CheckPointer((all_players + i)->plcredits.string);
    //Number of lives
    (all_players + i)->actual_lives = 4;
  }

  userinfo = (all_players + 0);
  AstInGame = 0;
  //Opening files that will swap player games 
  FILE *pfile = fopen("../assets/user1.bin", "w+"); // Abre el archivo
  if (pfile != NULL){
    fwrite(&AstInGame, sizeof(int), 1, pfile);
    fclose(pfile);
  }
  FILE *pfile2 = fopen("../assets/user2.bin", "w+"); // Abre el archivo
  if (pfile != NULL){
    fwrite(&AstInGame, sizeof(int), 1, pfile);
    fclose(pfile);
  }
}
void StartGame(Player* player, Asteroid* asteroid){
}
/*
.______    __          ___   ____    ____  _______ .______      
|   _  \  |  |        /   \  \   \  /   / |   ____||   _  \     
|  |_)  | |  |       /  ^  \  \   \/   /  |  |__   |  |_)  |    
|   ___/  |  |      /  /_\  \  \_    _/   |   __|  |      /     
|  |      |  `----./  _____  \   |  |     |  |____ |  |\  \----.
| _|      |_______/__/     \__\  |__|     |_______|| _| `._____|
*/
//Function that updates every player point to its actual location
void UpdatePlayer(Player* player, float posx, float posy){
  if(player->stage == 1){
    //Adding the speed to the center of the player to move
    player->center.x += player->speed.x;
    player->center.y += player->speed.y;

    //HEAD
    *(player->mainpoints + 0) = cosf(((0 * PI) / 180) + player->angle) * 18 + posx;
    *(player->mainpoints + 1) = sinf(((0 * PI) / 180) + player->angle) * 18 + posy;

    //SIDES
    *(player->mainpoints + 2) = cosf((( 153 * PI) / 180) +  player->angle) * 25 + posx;
    *(player->mainpoints + 3) = sinf((( 153 * PI) / 180) + player->angle) * 25 + posy;
    *(player->mainpoints + 8) = cosf(((-153 * PI) / 180) + player->angle) * 25 + posx;
    *(player->mainpoints + 9) = sinf(((-153 * PI) / 180) + player->angle) * 25 + posy;

    //BACK
    *(player->mainpoints + 4) = cosf((( 160 * PI) / 180) + player->angle) * 18 + posx;
    *(player->mainpoints + 5) = sinf((( 160 * PI) / 180) + player->angle) * 18 + posy;
    *(player->mainpoints + 6) = cosf(((-160 * PI) / 180) + player->angle) * 18 + posx;
    *(player->mainpoints + 7) = sinf(((-160 * PI) / 180) + player->angle) * 18 + posy;
    //Center point to some colides calculations
    *(player->mainpoints + 10) = player->center.x;
    *(player->mainpoints + 11) = player->center.y;
    //Only calculate fire points if player is going forward
    if(esat::IsKeyPressed('W')){
      //FIRE
      *(player->fire + 0) = cosf((( 164 * PI) / 180) + player->angle) * 18 + posx;
      *(player->fire + 1) = sinf((( 164 * PI) / 180) + player->angle) * 18 + posy;
      *(player->fire + 2) = cosf((( 180 * PI) / 180) + player->angle) * 25 + posx;
      *(player->fire + 3) = sinf((( 180 * PI) / 180) + player->angle) * 25 + posy;
      *(player->fire + 4) = cosf(((-164 * PI) / 180) + player->angle) * 18 + posx;
      *(player->fire + 5) = sinf(((-164 * PI) / 180) + player->angle) * 18 + posy;
    }
  }
}
//Function that draws player and draws & calcs fakeplayer on the top corners
void DrawPlayer(Player* player, Fakeplayer* fakeplayer){
  if(player->stage == 1){    
  
    //DRAW SHIP 
    esat::DrawSolidPath(player->mainpoints, 5);
  
    //DRAW FIRE only if is pressed 'w' and the frame is odd
    if(esat::IsKeyPressed('W') && (frameCounter % 2 == 0)){
      esat::DrawSolidPath(player->fire, 3);
    }
  }  

  PlayerDying(player, fakeplayer);
  float pos_player = 0;
  for(int e = 0; e < 2; e++){
    //Depending of e the fakeplayer is drawn on top left or top right, in each frame is drawn on both
    if(e == 0) {
      pos_player = 50;
    }else{
      pos_player = 890;
    }
    for(int i = 0; i < (all_players + e)->actual_lives - 1; i++){
      //HEAD
      *((fakeplayer + i)->points + 0) = cosf(((0 * PI) / 180) - 90 * PI / 180) * 18 + pos_player + i * 30;
      *((fakeplayer + i)->points + 1) = sinf(((0 * PI) / 180) - 90 * PI / 180) * 18 + 80;
      //SIDES
      *((fakeplayer + i)->points + 2) = cosf((( 153 * PI) / 180) - 90 * PI / 180) * 25 + pos_player + i * 30;
      *((fakeplayer + i)->points + 3) = sinf((( 153 * PI) / 180) - 90 * PI / 180) * 25 + 80;
      *((fakeplayer + i)->points + 8) = cosf(((-153 * PI) / 180) - 90 * PI / 180) * 25 + pos_player + i * 30;
      *((fakeplayer + i)->points + 9) = sinf(((-153 * PI) / 180) - 90 * PI / 180) * 25 + 80;
      //BACK
      *((fakeplayer + i)->points + 4) = cosf((( 160 * PI) / 180) - 90 * PI / 180) * 18 + pos_player + i * 30;
      *((fakeplayer + i)->points + 5) = sinf((( 160 * PI) / 180) - 90 * PI / 180) * 18 + 80;
      *((fakeplayer + i)->points + 6) = cosf(((-160 * PI) / 180) - 90 * PI / 180) * 18 + pos_player + i * 30;
      *((fakeplayer + i)->points + 7) = sinf(((-160 * PI) / 180) - 90 * PI / 180) * 18 + 80;

      *((fakeplayer + i)->points + 10) = cosf(((0 * PI) / 180) - 90 * PI / 180) * 18 + pos_player + i * 30;
      *((fakeplayer + i)->points + 11) = sinf(((0 * PI) / 180) - 90 * PI / 180) * 18 + 80;
      //Draw this fakeplayer
      esat::DrawSolidPath((fakeplayer + i)->points, 5);
    }
  }
}
/*
 _____                  _   
|_   _|                | |  
  | | _ __  _ __  _   _| |_ 
  | || '_ \| '_ \| | | | __|
 _| || | | | |_) | |_| | |_ 
 \___/_| |_| .__/ \__,_|\__|
           | |              
           |_|              
*/
//Function that gets every input from the player ingame
void InputPlayer(Player* player){
  //Only if player is alive
  if(player->stage == 1){
    //If !pressed W player speed is reduced on both, x and y
    if(player->speed.x != 0 && !esat::IsKeyPressed('W')){
      player->speed.x *= 0.965;
    }
    if(player->speed.y != 0 && !esat::IsKeyPressed('W')){
      player->speed.y *= 0.965;
    }
    //Controlls to go left or right
    if(esat::IsKeyPressed('A')){
      player->angle -= 0.065;
    }
    if(esat::IsKeyPressed('D')){
      player->angle += 0.065;
    }
    //To fo forward when is pressed w there is a limit on speed of 15
    if(esat::IsKeyPressed('W')){
      if(player->speed.x < 15 && player->speed.x > -15){
        player->speed.x += cosf(player->angle) * 0.1;
      }
      if(player->speed.y < 15 && player->speed.y > -15){
        player->speed.y += sinf(player->angle) * 0.1;
      }
    }
    //Key to shoot
    if(esat::IsSpecialKeyDown(esat::kSpecialKey_Space)){
      //This function searches the first of the 5 player bullets that is dead and 
      //it gives it the correct stats
      int ArrowCounter = 0;
      //This is to search the first bullet
      while((player->bullet + ArrowCounter)->active != false && ArrowCounter < kNBullets -1){
        ArrowCounter++;
      }
      //If that bullet found is really dead
      if((player->bullet + ArrowCounter)->active == false){
        (player->bullet + ArrowCounter)->active = true;
        DefineBullet(player, (player->bullet + ArrowCounter));
      }
    }
    //Key to teleport, also the is a added chance that player can die if he pressed
    if(esat::IsKeyDown('G')){
      player->center.x = (rand() % (kWindowWidth - 200)) + 100;
      player->center.y = (rand() % (kWindowHeight - 200)) + 100;
      player->angle = rand() % 360 * PI / 180;
      if(rand() % 20 == 0){
        player->lives--;
        player->stage = 2;
      }
    }
  }
}
//Function to respawn player, it checks if player->lives has changed
void PlayerRespawn(Player* player, Fakeplayer* fakeplayer, Asteroid* asteroid, UFO* ufo){
  static int lastlive = 4;
  if(player->lives != lastlive){
    userinfo->actual_lives = player->lives;
    player->stage = 2;
    player->counter++;
    //When player looses a live the function starts a counter
    //If player lives are 0 and the counter has maked 2 secs game ends
    

    //If there is any asteroid near player he won't respawn
    int asteroid_near = 0;
    for(int i = 0; i < kNAsteroids; i++){
      if((asteroid + i)->stage > 0 && (asteroid + i)->stage < 4){
        float vec1 = (asteroid + i)->center.x - player->center.x;
        float vec2 = (asteroid + i)->center.y - player->center.y;
        //It checks if there is any asteroid near 100 pixels
        if(sqrtf(vec1 * vec1 + vec2 * vec2) < 120){
          asteroid_near++;
        }
      }
    }


    //If timer is up to 2 secs and there is no asteroids -> player revives
    if(player->counter >= 120 && asteroid_near == 0){
      lastlive = player->lives;
      player->counter = 0;
      player->stage = 1;
      player->angle = -90 * PI / 180;
      player->center.x = kWindowWidth >> 1;
      player->center.y = kWindowHeight >> 1;
      player->speed.x = 0;
      player->speed.y = 0;
      //If there is 2 players players are swiched
      if(kNPlayers == 2){
        SwitchPlayers();
        player->lives = userinfo->actual_lives;
        lastlive = player->lives;
        SwitchPlayerstats(asteroid, ufo);
      }

      if(player->lives == 0 && player->counter == 0){
        //If all this conditions game ends and both player have 1 credit less
        for(int i = 0; i < kNPlayers; i++){
          (all_players + i)->plcredits.number--;
          snprintf((all_players + i)->plcredits.string, sizeof(char) * 3, "%d",(all_players + i)->plcredits.number);
          (all_players + i)->plactualround.number = 1;
        }
        InsertScore();
        KillAsteroids(asteroid);
        gamestate = GameState::credits;
      }
    }
    //If counter = 1 player dieing animation starts, 
    //player points are copied on fakeplayer 6 (5) and DrawPlayer will begin the drawing
    if(player->counter == 1){
      for(int i = 0; i < 10; i++){
        //HEAD
        *((fakeplayer + 5)->points + i) = *(player->mainpoints + i);
      }
      *((fakeplayer + 5)->points + 10) = *(player->mainpoints + 0);
      *((fakeplayer + 5)->points + 11) = *(player->mainpoints + 1);
    }
  }
  //This part will give player extra live if he gets 10000 points, and then every 10k
  if(userinfo->plscore.number >= userinfo->kNextLive){
    userinfo->kNextLive += 10000;
    if(player->lives <= 5){
      player->lives++;
      userinfo->actual_lives++;
    }
  }
}
//Function that calc every colide with player and asteroid (Not bullet to asteroid)
void PlayerColides(Player* player, Asteroid* asteroid, Faketeroid* faketeroid){
  if(player->stage == 1){//If player is alive, for every asteroid if they are alive
    for(int i = 0; i < kNAsteroids; i++){
      for(int e = 0; e < 6; e++){ //For every player point
        //Then it calcs if that asteroid is near a distance, depending on the size of it
        //For big asteroids 50 px medium 25 px and small 15 px
        if((asteroid + i)->stage > 0 && (asteroid + i)->stage < 4 && (asteroid + i)->stage < 4){
          float vec1 = (asteroid + i)->center.x - *(player->mainpoints + e * 2);
          float vec2 = (asteroid + i)->center.y - *(player->mainpoints + e * 2 + 1);
          float distance = sqrtf(vec1 * vec1 + vec2 * vec2);
          bool near = false;
          bool colide = false;

          switch((asteroid + i)->stage){
            case 1:{
              if(distance <= 50){
                near = true;
              }
              break;
            }
            case 2:{
              if(distance <= 25){
                near = true;
              }
              break;
            }
            case 3:{
              if(distance <= 15){
                near = true;

              }
              break;
            }
          }
          //If its near that distance enters on the calculation fuction
          if(near == true){
            colide = AsteroidTriangulate((asteroid + i), (asteroid + i)->type, *(player->mainpoints + e * 2),*(player->mainpoints + e * 2 + 1));
            if(colide == true){
              //If its inside it takes one live of the player and he starts stage 2
              //One asteroid is killed and the function AsteroidOblibion 
              //Gives live to 2 child asteroids of that one
              //Then by multiplying the stage x10 starts explosion animation on asteroid
              player->lives--;
              player->stage = 2;
              AstInGame--;
              AsteroidOblivion(asteroid, faketeroid, (asteroid + i)->stage, (asteroid + i)->center.x, (asteroid + i)->center.y);
              (asteroid + i)->stage *= 10;
              (asteroid + i)->counter++;
            }
          } 
        }
      } 
    }
  }
}
//Function that draws player dieing and calcs every point for it
//The function player respawn will copy player points on fakeplater 4
void PlayerDying(Player* player, Fakeplayer* fakeplayer){
  if(player->stage == 2 && player->counter < 50){
    //Player->stage 2 -> animation dieing and it lasts 50 ticks, almost 1 sec
    esat::DrawLine(*((fakeplayer + 5)->points + 0),*((fakeplayer + 5)->points + 1),
                  *((fakeplayer + 5)->points + 2),*((fakeplayer + 5)->points + 3));
    esat::DrawLine(*((fakeplayer + 5)->points + 10),*((fakeplayer + 5)->points + 11),
                  *((fakeplayer + 5)->points + 8),*((fakeplayer + 5)->points + 9));
    esat::DrawLine(*((fakeplayer + 5)->points + 4),*((fakeplayer + 5)->points + 5),
                  *((fakeplayer + 5)->points + 6),*((fakeplayer + 5)->points + 7)); 

    //Function separates every point to make illusion            
    *(((fakeplayer + 5)->points) + 0) += 0.5;
    *(((fakeplayer + 5)->points) + 1) += 0.3;
    *(((fakeplayer + 5)->points) + 2) += 0.5;
    *(((fakeplayer + 5)->points) + 3) += 0.4;

    *(((fakeplayer + 5)->points) + 10) -= 0.5;
    *(((fakeplayer + 5)->points) + 11) -= 0.4;
    *(((fakeplayer + 5)->points) + 8) += 0.3;
    *(((fakeplayer + 5)->points) + 9) += 0.2;

    *(((fakeplayer + 5)->points) + 4) -= 0.3;
    *(((fakeplayer + 5)->points) + 6) += 0.5;
    *(((fakeplayer + 5)->points) + 6) -= 0.2;
    *(((fakeplayer + 5)->points) + 7) += 0.1;
  }
}
/*
.______    __    __   __       __       _______ .___________.
|   _  \  |  |  |  | |  |     |  |     |   ____||           |
|  |_)  | |  |  |  | |  |     |  |     |  |__   `---|  |----`
|   _  <  |  |  |  | |  |     |  |     |   __|      |  |     
|  |_)  | |  `--'  | |  `----.|  `----.|  |____     |  |     
|______/   \______/  |_______||_______||_______|    |__|                                                                 
*/
//This function gives selected bullet player points and the necesary speed
void DefineBullet(Player* player, Bullet* bullet){
  bullet->center.x = *(player->mainpoints + 0);
  bullet->center.y = *(player->mainpoints + 1);

  //Then calculates the angle that has the player to go forward that direction
  bullet->speed.x = cosf(player->angle) * 15;
  bullet->speed.y = sinf(player->angle) * 15;
  bullet->counter = 0;
  //And sets its counter to 0
}

void UpdateBullet(Bullet* bullet){
  //For every bullet it calcs its new position by adding speed each frame
  for(int i = 0; i < kNBullets; i++){
    if((bullet + i)->active == 1){
      (bullet + i)->center.x += (bullet + i)->speed.x;
      (bullet + i)->center.y += (bullet + i)->speed.y;
      (bullet + i)->counter++;
      //And add +1 counter every frame so when timer is 60 bullet dies
      if((bullet + i)->counter > 60){
        (bullet + i)->active = false;
        (bullet + i)->counter = 0;
      }
    }
  }
}
//There are 8 fake points to every bullet that are drawn on
void DrawBullet(Bullet* bullet){
  for(int i = 0; i < kNBullets; i++){
    if((bullet + i)->active == 1){
      //each of that fake point is copied around center of the bullet and then is painted
      *(bullet_points + 0) = (bullet + i)->center.x - 1;
      *(bullet_points + 1) = (bullet + i)->center.y;
      *(bullet_points + 2) = (bullet + i)->center.x;
      *(bullet_points + 3) = (bullet + i)->center.y + 1;
      *(bullet_points + 4) = (bullet + i)->center.x + 1;
      *(bullet_points + 5) = (bullet + i)->center.y;
      *(bullet_points + 6) = (bullet + i)->center.x;
      *(bullet_points + 7) = (bullet + i)->center.y - 1;
      //Then is drawn 2 times to make it lighter
      esat::DrawSolidPath(bullet_points, 4);
      esat::DrawSolidPath(bullet_points, 4);
    }
  }
}
/*   ___           _______.___________. _______ .______        ______    __   _______       _______.
    /   \         /       |           ||   ____||   _  \      /  __  \  |  | |       \     /       |
   /  ^  \       |   (----`---|  |----`|  |__   |  |_)  |    |  |  |  | |  | |  .--.  |   |   (----`
  /  /_\  \       \   \       |  |     |   __|  |      /     |  |  |  | |  | |  |  |  |    \   \    
 /  _____  \  .----)   |      |  |     |  |____ |  |\  \----.|  `--'  | |  | |  '--'  |.----)   |   
/__/     \__\ |_______/       |__|     |_______|| _| `._____| \______/  |__| |_______/ |_______/                                                                                                       
*/
//Function that is called each start of new round to generate big asteroids
void AsteroidGeneration(Asteroid* asteroid, Faketeroid* faketeroid){
  //It searched for the first dead asteroid and then gives him every value
  int counter = 0;
  while((asteroid + counter)->stage != 0 && counter < kNAsteroids - 1){
    counter++;
  }
  if((asteroid + counter)->stage == 0){
    //Then we add a asteoid to AstInGame and give him all stats
    AstInGame++;
    (asteroid + counter)->stage = 1;
    (asteroid + counter)->counter = 0;

    int side = rand() % 4; //Here we select the side he will spawn
    float dir = (((rand() % 2) * 2) -1);  //And the direction he will follow
    //Little abreviation for the asteroid
    Asteroid *asteroi = asteroid + counter;

    // Range of speed betwwen 2 - 4
    float speedX = ((rand() % 200) + 200) / 100.0f;
    float speedY = ((rand() % 200) + 200) / 100.0f;

    switch(side){
      case 0:{ //If he spawns on top
        asteroi->center.x = rand() % kWindowWidth;
        asteroi->center.y = -50;
        asteroi->speed.x = speedX * dir;
        asteroi->speed.y = speedY;
        break;
      }
      case 1:{ //If he spawns on the bottom
        asteroi->center.x = rand() % kWindowWidth;
        asteroi->center.y = kWindowHeight + 50;
        asteroi->speed.x = speedX * dir;
        asteroi->speed.y = -speedY;
        break;
      }
      case 2:{ //If he spawns on left
        asteroi->center.x = -50;
        asteroi->center.y = rand() % kWindowHeight;
        asteroi->speed.x = speedX;
        asteroi->speed.y = speedY * dir;
        break;
      }
      case 3:{ //If he spawns on right
        asteroi->center.x = kWindowWidth + 50;
        asteroi->center.y = rand() % kWindowHeight;
        asteroi->speed.x = -speedX;
        asteroi->speed.y = speedY * dir;
        break;
      }
    }
    //Type, angle in wich appears and number of points of that type
    (asteroid + counter)->type = rand() % 4;
    (asteroid + counter)->angle = (rand() % 360) * PI / 180;
    (asteroid + counter)->kNpoints = (faketeroid + (asteroid + counter)->type)->kNpoints;
  }
}
//Function that puts every asteroid point where corresponds and translate every asteroid
//Also makes the explosion animation
void UpdateAsteroid(Asteroid* asteroid, Faketeroid* faketeroid){
  //For every asteorid
  for(int i = 0; i < kNAsteroids; i++){
    if((asteroid + i)->stage > 0 && (asteroid + i)->stage < 4){
      //If he is in stage between 1 - 3
      (asteroid + i)->center.x += (asteroid + i)->speed.x; //Moving ast on x 
      (asteroid + i)->center.y += (asteroid + i)->speed.y; //Moving ast on y
      //This part calcs with math every point with the correct size and puts it in 
      //-> asteroid->points
      esat::Mat3 m = esat::Mat3Identity();
      switch ((asteroid + i)->stage) {
        //Here is the scale for every asteroid stage
        case 1: {
          m = esat::Mat3Multiply(esat::Mat3Scale(1.0f, 1.0f), m);
          break;
        }
        case 2: {
          m = esat::Mat3Multiply(esat::Mat3Scale(0.5f, 0.5f), m);
          break;
        }
        case 3: {
          m = esat::Mat3Multiply(esat::Mat3Scale(0.30f, 0.30f), m);
          break;
        }
      }
      //This rotates it with their angle
      m = esat::Mat3Multiply(esat::Mat3Rotate((asteroid + i)->angle * PI / 180), m);
      //this translates the points to the center of the ast
      m = esat::Mat3Multiply(esat::Mat3Translate((asteroid + i)->center.x, (asteroid + i)->center.y), m);
      //this multiplies the matrix created into the points
      for (int e = 0; e < (asteroid + i)->kNpoints; e++) {
        esat::Vec3 tmp = esat::Mat3TransformVec3(m, *((faketeroid + (asteroid + i)->type)->points + e));
        *((asteroid + i)->points + e) = { tmp.x, tmp.y };
      }
    }else{
      //ASTEROID EXPLOSION ANIMATION
      //When an asteroid dies its stage is multiplied x10 so every asteroid with
      //stage over 10 will be exploding
      //With their counter we calculate its size of explosion each frame
      //Then we multiply by its size
      if((asteroid + i)->stage >= 10){
        esat::Mat3 m = esat::Mat3Identity();
        double aumentasion = (asteroid + i)->counter / 10;
        switch ((asteroid + i)->stage) {
          case 10: {
            m = esat::Mat3Multiply(esat::Mat3Scale(aumentasion * 0.8, aumentasion * 0.8), m);
            break;
          }
          case 20: {
            m = esat::Mat3Multiply(esat::Mat3Scale(aumentasion * 0.5, aumentasion * 0.5), m);
            break;
          }
          case 30: {
            m = esat::Mat3Multiply(esat::Mat3Scale(aumentasion * 0.3 ,aumentasion * 0.3), m);
            break;
          }
        }
        m = esat::Mat3Multiply(esat::Mat3Rotate(0.0f), m);
        m = esat::Mat3Multiply(esat::Mat3Translate((asteroid + i)->center.x, (asteroid + i)->center.y), m);
        //Then we apply that matrix to the points
        for (int e = 0; e < (asteroid + i)->kNpoints; e++) {
          esat::Vec3 tmp = esat::Mat3TransformVec3(m, *((faketeroid + (asteroid + i)->type)->points + e));
          *((asteroid + i)->points + e) = { tmp.x, tmp.y };
        }
      }
    }
  }
}
//Function that draws asteroids
void DrawAsteroid(Asteroid* asteroid){
  for(int i = 0; i < kNAsteroids; i++){
    //Simply it draws a solid path trough every asteroid with their number of points
    if((asteroid + i)->stage > 0 && (asteroid + i)->stage < 4){
      esat::DrawSolidPath( &(asteroid + i)->points->x, (asteroid + i)->kNpoints, true);
    }
  }
}
//This function makes asteroid children by inserting its position and stage
void AsteroidOblivion(Asteroid* asteroid,Faketeroid* faketeroid, int father_stage, float centerx, float centery){ 
  for(int k = 0; k < 2; k++){ //For makes 2 iterations, 1 per child
    //As always function searches for the first dead asteroid and insert all stats on him
    int counter = 0;
    while((asteroid + counter)->stage != 0 && counter < kNAsteroids - 1){
      counter++;
    }
    if((asteroid + counter)->stage == 0){
      if(father_stage != 3){
        //If father stage is 3 so asteroid is small we dont generate new ones
        (asteroid + counter)->stage = father_stage + 1; //Making new ast smaller
        (asteroid + counter)->counter = 0;
        AstInGame++; //Adding 1 ast to the game

        (asteroid + counter)->speed.x = (((float)(rand() % 200) + 150.0f) / 100.0f) * (((rand() % 2) * 2) - 1);
        (asteroid + counter)->speed.y = (((float)(rand() % 200) + 150.0f) / 100.0f) * (((rand() % 2) * 2) - 1);
        //This one insert a rand of float to make the speed a float 

        (asteroid + counter)->center.x = centerx; //The center of the 2 must be the center of the father
        (asteroid + counter)->center.y = centery;
  
        (asteroid + counter)->type = rand() % 4; //Setting its type and number of points of that one
        (asteroid + counter)->kNpoints = (faketeroid + (asteroid + counter)->type)->kNpoints;
      }
    }
  }
}
//This function paints the animation explosion by not painting lines
//It paints every point of the ast individually
void AsteroidExplosion(Asteroid* asteroid,Faketeroid* faketeroid){
  for(int i = 0; i < kNAsteroids; i++){
    if((asteroid + i)->counter > 0 && (asteroid + i)->stage > 4){
      (asteroid + i)->counter++;
      //this increases the counter that increases the size of the explosion
      //If timer gets to 30 (Half a sec) it ends and ast dies oficially
      if((asteroid + i)->counter < 30){
        for(int e = 0; e < (asteroid + i)->kNpoints; e++){
          esat::DrawLine(((asteroid + i)->points + e)->x, ((asteroid + i)->points + e)->y,
                         ((asteroid + i)->points + e)->x + 1,((asteroid + i)->points + e)->y + 1);
        }
      }else{
        (asteroid + i)->counter = 0;
        (asteroid + i)->stage = 0;
      }
    }
  }
}
//This function calcs every Bullet -> Asteroid colide
void AsteroidColides(Asteroid* asteroid,Faketeroid* faketeroid, Bullet* ufo_bullet, Player* player){
  for(int e = 0; e < kNBullets; e++){
    for(int i = 0; i < kNAsteroids; i++){
      //For every asteroid and bullet if both of them are alive
      if((asteroid + i)->stage > 0 && (asteroid + i)->stage < 4 && (player->bullet + e)->active != false){
        float distance = sqrtf(((asteroid + i)->center.x - (player->bullet + e)->center.x) * ((asteroid + i)->center.x - (player->bullet + e)->center.x) 
                    + ((asteroid + i)->center.y - (player->bullet + e)->center.y) * ((asteroid + i)->center.y - (player->bullet + e)->center.y));
        bool near = false;
        switch((asteroid + i)->stage){
          case 1:{
            if(distance <= 50) near = true;
            break;
          }
          case 2:{
            if(distance <= 25) near = true;
            break;
          }
          case 3:{
            if(distance <= 15) near = true;
            break;
          }
        }
        //Again above here we calc the distance, if bullet is near 50 px con big ast, 25 px mid ast and 15 small
        bool colide = false;
        if(near == true){
          //If is near we calc the colide with AsteroidTriangulate
          //For stage 1 and 2 we create to chil ast and substract 80 and 50 points cause everytime we add 100
          //So on stage 3 will be added 100 points and no substracted anything and no children
          colide = AsteroidTriangulate(asteroid + i, (asteroid + i)->type, (player->bullet + e)->center.x,(player->bullet + e)->center.y);
          if(colide == true){
            switch((asteroid + i)->stage){
              case 1:{
                AsteroidOblivion(asteroid, faketeroid, (asteroid + i)->stage, (asteroid + i)->center.x, (asteroid + i)->center.y);
                userinfo->plscore.number -= 80;
                break;
              }
              case 2:{
                AsteroidOblivion(asteroid, faketeroid, (asteroid + i)->stage, (asteroid + i)->center.x, (asteroid + i)->center.y);
                userinfo->plscore.number -= 50;
                break;
              }
            }
            //Bullet is killed and ast explosion starts
            userinfo->plscore.number += 100;
            (player->bullet + e)->active = 3;
            (asteroid + i)->stage *= 10;
            (asteroid + i)->counter++;
            AstInGame--;
          }
        } 
      }
      //THIS IS EXACT THE SAME FOR UFO BULLET WITHOU ADDING SCORE
      if((asteroid + i)->stage > 0 && (asteroid + i)->stage < 4 && (ufo_bullet + e)->active != false){
        float distance = sqrtf(((asteroid + i)->center.x - (ufo_bullet + e)->center.x) * ((asteroid + i)->center.x - (ufo_bullet + e)->center.x) 
                    + ((asteroid + i)->center.y - (ufo_bullet + e)->center.y) * ((asteroid + i)->center.y - (ufo_bullet + e)->center.y));
        bool near = false;
        switch((asteroid + i)->stage){
          case 1:{
            if(distance <= 50){
              near = true;
            }
            break;
          }
          case 2:{
            if(distance <= 25){
              near = true;
            }
            break;
          }
          case 3:{
            if(distance <= 15){
              near = true;
            }
            break;
          }
        }

        bool colide = false;
        if(near == true){
          colide = AsteroidTriangulate(asteroid + i, (asteroid + i)->type, (ufo_bullet + e)->center.x,(ufo_bullet + e)->center.y);
          if(colide == true){
            switch((asteroid + i)->stage){
              case 1:{
                AsteroidOblivion(asteroid, faketeroid, (asteroid + i)->stage, (asteroid + i)->center.x, (asteroid + i)->center.y);
                break;
              }
              case 2:{
                AsteroidOblivion(asteroid, faketeroid, (asteroid + i)->stage, (asteroid + i)->center.x, (asteroid + i)->center.y);
                break;
              }
            }
            (ufo_bullet + e)->active = 3;
            (asteroid + i)->stage *= 10;
            (asteroid + i)->counter++;
            AstInGame--;
          }
        } 
      } 
    }
    if((player->bullet + e)->active == 3){
      (player->bullet + e)->active = 0;
    }
    if((ufo_bullet + e)->active == 3){
      (ufo_bullet + e)->active = 0;
    }
  }
}
void KillAsteroids(Asteroid* asteroid){
  for(int i = 0; i < kNAsteroids; i++){
    if((asteroid + i)->stage > 0){
      (asteroid + i)->stage = 0;
    }
  }
}
/* __    __   _______   ______
  |  |  |  | |   ____| /  __  \ 
  |  |  |  | |  |__   |  |  |  |
  |  |  |  | |   __|  |  |  |  |
  |  '--'  | |  |     |  '--'  |
   \______/  |__|      \______/ 
*/
//Function that generates ufo
void UFOGeneration(UFO* ufo, Player* player){
  //First, if player hasn't shoot for a while it starts a counter to start a ufo
  int dead_bullet = 0;
  for(int i = 0; i < kNBullets; i++){
    if((player->bullet + i)->active != 0){
      dead_bullet++;
    }
  }
  //Or if there are less than 3 ast in game it will be a chance to spawn ufo
  if((AstInGame < 3 && rand() % 800 == 0 && gamecounter == 0)
  || (dead_bullet == 0 && rand() % 1500 == 0 && gamecounter == 0)){
    if(ufo->stage == 0){
      //Function gives a stage 1 -> big ufo || 2 -> small ufo
      ufo->stage = (rand() % 2) + 1;
      //if ufo stage is 2 (small one) he will go faster
      ufo->speed.x = (((rand() % 2) * 2) - 1) * ufo->stage * 1.5;
      ufo->speed.y = ((rand() % 3) - 1) * 0.4;
      //Deciding which side to appear
      if(ufo->speed.x > 0){
        ufo->center.x = -50;
      }else{
        ufo->center.x = kWindowWidth + 50;
      }
      ufo->center.y = rand() % (kWindowWidth - 400) + 200;
      
    }
  }
}
//Function that draws every ufo point
void DrawUFO(UFO* ufo){
  if(ufo->stage > 0){
    //First it draws the silluete and then 2 lines that transvers the ufo
    esat::DrawSolidPath(&ufo->points->x, 8);
    esat::DrawLine((ufo->points + 0)->x,(ufo->points + 0)->y,(ufo->points + 3)->x,(ufo->points + 3)->y);
    esat::DrawLine((ufo->points + 7)->x,(ufo->points + 7)->y,(ufo->points + 4)->x,(ufo->points + 4)->y);
  }
}
//Function that moves ufo and sets its points
void UpdateUFO(UFO* ufo){
  if(ufo->stage > 0){
    //UFO MOVEMENT
    //little change con y movement sometimes
    if(rand() % 420 == 0){
      ufo->speed.y = ((rand() % 3) - 1) * 0.3;
    }
    //direction swap
    if(rand() % 600 == 0 && ufo->center.x < 800 && ufo->center.x > 300){
      ufo->speed.x = -ufo->speed.x;
    }
    //Moving the ufo con x and y
    ufo->center.x += ufo->speed.x;
    ufo->center.y += ufo->speed.y;
    //This part cals every point and its size on its center
    esat::Mat3 m = esat::Mat3Identity();
    switch (ufo->stage) {
      case 1: {
        m = esat::Mat3Multiply(esat::Mat3Scale(2.5f, 2.5f), m);
        break;
      }
      case 2: {
        m = esat::Mat3Multiply(esat::Mat3Scale(1.5f, 1.5f), m);
        break;
      }
    }
    m = esat::Mat3Multiply(esat::Mat3Rotate(0.0f), m);
    m = esat::Mat3Multiply(esat::Mat3Translate(ufo->center.x, ufo->center.y), m);
    //Then we add the matrix created to each point
    for (int e = 0; e < 8; e++){
      esat::Vec3 tmp = esat::Mat3TransformVec3(m, *(ufo->basepoints + e));
      *(ufo->points + e) = { tmp.x, tmp.y};
    }
  }
}
//Function that calcs attacks on the UFO
void UFOAttacks(UFO* ufo, Bullet* ufo_bullet, Player* player){
  if(ufo->stage > 0){
    //If ufo its alive counter++
    ufo->counter++;
    //Each shot hast to wait 1 sec and a half to be and if the rand % 120 is 0 it created a bullet
    //As always it searches for the first dead bullet
    if(rand() % 120 == 0 && ufo->counter >= 100){
      ufo->counter = 0;
      int ArrowCounter = 0;
      while((ufo_bullet + ArrowCounter)->active != false && ArrowCounter < kNBullets -1){
        ArrowCounter++;
      }
      //Depending on the ufo type, bullet will go everywhere or will go to the player
      if((ufo_bullet + ArrowCounter)->active == false){
        switch(ufo->stage){
          case 1:{
            //Here we give bullet center on ufo center
            (ufo_bullet + ArrowCounter)->center.x = ufo->center.x;
            (ufo_bullet + ArrowCounter)->center.y = ufo->center.y;
    
            //Bullet direction follows goes with no intention
            (ufo_bullet + ArrowCounter)->speed.x = cosf(rand() % 3600) * 15;
            (ufo_bullet + ArrowCounter)->speed.y = sinf(rand() % 3600) * 15;
            (ufo_bullet + ArrowCounter)->counter = 0;
            (ufo_bullet + ArrowCounter)->active = true;
            break;
          }
          case 2:{
            /*If ufo is the small one bullet -> player direction is calculated and
            setted on this bullet with a little bloom to not to be a perfect shot*/
            float vec1 = player->center.x - ufo->center.x;
            float vec2 = player->center.y - ufo->center.y;
            float normalice = sqrtf(vec1 * vec1 + vec2 * vec2);
            float bloom = rand() % 1000 * 0.005;
            (ufo_bullet + ArrowCounter)->center.x = ufo->center.x;
            (ufo_bullet + ArrowCounter)->center.y = ufo->center.y;
    
            (ufo_bullet + ArrowCounter)->speed.x = vec1 / normalice * 15 + bloom;
            (ufo_bullet + ArrowCounter)->speed.y = vec2 / normalice * 15 + bloom;
            (ufo_bullet + ArrowCounter)->counter = 0;
            (ufo_bullet + ArrowCounter)->active = true;
            break;
          }
        }
      }
    }
  }
}
//Here are the calcs between player->bullet shoting ufo   &&   ufo->bullet shooting player
void UFOColides(UFO* ufo, Bullet* ufo_bullet, Player* player){
  if(ufo->stage > 0){
    //For every bullet if ufo is alive
    for(int e = 0; e < kNBullets; e++){
      if(ufo->stage > 0 && (player->bullet + e)->active != false){
        //Function calcs if bullet is near x distance depending on ufo size
        float distance = sqrtf((ufo->center.x - (player->bullet + e)->center.x) * (ufo->center.x - (player->bullet + e)->center.x) 
                    + (ufo->center.y - (player->bullet + e)->center.y) * (ufo->center.y - (player->bullet + e)->center.y));
        bool near = false;
        switch(ufo->stage){
          case 1:{
            if(distance <= 50){
              near = true;
            }
            break;
          }
          case 2:{
            if(distance <= 25){
              near = true;
            }
            break;
          }
        }
        /*If bullet is near ufo it starts the calcs, function calcs distance from ufo each line to 
        bullet, if taht distance is negativa it add 1 to a count if in that count every line has 
        negative distance bullet will be inside*/

        if(near == true){
          //This calcs two perimeters on the ufo, the ship <[]> and the head n 
          if( CheckPerimeter((ufo->points + 0)->x, (ufo->points + 0)->y, (ufo->points + 1)->x, (ufo->points + 1)->y, (player->bullet + e)->center.x, (player->bullet + e)->center.y) + 
              CheckPerimeter((ufo->points + 1)->x, (ufo->points + 1)->y, (ufo->points + 2)->x, (ufo->points + 2)->y, (player->bullet + e)->center.x, (player->bullet + e)->center.y) +
              CheckPerimeter((ufo->points + 2)->x, (ufo->points + 2)->y, (ufo->points + 3)->x, (ufo->points + 3)->y, (player->bullet + e)->center.x, (player->bullet + e)->center.y) +
              CheckPerimeter((ufo->points + 3)->x, (ufo->points + 3)->y, (ufo->points + 4)->x, (ufo->points + 4)->y, (player->bullet + e)->center.x, (player->bullet + e)->center.y) +
              CheckPerimeter((ufo->points + 4)->x, (ufo->points + 4)->y, (ufo->points + 7)->x, (ufo->points + 7)->y, (player->bullet + e)->center.x, (player->bullet + e)->center.y) +
              CheckPerimeter((ufo->points + 7)->x, (ufo->points + 7)->y, (ufo->points + 0)->x, (ufo->points + 0)->y, (player->bullet + e)->center.x, (player->bullet + e)->center.y)
              == 6 || //Ship calcs
              CheckPerimeter((ufo->points + 7)->x, (ufo->points + 7)->y, (ufo->points + 4)->x, (ufo->points + 4)->y, (player->bullet + e)->center.x, (player->bullet + e)->center.y) + 
              CheckPerimeter((ufo->points + 4)->x, (ufo->points + 4)->y, (ufo->points + 5)->x, (ufo->points + 5)->y, (player->bullet + e)->center.x, (player->bullet + e)->center.y) +
              CheckPerimeter((ufo->points + 5)->x, (ufo->points + 5)->y, (ufo->points + 6)->x, (ufo->points + 6)->y, (player->bullet + e)->center.x, (player->bullet + e)->center.y) +
              CheckPerimeter((ufo->points + 6)->x, (ufo->points + 6)->y, (ufo->points + 7)->x, (ufo->points + 7)->y, (player->bullet + e)->center.x, (player->bullet + e)->center.y)
              == 4) //Head calcs
          { 
            //If enters depending on the ufo stage it gives x score
            switch(ufo->stage){
              case 1:{
                userinfo->plscore.number += 200;
                ufo->stage = 0;
                (player->bullet + e)->active = false;
              }
              case 2:{
                userinfo->plscore.number += 1000;
                ufo->stage = 0;
                (player->bullet + e)->active = false;
              }
            }
          }
        }
      }
    }
  }
  //This part calcs ufo bullets to player
  if(ufo->stage > 0){
    for(int e = 0; e < kNBullets; e++){
      if((ufo_bullet + e)->active == 1){
        //For every ufo bullet it calcs distance, if distance is lower that 50px it enters
        float distance = sqrtf((player->center.x - (ufo_bullet + e)->center.x) * (player->center.x - (ufo_bullet + e)->center.x) 
                    + (player->center.y - (ufo_bullet + e)->center.y) * (player->center.y - (ufo_bullet + e)->center.y));
        if(distance <= 50){
          //Here we check the triangle of the player
          if( CheckPerimeter(*(player->mainpoints + 2), *(player->mainpoints + 3), *(player->mainpoints + 0), *(player->mainpoints + 1), (ufo_bullet + e)->center.x, (ufo_bullet + e)->center.y) + 
              CheckPerimeter(*(player->mainpoints + 8), *(player->mainpoints + 9), *(player->mainpoints + 2), *(player->mainpoints + 3), (ufo_bullet + e)->center.x, (ufo_bullet + e)->center.y) +
              CheckPerimeter(*(player->mainpoints + 0), *(player->mainpoints + 1), *(player->mainpoints + 8), *(player->mainpoints + 9), (ufo_bullet + e)->center.x, (ufo_bullet + e)->center.y)
              == 3){ 
                //If enters player dies
                player->lives--;
                player->stage = 2;
                (ufo_bullet + e)->active = false;
          }
        }
      }
    }
  }
}
 /* .______     ______   .______       _______   _______ .______      
    |   _  \   /  __  \  |   _  \     |       \ |   ____||   _  \     
    |  |_)  | |  |  |  | |  |_)  |    |  .--.  ||  |__   |  |_)  |    
    |   _  <  |  |  |  | |      /     |  |  |  ||   __|  |      /     
    |  |_)  | |  `--'  | |  |\  \----.|  '--'  ||  |____ |  |\  \----.
    |______/   \______/  | _| `._____||_______/ |_______|| _| `._____|
*/
//Function that calcs the colides of the borders
void BorderColide(Asteroid* asteroid, UFO* ufo, Player* player){
  //For every asteroid if it is alive it checks if it has passed a border
  //If ast is outside the border this function teleports
  for(int i = 0; i < kNAsteroids; i++){
    if((asteroid + i)->stage > 0){
      if((asteroid + i)->center.x > kWindowWidth + 60){
        (asteroid + i)->center.x = (asteroid + i)->center.x - kWindowWidth - 30;
      }else if((asteroid + i)->center.x < -60){
        (asteroid + i)->center.x = (asteroid + i)->center.x + kWindowWidth + 30;
      }else if((asteroid + i)->center.y > kWindowHeight + 60){
        (asteroid + i)->center.y = (asteroid + i)->center.y - kWindowHeight - 30;
      }else if((asteroid + i)->center.y < - 60){
        (asteroid + i)->center.y = (asteroid + i)->center.y + kWindowHeight + 30;
      }
    }
  }
  //If ufo goes outside borders he will die 
  if(ufo->center.x < -50){
    ufo->stage = 0;
  }
  if(ufo->center.x > kWindowWidth + 50){
    ufo->stage = 0;
  }
  if(ufo->center.y < -50){
    ufo->stage = 0;
  }
  if(ufo->center.y > kWindowHeight + 50){
    ufo->stage = 0;
  }
  if(player != NULL){
    /*Long time ago game could paint on the other border if player had a part of it outside
    but that complicated so much the program and was removed.
    If player gets near any border he will teleport to the other side.*/

    //UpdatePlayer(player, player->center.x + kWindowWidth, player->center.y);
    //DrawPlayer(player, player->center.x + kWindowWidth, player->center.y);
    if(player->center.x < 0){
      player->center.x = kWindowWidth;
    }
    //UpdatePlayer(player, player->center.x - kWindowWidth, player->center.y);
    //DrawPlayer(player, player->center.x - kWindowWidth, player->center.y);
    if(player->center.x > kWindowWidth){
      player->center.x = 0;
    }
    //UpdatePlayer(player, player->center.x, player->center.y + kWindowHeight);
    //DrawPlayer(player, player->center.x, player->center.y + kWindowHeight);
    if(player->center.y < 0){
      player->center.y = kWindowHeight;
    }
    //UpdatePlayer(player, player->center.x, player->center.y - kWindowHeight);
    //DrawPlayer(player, player->center.x, player->center.y - kWindowHeight);
    if(player->center.y > kWindowHeight){
      player->center.y = 0;
    }
    //This calcs when bullet gets to a border to
    for(int i = 0; i < kNBullets; i++){
      if((player->bullet + i)->center.x > kWindowWidth){
        (player->bullet + i)->center.x = 0;
      }
      if((player->bullet + i)->center.x < 0){
        (player->bullet + i)->center.x = kWindowWidth;
      }
      if((player->bullet + i)->center.y > kWindowHeight){
        (player->bullet + i)->center.y = 0;
      }
      if((player->bullet + i)->center.y < 0){
        (player->bullet + i)->center.y = kWindowHeight;
      }
    }
  }
}
//Function that calcs colides with asteroids
/*Function works calculating distance between asteroid lines and bullet, if distance is negative in each line
then bullet will be inside asteroid, as asteroids have angles above 180º we can calc that distance with the
whole ast, so we divide asteroids in sections that have angles below 180*/
bool AsteroidTriangulate(Asteroid* asteroid, int type, float pointx, float pointy){
  //Function tracks the type of the asteroid and calcs for every type of asteroid
  switch (type){
    case 0:{
      if((CheckPerimeter(((asteroid)->points + 0)->x, ((asteroid)->points + 0)->y, ((asteroid)->points + 1)->x, ((asteroid)->points + 1)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 1)->x, ((asteroid)->points + 1)->y, ((asteroid)->points + 2)->x, ((asteroid)->points + 2)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 2)->x, ((asteroid)->points + 2)->y, ((asteroid)->points + 5)->x, ((asteroid)->points + 5)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 5)->x, ((asteroid)->points + 5)->y, ((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, ((asteroid)->points + 7)->x, ((asteroid)->points + 7)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 7)->x, ((asteroid)->points + 7)->y, ((asteroid)->points + 8)->x, ((asteroid)->points + 8)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 8)->x, ((asteroid)->points + 8)->y, ((asteroid)->points + 11)->x,((asteroid)->points + 11)->y,pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 11)->x,((asteroid)->points + 11)->y,((asteroid)->points + 0)->x, ((asteroid)->points + 0)->y, pointx, pointy)
        ) == 8
        ||
        ( CheckPerimeter(((asteroid)->points + 2)->x, ((asteroid)->points + 2)->y, ((asteroid)->points + 3)->x, ((asteroid)->points + 3)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 3)->x, ((asteroid)->points + 3)->y, ((asteroid)->points + 4)->x, ((asteroid)->points + 4)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 4)->x, ((asteroid)->points + 4)->y, ((asteroid)->points + 5)->x, ((asteroid)->points + 5)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 5)->x, ((asteroid)->points + 5)->y, ((asteroid)->points + 2)->x, ((asteroid)->points + 2)->y, pointx, pointy)
        ) == 4
        || 
        ( CheckPerimeter(((asteroid)->points + 11)->x,((asteroid)->points + 11)->y,((asteroid)->points + 8)->x, ((asteroid)->points + 8)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 8)->x, ((asteroid)->points + 8)->y, ((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, ((asteroid)->points + 10)->x,((asteroid)->points + 10)->y,pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 10)->x,((asteroid)->points + 10)->y,((asteroid)->points + 11)->x,((asteroid)->points + 11)->y,pointx, pointy)
        ) == 4){
        return true;
      }else{
        return false;
      }
      break;
    }
    case 1:{
      if((CheckPerimeter(((asteroid)->points + 0)->x, ((asteroid)->points + 0)->y, ((asteroid)->points + 1)->x, ((asteroid)->points + 1)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 1)->x, ((asteroid)->points + 1)->y, ((asteroid)->points + 2)->x, ((asteroid)->points + 2)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 2)->x, ((asteroid)->points + 2)->y, ((asteroid)->points + 3)->x, ((asteroid)->points + 3)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 3)->x, ((asteroid)->points + 3)->y, ((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, ((asteroid)->points + 0)->x, ((asteroid)->points + 0)->y, pointx, pointy)
        ) == 5
        ||
        ( CheckPerimeter(((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, ((asteroid)->points + 3)->x, ((asteroid)->points + 3)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 3)->x, ((asteroid)->points + 3)->y, ((asteroid)->points + 4)->x, ((asteroid)->points + 4)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 4)->x, ((asteroid)->points + 4)->y, ((asteroid)->points + 5)->x, ((asteroid)->points + 5)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 5)->x, ((asteroid)->points + 5)->y, ((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, ((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, pointx, pointy)
        ) == 5
        ||
        ( CheckPerimeter(((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, ((asteroid)->points + 7)->x, ((asteroid)->points + 7)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 7)->x, ((asteroid)->points + 7)->y, ((asteroid)->points + 8)->x, ((asteroid)->points + 8)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 8)->x, ((asteroid)->points + 8)->y, ((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, ((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, pointx, pointy)
        ) == 4){
        return true;
      }else{
        return false;
      }
      break;
    }
    case 2:{
      if((CheckPerimeter(((asteroid)->points + 0)->x, ((asteroid)->points + 0)->y, ((asteroid)->points + 1)->x, ((asteroid)->points + 1)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 1)->x, ((asteroid)->points + 1)->y, ((asteroid)->points + 2)->x, ((asteroid)->points + 2)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 2)->x, ((asteroid)->points + 2)->y, ((asteroid)->points + 3)->x, ((asteroid)->points + 3)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 3)->x, ((asteroid)->points + 3)->y, ((asteroid)->points + 11)->x, ((asteroid)->points + 11)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 11)->x, ((asteroid)->points + 11)->y, ((asteroid)->points + 0)->x, ((asteroid)->points + 0)->y, pointx, pointy)
        ) == 5
        ||
        ( CheckPerimeter(((asteroid)->points + 11)->x, ((asteroid)->points + 11)->y, ((asteroid)->points + 3)->x, ((asteroid)->points + 3)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 3)->x, ((asteroid)->points + 3)->y, ((asteroid)->points + 4)->x, ((asteroid)->points + 4)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 4)->x, ((asteroid)->points + 4)->y, ((asteroid)->points + 5)->x, ((asteroid)->points + 5)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 5)->x, ((asteroid)->points + 5)->y, ((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, ((asteroid)->points + 11)->x, ((asteroid)->points + 11)->y, pointx, pointy)
        ) == 5
        ||
        ( CheckPerimeter(((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, ((asteroid)->points + 7)->x, ((asteroid)->points + 7)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 7)->x, ((asteroid)->points + 7)->y, ((asteroid)->points + 8)->x, ((asteroid)->points + 8)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 8)->x, ((asteroid)->points + 8)->y, ((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, ((asteroid)->points + 10)->x, ((asteroid)->points + 10)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 10)->x, ((asteroid)->points + 10)->y, ((asteroid)->points + 11)->x, ((asteroid)->points + 11)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 11)->x, ((asteroid)->points + 11)->y, ((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, pointx, pointy)
        ) == 6){
        return true;
      }else{
        return false;
      }
      break;
    }
    case 3:{
      if((CheckPerimeter(((asteroid)->points + 0)->x, ((asteroid)->points + 0)->y, ((asteroid)->points + 1)->x, ((asteroid)->points + 1)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 1)->x, ((asteroid)->points + 1)->y, ((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, ((asteroid)->points + 10)->x, ((asteroid)->points + 10)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 10)->x, ((asteroid)->points + 10)->y, ((asteroid)->points + 0)->x, ((asteroid)->points + 0)->y, pointx, pointy)
        ) == 4
        ||
        ( CheckPerimeter(((asteroid)->points + 1)->x, ((asteroid)->points + 1)->y, ((asteroid)->points + 2)->x, ((asteroid)->points + 2)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 2)->x, ((asteroid)->points + 2)->y, ((asteroid)->points + 3)->x, ((asteroid)->points + 3)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 3)->x, ((asteroid)->points + 3)->y, ((asteroid)->points + 4)->x, ((asteroid)->points + 4)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 4)->x, ((asteroid)->points + 4)->y, ((asteroid)->points + 5)->x, ((asteroid)->points + 5)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 5)->x, ((asteroid)->points + 5)->y, ((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, ((asteroid)->points + 1)->x, ((asteroid)->points + 1)->y, pointx, pointy)
        ) == 6
        ||
        ( CheckPerimeter(((asteroid)->points + 1)->x, ((asteroid)->points + 1)->y, ((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, ((asteroid)->points + 7)->x, ((asteroid)->points + 7)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 7)->x, ((asteroid)->points + 7)->y, ((asteroid)->points + 8)->x, ((asteroid)->points + 8)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 8)->x, ((asteroid)->points + 8)->y, ((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, ((asteroid)->points + 1)->x, ((asteroid)->points + 1)->y, pointx, pointy)
        ) == 5){
        return true;
      }else{
        return false;
      }
      break;
    }
    default:{
      return false;
      break;
    }
  }
}
/*   _______.  ______   ______   .______       _______ 
    /       | /      | /  __  \  |   _  \     |   ____|
   |   (----`|  ,----'|  |  |  | |  |_)  |    |  |__   
    \   \    |  |     |  |  |  | |      /     |   __|  
.----)   |   |  `----.|  `--'  | |  |\  \----.|  |____ 
|_______/     \______| \______/  | _| `._____||_______|
*/
#pragma region Score
//Function that calcs and updates every string that is shown on the HUD
void UpdateDrawScore(Player* player){
  //Here we update and draw both player scores on screen
  //PLAYER 1
  snprintf((all_players + 0)->plscore.string, sizeof((all_players + 0)->plscore.string) * 5, "%04d", (all_players + 0)->plscore.number);
  esat::DrawSetFillColor(255,255,255);
  esat::DrawText(20,50 ,(all_players + 0)->plscore.string);
  //PLAYER 2
  snprintf((all_players + 1)->plscore.string, sizeof((all_players + 1)->plscore.string) * 5, "%04d", (all_players + 1)->plscore.number);
  esat::DrawSetFillColor(255,255,255);
  esat::DrawText(860,50 ,(all_players + 1)->plscore.string);

  //Here is calculated max score that is shown on the center of the screen
  snprintf(max_score.string, sizeof(max_score.string) * 5, "%04d", max_score.number);
  esat::DrawSetTextSize(20);
  esat::DrawSetFillColor(255,255,255);
  esat::DrawText(450,70 ,max_score.string);
  esat::DrawSetTextSize(50);
  
  

  //If someone player score is higher that max score it is replaced
  if((all_players + 0)->plscore.number > max_score.number){
    max_score.number = (all_players + 0)->plscore.number;
  }
  if((all_players + 1)->plscore.number > max_score.number){
    max_score.number = (all_players + 1)->plscore.number;
  }
  if((all_players + 0)->plscore.number > atoi((all_players + 0)->maxscore)){
    snprintf((all_players + 0)->maxscore, sizeof(char) * 5, "%d", (all_players + 0)->plscore.number);
  }
  if((all_players + 1)->plscore.number > atoi((all_players + 1)->maxscore)){
    snprintf((all_players + 1)->maxscore, sizeof(char) * 5, "%d", (all_players + 1)->plscore.number);
  }


  //This part paints every beginning of round the "Round X" message, using the counter gamecounter as a 
  //color degradator
  if(gamecounter != 0){
    snprintf(userinfo->plactualround.string,sizeof(userinfo->plactualround.string), "%02d", userinfo->plactualround.number + 1);
    esat::DrawSetFillColor(gamecounter << 1,gamecounter << 1,gamecounter << 1);
    esat::DrawText(350,300 ,"Round");
    esat::DrawText(550,300 ,userinfo->plactualround.string);
    esat::DrawSetFillColor(255,255,255);
  }
  //Same but for game over message and with player counter, if he has 0 lives
  if(player->lives == 0){
    esat::DrawSetFillColor(player->counter << 1,player->counter << 1,player->counter << 1);
    esat::DrawText(350,300 ,"Game Over");
    esat::DrawSetFillColor(255,255,255);
  }

  /*
  char frametime[10];
  snprintf(frametime ,sizeof(frametime), "%.02f", deltaTime);
  esat::DrawText(170,50 ,frametime);

  char numasteroid[10];
  snprintf(numasteroid ,sizeof(numasteroid), "%d", AstInGame);
  esat::DrawText(370,50 ,numasteroid);
  */

}
#pragma endregion
/* _______ .______       _______  _______    .___  ___.  _______ .___  ___.   ______   .______     ____    ____ 
  |   ____||   _  \     |   ____||   ____|   |   \/   | |   ____||   \/   |  /  __  \  |   _  \    \   \  /   / 
  |  |__   |  |_)  |    |  |__   |  |__      |  \  /  | |  |__   |  \  /  | |  |  |  | |  |_)  |    \   \/   /  
  |   __|  |      /     |   __|  |   __|     |  |\/|  | |   __|  |  |\/|  | |  |  |  | |      /      \_    _/   
  |  |     |  |\  \----.|  |____ |  |____    |  |  |  | |  |____ |  |  |  | |  `--'  | |  |\  \----.   |  |     
  |__|     | _| `._____||_______||_______|   |__|  |__| |_______||__|  |__|  \______/  | _| `._____|   |__|     
*/
//Function that frees every pointer
//It frees player, fakeplayer, asteroid, faketeroid, ufo, ufo bullet, blackhole, playerinfo and some auxstrings
void FreeMemory(Player* player, Asteroid* asteroid, Faketeroid* faketeroid, Fakeplayer* fakeplayer, UFO* ufo, Bullet* ufo_bullet, BlackHole* blackhole){
  // PLAYER
  if(player != NULL){
    for(int i = 0; i < kNPlayers; i++){
      if((player + i)->mainpoints != NULL){ FreePointer((player + i)->mainpoints); }
      if((player + i)->fire != NULL){ FreePointer((player + i)->fire); }
      // BULLET
      if((player + i)->bullet != NULL){ FreePointer((player + i)->bullet); } 
    }
    FreePointer(player);
  }
  if(bullet_points != NULL){ FreePointer(bullet_points); }
  // FAKEPLAYER
  if(fakeplayer != NULL){
    for(int i = 0; i < 6; i++) {
      if((fakeplayer + i)->points != NULL){ FreePointer((fakeplayer + i)->points); }
    }
    FreePointer(fakeplayer);
  }
  // ASTEROID
  if(asteroid != NULL){
    for(int i = 0; i < kNAsteroids; i++) {
      if((asteroid + i)->points != NULL){ FreePointer((asteroid + i)->points); }
    }
    FreePointer(asteroid);
  }

  // FAKETEROID
  if(faketeroid != NULL){
    for(int i = 0; i < 4; i++) {
      if((faketeroid + i)->points != NULL){ FreePointer((faketeroid + i)->points); }
    }
    FreePointer(faketeroid);
  }

  // UFO
  if(ufo != NULL){
    if(ufo->points != NULL){ FreePointer(ufo->points); } 
    if(ufo->basepoints != NULL){ FreePointer(ufo->basepoints); } 
    FreePointer(ufo);
  }
  if(ufo_bullet != NULL){ FreePointer(ufo_bullet); }

  //BLACKHOLE
  if(blackhole != NULL){
    for(int i = 0; i < kNHoles; i++){
      if((blackhole + i)->points != NULL){ FreePointer((blackhole + i)->points); }
      if((blackhole + i)->base != NULL){ FreePointer((blackhole + i)->base); }
    }
    FreePointer(blackhole);
  }


  //PLAYER INFO
  if(all_players != NULL){
    for(int i = 0; i < 2; i++){
      if((all_players + i)->username != NULL){ FreePointer((all_players + i)->username); }  
      if((all_players + i)->user_password != NULL){ FreePointer((all_players + i)->user_password); } 
      if((all_players + i)->maxscore != NULL){ FreePointer((all_players + i)->maxscore); }
      if((all_players + i)->user_credits != NULL){ FreePointer((all_players + i)->user_credits); }
      if((all_players + i)->round != NULL){ FreePointer((all_players + i)->round); }
      if((all_players + i)->score != NULL){ FreePointer((all_players + i)->score); }

      if((all_players + i)->plscore.string != NULL){ FreePointer((all_players + i)->plscore.string); }
      if((all_players + i)->plactualround.string != NULL){ FreePointer((all_players + i)->plactualround.string); }
      if((all_players + i)->plcredits.string != NULL){ FreePointer((all_players + i)->plcredits.string); }
    }
    FreePointer(all_players);
  } 

  if(auxstring != NULL){ FreePointer(auxstring); }
  if(auxstring2 != NULL){ FreePointer(auxstring2); }
  if(auxstring3 != NULL){ FreePointer(auxstring3); }
  if(max_score.string != NULL){ FreePointer(max_score.string); }
  if(sql != NULL){ FreePointer(sql); }
}

//Function that checks every pointed when it is created to shut down program bc it hasnt been allocated
void CheckPointer(void* punterito) {
  if(punterito == NULL) {
    // 1. Registrar el error de manera más informativa
    fprintf(stderr, "\nERROR CRÍTICO: Puntero nulo en %s, línea %d", __FILE__, __LINE__);
    gamestate = GameState::error;
  }
}
//Function that resets a tring to pure \0
void ResetString(char* string, int caracters){
  for(int i = 0; i < caracters - 1; i++){
    *(string + i) = '\0';
  }
}
//Function that frees pointers and sets them as NULL
void FreePointer(void* punterito){
  if(punterito != NULL) {
    free(punterito);
    punterito = NULL;
  }
}
//Function to open pointers
void* OpenPointer(int howmuch){
  void* punterito = malloc(howmuch);
  CheckPointer(punterito);

  return punterito;
}
//Function to open strings pointers and set them '\0'
void* OpenString(int howmuch){
  void* punterito = malloc(howmuch);
  CheckPointer(punterito);

  ResetString((char*) punterito, howmuch / sizeof(char));

  return punterito;
}
//Function that is used to calc distance between bullets and asteroids line, using math formula 
//to calc distance, if distance is negative it returns 1 if not 0
int CheckPerimeter(float lado1x, float lado1y, float lado2x, float lado2y, float puntox, float puntoy){
  if(((puntox - lado1x) * -(lado1y - lado2y) + (puntoy - lado1y) * (lado1x - lado2x)) < 0){
    return 0;
  }else{
    return 1;
  }
}

/*_______      ___      .___  ___.  _______    .___  ___.      ___      .__   __.      ___       _______  _______ .______      
 /  _____|    /   \     |   \/   | |   ____|   |   \/   |     /   \     |  \ |  |     /   \     /  _____||   ____||   _  \     
|  |  __     /  ^  \    |  \  /  | |  |__      |  \  /  |    /  ^  \    |   \|  |    /  ^  \   |  |  __  |  |__   |  |_)  |    
|  | |_ |   /  /_\  \   |  |\/|  | |   __|     |  |\/|  |   /  /_\  \   |  . `  |   /  /_\  \  |  | |_ | |   __|  |      /     
|  |__| |  /  _____  \  |  |  |  | |  |____    |  |  |  |  /  _____  \  |  |\   |  /  _____  \ |  |__| | |  |____ |  |\  \----.
 \______| /__/     \__\ |__|  |__| |_______|   |__|  |__| /__/     \__\ |__| \__| /__/     \__\ \______| |_______|| _| `._____|
*/
//Function that helps going to the next level
 void Gameplaygamestate(Player* player, Asteroid* asteroid, Faketeroid* faketeroid, Fakeplayer* fakeplayer){
  //When there arent still no ast this function starts a timer and when it goes to 2sec next round starts
  if(AstInGame == 0 && gamecounter >= 120){
    //Each new round userinfo round is increased ans user stats are saved incase player shuts down game
    userinfo->plactualround.number++;
    InsertScore(); //This saves score on the database
    int fakeround = userinfo->plactualround.number;
    //Fakeround is used bc on the assets there is a list of asteroids per level
    //So round 6 is the maximum of asteroids can appear so it remains in 6 but real round continues to grow
    if(userinfo->plactualround.number > 6){
      fakeround = 6;
    }
    int generation = 0;
    //Opens file that contains asteroids per round
    FILE *pfile = fopen("../assets/rounds.txt", "r+"); // Abre el archivo
    if (pfile != NULL){
      fseek(pfile, 0, SEEK_SET);
      int roundd = 0;
      //It searches up to the round that is now being played, when its found it reads numb of ast to generate
      while(roundd != fakeround){
        fscanf(pfile, "%d %d", &roundd, &generation);
      }
      fclose(pfile);
    }
    //Here are generated number os asteroids that correspond to this round
    for(int i = 0; i < generation; i++){
      AsteroidGeneration(asteroid, faketeroid);
    }
    //And timer sets to 0
    gamecounter = 0;
  }else{
    if(AstInGame == 0){
      gamecounter++;
    }
  }
}
/*.___  ___.      ___       __  .__   __.    .___  ___.  _______ .__   __.  __    __  
|   \/   |     /   \     |  | |  \ |  |    |   \/   | |   ____||  \ |  | |  |  |  | 
|  \  /  |    /  ^  \    |  | |   \|  |    |  \  /  | |  |__   |   \|  | |  |  |  | 
|  |\/|  |   /  /_\  \   |  | |  . `  |    |  |\/|  | |   __|  |  . `  | |  |  |  | 
|  |  |  |  /  _____  \  |  | |  |\   |    |  |  |  | |  |____ |  |\   | |  `--'  | 
|__|  |__| /__/     \__\ |__| |__| \__|    |__|  |__| |_______||__| \__|  \______/  
*/
//Function that starts asteroids that are shown on each menu
void MenuAsteroids(Asteroid* asteroid, Faketeroid* faketeroid){
  if(AstInGame == 0){
    for(int i = 0; i < 4; i++){
      AsteroidGeneration(asteroid, faketeroid);
      AsteroidOblivion(asteroid, faketeroid, 1, rand() % 200 * i, rand() % 150 * i);
      AsteroidOblivion(asteroid, faketeroid, 2, rand() % 200 * i, rand() % 150 * i);
    }
  }
}
//Function that kills those asteroids that were generated on the menu, and then it changes gamestate to fullgame
void EndMenu(Asteroid* asteroid){
  for(int i = 0; i < kNAsteroids; i++){
    if((asteroid + i)->stage > 0){
      (asteroid + i)->stage = 0;
    }
  }
  gamestate = GameState::fullgame;
  AstInGame = 0;
}
//Function that draws the menu that has the play button, also it updates and draws asteroids
void DrawCalcMenu(Asteroid* asteroid, Faketeroid* faketeroid){
  UpdateAsteroid(asteroid, faketeroid);
  //Here it updates every asteroid point (Look function description)
  esat::DrawBegin();
  esat::DrawClear(0,0,0);
  esat::DrawSetStrokeColor(255,255,255);
  esat::DrawSetFillColor(255,255,255,0);
  DrawAsteroid(asteroid);
  //Here draws those asteroids to make a realistic background
  esat::DrawSetFillColor(255,255,255,255);
  esat::DrawSetTextSize(90);
  esat::DrawText(270, 120, "ASTEROIDS");
  esat::DrawSetTextSize(50);
  /*Here there is a transcription to make shure player has enough credits to play the game
  if he doesnt have it will show: You have no credits left*/
  (all_players + 0)->plcredits.number = atoi((all_players + 0)->user_credits);
  (all_players + 1)->plcredits.number = atoi((all_players + 1)->user_credits);
  bool play = false;
  if(kNPlayers == 2){
    if((all_players + 0)->plcredits.number > 0 && (all_players + 1)->plcredits.number > 0){
      play = 1;
    }
  }else{
    if((all_players + 0)->plcredits.number > 0){
      play = 1;
    }
  }
  if(play == 1){
    esat::DrawText(290, 750, "1 COIN 1     1");
    //This part checks a hover over the text so if you put your mouse over it it grows, if you click enters a menu
    if((esat::MousePositionX() > 545 && esat::MousePositionX() < 660 && esat::MousePositionY() > 705 && esat::MousePositionY() < 758) || esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)){
      esat::DrawSetTextSize(55);
      if(esat::MouseButtonDown(0) || esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)){
        EndMenu(asteroid);
        //StartGame(player);
      }
    }else{
      //Here the hover is removed if mouse isn't in place
      esat::DrawSetTextSize(50);
    }
    esat::DrawText(550, 750, "PLAY");
  }else{
    //Here is said this if player has no credits
    esat::DrawText(180, 750, "You Have No Credits Left");
  }
  //SCOREBOARD BUTTON
  //Again a hover over ScoreBoard
  if(( esat::MousePositionX() > 370 && esat::MousePositionX() < 700 
    && esat::MousePositionY() > 550 && esat::MousePositionY() < 600)){

    if(esat::MouseButtonDown(0)){
      gamestate = GameState::scoreboard;
    }
    esat::DrawSetTextSize(55);
    esat::DrawText(370, 600, "ScoreBoard");
  }else{
    esat::DrawSetTextSize(50);
    esat::DrawText(370, 600, "ScoreBoard");
  }
  esat::DrawSetTextSize(50);
  if(kNPlayers == 2){
    if(user_index == 0){
      esat::DrawText(420, 200, "PLAYER 1"); 
    }else{
      esat::DrawText(420, 200, "PLAYER 2"); 
    }
  }
  //ALL of this constitus text to draw the menu
  esat::DrawSetTextSize(20);
  esat::DrawText(385, 870, "ASTEROIDS BY PAUU");  
  esat::DrawSetTextSize(50);
  esat::DrawEnd();
}
//This is the first menu of the game to select 1 or 2 players and gamemode
//Simply has a switch to go trough the stages of the "cuestionary"
void DrawCalcPlayerMenu(Player** player, Asteroid* asteroid, Faketeroid* faketeroid){
  static int infocounter = 0;
  UpdateAsteroid(asteroid, faketeroid);
  //This also has asteroids to ambientate, so we Update them and draw them, as well as all the text to decorate
  esat::DrawBegin();
  esat::DrawClear(0,0,0);
  esat::DrawSetStrokeColor(255,255,255);
  esat::DrawSetFillColor(255,255,255,0);
  DrawAsteroid(asteroid);

  esat::DrawSetFillColor(255,255,255,255);
  esat::DrawSetTextSize(90);
  esat::DrawText(270, 120, "ASTEROIDS");  
  esat::DrawSetTextSize(20);
  esat::DrawText(385, 870, "ASTEROIDS BY PAUU");  
  esat::DrawSetTextSize(50);
  //This is the switch to select the players and gamemode
  switch(infocounter){
    case 0:{
      //This is the first hover to click on 1 player
      if(( esat::MousePositionX() > 370 && esat::MousePositionX() < 700 
        && esat::MousePositionY() > 350 && esat::MousePositionY() < 400) || esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)){
        if(esat::MouseButtonDown(0) || esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)){
          //This hover selects number of players, so kNplayers will flow depending on the 'button' pressed
          infocounter++;
          kNPlayers = 1;
          InitPlayers(player);
        }
        esat::DrawSetTextSize(55);
        esat::DrawText(400, 400, "1 PLAYER");
      }else{
        esat::DrawSetTextSize(50);
        esat::DrawText(400, 400, "1 PLAYER");
      }
      //Hover to 2 players
      if(( esat::MousePositionX() > 370 && esat::MousePositionX() < 700 
        && esat::MousePositionY() > 450 && esat::MousePositionY() < 500)){
        //Here's the button for 2 players
        if(esat::MouseButtonDown(0)){
          infocounter++;
          kNPlayers = 2;
          InitPlayers(player);
        }

        esat::DrawSetTextSize(55);
        esat::DrawText(400, 500, "2 PLAYERS");
      }else{
        esat::DrawSetTextSize(50);
        esat::DrawText(400, 500, "2 PLAYERS");
      }
      break;
    }
    case 1:{
      //HOVER TO SELECT GAMEMODE, NORMAL AND BLACKHOLE
      esat::DrawText(400, 200, "Gamemode");
      if(( esat::MousePositionX() > 370 && esat::MousePositionX() < 700 
        && esat::MousePositionY() > 350 && esat::MousePositionY() < 400) || esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)){
        if(esat::MouseButtonDown(0) || esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)){
          //Here is selected normal gamemode this leads to unlogged screen
          gamemode = GameMode::normal;
          gamestate = GameState::unlogged;
        }
        //Hover for normal gamemode
        esat::DrawSetTextSize(55);
        esat::DrawText(400, 400, "Normal");
      }else{
        esat::DrawSetTextSize(50);
        esat::DrawText(400, 400, "Normal");
      }
  
      if(( esat::MousePositionX() > 370 && esat::MousePositionX() < 700 
        && esat::MousePositionY() > 450 && esat::MousePositionY() < 500)){
        //Hover to blackhole gamemode, this leads to unlogged screen
        if(esat::MouseButtonDown(0)){
          gamemode = GameMode::black_hole;
          gamestate = GameState::unlogged;
        }
        esat::DrawSetTextSize(55);
        esat::DrawText(400, 500, "Black Hole");
      }else{
        esat::DrawSetTextSize(50);
        esat::DrawText(400, 500, "Black Hole");
      }
    }
  }
  esat::DrawEnd();
}
//This parts shows when you die your score and your friends one, 
//if you or your friends are top 10 it will show
void DieScoreBoard(Asteroid* asteroid, Faketeroid* faketeroid){
  //This checks if game has already been saved
  if(gamesaved == false){
    SaveScore();
    gamesaved = true;
  }
  //This screen has asteroids on background so we have to update and draw them
  UpdateAsteroid(asteroid, faketeroid);

  esat::DrawBegin();
  esat::DrawClear(0,0,0);
  esat::DrawSetFillColor(255,255,255,0);
  //Here is the asteroids draw
  DrawAsteroid(asteroid);
  esat::DrawSetStrokeColor(255,255,255);
  esat::DrawSetFillColor(255,255,255,255);
  esat::DrawSetTextSize(90);
  esat::DrawText(270, 120, "ASTEROIDS");  //Asteroids title
  //This screen will repeat 2 times if theres 2 players
  for(int i = 0; i < kNPlayers; i++){
    esat::DrawSetTextSize(50);
    //This shows you user and score
    esat::DrawText(450, 430 + i * 200, userinfo->username);
    esat::DrawText(240, 500 + i * 200, "Your Score Is: ");
    esat::DrawText(680, 500 + i * 200, userinfo->plscore.string);
    //If you are top 10 it will show, 
    //Topplayer settings-> |  -> No top | 1 -> Player 1 top | 2 -> Player 2 top | 3 -> Both Player top
    switch(topplayer){
      case 1:{
        esat::DrawText(700, 430, "TOP 10");
      }
      case 2:{
        esat::DrawText(700, 430 + i * 200, "TOP 10");
      }
      case 3:{
        esat::DrawText(700, 430, "TOP 10");
        esat::DrawText(700, 430 + i * 200, "TOP 10");
      }
    }
    //this part switches players so we can se both of them stats
    if(kNPlayers == 2){
      SwitchPlayers();
    }
  }
  esat::DrawSetTextSize(20);
  esat::DrawText(385, 870, "ASTEROIDS BY PAUU");
  esat::DrawEnd();
}
//This function draws all of the unlogged menu, where player has to login or register
void UnLoggedMenu(){
  esat::DrawBegin();
  esat::DrawClear(0,0,0);
  esat::DrawSetStrokeColor(255,255,255);
  esat::DrawSetFillColor(255,255,255,0);

  
  esat::DrawSetFillColor(255,255,255,255);
  esat::DrawSetTextSize(90);
  esat::DrawText(270, 120, "ASTEROIDS");  
  //LOGIN HERE IS THE HOVER OVER LOGIN BUTTON
  if(( esat::MousePositionX() > 400 && esat::MousePositionX() < 550 
    && esat::MousePositionY() > 350 && esat::MousePositionY() < 400) || esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)){
    if(esat::MouseButtonDown(0) || esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)){
      //If you click or intro you will go to login
      gamestate = GameState::login;
    }
    esat::DrawSetTextSize(55);
    esat::DrawText(400, 400, "Login");
  }else{
    esat::DrawSetTextSize(50);
    esat::DrawText(400, 400, "Login");
  }
  //REGIStrTER BUTTON
  if(( esat::MousePositionX() > 370 && esat::MousePositionX() < 700 
    && esat::MousePositionY() > 450 && esat::MousePositionY() < 500)){

    if(esat::MouseButtonDown(0)){
      //If you click on register you will go to register
      gamestate = GameState::registr;
    }
    esat::DrawSetTextSize(55);
    esat::DrawText(370, 500, "Registrter");
  }else{
    esat::DrawSetTextSize(50);
    esat::DrawText(370, 500, "Registrter");
  }
  esat::DrawSetTextSize(20);
  esat::DrawText(385, 870, "ASTEROIDS BY PAUU");  
  esat::DrawSetTextSize(50);
  esat::DrawEnd();
}
//This is register menu, where player will create his credentials
//Also function will introduce them into database
//IMPORTANT userinfo-> is a pointer to the real credentials on (all_player 0/1)->info
//IMPORTANTSo every time we switch players in reality we switch userinfo-> to (all_player 0 or 1)
void RegistrterMenu(){
  static int charcounter = 0; //This is a counter to help graphic text
  static int infocounter = 0; //This is a counter to move trough screens
  static int correct = 0;     //This is the helper if the user is repeated
  esat::DrawBegin();
  esat::DrawClear(0,0,0);
  esat::DrawSetStrokeColor(255,255,255);
  esat::DrawSetFillColor(255,255,255,0);

  esat::DrawSetFillColor(255,255,255,255);
  esat::DrawSetTextSize(90);
  esat::DrawText(270, 120, "ASTEROIDS");  
  //LOGIN BUTTON
  esat::DrawSetTextSize(50);
  //Infocounter 0 will be to introduce you user
  //If correct is 1 you won't go to passw and have to introduce other user
  //When you press enter function InsertVNN will give you back correct to check if user is wrong
  switch(infocounter){
    case 0:{
      esat::DrawText(220, 400, "Enter Your Username");
      esat::DrawText(300, 470, userinfo->username);

      if(correct == 1){
        esat::DrawText(220, 340, "Unvalid Username");
      }
      //This is the helper to introduce your credentials
      /*Auxkey is the key you introduce every time, and charcounter the key on the string 
      that you are right now program filters spaces and that string is less that 12 character*/
      char auxkey = esat::GetNextPressedKey();
      if(auxkey != '\0' && charcounter < 12 && auxkey != ' '){
        *(userinfo->username + charcounter) = auxkey;
        charcounter++; 
      }
      if(esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace) && charcounter > 0){
        //If user pressed backspaces that key will be deleted and you'll go back on the string
        charcounter--;
        *(userinfo->username + charcounter) = '\0';
      }
      esat::ResetBufferdKeyInput();
      break;
    }
    case 1:{
      //This is the same but with passw, here auxstring is replaces with * as many char you have
      //It will show as many * as char you have on you pass
      //The real pass is stored in userinfo->userpasww
      for(int i = 0; i < charcounter; i++){
        *(auxstring3 + i) = '*';
      }
      esat::DrawText(220, 400, "Enter Your Password");
      esat::DrawText(300, 470, auxstring3);
      //Here is reseted auxstring 3 to then regrite it if is has been deleted it
      //This is the same for every part that has text (Read first part line 2260 aprox)
      ResetString(auxstring3,20);
      char auxkey = esat::GetNextPressedKey();
      if(auxkey != '\0' && charcounter < 12 && auxkey != ' '){
        *(userinfo->user_password + charcounter) = auxkey;
        charcounter++; 
      }else if(esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace) && charcounter > 0){
        charcounter--; 
        *(userinfo->user_password + charcounter) = '\0';
      }
      esat::ResetBufferdKeyInput();
      break;
    }
    //This is the same for every part that has text (Read first part line 2260 aprox)
    case 2:{
      esat::DrawText(220, 400, "Enter Your Name");
      esat::DrawText(300, 470, auxstring);
      
      char auxkey = esat::GetNextPressedKey();
      if(auxkey != '\0' && charcounter < 12 && auxkey != ' '){
        *(auxstring + charcounter) = auxkey;
        charcounter++;
      }else if(esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace) && charcounter > 0){
        charcounter--; 
        *(auxstring + charcounter) = '\0';
      }
      esat::ResetBufferdKeyInput();
      break;
    }
    //This is the same for every part that has text (Read first part line 2260 aprox)
    case 3:{
      esat::DrawText(220, 400, "Enter Both Lastnames");
      esat::DrawText(300, 470, auxstring);

      char auxkey = esat::GetNextPressedKey();
      if(auxkey != '\0' && charcounter < 20){
        if(auxkey == ' '){
          *(auxstring + charcounter) = '_';
        }else{
          *(auxstring + charcounter) = auxkey;
        }
        charcounter++; 
      }else if(esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace) && charcounter > 0){
        charcounter--; 
        *(auxstring + charcounter) = '\0';
      }
      esat::ResetBufferdKeyInput();
      break;
    }
    //This is the same for every part that has text (Read first part line 2260 aprox)
    //In this one if is pressed alt you can write @ nc normaly fucntion getpressedkey won't capture @
    case 4:{
      esat::DrawText(220, 400, "Enter Your Email");
      esat::DrawText(200, 470, auxstring);

      char auxkey = esat::GetNextPressedKey();
      if(auxkey != '\0' && auxkey != ' ' && charcounter < 40){
        if(auxkey == '2' && esat::IsSpecialKeyPressed(esat::kSpecialKey_Alt)){
          *(auxstring + charcounter) = '@';
        }else{
          *(auxstring + charcounter) = auxkey;
        }
        
        charcounter++; 
      }else if(esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace) && charcounter > 0){
        charcounter--; 
        *(auxstring + charcounter) = '\0';
      }
      esat::ResetBufferdKeyInput();
      break;
    }
    //This is the same for every part that has text (Read first part line 2260 aprox)
    case 5:{
      esat::DrawText(220, 400, "Enter your city");
      esat::DrawSetTextSize(30);
      esat::DrawText(350, 425,"Where you live");
      esat::DrawSetTextSize(50);
      esat::DrawText(300, 480, auxstring);

      char auxkey = esat::GetNextPressedKey();
      if(auxkey != '\0' && charcounter < 12 && auxkey != ' '){
        *(auxstring + charcounter) = auxkey;
        charcounter++;
      }else if(esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace) && charcounter > 0){
        charcounter--; 
        *(auxstring + charcounter) = '\0';
      }
      esat::ResetBufferdKeyInput();
      break;
    }
    //This is the same for every part that has text (Read first part line 2260 aprox)
    case 6:{
      esat::DrawText(220, 400, "Enter your country");
      esat::DrawSetTextSize(30);
      esat::DrawText(350, 425,"Where you live");
      esat::DrawSetTextSize(50);
      esat::DrawText(300, 480, auxstring);

      char auxkey = esat::GetNextPressedKey();
      if(auxkey != '\0' && charcounter < 12 && auxkey != ' '){
        *(auxstring + charcounter) = auxkey;
        charcounter++; 
      }else if(esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace) && charcounter > 0){
        charcounter--; 
        *(auxstring + charcounter) = '\0';
      }
      esat::ResetBufferdKeyInput();
      break;
    }
    //This is the same for every part that has text (Read first part line 2260 aprox)
    case 7:{
      esat::DrawText(220, 400, "Enter your birthdate");
      esat::DrawSetTextSize(30);
      esat::DrawText(350, 425,"Like this 01/01/2000");
      esat::DrawSetTextSize(50);
      esat::DrawText(300, 480, auxstring);

      char auxkey = esat::GetNextPressedKey();
      if(auxkey != '\0' && charcounter < 12 && auxkey != ' '){
        if(auxkey == '7' && esat::IsSpecialKeyPressed(esat::kSpecialKey_Shift)){
          *(auxstring + charcounter) = '/';
        }else{
          *(auxstring + charcounter) = auxkey;
        }
        charcounter++; 
      }else if(esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace) && charcounter > 0){
        charcounter--; 
        *(auxstring + charcounter) = '\0';
      }
      esat::ResetBufferdKeyInput();
      break;
    }
  }

  esat::DrawSetTextSize(20);
  esat::DrawText(385, 870, "ASTEROIDS BY PAUU");
  esat::DrawSetTextSize(50);
  esat::DrawEnd();
  //Here starts the info input
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Enter) && charcounter > 0){
    switch (infocounter){
      //USERNAME AND PASSWORD
      case 0:{
        //In the first case we will know if username is duplicated,
        bool duplicado = false;
        InsertVNN(&duplicado);
        if(duplicado == false){
          infocounter++;
          charcounter = 0;
        }else{
          correct = 1;
        }
        break;
      }
      //In the other cases we will introduce data as usually
      case 1: //PASSWORD
      case 2: //FIRST NAME
      case 3: //BOTH LASTNAMES
      case 4: //EMAIL
      case 5: //CITY
      case 6: //COUNTRY
      {  
        InsertData(auxstring, infocounter);
        infocounter++;
        charcounter = 0;
        break;
      }
      case 7:{
        InsertData(auxstring, infocounter);
        infocounter++;
        charcounter = 0;
        //Here, if there is 2 players we will go back to unlogged bc to log other player
        //If not we will simply go to mainmeny to play
        //Also this function swiched players to switch pointer userinfo
        if(kNPlayers == 2){
          if(user_index == 0){
            gamestate = GameState::unlogged;
          }else{
            gamestate = GameState::mainmenu;
          }
          infocounter = 0;
          ResetString(auxstring,30);
          ResetString(auxstring2,30);
          SwitchPlayers();
        }else{
          gamestate = GameState::mainmenu;
        }
        break;
      }
    }
    ResetString(auxstring,30);
  }
}
//IMPORTANT userinfo-> is a pointer to the real credentials on (all_player 0/1)->info
//IMPORTANTSo every time we switch players in reality we switch userinfo-> to (all_player 0 or 1)
//Infocounter 0 will be to introduce you user
//If correct is 1 you won't go to passw and have to introduce other user
//When you press enter function InsertVNN will give you back correct to check if user is wrong
void LoginMenu(){
  //Charcounter represents the key you are in the string
  static int charcounter = 0;
  static int infocounter = 0;
  static int correct = 0;
  esat::DrawBegin();
  esat::DrawClear(0,0,0);
  esat::DrawSetStrokeColor(255,255,255);
  esat::DrawSetFillColor(255,255,255,0);

  esat::DrawSetFillColor(255,255,255,255);
  esat::DrawSetTextSize(90);
  esat::DrawText(270, 120, "ASTEROIDS"); 
  esat::DrawSetTextSize(20);
  esat::DrawText(385, 870, "ASTEROIDS BY PAUU");  
  esat::DrawSetTextSize(50);
  esat::DrawEnd(); 
  //LOGIN BUTTON HOVER
  esat::DrawSetTextSize(50);
  switch(infocounter){
    case 0:{
      esat::DrawText(220, 400, "Enter Your Username");
      esat::DrawText(300, 470, auxstring);
      if(correct == 1){
        esat::DrawText(220, 340, "Unvalid Username");
      }
      //This is the helper to introduce your credentials
      /*Auxkey is the key you introduce every time, and charcounter the key on the string 
      that you are right now, program filters spaces and that string is less that 12 character*/
      //If user is invalid game will show text to Invalid user
      char auxkey = esat::GetNextPressedKey();
      if(auxkey != '\0' && charcounter < 12 && auxkey != ' '){
        *(auxstring + charcounter) = auxkey;
        *(userinfo->username + charcounter) = auxkey;
        charcounter++; 
      }
      if(esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace) && charcounter > 0){
        charcounter--;
        *(auxstring + charcounter) = '\0';
        *(userinfo->username + charcounter) = '\0';
        
      }
      esat::ResetBufferdKeyInput();
      break;
    }
    //SAME BUT FOR PASSWORD
    case 1:{
      for(int i = 0; i < charcounter; i++){
        *(auxstring3 + i) = '*';
      }
      esat::DrawText(220, 400, "Enter Your Password");
      esat::DrawText(300, 470, auxstring3);
      if(correct == 1){
        esat::DrawText(220, 340, "Unvalid Password");
      }
      char auxkey = esat::GetNextPressedKey();
      if(auxkey != '\0' && charcounter < 12 && auxkey != ' '){
        *(auxstring2 + charcounter) = auxkey;
        charcounter++; 
      }else if(esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace) && charcounter > 0){
        charcounter--; 
        *(auxstring2 + charcounter) = '\0';
      }
      esat::ResetBufferdKeyInput();
      ResetString(auxstring3, 20);
      break;
    }
    //This part takes you to unlogged if there is another user unlogged or to mainmanu
    //This part also switch userinfo-> to (all_users 1 or 0)->
    case 2:{
      if(kNPlayers == 2){
        if(*((all_players + 1)->username + 0) == '\0'){
          gamestate = GameState::unlogged;
        }else{
          gamestate = GameState::mainmenu;
        }
        charcounter = 0;
        infocounter = 0;
        ResetString(auxstring,30);
        ResetString(auxstring2,30);
        SwitchPlayers();
      }else{
        gamestate = GameState::mainmenu;
      }
      
      break;
    }
  }
  //This is the input part
  //When you introduce you user, function GetUser first returns if thats a valid user and then returns
  //The passw on userinfo->password so we can use that later
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)){
    switch(infocounter){
      case 0: {
        correct = 0;
        bool duplicado = false;
        GetUser(&duplicado);
        if(strcmp(auxstring, userinfo->username) == 0){
          infocounter++;
          charcounter = 0;
        }else{
          correct = 1;
        }
        break;
      }
      //Here if the passw introduced by user is the same a userpasww tgen program goes to mainmenu screen to continue
      case 1: {
          if(strcmp(auxstring2, userinfo->user_password) == 0){
            infocounter++;
            correct = 0;
            charcounter = 0;
          }else{
            correct = 1;
          }
          break;
      }
    }
  }
}

//Calback to return some user info
static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
  int i;
  for(i = 0; i<argc; i++) {
     printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

//This function inserts username passw credits and score of the player when player just have registered
void InsertVNN(bool* duplicado){
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
   /* Open database */
   rc = sqlite3_open("../assets/database/usuarios.db", &db);
   
   if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
   } else {
      fprintf(stderr, "Opened database successfully\n");
   }

   /* Create SQL statement */
   //Simply just inserts data on the table
   snprintf(sql, sizeof(char) * 100, "INSERT INTO USUARIOS (USERNAME,PASSWORD,CREDITS,SCORE) " \
                                      "VALUES ('%s','0',10,0);",userinfo->username);
   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   snprintf(userinfo->user_credits, sizeof(char) * 3, "10");
   
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      *duplicado = true;
   } else {
      fprintf(stdout, "Records created successfully\n");
      *duplicado = false;
   }
   sqlite3_close(db);
}
//This function gets the infocounter to introduce the data every time,
//Including password, name, lastname, email, country, city and birthdate
void InsertData(bool duplicado, int infocounter){
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  ResetString(sql, 200);
  /* Open database */
  rc = sqlite3_open("../assets/database/usuarios.db", &db);

  if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
  } else {
      fprintf(stderr, "Opened database successfully\n");
  }
  /* Create SQL statement */
  switch(infocounter){
    case 1:{
      snprintf(sql, sizeof(char) * 200, "UPDATE USUARIOS set PASSWORD = '%s' where USERNAME = '%s'; " \
         "SELECT * from USUARIOS", userinfo->user_password, userinfo->username);
      break;
    }
    case 2:{
      snprintf(sql, sizeof(char) * 200, "UPDATE USUARIOS set NAME = '%s' where USERNAME = '%s'; " \
         "SELECT * from USUARIOS", auxstring, userinfo->username);
      break;
    }
    case 3:{
      snprintf(sql, sizeof(char) * 200, "UPDATE USUARIOS set LASTNAME = '%s' where USERNAME = '%s'; " \
         "SELECT * from USUARIOS", auxstring, userinfo->username);
      break;
    }
    case 4:{
      snprintf(sql, sizeof(char) * 200, "UPDATE USUARIOS set EMAIL = '%s' where USERNAME = '%s'; " \
         "SELECT * from USUARIOS", auxstring, userinfo->username);
      break;
    }
    case 5:{
      snprintf(sql, sizeof(char) * 200, "UPDATE USUARIOS set CITY = '%s' where USERNAME = '%s'; " \
         "SELECT * from USUARIOS", auxstring, userinfo->username);
      break;
    }
    case 6:{
      snprintf(sql, sizeof(char) * 200, "UPDATE USUARIOS set COUNTRY = '%s' where USERNAME = '%s'; " \
         "SELECT * from USUARIOS", auxstring, userinfo->username);
      break;
    }
    case 7:{
      snprintf(sql, sizeof(char) * 200, "UPDATE USUARIOS set BIRTHDATE = '%s' where USERNAME = '%s'; " \
         "SELECT * from USUARIOS", auxstring, userinfo->username);
      break;
    }
  }
  /* Execute SQL statement */
  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

  if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }else{
    fprintf(stdout, "Records created successfully\n");
  }
  sqlite3_close(db);
}
//This function uses custom callback to get every info of the user (See declaration of callbackuser)
void GetUser(bool* duplicado){
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;

  rc = sqlite3_open("../assets/database/usuarios.db", &db);
  if (rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
  }else{
    snprintf(sql, 200, "SELECT USERNAME, PASSWORD ,MAXSCORE, CREDITS, SCORE, ROUND FROM USUARIOS WHERE USERNAME = '%s';", userinfo->username);
    //You simply have to introduce a struct with the number of args you want with strings
    //You want 5 args -> struct with 5 strings consecutives
    //That struct if userinfo
    //This function also returns when a username isn't valid
    rc = sqlite3_exec(db, sql, callbackuser, &userinfo, &zErrMsg);

    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      *duplicado = true;
    } else {
      *duplicado = false;
    }
  }
  sqlite3_close(db);
  printf("%s %s",userinfo->username, userinfo->user_password);

  if(*(userinfo->user_password + 0) == '\0'){
    *(userinfo->username + 0) = '\0';
  }
}
//This is s a custom callback to return 5 strings on userinfo->
//Bc userinfo has his 5 args strings if we introduce it this callback
//will write every string with the info of the user introduced
static int callbackuser(void *data, int argc, char **argv, char **azColName){
  for(int i = 0; i < argc; i++){
    if (strcmp(azColName[i], "USERNAME") == 0 && *(argv + 0)){
      strncpy(userinfo->username, *(argv + 0), sizeof(char) * 12);
    }
    if (strcmp(azColName[i], "PASSWORD") == 0 && *(argv + 1)){
      strncpy(userinfo->user_password, *(argv + 1), sizeof(char) * 12);
    }
    if (strcmp(azColName[i], "MAXSCORE") == 0 && *(argv + 2)){
      strncpy(userinfo->maxscore, *(argv + 2), sizeof(char) * 6);
      max_score.number = atoi(userinfo->maxscore);
    }
    if (strcmp(azColName[i], "CREDITS") == 0 && *(argv + 3)){
      strncpy(userinfo->user_credits, *(argv + 3), sizeof(char) * 5);
      userinfo->plcredits.number = atoi(userinfo->user_credits);
    }
    if (strcmp(azColName[i], "SCORE") == 0 && *(argv + 4)){
      strncpy(userinfo->plscore.string, *(argv + 4), sizeof(char) * 6);
      userinfo->plscore.number = atoi(userinfo->plscore.string);
      snprintf(userinfo->plscore.string,sizeof(char) * 3,"%d",userinfo->plscore.number);
    }
    if (strcmp(azColName[i], "ROUND") == 0 && *(argv + 5)){
      strncpy(userinfo->plactualround.string, *(argv + 5), sizeof(char) * 3);
      userinfo->plactualround.number = atoi(userinfo->plactualround.string) - 1;
      snprintf(userinfo->plactualround.string,sizeof(char) * 3,"%d",userinfo->plactualround.number);
    }
  }
  return 0;
}
//This function inserts score in database every time players pass a round, so if program closes, player
//Can still continue when he started round
void InsertScore(){
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  ResetString(sql, 200);
  /* Open database */
  rc = sqlite3_open("../assets/database/usuarios.db", &db);

  if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
  } else {
      fprintf(stderr, "Opened database successfully\n");
  }
  /* Create SQL statement */
  //Here we insert score and round on that username
  snprintf(sql, sizeof(char) * 200, "UPDATE USUARIOS set SCORE = %d, ROUND = %d where USERNAME = '%s'; " \
      "SELECT * from USUARIOS", userinfo->plscore.number,userinfo->plactualround.number, userinfo->username);
  /* Execute SQL statement */
  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

  if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }else{
    fprintf(stdout, "Records created successfully\n");
  }
  sqlite3_close(db);
}

//This function saves the player's final score and updates database if player ranks top 10
void SaveScore(){
  if(userinfo != NULL){
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    //Open database 
    rc = sqlite3_open("../assets/database/usuarios.db", &db);
    if(rc){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return;
    }

    fprintf(stdout, "Opened database successfully\n");

    //Open a pointer to store top 10 scores
    int* topScores = (int*) malloc(sizeof(int) * 10);
    CheckPointer(topScores);

    //This for is to check both player's score
    for(int i = 0; i < kNPlayers; i++){
      ResetString(sql, 200);

      int playerScore = userinfo->plscore.number;
      int currentCredits = atoi(userinfo->user_credits);

      //Get top 10 scores from database
      int count = 0;
      snprintf(sql,sizeof(char) * 100,"SELECT MAXSCORE FROM USUARIOS ORDER BY MAXSCORE DESC LIMIT 10;");
      sqlite3_stmt *stmt;
      rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

      if(rc == SQLITE_OK){
        while(sqlite3_step(stmt) == SQLITE_ROW && count < 10){
          *(topScores + count++) = sqlite3_column_int(stmt, 0);
          //This part saves in pointer every score on order
        }
      }
      sqlite3_finalize(stmt); //Free statement resources

      //Check if player's score enters in top 10
      bool playerTop = false;
      if(count < 10){
        playerTop = true; //If there are less than 10 scores, it enters on top
      }else{
        int* lowestTop = (topScores + count - 1); //Check the lowest top 10 score
        if(playerScore > *lowestTop){
          playerTop = true; //Player enters top 10 if his score is higher than the lowest
        }
      }

      //If player qualifies for top 10, function gives him 5 credits,
      //This part checks if player 1 is in, player 2 or both of them are in
      //If player 1 -> topplayer = 1 | if player 2 -> topplayer = 2 | if both player -> topplayer = 3
      if(playerTop){
        currentCredits += 5;
        if(user_index == 0){
          if(topplayer == 2){
            topplayer = 3;
          }else{
            topplayer = 1;
          }
        }else{
          if(topplayer == 1){
            topplayer = 3;
          }else{
            topplayer = 2;
          }
        }
      }

      //If current score is higher that maxscore we update it and the date too
      if(userinfo->plscore.number > atoi(userinfo->maxscore)){
        char* datetoday = (char*) calloc(9,sizeof(char)); //Format: MM/DD/YY
        CheckPointer(datetoday);
        #ifdef WIN32
        _strdate_s(datetoday,9); //Get today's date
        #endif
        snprintf(sql, sizeof(char) * 200,
              "UPDATE USUARIOS SET MAXSCORE = %s, CREDITS = %d, DATE = '%s' WHERE USERNAME = '%s';",
              userinfo->maxscore, currentCredits - 1, datetoday, userinfo->username);
        FreePointer(datetoday);
      }else{
        //Otherwise, just update score and credits
        snprintf(sql, sizeof(char) * 200,
              "UPDATE USUARIOS SET MAXSCORE = %s, CREDITS = %d WHERE USERNAME = '%s';",
              userinfo->maxscore, currentCredits - 1, userinfo->username);
      }

      //Execute the update statement
      rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
      if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
      } else {
        fprintf(stdout, "Record updated successfully for player %d\n", i);
      }

      //Switch to next player
      SwitchPlayers();  
    }
    //Close database and free memory used for top 10 array
    sqlite3_close(db);
    FreePointer(topScores);
  }
}

//Function that switches players by changing user index from 0 to 1 everytime
void SwitchPlayers(){
  user_index = (user_index + 1) % 2;
  userinfo = all_players + user_index;
}

//This function swaps player screens by saving in to .dats every asteroid info
//Function first saves how many asteroids we have and then reads how many has the other player to spawn them
//This function is activated when player dies
void SwitchPlayerstats(Asteroid* asteroid, UFO* ufo){
  ufo->stage = 0;
  switch(user_index){
    case 1:{
      FILE *pfile = fopen("../assets/user1.bin", "w+b"); // Abre el archivo
      if (pfile != NULL){
        fwrite(&AstInGame, sizeof(int), 1, pfile);
        for(int i = 0; i < kNAsteroids; i++){
          if((asteroid + i)->stage > 0 && (asteroid + i)->stage < 4){
            fwrite(&((asteroid + i)->kNpoints), sizeof(int), 1, pfile);
            fwrite(&((asteroid + i)->stage), sizeof(int), 1, pfile);
            fwrite(&((asteroid + i)->type), sizeof(int), 1, pfile);
            fwrite(&((asteroid + i)->center.x), sizeof(float), 1, pfile);
            fwrite(&((asteroid + i)->center.y), sizeof(float), 1, pfile);
            fwrite(&((asteroid + i)->speed.x), sizeof(float), 1, pfile);
            fwrite(&((asteroid + i)->speed.y), sizeof(float), 1, pfile);

            for(int e = 0; e < (asteroid + i)->kNpoints; e++){
              fwrite(&((asteroid + i)->points + e)->x, sizeof(float), 1, pfile);
              fwrite(&((asteroid + i)->points + e)->y, sizeof(float), 1, pfile);
            }

            (asteroid + i)->stage = 0;
          }
        }
        fclose(pfile);
      }
      FILE *pfile2 = fopen("../assets/user2.bin", "rb"); // Abre el archivo
      if (pfile2 != NULL){
        fread(&AstInGame, sizeof(int), 1, pfile2);
        for(int i = 0; i < AstInGame; i++){
          fread(&((asteroid + i)->kNpoints), sizeof(int), 1, pfile2);
          fread(&((asteroid + i)->stage), sizeof(int), 1, pfile2);
          fread(&((asteroid + i)->type), sizeof(int), 1, pfile2);
          fread(&((asteroid + i)->center.x), sizeof(float), 1, pfile2);
          fread(&((asteroid + i)->center.y), sizeof(float), 1, pfile2);
          fread(&((asteroid + i)->speed.x), sizeof(float), 1, pfile2);
          fread(&((asteroid + i)->speed.y), sizeof(float), 1, pfile2);

          for(int e = 0; e < (asteroid + i)->kNpoints; e++){
            fread(&((asteroid + i)->points + e)->x, sizeof(float), 1, pfile2);
            fread(&((asteroid + i)->points + e)->y, sizeof(float), 1, pfile2);
          }
        }
        fclose(pfile2);
      }
      break;
    }
    case 0:{
      FILE *pfile = fopen("../assets/user2.bin", "w+b"); // Abre el archivo
      if (pfile != NULL){
        fwrite(&AstInGame, sizeof(int), 1, pfile);
        for(int i = 0; i < kNAsteroids; i++){
          if((asteroid + i)->stage > 0 && (asteroid + i)->stage < 4){
            fwrite(&((asteroid + i)->kNpoints), sizeof(int), 1, pfile);
            fwrite(&((asteroid + i)->stage), sizeof(int), 1, pfile);
            fwrite(&((asteroid + i)->type), sizeof(int), 1, pfile);
            fwrite(&((asteroid + i)->center.x), sizeof(float), 1, pfile);
            fwrite(&((asteroid + i)->center.y), sizeof(float), 1, pfile);
            fwrite(&((asteroid + i)->speed.x), sizeof(float), 1, pfile);
            fwrite(&((asteroid + i)->speed.y), sizeof(float), 1, pfile);

            for(int e = 0; e < (asteroid + i)->kNpoints; e++){
              fwrite(&((asteroid + i)->points + e)->x, sizeof(float), 1, pfile);
              fwrite(&((asteroid + i)->points + e)->y, sizeof(float), 1, pfile);
            }

            (asteroid + i)->stage = 0;
          }
        }
        fclose(pfile);
      }
      FILE *pfile2 = fopen("../assets/user1.bin", "rb"); // Abre el archivo
      if (pfile2 != NULL){
        fread(&AstInGame, sizeof(int), 1, pfile2);
        for(int i = 0; i < AstInGame; i++){
          fread(&((asteroid + i)->kNpoints), sizeof(int), 1, pfile2);
          fread(&((asteroid + i)->stage), sizeof(int), 1, pfile2);
          fread(&((asteroid + i)->type), sizeof(int), 1, pfile2);
          fread(&((asteroid + i)->center.x), sizeof(float), 1, pfile2);
          fread(&((asteroid + i)->center.y), sizeof(float), 1, pfile2);
          fread(&((asteroid + i)->speed.x), sizeof(float), 1, pfile2);
          fread(&((asteroid + i)->speed.y), sizeof(float), 1, pfile2);

          for(int e = 0; e < (asteroid + i)->kNpoints; e++){
            fread(&((asteroid + i)->points + e)->x, sizeof(float), 1, pfile2);
            fread(&((asteroid + i)->points + e)->y, sizeof(float), 1, pfile2);
          }
        }
        fclose(pfile2);
      }
      break;
    }
  }
}
//This function displays the scoreboard sorted by highest score (MAXSCORE).
//It queries the top 10 users per page and allows paging through results.
//Pressing up/down arrow keys moves pages, and clicking back button or pressing enter returns to the main menu.
void ShowScoreBoard(){
  static int offset = 0;
  static int page = 0;

  //Change page using up/down keys
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Up)){
    page++;
  }
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Down)){
    if(page > 0){
      page--;
    }
  }

  offset = page * 10;

  sqlite3 *db;
  sqlite3_stmt *stmt;

  //Open database and prepare SQL query for top 10 scores with offset
  if (sqlite3_open("../assets/database/usuarios.db", &db) != SQLITE_OK) {
    fprintf(stderr, "No se pudo abrir la base de datos: %s\n", sqlite3_errmsg(db));
    return;
  }else{
    snprintf(sql, sizeof(char) * 200,
      "SELECT USERNAME, MAXSCORE FROM USUARIOS ORDER BY MAXSCORE DESC LIMIT 10 OFFSET %d;", offset);

    if(sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK){
      int i = 0;
      esat::DrawBegin();
      esat::DrawClear(0,0,0);

      //Loop through the results and display username + score
      while(sqlite3_step(stmt) == SQLITE_ROW){
          const unsigned char *username = sqlite3_column_text(stmt, 0);
          int score = sqlite3_column_int(stmt, 1);

          esat::DrawSetTextSize(40);
          snprintf(sql, sizeof(char) * 200,"%d. Usuario: %s", i + 1, username);
          esat::DrawText(200,100 + i * 80, sql);
          snprintf(sql, sizeof(char) * 200,"   Puntuacion: %d", score);
          esat::DrawText(200,145 + i * 80, sql);
          i++;
      }

      sqlite3_finalize(stmt);

      //Back to menu button (mouse or enter)
      if((esat::MousePositionX() > 50 && esat::MousePositionX() < 200 
      && esat::MousePositionY() > 20 && esat::MousePositionY() < 80) || esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)){
        if(esat::MouseButtonDown(0) || esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)){
          gamestate = GameState::mainmenu;
        }
        esat::DrawSetTextSize(55);
        esat::DrawText(50, 50, "BACK TO PLAY");
      }else{
        esat::DrawSetTextSize(50);
        esat::DrawText(50, 50, "BACK TO PLAY");
      }

      //Draw page number
      esat::DrawText(820, 850, "PAGE");
      snprintf(auxstring3, sizeof(char) * 5, "%d", page + 1);
      esat::DrawText(950, 850, auxstring3);

      esat::DrawEnd();
    }else{
      fprintf(stderr, "Error en la consulta SQL: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_close(db);
  }
}

//Function that updates blackhole
void BlackHoleUpdate(BlackHole* blackhole,Player* player, Asteroid* asteroid){
  //First we have the generation part, where if gamemode is blackhole and a rand of 1800 is 0 a hole generates
  if((rand() % 1800 == 0 && gamemode == GameMode::black_hole)){
    //It searches for the first hole dead to generate in it
    int counter = 0;
    while((blackhole + counter)->stage != 0 && counter < kNHoles - 1){
      counter++;
    }
    //Here it gives him is stats to where to spawn
    if((blackhole + counter)->stage == 0){
      (blackhole + counter)->stage = 1;
      (blackhole + counter)->center.x = rand() % (kWindowWidth - 400) + 200;
      (blackhole + counter)->center.y = rand() % (kWindowWidth - 400) + 200;
    }
  }
  //Here is calculated every pull from holes
  for(int j = 0; j < kNHoles;j++){
    if((blackhole + j)->stage >= 1){
      //First we get the gravity of the player and if its less than 1 he dies
      //If not we add gravity to his speed to make im go to blackhole
      esat::Vec3 gravity = GetGravity(player->center, (blackhole + j)->center);
      printf("\n%03f",gravity.z);
      if(gravity.z < 1){
        player->lives--;
        player->stage = 2;
      }

      float actualGravity = kGravity * (blackhole + j)->mass;
      player->speed.x += gravity.x * actualGravity;
      player->speed.y += gravity.y * actualGravity;
      //We make this also for every bullet, get gravity and add it to speed
      for(int i = 0; i < kNBullets; i++){
        if((player->bullet + i)->active == 1){

          esat::Vec3 gravity = GetGravity((player->bullet + i)->center, (blackhole + j)->center);
          (player->bullet + i)->speed.x += gravity.x * actualGravity * 8;
          (player->bullet + i)->speed.y += gravity.y * actualGravity * 8;
        }
      }
      //Here also for asteroids, get gravity and add it to speed
      for(int i = 0; i < kNAsteroids; i++){
        if((asteroid + i)->stage > 0 && (asteroid + i)->stage <= 4){
          esat::Vec3 gravity = GetGravity((asteroid + i)->center, (blackhole + j)->center);

          (asteroid + i)->speed.x += gravity.x * actualGravity;
          (asteroid + i)->speed.y += gravity.y * actualGravity;
        }
      }
      //here is the timer of the hole
      (blackhole + j)->counter++;
      //If timer gets to 400 mass starts growing
      if((blackhole + j)->counter < 400){
        (blackhole + j)->mass += 0.01 / 4;
      }
      //If mass gets to 2000 it stops growing and starts getting small
      if((blackhole + j)->counter > 2000){
        (blackhole + j)->mass -= 0.01 / 5;
      }
      //If timer gets to 2500 it dies
      if((blackhole + j)->counter > 2500){
        (blackhole + j)->stage = 0;
        (blackhole + j)->counter = 0;
        (blackhole + j)->mass = 0;
      }
    }
  }
}
//This function updates every point of the hole and draws it on screen
//Hole really is just a pentagon
//This part draws 5 pentagons from biggest to smallest with diffrent iluminations
//To make it shinier and a better animation
void DrawBlackHole(BlackHole* blackhole){
  for(int j = 0; j < kNHoles; j++){
    if((blackhole + j)->stage > 0){
      for(int i = 4; i >= 0; i--){
        esat::Mat3 m = esat::Mat3Identity();
        float size = (blackhole + j)->mass * i * 0.25 + 1;
        m = esat::Mat3Multiply(esat::Mat3Translate(0.0f, 0.0f),m);
        m = esat::Mat3Multiply(esat::Mat3Scale(size, size), m);
        m = esat::Mat3Multiply(esat::Mat3Rotate((i * 33 * PI / 180 + esat::Time() * 0.001) * (((i % 2) * 2) - 1)), m);
        m = esat::Mat3Multiply(esat::Mat3Translate((blackhole + j)->center.x	, (blackhole + j)->center.y), m);
    
        for (int e = 0; e < 5; e++) {
          esat::Vec3 tmp = esat::Mat3TransformVec3(m, *((blackhole + j)->base + e));
          *((blackhole + j)->points + e) = { tmp.x, tmp.y };
        }
        esat::DrawSetStrokeColor(5 + i * 50, 5 + i * 50, 5 + i * 50, 255);
        esat::DrawSetFillColor(i * 10, i * 10, i * 10, 255);
        esat::DrawSolidPath(&(blackhole + j)->points->x, 5, true);
      }
    }
  }
}
//This function returns gravity by inserting pos of object and the hole you are using
//Then calcs the gravity that is going to have and returns that vector
esat::Vec3 GetGravity(esat::Vec2 center, esat::Vec2 hole){
  esat::Vec3 gravity = {hole.x - center.x, hole.y - center.y};
  float norm = sqrtf(gravity.x * gravity.x + gravity.y * gravity.y);
  if(norm < 0.01){ norm = 0.01;}
  gravity.z = norm;
  norm = norm * norm;

  gravity.x = gravity.x / norm;
  gravity.y = gravity.y / norm;

  return gravity;
}