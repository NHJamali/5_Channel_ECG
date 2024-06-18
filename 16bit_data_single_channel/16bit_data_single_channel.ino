#include <SPI.h>
#include <SD.h>

// Constants from the .hea file
const int samplingFrequency = 100; // in Hz (updated to match the .hea file)
const float gain = 1000.0; // conversion factor to mV
const int baseline = 0; // baseline value from .hea file
const int initialZero = -661; // initial value for lead II from .hea file

// SD card chip select pin
const int chipSelect = 4;

// Maximum number of samples to store in memory (adjust based on file size and available RAM)
const int maxSamples = 1000;
int16_t ecgData[maxSamples];
int numSamples = 0;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Initialize the SD card
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  // Open the data file
  File dataFile = SD.open("01000_lr.dat");
  if (!dataFile) {
    Serial.println("Error opening 01000_lr.dat");
    while (1);
  }

  // Read the file into memory
  numSamples = readFileIntoMemory(dataFile);

  // Close the file
  dataFile.close();
}

int readFileIntoMemory(File& dataFile) {
  int index = 0;
  // Seek to the start of lead II data
  dataFile.seek(2 * maxSamples); // Each sample is 2 bytes, skip first 1000 samples (Lead I)
  
  while (dataFile.available() && index < maxSamples) {
    // Read two bytes at a time (16-bit sample)
    int16_t sample = dataFile.read() | (dataFile.read() << 8);
    ecgData[index++] = sample;
  }
  Serial.print("Read ");
  Serial.print(index);
  Serial.println(" samples into memory.");
  return index;
}

void outputECGSignal() {
  unsigned long interval = 10000; // in microseconds
  unsigned long previousMicros = 0;
  unsigned long currentMicros = 0;

  while (true) { // Continuous loop
    for (int i = 0; i < numSamples; i++) {
      // Get the current time in microseconds
      currentMicros = micros();

      // Wait until the correct sampling interval has passed
      if (currentMicros - previousMicros >= interval) {
        previousMicros = currentMicros;

        // Convert the raw value to millivolts
        int16_t rawValue = ecgData[i];
        float millivolts = (rawValue - initialZero) * (gain / 1000.0) + baseline;

        // Convert millivolts to a value suitable for the DAC
        int dacValue = map(millivolts, 0, 3300, 0, 4095); // Assuming -3.3V to 3.3V range for DAC

        // Output the value to DAC0
        analogWrite(DAC0, dacValue);
        
      }
      //delayMicroseconds(45);
    }
    analogWrite(DAC0, baseline);
    delay(50);
  }
}

void loop() {
  // Continuously output the ECG signal from memory
  outputECGSignal();
}
