
/**************************************************************************************

DESCRIPTION
  Firmware de microcontroleur programmable par touches - code développé pour ATtiny84
	  3 boutons (S1, S2, Reset)
	  4 entrées numériques (E1-E4)
	  2 entrées analogiques (AE1-AE2)
	  4 sorties numériques (A1-A4)
	  1 sortie PWM (PWM1)

***************************************************************************************/


#include <EEPROM.h>




#define S1      10
#define S2      9



#define PWM1    8

#define E1      7
#define E2      6
#define E3      5
#define E4      4

#define AE1     7
#define AE2     6

#define A1      3
#define A2      2
#define A3      1
#define A4      0





byte    eebyte = 0;       
byte    addr = 0;       
byte    dat = 0;        
byte    kom = 0;        
byte    adrlo = 0;        
byte    adrhi = 0;        
byte    adrret = 0;       
byte    prog = 0;       
byte    puffer = 0;       
word    ff = 0;         
word    ms = 0;         

byte    A = 0;          
byte    B = 0;          
byte    C = 0;          
byte    D = 0;          

unsigned long delayTbl[] = {1, 2, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000, 30000, 60000};
byte eepromTbl[] = {0x64, 0x51, 0x4E, 0x80, 0xC3, 0x98, 0x82, 0x95, 0x4D, 0x80, 0xC3, 0x9E, 0x82, 0x9A, 0x4B, 0x81, 0xC3, 0x94, 0x83, 0x90, 0x47, 0x81, 0xC3, 0x9A, 0x83, 0x94, 0x43, 0x82, 0xC3, 0x90, 0x84, 0x90, 0x11, 0x28, 0x18, 0x28, 0x34, 0x71, 0x54, 0x59, 0x26, 0x34, 0x69, 0x54, 0x59, 0x26, 0x34, 0xFF, 0x54, 0xCE, 0x71, 0x33, 0x22, 0xCC, 0x32, 0x40, 0x22, 0x71, 0x54, 0xCE, 0x34, 0x39, 0xFF, 0xFF, 0x86, 0xD0, 0x40, 0x71, 0x54, 0x23, 0xCD, 0x34, 0xD8, 0x40, 0x54, 0x3B, 0xFF, 0xFF, 0xFF, 0xFF, 0x4F, 0x93, 0x45, 0x53, 0x19, 0x11, 0x21, 0x19, 0x11, 0x21, 0x19, 0x11, 0x20, 0xB4, 0x10, 0xE0, 0x23, 0xCE, 0x32, 0x23, 0xCC, 0x31, 0xE0, 0xFF, 0x23, 0xCF, 0x32, 0x23, 0xCD, 0x31, 0xE0, 0xFF, 0xCC, 0x31, 0x40, 0x54, 0x23, 0xCE, 0x32, 0xCF, 0xE0, 0xCC, 0x33, 0x71, 0x23, 0xCC, 0x31, 0x3C};

void setup()
{
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);

  pinMode(PWM1, OUTPUT);

  pinMode(E1, INPUT_PULLUP);
  pinMode(E2, INPUT_PULLUP);
  pinMode(E3, INPUT_PULLUP);
  pinMode(E4, INPUT_PULLUP);

  pinMode(S1, INPUT_PULLUP);
  pinMode(S2, INPUT_PULLUP);

  analogReference(DEFAULT);

  addr = 0;                 
  prog = 0;

  if (digitalRead(S2) == LOW)
  {
    PROG();
  }

  if (digitalRead(S1) == LOW)
  {
    TEST();
  }
}

void PORT(byte dat)
{
  digitalWrite(A1, (dat & 1) ? HIGH:LOW);
  digitalWrite(A2, (dat & 2) ? HIGH:LOW);
  digitalWrite(A3, (dat & 4) ? HIGH:LOW);
  digitalWrite(A4, (dat & 8) ? HIGH:LOW);
}

byte PIN(void)
{
  return
    digitalRead(E1)+
    digitalRead(E2)*2+
    digitalRead(E3)*4+
    digitalRead(E4)*8;
}

byte PIN_BIT(byte pin)
{
  return (digitalRead(pin) == HIGH) ? 1:0;
}

