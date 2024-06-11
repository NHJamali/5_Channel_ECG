#include <SPI.h>
#include <SD.h>

const int chipSelect = 4; // Change to your SD card CS pin
const int dacPin0 = DAC0; // DAC output pin
const int signalSamplingRate = 1000; // Sampling rate in Hz
const int signalDelay = 10 / signalSamplingRate; // Microseconds delay
const int numberOfSignals = 12; // Total number of signals as per .hea file
const int signalIndexLeadII = 0; // Assume Lead II is the 2nd signal (index 1 in 0-based indexing)

File dataFile;

// Function to read a 16-bit signed integer from the binary file
int16_t read16bitSigned(File &file) {
  int16_t value;
  file.read((char *)&value, 2);
  return value;
}

void setup() {
  // Initialize Serial (optional, can be removed if not needed)
  Serial.begin(115200);
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
  // Open the .dat file
  File dataFile = SD.open("01000_lr.dat");
  if (!dataFile) {
    Serial.println("Error opening 01000_lr.dat");
    return;
  }
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
    // Skip to the desired signal
    for (int i = 0; i < numberOfSignals; i++) {
      int16_t data = read16bitSigned(dataFile);

      // Process Lead II signal
      if (i == signalIndexLeadII) {
        int scaledDataLeadII = map(data, -32768, 32767, 0, 4095);
        analogWrite(dacPin0, scaledDataLeadII);

        // Delay to match the sampling rate
        delayMicroseconds(signalDelay);
      }
    }
  }

  // Close the file and reopen it to loop continuously
  dataFile.close();
}