#include <SPI.h>
#include <SD.h>

const int chipSelect = 4; // Change to your SD card CS pin
const int dacPin0 = DAC0; // DAC output pin
const int signalSamplingRate = 1000; // Sampling rate in Hz
const int signalDelay = 10000; // signalSamplingRate; // Microseconds delay
const int numberOfSignals = 12; // Total number of signals as per .hea file
const int signalIndexLeadII = 1; // Assume Lead II is the 2nd signal (index 1 in 0-based indexing)
const int maxSamples = 10000; // Adjust this based on your memory constraints

int16_t leadIISignal[maxSamples];
int totalSamples = 0;

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

  // Read the Lead II signal data from the file and store in an array
  while (dataFile.available() && totalSamples < maxSamples) {
    for (int i = 0; i < numberOfSignals; i++) {
      int16_t data = read16bitSigned(dataFile);

      // Store Lead II signal
      if (i == signalIndexLeadII) {
        leadIISignal[totalSamples] = data;
        totalSamples++;
        if (totalSamples >= maxSamples) break;
      }
    }
  }

  dataFile.close();
  Serial.print("Total samples stored: ");
  Serial.println(totalSamples);
}

void loop() {
  // Produce the signal continuously from the stored data
  for (int i = 0; i < totalSamples; i++) {
    int scaledDataLeadII = map(leadIISignal[i], -32768, 32767, 0, 4095);
    analogWrite(dacPin0, scaledDataLeadII);
    delayMicroseconds(signalDelay);
  }
}