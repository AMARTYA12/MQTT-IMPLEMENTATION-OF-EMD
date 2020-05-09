#include <TH02_dev.h>

#include <TH02_dev.h>

int sensorValue;
String str;

#include "Arduino.h"
#include "Wire.h" 
#include <SoftwareSerial.h>
#include <Time.h>
#include <TimeLib.h>
#include <SPI.h>
#include <SD.h>
#include <DS1307RTC.h>

#include <Wire.h>

#include "RTClib.h"
RTC_DS1307 rtc;

#define TH02_EN 1 
const int pin_scl= 2;
const int pin_sda =3;
File myFile;
int DATE_RTC,MONTH_RTC,YEAR_RTC,HOUR_RTC,MIN_RTC,SEC_RTC;
char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
#define LENG 31   //0x42 + 31 bytes equal to 32 bytes
unsigned char buf[LENG];

int PM01Value=0;          //define PM1.0 value of the air detector module
int PM2_5Value=0;         //define PM2.5 value of the air detector module
int PM10Value=0;
int a=0;  

void setup()
{
    Serial2.begin(9600);
    Serial2.setTimeout(1500);
    Serial.begin(9600);    
    Serial1.begin(9600);
    TH02.begin(pin_scl,pin_sda);
    Serial.begin(9600);
    while(!Serial);

  
while (! rtc.begin()) 
  {
    Serial.print("Couldn't find RTC");
   // while (1);
  }
   if (! rtc.isrunning()) 
  {
   Serial.print("RTC is NOT running!");
  }else {
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  
  } 
  pinMode(53, OUTPUT);
Serial.println("Initializing SD Card...");
if (!SD.begin(53)) {
Serial.println("initialization failed!");
while (1);
}
Serial.println("initialization done.");

File myFile = SD.open("Book1.csv", FILE_WRITE);
 

  // if the file is available, write to it:
  myFile.println("Date,Time,PM1.0.PM2.5,PM10.0,HUMIDITY,TEMPERATURE,MQ135");

myFile.close();
   
   // ope.beginn the file. note that only one file dan be open at a time,
// so you have to close this one before opening another.
// if the file opened okay, write to it:
if (myFile) {
  Serial.print("Writing to test.txt...");
 
  myFile.close();
  Serial.print("done");

} else {

Serial.println("error opening test.txt");
}  
}
 
