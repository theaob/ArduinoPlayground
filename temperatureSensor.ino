#include <LiquidCrystal.h>
#include <SPI.h>
#include <SD.h>
#include <OneWire.h>


OneWire ds(2);
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

Sd2Card card;

File logFile;

const int chipSelect = 4;

void setup() {
  Serial.begin(9600);

  SD.begin(4);
  
  logFile = SD.open("datalog.txt", FILE_WRITE);

  lcd.begin(16, 2);              // start the library
  lcd.setCursor(0,0);


  if (SD.exists("datalog.txt")) {
    lcd.print("FILE OK");
  } else {
    lcd.print("FILE NOT FOUND");
  }

  logFile.close();

  delay(10000);
  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
  float temp = getTemp(ds);
  String tempStr = String(temp);
  String dataString = "";
  dataString += tempStr;
  dataString += ",";

  lcd.setCursor(0,0);
  lcd.print(tempStr);
  lcd.print(" C");
  lcd.setCursor(0,1);
  if(temp > 25)
  {
    lcd.print("ORTAM SOGUT");
  }
  else
  {
    lcd.print("ORTAMI ISIT");
  }
  
  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  
  logFile = SD.open("datalog.txt", FILE_WRITE);
  
  // if the file is available, write to it:
  if (logFile) {
    logFile.print(dataString);
    logFile.close();
    // print to the serial port too:
  }
  // if the file isn't open, pop up an error:
  else {
    logFile.close();
    Serial.println("error opening datalog.txt");
  }

  Serial.println(dataString);

  delay(100);
}


float getTemp(OneWire sensor){
byte data[12];
byte addr[8];

if ( !sensor.search(addr)) {
   //no more sensors on chain, reset search
   sensor.reset_search();
   return -1000;
}

if ( OneWire::crc8( addr, 7) != addr[7]) {
   Serial.println("CRC is not valid!");
   return -1000;
}

if ( addr[0] != 0x10 && addr[0] != 0x28) {
   Serial.print("Device is not recognized");
   return -1000;
}

sensor.reset();
sensor.select(addr);
sensor.write(0x44,1); // start conversion, with parasite power on at the end

byte present = sensor.reset();
sensor.select(addr);  
sensor.write(0xBE); // Read Scratchpad

for (int i = 0; i < 9; i++) { // we need 9 bytes
  data[i] = sensor.read();
}

sensor.reset_search();

byte MSB = data[1];
byte LSB = data[0];

float tempRead = ((MSB << 8) | LSB); //using two's compliment
float TemperatureSum = tempRead / 16;

return TemperatureSum;
}
