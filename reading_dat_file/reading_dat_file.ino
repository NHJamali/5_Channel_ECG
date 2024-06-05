// Reading .dat and .hea files from SD card with Arduino Due
//04 June 2024

#include <SPI.h>
#include <SD.h>


File fxdata;


void setup() {
  // put your setup code here, to run once:
  pinMode(10, OUTPUT); // CS/SS pin as output for SD library to work.
  digitalWrite(10, HIGH); // workaround for sdcard failed error...

  if (!SD.begin(10))
  {
    Serial.println("sdcard initialization failed!");
    return;
  }
  Serial.println("sdcard initialization done.");
  
  // test file open
  fxdata = SD.open("01000_lr.dat");  // read only
  if (fxdata)
  {
    Serial.println("file open ok");      
    fxdata.close();
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  fxdata.readBytes();
  delay(20);
  fxdata.close();
}
