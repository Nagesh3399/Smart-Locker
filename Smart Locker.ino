#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SoftwareSerial.h>

SoftwareSerial SIM900A(10,11); //10=tx of sim900a

#define Password_Length 7
int  LASER=12;
int  LED_BEEP=13;
int  OTP=0;
char Data[Password_Length];
char Master[Password_Length] = "B2*A57";
byte data_count = 0, master_count = 0;
char customKey;
int Laser_Value=120;

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = 
{
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9,8,7,6};
byte colPins[COLS] = {5,4,3,2};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

LiquidCrystal_I2C lcd(0x27,16,2);  

void setup()
{
  SIM900A.begin(9600);   // Setting the baud rate of GSM Module  
  Serial.begin(9600);
  lcd.init();
  lcd.clear(); 
  lcd.backlight();
  randomSeed(analogRead(A1));
  pinMode(LASER, OUTPUT);// trial led for otp verification
  pinMode(LED_BEEP, OUTPUT);
  digitalWrite(LASER,HIGH); 
}

void loop()
{
  if (OTP==0)
  {
  lcd.setCursor(1,0);
  lcd.print("Enter Password");
  }
  else
  {
  lcd.setCursor(3,0);
  lcd.print("Enter OTP");
  }
  int ldr=analogRead(A1);
  int PWMValue=map(ldr,0,1023,0,255);
  Serial.println(PWMValue);
  
     if (analogRead(A0)>200)
    {
      digitalWrite(LASER,HIGH);
      analogWrite(A3,255);
      analogWrite(A2,0);
      delay(450);
      analogWrite(A2,0);
      analogWrite(A3,0); 
    } 
    
   if(PWMValue <  Laser_Value) 
   {
    for (int i=0;i<=3;i++)
    {
    digitalWrite(LED_BEEP,HIGH);
    delay(250);    
    digitalWrite(LED_BEEP,LOW);
    delay(250);
    }
     SendMessage2();
   }
   
  
  customKey = customKeypad.getKey();
  if(customKey)
  {
    Serial.println(customKey);
    Data[data_count] = customKey; 
    lcd.setCursor(7,data_count); 
    lcd.print("*"); 
    data_count++; 
  }

  if(data_count == Password_Length-1)
  {
    lcd.clear();
    if(!strcmp(Data, Master))
    {
      lcd.setCursor(1,6);
      lcd.print("Correct"); 
      delay(2000);
      analogWrite(A3,0);
      analogWrite(A2,255);
      delay(450);
      analogWrite(A2,0);
      analogWrite(A3,0); 
      SendMessage1();
     }
    else 
    {
      lcd.setCursor(1,6);
      lcd.print("Incorrect");
      delay(4000);
     }
    lcd.clear();
    clearData();  
  }
  
 else if((data_count == 4)&&(OTP!=0))
 { 
   String OTP1=String(OTP);
   String Data1=String(Data);
   lcd.clear();
   if(Data1.equals(OTP1))
   {
    lcd.setCursor(1,0);
    lcd.print("Verification");
    lcd.setCursor(5,1);
    lcd.print("Successful");
    OTP=0;
    SendMessage3();
    Laser_Value=1;
    digitalWrite(LASER,LOW);
   }
    else
    {
      lcd.setCursor(1,0);
      lcd.print("Incorrect OTP");
    }
    lcd.clear();
    clearData();
  }
}

void clearData()
{
  while(data_count !=0)
  {
    Data[data_count--] = 0; 
  }
  return;
}


void SendMessage1()
{
  OTP=rnd(); 
  Serial.println(OTP);
  Serial.println ("Sending Message please wait....");
  SIM900A.println("AT+CMGF=1");    //Text Mode initialisation 
  delay(1000);
  Serial.println ("Set SMS Number");
  SIM900A.println("AT+CMGS=\"+916363407487\"\r"); // Receiver's Mobile Number
  delay(1000);
  Serial.println ("Set SMS Content");
  SIM900A.println(OTP);// Messsage content
  delay(1000);
  Serial.println ("Done");
  SIM900A.println((char)26);  //   delay(1000);
  Serial.println ("Message sent succesfully");
 
}
void SendMessage2()
{
SIM900A.println("AT+CMGF=1");    //Text Mode initialisation 
delay(500);
SIM900A.println("AT+CMGS=\"+916363407487\"\r"); // Receiver's Mobile Number
delay(500); 
SIM900A.println("Warning!! Security Breach");// Messsage content
delay(500);
SIM900A.println((char)26);  //   delay(1000);
}

void SendMessage3()
{
   
  Serial.println ("Sending Message please wait....");
  SIM900A.println("AT+CMGF=1");    //Text Mode initialisation 
  delay(1000);
  Serial.println ("Set SMS Number");
  SIM900A.println("AT+CMGS=\"+916363407487\"\r"); // Receiver's Mobile Number
  delay(1000);
  Serial.println ("Set SMS Content");
  SIM900A.println("Authorization Successful");// Messsage content
  delay(1000);
  Serial.println ("Done");
  SIM900A.println((char)26);  //   delay(1000);
  Serial.println ("Message sent succesfully");

}
int rnd()
 {
  int b;
  b=random(1000,9999);
  return b;
 } //Random generator
