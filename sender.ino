#include <EmonLib.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define ss 10
#define rst 3
#define dio0  2
EnergyMonitor emon1;

int counter = 0;
const int MPU_addr=0x68;
int16_t AcX,AcY,AcZ,befX,befY,befZ,prevX,prevY,prevZ;
int minVal=265; int maxVal=402;
double x,y,z;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {

  Serial.begin(9600);
  emon1.current(1, 111.1);
  while (!Serial);
  Serial.println("LoRa Sender");
  LoRa.setPins(ss, rst, dio0);
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);   
  Wire.endTransmission(true);

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
    }
  }

void loop() {
  LoRa.beginPacket();
  LoRa.print("2 : ");
  Wire.beginTransmission(MPU_addr); Wire.write(0x3B); Wire.endTransmission(false); Wire.requestFrom(MPU_addr,14,true); AcX=Wire.read()<<8|Wire.read(); AcY=Wire.read()<<8|Wire.read(); AcZ=Wire.read()<<8|Wire.read(); int xAng = map(AcX,minVal,maxVal,-90,90); int yAng = map(AcY,minVal,maxVal,-90,90); int zAng = map(AcZ,minVal,maxVal,-90,90);
  x= RAD_TO_DEG * (atan2(-yAng, -zAng)+PI); y= RAD_TO_DEG * (atan2(-xAng, -zAng)+PI); z= RAD_TO_DEG * (atan2(-yAng, -xAng)+PI);
  if(z<180)
  {
    z = 360-z;
  }
  double Irms = emon1.calcIrms(1480);
  Serial.print("Irms : ");
  Serial.print(Irms*0.3);
  Serial.print(" A ");
 

  if(Irms*0.3>=0.10){
      lcd.begin();
      lcd.backlight();
      lcd.setCursor(0, 0);
      lcd.print("Irms : ");
      lcd.print(Irms*0.3);
      lcd.print(" A ");
      lcd.setCursor(0, 1);
      lcd.print("Working...");
      LoRa.print("Current=");
      LoRa.print(Irms*0.3);
      LoRa.print(" A, ");
    }

  else{    
      lcd.begin();
      lcd.backlight();
      lcd.setCursor(0, 0);
      lcd.print("Irms : ");
      lcd.print(Irms*0.3);
      lcd.print(" A ");
      lcd.setCursor(0, 1);
      lcd.print("Warning!!!");
      LoRa.print("Current= ");
      LoRa.print(Irms*0.3);
      LoRa.print(" A, ");
    }

  LoRa.print("(X,Y)=(");
  LoRa.print(90-y);
  LoRa.print(",");
  LoRa.print(360-z);
  LoRa.print(") Degrees");
    
    if(Irms*0.3<0.10 and sqrt((90-y)*(90-y)+(360-z)*(360-z))>=10){
      LoRa.print(" !The pole fell and Electric problem! ");
    }
    else if(Irms*0.3<0.10){
      LoRa.print(" !Electric problem! ");
     }
     else if(sqrt((90-y)*(90-y)+(360-z)*(360-z))>=10){
      LoRa.print(" !The pole fell! ");
     }
  LoRa.endPacket();
  counter++;
  delay(6000);
  //delay(6000);
}
