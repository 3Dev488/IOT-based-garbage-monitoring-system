/*THIS TUTORIAL USED GSM SIM900A MINI V3.9.2
 
  Connect 5VT to D9 and 5VR to D10
  Feed GSM SIM900A with Arduino's 5V

*/

#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
SoftwareSerial mySerial(9, 10);
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
int temp=0,i=0,x=0,k=0;
const int read_dustbin=8;
char str[100];
char msg[100];
 int a_10=0;
 int dustbin_status =0;
void setup()
{
  pinMode(read_dustbin, INPUT);
  mySerial.begin(9600);   // Setting the baud rate of GSM Module  
  Serial.begin(9600);    // Setting the baud rate of Serial Monitor (Arduino)
  Serial.println("GSM SIM900A BEGIN");
  delay(100);
    lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("WELCOME");
delay(1000);
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("  IoT Based");
lcd.setCursor(0, 1);
lcd.print("Smart Dustbin");
delay(2000);
}

void loop()
{  
 byte b;
   char* B;
   static int cnt_loop;
   static int reset_bit;
  for(unsigned int t=0;t<60000;t++)
  {
    
    serialEvent();
  if(temp==1)
  {
    x=0,k=0,temp=0;
    while(x<i)
    {
      while(str[x]=='*')
      {
        x++;
        while(str[x]!='#')
        {
          msg[k++]=str[x++];
        }
      }
      x++;
    }
    msg[k]='\0';
    Serial.println("msg is");
     Serial.println(msg);
     lcd.clear();
lcd.setCursor(0, 0);
lcd.print("lat. & lon.");
lcd.setCursor(0,1);
lcd.print(msg);
delay(1000);
     dustbin_status=digitalRead(read_dustbin);
     Serial.println("dustbin_status");
     Serial.println(dustbin_status);
     lcd.clear();
lcd.setCursor(0, 0);
lcd.print("dustbin status:");
lcd.print(dustbin_status);
delay(500);
     if(dustbin_status == HIGH)
     {
      lcd.setCursor(0,1);
lcd.print("send msg...");
   Serial.println("send msg");
   mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\"+917776004923\"\r"); // Replace x with mobile number
  delay(1000);
  mySerial.println("Dustbin full");// The SMS text you want to send
  mySerial.println(msg);// The SMS text you want to send
  delay(100);
   mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(5000);
  }
  }
  }
      
}

void serialEvent()
{
 
  while(Serial.available())
  {
    char ch=(char)Serial.read();
    
    if(ch == '*')
    {
    a_10=1;
    }
    if(a_10 == 1)
    {
      Serial.print(ch);
    str[i++]=ch;
    if(ch == '#')
    {
      a_10=0;
      temp=1;
      delay(1000);
    }
    }
  }
}




