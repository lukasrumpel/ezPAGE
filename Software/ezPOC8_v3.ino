//Firmware for Si446x modules exclusively!
#include <SPI.h>
#include <RH_RF24.h>
#include <POCSAG.h>
#include <Wire.h> 

#define TX_LED 4
#define WDP 3

RH_RF24 rf24;

Pocsag pocsag;

double freq;
int power;
/*
 * Power Levels Speicherblock 0x07:
 * 1 = 1dBm
 * 2 = 2dBm
 * 3 = 5dBm
 * 4 = 8dBm
 * 5 = 11dBm
 * 6 = 14dBm
 * 7 = 17dBm
 * 8 = 20dBm
 */

//-----------------------------------------------------------------------------------------------------------------
void init_tx(void);
void Hauptschleife(void);
double freg_eeprom(int eepromadresse); // Adresse für das gewählte EEPROM 0x50 also a bis a2 mit GND verbunden speicheradresse[0x00 -> hunderter] [0x01 -> zehner] [0x02 -> einer] [0x03 -> komma1] [0x04 -> komma2] [0x05 -> komma3] [0x06 -> komma4] [0x07 -> Power Level]
int eeprom_read(int chipadresse, int speicheradresse);
void eeprom_write(int chipadresse, int speicheradresse, byte data);
int get_power(int eepromadresse, int speicherblock);
void write_freq(int chipadresse);
//-----------------------------------------------------------------------------------------------------------------
void setup() {
  init_tx();
  pinMode(TX_LED, OUTPUT); //SendeLED;
  pinMode(WDP, OUTPUT); //Statuspin für Watchdog
  digitalWrite(WDP, LOW); // active LOW, da Watchdog bei HIGHPegel scharf ist
  digitalWrite(TX_LED, LOW);

}

void loop() {
  Hauptschleife();

}

//-----------------------------------------------------------------------------------------------------------------

void init_tx(void){
  Serial.begin(9600);
  if(rf24.init()){
    Serial.println("\nezPOC8 init OK");
    }
  else{
    Serial.println("\nezPOC8 init FAILED");
    }


  freq = freg_eeprom(0x50);
  power = get_power(0x50, 0x07);
  Serial.println("Freq:");
  Serial.println(freq, DEC);
  rf24.setFrequency(freq, 0.00);
  

  Serial.println("Power[dBm]:");
  switch(power){
    case 1:
    rf24.setTxPower(0x1E);
    Serial.println("1");
    break;
    case 2:
    rf24.setTxPower(0x21);
    Serial.println("2");
    break;
    case 3:
    rf24.setTxPower(0x28);
    Serial.println("5");
    break;
    case 4:
    rf24.setTxPower(0x32);
    Serial.println("8");
    break;
    case 5:
    rf24.setTxPower(0x43);
    Serial.println("11");
    break;
    case 6:
    rf24.setTxPower(0x64);
    Serial.println("14");
    break;
    case 7:
    rf24.setTxPower(0x73);
    Serial.println("17");
    break;
    case 8:
    rf24.setTxPower(0x7F);
    Serial.println("20");
    break;
    default:
    rf24.setTxPower(0x32);
    Serial.println("8dBm");
    break;
    }
  
}

  void Hauptschleife(void){
  int anz = 76;
  int mod = 0;
  int rc;
  int state = 0;
  long int adress = 0;
  int adresssource = 0;
  int repeat = 0;
  char textmsg[(anz+2)];
  int msgsize = 0;

  Serial.println("Format: ");
  Serial.println("P <adress> <source> <repeat> <Message>"); // Leerzeichen müssen vorhanden sein Source muss zwischen 0 und 3 sein, Wiederholungen zwischen 1 und 9, Die Nachricht darf max. 'anz' lang sein
 //möglicher TestString P 123456 0 0 TEST
  Serial.println("F(7 digits for Freq)p (p = TX POWER) [F43998754]");
  while( state >= 0){
    char c;
      while(!Serial.available()){
        }
      c = Serial.read();
      digitalWrite(TX_LED, HIGH);
      digitalWrite(WDP, HIGH);
      if(c == 0x1b){ //c == ASCII ESC
        state = -999;
        break;
        }

        
      if (state == 0){
        if ((c == 'p') || (c == 'P')){
          state = 1;
          Serial.write(c);
          }
        if ((c == 'f') || (c == 'F')){
          write_freq(0x50);
          init_tx();
          digitalWrite(WDP, LOW);
          digitalWrite(TX_LED, LOW);
          break;
          }
        }

      if(state == 1){
        if(c == ' '){
          state = 2;
          Serial.write(c);
          continue;
          }
         else if((c >= '0') && c <= '9'){
          state = 2;
          }
        }

      if(state == 2){
        if((c >= '0') && (c <= '9')){
          long int newadress;
          newadress = adress *10 +(c - '0');

          if(newadress <= 0x1FFFFF){
            adress = newadress;
            Serial.write(c);
            }
          }
         else if(c == ' '){
          Serial.write(c);
          state = 3;
          }
          mod = adress % 10;
        switch(mod){
          case 0:
          anz = 76;
          break;
          case 1:
          anz = 76;
          break;
          case 2:
          anz = 76;
          break;
          case 3:
          anz = 76;
          break;
          case 4:
          anz = 76;
          break;
          case 5:
          anz = 68;
          break;
          case 6:
          anz = 60;
          break;
          case 7:
          anz = 58;
          break;
          case 8:
          anz = 50;
          break;
          case 9:
          anz = 48;
          break;
          default:
          anz = 76;
          break;
          }
        continue;
        }

     if(state == 3){
      if((c >= '0') && (c <= '3')){
        adresssource = c - '0';
        Serial.write(c);
        state = 4;
        }
      continue;
      }

    if(state == 4){
      if(c == ' '){
        Serial.write(c);
        state = 6;
        }
        continue;
      }

    if(state == 6){
      if ((c >= '0') && (c <= '9')){
        repeat = c - '0';
        state = 7;
        }
        continue;
      }

    if(state == 7){
      if(c == ' '){
        Serial.write(c);
        state = 8;
        }
        continue;
      }

    if(state == 8){
      if((c >= ' ') && (c <= '~')){
        if(msgsize < anz){
          Serial.write(c);
          textmsg[msgsize] = c;
          msgsize++;
          }
        }
       else if ((c >= 0x0a) && (c<= 0x0d)){
        Serial.println("");
        textmsg[msgsize] = 0x00;
        state = -1;
        break;
        }
      } 
      digitalWrite(WDP, LOW);
      digitalWrite(TX_LED, LOW);
    }
  

   if(state == -1){
     
    
    

    rc = pocsag.CreatePocsag(adress, adresssource, textmsg);

    if(!rc){
      Serial.print("Error in createpocsag! Error: ");
    Serial.println(pocsag.GetError());

    // sleep 10 seconds
    delay(10000);
      }
    else{
      for(int l= -1; l < repeat; l++){
        digitalWrite(TX_LED, HIGH);
        digitalWrite(WDP, HIGH);//Watchdog activation
        
        Serial.println("TRANSMITTING");
        rf24.send((uint8_t *)pocsag.GetMsgPointer(), pocsag.GetSize());
        rf24.waitPacketSent();
      
        delay(1000);
        digitalWrite(TX_LED, LOW);
        digitalWrite(WDP, LOW); //do not forget to deactivate the Watchdog
        }
      Serial.println("Transmission done");
      Serial.println("*~*");
      }
      
    } 
}

