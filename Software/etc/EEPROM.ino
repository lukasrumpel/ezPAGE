//Sketch zur Programmierung des ezPOC8 EEPROMS
#include <Wire.h>


double freq;
byte puff;
int eeprom_adr = 80;
int power;

double freq_eeprom(int eepromadresse);
int eeprom_read(int chipadresse, int speicheradresse);
void eeprom_write(int chipadresse, int speicheradresse, byte data);
int get_power(int eepromadresse, int speicherblock);

void setup() {
  Serial.begin(9600);
  Serial.println("Bereit");
  Wire.begin();
  eeprom_write(eeprom_adr, 0x00, 0x04); //Hunderter der Frequenz
  eeprom_write(eeprom_adr, 0x01, 0x03); //Zehner der Frequenz
  eeprom_write(eeprom_adr, 0x02, 0x09); //Einer der Frequenz
  eeprom_write(eeprom_adr, 0x03, 0x09); //1. Nachkommastelle der Frequenz
  eeprom_write(eeprom_adr, 0x04, 0x08); //2. Nachkommastelle der Frequenz
  eeprom_write(eeprom_adr, 0x05, 0x07); //3. Nachkommastelle der Frequenz
  eeprom_write(eeprom_adr, 0x06, 0x05); //4. Nachkommastelle der Frequenz
  eeprom_write(eeprom_adr, 0x07, 0x04); //Power Level
  freq = freq_eeprom(80);
  power = get_power(80, 0x07);
  Serial.println(freq, 4);
  Serial.println(power);
}

void loop() {
  

}

double freq_eeprom(int eepromadresse){
  double freq_puff;
  unsigned int hunderter, zehner, einer, komma1, komma2, komma3, komma4;
  int rdata;
  //Wire.begin();
  hunderter = eeprom_read(eepromadresse, 0x00);
  Serial.println(hunderter);
  zehner = eeprom_read(eepromadresse, 0x01);
  Serial.println(zehner);
  einer = eeprom_read(eepromadresse, 0x02);
  Serial.println(einer);
  komma1 = eeprom_read(eepromadresse, 0x03);
  Serial.println(komma1);
  komma2 = eeprom_read(eepromadresse, 0x04);
  Serial.println(komma2);
  komma3 = eeprom_read(eepromadresse, 0x05);
  Serial.println(komma3);
  komma4 = eeprom_read(eepromadresse, 0x06);
  Serial.println(komma4);

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
