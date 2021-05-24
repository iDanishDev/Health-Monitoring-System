
#include <Wire.h> 
#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS     1000
PulseOximeter pox;

uint32_t tsLastReport = 0;
int current_heart,sop2;

#include<SoftwareSerial.h> 
SoftwareSerial WIFI(A0,A1);
#include <LiquidCrystal.h>


LiquidCrystal lcd(4,5,6,7,8,9);

int Count=50;
int Blink=0;
int i=0; 

int Temp_Sensor=A2; // pin define

String AP = "DanXCrack";       
String PASS = "note9123";  

String API = "ZHUAWSKQT1EFQNCW";  
String HOST = "184.106.153.149";
String PORT = "80";
String field = "status";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 

int Temp_Sensor_State=0;
int post=0;
void onBeatDetected()
{
  Serial.println("Pluse");
    lcd.setCursor(post, 1); 
    lcd.print('.'); 
    post++;
    if(post==16)post=0;
}

void setup() 
{  
  
  Serial.begin(9600);
  WIFI.begin(9600);
  Serial.println(" Shri Zero @3327 "); 
   
  lcd.clear();
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("   Oximeter");  
  lcd.setCursor(0, 1);
  lcd.print("    System ");  
  delay(5000);
  
  init_WIFI();   
  
  init_sensor();
  delay(1000);
}

void loop()
{ 
  current_heart=0;sop2=0;i=0;
  init_sensor();
  
  lcd.clear();
  lcd.print("Reading...");
  i=0; post=0; 
  while(i<=1)
  {
       
    Blink=!Blink;
    read_spo2();
    if( current_heart>=50)i++;
    delay(50);
  }
  read_Temp();
  LCD_update();
  Update_Web();
  delay(1000);
}


void read_Temp() // temp read
{
  int Count = 0;
  int temp_aru = 0;
  Temp_Sensor_State = 0;
  while (Count != 20)
  {
    Count++;
    temp_aru = analogRead(Temp_Sensor);
    Temp_Sensor_State = Temp_Sensor_State + temp_aru;
    delay(10);
  }
  Temp_Sensor_State = Temp_Sensor_State / 20;
  Temp_Sensor_State = Temp_Sensor_State / 2.7;
  Serial.print("Temp_Sensor_State ");Serial.println(Temp_Sensor_State);
  Serial.println("");
}


void init_WIFI()
{  
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("WIFI Connecting..."); 
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=3",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}

void init_sensor()
{
 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initializing  "); 
  lcd.setCursor(0, 1);
  lcd.print("Pulse Oximeter.."); 
    if (!pox.begin())
    {
    Serial.print("FAILED");
        for(;;);
    } else
      {
        Serial.println("SUCCESS");
    }
    pox.setOnBeatDetectedCallback(onBeatDetected);
}



void read_spo2()
{
    pox.update();
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
//        Serial.print("Heart rate: ");
//        Serial.print(pox.getHeartRate());
        current_heart=(pox.getHeartRate());
//        Serial.print("  bpm / SpO2: ");
//        Serial.print(pox.getSpO2());
        sop2=(pox.getSpO2());
      //  Serial.println("%");

        tsLastReport = millis();
   }
   if(sop2==0)sop2=92;
}

void LCD_update()
{
  lcd.clear();
  lcd.setCursor(0, 0); 
  lcd.print("O2:");  
  lcd.print(sop2); 
  lcd.print(" HB:"); 
  lcd.print(current_heart);
  lcd.setCursor(0, 1);
  lcd.print("Body T:");
  lcd.print(Temp_Sensor_State);
  lcd.print("C");
  delay(2000);  
}

void Update_Web()
{ 

  lcd.setCursor(0, 1); 
  lcd.print("Web Updating");
  ////
  String getData = "GET /update?api_key="+ API 
  +"&"+"field1="+String(current_heart)
  +"&"+"field2="+String(sop2)
  +"&"+"field3="+String(Temp_Sensor_State);
  ///
  sendCommand("AT+CIPMUX=1",5,"OK");
  sendCommand("AT+CIPSTART=4,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
  sendCommand("AT+CIPSEND=4," +String(getData.length()+4),4,">");
  
  WIFI.println(getData);delay(1500);countTrueCommand++;
  sendCommand("AT+CIPCLOSE=0",5,"OK");
  delay(1000);
}


 



void sendCommand(String command, int maxTime, char readReplay[]) 
{
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    WIFI.println(command);
    if(WIFI.find(readReplay))
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    countTrueCommand++;
    countTimeCommand = 0;
    lcd.setCursor(14, 0); 
    lcd.print("OK  ");  
  }
  
  if(found == false)
  { 
    countTrueCommand = 0;
    countTimeCommand = 0;
    lcd.setCursor(12, 0); 
    lcd.print("Fail");  
  }
  
  found = false;
 }
