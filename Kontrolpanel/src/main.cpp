//************************************************* Program description *******************************************************
/*
 
 */

//****************************************************************************************************************************
//************************************************* libraries ****************************************************************
//#include <ds3231.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//****************************************************************************************************************************
//************************************************* Pin Setup ****************************************************************


 static const short int COL1 = 19;//              COL1   COL2    COL3     COL4
 static const short int COL2 = 18;//               |       |       |       |
 static const short int COL3 = 17;//      ROW1 - |*1*| - |*2*| - |*3*| - |*A*| 
 static const short int COL4 = 16;//               |       |       |       |
                                  //      ROW2 - |*4*| - |*5*| - |*6*| - |*B*| 
 static const short int ROW1 = 23;//               |       |       |       |
 static const short int ROW2 = 25;//      ROW3 - |*7*| - |*8*| - |*9*| - |*C*|
 static const short int ROW3 = 33;//               |       |       |       |
 static const short int ROW4 = 26;//      ROW4 - |*0*| - |*F*| - |*E*| - |*D*|


//*************************************************************************************************************************  
//************************************************* EEPROM config *********************************************************

#define EEPROM_size 12

LiquidCrystal_I2C lcd(0x3F, 20, 4); // I2C address 0x27, 16 column and 2 rows
int Password1[4];
char key = 'n';

 
//*************************************************************************************************************************  
//************************************************* Function Prototype ****************************************************
char getkey_keypad();
char scan_keypad();
int enter_Password();
void change_Password();
//************************************************************************************************************************* 
//************************************************************************************************************************* 

void setup()
{
  Serial.begin(115200);
  EEPROM.begin(EEPROM_size);
  
  lcd.init(); // initialize the lcd
  lcd.backlight();
  
  pinMode(COL1,OUTPUT);
  pinMode(COL2,OUTPUT);
  pinMode(COL3,OUTPUT);
  pinMode(COL4,OUTPUT);
  
  pinMode(ROW1,INPUT);
  pinMode(ROW2,INPUT);
  pinMode(ROW3,INPUT);
  pinMode(ROW4,INPUT);
  
  /*
  EEPROM.write(0,1);
  EEPROM.write(1,2);
  EEPROM.write(2,3);
  EEPROM.write(3,4);
*/

}
void loop()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("UTC: 08:47");
  
  lcd.setCursor(0,1);
  lcd.print("ROOM TEMP 22 grader");
  
  lcd.setCursor(0,2);
  lcd.print("CO2 900 ppm");

  lcd.setCursor(0,3);
  lcd.print("Menu Press 8");


  if(getkey_keypad() == 8)
  {
   // enter_Password();
    
    if(enter_Password()== 1)
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Change Clock Press 1");
    
      lcd.setCursor(0,1);
      lcd.print("Change Temp Press  2");
  
      lcd.setCursor(0,2);
      lcd.print("New Password Press 3");
   
     switch(getkey_keypad())  
      { 
        case 3: 
        change_Password();
        break;
      
      }
    
  }
}
}
char getkey_keypad()
  {
    char key = 'n';   
    while(key == 'n')
    key = scan_keypad();
    return key-48;
  }
  
char scan_keypad()
  {
  digitalWrite(COL1,0); digitalWrite(COL2,HIGH); digitalWrite(COL3,HIGH); digitalWrite(COL4,HIGH);

    if(digitalRead(ROW1) == LOW){delay(300); return '1';}
    if(digitalRead(ROW2) == LOW){delay(300); return '4';}
    if(digitalRead(ROW3) == LOW){delay(300); return '7';}
    if(digitalRead(ROW4) == LOW){delay(300); return '0';}
  
  digitalWrite(COL1,HIGH); digitalWrite(COL2,LOW); digitalWrite(COL3,HIGH); digitalWrite(COL4,HIGH);

    if(digitalRead(ROW1) == LOW){delay(300); return '2';}
    if(digitalRead(ROW2) == LOW){delay(300); return '5';}
    if(digitalRead(ROW3) == LOW){delay(300); return '8';}
    if(digitalRead(ROW4) == LOW){delay(300); return 'F';}

  digitalWrite(COL1,HIGH); digitalWrite(COL2,HIGH); digitalWrite(COL3,LOW); digitalWrite(COL4,HIGH);

    if(digitalRead(ROW1) == LOW){delay(300); return '3';}
    if(digitalRead(ROW2) == LOW){delay(300); return '6';}
    if(digitalRead(ROW3) == LOW){delay(300); return '9';}
    if(digitalRead(ROW4) == LOW){delay(300); return 'E';}

  digitalWrite(COL1,HIGH); digitalWrite(COL2,HIGH); digitalWrite(COL3,HIGH); digitalWrite(COL4,LOW);

    if(digitalRead(ROW1) == LOW){delay(300); return 'A';}
    if(digitalRead(ROW2) == LOW){delay(300); return 'B';}
    if(digitalRead(ROW3) == LOW){delay(300); return 'C';}
    if(digitalRead(ROW4) == LOW){delay(300); return 'D';}

  return 'n';
  }  


  int enter_Password()
  {
    int compare_array[4];
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Enter Password");

    for(int i = 0; i < 4;i++)
    {
    compare_array[i] = getkey_keypad();
    lcd.setCursor(i,1);
    lcd.print(compare_array[i]);
    
    }

    delay(1000);
    
    if(compare_array[0] ==  EEPROM.read(0) &&  compare_array[1] ==  EEPROM.read(1) &&  compare_array[2] ==  EEPROM.read(2) && compare_array[3] ==  EEPROM.read(3))
    {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Password correct");
    delay(1000);
    return 1;
    }
    else
    {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Password incorrect");
    delay(1000);
    return 0;
    }
  }

void change_Password()
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Type in new password");
    for(int i=0; i < 4; i++)
    {
    Password1[i] = (getkey_keypad());
    EEPROM.write(i,Password1[i]);
    EEPROM.commit();
    lcd.setCursor(i,1);
    lcd.print(Password1[i]);
    }
    delay(1000);

}