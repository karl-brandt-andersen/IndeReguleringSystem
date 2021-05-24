//************************************************* Program description *******************************************************
/*
 This program was made for a controlpanel, whereas the user can regulate the tempeture and the co2 level in a room.
 The system contents of a keypad, LCD screen and are programmed on a ESP32.
 The user can store a 4 digit password on the memory flash storage on the ESP32.
 the password is use to get access to the regulation.  
 */
//****************************************************************************************************************************
//************************************************* libraries ****************************************************************
//#include <ds3231.h>
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "WiFiHandler.h"
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


//****************************************************************************************************************************  
//************************************************* Config *******************************************************************

#define EEPROM_size 4 // setting the size of memory addresses in the memory flash storage  
LiquidCrystal_I2C lcd(0x3F, 20, 4); // I2C address 0x27, 20 column and 4 rows

//****************************************************************************************************************************  
//************************************************* Globel variables *********************************************************
 
char key = 'n';     // Key from keypad, n default no key is pressed  
float stemp;        // Variable for temp setting 
int sco2;           // Variable for sco2 setting 

//**************************************************************************************************************************** 
//************************************************* Keypad functions *********************************************************

char scan_keypad(void)
  {
  digitalWrite(COL1,LOW); digitalWrite(COL2,HIGH); digitalWrite(COL3,HIGH); // set first column to low the rest to high
  //digitalWrite(COL4,HIGH);  

    if(digitalRead(ROW1) == LOW){delay(300); return '1';} 
    if(digitalRead(ROW2) == LOW){delay(300); return '4';}  
    if(digitalRead(ROW3) == LOW){delay(300); return '7';} 
    if(digitalRead(ROW4) == LOW){delay(300); return '0';}
  
  digitalWrite(COL1,HIGH); digitalWrite(COL2,LOW); digitalWrite(COL3,HIGH); // set second column to low the rest to high
  //digitalWrite(COL4,HIGH); 

    if(digitalRead(ROW1) == LOW){delay(300); return '2';}
    if(digitalRead(ROW2) == LOW){delay(300); return '5';}
    if(digitalRead(ROW3) == LOW){delay(300); return '8';}
    if(digitalRead(ROW4) == LOW){delay(300); return 'F';}

  digitalWrite(COL1,HIGH); digitalWrite(COL2,HIGH); digitalWrite(COL3,LOW); //set third column to low the rest to high
//digitalWrite(COL4,HIGH);
    if(digitalRead(ROW1) == LOW){delay(300); return '3';}
    if(digitalRead(ROW2) == LOW){delay(300); return '6';}
    if(digitalRead(ROW3) == LOW){delay(300); return '9';}
    if(digitalRead(ROW4) == LOW){delay(300); return 'E';}

 // digitalWrite(COL1,HIGH); digitalWrite(COL2,HIGH); digitalWrite(COL3,HIGH); digitalWrite(COL4,LOW); //set fire column to low the rest to high

 //   if(digitalRead(ROW1) == LOW){delay(300); return 'A';}
 //   if(digitalRead(ROW2) == LOW){delay(300); return 'B';}
 //   if(digitalRead(ROW3) == LOW){delay(300); return 'C';}
 //   if(digitalRead(ROW4) == LOW){delay(300); return 'D';}

  return 'n'; // return 'n' if none key 
  }  

  char menu_keypad(void)
  {

  digitalWrite(COL1,HIGH); digitalWrite(COL2,LOW); digitalWrite(COL3,HIGH); //digitalWrite(COL4,HIGH); // set second column to low the rest to high
    if(digitalRead(ROW3) == LOW){ return '8';}
  
  return 'n'; // return 'n' if none key 
  }  

  char getkey_keypad()
  {
    char key = 'n';   //
    while(key == 'n') // while key is 'n' wait for user to press a key.
    key = scan_keypad();
    return key-48;
    
  }
  
  //****************************************************************************************************************************  
  //************************************************* Password functions *******************************************************
  
  int enter_Password()
  {
    int compare_array[4]; // local variable array to store keys pressed by the user.
    lcd.clear();          
    lcd.setCursor(0,0);
    lcd.print("Enter Password");

    for(int i = 0; i < 4; i++) // loop 4 time, to store the 4 digit value in each element of the array  
    {
    compare_array[i] = getkey_keypad(); // the user store keys in the array
    lcd.setCursor(i,1); // move the position to ix1
    lcd.print(compare_array[i]); // print the array the user typed in on the lcd screen
    
    }

    delay(1000); // wait 1 second
    
    if(compare_array[0] ==  EEPROM.read(0) && // Comparing the 4 digit the user is typing in, with the storage value in Memory Flash
       compare_array[1] ==  EEPROM.read(1) && 
       compare_array[2] ==  EEPROM.read(2) && 
       compare_array[3] ==  EEPROM.read(3))
    {
    lcd.clear(); 
    lcd.setCursor(0,0); // move the position to 0x0 
    lcd.print("Password correct");
    delay(1000); // wait 1 second
    lcd.clear();
    return 1;  // if the user typed the same array in as the stored in memory flash, then return 1 for true.
    }
    else
    {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Password incorrect");
    lcd.clear();
    delay(1000); // wait 1 second
    return 0; // If the user didnt type the same array as the stored in memory flash, then return 0 for false
    }
  }

