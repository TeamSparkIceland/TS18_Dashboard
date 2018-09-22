// https://learn.adafruit.com/adafruit-gfx-graphics-library

// Við viljum hæsta hitastig í acc, lægstu spennu á sellu og heildarspennu
// Það væri líka frábært að hafa eithvað sem gefur til kynna að það sé villa í pedalaboxinu.
// Implosebilaty og það dót

// Ef hiti er notaður þarf að deila með 10

#include <SPI.h>
#include "mcp_can.h"
//#include "CAN_ID.h"

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library

// The control pins for the LCD
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

//--- CAN-addresses --------------
#define TempAv  0x162
#define TempMax 0x161
#define Speed   0x140   // set addressuna á Dataloggernum úr CAN master skjalinu á drive
#define VoltMin 0x165
#define VoltTOT 0x166

//Variable declaration

const int SPI_CS_PIN = 10;  // SPI chip select pin
unsigned char len = 0;
unsigned char buf[8];  // data buffer
int canID;                 // CAN ID of resiving mesage
union Data_int {               // Convert char to float
  char buf[2];
  unsigned int data;
} data_int;
union Data_long {
  char buf[4];
  long data;
} data_long;
union Data_float {
  char buf[4];
  float data;
} data_float;

uint16_t identifier;

MCP_CAN CAN(SPI_CS_PIN);    // Set CS pin

//Function declarations

int resieve_Mesege(char buf[8]);
float convert_data(char buf[8]);
//void reset_buffer();
//void power_config();


// Assign human-readable names to some common 16-bit color values:
// Color picker:  http://www.barth-dev.de/online/rgb565-color-picker/
//           or:  https://ee-programming-notepad.blogspot.is/2016/10/16-bit-color-generator-picker.html
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GRAY    0x8410
#define PURPLE  0x7173
#define ORANGE  0xFCA3

// The screen
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

void setup(void) {
  // 480x320   Change in Adafruit_TFTLCD.cpp

  // Raunverulegt setup
  Serial.begin(115200);

  while ( CAN_OK != CAN.begin(CAN_500KBPS) ) { // should be CAN.begin(CAN_1000KBPS)) {  but because of Power meter its 500
    delay(1);
    //Serial.println("CAN not initialized");
  }
  Serial.println("CAN initialized");

  delay(100);
  identifier = tft.readID();
  identifier = 0x9341; //0x9341;
  tft.begin(identifier);

  delay(10);

  tft.setRotation(3); // Set rotation of screen pixels to "normal" rotation

  //long timer = millis();
  //while( millis() - timer < 50 );
  delay(10);
  tft.fillScreen(BLACK);

  /*
    tft.drawFastHLine(20,20,300,GREEN);
    tft.drawFastHLine(20,50,300,GREEN);
    tft.drawFastVLine(20,20,30,GREEN);
    tft.drawFastVLine(320,20,30,GREEN);
  */

  tft.setTextSize(2);

  // spennutexti
  tft.setTextColor(WHITE);
  tft.setCursor(80, 30);
  tft.print("Min Voltage: ");
  tft.setCursor(80, 60);
  tft.print("Total Voltage: ");

  // hitatexti
  //tft.setTextColor(ORANGE);
  tft.setCursor(80, 90);
  tft.print("Max Temperature: ");

  tft.setCursor(80, 120);
  tft.print("Speed: ");

}

float minV = 0;
float totalV = 0;
int maxT = 0;
float gps_speed = 0;

void loop(void) {
  while(true) {
  long timer = millis();
  while ( millis() - timer < 500 ) { // read CAN for 500ms
    while (CAN_MSGAVAIL != CAN.checkReceive() ) {
    }
    int id = read_CAN();
    if( id == TempMax || id == VoltMin || id == VoltTOT ) {
    Serial.print("CAN id: ");
    Serial.println(id,HEX);
    }

    switch (id) {
      case TempMax:
        data_int.buf[0] = buf[0];
        data_int.buf[1] = buf[1];
        maxT = data_int.data;
        break;
      case VoltMin:
        data_int.buf[0] = buf[0];
        data_int.buf[1] = buf[1];
        minV = (float) data_int.data / 10000.0;
        break;
      case VoltTOT:
        data_long.buf[0] = buf[0];
        data_long.buf[1] = buf[1];
        data_long.buf[2] = buf[2];
        data_long.buf[3] = buf[3];
        totalV = (float) data_long.data / 10000.0;
        break;
      case Speed:
        data_int.buf[0] = buf[0];
        data_int.buf[1] = buf[1];
        gps_speed = (float) data_int.data / 100.0;
    }
  }

  // Update screen
  
  tft.fillRect(285,90,80,15,BLACK);
  tft.setCursor(290, 90);
  tft.print(maxT);
  tft.fillRect(285,30,80,15,BLACK);
  tft.setCursor(290, 30);
  tft.print(minV);
  tft.fillRect(285, 60, 80, 15, BLACK);  // fillRect( x1, y1, x2 width, y2 height downwards)
  //tft.setTextColor(WHITE);
  tft.setCursor(290, 60);    // setCursor(x1, y1)
  tft.print(totalV);
  
  tft.fillRect(285, 120, 80, 15, BLACK);  // fillRect( x1, y1, x2 width, y2 height downwards)
  //tft.setTextColor(WHITE);
  tft.setCursor(290, 120);    // setCursor(x1, y1)
  tft.print(gps_speed);
  }
}



// Stores the 8 byte CAN data in buf
// Returns the CAN-ID or -1 if no mesege resived
int read_CAN() {
  canID = -1;
  CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
  canID = CAN.getCanId();
  return canID;
}

// Converts the char array to a float
float PWM_char2float() {

  data_float.buf[0] = buf[5];
  data_float.buf[1] = buf[4];
  data_float.buf[2] = buf[3];
  data_float.buf[3] = buf[2];

  float current = data_float.data;
  return current;
}

// Clears the message buffer
void reset_buffer() {
  for (uint8_t count = 0; count < 8; count++) {
    buf[count] = 0x00;
  }
}


