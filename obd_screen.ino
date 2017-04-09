/*  D4 :  RESET
    D5 :  CS
    D6 :  D/C
    D7 :  LED
    D11 : MOSI
    D12 : MISO
    D13 : SCK   
*/
//fillRectangle(INT16U poX, INT16U poY, INT16U length, INT16U width, INT16U color);
//drawNumber(long long_num,INT16U poX, INT16U poY,INT16U size,INT16U fgcolor);

#include <stdint.h>
#include <TFTv2.h>
#include <RFM69.h>
#include <SPI.h>

int Nplace1 = 0;int Oplace1 = 0;
int Nplace2 = 0;int Oplace2 = 0;

int mpgcolor = GRAY2;
int pushstate1 = 0;
int buttonState1 = 0;

int vehicleRPM = 0;
int vehicleVSS = 0;
int vehicleMAF = 0;
int vehicleIMP = 0;
int vehicleIAT = 0;
int vehicleFPS = 0;
int vehicleTPS = 0;
int vehicleMPG = 0;

int place1 = 0;int place2 = 0;int place3 = 0;int place4 = 0;int place5 = 0;int place6 = 0;int place7 = 0;int place8 = 0;

int Nvss1 = 0;int Nvss2 = 0;int Nvss3 = 0;
int Ovss1 = 0;int Ovss2 = 0;int Ovss3 = 0;

int Nrpm1 = 0;int Nrpm2 = 0;int Nrpm3 = 0;int Nrpm4 = 0;
int Orpm1 = 0;int Orpm2 = 0;int Orpm3 = 0;int Orpm4 = 0;

int Nmaf1 = 0;int Nmaf2 = 0;int Nmaf3 = 0;
int Omaf1 = 0;int Omaf2 = 0;int Omaf3 = 0;

int Nimp1 = 0;int Nimp2 = 0;int Nimp3 = 0;
int Oimp1 = 0;int Oimp2 = 0;int Oimp3 = 0;

int Niat1 = 0;int Niat2 = 0;int Niat3 = 0;
int Oiat1 = 0;int Oiat2 = 0;int Oiat3 = 0;

int Ntps1 = 0;int Ntps2 = 0;int Ntps3 = 0;
int Otps1 = 0;int Otps2 = 0;int Otps3 = 0;

int Nfps1 = 0;int Nfps2 = 0;int Nfps3 = 0;
int Ofps1 = 0;int Ofps2 = 0;int Ofps3 = 0;

int Nmpg1 = 0;int Nmpg2 = 0;int Nmpg3 = 0;
int Ompg1 = 0;int Ompg2 = 0;int Ompg3 = 0;


#define NODEID      2
#define NETWORKID   88
#define GATEWAYID   77
#define FREQUENCY   RF69_915MHZ
#define KEY         "abcdhondacivicsi" //has to be same 16 characters/bytes on all nodes, not more not less!
#define LED         9

const int lcdbrightness = 3;
const int button1 = 7;
const int mpgled = 8;

RFM69 radio;

bool promiscuousMode = false; //set to 'true' to sniff all packets on the same network

typedef struct {		
  int         VSS;  //speed
  int         RPM;  //rpm
  int         MAF;  //mass air flow
  int         IMP;  //intake manifold pressure
  int         IAT;  //intake air temperature
  int         TPS;   //throttle postition
  int         FPS;  //Ethanol fuel %
  int         MPG;

} Payload;
Payload theData;

