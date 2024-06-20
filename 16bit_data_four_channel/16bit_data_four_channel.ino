#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_MCP4728.h>

Adafruit_MCP4728 mcp;

// SD card chip select pin
const int chipSelect = 4;

// Number of samples in the .dat file (as per the .hea file)
const int numSamples = 1000;

// Number of leads in the .dat file (as per the .hea file)
const int numLeads = 12;

// Buffers to store raw and adjusted signal data
int16_t rawData[numLeads][numSamples];
float adjustedData[numLeads][numSamples];

// Gain and baseline arrays
float gain[numLeads];
int baseline[numLeads];

// Function to read and parse the .hea file
bool readHeaFile(const char *filename) {
  File heaFile = SD.open(filename);
  if (!heaFile) {
    Serial.println("Error opening .hea file");
    return false;
  }

  // Skip the first line of the .hea file
  heaFile.readStringUntil('\n');

  // Read gain and baseline values for each lead
  for (int i = 0; i < numLeads; i++) {
    String line = heaFile.readStringUntil('\n');
    char buffer[100];
    line.toCharArray(buffer, 100);

    // Tokenize the line to extract gain and baseline
    char *token = strtok(buffer, " ");
    token = strtok(NULL, " "); // Skip the filename
    token = strtok(NULL, " "); // Skip the format (e.g., 16)
    token = strtok(NULL, "("); // Start of gain
    gain[i] = atof(token);
    token = strtok(NULL, " "); // Skip /mV
    token = strtok(NULL, " "); // Skip format again (e.g., 16)
    token = strtok(NULL, " "); // Skip 0
    baseline[i] = atoi(token);
  }

  heaFile.close();
  return true;
}

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

// Function to adjust the raw data using gain and baseline
void adjustData() {
  for (int i = 0; i < numLeads; i++) {
    for (int j = 0; j < numSamples; j++) {
      adjustedData[i][j] = (rawData[i][j] - baseline[i]) /gain[i];
    }
  }
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

  // Read and parse the .hea file
  if (!readHeaFile("01000_lr.hea")) {
    Serial.println("Failed to read .hea file");
    while (1) { delay(10); }
  }

  // Read the .dat file
  if (!readDatFile("01000_lr.dat")) {
    Serial.println("Failed to read .dat file");
    while (1) { delay(10); }
  }

  // Adjust the raw data using gain and baseline
  adjustData();
}

void loop() {
  for (int i = 255; i < 341; i++) {
    // Convert the adjusted data to a range suitable for DAC (0-4095 for 12-bit DAC)
    uint16_t valueA = (uint16_t)((adjustedData[0][i] + 5.0) / 10.0 * 4095);
    uint16_t valueB = (uint16_t)((adjustedData[1][i] + 5.0) / 10.0 * 4095);
    uint16_t valueC = (uint16_t)((adjustedData[6][i] + 5.0) / 10.0 * 4095);
    uint16_t valueD = (uint16_t)((adjustedData[7][i] + 5.0) / 10.0 * 4095);
    uint16_t dacValue = (uint16_t)((adjustedData[2][i] + 5.0) / 10.0 * 4095);

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
