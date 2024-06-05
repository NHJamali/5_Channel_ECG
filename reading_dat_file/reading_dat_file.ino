#include <SPI.h>
#include <SD.h>

const int chipSelect = 4; // Change to your SD card CS pin
const int dacPin0 = DAC0; // First DAC output pin
const int dacPin1 = DAC1; // Second DAC output pin
const int adcPin0 = A0; // First ADC input pin
const int adcPin1 = A1; // Second ADC input pin
const int signalSamplingRate = 1000; // Sampling rate in Hz
const int signalDelay = 1000000 / signalSamplingRate; // Microseconds delay
const int numberOfSignals = 12; // Total number of signals as per .hea file
const int signalIndexV1 = 6; // V1 is the 7th signal (index 6 in 0-based indexing)
const int signalIndexLeadII = 1; // Assume Lead II is the 2nd signal (index 1 in 0-based indexing)

// Function to read a 16-bit signed integer from the binary file
int16_t read16bitSigned(File &file) {
  int16_t value;
  file.read((char *)&value, 2);
  return value;
}

void setup() {
  // Initialize Serial for plotting
  Serial.begin(500000);
  while (!Serial) {
    ; // Wait for serial port to connect. Needed for native USB port only
  }

  // Initialize SD card
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Initialization failed!");
    return;
  }
  Serial.println("Initialization done.");
}

void loop() {
  // Open the .dat file
  File dataFile = SD.open("01000_lr.dat");
  if (!dataFile) {
    Serial.println("Error opening 01000_lr.dat");
    return;
  }

  // Read and output data continuously
  while (dataFile.available()) {
    // Skip to the desired signals
    for (int i = 0; i < numberOfSignals; i++) {
      int16_t data = read16bitSigned(dataFile);

      // Process V1 signal
      if (i == signalIndexV1) {
        int scaledDataV1 = map(data, -32768, 32767, 0, 4095);
        analogWrite(dacPin0, scaledDataV1);
        int adcValueV1 = analogRead(adcPin0);
        Serial.print("V1: ");
        Serial.print(adcValueV1);
        Serial.print("\t");
      }

      // Process Lead II signal
      if (i == signalIndexLeadII) {
        int scaledDataLeadII = map(data, -32768, 32767, 0, 4095);
        analogWrite(dacPin1, scaledDataLeadII);
        int adcValueLeadII = analogRead(adcPin1);
        Serial.print("Lead II: ");
        Serial.println(adcValueLeadII);
      }
    }

    // Delay to match the sampling rate
    delayMicroseconds(signalDelay);
  }

  // Close the file and reopen it to loop continuously
  dataFile.close();
}