void loop()
{ 
   if(a==0){
   if(Serial1.available()){
   String t=Serial1.readString(); 
   if(t.length()==14){
  
   Serial.println(t);
   int x=t.length();
   Serial.println("the Length");
   Serial.println(x);
   
   char date[20];
   for(int i=0;i<14;i++){
    date[i]=(t[i]);
   }
    
    DATE_RTC=(date[5]-48)*10+(date[6]-48);
    MONTH_RTC=date[4]-48;
    YEAR_RTC=(date[0]-48)*1000+(date[1]-48)*100+(date[2]-48)*10+(date[3]-48);
    HOUR_RTC=(date[7]-48)*10+(date[8]-48);
    MIN_RTC=(date[9]-48)*10+(date[10]-48);
    SEC_RTC=(date[11]-48)*10+(date[12]-48);
    Serial.print(" DATE_RTC:");
    Serial.print(DATE_RTC);
    Serial.print("\n");
    Serial.print("MONTH_RTC:");
    Serial.print(MONTH_RTC);
    Serial.print("\n");
    Serial.print("YEAR_RTC:");
    Serial.print(YEAR_RTC);
     Serial.print("\n");
     Serial.print("HOUR_RTC:");
    Serial.print(HOUR_RTC);
     Serial.print("\n");
    Serial.print("MIN_RTC:");
    Serial.print(MIN_RTC);
     Serial.print("\n");
     Serial.print("SEC_RTC:");
    Serial.println(SEC_RTC);
    rtc.adjust(DateTime(YEAR_RTC,MONTH_RTC,DATE_RTC,HOUR_RTC,MIN_RTC,SEC_RTC));  
    Serial.println("sensorValue ,Temperature:,Humidity:,PM1.0:,PM2.5:,PM1 0: ");
    a++;
    delay(1000);
   }
   }
   else{
    Serial.println("Error occured");
   }
   
   }


  if(a==1){ 
    DateTime now = rtc.now();
    

    Serial.print(now.hour());
    Serial.print(':');
    Serial.print(now.minute());
    Serial.print(':');
    Serial.print(now.second());
    Serial.print("   ");

   
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(" ,");
     Serial.print(now.day());
    Serial.print('/');
    Serial.print(now.month());
    Serial.print('/');
    Serial.print(now.year());
    Serial.print("-->");
    delay(100);
   sensorValue=analogRead(A8);
   
   Serial.print(sensorValue);
   Serial.print("\t");
   int temper = TH02.ReadTemperature(); 
   Serial.print(temper);
   Serial.print("C\r\t");
   
   int humidity = TH02.ReadHumidity();
   Serial.print(humidity);
   Serial.print("%\r\t");
    if(Serial2.find(0x42)){
    Serial2.readBytes(buf,LENG);

    if(buf[0] == 0x4d){
      if(checkValue(buf,LENG)){
        PM01Value=transmitPM01(buf); //count PM1.0 value of the air detector module
        PM2_5Value=transmitPM2_5(buf);//count PM2.5 value of the air detector module
        PM10Value=transmitPM10(buf); //count PM10 value of the air detector module
      }
    }
  }

  static unsigned long OledTimer=millis();
    if (millis() - OledTimer >=1000)
    {
      OledTimer=millis();

     
      Serial.print(PM01Value);
      Serial.print("  ug/m3");
      Serial.print("\t");
      Serial.print(PM2_5Value);
      Serial.print("  ug/m3");
      Serial.print("\t");
      Serial.print(PM10Value);
      Serial.print("  ug/m3");
      Serial.println();
    }
  
   delay(1000);
   str =String(sensorValue)+String (" ")+String(temper)+String(" ")+String(humidity)+String(" ")+String(PM2_5Value)+String(" ")+String(PM10Value);
   Serial1.println(str);
  File myFile = SD.open("Book1.csv", FILE_WRITE);
 // if the file is available, write to it


  if (myFile) {
   myFile.print( now.day());
   myFile.print("-");
   myFile.print(now.month());
   myFile.print("-");
   myFile.print(now.year());
   myFile.print(",");
   myFile.print(now.hour());
   myFile.print(":");
   myFile.print(now.minute());
   myFile.print(":");
   myFile.print(now.second());
   myFile.print(",");
   myFile.print(PM01Value);
   myFile.print(",");
   myFile.print(PM2_5Value);
   myFile.print(",");
   myFile.print(PM10Value);
   myFile.print(",");
    myFile.print(temper);
    myFile.print(",");
    myFile.print(humidity);
    myFile.print(",");
    myFile.print(sensorValue);
    myFile.println();
    myFile.close();

    // print to the serial port too:

  

  } 
  else {
    Serial.println("error opening text.txt");
  }

  }
}
       //define PM10 value of the air detector module




char checkValue(unsigned char *thebuf, char leng)
{
  char receiveflag=0;
  int receiveSum=0;

  for(int i=0; i<(leng-2); i++){
  receiveSum=receiveSum+thebuf[i];
  }
  receiveSum=receiveSum + 0x42;

  if(receiveSum == ((thebuf[leng-2]<<8)+thebuf[leng-1]))  //check the serial data
  {
    receiveSum = 0;
    receiveflag = 1;
  }
  return receiveflag;
}

int transmitPM01(unsigned char *thebuf)
{
  int PM01Val;
  PM01Val=((thebuf[3]<<8) + thebuf[4]); //count PM1.0 value of the air detector module
  return PM01Val;
}

//transmit PM Value to PC
int transmitPM2_5(unsigned char *thebuf)
{
  int PM2_5Val;
  PM2_5Val=((thebuf[5]<<8) + thebuf[6]);//count PM2.5 value of the air detector module
  return PM2_5Val;
  }

//transmit PM Value to PC
int transmitPM10(unsigned char *thebuf)
{
  int PM10Val;
  PM10Val=((thebuf[7]<<8) + thebuf[8]); //count PM10 value of the air detector module
  return PM10Val;
}
