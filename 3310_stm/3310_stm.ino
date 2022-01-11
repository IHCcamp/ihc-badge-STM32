/*
    1337 3310 WiFi - a badge in the form factor of the best phone ever
    Copyright (C) 2018 Mastro Gippo
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/ 

#include <Arduino.h>
#define BOARDV3 1
//#define TEST 1
//#define SER_TEST 1

#include <NonBlockingRtttl.h>
#define kp_Pin PA0
#define kx1_Pin PA1
#define kx2_Pin PA2
#define kx3_Pin PA3
#define kx4_Pin PA4
#define ky1_Pin PA5
#define ky2_Pin PA6
#define ky3_Pin PA7
#define ky4_Pin PA8
#define vBat_Pin PB1
#define ESP_TX_Pin PB10
#define ESP_RX_Pin PB11
#ifdef BOARDV3
  #define mot_Pin PB6
  #define buz_Pin PB7
  #define usbp_Pin PB3 //only in HW v0.2+
  #define chg_Pin PB12
#else
  #define mot_Pin PB13 //changes in HW v0.2+
  #define buz_Pin PB14 //changes in HW v0.2+
#endif
#define esp_b0_Pin PB4
#define esp_rst_Pin PB5
#define led_Pin PB9


#ifdef SER_TEST
  #define SerialESP Serial
#else
  #define SerialESP Serial3
#endif
#define SerialPC Serial

#define ESP_OFF 1

#define ESP_OFF 1
#define ESP_ON 5
#define ESP_ON_BOOT 10

#define CMD_SN 'S' //S0
#define CMD_BATT 'B' //B0
#define CMD_RING 'R' //Rn numero suoneria - max 10, RS ferma
#define CMD_NOTI 'N' //Nn numero notifica - max 10
#define CMD_TONE 'T' //Tnm n = durata in 4ms increments, byte raw (non testo) m = frequenza (m*40)
#define CMD_VIB 'V' //Vn n = durata in 10ms increments, byte raw (non testo)
#define CMD_LED 'L' //L0 off, L1 on, LKn on con timer n*1sec che riparte se si preme un tasto, LDn dove n > 2 con timer *100ms. LK0 disabilita funzione tastiera

#define NUM_RINGTONE 10
const char * tetris = "tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,2a,8p,d6,8f6,a6,8g6,8f6,e6,8e6,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,a";
const char * mario = "mario:d=4,o=5,b=100:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,16p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16c7,16p,16c7,16c7,p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16d#6,8p,16d6,8p,16c6";
const char * nokia = "Nokia tune:d=16,o=4,b=64,32f5,32d#5,g,a,32d5,32c5,d#,f,32c5,32a#,d,f,8a#.";
const char * kick = "Kick:d=32,o=6,b=193,l=15:8f4,p,f,16p,f,16p,8d#4,p,f,8p,8c#4,p,f,16p,f,16p,8c4,8p.,f,16p,f,16p,8f4,p,8d#4,p,f,8p,8c#4,p,f,16p,f,16p,4c4,2p,1p.,8p.,8f4,p,f,16p,f,16p,8d#4,p,f,8p,8c#4,p,f,16p,f,16p,8c4,8p.,f,16p,f,16p,8f4,p,8d#4,p,f,8p,8c#4,p,f,16p,f,16p,4c4,2p,1p.";
const char * pg = "Playground:d=32,o=4,b=64,l=15:a,p,a,p,f#,p,b,p,a,p,a,p,16f#,8p,e6,p,e6,p,c#6,p,f#6,p,e6,p,e6,p,16c#6";
const char * ti = "Thats it:d=32,o=5,b=106,l=15:b,16p.,f#,p,f#,p,g#,16p.,f#,8p.,a#,16p.,16b,p,4p,e6,16p.,b,p,b,p,c#6,16p.,b,8p.,d#6,16p.,16e6";
const char * fr = "Frog:d=32,o=3,b=333,l=15:a#,p,a#,p,a#,p,a#,p,a#,p,a#,p,2p,a#,p,a#,p,a#,p,a#,p,a#,p,a#,p,2p,a#,p,a#,p,a#,p,a#,p,a#,p,a#";
const char * ju = "Jumping:d=32,o=4,b=60,l=15:c#,16p.,c#5,16p.,c5,g#,p,a#,g#,p,f,p,c#,g#,p,c#,f#,p,c#,p,f,c#,d#,p,c#";
//long... const char * hg = "Hurdy-gurdy:d=32,o=4,b=137,l=15:f,c#,16p,a#,f#,16p,f,c#,8p.,f,c#,16p,a#,f#,16p,f,c#,8p.,f,c#,16p,a#,f#,16p,f,c#,p,e6,8f6,f,c#,16p,a#,f#,16c#6,f,c#,16c6,8a#5,f,c#,16p,a#,f#,16p,f,c#,16c6,8c#6,f,c#,16f6,a#,f#,16g#6,f,c#,16g6,8f6,f,c#,16p,a#,f#,16p,f,c#,p,g#6,8a#6,f,c#,16p,a#5,f#5,16g#6,f,c#,f6,e6,8d#6,f,c#,16p,a#,f#,16p,f,c#,16e6,8f6,f,c#,16a#5,a#,f#,16c#6,f,c#,16c6,8a#5,f,c#,16p,a#,f#,16p,f,c,16p,f,c#,16p,a#,f#,16p,f,c#,8p.,f,c#,16p,a#,f#,16p,f,c#,8p.,f,c#,16p,a#,f#,16p,f,c#,p,e6,8f6,f,c#,16p,a#,f#,16c#6,f,c#,16c6,8a#5,f,c#,16p,a#,f#,16p,f,c#,16c6,8c#6,f,c#,16f6,a#,f#,16g#6,f,c#,16g6,8f6,f,c#,16p,a#,f#,16p,f,c#,p,g#6,8a#6,f,c#,16p,a#5,f#5,16g#6,f,c#,f6,e6,8d#6,f,c#,16p,a#,f#,16p,f,c#,16e6,8f6,f,c#,16a#5,a#,f#,16c#6,f,c#,16c6,8a#5,f,c#,16p,a#,f#,16p,f,c#";
//const char * tb = "Hurdy-gurdy:d=32,o=4,b=137,l=15:16a#3,16p,f,c#,16p,a#,f#,16p,f,c#,8p.,f,c#,16p,a#,f#,16p,f,c#,8p.,f,c#,16p,a#,f#,16p,f,c#,p,e6,8f6,f,c#,16p,a#,f#,16c#6,f,c#,16c6,8a#5,f,c#,16p,a#,f#,16p,f,c#,16c6,8c#6,f,c#,16f6,a#,f#,16g#6,f,c#,16g6,8f6,f,c#,16p,a#,f#,16p,f,c#,p,g#6,8a#6,f,c#,16p,a#5,f#5,16g#6,f,c#,f6,e6,8d#6,f,c#,16p,a#,f#,16p,f,c#,16e6,8f6,f,c#,16a#5,a#,f#,16c#6,f,c#,16c6,8a#5,f,c#,16p,a#,f#,16p,f,c#,8a#3.";
const char * hg = "Hurdy-gurdy:d=32,o=4,b=137,l=15:16a#3,16p,f,c#,16p,a#,f#,16p,f,c#,8p.,f,c#,16p,a#,f#,16p,f,c#,8p.,f,c#,16p,a#,f#,16p,f,c#,p,e6,8f6,f,c#,16p,a#,f#,16c#6,f,c#,16c6,8a#5,f,c#,16p,a#,f#,16p,f,c#,16c6,8c#6,f,c#,16f6,a#,f#,16g#6,f,c#,16g6,8f6,f,c#,16p,a#,f#,16p,f,c#,p,g#6,8a#6,f,c#,16p,a#5,f#5,16g#6,f,c#,f6,e6,8d#6,f,c#,16p,a#,f#,16p,f,c#,16e6,8f6,f,c#,16a#5,a#,f#,16c#6,f,c#,16c6,8a#5,f,c#,16p,a#,f#,16p,f,c#,8a#3.";
//const char * hg = "Hurdy-gurdy:d=32,o=4,b=137,l=15:f,c#,16p,a#,f#,16p,f,c#,8p.,f,c#,16p,a#,f#,16p,f,c#,8p.,f,c#,16p,a#,f#,16p,f,c#,p,e6,8f6,f,c#,16p,a#,f#,16c#6,f,c#,16c6,8a#5,f,c#,16p,a#,f#,16p,f,c#,16c6,8c#6,f,c#,16f6,a#,f#,16g#6,f,c#,16g6,8f6,f,c#,16p,a#,f#,16p,f,c#,p,g#6,8a#6,f,c#,16p,a#5,f#5,16g#6,f,c#,f6,e6,8d#6,f,c#,16p,a#,f#,16p,f,c#,16e6,8f6,f,c#,16a#5,a#,f#,16c#6,f,c#,16c6,8a#5,f,c#,16p,a#,f#,16p,f,c#";
const char * rr = "RickRoll:d=4,o=5,b=200:8g,8a,8c6,8a,e6,8p,e6,8p,d6.,p,8p,8g,8a,8c6,8a,d6,8p,d6,8p,c6,8b,a.,8g,8a,8c6,8a,2c6,d6,b,a,g.,8p,g,2d6,2c6.,p,8g,8a,8c6,8a,e6,8p,e6,8p,d6.,p,8p,8g,8a,8c6,8a,2g6,b,c6.,8b,a,8g,8a,8c6,8a,2c6,d6,b,a,g.,8p,g,2d6,2c6.";
const char * ringtones[NUM_RINGTONE] = {tetris,mario,nokia,kick,pg,ti,fr,ju,hg,rr};

#define NUM_NOTIF 4
const char * upr = "LB:d=8,o=0,b=192,l=15:32c#,d";
const char * robot = "R:d=32,o=5,b=192,l=15:c#,b,d#,a,f,g,g,f,a,d#,b,c#,c#6,b4,d#6";
const char * win = "win:d=8,o=4,b=320,l=15:p.,d#,32p,d#,32p,2a#,32p,d#,32p,2a#";
const char * SMSr = "SMS:d=16,o=0,b=96,l=15:g#,32p,g#,32p,4p,g#,32p,g#";
const char * notification[NUM_RINGTONE] = {upr,robot,win,SMSr};

#define BlinkPeriod 300
#define REPORT_INTERVAL 10000


//KEYPAD STUFF
static volatile int Status = ESP_OFF;
static volatile char keyp = 0;

volatile bool EnableKeyLight = true; //Abilita l'accensione dei LED alla pressione dei tasti

#define boot_key KeyStatus[12] == key_pressed // *
#define C_key KeyStatus[3] == key_pressed // C

#define debounce_ms 30

#define key_idle 0
#define key_pressed_debounce 5
#define key_pressed 10
#define key_released_debounce 15
#define key_released 20

char KeyMap[16] = {'1', '2', '3', 'C', '4', '5', '6', 'M', '7', '8', '9', 'D', '*', '0', '#', 'U'};
unsigned long KeyDebounce[16];
unsigned char KeyStatus[16];

byte rowPins[4] = {ky1_Pin, ky2_Pin, ky3_Pin, ky4_Pin}; //connect to the row pinouts of the keypad
byte colPins[4] = {kx1_Pin, kx2_Pin, kx3_Pin, kx4_Pin}; //connect to the column pinouts of the keypad


//end KEYPAD STUFF


unsigned long TimeVIB = 0;
unsigned long TimeLED = 0;
unsigned long TimeLED_K = 0;
unsigned long TimeLEDrst = 0;
unsigned long TimeLED_Krst = 15000; //default led accesi 10 sec
unsigned long TimeReport = 0;

unsigned long TimeLEDBlink = 0;

void setup() {
   
  pinMode(kp_Pin, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(kp_Pin), kp_int, FALLING);

  //http://docs.leaflabs.com/static.leaflabs.com/pub/leaflabs/maple-docs/0.0.12/lang/api/analogwrite.html
  pinMode(mot_Pin, OUTPUT);
  digitalWrite(mot_Pin, LOW);

  pinMode(buz_Pin, OUTPUT);
  digitalWrite(buz_Pin, LOW);

  pinMode(esp_b0_Pin, OUTPUT);
  digitalWrite(esp_b0_Pin, LOW);

  pinMode(esp_rst_Pin, OUTPUT);
  digitalWrite(esp_rst_Pin, LOW);
  
  pinMode(led_Pin, OUTPUT);
  digitalWrite(led_Pin, LOW);
  
  #ifdef BOARDV3
    pinMode(usbp_Pin, INPUT_PULLUP);
    pinMode(chg_Pin, INPUT_PULLUP);
  #endif
  
  SerialPC.begin(115200);
  SerialESP.begin(115200); //ESP32

#ifdef TEST
  delay(100);
  SerialPC.flush();
  while (!SerialPC.available());
  SerialPC.println("STM32 Booting");
  GetSN();
  if (!digitalRead(kp_Pin))
    SerialPC.println("Error: Power key pressed");

  //Test keyboard
  for (int i = 0; i < 4; i++)
  {
    pinMode(colPins[i], OUTPUT);
    digitalWrite(colPins[i], LOW);
  }

  for (int i = 0; i < 4; i++)
  {
    pinMode(rowPins[i], INPUT_PULLUP);

    if (!digitalRead(rowPins[i]))
    {
      SerialPC.print("Error: LOW on row ");
      SerialPC.println(i);
    }
  }

  digitalWrite(mot_Pin, HIGH);
  delay(200);
  digitalWrite(mot_Pin, LOW);

  tone(buz_Pin, 500, 8);

  //Accendo ESP Bootloader
  SerialPC.println("ESP32 boot");
  digitalWrite(esp_rst_Pin, LOW);
  digitalWrite(esp_b0_Pin, LOW);
  digitalWrite(led_Pin, HIGH);
  delay(200);
  digitalWrite(esp_rst_Pin, HIGH);
  digitalWrite(led_Pin, LOW);
#endif

  Keypad_init();
  //keypad.addEventListener(keypadEvent);
  TimeReport = millis() + REPORT_INTERVAL;
}

void DoCommPC() {
#ifndef SER_TEST
  
  static byte state = 0;
  
//  if (SerialPC.available()) SerialESP.write(SerialPC.read());
  while (SerialPC.available()) 
  {
    byte in = SerialPC.read();
    SerialESP.write(in);
    if (Status == ESP_OFF) //ESP is OFF, can go into bootloader
    switch (state)
    {
      case 0:
        if (in == '&')
          state = 2;
        break;
      case 2:
        if (in == 'B')
        {
          ESP32_Bootloader();
        }
        state = 0;
        break;
    }
  }
#endif
}

void DoCommESP() {
  static byte state = 0;
  static byte cmd = 0;
  static byte toneF = 0;

  while (SerialESP.available()) 
  {
    byte in = SerialESP.read();
    SerialPC.write(in);
    
    #ifndef SER_TEST
      if (Status == ESP_ON) //ESP is ON, normal boot. Avoid bootloader stuff
    #endif
    {
      switch (state)
      {
        case 0:
          if (in == '&')
            state = 2;
          break;
        case 2:
          if (in == '&')
            state = 5;
          break;
        case 5:
          cmd = in;
          state = 10;
          break;
        case 10:
          switch (cmd)
          {
            case CMD_SN: //&&S0
              if (in == '0')
              {
                SerialPC.println("SN req");
                uint8 *idBase0 =  (uint8 *) (0x1FFFF7E8);
                SerialESP.print("$SN,");
                for (int i = 0; i < 12; i++)
                {
                  if (*(idBase0 + i) < 0x10) SerialESP.print("0");
                  SerialESP.print(*(idBase0 + i), HEX);
                }
                SerialESP.print("++");
              }
              cmd = 0;
              break;

            case CMD_BATT:
              if (in == '0')
              {
                //AGGIUNGERE SE STA CARICANDO + NOTIFICA DI BATT SCARICA?
                int raw = analogRead(vBat_Pin);
                SerialPC.print("BATT req: ");
                SerialPC.print(raw * 1.61);
                SerialPC.println(" mV");

                SerialESP.print("$BV,");
                SerialESP.print(raw);
                SerialESP.print("++");
              }
              cmd = 0;
              break;
              
            case CMD_RING:
              if(in == 'S')
                rtttl::stop();
              else
              {
                in = in - '0'; //I get an ASCII
                if(in >= NUM_RINGTONE) in = 3; //default Nokia
                rtttl::begin(buz_Pin, ringtones[in]);
              }
              cmd = 0;
              break;

            case CMD_NOTI:
              in = in - '0'; //I get an ASCII
              if(in < NUM_NOTIF)
                rtttl::begin(buz_Pin, notification[in]);
              cmd = 0;
              break;
              
            case CMD_TONE:
              toneF = in; 
              state = 25;
              return;
              
            case CMD_VIB: //££Vn
              if(in == 0) 
                digitalWrite(mot_Pin, LOW);
              else
              {
                digitalWrite(mot_Pin, HIGH);
                TimeVIB = millis() + ((unsigned long)(in)*10);
              }
              cmd = 0;
              break;
              
            case CMD_LED:
              switch(in)
              {
                case '0': digitalWrite(led_Pin, LOW); cmd = 0; break;
                case '1': digitalWrite(led_Pin, HIGH); cmd = 0; break;
                case 'K': cmd = in; state = 20; return;
                case 'D': cmd = in; state = 20; return;
              }
              break;
          }
          state = 0;
          break;

        case 20:
          if(cmd == 'K') 
          {
            if(in == 0) 
            {
              EnableKeyLight = false;
            }
            else
            {
              digitalWrite(led_Pin, HIGH);
              TimeLED_Krst = ((unsigned long)(in)*1000);
              TimeLED_K = millis() + TimeLED_Krst;
              EnableKeyLight = true;
            }              
          }
          if(cmd == 'D') TimeLED = millis() + ((unsigned long)(in)*100);
          {
            digitalWrite(led_Pin, HIGH);
            TimeLEDrst = ((unsigned long)(in)*1000);
            TimeLED = millis() + TimeLEDrst;
          }
          cmd = 0;
          state = 0; 
          break;

        case 25:
          tone(buz_Pin, toneF*40, in*4);
          cmd = 0;
          state = 0; 
          break;         
      }
    }
  }
}

void DoTimers()
{
  if(TimeVIB != 0) 
    if(millis() >= TimeVIB) 
    {
      digitalWrite(mot_Pin, LOW);
      TimeVIB = 0;
    }
    
  if(TimeLED != 0) 
    if(millis() >= TimeLED) 
    {
      digitalWrite(led_Pin, LOW);
      TimeLED = 0;
    }
    
  if(TimeLED_K != 0) 
    if(millis() >= TimeLED_K)
    {
      digitalWrite(led_Pin, LOW);
      TimeLED_K = 0;
    }

  #ifndef SER_TEST
    if (Status == ESP_ON) //ESP is ON, normal boot. Avoid bootloader stuff
  #endif
  if(millis() >= TimeReport) 
  {
    //$R,2554,1,0++ vbat,usb_present,charging
    SerialESP.print("$R,");
    SerialESP.print(analogRead(vBat_Pin));

    #ifdef BOARDV3
      if(digitalRead(usbp_Pin))
        SerialESP.print(",1,");
      else
        SerialESP.print(",0,");
      if(!digitalRead(chg_Pin))
        SerialESP.print("1");
      else
        SerialESP.print("0");    
    #else
      SerialESP.print(",0,0"); 
    #endif
    SerialESP.print("++");
    TimeReport = millis() + REPORT_INTERVAL;
  }

  if(TimeLEDBlink != 0)
    if(millis() >= TimeLEDBlink)
    {
      digitalWrite(led_Pin, !digitalRead(led_Pin));
      TimeLEDBlink = millis() + BlinkPeriod;
    }
}

void loop() {
  while (1)
  {
    DoCommPC();
    DoCommESP();

    DoKeypad();

    PowerCheck();
    DoTimers();
    
    rtttl::play();
  }
}

void ESP32_Bootloader()
{
  tone(buz_Pin, 500, 8);
  digitalWrite(esp_rst_Pin, LOW);
  digitalWrite(esp_b0_Pin, LOW);
  delay(200);
  digitalWrite(esp_rst_Pin, HIGH);
  TimeLEDBlink = millis() + BlinkPeriod;
  Status = ESP_ON_BOOT;
}

void PowerCheck() {
  static unsigned long keyTime = 0;
  static byte stat = 0;
  switch (stat) {
    case 0:
      if (!digitalRead(kp_Pin))
      {
        stat = 5;
        keyTime = millis();
      }
      break;
    case 5:
      if ((keyTime + 1000) <= millis())
      {
        if (digitalRead(kp_Pin))
        {
          stat = 0;
          break;
        }
        if (Status == ESP_OFF)
        {
          SerialPC.print("Power ON [");
          SerialPC.print(boot_key);
          //Accendo ESP
          if (boot_key)
          { //Bootloader se accendo con * premuto!
            SerialPC.println("] boot");
            ESP32_Bootloader();
          }
          else
          {
            tone(buz_Pin, 1000, 8);
            SerialPC.println("] normal");
            digitalWrite(esp_rst_Pin, LOW);
            digitalWrite(esp_b0_Pin, HIGH);
            digitalWrite(led_Pin, HIGH);
            TimeLED_K = millis() + TimeLED_Krst;
            delay(100);
            digitalWrite(esp_rst_Pin, HIGH);
            Status = ESP_ON;
          }
        }
        else
        {
          SerialESP.print("$P0++");
          delay(200);
          digitalWrite(esp_rst_Pin, LOW);
          digitalWrite(led_Pin, LOW);
          Status = ESP_OFF;
          tone(buz_Pin, 100, 50);
          TimeLEDBlink = 0;
        }
        stat = 10;
      }

      break;

    default:
      stat = 0;
  }
}

void GetSN() {
  uint16 *flashSize = (uint16 *) (0x1FFFF7E0);
  uint8 *idBase0 =  (uint8 *) (0x1FFFF7E8);

  SerialPC.print("Flash size is ");
  SerialPC.print(*flashSize );
  SerialPC.println("k");

  SerialPC.print("Unique ID is ");
  for (int i = 0; i < 12; i++)
  {
    if (*(idBase0 + i) < 0x10) SerialPC.print("0"); //Arduino merda
    SerialPC.print(*(idBase0 + i), HEX);
    SerialPC.print(" ");
  }

  SerialPC.println();
}


//all keypad implementatins are CRAP. let's make a better one.
//constraint: it's very improbable that people will press more than 2 buttons at the same time. I'm not afraid of ghosts

void Keypad_init()
{
  for(int i = 0; i < 4; i++)
  {
    pinMode(rowPins[i], INPUT_PULLUP);
    pinMode(colPins[i], INPUT);
  }
  for(int i = 0; i < 16; i++)
  {
    KeyStatus[i] = key_idle;
    KeyDebounce[i] = 0;
  }
}

void DoKeypad()
{
  int ki = 0xFF; //key index
  for(int i = 0; i < 4; i++)
  {
    pinMode(colPins[i], OUTPUT);
    digitalWrite(colPins[i], LOW);
    for(int j = 0; j < 4; j++)
    {//scan rows
      ki = (i*4)+j;
      
      switch(KeyStatus[ki])
      {
        case key_idle:
          if(!digitalRead(rowPins[j]))
          {
            KeyDebounce[ki] = millis() + debounce_ms;
            KeyStatus[ki] = key_pressed_debounce;
          }
        break;

        case key_pressed_debounce:
          if(millis() > KeyDebounce[ki])
          {
            KeyStatus[ki] = key_pressed;

            SerialESP.print("$D");
            SerialESP.print(KeyMap[ki]);
            SerialESP.print("++");
            
            //manage backlight
            if ((Status == ESP_ON) && EnableKeyLight) 
            {
              digitalWrite(led_Pin, HIGH);
              TimeLED_K = millis() + TimeLED_Krst;
            }

            //Dirty fix: if RTTTL is playing, stop
            if ( rtttl::isPlaying() && (C_key) )
              rtttl::stop();
          }
        break;

        case key_pressed:
          if(digitalRead(rowPins[j]))
          {
            KeyDebounce[ki] = millis() + debounce_ms;
            KeyStatus[ki] = key_released_debounce;
          }
        break;
        
        case key_released_debounce:
          if(millis() > KeyDebounce[ki])
          {
            KeyStatus[ki] = key_idle;

            SerialESP.print("$U");
            SerialESP.print(KeyMap[ki]);
            SerialESP.print("++");
          }
        break;
      }
    }
    pinMode(colPins[i], INPUT);
  }
  /*
  if (key) {
      
      SerialPC.print("K");
      SerialPC.println(key);
      
      if ( rtttl::isPlaying() && (key == 'C') )
        rtttl::stop();
        
      if(key == '*') KKK=true;
      else KKK=false;
      
      if ((Status == ESP_ON) && EnableKeyLight) 
      {
        digitalWrite(led_Pin, HIGH);
        TimeLED_K = millis() + TimeLED_Krst;
      }
    }*/
}