void WAIT_RELEASE(byte pin)
{
  delay(40);
  while (digitalRead(pin)==LOW)
  {
    delay(5);
  }
  delay(40);
}


void loop()
{
  eebyte = EEPROM.read(addr);       
  dat = eebyte & 15;            
  kom = eebyte / 16;            
  addr = addr + 1;            

  if (kom == 1)             
  {
    PORT(dat);
  }

  if (kom == 2)             
  {
    if (dat < 16)
      delay(delayTbl[dat]);
  }
  
  if (kom == 3)             
  {
    addr = addr - 1;
    addr = addr - dat;
  }
  
  if (kom == 4)             
  {
    A = dat;
  }
  
  if (kom == 5)             
  {
    if (dat == 1) B = A;        
    if (dat == 2) C = A;
    if (dat == 3) D = A;
    if (dat == 4) PORT(A);        
    if (dat == 5)           
      digitalWrite(A1, A ? HIGH:LOW);
    if (dat == 6)     
      digitalWrite(A2, A ? HIGH:LOW);
    if (dat == 7)
      digitalWrite(A3, A ? HIGH:LOW);
    if (dat == 8)
      digitalWrite(A4, A ? HIGH:LOW);
    if (dat == 9)           
      analogWrite(PWM1, A*17);
  }
  
  if (kom == 6)             
  {
    if (dat == 1) A = B;        
    if (dat == 2) A = C;
    if (dat == 3) A = D;
    if (dat == 4) A = PIN();      
    if (dat == 5)           
      A = PIN_BIT(E1);
    if (dat == 6)
      A = PIN_BIT(E2);
    if (dat == 7)
      A = PIN_BIT(E3);
    if (dat == 8)
      A = PIN_BIT(E4);
    if (dat == 9)
      A = analogRead(AE1)/64;     
    if (dat == 10)
      A = analogRead(AE2)/64;
  }

  if (kom == 7)             
  {
    if (dat == 1) A = A + 1;
    if (dat == 2) A = A - 1;
    if (dat == 3) A = A + B;
    if (dat == 4) A = A - B;
    if (dat == 5) A = A * B;
    if (dat == 6) A = A / B;
    if (dat == 7) A = A & B;      
    if (dat == 8) A = A | B;
    if (dat == 9) A = A ^ B;
    if (dat == 10) A = ~A;
  }
  
  if (kom == 8) adrhi = dat;        
  
  if (kom == 9)             
  {
    addr = adrhi * 16;
    addr = addr + dat;
  }
  
  if (kom == 10)              
  {
    if (C > 0)
    {
      C = C - 1;
      C = C & 15;
      addr = adrhi * 16;
      addr = addr + dat;
    }
  }
  
  if (kom == 11)              
  {
    if (C > 0)
    {
      D = D - 1;
      D = D & 15;
      addr = adrhi * 16;
      addr = addr + dat;
    }
  }   
  
  if (kom == 12)              
  {
    if (dat == 1)
    {
      if (A > B) addr = addr + 1;
    }
    if (dat == 2)
    {
      if (A < B) addr = addr + 1;
    }     
    if (dat == 3)
    {
      if (A == B) addr = addr + 1;
    }
    if (dat == 4)
    {
      addr = addr + PIN_BIT(E1);
    }
    if (dat == 5)
    {
      addr = addr + PIN_BIT(E2);
    }
    if (dat == 6)
    {
      addr = addr + PIN_BIT(E3);
    }
    if (dat == 7)
    {
      addr = addr + PIN_BIT(E4);
    }
    if (dat == 8)
    {
      addr = addr + 1-PIN_BIT(E1);
    }
    if (dat == 9)
    {
      addr = addr + 1-PIN_BIT(E2);
    }
    if (dat == 10)
    {
      addr = addr + 1-PIN_BIT(E3);
    }
    if (dat == 11)
    {
      addr = addr + 1-PIN_BIT(E4);
    }
    if (dat == 12)
    {
      addr = addr + 1-PIN_BIT(S1);
    }
    if (dat == 13)
    {
      addr = addr + 1-PIN_BIT(S2);
    }
    if (dat == 14)
    {
      addr = addr + PIN_BIT(S1);
    }
    if (dat == 15)
    {
      addr = addr + PIN_BIT(S2);
    }
  }
  
  if (kom == 13)              
  {
    adrret = addr;
    addr = adrhi * 16;
    addr = addr + dat;
  }
  
  if (kom == 14)              
  {
    addr = adrret;
  }
  
  if (kom == 15 )             
  {
    if ((dat == 15) && (addr < 3))
    {
      ff++;
      if (ff == 2)
      {
        for (int i=0; i<128; i++)
        {
          EEPROM.update(i, eepromTbl[i]);
        }       
        ff = 0;
      }
    }
  }
}


