#include <Wire.h>
#define Start_Rev 0xFF
#define End_Rev 0xFE
#define endl Serial.print("\n")
struct Address_position{
    byte address[4];
    int px[4],py[4];
    Address_position(){
        for(int i = 0 ; i < 4; i++)address[i] = 0x10+i;
        py[0] = 7;
        py[1] = 15;
        py[2] = 23;
        py[3] = 31;
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
}Board[8][32];
const RGBColor infor = RGBColor(0,0,255);
void setup(){
    Wire.begin();
    Serial.begin(115200);
    randomSeed(analogRead(0));
}
void loop(){
      iniit();
    update_board();
    //while(1){
      //Bang();
      //update_board();
      //Serial.print("Bang\n");
    //}
}
void iniit(){
  Board[2][29] = infor;
    for(int j = 1 ; j <= 5 ; j++){
      Board[1][j] = infor;
      Board[6][j] = infor;
    }for(int i = 2; i <= 5; i++){
      Board[i][3] = infor;
    }
    for(int i = 1; i <= 6 ; i++){
      Board[i][7] = infor;
      Board[i][11] = infor;
      Board[i][13] = infor;
    }
    for(int j = 8,i = 2 ; j <= 10;j++,i++){
      Board[i][j] = Board[i+1][j] = infor;
    }
    for(int j = 14 ; j <= 17 ; j++){
      Board[1][j] = Board[4][j] = infor;
    }
    for(int i = 2; i <= 5 ; i++){
      Board[i][19] = Board[i][23] = infor;
    }
    for(int j = 20 ; j <= 22 ; j++){
      Board[1][j] = Board[6][j] = infor;
    }
    for(int i = 1; i <= 6 ; i++){
      Board[i][25] = infor;
    }
    for(int j = 26; j <= 29 ; j++){
      Board[1][j] = Board[3][j] = infor;
    }
    Board[4][27] = Board[5][28] = Board[6][29] = infor;
}
void Bang(){
    for(int i = 0 ; i < 8 ; i++){
      RGBColor tmp = Board[i][31];
      for(int j = 31 ; j >= 1 ; j--){
        Board[i][j] = Board[i][j-1];
      }
      Board[i][0] = tmp;
    }
}
void update_board(){
  for(int i = 0 ; i < 4; i++){
    for(byte j = 1 ; j <= 3; j++){
        Wire.beginTransmission(slave.address[i]);
        Wire.write(Start_Rev);
        Wire.write(j);
        switch(j){
          case 1:
            for(int x = 0; x < 8 ; x++){
                for(int y = slave.py[i] ; y > slave.py[i]-8 ; y--){
                    Wire.write(Board[x][y].r);
                }
            }break;
          case 2:
            for(int x = 0; x < 8 ; x++){
                for(int y = slave.py[i] ; y > slave.py[i]-8 ; y--){
                    Wire.write(Board[x][y].g);
                }
            }break;
          case 3:
            for(int x = 0; x < 8 ; x++){
                for(int y = slave.py[i] ; y > slave.py[i]-8 ; y--){
                    Wire.write(Board[x][y].b);
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
