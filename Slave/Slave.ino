 #include <Colorduino.h>
#include <Wire.h>
#define address 0x10
#define Start_Rev 0xFF
#define End_Rev 0xFE
void setup()
{
  Colorduino.Init();
  Wire.begin(address);
  Serial.begin(115200);
  Wire.onReceive(got);
  unsigned char whiteBalVal[3] = {30,63,63};
  Colorduino.SetWhiteBal(whiteBalVal);
}
void loop() {
//   Colorduino.ColorFill(255,255,255);
  while(Wire.available() >= 67){
    if(Wire.read() != Start_Rev){
        while(Wire.read() != End_Rev){}
        break;
    }
    byte color = Wire.read();
    PixelRGB *p = Colorduino.GetPixel(0,0);
    if(color == 1){
        for(int i = 0 ; i < 8; i++){
            for(int j = 0 ; j < 8; j++){
                p->r = Wire.read();
                p++;
            }
        }
    }else if(color == 2){
        for(int i = 0 ; i < 8; i++){
            for(int j = 0 ; j < 8; j++){
                p->g = Wire.read();
                p++;
            }
        }
    }else if(color == 3){
        for(int i = 0 ; i < 8; i++){
            for(int j = 0 ; j < 8; j++){
                p->b = Wire.read();
                p++;
            }
        }
    }
    if(Wire.read() != End_Rev)break;
    if(color == 3)Colorduino.FlipPage();
  }
}
void got(){}

