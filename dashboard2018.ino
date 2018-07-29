// https://learn.adafruit.com/adafruit-gfx-graphics-library

// Við viljum hæsta hitastig í acc, lægstu spennu á sellu og heildarspennu
// Það væri líka frábært að hafa eithvað sem gefur til kynna að það sé villa í pedalaboxinu. 
// Implosebilaty og það dót

// Ef hiti er notaður þarf að deila með 10


#include <SPI.h>
#include "mcp_can.h"
#include "CAN_ID.h"

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library

// The control pins for the LCD
#define LCD_CS A3 
#define LCD_CD A2 
#define LCD_WR A1 
#define LCD_RD A0 
#define LCD_RESET A4 

//Variable declaration

const int SPI_CS_PIN = 9;  // SPI chip select pin
unsigned char RxTxBuf[8];  // data buffer
int canID;                 // CAN ID of resiving mesage
union Data{                // Convert char to float
  char data_buf[4];
  long mesurement;
};
String message = "";         // String for serial message
float send_data[2] ={0, 0};  // stores mesured data befor serial transmit

MCP_CAN CAN(SPI_CS_PIN);    // Set CS pin


//Function declarations

int resieve_Mesege(char buf[8]);
float convert_data(char buf[8]);
void reset_buffer();
void power_config();


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

int speed = 7;

// The screen
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

void setup(void) {

  //tft.reset();

}

