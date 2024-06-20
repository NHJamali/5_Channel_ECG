#include <Wire.h>
#include <Adafruit_MCP4728.h>
#include "V1.h"
#include "V2.h"
#include "Lead_I.h"
#include "Lead_II.h"
#include "Lead_III.h"

Adafruit_MCP4728 mcp;

const int numSamples = 100; // Number of samples in each array

void setup() {
  Serial.begin(9600);

  // Initialize I2C
  Wire.begin();

  // Initialize DAC
  if (!mcp.begin()) {
    Serial.println("Failed to find MCP4728 chip");
    while (1) { delay(10); }
  }
}

void loop() {
  for (int i = 0; i < numSamples; i++) {
    // Convert the adjusted data to a range suitable for DAC (0-4095 for 12-bit DAC, 0-3.3V)
    uint16_t valueA = map((V1[i] * 1000), -3300, 3300, 0, 4095); // V1
    uint16_t valueB = map((V2[i] * 1000), -3300, 3300, 0, 4095); // V2
    uint16_t valueC = map((Lead_I[i] * 1000), -3300, 3300, 0, 4095); // Lead I
    uint16_t valueD = map((Lead_II[i] * 1000), -3300, 3300, 0, 4095); // Lead II

    // Set the DAC values
    mcp.setChannelValue(MCP4728_CHANNEL_A, valueA);
    mcp.setChannelValue(MCP4728_CHANNEL_B, valueB);
    mcp.setChannelValue(MCP4728_CHANNEL_C, valueC);
    mcp.setChannelValue(MCP4728_CHANNEL_D, valueD);

    // Delay to simulate real-time output
    delay(10); // Adjust this delay as needed for real-time playback
  }
}
