#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_MCP4728.h>

Adafruit_MCP4728 mcp;

// SD card chip select pin
const int chipSelect = 4;

// Number of samples in the .dat file (as per the .hea file)
const int numSamples = 173;

// Number of leads in the .dat file (as per the .hea file)
const int numLeads = 5;

// Buffers to store raw and adjusted signal data
int16_t rawData[numLeads][numSamples];

// Gain and baseline arrays
float gain[numLeads];
int baseline[numLeads];

// Function to read the .dat file
bool readDatFile(const char *filename) {
  File datFile = SD.open(filename, FILE_READ);
  if (!datFile) {
    Serial.println("Error opening .dat file");
    return false;
  }

  // Read the raw data from the .dat file
  for (int i = 0; i < numSamples; i++) {
    for (int j = 0; j < numLeads; j++) {
      if (datFile.available()) {
        rawData[j][i] = datFile.read() | (datFile.read() << 8); // 16-bit data
      } else {
        Serial.println("Not enough data in .dat file");
        return false;
      }
    }
  }

  datFile.close();
  return true;
}

void setup() {
  Serial.begin(9600);

  // Initialize I2C
  Wire.begin();

  // Initialize DAC
  if (!mcp.begin()) {
    Serial.println("Failed to find MCP4728 chip");
    while (1) { delay(10); }
  }

  // Initialize SD card
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed");
    while (1) { delay(10); }
  }

  // Read the .dat file
  if (!readDatFile("electrode_signals.dat")) {
    Serial.println("Failed to read .dat file");
    while (1) { delay(10); }
  }

}

void loop() {
  for (int i = 1; i < 160; i++) {

    // Convert the adjusted data to a range suitable for DAC (0-4095 for 12-bit DAC)
    uint16_t valueA = (uint16_t)(map(rawData[0][i]-(-710),700,1845,0,4095));
    uint16_t valueB = (uint16_t)(map(rawData[1][i]-(-661),738,1191,0,4095));
    uint16_t valueC = (uint16_t)(map(rawData[3][i]-260,-956,-196,0,4095));
    uint16_t dacValue = (uint16_t)(map(rawData[2][i]-49,-1024,52,0,4095));
    uint16_t valueD = (uint16_t)(map(rawData[4][i]-(-229),-352,529,0,4095));

    //float millivolts = (rawValue - initialZero) * (gain / 100.0) + baseline;
    //int dacValue = map(millivolts, -3300, 3300, 0, 4095);

    // Set the DAC values
    mcp.setChannelValue(MCP4728_CHANNEL_A, valueA);
    mcp.setChannelValue(MCP4728_CHANNEL_B, valueB);
    mcp.setChannelValue(MCP4728_CHANNEL_C, valueC);
    mcp.setChannelValue(MCP4728_CHANNEL_D, valueD);
    analogWrite(DAC0, dacValue);

    // Delay to simulate real-time output
    delayMicroseconds(9900); // Adjust this delay as needed for real-time playback
  }
  
}