void setup(){
   Serial.begin(38400);
   analogWrite(lcdbrightness, 255);
   TFT_BL_ON;                                      // turn on the background light   
   Tft.TFTinit();                                  // init TFT library
   
   pinMode(lcdbrightness, OUTPUT);
   pinMode(mpgled, OUTPUT);
   pinMode(button1, INPUT);
   Blink(LED,1000);

  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.encrypt(KEY);
  radio.promiscuous(promiscuousMode);
  char buff[50];
  sprintf(buff, "\nListening at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  
  Tft.drawString("2008 HONDA CIVIC SI",7,10,2,GRAY1);  //random(0xFFFF)
  Tft.drawHorizontalLine(0, 35, 240, 0xFFF0); // (x, y, length, color)
  Tft.drawHorizontalLine(0, 36, 240, 0xFFF0); // (x, y, length, color)
  Tft.drawString("JIMMY'S PROJECT",30,120,2,GRAY2);  //random(0xFFFF)
  Tft.drawString("SYSTEM READY",47,150,2,GRAY1);  //random(0xFFFF) 
  for (int i=0; i<240; i=i+12){
    Tft.drawString(">",i,185,2,0xCB20);
    digitalWrite(mpgled, HIGH);
    delay(200);
    digitalWrite(mpgled, LOW);
    delay(300);
  }
  Tft.fillRectangle(0, 120, 240,100,BLACK);
  
  Tft.drawString("2008 HONDA CIVIC SI",7,10,2,GRAY1);  //random(0xFFFF)
  Tft.drawHorizontalLine(0, 35, 240, 0xFFF0); // (x, y, length, color)
  Tft.drawHorizontalLine(0, 36, 240, 0xFFF0); // (x, y, length, color)
        
  Tft.drawString("MPH", 130, 50, 2, 0xFFF0);
  
  Tft.drawString("RPM", 130, 101, 2, 0xFFF0);
  
  Tft.drawString("MAF", 130, 152, 2, 0xFFF0);
  Tft.drawString("GRAM/S", 130, 173, 2, GRAY1);
  
  Tft.drawString("BOOST", 130, 203, 2, 0xFFF0);
  Tft.drawString("PSI", 130, 224, 2, GRAY1);
  
  Tft.drawHorizontalLine(0, 250, 240, 0xFFF0); // (x, y, length, color)
  Tft.drawHorizontalLine(0, 251, 240, 0xFFF0); // (x, y, length, color)
  
  Tft.drawString("IAT:", 0, 263, 2, 0xFFF0);
  Tft.drawCircle(92, 265, 2,GRAY1);
  Tft.drawString("F", 95, 263, 2, GRAY1);
  
  Tft.drawString("BAT:", 0, 290, 2, 0xFFF0);
  Tft.drawString("V", 90, 290, 2, GRAY1);
  
  Tft.drawString("TPS:", 135, 263, 2, 0xFFF0);
  Tft.drawString("%", 225, 263, 2, GRAY1);
  
  Tft.drawString("MPG:", 135, 290, 2, 0xFFF0);
  Tft.drawString("N", 225, 290, 2, GRAY1);
  

}

void loop(){
  
  buttonState1 = digitalRead(button1);
  
  if (buttonState1 == HIGH && pushstate1 == 0){
    analogWrite(lcdbrightness, 100);
    //digitalWrite(lcdbrightness, LOW);
    pushstate1 = 1;
    buttonState1 = LOW;
    delay(300);
  }
  
  if (buttonState1 == HIGH && pushstate1 == 1){
    analogWrite(lcdbrightness, 255);
    //digitalWrite(lcdbrightness, HIGH);
    pushstate1 = 0;
    buttonState1 = LOW;
    delay(300);
  }
  
  if (radio.receiveDone()){
    
      if (radio.DATALEN != sizeof(Payload))
            Serial.print("Invalid payload received, not matching Payload struct!");
      else
          {
            theData = *(Payload*)radio.DATA; //assume radio.DATA actually contains our struct and not something else

            
            vehicleRPM = theData.RPM;
            vehicleVSS = theData.VSS;
            vehicleIMP = theData.IMP;
            vehicleMAF = theData.MAF;
            vehicleIAT = theData.IAT;
            vehicleTPS = theData.TPS;
            vehicleFPS = theData.FPS;
            vehicleMPG = theData.MPG;
            
            Serial.print("MAF: ");
            Serial.println(vehicleMAF);
            Serial.print("RPM: ");
            Serial.println(vehicleRPM);
            
            Nvss1 = vehicleVSS/100; Nvss2 = (vehicleVSS/10)-(Nvss1*10); Nvss3 = vehicleVSS-Nvss1*100-Nvss2*10;
            Nrpm1 = vehicleRPM/1000; Nrpm2 = (vehicleRPM/100)-(Nrpm1*10); Nrpm3 = (vehicleRPM-Nrpm1*1000-Nrpm2*100)/10; Nrpm4 = vehicleRPM-Nrpm1*1000-Nrpm2*100-Nrpm3*10;
            Nmaf1 = vehicleMAF/100; Nmaf2 = (vehicleMAF/10)-(Nmaf1*10); Nmaf3 = vehicleMAF-Nmaf1*100-Nmaf2*10;
            Nimp1 = vehicleIMP/100; Nimp2 = (vehicleIMP/10)-(Nimp1*10); Nimp3 = vehicleIMP-Nimp1*100-Nimp2*10;
            Niat1 = vehicleIAT/100; Niat2 = (vehicleIAT/10)-(Niat1*10); Niat3 = vehicleIAT-Niat1*100-Niat2*10;
            Nfps1 = vehicleFPS/100; Nfps2 = (vehicleFPS/10)-(Nfps1*10); Nfps3 = vehicleFPS-Nfps1*100-Nfps2*10;
            Ntps1 = vehicleTPS/100; Ntps2 = (vehicleTPS/10)-(Ntps1*10); Ntps3 = vehicleTPS-Ntps1*100-Ntps2*10;
            Nmpg1 = vehicleMPG/100; Nmpg2 = (vehicleMPG/10)-(Nmpg1*10); Nmpg3 = vehicleMPG-Nmpg1*100-Nmpg2*10;
            
            if (vehicleVSS >= 100){
              place1 = 30;}
              else if (vehicleVSS <= 99 && vehicleVSS >=10) {
                place1 = 60;}
                else {
                  place1 = 90;}
              
            if (vehicleRPM >= 1000){
              place2 = 0;}
              else if (vehicleRPM <=999 && vehicleRPM >= 100){
                place2 = 30;}
                else if (vehicleRPM <= 99 && vehicleRPM >= 10){
                  place2 = 60;}
                  else{
                    place2 = 90;}
                    
             if (vehicleMAF >= 100){
               place3 = 30;}
               else if (vehicleMAF <= 99 && vehicleMAF >=10) {
                 place3 = 60;}
                 else {
                   place3 = 90;}
                  
             if (vehicleIMP >= 100){
               place4 = 30;}
               else if (vehicleIMP <= 99 && vehicleIMP >=10) {
                 place4 = 60;}
                 else {
                   place4 = 90;}
                   
            if (vehicleIAT >= 100){
               place5 = 50;}
               else if (vehicleIAT <= 99 && vehicleIAT >=10) {
                 place5 = 62;}
                 else {
                   place5 = 74;}
            
            if (vehicleFPS >= 100){
               place6 = 50;}
               else if (vehicleFPS <= 99 && vehicleFPS >=10) {
                 place6 = 62;}
                 else {
                   place6 = 74;} 
                   
            if (vehicleTPS >= 100){
               place7 = 186;}
               else if (vehicleTPS <= 99 && vehicleTPS >=10) {
                 place7 = 198;}
                 else {
                   place7 = 210;}
                   
              if (vehicleMPG >= 100){
               place8 = 186;}
               else if (vehicleMPG <= 99 && vehicleMPG >=10) {
                 place8 = 198;}
                 else {
                   place8 = 210;}
                   
            if (Nvss1 != Ovss1){Tft.fillRectangle(30, 50, 31,35,BLACK);}
            if (Nvss2 != Ovss2){Tft.fillRectangle(61, 50, 31,35,BLACK);}
            if (Nvss3 != Ovss3){Tft.fillRectangle(92, 50, 31,35,BLACK);}
            
            if (Nrpm1 != Orpm1){Tft.fillRectangle(0, 101, 31,35,BLACK);}
            if (Nrpm2 != Orpm2){Tft.fillRectangle(30, 101, 31,35,BLACK);}
            if (Nrpm3 != Orpm3){Tft.fillRectangle(61, 101, 31,35,BLACK);}
            if (Nrpm4 != Orpm4){Tft.fillRectangle(92, 101, 31,35,BLACK);}
            
            if (Nmaf1 != Omaf1){Tft.fillRectangle(30, 152, 31,35,BLACK);}
            if (Nmaf2 != Omaf2){Tft.fillRectangle(61, 152, 31,35,BLACK);}
            if (Nmaf3 != Omaf3){Tft.fillRectangle(92, 152, 31,35,BLACK);}
            
            if (Nimp1 != Oimp1){Tft.fillRectangle(30, 203, 31,35,BLACK);}
            if (Nimp2 != Oimp2){Tft.fillRectangle(61, 203, 31,35,BLACK);}
            if (Nimp3 != Oimp3){Tft.fillRectangle(92, 203, 31,35,BLACK);}
            
            if (Niat1 != Oiat1){Tft.fillRectangle(51, 263, 11,14,BLACK);}
            if (Niat2 != Oiat2){Tft.fillRectangle(63, 263, 11,14,BLACK);}
            if (Niat3 != Oiat3){Tft.fillRectangle(75, 263, 11,14,BLACK);}
            
            if (Nfps1 != Ofps1){Tft.fillRectangle(51, 290, 11,14,BLACK);}
            if (Nfps2 != Ofps2){Tft.fillRectangle(63, 290, 11,14,BLACK);}
            if (Nfps3 != Ofps3){Tft.fillRectangle(75, 290, 11,14,BLACK);}
            
            if (Ntps1 != Otps1){Tft.fillRectangle(187, 263, 11,14,BLACK);}
            if (Ntps2 != Otps2){Tft.fillRectangle(199, 263, 11,14,BLACK);}
            if (Ntps3 != Otps3){Tft.fillRectangle(211, 263, 11,14,BLACK);}
            
            if (Nmpg1 != Ompg1){Tft.fillRectangle(187, 290, 11,14,BLACK);}
            if (Nmpg2 != Ompg2){Tft.fillRectangle(199, 290, 11,14,BLACK);}
            if (Nmpg3 != Ompg3){Tft.fillRectangle(211, 290, 11,14,BLACK);}
            
            
            
            Tft.drawNumber(vehicleVSS, place1, 50, 5, RED); 
            // changing the speed cation indicator.
            if (vehicleVSS > 50){ 
              Nplace1 = 1;
              if (Oplace1 != Nplace1){
              Tft.drawString("NORMAL", 130, 71, 2, BLACK);
              }
              if (vehicleVSS <= 75 && vehicleVSS >= 50){
              Tft.drawString("CATION!", 130, 71, 2, 0xCB20);
              }
              else{
                Tft.drawString("CATION!", 130, 71, 2, RED);
              }  
            }
            else{
              Nplace1 = 0;
              if (Oplace1 != Nplace1){
              Tft.drawString("CATION!", 130, 71, 2, BLACK);
              }
              Tft.drawString("NORMAL", 130, 71, 2, GREEN);
            }
            Oplace1 = Nplace1;
            
            //display the rpm value.
            Tft.drawNumber(vehicleRPM, place2, 101, 5, RED);
            
            
            //changing the vtec output indicator.
            if (vehicleRPM > 6300){
              Nplace2 = 1;
              if (Oplace2 != Nplace2){
                Tft.drawString("VTEC OFF", 130, 122, 2, BLACK);
              }
              if (vehicleRPM <=7000 && vehicleRPM >= 6300){
                Tft.drawString("VTEC", 130, 122, 2, 0xCB20);
                Tft.drawString("ON", 190, 122, 2, GREEN);
              }
              else{
                Tft.drawString("VTEC", 130, 122, 2, RED);
                Tft.drawString("ON", 190, 122, 2, GREEN);
            }
            }
            else{
              Nplace2 = 0;
              if (Oplace2 != Nplace2){
                Tft.drawString("VTEC ON", 130, 122, 2, BLACK);
              }
              Tft.drawString("VTEC OFF", 130, 122, 2, GRAY1);
            }
            Oplace2 = Nplace2;
            
            Tft.drawNumber(vehicleMAF, place3, 152, 5, RED);       
            
            Tft.drawNumber(vehicleIMP, place4, 203, 5, RED);      
            
            Tft.drawNumber(vehicleIAT, place5, 263, 2, RED);     
            
            Tft.drawNumber(vehicleFPS, place6, 290, 2, RED);      

            Tft.drawNumber(vehicleTPS, place7, 263, 2, RED);      
                       
            
            if (vehicleMPG < 15){
              mpgcolor = RED;
              digitalWrite(mpgled, LOW);
            }
            else if (vehicleMPG <=30 && vehicleMPG >=15){
              mpgcolor = 0xCB20;
              digitalWrite(mpgled, LOW);
            }
            else {
              mpgcolor = GREEN;
              digitalWrite(mpgled, HIGH);
            }
            
            Tft.drawNumber(vehicleMPG, place8, 290, 2, mpgcolor);      

            
            Ovss1 = Nvss1;Ovss2 = Nvss2;Ovss3 = Nvss3;
            Orpm1 = Nrpm1;Orpm2 = Nrpm2;Orpm3 = Nrpm3;Orpm4 = Nrpm4;
            Omaf1 = Nmaf1;Omaf2 = Nmaf2;Omaf3 = Nmaf3;
            Oimp1 = Nimp1;Oimp2 = Nimp2;Oimp3 = Nimp3; 
            Oiat1 = Niat1;Oiat2 = Niat2;Oiat3 = Niat3;
            Ofps1 = Nfps1;Ofps2 = Nfps2;Ofps3 = Nfps3;
            Otps1 = Ntps1;Otps2 = Ntps2;Otps3 = Ntps3;  
            Ompg1 = Nmpg1;Ompg2 = Nmpg2;Ompg3 = Nmpg3;        
          }
      Blink(LED,10);
  }

}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