void PROG()
{
  do                      
  {                     
    adrlo = addr & 15;            
    PORT(adrlo);              
    delay(300);
    PORT(0);                
    delay(200);
    eebyte = EEPROM.read(addr);       
    dat = eebyte & 15;            
    kom = eebyte / 16;            
    PORT(kom);                
    WAIT_RELEASE(S2);           
    prog = 1;               
    do                    
    {
      if (digitalRead(S1)==LOW)     
      {
        if (prog == 1)          
        {               
          prog = 2;
          kom = 15;       
        }
        if (prog == 2)          
        {               
          kom = kom + 1;        
          kom = kom & 15;
          PORT(kom);
        }
        if (prog == 3)          
        {               
          prog = 5;
          dat = 15;
        }
        if (prog == 4)          
        {               
          prog = 5;
          dat = 15;
        }
        if (prog == 5)          
        {               
          dat = dat + 1;        
          dat = dat & 15;
          PORT(dat);
        }
        WAIT_RELEASE(S1);       
      }
      if (digitalRead(S2)==LOW)     
      {
        if (prog == 3)          
        {               
          prog = 7;
        }
        if (prog == 1)          
        {
          PORT(dat);
          prog = 3;
        }
        if (prog == 4)          
        { 
          PORT(dat);
          prog = 6;
        }
        if (prog == 2)          
        {
          PORT(dat);
          prog = 4;
        }
        if (prog == 6)          
        {               
          dat = dat & 15;       
          eebyte = kom * 16;      
          eebyte = eebyte + dat;
          EEPROM.update(addr, eebyte);
          PORT(0);          
          delay(600);
          addr = addr + 1;      
          prog = 0;
        }
        if (prog == 5)          
        {               
          dat = dat & 15;       
          eebyte = kom * 16;      
          eebyte = eebyte + dat;
          EEPROM.update(addr, eebyte);
          PORT(0);          
          delay(600);
          addr = addr + 1;      
          prog = 0;         
        }
        if (prog == 7)          
        {
          addr = addr + 1;
          prog = 0;
        }
        WAIT_RELEASE(S2);       
      }
    }
    while(!prog == 0);
  }
  while (!0);
}
  

void TEST()
{
  do
  {
    if (digitalRead(S1)==LOW)   
    {               
      digitalWrite(A1, HIGH);
      delay(200);
      digitalWrite(A2, HIGH);
      delay(200);
      digitalWrite(A3, HIGH);
      delay(200);
      digitalWrite(A4, HIGH);
      delay(200);
    }

    if (digitalRead(S2)==LOW)   
    {               
      digitalWrite(A1, LOW);
      delay(200);
      digitalWrite(A2, LOW);
      delay(200);
      digitalWrite(A3, LOW);
      delay(200);
      digitalWrite(A4, LOW);
      delay(200);
    }

    if (digitalRead(E1)==LOW)   
    {               
      delay(400);         
      analogWrite(PWM1, 50);
      delay(400);
      analogWrite(PWM1, 100);
    }

    if (digitalRead(E2)==LOW)   
    {             
      delay(400);     
      analogWrite(PWM1, 150);
      delay(400);
      analogWrite(PWM1, 200);
      delay(400);
      analogWrite(PWM1, 250);
    } 
  
    if (digitalRead(E3)==LOW)   
    {             
      analogWrite(PWM1, analogRead(AE1)/4);
    }
  }
  while (!0);
}
