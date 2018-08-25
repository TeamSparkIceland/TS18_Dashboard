
#include <Arduino.h>
#include <stdint.h>


String message = "";         // String for serial message
float send_data[4] = {0, 0, 0, 0}; // stores measured data before serial transmit

/*

void error() {  
{
  // Error svæði
  /*
  tft.fillRect(250,200,230,120,BLUE);
  tft.setCursor(260,210);
  tft.setTextColor(WHITE);
  tft.print("Error area, will"); 
  tft.setCursor(260,240);
  tft.print("be black/invisible");
  

  uint16_t speedColor;
  
  // raunverulegt loop
  while(true){

    message = "";      // Empty string for new transmition
    get_mesurements(); // Saves the mesurements in send_data array

    // Tökum algildi af spennu- og straummælingum.
    if(send_data[0] < 0){
      send_data[0]*=(-1);
    }
    if(send_data[1] < 0){
      send_data[1]*=(-1);
    }
    
    // lesa hraða og birta grafískt ásamt tölu.
    int speed = (int) send_data[1]/10;
    
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
    
    int cells[144];
    int totVolt = (int) send_data[1]/10000;
    
    //Senda minVolt sérstaklega?
    int minVolt = 10;
    for (int i = 0; i < 144; i++){
      cells[i] = random(3.0,4.5);
      //totVolt = totVolt + cells[i];
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
    int temp = send_data[0];
    tft.fillRect(320,130,35,50,BLACK);
    tft.setTextColor(ORANGE);
    tft.setCursor(320,150);
    tft.print(temp); tft.print(" C");
  }



  // Hér fyrir neðan eru ýmis CAN-föll
// waits for each mesurement and saves it in the send_data array
void get_mesurements() {

  if (resive_Mesege(RxTxBuf) == TempAv);     // Waits for current mesurement
  send_data[0] = convert_data(RxTxBuf);  // Saves mesurement to array

  if (resive_Mesege(RxTxBuf) == VoltAv);
  send_data[1] = convert_data(RxTxBuf);

  if (resive_Mesege(RxTxBur) == Speed);
  send_data[2] = convert_data(RxTxBuf);

  if (resive_Mesege(RxTxBuf) == VoltMin);
  send_data[3] = convert_data(RxTxBuf);

}
*/