double freg_eeprom(int eepromadresse){
  float freq_puff;
  unsigned int hunderter, zehner, einer, komma1, komma2, komma3, komma4;
  int rdata;
  Wire.begin();
  hunderter = eeprom_read(eepromadresse, 0x00);
  zehner = eeprom_read(eepromadresse, 0x01);
  einer = eeprom_read(eepromadresse, 0x02);
  komma1 = eeprom_read(eepromadresse, 0x03);
  komma2 = eeprom_read(eepromadresse, 0x04);
  komma3 = eeprom_read(eepromadresse, 0x05);
  komma4 = eeprom_read(eepromadresse, 0x06);

  freq_puff = (100 * hunderter) + (10 * zehner) + einer + (0.1 * komma1) + (0.01 * komma2) + (0.001 * komma3) + (0.0001 * komma4);
  return freq_puff;
}

int get_power(int eepromadresse, int speicherblock){
  return eeprom_read(eepromadresse, 0x07);
} 

int eeprom_read(int chipadresse, int speicheradresse){
  byte rdata;
  Wire.beginTransmission(chipadresse);
  //Wire.write((int)(speicheradresse >> 8));
  //Wire.write((int)(speicheradresse & 0xFF));
  Wire.write((int)(speicheradresse));
  Wire.endTransmission();

  Wire.requestFrom(chipadresse, 1);
  if(Wire.available()){
    rdata = Wire.read();
    }
  return rdata;
}

void eeprom_write(int chipadresse, int speicheradresse, byte data){
  Wire.beginTransmission(chipadresse);
  //Wire.write((int)(speicheradresse >>8));
  //Wire.write((int)(speicheradresse & 0xFF));
  Wire.write((int)(speicheradresse));
  Wire.write(data);
  Wire.endTransmission();
  delay(20);
  
}

void write_freq(int chipadresse){
  char buffer;
  byte data;

  buffer = Serial.read();

  buffer = Serial.read(); //foo Read
  eeprom_write(chipadresse, 0x00, buffer-48);

  buffer = Serial.read(); //Hunderter MHZ
  eeprom_write(chipadresse, 0x00, buffer-48);
  
  buffer = Serial.read(); //Zehner MHZ
  eeprom_write(chipadresse, 0x01, buffer-48);
  
  
  buffer = Serial.read(); //Einer MHZ
  eeprom_write(chipadresse, 0x02, buffer-48);
  
  buffer = Serial.read(); //Hunderter KHZ
  eeprom_write(chipadresse, 0x03, buffer-48);
  
  buffer = Serial.read(); //Zehner KHZ
  eeprom_write(chipadresse, 0x04, buffer-48);
  
  buffer = Serial.read(); //Einer KHZ
  eeprom_write(chipadresse, 0x05, buffer-48);
  
  buffer = Serial.read(); //Hunderter HZ
  eeprom_write(chipadresse, 0x06, buffer-48);

  buffer = Serial.read(); //TX Power
  eeprom_write(chipadresse, 0x07, buffer-48);
  
  buffer = Serial.read();

}