void change_Password()
{
    int Password[4]; // local variable for user to type in 
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Type in new password");
    for(int i=0; i < 4; i++)
    {
    Password[i] = getkey_keypad(); // wait for user to type in a key from keypad
    EEPROM.write(i,Password[i]); // write the value (addresse, value) 
    EEPROM.commit();  
    lcd.setCursor(i,1); // move the position to 
    lcd.print(Password[i]);
    }
    delay(1000); // wait 1 second 

}

//****************************************************************************************************************************  
//************************************************* set temp/co2 functions ***************************************************

void set_temp()
{
    int array_temp[3]; // Local array variable to store value for temp
    
    lcd.clear();
    lcd.setCursor(0,0); // move position to 0x0
    lcd.println("Set temp");
    lcd.setCursor(0,1); // move position to 0x1
    lcd.println("  , grader");
    
    array_temp[0] = (getkey_keypad()); 
    lcd.setCursor(0,1);  // Move position to 0x1
    lcd.print(array_temp[0]);
    
    array_temp[1] = (getkey_keypad());
    lcd.setCursor(1,1); // Move position to 1x1
    lcd.print(array_temp[1]);

    array_temp[2] = (getkey_keypad());
    lcd.setCursor(3,1); // Move position to 3x1
    lcd.print(array_temp[2]);
    delay(1000); // wait 1 second 

    stemp = (float) array_temp[0]*10 + array_temp[1]*1 + array_temp[2]*0.1; // making the array to a float 

    SendData(stemp,sco2); // send setpoint for temp
    
}


void set_co2()
{
    int array_co2[3]; // Local array variable 
    
    lcd.clear(); // clear screen
    lcd.setCursor(0,0); // move cursor to position 0x0
    lcd.println("Set co2");
    lcd.setCursor(0,1); // move cursor to position 0x1
    lcd.println("    ppm");
    
    array_co2[0] = getkey_keypad(); // waiting for the user to type in the first element in the array
    lcd.setCursor(0,1); // Move cursor to position 0x1
    lcd.print(array_co2[0]); // print first element in array 
    
    array_co2[1] = getkey_keypad(); // waiting for the user to type in the second element in the array
    lcd.setCursor(1,1); // move cursor to position 1x1 
    lcd.print(array_co2[1]); // print second element in array

    array_co2[2] = getkey_keypad(); // waiting for the user to type in the third element in the array
    lcd.setCursor(2,1); // move cursor to position 2x1
    lcd.print(array_co2[2]); // print third element in array

    sco2 = array_co2[0]*1000 + array_co2[1]*100 + array_co2[2]*10; + array_co2[3]; // making the array to a float 

    SendData(stemp,sco2); // send setpoint for so2 over wifi
}

//**************************************************************************************************************************** 
//************************************************* setup ********************************************************************

void setup()
{
  
  Serial.begin(115200); // begin serial port, set speed
  delay(1000); // wait 1 second 
  WifiSetup(); 
  delay(1000); // wait 1 second
  EEPROM.begin(EEPROM_size); // Begin EEPROM

  lcd.init(); // initialize the lcd
  lcd.backlight(); // turn on backlight
  
  pinMode(COL1,OUTPUT);
  pinMode(COL2,OUTPUT);
  pinMode(COL3,OUTPUT);
  pinMode(COL4,OUTPUT);
  
  pinMode(ROW1,INPUT);
  pinMode(ROW2,INPUT);
  pinMode(ROW3,INPUT);
  pinMode(ROW4,INPUT);
  
  
  EEPROM.write(0,2); // store(address, value)
  EEPROM.write(1,2); // store(address, value)
  EEPROM.write(2,2); // store(address, value)
  EEPROM.write(3,2); // store(address, value)


}

//****************************************************************************************************************************  
//************************************************* Main Program *************************************************************

void loop()
{
  while(menu_keypad() == '8') // If '8' is pressed go to menu  
  {
 
  SendData(0,0); // send 0 over wifi to update the value 
  lcd.clear();
  lcd.setCursor(0,0); // move the position to 0x0
  lcd.print("UTC: 08:47"); // show time 
  
  lcd.setCursor(0,1); // move the position to 0x1
  lcd.print("TEMP "+String(getTMP())+(char)223+"C"); // print the value from the heater

  lcd.setCursor(0,2); // move the position to 0x2
  lcd.print("CO2  "+String(getCO2())+" ppm"); // print the value from the co2 sensor

  lcd.setCursor(0,3); // move the position to 0x3
  lcd.print("Menu Press 8");
  
  //delay(300);
  }
  delay(500); // Wait 500ms
  
    
    if(enter_Password() == 1) 
    {
      lcd.clear();
      lcd.setCursor(0,0); // move the position to 0x0  
      lcd.print("Change Temp Press 1");
    
      lcd.setCursor(0,1); // move the position to 0x1  
      lcd.print("Change Co2 Press  2");
  
      lcd.setCursor(0,2); // move the position to 0x2  
      lcd.print("New Password Press 4");
   
     switch(getkey_keypad()) // Press a button from 1-3 to select what to set or change   
      { 
        case 1: 
        set_temp(); // If button '1' is pressed then set temp 
        break;
        
        case 2:
        set_co2(); // If button '2' is pressed then set temp
        break;

        case 3: 
        change_Password(); // If button '3' is pressed then set temp
        break;
      
      }
    
  }

}
//****************************************************************************************************************************  
//************************************************* Program end **************************************************************


 