// 480x320
void loop(void) {

  // Raunverulegt setup
  
  uint16_t identifier = tft.readID();
  identifier=0x9341;
  tft.begin(identifier);

  tft.setRotation(3);

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
  tft.setCursor(20,80);
  tft.print("Min Voltage:    "); 
  tft.setCursor(20,110);
  tft.print("Tot Voltage: "); 

  // hitatexti
  tft.setTextColor(ORANGE);
  tft.setCursor(20,150);
  tft.print("Accumulator temperature: ");

  // Error svæði
  tft.fillRect(250,200,230,120,BLUE);
  tft.setCursor(260,210);
  tft.setTextColor(WHITE);
  tft.print("Error area, will"); 
  tft.setCursor(260,240);
  tft.print("be black/invisible");

  //Komment
  //tft.setCursor(300,90);
  //tft.setTextColor(YELLOW);
  //tft.print("Elding!!!");

  //tft.setCursor(50,240);
  //tft.print("Pikachu!");

  uint16_t speedColor;
  
  // raunverulegt loop
  while(true){

    if(CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
    {
      CanRead();
    }
    
    // lesa hraða og birta grafískt ásamt tölu.
    //int speed = random(0,150);
    if (speed > 140) speed = random(0,150);
    else speed = speed+1;

    tft.setTextSize(3);
    
    if (speed<50) speedColor = GREEN;
    else if (speed<100) speedColor = YELLOW;
    else speedColor = RED;
    tft.drawFastHLine(20,20,300,speedColor);
    tft.drawFastHLine(20,50,300,speedColor);
    tft.drawFastVLine(20,20,30,speedColor);
    tft.drawFastVLine(320,20,30,speedColor);
    tft.fillRect(20,20,2*speed,30,speedColor);
    if (speed == 0) tft.fillRect(21,21,299,29,BLACK);
    else tft.fillRect(2*speed+20,21,300-2*speed,29,BLACK);
    tft.fillRect(360,20,120,30,BLACK);
    tft.setCursor(360,25);
    tft.setTextColor(speedColor);
    //tft.setTextSize(3);
    tft.print(speed);
    tft.println(" km/h");

    // mesta og minnsta spenna (búa til fylki og leggja saman)
    // Rafhlaðan hefur 144 sellur, hver á að vera 4,15 V.
    // (3.7 V nom 3.3-4.15)
    
    double cells[144];
    double totVolt = 0;
    double minVolt = 10;
    for (int i = 0; i < 144; i++){
      cells[i] = random(3.0,4.5);
      totVolt = totVolt + cells[i];
      if (cells[i] < minVolt) minVolt = cells[i];
    }
    
    tft.fillRect(200,61,40,70,BLACK);
    tft.setTextColor(WHITE);
    tft.setCursor(200,80);
    tft.print(minVolt); tft.print(" V");
    tft.setCursor(200,110);
    tft.print(totVolt); tft.print(" V");

    // Rafhlaða
   
    // tft.fillRect(280,80,351,20,BLACK);
    if (totVolt > 530) tft.fillRect(280,80,20,20,GREEN);
    else { 
      tft.fillRect(280,80,20,20,BLACK);
      tft.fillRect(302,80,20,20,BLACK);
      tft.fillRect(324,80,20,20,BLACK);
      tft.fillRect(346,85,5,10,BLACK);
    }
    if (totVolt > 550) tft.fillRect(302,80,20,20,GREEN);
    else{
      tft.fillRect(302,80,20,20,BLACK);
      tft.fillRect(324,80,20,20,BLACK);
      tft.fillRect(346,85,5,10,BLACK);
    }
    if (totVolt > 580) tft.fillRect(324,80,20,20,GREEN);
    else{
      tft.fillRect(324,80,20,20,BLACK);
      tft.fillRect(346,85,5,10,BLACK);
    }
    if (totVolt > 595) tft.fillRect(346,85,5,10,GREEN);
    else tft.fillRect(346,85,5,10,BLACK);

    // Accumulator temperature
    int temp = random(40,80);
    tft.fillRect(320,130,35,50,BLACK);
    tft.setTextColor(ORANGE);
    tft.setCursor(320,150);
    tft.print(temp); tft.print(" C");

    delay(500);
  }
}



// Hér fyrir neðan eru ýmis CAN-föll
// waits for each mesurement and saves it in the send_data array
void get_mesurements(){

  while(resive_Mesege(RxTxBuf) != CURRENT);   // Waits for current mesurement
      send_data[0] = convert_data(RxTxBuf);   // Saves mesurement to array
  

  while(resive_Mesege(RxTxBuf) != VOLTAGE1); 
      send_data[1] = convert_data(RxTxBuf);
  
}


// Stores the 8 byte CAN data in buf
// Returns the CAN-ID or -1 if no mesege resived
int resive_Mesege(unsigned char buf[8]){
  
  unsigned char len = 0; // Stores mesage length
  union Data Mesurements; 
  canID = -1;
    
  if(CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
  {
      CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
      canID = CAN.getCanId();
      
  }
  return canID;
}

// Converts the char array to a float
float convert_data(unsigned char buf[]){
  
  union Data Mesurements;
  
  Mesurements.data_buf[0] = buf[5];
  Mesurements.data_buf[1] = buf[4];
  Mesurements.data_buf[2] = buf[3];
  Mesurements.data_buf[3] = buf[2];
  
  float current = Mesurements.mesurement;
  return current;
}


// Sets the configeration of power meater
void power_config(){

  // Set mode stop
  reset_buffer();
  RxTxBuf[0] = 0x34;
  RxTxBuf[2] = 0x01;  
  CAN.sendMsgBuf(CAN_COMMAND, 0, 8, RxTxBuf);

  
  reset_buffer();
  while(resive_Mesege(RxTxBuf) != CAN_RESPONSE);
  if(RxTxBuf[0] == 0xB4 && RxTxBuf[1] == 0x00){
      Serial.println("Mode: STOP");
  }

  // Store configerations
  reset_buffer();
  RxTxBuf[0] = 0x32;
  CAN.sendMsgBuf(CAN_COMMAND, 0, 8, RxTxBuf);

  reset_buffer();
  while(resive_Mesege(RxTxBuf) != CAN_RESPONSE);
  if(RxTxBuf[0] == 0xB2 && RxTxBuf[1] == 0x02){
      Serial.println("CONFIG STRED");
  }
  
  // Set mode run
  reset_buffer();
  RxTxBuf[0] = 0x34;
  RxTxBuf[1] = 0x01;
  RxTxBuf[2] = 0x01;  
  CAN.sendMsgBuf(CAN_COMMAND, 0, 8, RxTxBuf);
  
  reset_buffer();
  while(resive_Mesege(RxTxBuf) != CAN_RESPONSE);
  if(RxTxBuf[0] == 0xB4 && RxTxBuf[1] == 0x01){
    Serial.println("Mode: RUN");
  }
  
}

// Clears the message buffer
void reset_buffer(){
  for(uint8_t caunt = 0; caunt < 8; caunt++){
    RxTxBuf[caunt] = 0x00;
  }
}

// Read message from CAN BUS and transmit it over XBee module
void CanRead() 
{
  CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
  int canId = CAN.getCanId();
  String nameCanId = nameSender(canId);
  if( nameCanId != "NA$")                 // if the id is recognized
    {
      int nameLength = nameCanId.length();
      char Idname[nameLength+1];
      nameCanId.toCharArray(Idname, nameLength+1);
      // add message
      // if message is from Powermeter
      if(canId == 0x521 || canId == 0x524 || canId == 0x525 || canId == 0x526 || canId == 0x528)
      {
        union Data Mesurments;
        Mesurments.data_buf[0] = buf[5];
        Mesurments.data_buf[1] = buf[4];
        Mesurments.data_buf[2] = buf[3];
        Mesurments.data_buf[3] = buf[2];
      
        float current = Mesurments.mesurment;
        if(canId == 0x525)
        {
          current = current / 10;
        }
          
        if(current > 10000)   // if data is to large throw it away 
        {
          return;
        }
        // add message name
        for(int i = 0; i < nameLength; i++) 
        {
          MsgBuff[i+indexBuff] = Idname[i];
        }
        indexBuff = indexBuff + nameLength;
        // add message
        String tmp = String(current);
        for(int t = 0; t < tmp.length(); t++) 
        {
          MsgBuff[indexBuff+t] = tmp[t];
        }
        MsgBuff[indexBuff+tmp.length()] = '$';
        indexBuff = indexBuff + tmp.length() + 1;
      }
      else
      {
        // add message name
        for(int i = 0; i < nameLength; i++) {
          MsgBuff[i+indexBuff] = Idname[i];
        }
        indexBuff = indexBuff + nameLength;
        // add message
        for(int j = 0; j < len; j++)
        {
          MsgBuff[j+indexBuff] = (char) buf[j];
        }
        MsgBuff[indexBuff+len] = '$';
        indexBuff = indexBuff + len + 1;
      }
      xbeeSend(indexBuff);  // send recieved message
  }
        /*  Used for debugging writes all recieved CAN messages to Serial
        Serial.println("-----------------------------");
        Serial.println("get data from ID: ");
        Serial.println(canId);
        for(int i = 0; i<len; i++)    // print the data
        {
            Serial.print(buf[i]);
            Serial.print("\t");
        }
        Serial.println();
        */
}

// send message of length Msglength over Xbee
void xbeeSend(int Msglength) {
    //noInterrupts();
    /*
    for(int i = 0; i <= indexBuff; i++) {
      Serial.print(MsgBuff[i]);
    }
    indexBuff = 0;
    */
    // start message
    Serial.print("#");
    // send message of length Msglength over serial (Xbee)
    for(int i = 0; i < Msglength; i++) {
      Serial.print(MsgBuff[i]);
    }
    // Delete sent message from buffer
    for(int j = 0; j < Msglength && j < 50-Msglength; j++) {
      MsgBuff[j] = MsgBuff[j+Msglength];
    }
    indexBuff = indexBuff-Msglength;
    //Serial.println();
    //interrupts();
}

// Used to identify revieced CAN message to transmit over XBee
String nameSender(int canID) {
  switch(canID) {
    case 0x01:
      return "Volt$";
    case 0x02:
      return "Ohm$";
    case 0x03:
      return "Amp$";
    case 0x521:
      return "PCurrent$";
    case 0x524:
      return "PVolt$";
    case 0x525:
      return "PTemp$";
    case 0x526:
      return "PPower$";
    case 0x528:
      return "PTotEner$";
    default:
      return "NA$";
  }
}



