#include <Wire.h>
#include <IRremote.h>
#include <Timer.h>
#define Start_Rev 0xFF
#define End_Rev 0xFE
#define endl Serial.print("\n")
#define Bird 1
#define Tube 2
const int Rev_pin = 6;
IRrecv irrecv(Rev_pin);
decode_results results;
Timer t1,t2,t3;
int cnt = 0;
int bird_pos;
int stop2,stop3;
bool died;
struct Address_position{
    byte address[4];
    int px[4],py[4];
    Address_position(){
        for(int i = 0 ; i < 4; i++)address[i] = 0x10+i;
        px[0] = py[0] = 0;
        px[1] = 0;py[1] = 8;
        px[2] = 8;py[2] = 0;
        px[3] = py[3] = 8;
    }
}slave = Address_position();
struct RGBColor{
  unsigned char r,g,b;
    RGBColor(){
      r = g = b = 0;
    }
    RGBColor(unsigned char _r, unsigned char _g, unsigned char _b){
      r = _r;
      g = _g;
      b = _b;
    }
};  
struct Colorduino_board{
    RGBColor color;
    int type;//0->none ,1->bird ,2->tube
    Colorduino_board(){
      color = RGBColor();
      type = 0;
    }
    void set(RGBColor _color,int _type){
      color = _color;
      type = _type;
    }
}Board[16][16];
//color_set
const RGBColor none = RGBColor(0,0,0);
const RGBColor bird = RGBColor(21,21,134);
const RGBColor tube = RGBColor(0,222,0);
const RGBColor full = RGBColor(255,255,255);
void setup(){
    Wire.begin();
    Serial.begin(115200);
    irrecv.enableIRIn();
    randomSeed(analogRead(0));
}
void loop(){
    game_start();
    died = 0;
    while(!died){
        iniit();
        if(irrecv.decode(&results)){
          jump();
          irrecv.resume();
        }
        update_board();
    }
    gameover();
}
void iniit(){
  t1.update();
  t2.update();
  t3.update();
}
void game_start(){
    for(int i = 7; i <= 8 ; i++)
        for(int j = 3; j <= 4 ; j++)
          Board[i][j].set(bird,1);
    update_board();
    bird_pos = 7;
    t1.after(2000,tube_prepare);
    stop3 = t3.every(500,gravity);
}
void tube_prepare(){stop2 = t2.every(100,tube_appear);}
void tube_appear(){
  for(int x = 0 ; x < 16 ; x++){
    for(int y = 1 ; y < 16; y++){
      if(Board[x][y].type == Bird){
        if(Board[x][y-1].type == Bird)continue;
        else Board[x][y-1].set(none,0);
      }
      else if(Board[x][y].type == Tube && Board[x][y-1].type == Bird)died = 1;
      else if(Board[x][y].type == 0 && Board[x][y-1].type == Bird)continue;
      else Board[x][y-1] = Board[x][y];
    }
  }
   int tube_height = random(2,9);
  switch(cnt){
    case 0:
      for(int x = 0 ; x <= tube_height ; x++)Board[x][15].set(tube,2);
      for(int x = tube_height+6 ; x < 16 ; x++)Board[x][15].set(tube,2);
      break;
    case 1 ... 3:
      for(int x = 0 ; x < 16 ; x++)Board[x][15] = Board[x][14];
      break;
    case 4 ... 8:
      for(int x = 0 ; x < 16 ; x++)Board[x][15].set(none,0);
      break;
  }
  cnt == 8 ? cnt = 0 : cnt++;
}
void jump(){
  if(bird_pos == 0)return;
  if(Board[bird_pos-1][3].type == Tube || Board[bird_pos-1][4].type == Tube)died = 1;
  Board[bird_pos-1][3].set(bird,1);
  Board[bird_pos-1][4].set(bird,1);
  Board[bird_pos+1][3].set(none,0);
  Board[bird_pos+1][4].set(none,0);
  bird_pos--;
}
void gravity(){
  if(bird_pos == 15){died = 1;return;}
  if(Board[bird_pos+1][3].type == Tube || Board[bird_pos+1][4].type == Tube)died = 1;
  Board[bird_pos+1][3].set(bird,1);
  Board[bird_pos+1][4].set(bird,1);
  Board[bird_pos-1][3].set(none,0);
  Board[bird_pos-1][4].set(none,0);
  bird_pos++;
}
void gameover(){
  t2.stop(stop2);
  t3.stop(stop3);
  for(int i = 0 ; i < 16; i++){
    for(int j = 0 ; j < 16; j++){
      Board[i][j].set(none,0);
    }
  }
  update_board();
  delay(1000);
}
void update_board(){
  for(int i = 0 ; i < 4; i++){
    for(byte j = 1 ; j <= 3; j++){
        Wire.beginTransmission(slave.address[i]);
        Wire.write(Start_Rev);
        Wire.write(j);
        switch(j){
          case 1:
            for(int x = slave.px[i] ; x < slave.px[i]+8 ; x++){
                for(int y = slave.py[i] ; y < slave.py[i]+8 ; y++){
                    Wire.write(Board[y][x].color.r);
                }
            }break;
          case 2:
            for(int x = slave.px[i] ; x < slave.px[i]+8 ; x++){
                for(int y = slave.py[i] ; y < slave.py[i]+8 ; y++){
                    Wire.write(Board[y][x].color.g);
                }
            }break;
          case 3:
            for(int x = slave.px[i] ; x < slave.px[i]+8 ; x++){
                for(int y = slave.py[i] ; y < slave.py[i]+8 ; y++){
                    Wire.write(Board[y][x].color.b);
                }
            }break;
          default:
            Serial.print("error\n");
            break;
        }
        Wire.write(End_Rev);
        Wire.endTransmission(slave.address[i]);
      }
  }
}
// void send(){
//  Wire.beginTransmission()
//  Wire.write();
//  Wire.endTransmission()
// }
// void get(){
//  irrecv.decode(&results)
//     irrecv.resume();
// }
