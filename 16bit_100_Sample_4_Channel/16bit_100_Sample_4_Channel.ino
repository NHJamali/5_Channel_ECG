#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_MCP4728.h>

Adafruit_MCP4728 mcp;

// SD card chip select pin
const int chipSelect = 4;

// Number of leads in the .dat file
const int numLeads = 12;

// Buffers to store raw and adjusted signal data
int16_t rawData[numLeads][100];
float adjustedData[4][100];

// Gain and baseline arrays for 4 leads (V1, V2, Lead I, Lead II)
float gain[4];
int baseline[4];

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
    float currentGain = atof(token);
    token = strtok(NULL, " "); // Skip /mV
    token = strtok(NULL, " "); // Skip format again (e.g., 16)
    token = strtok(NULL, " "); // Skip 0
    int currentBaseline = atoi(token);

    // Store the gain and baseline for the relevant leads
    if (i == 6) { // V1
      gain[0] = currentGain;
      baseline[0] = currentBaseline;
    } else if (i == 7) { // V2
      gain[1] = currentGain;
      baseline[1] = currentBaseline;
    } else if (i == 0) { // Lead I
      gain[2] = currentGain;
      baseline[2] = currentBaseline;
    } else if (i == 1) { // Lead II
      gain[3] = currentGain;
      baseline[3] = currentBaseline;
    }
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

  // Skip to the relevant sample range
  datFile.seek(341 * numLeads * 2);

  // Read the raw data from the .dat file for the specific range
  for (int i = 0; i < 100; i++) {
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
  for (int j = 0; j < 100; j++) {
    adjustedData[0][j] = (rawData[6][j] - baseline[0]) / gain[0]; // V1
    adjustedData[1][j] = (rawData[7][j] - baseline[1]) / gain[1]; // V2
    adjustedData[2][j] = (rawData[0][j] - baseline[2]) / gain[2]; // Lead I
    adjustedData[3][j] = (rawData[1][j] - baseline[3]) / gain[3]; // Lead II
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
  for (int i = 0; i < 100; i++) {
    // Convert the adjusted data to a range suitable for DAC (0-4095 for 12-bit DAC)
    // uint16_t valueA = (uint16_t)((adjustedData[0][i] + 1.65) / 3.3 * 4095); // V1
    // uint16_t valueB = (uint16_t)((adjustedData[1][i] + 1.65) / 3.3 * 4095); // V2
    // uint16_t valueC = (uint16_t)((adjustedData[2][i] + 1.65) / 3.3 * 4095); // Lead I
    // uint16_t valueD = (uint16_t)((adjustedData[3][i] + 1.65) / 3.3 * 4095); // Lead II

    uint16_t valueA = map(adjustedData[0][i], 0, 3.3, 0, 4095); // V1
    uint16_t valueB = map(adjustedData[1][i], 0, 3.3, 0, 4095); // V2
    uint16_t valueC = map(adjustedData[2][i], 0, 3.3, 0, 4095); // Lead I
    uint16_t valueD = map(adjustedData[3][i], 0, 3.3, 0, 4095); // Lead II

    // Set the DAC values
    mcp.setChannelValue(MCP4728_CHANNEL_A, valueA);
    mcp.setChannelValue(MCP4728_CHANNEL_B, valueB);
    mcp.setChannelValue(MCP4728_CHANNEL_C, valueC);
    mcp.setChannelValue(MCP4728_CHANNEL_D, valueD);

    // Delay to simulate real-time output
    delay(10); // Adjust this delay as needed for real-time playback
  }
}
